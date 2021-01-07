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

#ifndef POLINTERPLUGIN_H
#define POLINTERPLUGIN_H

#include <QObject>
#include <QString>
#include <QtPlugin>
#include <QAction>
#include <QMessageBox>
#include <QDebug>
#include "pidialog.h"
#include "../../kpedit/src/interfaces.h"

class PolinterPlugin : public QObject, public KPPluginInterface
{
     Q_OBJECT;
    Q_PLUGIN_METADATA(IID "polinterplugin")

     Q_INTERFACES(KPPluginInterface);
public:
     PolinterPlugin();
     virtual ~PolinterPlugin();
     
     QString pluginVersion() const {return "0.1";}
     void onLoad(KPInterface* kp);
     void onUnload();
     QAction* createAction();

     public slots:
	  void activate();

private:
	  KPInterface* kpedit;
	  
};

#endif
