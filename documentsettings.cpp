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
#include "documentsettings.h"

#include "pdfview.h"

const double inch_factor = 2.54;

DocumentSettings::DocumentSettings(document_units::dpi dpiX, document_units::dpi dpiY, QObject *parent)
 : QObject(parent),
   _margins(document_units::centimeter(0.0), document_units::centimeter(0.0), document_units::centimeter(0.0), document_units::centimeter(0.0)),
   _dpi(dpiX, dpiY)
{
	m_bAutoWidth = false;
	m_view = nullptr;
}

PdfView* DocumentSettings::view() const
{
	return m_view;
}

void DocumentSettings::registerView(PdfView *view)
{
	m_view = view;
}

document_units::rect<document_units::centimeter> DocumentSettings::active_area(document_units::size<document_units::centimeter> sz) const {
	document_units::size<document_units::centimeter> active_size(sz.width - _margins.left - _margins.right, sz.height - _margins.top - _margins.bottom);
	document_units::coordinate<document_units::centimeter> active_coord(_margins.left, _margins.top);
	return document_units::rect<document_units::centimeter>(active_coord, active_size);
}

void DocumentSettings::setAutoWidth(bool autowidth, document_units::centimeter leftMargin, document_units::centimeter rightMargin)
{
	_margins.left = leftMargin;
	_margins.right = rightMargin;
	m_bAutoWidth = autowidth;
	emit marginsChanged();
}

void DocumentSettings::setTopMargin(document_units::centimeter topMargin)
{
	_margins.top = topMargin;
	emit marginsChanged();
}

void DocumentSettings::setBottomMargin(document_units::centimeter bottomMargin)
{
	_margins.bottom = bottomMargin;
	emit marginsChanged();
}

bool DocumentSettings::autoWidth() const
{
	return m_bAutoWidth;
}

double DocumentSettings::leftMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return _margins.left.value;
	else
		return _dpi.x_to<document_units::pixel>(_margins.left).value;
}

double DocumentSettings::rightMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return _margins.right.value;
	else
		return _dpi.x_to<document_units::pixel>(_margins.right).value;
}

double DocumentSettings::topMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return _margins.top.value;
	else
		return _dpi.y_to<document_units::pixel>(_margins.top).value;
}

double DocumentSettings::bottomMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return _margins.bottom.value;
	else
		return _dpi.y_to<document_units::pixel>(_margins.bottom).value;
}


DocumentSettings::~DocumentSettings()
{
}


