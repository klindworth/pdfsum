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
#include <QLinkedList>
#include <QMutex>
#include <memory>
#include <deque>
#include <cassert>

class PdfMarker;
class DocumentSettings;
class SummarizeDocument;
class QGraphicsScene;

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class DocumentPage
{
	public:
		DocumentPage(SummarizeDocument *sdoc, int pagenumber);
		~DocumentPage();
		void addMarker(PdfMarker *marker);
		std::shared_ptr<QImage> renderPage(const DocumentSettings *settings, double scale);
		
		void setGraphicsScene(QGraphicsScene *scene);
		QGraphicsScene* graphicsScene();
		void removeMarker(PdfMarker *marker);
		int number() const;
		void removeAllMarkers(bool onlyAutomatic);
		double markedHeight() const;
		SummarizeDocument* document();
		QSizeF pageSizeInCm(const DocumentSettings *settings) const;
		const std::deque<PdfMarker*>& markers() const;

	public slots:
		void autoMarkCombined(const DocumentSettings *settings, uint threshold, double dHeightThreshold, bool determineVert, bool boundingBox);
		
	protected:
		std::shared_ptr<QImage> rerenderPage(const DocumentSettings *settings, double scale);

	private:
		std::shared_ptr<QImage> m_renderedPage;
		std::deque<PdfMarker*> m_markers;
		double m_dRenderedScale;
		const int m_iPageNumber;
		QGraphicsScene *m_scene;
		SummarizeDocument* m_sdoc;

		QMutex m_mutex;
		QMutex m_markermutex;
		
};

#endif
