/***************************************************************************
 *   Copyright (C) 2009, 2015 by Kai Klindworth                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "documentpage.h"

#include "pdfmarker.h"
#include "pdfmarkeritem.h"
#include "documentsettings.h"
#include "summarizedocument.h"

#include <QGraphicsScene>
#include <QMutexLocker>
#include <QtAlgorithms>

#include <algorithm>

#include "automarker.h"

DocumentPage::DocumentPage(SummarizeDocument *sdoc, int pagenumber) : m_iPageNumber(pagenumber), m_sdoc(sdoc)
{
	m_scene = nullptr;
	m_dRenderedScale = 1.0;
}

SummarizeDocument* DocumentPage::document()
{
	return m_sdoc;
}

int DocumentPage::number() const
{
	return m_iPageNumber;
}

document_units::size<document_units::centimeter> DocumentPage::pageSize(const document_units::resolution_setting& settings) const
{
	if(m_renderedPage)
	{
		using namespace document_units;

		pixel width(m_renderedPage->width() / m_dRenderedScale);
		pixel height(m_renderedPage->height() / m_dRenderedScale);
		size<pixel> pixelsize(width, height);

		return settings.to<centimeter>(pixelsize);
	}
	else
		throw std::runtime_error("no rendered page");
}

void DocumentPage::addMarker(PdfMarker *marker)
{
	QMutexLocker locker(&m_markermutex);

	if(graphicsScene() && marker->item())
		m_scene->addItem(marker->item());
	else
		qWarning("no scene");
	
	//insert the marker at the right position in the list. the list is ordered by position y
	auto it = std::lower_bound(m_markers.begin(), m_markers.end(), marker, [](PdfMarker* lhs, PdfMarker *rhs){return lhs->rect().y() < rhs->rect().y();});
	m_markers.insert(it, marker);
}

void DocumentPage::setGraphicsScene(QGraphicsScene *scene)
{
	QMutexLocker locker(&m_mutex);
	m_scene = scene;
}

QGraphicsScene* DocumentPage::graphicsScene()
{
	if(!m_scene)
	{
		m_scene = new QGraphicsScene();
		if(m_renderedPage)
			m_scene->setSceneRect(0, 0, m_renderedPage->rect().width()/m_dRenderedScale, m_renderedPage->rect().height()/m_dRenderedScale);
	}
	return m_scene;
}

std::shared_ptr<QImage> DocumentPage::renderPage(const DocumentSettings *settings, double scale)
{
	//QMutexLocker locker(&m_mutex);
	if(m_renderedPage && m_dRenderedScale == scale)
		return m_renderedPage;
	else
		return rerenderPage(settings, scale);
}

std::shared_ptr<QImage> DocumentPage::rerenderPage(const DocumentSettings *settings, double scale)
{
	m_renderedPage = document()->renderedPage(m_iPageNumber, scale, settings);
	m_dRenderedScale = scale;

	return m_renderedPage;
}

void DocumentPage::removeMarker(PdfMarker *marker)
{
	QMutexLocker locker(&m_markermutex);
	m_markers.erase(std::remove(m_markers.begin(), m_markers.end(), marker), m_markers.end());
	
	delete marker;
}

void DocumentPage::removeAllMarkers(bool onlyAutomatic)
{
	QMutexLocker locker(&m_markermutex);
	auto eval = [=](PdfMarker* marker) {
		return marker->automaticMarker() || !onlyAutomatic;
	};

	std::for_each(m_markers.begin(), m_markers.end(), [=](PdfMarker *marker){
		if(eval(marker))
			delete marker;
	});

	auto it = std::remove_if(m_markers.begin(), m_markers.end(), eval);

	m_markers.erase(it, m_markers.end());
}

void DocumentPage::autoMarkCombined(const DocumentSettings *settings, uint threshold, document_units::centimeter heightThreshold, bool determineVert, bool boundingBox)
{
	if(!m_renderedPage)
		renderPage(settings, 1.0);
	
	if(m_renderedPage)
	{
		std::vector<QRectF> res = autoMarkCombinedInternal(*m_renderedPage, *settings, threshold, heightThreshold, !determineVert, boundingBox, m_dRenderedScale);
		for(QRectF rt : res)
		{
			PdfMarker *marker = new PdfMarker(this, settings, rt);
			marker->setAutomaticMarker(true);
			addMarker(marker);
		}
	}
}

double DocumentPage::markedHeight() const
{
	double result = 0.0;
	for(PdfMarker * marker : m_markers)
		result += marker->height().value;
	
	return result;
}

const std::deque<PdfMarker *>& DocumentPage::markers() const
{
	return m_markers;
}

DocumentPage::~DocumentPage()
{
	for(PdfMarker * marker : m_markers)
		delete marker;

	if(m_scene)
		delete m_scene;
}

