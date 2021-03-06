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
#ifndef DOCUMENTPAGE_H
#define DOCUMENTPAGE_H

#include <QImage>
#include <QMutex>
#include <memory>
#include <deque>
#include <cassert>

#include "document_units.h"
#include "automarker.h"

class PdfMarker;
class PdfMarkerItem;
class DocumentSettings;
class SummarizeDocument;
class QGraphicsScene;

class DocumentPage;

struct rendered_page
{
	rendered_page(QImage pimage, document_units::resolution_setting presolution, double pscale, DocumentPage* ppage);
	~rendered_page();
	rendered_page(const rendered_page&) = delete;
	const QImage image;
	const document_units::size<document_units::centimeter> size;
	const document_units::resolution_setting resolution;
	QGraphicsScene *scene;
	const double scale;

	DocumentPage* page;
};

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class DocumentPage
{
	public:
		DocumentPage(SummarizeDocument *sdoc, int pagenumber);
		~DocumentPage();
		void addMarker(PdfMarker* marker, bool no_item_creation = false);
		std::shared_ptr<rendered_page> render_page(document_units::resolution_setting settings, double scale);
		std::shared_ptr<rendered_page> rerender_page(document_units::resolution_setting settings, double scale);
		
		void setGraphicsScene(QGraphicsScene *scene);
		QGraphicsScene* graphicsScene();
		void removeMarker(PdfMarker *marker);
		void removeMarker(PdfMarkerItem *marker);
		void removeCorrespondingViewMarker(PdfMarker *marker);
		int number() const;
		void removeAllMarkers(bool onlyAutomatic);
		document_units::centimeter markedHeight() const;
		SummarizeDocument* document();
		document_units::size<document_units::centimeter> pageSize() const;
		const std::vector<PdfMarker*>& markers() const;
		void create_items();

	public slots:
		void automark_combined(automark_settings settings);

	private:
		std::shared_ptr<rendered_page> _renderedPage;
		std::vector<PdfMarker*> _markers;
		const int _pageNumber;
		SummarizeDocument* m_sdoc;

		QMutex m_mutex;
		QMutex m_markermutex;
		
};

#endif
