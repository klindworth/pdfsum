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


#include "latexexportdialog.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include "summarizedocument.h"
#include "pdfmarker.h"
#include "documentpage.h"
#include "documentsettings.h"
#include "latexrunner.h"

LatexExportDialog::LatexExportDialog(QWidget* parent)
: QDialog( parent ), Ui::Dialog()
{
	init();
}

LatexExportDialog::LatexExportDialog(std::shared_ptr<SummarizeDocument> &sdoc, DocumentSettings *settings, QWidget* parent): QDialog( parent ), Ui::Dialog(), m_sdoc(sdoc), m_settings(settings)
{
	init();
}

void LatexExportDialog::init()
{
	setupUi(this);

	connect(pbCreateOutput, SIGNAL(clicked()), this, SLOT(createOutput()));
	connect(pbSavePdf, SIGNAL(clicked()), this, SLOT(createPdf()));
	connect(btnChoosePath, SIGNAL(clicked()), this, SLOT(openFileDialog()));

	m_latexrunner = nullptr;
}

void LatexExportDialog::createPdf()
{
	QString saveas = QFileDialog::getSaveFileName(this, tr("Choose place for PDF"), QString(), "*.pdf");
	if(!saveas.isEmpty())
	{
		lwStatus->clear();
		lwStatus->addItem("started");
		saveas += ".pdf";
		QFileInfo fileinfo(m_sdoc->filepath());
		
		QString wd = fileinfo.absoluteDir().absolutePath();
		QString input = createLatex(true, false);
		
		lwStatus->addItem(tr("LaTeX sourcecode generated"));

		if(m_latexrunner)
			delete m_latexrunner;
		
		m_latexrunner = new LatexRunner(input, wd, saveas, this);
		connect(m_latexrunner, SIGNAL(statusMessage(QString)), this, SLOT(statusMessageReceived(QString)));
		connect(m_latexrunner, SIGNAL(errorMessage(QString)), this, SLOT(statusMessageReceived(QString)));
		connect(m_latexrunner, SIGNAL(finished(LatexRunner*)), this, SLOT(pdfCreationFinished()));
	}
}

void LatexExportDialog::statusMessageReceived(QString message)
{
	lwStatus->addItem(message);
}

void LatexExportDialog::pdfCreationFinished()
{
	lwStatus->addItem(tr("Finished!"));
}


void LatexExportDialog::setSummarizeDocument(std::shared_ptr<SummarizeDocument>& sdoc)
{
	m_sdoc = sdoc;
}

void LatexExportDialog::setDocumentSettings(DocumentSettings *settings)
{
	m_settings = settings;
}

QString LatexExportDialog::createLatex(bool bComplete, bool bAbsolutePaths)
{
	QString output;
	int count = m_sdoc->pageCount();
	
	bool bIncludePdf = rbSeperatePages->isChecked();
	
	double dPageHeight = 28.0 / dsZoom->value(); //fit pagesize to zoom...
	
	QString sZoom = dsZoom->value() == 1.0 ? "" : ", scale=" + QString::number(dsZoom->value(), 'f', 2);
	//qDebug(sZoom.toAscii());
	
	if(bComplete)
	{
		output += "\\documentclass[a4paper,10pt]{article}\n";
		if(bIncludePdf)
			output += "\\usepackage{pdfpages}\n";
		else
		{
			output += "\\usepackage{graphicx}\n";
			output += "\\usepackage[paper=a4paper,left=" + QString::number(m_settings->margins().left.value*10.0) + "mm,right=1mm,top=5mm,bottom=5mm]{geometry}\n";
		}
		output += "\\begin{document}\n";
	}

	QString path;
	if(bAbsolutePaths)
		path = m_sdoc->filepath();
	else
	{
		QFileInfo file(m_sdoc->filepath());
		path = file.fileName();
	}
	
	double usedHeight = 0.0;
	for(int i = 0; i < count; ++i)
	{
		if(usedHeight + m_sdoc->page(i)->markedHeight().value > dPageHeight - usedHeight && cbCompletePage->isChecked())
		{
			usedHeight = 0;
			output += "\\pagebreak\n";
		}
		
		usedHeight += m_sdoc->page(i)->markedHeight().value;
		
		//const std::deque<PdfMarker*>& list = m_sdoc->page(i)->markers();
		//for(std::deque<PdfMarker*>::const_iterator it = list.cbegin(); it != list.cend(); ++it)
		for(const PdfMarker* marker : m_sdoc->page(i)->markers())
		{
			if(bIncludePdf)
				output += "\\includepdf[fitpaper, clip=true," + marker->toLatexViewport()+ ", pages={" + QString::number(i+1) + "}" + sZoom + "]{" + path +"}\n";
			else
				output += "\\includegraphics*[" + marker->toLatexViewport()+ ", page=" + QString::number(i+1) + sZoom + "]{" + path +"}\\\\\n";
		}
	}
	if(bComplete)
		output += "\\end{document}\n";
	
	return output;
}

void LatexExportDialog::createOutput()
{
	bool bComplete = cbWholeDocument->isChecked();
	bool bAbsolutePaths = rbAbsolutePaths->isChecked();
	QString output = createLatex(bComplete, bAbsolutePaths);

	txtOutput->setText(output);
}

void LatexExportDialog::openFileDialog()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Choose place for file"), QString(), ".tex");
	if(! path.isEmpty())
		lePath->setText(path + ".tex");
	else
		lePath->setText("");
}

void LatexExportDialog::closeEvent(QCloseEvent *event)
{
	if(m_latexrunner->isRunning())
	{
		QMessageBox::StandardButton result = QMessageBox::warning(this, tr("Not finished"), "Processing hasn't finshed. Abort and force to quit?", QMessageBox::Yes | QMessageBox::No);
		if(result == QMessageBox::No)
		{
			event->ignore();
			return;
		}
	}
	event->accept();
}

