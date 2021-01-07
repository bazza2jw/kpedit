/************************************************************************
** KPEdit - A 2D skeletal animation editor.
** Copyright (C) 2013 Lukasz Chodyla <chodak166@op.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
************************************************************************/

#ifndef RENDERDIALOG_H
#define RENDERDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QMessageBox>
#include "ui_renderdialog.h"

struct RenderSettings
{
     float pointSize; //promien 0.0 - 1.0, proporcjonalnie do wys. i szer. klatki (sredniej)
     float linkSize; //rozmiar polaczenia proporcjonalnie do wielkosci punktu
     QColor linkColor;
     QColor pointColor;
     QColor highlightColor;
     QColor backgroundColor;
};

class RenderDialog : public QDialog, public Ui::RenderDialog
{
private:
     Q_OBJECT;
public:
     RenderDialog(RenderSettings* rs);
     virtual ~RenderDialog();

private:
     RenderSettings* set;
     private slots:
	  void pcolorClicked();
	  void lcolorClicked();
	  void hcolorClicked();
	  void bcolorClicked();
	  void saveClicked();
};

#endif
