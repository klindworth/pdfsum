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

DocumentSettings::DocumentSettings(int dpiX, int dpiY, QObject *parent)
 : QObject(parent)
{
	m_dLeftMargin = 0.0;
	m_dRightMargin = 0.0;
	m_dBottomMargin = 0.0;
	m_dTopMargin = 0.0;
	m_dpiX = dpiX;
	m_dpiY = dpiY;
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

void DocumentSettings::setAutoWidth(bool autowidth, double leftMargin, double rightMargin)
{
	m_dLeftMargin = leftMargin;
	m_dRightMargin = rightMargin;
	m_bAutoWidth = autowidth;
	emit marginsChanged();
}

void DocumentSettings::setTopMargin(double topMargin)
{
	m_dTopMargin = topMargin;
	emit marginsChanged();
}

void DocumentSettings::setBottomMargin(double bottomMargin)
{
	m_dBottomMargin = bottomMargin;
	emit marginsChanged();
}

bool DocumentSettings::autoWidth() const
{
	return m_bAutoWidth;
}

double DocumentSettings::leftMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return m_dLeftMargin;
	else
		return cmInPixelX(m_dLeftMargin);
}

double DocumentSettings::rightMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return m_dRightMargin;
	else
		return cmInPixelX(m_dRightMargin);
}

double DocumentSettings::topMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return m_dTopMargin;
	else
		return cmInPixelY(m_dTopMargin);
}

double DocumentSettings::bottomMargin(Unit unit) const
{
	if(unit == Unit::cm)
		return m_dBottomMargin;
	else
		return cmInPixelY(m_dBottomMargin);
}

double DocumentSettings::pixelInCmX(int pixel) const
{
	return pixel/(double)m_dpiX * 2.54;
}

double DocumentSettings::pixelInCmY(int pixel) const
{
	return pixel/(double)m_dpiY * 2.54;
}

int DocumentSettings::cmInPixelX(double cm) const
{
	return cm*(double)m_dpiX / 2.54;
}

int DocumentSettings::cmInPixelY(double cm) const
{
	return cm*(double)m_dpiY / 2.54;
}

int DocumentSettings::dpiX() const
{
	return m_dpiX;
}

int DocumentSettings::dpiY() const
{
	return m_dpiY;
}


DocumentSettings::~DocumentSettings()
{
}


