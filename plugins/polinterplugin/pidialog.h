/************************************************************************
** KPEdit plug-in
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

#ifndef PIDIALOG_H
#define PIDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPoint>
#include <QColor>
#include <QPixmap>
#include <QList>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDebug>
#include "polinter.h"
#include "ui_pidialog.h"

class PIDialog : public QDialog, public Ui::PIDialog
{
     Q_OBJECT;
public:
     PIDialog(KPInterface* kp);
     virtual ~PIDialog();

private:
     void drawDot(QPixmap* pix, double nx, double ny, int r, QColor = QColor("black"));
     void setNodes(QList<KeyPointPos> n);
     void showEvent(QShowEvent*) {refresh();}
     void resizeEvent(QResizeEvent*){refresh(true);}
     QPixmap last;
     KPInterface* kpedit;
     QList<KeyPointPos> nodes;
     bool areFramesSet();

     private slots:
     void refresh(bool lastonly = false);
     void interpolate();
};

#endif
