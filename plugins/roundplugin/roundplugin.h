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
#include <math.h>
#include <QObject>
#include <QString>
#include <QtPlugin>
#include <QAction>
#include <QDebug>
#include "rounddialog.h"
#include "../../kpedit/src/interfaces.h"

class RoundDialog;

class RoundPlugin : public QObject, public KPPluginInterface
{
     Q_OBJECT;
    Q_PLUGIN_METADATA(IID "roundplugin")

     Q_INTERFACES(KPPluginInterface);
     
public:
     RoundPlugin();
     virtual ~RoundPlugin();
     QString pluginVersion() const {return "0.1";}
     void onLoad(KPInterface*);
     void onUnload();
     QAction* createAction();
     

     static double round(double n, int d)
     {return floorf(n * pow(10., d)+.5) / pow(10., d);}
     static double floor(double n, int d)
     {return floorf(n * pow(10., d)) / pow(10., d);}
     static double ceil(double n, int d)
     {return ceilf(n * pow(10., d)) / pow(10., d);}

     public slots:
	  void activate();
	  
private:
	  KPInterface* kpedit;
     private slots:

};


#endif
