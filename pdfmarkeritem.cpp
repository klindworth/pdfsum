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
#include "pdfmarkeritem.h"

#include <QBrush>
#include <QPen>
#include <QKeyEvent>

#include "pdfmarker.h"
#include "documentpage.h"
#include "documentsettings.h"
#include "pdfview.h"
#include "documentmarkergui.h"

PdfMarkerItem::PdfMarkerItem(PdfMarker *marker, const QRectF& rect) : QGraphicsRectItem(rect)
{
	m_marker = marker;
		
	QBrush brush(QColor(137, 171, 238, 100), Qt::SolidPattern);
	QPen pen(QColor(137, 171, 238));
	pen.setWidth(2);
	this->setPen(pen);
	this->setBrush(brush);
	
	this->setFlag(QGraphicsItem::ItemIsSelectable);
	this->setFlag(QGraphicsItem::ItemIsFocusable);
}

PdfMarker* PdfMarkerItem::marker()
{
	return m_marker;
}

void PdfMarkerItem::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
	QGraphicsItem::mouseDoubleClickEvent(event);

	
	if(marker()->documentSettings()->view()->markerGui())
		marker()->documentSettings()->view()->markerGui()->setDocumentMarker(marker());
}

void PdfMarkerItem::keyReleaseEvent ( QKeyEvent * event ) 
{
	if(event->key() == Qt::Key_Delete)
	{
		marker()->documentSettings()->view()->markerGui()->setDocumentMarker(NULL);
		marker()->page()->removeMarker(marker());
	}
	else
		QGraphicsItem::keyReleaseEvent(event);
}


PdfMarkerItem::~PdfMarkerItem()
{
	if(scene())
		scene()->removeItem(this);
}


