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

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QMainWindow>
#include "ui_MainDialog.h"
#include <vector>
#include <memory>

class QComboBox;
class DocumentSettings;
class SummarizeDocument;
class PrerenderThread;
class AutoMarkThread;

class MainDialog : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

	public:
		MainDialog(QWidget* parent = 0);
		DocumentSettings* documentSettings() const;
		~MainDialog();

	public slots:
		void changeZoom(int index);
		void change_margins();
		void runAutoMark();
		void openDocument();
		void exportLatex();
		void forward();
		void backward();
		void refreshNavigationButtons();
		void changePage(int number);
		void loadDocument(QString fileName);
		
	protected slots:
		void autoMarkFinished();
		void autoMarkPage(DocumentPage *page, bool boundingBox);
		void openAutocut();

	private:
		QComboBox *m_cbZoom;
		QComboBox *m_cbPage;
		std::vector<double> m_scaleFactors;
		std::shared_ptr<DocumentSettings> m_settings;
		std::shared_ptr<SummarizeDocument> m_sdoc;
		std::unique_ptr<PrerenderThread> m_renderthread;
		AutoMarkThread* m_automarkthread;
};

#endif

