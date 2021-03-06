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
#include <QApplication>

#include <algorithm>

#include "automarker.h"

#include <poppler-qt5.h>

DocumentPage::DocumentPage(SummarizeDocument *sdoc, int pagenumber) :
	_pageNumber(pagenumber),
	m_sdoc(sdoc)
{
}

SummarizeDocument* DocumentPage::document()
{
	return m_sdoc;
}

int DocumentPage::number() const
{
	return _pageNumber;
}

document_units::size<document_units::centimeter> DocumentPage::pageSize() const
{
	if(_renderedPage)
		return _renderedPage->size;
	else
		throw std::runtime_error("no rendered page");
}

void DocumentPage::addMarker(PdfMarker* marker, bool no_item_creation)
{
	QMutexLocker locker(&m_markermutex);

	if(_renderedPage && !no_item_creation)
		_renderedPage->scene->addItem(marker->createViewItem(_renderedPage->resolution));

	//insert the marker at the right position in the list. the list is ordered by position y
	auto it = std::lower_bound(_markers.begin(), _markers.end(), marker, [](const PdfMarker* lhs, const PdfMarker* rhs){return lhs->rect()._coordinate.y < rhs->rect()._coordinate.y;});
	_markers.insert(it, marker);
}

std::vector<PdfMarkerItem*> convert_to_item_vector(QList<QGraphicsItem*> items)
{
	std::vector<PdfMarkerItem*> result;
	result.reserve(items.size());
	for(QGraphicsItem *current_item : items)
	{
		PdfMarkerItem* converted_item = qgraphicsitem_cast<PdfMarkerItem*>(current_item);
		if(converted_item)
			result.push_back(converted_item);
	}
	return result;
}

std::vector<PdfMarker*> convert_to_marker_vector(std::vector<PdfMarkerItem*> items)
{
	std::vector<PdfMarker*> result;
	std::transform(items.begin(), items.end(), std::back_inserter(result), [](PdfMarkerItem *item) {
		return item->marker();
	});
	return result;
}

std::vector<PdfMarker*> markers_without_item(std::vector<PdfMarker*> src, QGraphicsScene* scene)
{
	std::sort(src.begin(), src.end());

	std::vector<PdfMarker*> item_markers = convert_to_marker_vector(convert_to_item_vector(scene->items()));
	std::sort(item_markers.begin(), item_markers.end());

	std::vector<PdfMarker*> result;
	std::set_difference(src.begin(), src.end(), item_markers.begin(), item_markers.end(), std::back_inserter(result));
	return result;
}

void DocumentPage::create_items()
{
	QMutexLocker locker(&m_markermutex);

	std::vector<PdfMarker*> markers = markers_without_item(_markers, graphicsScene());
	for(PdfMarker* marker : markers)
		graphicsScene()->addItem(marker->createViewItem(_renderedPage->resolution));

	graphicsScene()->update();
}

QGraphicsScene* DocumentPage::graphicsScene()
{
	assert(_renderedPage);

	return _renderedPage->scene;
}

rendered_page::rendered_page(QImage pimage, document_units::resolution_setting presolution, double pscale, DocumentPage* ppage) :
	image(pimage),
	size(presolution.to<document_units::centimeter>(document_units::size<document_units::pixel>(document_units::pixel(pimage.width() / pscale), document_units::pixel(pimage.height() / pscale)))),
	resolution(presolution), scene(new QGraphicsScene()), scale(pscale), page(ppage)
{
	scene->setSceneRect(0, 0, image.rect().width()/scale, image.rect().height()/scale);
	scene->moveToThread(QApplication::instance()->thread());
}

rendered_page::~rendered_page()
{
	delete scene;
}

std::shared_ptr<rendered_page> DocumentPage::rerender_page(document_units::resolution_setting settings, double scale)
{
	QMutexLocker locker(&m_markermutex);
	Poppler::Page *page = m_sdoc->pdfDocument()->page(_pageNumber);
	std::shared_ptr<rendered_page> result = std::make_shared<rendered_page>(page->renderToImage(scale * settings.x.value, scale * settings.y.value), settings, scale, this);
	delete page;

	for(PdfMarker *marker : _markers)
	{
		result->scene->addItem(marker->createViewItem(settings));
	}

	return result;
}

std::shared_ptr<rendered_page> DocumentPage::render_page(document_units::resolution_setting settings, double scale)
{
	if(_renderedPage && _renderedPage->scale == scale)
		return _renderedPage;
	else
	{
		_renderedPage = rerender_page(settings, scale);
		return _renderedPage;
	}
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

PdfMarkerItem* find_corresponding_viewitem(PdfMarker* marker, QGraphicsScene* scene)
{
	for(QGraphicsItem *current_item : scene->items())
	{
		PdfMarkerItem* converted_item = qgraphicsitem_cast<PdfMarkerItem*>(current_item);
		if(converted_item && marker == converted_item->marker())
			return converted_item;
	}

	return nullptr;
}

void DocumentPage::removeCorrespondingViewMarker(PdfMarker* marker)
{
	if(_renderedPage)
	{
		PdfMarkerItem *item = find_corresponding_viewitem(marker, graphicsScene());
		if(item)
			graphicsScene()->removeItem(item);
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

void DocumentPage::automark_combined(automark_settings settings)
{
	if(!_renderedPage)
		_renderedPage = render_page(settings.resolution, 1.0);


	if(_renderedPage)
	{
		std::vector<document_units::rect<document_units::centimeter>> res = autoMarkCombinedInternal(_renderedPage->image, settings, _renderedPage->scale, pageSize());
		for(auto rt : res)
		{
			PdfMarker *marker = new PdfMarker(this, rt);
			marker->setAutomaticMarker(true);
			addMarker(marker, true);
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
}

