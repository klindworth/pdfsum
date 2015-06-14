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
#ifndef DOCUMENTSETTINGS_H
#define DOCUMENTSETTINGS_H

#include <QObject>

class PdfView;
enum class Unit {cm, pixel};
/**
	@author Kai Klindworth <KaiKlindworth@web.de>
*/
class DocumentSettings : public QObject
{
	Q_OBJECT
	public:		
		DocumentSettings(int dpiX, int dpiY, QObject *parent = 0);
		~DocumentSettings();
		double pixelInCmX(int pixel) const;
		int cmInPixelX(double cm) const;
		double pixelInCmY(int pixel) const;
		int cmInPixelY(double cm) const;
		int dpiX() const;
		int dpiY() const;
		void setAutoWidth(bool autowidth, double leftMargin, double rightMargin);
		bool autoWidth() const;
		double leftMargin(Unit unit) const;
		double rightMargin(Unit unit) const;
		double topMargin(Unit unit) const;
		double bottomMargin(Unit unit) const;

		void registerView(PdfView *view);
		PdfView* view() const;
		
	public slots:
		void setTopMargin(double topMargin);
		void setBottomMargin(double bottomMargin);
		
	private:
		double m_dLeftMargin, m_dRightMargin, m_dTopMargin, m_dBottomMargin;
		int m_dpiX, m_dpiY;
		bool m_bAutoWidth;
		PdfView *m_view;

	signals:
		void marginsChanged();	
		
};

#endif
