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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QSettings>
#include <QDir>
#include <QHash>
#include <QProgressDialog>
#include <QPluginLoader>
#include <QList>
#include <QFileDialog>
#include "kpplayer.h"
#include "linkdialog.h"
#include "keypoint.h"
#include "exportdialog.h"
#include "renderdialog.h"
#include "aboutdialog.h"
#include "interfaces.h"
#include "ui_mainwindow.h"

#define CACHE_SIZE 512

struct HistoryItem
{
HistoryItem():point(NULL),frame(-1){}
     ::KeyPoint* point;
     KeyPointPos pos;
     long frame;
     QString command;
     bool hasCache; //potrzebne bo cache może być puste
     QList<::KeyPoint> cache;
};

class MainWindow : public QMainWindow, public Ui::MainWindow, public KPInterface
{
     Q_OBJECT;
public:
     MainWindow();
     virtual ~MainWindow();

     QList<::KeyPoint>* getPoints() {return &points;}
     QSettings* settings() {return set;}
     bool openVideo(const QString& path);
     long countPlayerFrames() const {return player->countFrames();}
     long countPlayerRealFrames() const {return player->countRealFrames();}
     long currentPlayerPos() const {return player->getCurrentPos();}
     void setPlayerFakeFrames(long ff) {player->setFakeFramesCount(ff);}
     ::KeyPoint* getPointByName(const QString& name);
     int getPointIndex(const ::KeyPoint* point);
     QString pointsToCsv();
     void pointsFromCsv(const QString& csv);
     void redrawPlayerFrame(bool reseek=false);
     void redrawPreview();
     QString projectToXml();
     bool projectFromXml(const QString& xstr);
     QString relativeVideoPath();
     QString projectDir();
     void drawPoints(QImage* dst, const ::KeyPoint* p, bool antialiasing);
     void drawConnections(QImage* dst, bool antialiasing, bool allowHint = true);
     void completeFrames(::KeyPoint* p = NULL, long total = 0, long insertAt = -1);
     int currentPointIndex() 
     {return pointBox->count() ? pointBox->itemData(pointBox->currentIndex()).toInt() : -1;}
     int findClosestIndex(double x, double y);
     void completeFakeFrames(bool allowPlayerTrim = false);
     long getMaxFrameCount();
     bool isChanged() {return changed;}
     void cacheState(const QString& command, KP::CommandType type);

     static Mat qimage2mat(const QImage& qimage);

private:
     KPPlayer* player;
     QList<::KeyPoint> points;
     QSettings* set;
     QString projectFile;
     PreviewWidget* preview;
     bool ctrlMod;
     bool cacheMessage;
     RenderSettings renderSet;
     QList<HistoryItem> uhist;
     QList<HistoryItem> rhist;
     HistoryItem lastMove;
     bool changed;
     QList<KPPluginInterface*> plugins;
     KPPluginInterface* renderer;
     void keyPressEvent(QKeyEvent*k)
     {if(k->key() == Qt::Key_Shift){ctrlMod=true;actionSelectMode->setChecked(true);}}
     void keyReleaseEvent(QKeyEvent*k)
     {if(k->key() == Qt::Key_Shift){ctrlMod=false;actionSelectMode->setChecked(false);}}
     ::KeyPoint* getPointByName(QList<::KeyPoint>* list, const QString& name);
     void resizeEvent(QResizeEvent*){splitterMoved();}
     void closeEvent(QCloseEvent*);
     void showEvent(QShowEvent*);
     void updateTable(long frame, double x, double y);
     void matchSelectionToFrame();
     void saveProject(const QString& file);
     bool loadProject(const QString& file);
     void loadSettings();
     bool validateName(const QString& name);
     void cachePos();
     HistoryItem newHistoryItem();
     void loadPlugins();
     void unloadPlugins();
     void saveSplitter(QSplitter*, const QString&);
     void restoreSplitter(QSplitter*, const QString&, const QString&);
     void saveSplitters();
     void restoreSplitters();
     float cacheSize();
     float pointsSize(const QList<::KeyPoint>& list);

public slots:
     
     void playerStatusUpdated(KPPlayer::Status);
     void splitterMoved();
     void frameChanged(long frame);
     void playerClicked(double x, double y);
     void addPoint();
     void deletePoint();
     void refreshPointBox();
     void refreshTable();
     void currentItemChanged();
     void linkPoints();
     void openProject();
     void saveProject();
     void saveProjectAs();
     void newProject();
     void openVideo();
     void closeVideo(bool quiet=false);
     void selectMode(bool c){ctrlMod = c;}
     void renamePoint();

     void undo();
     void redo();
     void addFrame();
     void clearFrame();
     void deleteFrame();
     void duplicateFrame();
     void frameUp();
     void frameDown();
     void frameToBeginning();
     void frameToEnd();
     void about(){AboutDialog a; a.exec();}

     void pointsOnVideo(bool);
     void allPoints(bool);
     void playerAnti(bool);
     void previewAnti(bool);
     void hideLinks(bool);
     void prevFrameHint(bool);
     void highlightCurrent(bool);
     void drawOutline(bool);
     void exportPng();
     void exportAvi();
     void exportCsv();
     void importCsv();
     void renderSettings();

     void menuPluginsTriggered() {setChanged(true);}

     void clearCache() {uhist.clear();rhist.clear();undo();redo();lastMove=HistoryItem();}
     void setChanged(bool c = true);
};

#endif
