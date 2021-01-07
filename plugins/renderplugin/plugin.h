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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <QObject>
#include <QString>
#include <QtPlugin>
#include <QAction>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include "plugindialog.h"
#include "../../kpedit/src/interfaces.h"

class Plugindialog;

class Plugin : public QObject, public KPPluginInterface
{
     Q_OBJECT;
    Q_PLUGIN_METADATA(IID "renderplugin")

     Q_INTERFACES(KPPluginInterface);
     
public:
     Plugin();
     virtual ~Plugin();
     QString pluginVersion() const {return "0.1";}
     void onLoad(KPInterface*);
     void onUnload();
     QAction* createAction();
     bool handlesRendering(){return true;}
     bool drawBackground(QPixmap* bkg);
     bool drawPoints(QImage* dst, const KeyPoint* p, bool antialiasing);
     bool drawConnections(QImage* dst, bool antialiasing, bool allowHint);
     void reloadTextures();
     

     public slots:
	  void activate();
     
private:
	  QSettings* kpset;
	  QSettings* set;
	  KPInterface* kpedit;
	  QPixmap background;
	  QImage link;
	  QImage point;

	  void drawPoint(QPainter* painter, int x, int y, int rw, int rh, bool hl);
	  void drawLink(QPainter* painter, int x1, int y1, int x2, int y2, int width);

     private slots:

};


#endif
