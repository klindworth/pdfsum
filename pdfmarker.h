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

#include <QGraphicsRectItem>
#include <QRectF>

#include "document_units.h"

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/

class DocumentSettings;
class DocumentMarkerGui;
class DocumentPage;
class PdfMarkerItem;
class PdfView;

class PdfMarker //: public QGraphicsRectItem
{
	public:
		//PdfMarker(DocumentPage *page, const DocumentSettings* settings, const QRectF& rect);
		PdfMarker(DocumentPage *page, const DocumentSettings* settings, document_units::rect<document_units::centimeter> prect);
		~PdfMarker();
		QString toLatexViewport() const;
		DocumentPage* page() const;
		bool automaticMarker() const;
		void setAutomaticMarker(bool automarker);
		document_units::centimeter height() const;
		PdfMarkerItem* item() const;
		QRectF rect() const;
		document_units::rect<document_units::pixel> pixelRect() const;
		document_units::rect<document_units::centimeter> centimeterRect() const;
		//void setRect(const QRectF& rect);
		inline const DocumentSettings* documentSettings() const {
			return m_settings;
		}
		
	protected:

		
	private:
		document_units::rect<document_units::centimeter> _rect;
		//QRectF m_rect;
		PdfMarkerItem *m_item;
		const DocumentSettings* m_settings;
		DocumentPage *m_page;
		bool m_bAutomaticMarker;

};

#endif
