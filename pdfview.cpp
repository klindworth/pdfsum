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
#include "pdfview.h"

#include <QtDebug>
#include <QRubberBand>
#include <QMouseEvent>
#include "pdfmarkeritem.h"
#include "pdfmarker.h"
#include "documentsettings.h"
#include "documentpage.h"
#include "summarizedocument.h"
#include "documentmarkergui.h"
#include "document_units.h"

PdfView::PdfView(QWidget *parent)
 : QGraphicsView(parent)
{
	m_settings = nullptr;
	m_rubberBand = std::make_unique<QRubberBand>(QRubberBand::Rectangle, this);
	
	m_dScale = 1.0;
	m_page = nullptr;
	m_gui  = nullptr;
	
	setCacheMode(QGraphicsView::CacheBackground);
	
	m_bAddMarkerEnabled = true;
}

void PdfView::setDocumentSettings(DocumentSettings *settings)
{
	m_settings = settings;
	settings->registerView(this);
	connect(m_settings, SIGNAL(marginsChanged()), this, SLOT(refreshView()));
}

/**
* Updates the 'auto width' setting for new markers
* @param activate Enable/Disable the 'auto width'
* @param leftMargin The left margin in cm
* @param rightMargin The right margin in cm
*/
void PdfView::changeAutoWidth(bool activate, document_units::centimeter leftMargin, document_units::centimeter rightMargin)
{
	m_settings->setAutoWidth(activate, leftMargin, rightMargin);
}

/**
* Forces the view to update the view. This doesn't mean, that the internal pixmap of the page is recreated.
*/
void PdfView::refreshView()
{
	resetCachedContent();
	if(scene())
		scene()->update();
}

/**
* Returns the number of the currently shown page
* @return Number of currently shown page
*/
DocumentPage* PdfView::page() const
{
	return m_page;
}

/**
* Changes the zoom-factor.
* @param dZoom zoom-factor. E.g. 2.0 for 200%
*/
void PdfView::setZoom(double dZoom)
{
	if(scene() && m_page)
	{
		double scaleChange = dZoom/m_dScale;
		m_dScale = dZoom;
		//rerenderPage();
		m_renderedPage = m_page->renderPage(m_settings->resolution(), m_dScale);
		m_renderedPixmap = std::make_shared<QPixmap>(QPixmap::fromImage(*m_renderedPage));
		scale(scaleChange, scaleChange);
	}
	
}

/**
* Sets (another) page of the current document. The function handles all necessary steps like rendering
* the new page and updating the background. No further steps necessaray after calling this function
* @param iPage Number of the page, which should be rendered. Important: Counting starts with 0.
*/
void PdfView::setPage(DocumentPage* page)
{
	if(page)
	{
		m_page = page;
		setScene(m_page->graphicsScene());
		connect(m_page->graphicsScene(), &QGraphicsScene::selectionChanged, [&]() {
			emit selectionChanged(selection());
		} );

		//rerenderPage();
		m_renderedPage = m_page->renderPage(m_settings->resolution(), m_dScale);
		if(scene()->sceneRect().size().isEmpty())
			scene()->setSceneRect(0, 0, m_renderedPage->rect().width()/m_dScale, m_renderedPage->rect().height()/m_dScale);

		m_renderedPixmap = std::make_shared<QPixmap>(QPixmap::fromImage(*m_renderedPage));

		scene()->update();
		emit selectionChanged(selection());
	}
	else
		clear();
}

QList<PdfMarkerItem*> PdfView::selection() const
{
	QList<PdfMarkerItem*> selection;
	if(m_page->graphicsScene())
	{
		auto items = m_page->graphicsScene()->selectedItems();
		for(auto current_item : items)
		{
			PdfMarkerItem* converted_item = qgraphicsitem_cast<PdfMarkerItem*>(current_item);
			if(converted_item)
				selection.append(converted_item);
		}
	}
	return selection;
}

void PdfView::clear()
{
	m_page = nullptr;
	m_renderedPage.reset();
	m_renderedPixmap.reset();
}

/**
* This function is used for drawing the document and the 'auto width' margins
*/
void PdfView::drawBackground ( QPainter * painter, const QRectF & rect )
{
	if(m_renderedPixmap)
	{
		QBrush brush(QColor(137, 171, 238, 100), Qt::SolidPattern);
		QPen pen(QColor(137, 171, 238));
		pen.setWidth(2);
		painter->setPen(pen);
		painter->setBrush(brush);
		painter->drawRect(scene()->sceneRect());
		QRectF source(rect.x() * m_dScale, rect.y() * m_dScale, rect.width() * m_dScale, rect.height() * m_dScale);
		painter->drawPixmap(rect, *m_renderedPixmap, source);
		
		if(m_settings->autoWidth())
		{
			QBrush brushMargin(QColor(255, 91, 91, 100), Qt::SolidPattern);
			QPen penMargin(QColor(255, 91, 91));
			pen.setWidth(2);
			painter->setPen(penMargin);
			painter->setBrush(brushMargin);

			document_units::margins<document_units::pixel> margins = m_settings->resolution().to<document_units::pixel>(m_settings->margins());
			
			QRectF leftMargin(0,0, margins.left.value, scene()->sceneRect().height());
			QRectF rightMargin(scene()->sceneRect().width() - margins.right.value, 0, margins.right.value, scene()->sceneRect().height());
			
			painter->drawRect(leftMargin);
			painter->drawRect(rightMargin);
			//painter->drawRect(rect.intersected(leftMargin));
			//painter->drawRect(rect.intersected(rightMargin));
			
			QRectF topMargin(0,0, scene()->sceneRect().width(), margins.top.value);
			QRectF bottomMargin(0, scene()->sceneRect().height() - margins.bottom.value, scene()->sceneRect().width(), margins.bottom.value);
			painter->drawRect(topMargin);
			painter->drawRect(bottomMargin);
		}
	}
}

/**
Clicking on the document creates an rubberband
*/
void PdfView::mousePressEvent(QMouseEvent *event)
{
	if(scene() && m_bAddMarkerEnabled)
	{
		m_dragPosition = event->pos();
		m_rubberBand->setGeometry(QRect(m_dragPosition, QSize()));
		m_rubberBand->show();
	}
}

/**
* This overloaded Qt widget function is used for updating the rubberband on mouse movements
*/
void PdfView::mouseMoveEvent(QMouseEvent *event)
{
	if(scene() && m_bAddMarkerEnabled)
		m_rubberBand->setGeometry(QRect(m_dragPosition, event->pos()).normalized());
} 

/**
* This overloaded Qt widget function is used to capture the mouse events, which are necessary for a marker creation
*/
void PdfView::mouseReleaseEvent(QMouseEvent *event)
{
	if (!m_rubberBand->size().isEmpty() && scene() && m_settings && m_bAddMarkerEnabled)
	{
		QPointF pos = mapToScene(m_rubberBand->pos()); //m_rubberBand->rect's pos is 0,0, so ask for pos seperatly
		QRectF rectf = QRectF(pos, mapToScene(m_rubberBand->rect()).boundingRect().size());
		
		using namespace document_units;

		//document_units::rect<document_units::pixel>
		document_units::rect<pixel> prect(document_units::coordinate<pixel>(pixel(rectf.x()), pixel(rectf.y())), document_units::size<pixel>(pixel(rectf.width()), pixel(rectf.height())));
		document_units::rect<centimeter> crect = m_settings->resolution().to<centimeter>(prect);
		
		//if the 'auto width' setting is active, overwrite the position and width of the marked area
		if(m_settings->autoWidth())
		{
			auto pagearea = m_settings->active_area(m_page->pageSize());
			crect._coordinate.x = pagearea._coordinate.x;
			crect._size.width = pagearea._size.width;
		}
		//create a marker
		m_page->addMarker(new PdfMarker(m_page, crect));
		//hides the temporarily rubberband
		m_rubberBand->hide();
	}
	QGraphicsView::mouseReleaseEvent(event);
}

/**
* Normally it's not possible to add manually markers. This slot is intended to enable/disable it
* @param enable Decides if the manual creation of markers whether enabled or disabled
*/
void PdfView::setAddMarkerEnabled(bool enable)
{
	m_bAddMarkerEnabled = enable;
}

PdfView::~PdfView()
{
}


