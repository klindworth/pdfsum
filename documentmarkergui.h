/***************************************************************************
 *   Copyright (C) 2009, 2015 by Kai Klindworth                            *
 *                                                                         *
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

#ifndef DOCUMENTMARKERGUI_H
#define DOCUMENTMARKERGUI_H

#include <QWidget>
#include "ui_DocumentMarkerGui.h"

#include <QProcess>
#include <memory>

class PdfMarkerItem;
class LatexRunner;

class DocumentMarkerGui : public QWidget, private Ui::Form
{
	Q_OBJECT

	public:
		DocumentMarkerGui(QWidget* parent = 0);
		~DocumentMarkerGui();

	public slots:
		void setSelection(const QList<PdfMarkerItem*>& markerlist);

	protected slots:
		void removeMarker();
		void saveToPdf();
		void copyToAllOtherPages();
		
		
	protected:
		PdfMarkerItem *m_marker;
		std::unique_ptr<LatexRunner> m_runner;
};

#endif

