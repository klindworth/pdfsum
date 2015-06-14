/***************************************************************************
 *   Copyright (C) 2009, 2011, 2015 by Kai Klindworth                      *
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
#ifndef LATEXRUNNER_H
#define LATEXRUNNER_H

#include <QObject>
#include <QProcess>
#include <QDir>

/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/

class LatexRunner : public QObject
{
	Q_OBJECT
	public:
		LatexRunner(const QString& input, const QString& wd, const QString& dest, QObject *parent = 0);
		~LatexRunner();
		
	protected slots:
		void latexFinished(int exitCode, QProcess::ExitStatus exitStatus);	
		void latexError(QProcess::ProcessError err);
		
	protected:
		QProcess *m_proc;
		QDir m_tmppath;
		QString m_dest;
		
	signals:
		void finished(LatexRunner*);
		void statusMessage(QString);
		void errorMessage(QString);

};

#endif
