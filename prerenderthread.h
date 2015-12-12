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
#ifndef PRERENDERTHREAD_H
#define PRERENDERTHREAD_H

#include <QThread>
#include <QMutex>
#include <memory>
#include <atomic>

class DocumentSettings;
class SummarizeDocument;

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class PrerenderThread : public QThread
{
	Q_OBJECT

	public:
		PrerenderThread(std::shared_ptr<DocumentSettings> settings, std::shared_ptr<SummarizeDocument> &doc);
		~PrerenderThread();
		void stop();
		void pause(bool pause = true);
		void setViewingPage(int number);
		
	protected:
		std:shared_ptr<DocumentSettings> m_settings;
		std::shared_ptr<SummarizeDocument> m_sdoc;
		void run();
		int m_currentPage, m_pagecount;
		bool m_bStop, m_bViewingPage, m_bSelfStop;
		std::atomic<bool> m_bPause;
		QMutex m_stopmutex;

};

#endif
