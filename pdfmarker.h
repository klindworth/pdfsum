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
#ifndef PDFMARKER_H
#define PDFMARKER_H

#include "document_units.h"
#include <vector>

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/

class DocumentPage;
class PdfMarkerItem;
class QString;

class PdfMarker
{
	public:
		PdfMarker(DocumentPage *page, document_units::rect<document_units::centimeter> prect);
		~PdfMarker();
		PdfMarkerItem* createViewItem(document_units::resolution_setting settings);
		QString toLatexViewport() const;
		DocumentPage* page() const {
			return _page;
		}
		bool automaticMarker() const {
			return _bAutomaticMarker;
		}
		void setAutomaticMarker(bool automarker);
		document_units::centimeter height() const;
		inline document_units::rect<document_units::centimeter> rect() const {
			return _rect;
		}

		bool operator==(const PdfMarker& marker) const {
			return marker._rect == _rect;
		}
		
	protected:

		
	private:
		document_units::rect<document_units::centimeter> _rect;
		DocumentPage *_page;
		bool _bAutomaticMarker;
};

#endif
