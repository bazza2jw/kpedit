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

#ifndef ROUNDDIALOG_H
#define ROUNDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "ui_rounddialog.h"
#include "roundplugin.h"


enum Operation{OP_ROUND,OP_FLOOR,OP_CEIL};

struct RoundSet
{
RoundSet():ok(false){}
     int dec;
     long startFrame;
     long endFrame;
     Operation operation;
     bool ok;
};

class RoundDialog : public QDialog, public Ui::RoundDialog
{
     Q_OBJECT;
public:
     RoundDialog(long frames, RoundSet* rs);
     virtual ~RoundDialog();
     void end();
private:
     RoundSet* set;
     private slots:

     void updateExample();
     void roundClicked();
     void floorClicked();
     void ceilClicked();
};

#endif
