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

#ifndef LINKDIALOG_H
#define LINKDIALOG_H

#include "keypoint.h"
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include "interfaces.h"
#include "ui_linkdialog.h"


class LinkDialog : public QDialog, public Ui::LinkDialog
{
     Q_OBJECT;
public:
     LinkDialog(KPInterface* kp);
     virtual ~LinkDialog();
     
private:
     KPInterface* kpedit;
     QList<::KeyPoint>* points;
     void refreshTree();
     void refreshList();
     void closeEvent(QCloseEvent* e);
     bool changed;
private slots:
     void linkClicked();
     void unlinkClicked();
     void validateLink();
};

#endif
