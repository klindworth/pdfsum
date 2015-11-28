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
#include "summarizedocument.h"

#include <poppler-qt5.h>
#include "documentpage.h"
#include "documentsettings.h"

#include <QInputDialog>

SummarizeDocument::SummarizeDocument(const QString& path, Poppler::Document *doc, QObject *parent)
 : QObject(parent), m_path(path), m_doc(doc)
{
	m_doc->setRenderHint(Poppler::Document::Antialiasing);
	m_doc->setRenderHint(Poppler::Document::TextAntialiasing);
	
	m_pages = std::vector<DocumentPage*>(pageCount(), nullptr);
	//m_pages.reserve(pageCount());
	for(int i = 0; i < pageCount(); ++i)
		m_pages[i] = new DocumentPage(this, i);
		//m_pages.emplace_back(this, i);
}

const QString& SummarizeDocument::filepath() const
{
	return m_path;
}

Poppler::Document* SummarizeDocument::pdfDocument() const
{
	return m_doc;
}

DocumentPage* SummarizeDocument::page(int number)
{
	if(number >= pageCount() || number < 0)
		return nullptr;
	
	if(m_pages[number])
		return m_pages[number];
	else
	{
		m_pages[number] = new DocumentPage(this, number);
		return m_pages[number];
	}

	//return &(m_pages[number]);
}

std::shared_ptr<QImage> SummarizeDocument::renderedPage(int number, double scale, const DocumentSettings* settings)
{
	//QMutexLocker locker(&m_rendermutex);
	return std::make_shared<QImage>(pdfDocument()->page(number)->renderToImage(scale * settings->dpiX().value, scale * settings->dpiY().value));
}

int SummarizeDocument::pageCount() const
{
	return m_doc->numPages();
}

SummarizeDocument::~SummarizeDocument()
{
	for(int i = 0; i < pageCount(); ++i)
		delete m_pages[i];
	delete m_doc;
}


std::shared_ptr<SummarizeDocument> SummarizeDocument::load(const QString& path, const QString& openpass)
{
	Poppler::Document *doc = Poppler::Document::load(path, QByteArray(), openpass.toLatin1());
	if(doc && !doc->isLocked())
		return std::make_shared<SummarizeDocument>(path, doc);
	else
		return nullptr;
}

std::shared_ptr<SummarizeDocument> SummarizeDocument::loadGui(const QString& path, QWidget *parent)
{
	QString sPass;
	std::shared_ptr<SummarizeDocument> sdoc;
	while(true)
	{
		qDebug(sPass.toLatin1());
		sdoc = SummarizeDocument::load(path, sPass);
		
		if(!sdoc )
		{
			bool ok;
			sPass = QInputDialog::getText(parent, "PDF Summarizer", "Passwort", QLineEdit::Password, "", &ok);
			if(!ok)
				return sdoc;
		}
		else
			return sdoc;
	}
}

