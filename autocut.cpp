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


#include "autocut.h"

#include <QFileDialog>

#include "documentsettings.h"
#include "documentpage.h"
#include "summarizedocument.h"
#include "pdfmarker.h"
#include "latexrunner.h"

AutoCut::AutoCut(QWidget* parent)
: QDialog( parent ), Ui::AutocutDialog()
{
	setupUi(this);
	connect(btnTargetSelect, &QPushButton::clicked, this, &AutoCut::selectTarget);
	connect(btnSourceSelect, &QPushButton::clicked, this, &AutoCut::selectSource);
	connect(btnStart,        &QPushButton::clicked, this, &AutoCut::start);
}

void AutoCut::selectSource()
{
	QString source = QFileDialog::getExistingDirectory(this, tr("Choose source"));
	if(!source.isEmpty())
		leSource->setText(source);
}

void AutoCut::selectTarget()
{
	QString target = QFileDialog::getExistingDirectory(this, tr("Choose target"));
	if(!target.isEmpty())
		leTarget->setText(target);
}

void AutoCut::start()
{
	lwStatus->addItem(tr("Start Queue"));
	QDir dir(leSource->text());
	dir.setNameFilters(QStringList("*.pdf"));
	
	m_jobQueue.append(dir.entryList(QDir::Files));
	startJob();
}

void AutoCut::startJob()
{
	m_currentRunner.release();

	if(m_jobQueue.size() > 0)
	{
		QString cpath = m_jobQueue.dequeue();
		lwStatus->addItem(tr("Start job: ") + cpath);
		cutFile(QDir(leSource->text()), QDir(leTarget->text()), cpath);
	}
	else
	{
		lwStatus->addItem(tr("All done!"));
		lwStatus->addItem("--------------------------------");
	}
}

void AutoCut::statusMessageReceived(QString msg)
{
	lwStatus->addItem(msg);
}

void AutoCut::errorMessageReceived(QString msg)
{
	lwStatus->addItem(msg);
	startJob();
}

void AutoCut::cutFile(QDir dirSource, QDir dirTarget, QString fileName)
{
	DocumentSettings *settings = new DocumentSettings(physicalDpiX(), physicalDpiY(), this);
	//qDebug(path);
	std::shared_ptr<SummarizeDocument> doc = SummarizeDocument::loadGui(dirSource.filePath(fileName), this);
	if(doc->pageCount() > 0)
	{
		DocumentPage *page = doc->page(0);
		page->autoMarkCombined(settings, 250, document_units::centimeter(0.0), true, true);
		//page->autoBoundingBox(settings, 250);
		const std::deque<PdfMarker*>& markers = page->markers();
		if(markers.size() > 0)
		{
			PdfMarker *marker = markers.front();
			
			QString input = "\\documentclass[a4paper,10pt]{article}\n\\usepackage{graphicx}\n\\usepackage{pdfpages}\n\\begin{document}\n\\includepdf[";
			input += marker->toLatexViewport();
			input += ", clip=true, pages={1}, fitpaper]{" + fileName + "}\n\\end{document}\n";
			
			m_currentRunner = std::make_unique<LatexRunner>(input, dirSource.path(), dirTarget.filePath(fileName), this);
			connect(m_currentRunner.get(), &LatexRunner::statusMessage, this, &AutoCut::statusMessageReceived);
			connect(m_currentRunner.get(), &LatexRunner::errorMessage,  this, &AutoCut::errorMessageReceived);
			connect(m_currentRunner.get(), &LatexRunner::finished,      this, &AutoCut::startJob);
		}
		else
			lwStatus->addItem(fileName + tr(": no marker created"));
	//	page->
	}
	else
		lwStatus->addItem(fileName + tr(": no pages in document"));
}

AutoCut::~AutoCut()
{

}

/*$SPECIALIZATION$*/


