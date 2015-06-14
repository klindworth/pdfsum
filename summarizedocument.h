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
#ifndef SUMMARIZEDOCUMENT_H
#define SUMMARIZEDOCUMENT_H

#include <QObject>
#include <QMutex>
#include <memory>
#include <vector>

#include "documentpage.h"

namespace Poppler {
	class Document;
}

class DocumentPage;
class DocumentSettings;

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class SummarizeDocument : public QObject
{
	Q_OBJECT
	public:
		SummarizeDocument(const QString& path, Poppler::Document *doc, QObject *parent = 0);
		~SummarizeDocument();
		
		DocumentPage* page(int number);
		std::shared_ptr<QImage> renderedPage(int number, double scale, const DocumentSettings* settings);
		int pageCount() const;
		Poppler::Document* pdfDocument() const;
		const QString& filepath() const;

		inline std::vector<DocumentPage*>::iterator pagesBegin()
		{
			return m_pages.begin();
		}

		inline std::vector<DocumentPage*>::iterator pagesEnd()
		{
			return m_pages.end();
		}
		
		static std::shared_ptr<SummarizeDocument> load(const QString& path, const QString& openpass);
		static std::shared_ptr<SummarizeDocument> loadGui(const QString& path, QWidget *parent);
		
	protected:
		const QString m_path;
		Poppler::Document *m_doc;
		std::vector<DocumentPage*> m_pages;
		QMutex m_rendermutex;
};

#endif
