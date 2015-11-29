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
#include "automarkthread.h"

#include "summarizedocument.h"
#include "documentpage.h"
#include "documentsettings.h"

AutoMarkThread::AutoMarkThread(const std::shared_ptr<SummarizeDocument>& doc, const DocumentSettings *settings, bool boundingBox) : QThread(), m_sdoc(doc), m_settings(settings), m_boundingBox(boundingBox)
{

}

void AutoMarkThread::run()
{
	setPriority(QThread::LowPriority);
	int count = m_sdoc->pageCount();
	for(int i = 0; i < count; ++i)
	{
		//TODO: check, if 1.0 is a possible source of bugs, if page breaks are used
		m_sdoc->page(i)->renderPage(m_settings->resolution(), 1.0);
		emit readyForAutoMark(m_sdoc->page(i), m_boundingBox);
		emit progressChanged((i+1)*100/count);
	}
}

AutoMarkThread::~AutoMarkThread()
{
}


