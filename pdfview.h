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
#ifndef PDFVIEW_H
#define PDFVIEW_H

#include <QGraphicsView>
#include <QPoint>
#include <QImage>
#include <QList>

#include <memory>

class QRubberBand;
class PdfMarker;
class PdfMarkerItem;
class DocumentSettings;
class DocumentPage;
class SummarizeDocument;
class DocumentMarkerGui;
class rendered_page;

namespace document_units { struct centimeter; }

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class PdfView : public QGraphicsView
{
	Q_OBJECT
	public:
		PdfView(QWidget *parent);
		~PdfView();
		void setDocumentSettings(std::shared_ptr<DocumentSettings> settings);
		DocumentPage* page() const;
		QList<PdfMarkerItem*> selection() const;
		
	public slots:
		void setPage(DocumentPage *page);
		void setZoom(double dZoom);
		void setAddMarkerEnabled(bool enable);
		void refreshView();
		void clear();
	
	protected:
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void drawBackground ( QPainter * painter, const QRectF & rect );
		
		//stores one of the points of the rubberband
		QPoint m_dragPosition;
		//for creating new markers a rubberband is shown
		std::unique_ptr<QRubberBand> m_rubberBand;
		//Stores a pointer to the rendered page.
		std::shared_ptr<rendered_page> _renderedPage;
		//Stores a pointer to the pixmap of the rendered page.
		std::shared_ptr<QPixmap> m_renderedPixmap;
		//current zoomfactor
		double m_dScale;
		//?
		std::shared_ptr<DocumentSettings> _settings;
		//manual creation of markers enabled
		bool m_bAddMarkerEnabled;
		//current page (owned by the document)
		DocumentPage *m_page;
		//Widget for options like deleting and saving as pdf
		DocumentMarkerGui *m_gui;

	signals:
		void selectionChanged(QList<PdfMarkerItem*>);
};

#endif
