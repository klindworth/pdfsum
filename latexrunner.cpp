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
#include "latexrunner.h"

#include<QCoreApplication>
#include<QDir>

LatexRunner::LatexRunner(const QString& input, const QString& wd, const QString& dest, QObject *parent)
 : QObject(parent)
{
	m_dest = dest;
	m_proc = new QProcess(this);
	m_tmppath.setPath(QDir::tempPath());
	qDebug(m_tmppath.absolutePath().toLatin1());
	
	connect(m_proc, SIGNAL(finished( int, QProcess::ExitStatus )), this, SLOT(latexFinished(int, QProcess::ExitStatus)));
	connect(m_proc, SIGNAL(error( QProcess::ProcessError )), this, SLOT(latexError(QProcess::ProcessError)));
	
	m_proc->setWorkingDirectory(wd);
	QStringList args;
	args << "-jobname=pdfsum";
	args << "-output-directory" << m_tmppath.absolutePath();
	
	m_proc->start("pdflatex", args);
	m_proc->write(input.toLatin1());
}


void LatexRunner::latexFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode);
	if(exitStatus == QProcess::NormalExit)
	{
		//move the output
		QFile file(m_tmppath.filePath("pdfsum.pdf"));
		if(file.exists())
		{
			if(file.rename(m_dest))
			{
				QFile auxfile(m_tmppath.filePath("pdfsum.aux"));
				auxfile.remove();
				
				QFile logfile(m_tmppath.filePath("pdfsum.log"));
				logfile.remove();
				
				emit statusMessage(m_dest + tr(": File successfully created"));
				emit finished(this);
			}
			else
				emit errorMessage(m_dest + tr(": Moving the outputfile failed. Existed the final destination file before?"));
		}
		else
			emit errorMessage(m_dest + tr(": Outputfile couldn't be found"));
	}
	else
		emit errorMessage(m_dest + tr(": LaTeX execution error"));
}

void LatexRunner::latexError(QProcess::ProcessError err)
{
	if(err == QProcess::FailedToStart)
		emit errorMessage(tr("Failed to start! Latex installed?"));
	else
		emit errorMessage("ProcessError");
}

LatexRunner::~LatexRunner()
{
	//delete m_proc;
}


