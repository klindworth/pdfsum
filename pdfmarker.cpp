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
#include "pdfmarker.h"

#include <QGraphicsScene>
#include <QStringBuilder>


#include "documentsettings.h"
#include "documentmarkergui.h"
#include "documentpage.h"
#include "pdfmarkeritem.h"
#include "pdfview.h"

PdfMarker::PdfMarker(DocumentPage *page, const DocumentSettings* settings, const QRectF& rect) : m_rect(rect), m_settings(settings)
{
	m_item = nullptr;
	m_page = page;
	
	m_bAutomaticMarker = false;
	
	if(documentSettings()->view())
		m_item = new PdfMarkerItem(this, rect);
}

void PdfMarker::setRect(const QRectF& rect)
{
	m_rect = rect;
	if(m_item)
		m_item->setRect(rect);
}

QRectF PdfMarker::rect() const
{
	return m_rect;
}

PdfMarkerItem* PdfMarker::item() const
{
	return m_item;
}

void PdfMarker::setAutomaticMarker(bool automarker)
{
	m_bAutomaticMarker = automarker;
}

bool PdfMarker::automaticMarker() const
{
	return m_bAutomaticMarker;
}

DocumentPage* PdfMarker::page() const
{
	return m_page;
}

double PdfMarker::height() const
{
	return documentSettings()->pixelInCmY(rect().height());
}

QString PdfMarker::toLatexViewport() const
{
	//ATTENTION: rect.pos != item.pos !!!!
	//1 = point bottom left, 2 = top right
	float x1 = documentSettings()->pixelInCmX(rect().x());
	float y1 = page()->pageSizeInCm(documentSettings()).height() - documentSettings()->pixelInCmY(rect().y() + rect().height());
	float x2 = documentSettings()->pixelInCmX(rect().width() + rect().x());
	float y2 = page()->pageSizeInCm(documentSettings()).height() - documentSettings()->pixelInCmY(rect().y());

	QString text = "viewport= " % QString::number(x1) + "cm " % QString::number(y1) % "cm " %
	QString::number(x2) % "cm " % QString::number(y2) % "cm";
		
	return text;
}

PdfMarker::~PdfMarker()
{
	//will be deleted with scene?
	if(m_item)
		delete m_item;
}


