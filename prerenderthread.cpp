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
#include "prerenderthread.h"

#include "summarizedocument.h"
#include "documentpage.h"
#include "documentsettings.h"

#include <QMutexLocker>

PrerenderThread::PrerenderThread(DocumentSettings *settings, std::shared_ptr<SummarizeDocument>& doc)
 : QThread(), m_sdoc(doc)
{
	m_settings = settings;
	m_currentPage = 0;
	m_bStop = false;
	m_bPause = false;
	m_bViewingPage = false;
	m_bSelfStop = false;
	m_pagecount = m_sdoc->pageCount();
}

void PrerenderThread::run()
{
	setPriority(QThread::IdlePriority);

	bool restart = true;
	while(restart)
	{
		restart = false;
		for(int i = std::max(0, m_currentPage - 2); i < std::min(m_pagecount, m_currentPage + 10); ++i)
		{
			{
				QMutexLocker locker(&m_stopmutex);
				if(m_bStop)
					return;
				if(m_bViewingPage)
				{
					m_bViewingPage = false;
					restart = true;
					break;
				}
			}
			while(m_bPause)
				sleep(1);
			m_sdoc->page(i)->render_page(m_settings->resolution(), 1.0);
		}

		QMutexLocker locker2(&m_stopmutex);
		m_bSelfStop = true;
	}
}

void PrerenderThread::setViewingPage(int number)
{
	QMutexLocker locker(&m_stopmutex);
	m_bViewingPage = true;
	m_currentPage = number;
	if(m_bSelfStop)
	{
		m_bSelfStop = false;
		start();
	}
}

void PrerenderThread::pause(bool pause)
{
	m_bPause = pause;
}

void PrerenderThread::stop()
{
	QMutexLocker locker(&m_stopmutex);
	m_bStop = true;
}

PrerenderThread::~PrerenderThread()
{
}


