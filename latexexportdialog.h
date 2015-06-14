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

#ifndef LATEXEXPORTDIALOG_H
#define LATEXEXPORTDIALOG_H

#include <QDialog>
#include <memory>
#include "ui_ExportDialog.h"

class SummarizeDocument;
class DocumentSettings;
class LatexRunner;

class LatexExportDialog : public QDialog, private Ui::Dialog
{
	Q_OBJECT

	public:
		LatexExportDialog(QWidget* parent = 0);
		LatexExportDialog(std::shared_ptr<SummarizeDocument> &sdoc, DocumentSettings *settings, QWidget* parent = 0);
		~LatexExportDialog();
		
		void setSummarizeDocument(std::shared_ptr<SummarizeDocument> &sdoc);
		void setDocumentSettings(DocumentSettings *settings);
		QString createLatex(bool bComplete, bool bAbsolutePaths);


	public slots:
		void createOutput();
		void createPdf();
		void openFileDialog();
		
	protected slots:
		void statusMessageReceived(QString message);
		void pdfCreationFinished();

		
	private:
		void init();
		std::shared_ptr<SummarizeDocument> m_sdoc;
		DocumentSettings *m_settings;
		LatexRunner *m_latexrunner;

};

#endif

