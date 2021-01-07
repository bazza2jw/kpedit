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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <opencv2/highgui/highgui.hpp>
#include "ui_exportdialog.h"


struct ExportSettings
{
ExportSettings():ok(false){}
     double fps;
     QString fname;
     int width;
     int height;
     int codec;
     bool antialiasing;
     bool ok;
};

class ExportDialog : public QDialog, public Ui::ExportDialog
{
     Q_OBJECT;
public:
     enum ExportMode{ExportAVI,ExportPNG};
     ExportDialog(ExportSettings* es, ExportMode m);
     virtual ~ExportDialog();

private:
     ExportSettings* set;
     ExportMode mode;
     private slots:
	  void exportClicked();
	  void chooseClicked();
};

#endif
