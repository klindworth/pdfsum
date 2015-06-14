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


#include "documentmarkergui.h"

#include "documentpage.h"
#include "pdfmarker.h"
#include "latexrunner.h"
#include "summarizedocument.h"

#include <QFileDialog>

DocumentMarkerGui::DocumentMarkerGui(QWidget* parent)
: QWidget( parent ), Ui::Form()
{
	setupUi(this);
	
	connect(btnDeleteMarker, SIGNAL(clicked()), this, SLOT(removeMarker()));
	connect(btnSavePdf, SIGNAL(clicked()), this, SLOT(saveToPdf()));
}

void DocumentMarkerGui::setDocumentMarker(PdfMarker *marker)
{
	m_marker = marker;
	this->setEnabled(marker != nullptr);
}

void DocumentMarkerGui::saveToPdf()
{
	QString saveas = QFileDialog::getSaveFileName(this, tr("Choose place for PDF file"), QString(), "*.pdf");
	if(!saveas.isEmpty())
	{
		saveas += ".pdf";
		QFileInfo fileinfo(m_marker->page()->document()->filepath());
		
		QString input = "\\documentclass[a4paper,10pt]{article}\n\\usepackage{graphicx}\n\\usepackage{pdfpages}\n\\begin{document}\n\\includepdf[";
		input += m_marker->toLatexViewport();
		input += ", clip=true, pages={" + QString::number(m_marker->page()->number() + 1) +  "}, fitpaper]{" + fileinfo.fileName() + "}\n\\end{document}\n";
		
		QString wd = fileinfo.absoluteDir().absolutePath();

		m_runner = std::unique_ptr<LatexRunner>(new LatexRunner(input, wd, saveas, this));
	}

}

void DocumentMarkerGui::removeMarker()
{
	if(m_marker)
	{
		m_marker->page()->removeMarker(m_marker);
		setDocumentMarker(nullptr);
	}
}

DocumentMarkerGui::~DocumentMarkerGui()
{
}

