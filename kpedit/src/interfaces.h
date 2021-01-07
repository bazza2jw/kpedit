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

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QAction>
#include <QList>
#include <QSettings>
#include "keypoint.h"

class KPInterface;

class KPPluginInterface
{
public:
     virtual ~KPPluginInterface(){}
     virtual QString pluginVersion() const = 0;
     virtual void onLoad(KPInterface*) = 0;
     virtual void onUnload() = 0;
     virtual QAction* createAction() = 0;
     virtual void activate() = 0;

     //zewnetrzna obsluga renderingu
     virtual bool handlesRendering() {return false;}
     virtual bool drawBackground(QPixmap* bkg){return false;}
     virtual bool drawPoints(QImage* dst, const ::KeyPoint * p, bool antialiasing){return false;}
     virtual bool drawConnections(QImage* dst, bool antialiasing, bool allowHint){return false;}
};

namespace KP{
     enum CommandType{Undo=0,Redo=1};
}

class KPInterface
{
public:
     virtual ~KPInterface(){}

     virtual QList<::KeyPoint>* getPoints() = 0;
     virtual QSettings* settings() = 0;
     virtual bool openVideo(const QString& path) = 0;
     virtual void closeVideo(bool quiet=false) = 0;
     virtual long countPlayerFrames() const = 0;
     virtual long countPlayerRealFrames() const = 0;
     virtual long currentPlayerPos() const = 0;
     virtual void setPlayerFakeFrames(long ff) = 0;
     virtual ::KeyPoint* getPointByName(const QString& name) = 0;
     virtual int getPointIndex(const ::KeyPoint* point) = 0;
     virtual QString pointsToCsv() = 0;
     virtual void pointsFromCsv(const QString& csv) = 0;
     virtual void redrawPlayerFrame(bool reseek=false) = 0;
     virtual void redrawPreview() = 0;
     virtual QString projectToXml() = 0;
     virtual bool projectFromXml(const QString& xstr) = 0;
     virtual QString relativeVideoPath() = 0;
     virtual QString projectDir() = 0;
     virtual void drawPoints(QImage* dst, const ::KeyPoint* p, bool antialiasing) = 0;
     virtual void drawConnections(QImage* dst, bool antialiasing, bool allowHint = true) = 0;
     virtual void completeFrames(::KeyPoint* p = NULL, long total = 0, long insertAt = -1) = 0;
     virtual int currentPointIndex() = 0;
     virtual int findClosestIndex(double x, double y) = 0;
     virtual void completeFakeFrames(bool allowPlayerTrim = false) = 0;
     virtual long getMaxFrameCount() = 0;
     virtual void refreshPointBox() = 0;
     virtual void refreshTable() = 0;
     virtual bool isChanged() = 0;
     virtual void clearCache() = 0;
     virtual void cacheState(const QString& command, KP::CommandType type) = 0;
};

Q_DECLARE_INTERFACE(KPPluginInterface,"KPPluginInterface-1.0");

#endif
