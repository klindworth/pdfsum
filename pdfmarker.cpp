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

#include <QStringBuilder>

#include "documentpage.h"
#include "pdfmarkeritem.h"

PdfMarker::PdfMarker(DocumentPage *page, document_units::rect<document_units::centimeter> prect) : _rect(prect), _page(page), _bAutomaticMarker(false)
{
}

PdfMarker::~PdfMarker()
{
	_page->removeCorrespondingViewMarker(this);
}

QRectF toRectF(const document_units::rect<document_units::pixel>& rt)
{
	return QRectF(rt._coordinate.x.raw_value(), rt._coordinate.y.raw_value(), rt._size.width.raw_value(), rt._size.height.raw_value());
}

PdfMarkerItem* PdfMarker::createViewItem(document_units::resolution_setting resolution)
{
	auto pixelrect = resolution.to<document_units::pixel>(_rect);

	QRectF frect = toRectF(pixelrect);
	return new PdfMarkerItem(this, frect);
}

void PdfMarker::setAutomaticMarker(bool automarker)
{
	_bAutomaticMarker = automarker;
}

document_units::centimeter PdfMarker::height() const
{
	return rect()._size.height;
}

QString PdfMarker::toLatexViewport() const
{
	document_units::rect<document_units::centimeter> cmrect = rect();
	document_units::centimeter pgheight = page()->pageSize().height;

	document_units::coordinate<document_units::centimeter> bottom_left(cmrect.start_point().x, pgheight - cmrect.end_point().y);
	document_units::coordinate<document_units::centimeter> top_right  (cmrect.end_point().x,   pgheight - cmrect.start_point().y);

	QString text = QString("viewport= %1cm %2cm %3cm %4cm").arg(bottom_left.x.value).arg(bottom_left.y.value).arg(top_right.x.value).arg(top_right.y.value);

	return text;
}


