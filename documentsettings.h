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
#ifndef DOCUMENTSETTINGS_H
#define DOCUMENTSETTINGS_H

#include <QObject>
#include "document_units.h"

class PdfView;
enum class Unit {cm, pixel};
/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class DocumentSettings : public QObject
{
	Q_OBJECT
	public:		
		DocumentSettings(document_units::dpi dpiX, document_units::dpi dpiY, QObject *parent = 0);
		~DocumentSettings();

		inline const document_units::resolution_setting& resolution() const {
			return _dpi;
		}

		inline document_units::dpi dpiX() const {
			return _dpi.x;
		}

		inline document_units::dpi dpiY() const {
			return _dpi.y;
		}

		inline document_units::rect<document_units::centimeter> active_area(document_units::size<document_units::centimeter> sz) const {
			document_units::size<document_units::centimeter> active_size(sz.width - _margins.left - _margins.right, sz.height - _margins.top - _margins.bottom);
			document_units::coordinate<document_units::centimeter> active_coord(_margins.left, _margins.top);
			return document_units::rect<document_units::centimeter>(active_coord, active_size);
		}

		void setAutoWidth(bool autowidth, document_units::centimeter leftMargin, document_units::centimeter rightMargin);
		bool autoWidth() const;
		double leftMargin(Unit unit) const;
		double rightMargin(Unit unit) const;
		double topMargin(Unit unit) const;
		double bottomMargin(Unit unit) const;

		document_units::margins margins() const {
			return _margins;
		}

		void registerView(PdfView *view);
		PdfView* view() const;
		
	public slots:
		void setTopMargin(document_units::centimeter topMargin);
		void setBottomMargin(document_units::centimeter bottomMargin);
		
	private:
		document_units::margins _margins;
		document_units::resolution_setting _dpi;
		bool m_bAutoWidth;
		PdfView *m_view;

	signals:
		void marginsChanged();	
		
};

#endif
