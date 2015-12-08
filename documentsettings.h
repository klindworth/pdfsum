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

		document_units::rect<document_units::centimeter> active_area(document_units::size<document_units::centimeter> sz) const;
		void setAutoWidth(bool autowidth, document_units::centimeter leftMargin, document_units::centimeter rightMargin);
		bool autoWidth() const;
		double leftMargin(Unit unit) const;
		double rightMargin(Unit unit) const;
		double topMargin(Unit unit) const;
		double bottomMargin(Unit unit) const;

		document_units::margins<document_units::centimeter> margins() const {
			return _margins;
		}

		void registerView(PdfView *view);
		PdfView* view() const;
		
	public slots:
		void setTopMargin(document_units::centimeter topMargin);
		void setBottomMargin(document_units::centimeter bottomMargin);
		
	private:
		document_units::margins<document_units::centimeter> _margins;
		document_units::resolution_setting _dpi;
		bool m_bAutoWidth;
		PdfView *m_view;

	signals:
		void marginsChanged();	
		
};

#endif
