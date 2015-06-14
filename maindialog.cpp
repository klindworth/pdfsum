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


#include "maindialog.h"

#include <QComboBox>
#include <QFileDialog>
#include <QCoreApplication>
#include <stdexcept>
#include "documentsettings.h"
#include "summarizedocument.h"
#include "documentpage.h"
#include "pdfmarker.h"
#include "latexexportdialog.h"
#include "prerenderthread.h"
#include "automarkthread.h"
#include "autocut.h"

#include <memory>

MainDialog::MainDialog(QWidget* parent)
: QMainWindow( parent ), Ui::MainWindow()
{
	setupUi(this);

	m_automarkthread = nullptr;
	
	actionBack->setEnabled(false);
	actionForward->setEnabled(false);
	
	m_settings = std::unique_ptr<DocumentSettings>(new DocumentSettings(physicalDpiX(), physicalDpiY()));
	//m_settings->setAutoWidth(true, 2.0, 2.0);
	m_view->setDocumentSettings(documentSettings());
	m_view->setDocumentMarkerGui(markerProperties);
	
	m_scaleFactors = std::vector<double>{0.5, 0.75, 1.0, 1.5, 2.0};
	
	m_cbPage = new QComboBox();
	toolBar->addWidget(new QLabel("Seite"));
	toolBar->addWidget(m_cbPage);
	
	m_cbZoom = new QComboBox();
	for(double value : m_scaleFactors)
		m_cbZoom->addItem(QString::number((int)(value*100))+" %", value);
	m_cbZoom->setCurrentIndex(2);
	toolBar->addWidget(new QLabel("Zoom"));
	toolBar->addWidget(m_cbZoom);
	
	connect(m_cbZoom, SIGNAL(currentIndexChanged(int)), this, SLOT(changeZoom(int)));
	connect(dsLeftMargin, SIGNAL(valueChanged(double)), this, SLOT(changeAutoWidth()));
	connect(dsRightMargin, SIGNAL(valueChanged(double)), this, SLOT(changeAutoWidth()));
	connect(cbActivateAutoWidth, SIGNAL(stateChanged(int)), this, SLOT(changeAutoWidth()));
	connect(m_cbPage, SIGNAL(currentIndexChanged(int)), this, SLOT(changePage(int)));
	connect(pbAutoMark, SIGNAL(clicked()), this, SLOT(runAutoMark()));
	connect(actionEnableMarker, SIGNAL(toggled(bool)), m_view, SLOT(setAddMarkerEnabled(bool)));
	connect(action_open, SIGNAL(triggered( bool )), this, SLOT(openDocument()));
	connect(actionAutocut, SIGNAL(triggered(bool)), this, SLOT(openAutocut()));
	connect(dsTopMargin, SIGNAL(valueChanged(double)), documentSettings(), SLOT(setTopMargin(double)));
	connect(dsBottomMargin, SIGNAL(valueChanged(double)), documentSettings(), SLOT(setBottomMargin(double)));
	connect(actionLatex, SIGNAL(triggered( bool )), this, SLOT(exportLatex()));
	connect(actionBack, SIGNAL(triggered(bool)), this, SLOT(backward()));
	connect(actionForward, SIGNAL(triggered(bool)), this, SLOT(forward()));
	
	//loadDocument("/home/kai/tmp/latextest/dso-howto.pdf");
}

DocumentSettings* MainDialog::documentSettings() const
{
	return m_settings.get();
}

void MainDialog::openAutocut()
{
	AutoCut *acut = new AutoCut(this);
	acut->setVisible(true);
}

void MainDialog::backward()
{
	m_cbPage->setCurrentIndex(m_view->page()->number() - 1);
}

void MainDialog::forward()
{
	m_cbPage->setCurrentIndex(m_view->page()->number() + 1);
}

void MainDialog::changePage(int number)
{
	if( !(m_sdoc && number >= 0 && number < m_sdoc->pageCount()) )
		return;
		//throw std::invalid_argument("nonexisting page");

	//refresh view
	if(m_sdoc && number >= 0 && number < m_sdoc->pageCount() )
		m_view->setPage(m_sdoc->page(number));

	//refresh remaining stuff
	markerProperties->setDocumentMarker(nullptr);
	if(m_renderthread)
		m_renderthread->setViewingPage(number);

	refreshNavigationButtons();
}

void MainDialog::refreshNavigationButtons()
{
	if(m_view->page())
	{
		actionBack->setEnabled(m_view->page()->number() != 0);
		actionForward->setEnabled(m_view->page()->number() != m_sdoc->pageCount() - 1);
	}
}

void MainDialog::changeAutoWidth()
{
	m_view->changeAutoWidth(cbActivateAutoWidth->checkState() == Qt::Checked, dsLeftMargin->value(), dsRightMargin->value());
}

void MainDialog::changeZoom(int iIndex)
{
	double zoom = m_scaleFactors[iIndex];
	m_view->setZoom(zoom);
}

void MainDialog::autoMarkFinished()
{
	m_pbAutoProgressBar->setEnabled(false);
	if(m_renderthread)
		m_renderthread->pause(false);
	if(m_automarkthread)
	{
		m_automarkthread->disconnect();
		//m_automarkthread.reset();
		delete m_automarkthread;
		m_automarkthread = nullptr;
	}
	
	pbAutoMark->setEnabled(true);
	m_view->update();
	qDebug("automark finished");
}

void MainDialog::runAutoMark()
{
	bool removeAutomaticMarker = cbRemoveAutomatic->isChecked();

	pbAutoMark->setEnabled(false);
	if(m_renderthread)
		m_renderthread->pause();
	if(rbCompleteDocument->isChecked())
	{
		std::for_each(m_sdoc->pagesBegin(), m_sdoc->pagesEnd(), [=](DocumentPage* page){
			if(removeAutomaticMarker)
				page->removeAllMarkers(true);
		});

		m_pbAutoProgressBar->setEnabled(true);
		m_pbAutoProgressBar->setValue(0);

		m_automarkthread = new AutoMarkThread(m_sdoc, documentSettings(), cbBoundingBox->isChecked());
		connect(m_automarkthread, SIGNAL(readyForAutoMark(DocumentPage*, bool)), this, SLOT(autoMarkPage(DocumentPage*, bool)), Qt::QueuedConnection);
		connect(m_automarkthread, SIGNAL(progressChanged(int)), m_pbAutoProgressBar, SLOT(setValue(int)), Qt::QueuedConnection);
		connect(m_automarkthread, SIGNAL(finished()), this, SLOT(autoMarkFinished()), Qt::QueuedConnection);
		m_automarkthread->start();
		qDebug("automark started");
	}
	else
	{
		if(removeAutomaticMarker)
			m_view->page()->removeAllMarkers(true);
		m_view->page()->autoMarkCombined(documentSettings(), spGreyThreshold->value(), dspHeightThreshold->value(), !cbCompletePageWidth->isChecked(), cbBoundingBox->isChecked());
		autoMarkFinished();
	}
}

void MainDialog::autoMarkPage(DocumentPage *page, bool boundingBox)
{
	page->autoMarkCombined(documentSettings(), spGreyThreshold->value(), dspHeightThreshold->value(), !cbCompletePageWidth->isChecked(), boundingBox);
	
	if(page == m_view->page())
		m_view->scene()->update();
}

void MainDialog::openDocument()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("PDF files (*.pdf)"));
	if(!fileName.isEmpty())
		loadDocument(fileName);
}

void MainDialog::loadDocument(QString fileName)
{
	m_view->clear();

	if(m_renderthread)
	{
		m_renderthread->stop();
		m_renderthread->quit();
		m_renderthread->wait();
	}
	
	m_sdoc = SummarizeDocument::loadGui(fileName, this);
	m_cbPage->clear();
	for(int i = 0; i < m_sdoc->pageCount(); ++i)
		m_cbPage->addItem(QString::number(i + 1));

	m_view->setPage(m_sdoc->page(0));
	refreshNavigationButtons();
	
	m_renderthread = std::unique_ptr<PrerenderThread>( new PrerenderThread(documentSettings(), m_sdoc) );
	m_renderthread->start();
}

void MainDialog::exportLatex() 
{
	LatexExportDialog diag(m_sdoc, documentSettings(), this);
	diag.exec();
}

MainDialog::~MainDialog()
{

}


