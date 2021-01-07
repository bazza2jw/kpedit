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

#ifndef ROUND_H
#define ROUND_H

#include <QObject>
#include <QString>
#include <QtPlugin>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QAction>
#include <QApplication>
#include <QDebug>
#include "../../kpedit/src/interfaces.h"

class Plugin : public QObject, public KPPluginInterface
{
     Q_OBJECT;
    Q_PLUGIN_METADATA(IID "jsonplugin")
     Q_INTERFACES(KPPluginInterface);
     
public:
     Plugin();
     virtual ~Plugin();
     QString pluginVersion() const {return "0.1";}
     void onLoad(KPInterface*);
     void onUnload();
     QAction* createAction();


     public slots:
	  void activate();
	  
private:
	  KPInterface* kpedit;
	  QString pointsToJSON();
     private slots:

};


#endif
