/***************************************************************************
 *   Copyright (C) 2009, 2015 by Kai Klindworth                            *
 *                                                                         *
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

#ifndef AUTOCUT_H
#define AUTOCUT_H

#include <QDialog>
#include <QDir>
#include <QQueue>
#include <memory>
#include "ui_autocutdialog.h"

class LatexRunner;
class DocumentSettings;

class AutoCut : public QDialog, private Ui::AutocutDialog
{
	Q_OBJECT

public:
	AutoCut(QWidget* parent = 0);
	~AutoCut();
	void cutFile(QDir dirSource, QDir dirTarget, QString path);

protected:
	std::unique_ptr<LatexRunner> m_currentRunner;
	std::unique_ptr<DocumentSettings> _docsettings;
	QQueue<QString> m_jobQueue;

protected slots:
	void selectSource();
	void selectTarget();
	void start();
	void startJob();
	void errorMessageReceived(QString msg);
	void statusMessageReceived(QString msg);

};

#endif

