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

class PdfMarker;
class PdfMarkerItem;
class DocumentSettings;
class SummarizeDocument;
class QGraphicsScene;

struct rendered_page {
	QImage image;
	document_units::size<document_units::centimeter> size;
	double scale;
};

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class DocumentPage
{
	public:
		DocumentPage(SummarizeDocument *sdoc, int pagenumber);
		~DocumentPage();
		void addMarker(PdfMarker* marker);
		std::shared_ptr<QImage> renderPage(document_units::resolution_setting settings, double scale);
		
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

	public slots:
		void autoMarkCombined(const DocumentSettings& settings, uint threshold, document_units::centimeter HeightThreshold, bool determineVert, bool boundingBox);
		
	protected:
		std::shared_ptr<QImage> rerenderPage(document_units::resolution_setting settings, double scale);

	private:
		std::shared_ptr<QImage> m_renderedPage;
		document_units::size<document_units::centimeter> _pageSize;
		document_units::resolution_setting _resolution;
		std::vector<PdfMarker*> _markers;
		double m_dRenderedScale;
		const int m_iPageNumber;
		QGraphicsScene *m_scene;
		SummarizeDocument* m_sdoc;

		QMutex m_mutex;
		QMutex m_markermutex;
		
};

#endif
