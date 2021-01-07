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

#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "ui_plugindialog.h"
#include "../../kpedit/src/interfaces.h"

class PluginDialog : public QDialog, public Ui::PluginDialog
{
     Q_OBJECT;
public:
     PluginDialog(KPInterface* kp);
     virtual ~PluginDialog();
private:
     KPInterface* kpedit;
     void translate(long frame, double x, double y);
     private slots:
	  void okClicked();
};

#endif
