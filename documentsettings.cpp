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

DocumentSettings::DocumentSettings(document_units::dpi dpiX, document_units::dpi dpiY)
 : QObject(),
   _margins(document_units::centimeter(0.0), document_units::centimeter(0.0), document_units::centimeter(0.0), document_units::centimeter(0.0)),
   _dpi(dpiX, dpiY)
{
	_autoWidth = false;
}

document_units::rect<document_units::centimeter> DocumentSettings::active_area(document_units::size<document_units::centimeter> sz) const
{
	document_units::size<document_units::centimeter> active_size(sz.width - _margins.left - _margins.right, sz.height - _margins.top - _margins.bottom);
	document_units::coordinate<document_units::centimeter> active_coord(_margins.left, _margins.top);
	return document_units::rect<document_units::centimeter>(active_coord, active_size);
}

void DocumentSettings::set_margins(bool autowidth, document_units::margins<document_units::centimeter> margins)
{
	_margins = margins;
	_autoWidth = autowidth;
	emit marginsChanged();
}

