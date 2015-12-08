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

DocumentPage::DocumentPage(SummarizeDocument *sdoc, int pagenumber) :
	_pageSize(document_units::centimeter(0), document_units::centimeter(0)),
	_resolution(document_units::dpi(72), document_units::dpi(72)),
	m_iPageNumber(pagenumber),
	m_sdoc(sdoc)
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

document_units::size<document_units::centimeter> DocumentPage::pageSize() const
{
	if(m_renderedPage)
	{
		return _pageSize;
	}
	else
		throw std::runtime_error("no rendered page");
}

void DocumentPage::addMarker(PdfMarker* marker)
{
	QMutexLocker locker(&m_markermutex);

	if(graphicsScene())
		m_scene->addItem(marker->createViewItem(_resolution));
	else
		qWarning("no scene");

	//insert the marker at the right position in the list. the list is ordered by position y
	auto it = std::lower_bound(_markers.begin(), _markers.end(), marker, [](const PdfMarker* lhs, const PdfMarker* rhs){return lhs->rect()._coordinate.y < rhs->rect()._coordinate.y;});
	_markers.insert(it, marker);
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

std::shared_ptr<QImage> DocumentPage::renderPage(document_units::resolution_setting settings, double scale)
{
	//QMutexLocker locker(&m_mutex);
	if(m_renderedPage && m_dRenderedScale == scale)
		return m_renderedPage;
	else
		return rerenderPage(settings, scale);
}

std::shared_ptr<QImage> DocumentPage::rerenderPage(document_units::resolution_setting settings, double scale)
{
	m_renderedPage = document()->renderedPage(m_iPageNumber, scale, settings);
	m_dRenderedScale = scale;

	using namespace document_units;

	pixel width(m_renderedPage->width() / m_dRenderedScale);
	pixel height(m_renderedPage->height() / m_dRenderedScale);
	size<pixel> pixelsize(width, height);

	_pageSize = settings.to<centimeter>(pixelsize);
	_resolution = settings;

	return m_renderedPage;
}

void DocumentPage::removeMarker(PdfMarkerItem *marker)
{
	QMutexLocker locker(&m_markermutex);
	graphicsScene()->removeItem(marker);

	_markers.erase(std::remove(_markers.begin(), _markers.end(), marker->marker()), _markers.end());
}

void DocumentPage::removeMarker(PdfMarker* marker)
{
	QMutexLocker locker(&m_markermutex);

	_markers.erase(std::remove(_markers.begin(), _markers.end(), marker), _markers.end());
}

void DocumentPage::removeCorrespondingViewMarker(PdfMarker* marker)
{
	for(QGraphicsItem *current_item : graphicsScene()->items())
	{
		PdfMarkerItem* converted_item = qgraphicsitem_cast<PdfMarkerItem*>(current_item);
		if(converted_item && marker == converted_item->marker())
		{
			graphicsScene()->removeItem(current_item);
		}
	}
}

void DocumentPage::removeAllMarkers(bool onlyAutomatic)
{
	QMutexLocker locker(&m_markermutex);
	auto eval = [=](const PdfMarker* marker) {
		return marker->automaticMarker() || !onlyAutomatic;
	};

	std::for_each(_markers.begin(), _markers.end(), [=](PdfMarker *& marker){
		if(eval(marker))
			delete marker;
		marker = nullptr;
	});

	auto it = std::remove(_markers.begin(), _markers.end(), nullptr);

	_markers.erase(it, _markers.end());
}

void DocumentPage::autoMarkCombined(const DocumentSettings& settings, uint threshold, document_units::centimeter heightThreshold, bool determineVert, bool boundingBox)
{
	if(!m_renderedPage)
		renderPage(settings.resolution(), 1.0);
	
	if(m_renderedPage)
	{
		std::vector<document_units::rect<document_units::centimeter>> res = autoMarkCombinedInternal(*m_renderedPage, settings, threshold, heightThreshold, !determineVert, boundingBox, m_dRenderedScale, this);
		for(auto rt : res)
		{
			PdfMarker *marker = new PdfMarker(this, rt);
			marker->setAutomaticMarker(true);
			addMarker(marker);
		}
	}
}

document_units::centimeter DocumentPage::markedHeight() const
{
	return std::accumulate(_markers.begin(), _markers.end(), document_units::centimeter(0.0), [](document_units::centimeter sum, const PdfMarker* marker) {
		return sum + marker->height();
	});

}

const std::vector<PdfMarker*>& DocumentPage::markers() const
{
	return _markers;
}

DocumentPage::~DocumentPage()
{
	if(m_scene)
		delete m_scene;
}

