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

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class DocumentSettings : public QObject
{
	Q_OBJECT
	public:
		DocumentSettings(document_units::dpi dpiX, document_units::dpi dpiY);

		inline const document_units::resolution_setting& resolution() const {
			return _dpi;
		}

		bool autoWidth() const {
			return _autoWidth;
		}

		document_units::margins<document_units::centimeter> margins() const {
			return _margins;
		}
		
	public slots:
		void set_margins(bool autowidth, document_units::margins<document_units::centimeter> margins);

	private:
		document_units::margins<document_units::centimeter> _margins;
		document_units::resolution_setting _dpi;
		bool _autoWidth;

	signals:
		void marginsChanged();
		
};

#endif
