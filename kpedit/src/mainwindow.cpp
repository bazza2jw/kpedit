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

#include "mainwindow.h"

MainWindow::MainWindow():ctrlMod(false),changed(false),renderer(NULL),cacheMessage(false)
{
     setupUi(this);
     
     player = new KPPlayer(NULL);
     playerLayout->addWidget(player);
     player->setStatusBar(statusbar);
     player->controlPanel->hide();

     preview = new PreviewWidget(previewFrame);

     set = new QSettings("KPEdit","KPEdit");
     restoreSplitters();

     connect(player, SIGNAL(frameChanged(long)), this, SLOT(frameChanged(long)));
     connect(player, SIGNAL(previewClicked(double,double)), 
	  this, SLOT(playerClicked(double,double)));
     connect(preview, SIGNAL(clicked(double,double)), 
	  this, SLOT(playerClicked(double,double)));
     
     connect(pointBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshTable()));
     connect(pointTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
	     this, SLOT(currentItemChanged()));
     connect(splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
     connect(dockSplitter, SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
     connect(player, SIGNAL(statusUpdated(KPPlayer::Status)), 
	     this, SLOT(playerStatusUpdated(KPPlayer::Status)));
     connect(menuPlugins, SIGNAL(triggered(QAction*)), this, SLOT(menuPluginsTriggered()));
     

     connect(actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
     connect(actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
     connect(actionAddPoint, SIGNAL(triggered()), this, SLOT(addPoint()));
     connect(actionDeletePoint, SIGNAL(triggered()), this, SLOT(deletePoint()));
     connect(actionRenamePoint, SIGNAL(triggered()), this, SLOT(renamePoint()));
     connect(actionLink, SIGNAL(triggered()), this, SLOT(linkPoints()));
     connect(actionSelectMode, SIGNAL(triggered(bool)), this, SLOT(selectMode(bool)));
     connect(actionExportPng, SIGNAL(triggered()), this, SLOT(exportPng()));
     connect(actionExportAvi, SIGNAL(triggered()), this, SLOT(exportAvi()));
     connect(actionExportCsv, SIGNAL(triggered()), this, SLOT(exportCsv()));
     connect(actionImportCsv, SIGNAL(triggered()), this, SLOT(importCsv()));
     connect(actionRenderSettings, SIGNAL(triggered()), this, SLOT(renderSettings()));

     connect(actionOpen, SIGNAL(triggered()), this, SLOT(openProject()));
     connect(actionSave, SIGNAL(triggered()), this, SLOT(saveProject()));
     connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveProjectAs()));
     connect(actionNew, SIGNAL(triggered()), this, SLOT(newProject()));
     connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
     connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

     connect(actionOpenVideo, SIGNAL(triggered()), this, SLOT(openVideo()));
     connect(actionCloseVideo, SIGNAL(triggered()), this, SLOT(closeVideo()));
     connect(actionPlay, SIGNAL(triggered()), player, SLOT(play()));
     connect(actionStop, SIGNAL(triggered()), player, SLOT(stop()));
     connect(actionPause, SIGNAL(triggered()), player, SLOT(pause()));
     connect(actionNextFrame, SIGNAL(triggered()), player, SLOT(nextFrame()));
     connect(actionPrevFrame, SIGNAL(triggered()), player, SLOT(prevFrame()));
     connect(actionRecord, SIGNAL(triggered()), player, SLOT(record()));

     connect(actionAddFrame, SIGNAL(triggered()), this, SLOT(addFrame()));
     connect(actionClearFrame, SIGNAL(triggered()), this, SLOT(clearFrame()));
     connect(actionDeleteFrame, SIGNAL(triggered()), this, SLOT(deleteFrame()));
     connect(actionDuplicateFrame, SIGNAL(triggered()), this, SLOT(duplicateFrame()));
     connect(actionFrameUp, SIGNAL(triggered()), this, SLOT(frameUp()));
     connect(actionFrameDown, SIGNAL(triggered()), this, SLOT(frameDown()));
     connect(actionFrameToBeginning, SIGNAL(triggered()), this, SLOT(frameToBeginning()));
     connect(actionFrameToEnd, SIGNAL(triggered()), this, SLOT(frameToEnd()));

     connect(actionPointsOnVideo, SIGNAL(triggered(bool)), this, SLOT(pointsOnVideo(bool)));
     connect(actionAllPoints, SIGNAL(triggered(bool)), this, SLOT(allPoints(bool)));
     connect(actionPlayerAnti, SIGNAL(triggered(bool)), this, SLOT(playerAnti(bool)));
     connect(actionPreviewAnti, SIGNAL(triggered(bool)), this, SLOT(previewAnti(bool)));
     connect(actionHideLinks, SIGNAL(triggered(bool)), this, SLOT(hideLinks(bool)));
     connect(actionPrevFrameHint, SIGNAL(triggered(bool)), this, SLOT(prevFrameHint(bool)));
     connect(actionHighlightCurrent, SIGNAL(triggered(bool)), this, SLOT(highlightCurrent(bool)));
     connect(actionOutline, SIGNAL(triggered(bool)), this, SLOT(drawOutline(bool)));
     
     loadSettings();
     loadPlugins();
     newProject();
}

MainWindow::~MainWindow()
{
     unloadPlugins();
     delete player;
     delete set;
}

void MainWindow::loadPlugins()
{
     QString ppath;
     QDir dir;
//#if defined(Q_WS_X11)
     dir = QDir("/usr/local/lib/kpedit/plugins/");
     if(dir.exists())
      ppath = "/usr/local/lib/kpedit/plugins/";
     else
//#endif
     //ppath = qApp->applicationDirPath()+"/plugins/";

     dir = QDir(ppath);
     qDebug() << "plugins path: " << ppath;
     QStringList filters;
     filters << "*.so" << "*.dll" << "*.lib";
     QStringList files = dir.entryList(filters, QDir::NoDotAndDotDot | QDir::Files);
     //for(int i=0;i<files.size();i++) files[i] = ppath+files[i];
     
     for(int i=0;i<files.size();i++)
     {
	  QPluginLoader pl(ppath+files.at(i));
	  KPPluginInterface* p = qobject_cast<KPPluginInterface*>(pl.instance());
	  if(p)
	  {
	       qDebug() << files.at(i) << " loaded! Plugin version: " << p->pluginVersion();
	       p->onLoad(this);
	       if(p->handlesRendering() && !renderer)
		    renderer = p;
	       menuPlugins->addAction(p->createAction());
	       plugins.append(p);
	  }
	  else
	       qDebug() << "Cannot load plugin: " << files.at(i) << "!";
     }
}

void MainWindow::unloadPlugins()
{
}

void MainWindow::showEvent(QShowEvent*)
{
     redrawPlayerFrame(player->getStatus()==KPPlayer::Stopped);
     redrawPreview();
     pointTable->setColumnWidth(0,pointTable->width()/2);
}

void MainWindow::frameChanged(long frame)
{
     // qDebug() << "points data size: " << (float)(sizeof(KeyPoint)*points.size()*getMaxFrameCount()*sizeof(KeyPointPos))/1024 << "KB";
     // qDebug() << "KeyPointPos size: " << sizeof(KeyPointPos);
     // qDebug() << "keyPointPos size: " << sizeof(KeyPointPos);
     // qDebug() << "points data size: " << pointsSize(points);
     //  qDebug() << "cache size: " << cacheSize();

     //qDebug() << "player frame changed to: " << frame;
     frame--; //pozycja dla klatki ktora wlasnie zniknela
     redrawPlayerFrame();
     redrawPreview();
     if(player->getStatus() == KPPlayer::Recording && player->previewWidget()->underMouse())
     {
	  if(uhist.size() || rhist.size())
	       clearCache();
	  if(!points.size())
	       return;
	  double x = player->previewWidget()->normalizedX();
	  double y = player->previewWidget()->normalizedY();
	  qDebug() << "RECORDING POINT " << x << ", " << y << " FOR FRAME " << frame;

	  int cp = currentPointIndex();
	  if(cp<0) return;
	  QList<KeyPointPos>& fp = points[cp].framePos;
	  qDebug() << "fp size: " << fp.size();
	  KeyPointPos pos;
	  pos.x = x;
	  pos.y = y;
	  if(fp.size()<=frame)
	       fp.append(pos);
	  else
	       fp.replace(frame,pos);
	  updateTable(frame,x,y);
	  setChanged(true);
     }
     actionDeleteFrame->setEnabled(frame+1 >= player->countRealFrames());
     matchSelectionToFrame();
}

void MainWindow::playerClicked(double x, double y)
{
     this->setFocus();
     if(!points.size() || player->getCurrentPos() < 0)
	  return;

     if(ctrlMod)
     {
	  int ci = findClosestIndex(x,y);
	  if(ci>=0)
	       ci = pointBox->findData(QVariant(ci));
	  if(ci>=0)
	       pointBox->setCurrentIndex(ci);
	  return;
     }

     if(player->getCurrentPos() < points.at(currentPointIndex()).framePos.size())
     {
	  cachePos();
	  KeyPointPos& p = points[currentPointIndex()].framePos[player->getCurrentPos()];
	  p.x = x;
	  p.y = y;
	  lastMove = newHistoryItem();
	  updateTable(player->getCurrentPos(), x, y);
	  redrawPlayerFrame(true);
	  redrawPreview();
	  setChanged(true);
     }
}

void MainWindow::closeEvent(QCloseEvent* e)
{
     if(isChanged())
     {
	  switch(QMessageBox::question(
		  this, tr("Question"),tr("The project has changed - do you want to save the changes before exiting?"), tr("Yes"), tr("No"), tr("Cancel")))
	  {
	  case 0: saveProject(); if(!isChanged()) e->accept(); else e->ignore(); break;
	  case 1: e->accept(); break;
	  case 2: e->ignore();
	  }
	       
     }
     set->setValue("mw_size",size());
     set->setValue("mw_pos",QPoint(x(),y()));
     saveSplitters();
}

bool MainWindow::openVideo(const QString& path)
{
     long fakes = player->countFakeFrames();
     player->setFakeFramesCount(0);
     clearCache();
     if(player->openSource(path))
     {
         player->nextFrame();
	 if(points.size())
	 {
	      completeFakeFrames();
	      for(int i=0;i<points.size();i++)
		   points[i].framePos.reserve(player->countFrames());
	 }
	 setChanged(true);
	 completeFrames();
	 refreshTable();
	 refreshPointBox();
	 return true;
     }
     player->showDefaultScreen();
     player->setFakeFramesCount(fakes);
     return false;
}

void MainWindow::completeFakeFrames(bool allowPlayerTrim)
{
     if(!points.size())
	  return;
     long dif = getMaxFrameCount() - player->countRealFrames();
     qDebug() << "existing frames - video frames = " << dif;
     if(dif >= 0) //jesli istniejacych klatek jest wiecej niz we wczytanym wideo
     {
	  player->setFakeFramesCount(dif); //dodajemy/zerujemy falszywe klatki
     }
     else if(dif < 0 && !allowPlayerTrim)
	  completeFrames(NULL,player->countFrames());
     else
	  player->setFakeFramesCount(player->countFakeFrames()+dif);
}

void MainWindow::completeFrames(::KeyPoint* p, long total, long insertAt)
{
     if(!total)
	  total = max(player->countFrames(),getMaxFrameCount());
     if(p)
     {
	  qDebug() << "adding frames for point " << p->name << ", " << total << " in total";
	  for(int i=p->framePos.size();i<total;i++)
	  {
	       if(insertAt < 0 || insertAt > p->framePos.size())
		    p->framePos.append(KeyPointPos());
	       else
		    p->framePos.insert(insertAt,KeyPointPos());
	  }
     }
     else
	  for(int i=0;i<points.size();i++)
	       completeFrames(&points[i],total,insertAt);
}

long MainWindow::getMaxFrameCount()
{
     long mf = 0;
     for(int i=0;i<points.size();i++)
	  if(points.at(i).framePos.size() > mf)
	       mf = points.at(i).framePos.size();
     return mf;
}

void MainWindow::playerStatusUpdated(KPPlayer::Status status)
{
     actionPlay->setEnabled(status == KPPlayer::Stopped);
     actionRecord->setEnabled(status == KPPlayer::Stopped);
     actionPrevFrame->setEnabled(status == KPPlayer::Stopped);
     actionNextFrame->setEnabled(status == KPPlayer::Stopped);
}

void MainWindow::refreshPointBox()
{
     disconnect(pointBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshTable()));
     pointBox->clear();
     for(int i=0;i<points.size();i++)
	  pointBox->addItem(points.at(i).name, QVariant(i));
     if(points.size())
	  pointBox->setCurrentIndex(points.size()-1);  
     connect(pointBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshTable()));
}

void MainWindow::addPoint()
{
     QString name = QInputDialog::getText(this, tr("Add point"), tr("Point name:"));
     if(!name.isEmpty())
     {
	  if(getPointByName(name))
	  {
	       QMessageBox::critical(this, tr("Error"), 
			       tr("The point with this name already exists!"));
	       return;
	  }
	  if(!validateName(name))
	       return;
	  cacheState(tr("add point"),KP::Undo);
          ::KeyPoint p;
	  p.name = name;
	  p.framePos.reserve(player->countFrames());
	  completeFrames(&p);
	  points.append(p);
	  refreshPointBox();
	  refreshTable(); //refreshPointBox byl rozlaczony z aktualizacja tabeli
	  redrawPreview();
	  setChanged(true);
     }
}

void MainWindow::deletePoint()
{
     if(!points.size())
	  return;
     if(!QMessageBox::question(this, tr("Question"),
			       tr("This operation cannot be undone. Do you really want to remove the \"")+
			       points.at(currentPointIndex()).name+
			       tr("\" point?"), tr("Yes"), tr("No")))
     {
	  clearCache();
	  int cp = currentPointIndex();
	  //usuwamy wszystkie polaczenia z tym punktem
	  for(int i=0;i<points.size();i++)
	       for(int j=0;j<points.at(i).linkedPoints.size();j++)
		    if(points.at(i).linkedPoints.at(j) == &points.at(cp))
			 points[i].linkedPoints.removeAt(j--); 
	  // ^ nie przerywamy ze wzgledu na to ze polaczen nie bedzie az tak wiele,
	  // a zabezpieczy to blad przy istniejacych powtorkach

	  points.removeAt(cp);
	  refreshPointBox();
	  refreshTable(); //refreshPointBox byl rozlaczony z aktualizacja tabeli
	  redrawPreview();
	  redrawPlayerFrame(true);
	  setChanged(true); 
     }
}

void MainWindow::refreshTable()
{
     qDebug() << "refreshing table...";
     int cr = pointTable->currentRow();
     
     disconnect(pointTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
	     this, SLOT(currentItemChanged()));
     pointTable->clearContents();
     pointTable->setRowCount(0);
     if(!points.size())
	  return;
     int cp = currentPointIndex();
     qDebug() << "pointBox count: " << pointBox->count();
     qDebug() << "current point index: " << cp;
     qDebug() << "points size: " << points.size();
     const QList<KeyPointPos>& fp = points.at(cp).framePos;
     pointTable->setRowCount(fp.size());
     for(int i=0;i<fp.size();i++)
	  updateTable(i,fp.at(i).x,fp.at(i).y);

     if(pointTable->rowCount()-1 >= cr)
	  pointTable->selectRow(cr);

     if(actionHighlightCurrent->isChecked())
     {
	  redrawPlayerFrame(true);
	  redrawPreview();
     }
     connect(pointTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
	     this, SLOT(currentItemChanged()));
     if(pointTable->currentRow() == -1 && pointTable->rowCount())
	  pointTable->selectRow(0);
     matchSelectionToFrame();
}

void MainWindow::splitterMoved()
{
     pointTable->setColumnWidth(0,pointTable->width()/2);
     redrawPreview();
}

void MainWindow::updateTable(long frame, double x, double y)
{
     if(pointTable->rowCount()<frame+1)
	  pointTable->setRowCount(frame+1);
     QTableWidgetItem* fitem = new QTableWidgetItem(QString::number(frame));
     QTableWidgetItem* xitem = new QTableWidgetItem(
	  x >=0 ? QString::number(x) : tr("not set"));
     QTableWidgetItem* yitem = new QTableWidgetItem(
	  y >=0 ? QString::number(y) : tr("not set"));
    
     if(frame >= player->countRealFrames())
     {
	  fitem->setIcon(QIcon(":/res/edit.png"));
	  xitem->setForeground(QColor("#008000"));
	  yitem->setForeground(QColor("#008000"));
     }
     pointTable->setVerticalHeaderItem(frame,fitem);
     pointTable->setItem(frame,0,xitem);
     pointTable->setItem(frame,1,yitem);
}

void MainWindow::currentItemChanged()
{
     //qDebug() << "current row: " << pointTable->currentRow();
     player->seek(pointTable->currentRow());
}

void MainWindow::matchSelectionToFrame()
{
     disconnect(pointTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
	     this, SLOT(currentItemChanged()));
     if(pointTable->rowCount()-1>=player->getCurrentPos())
	  pointTable->selectRow(player->getCurrentPos());
     else
     {
	  // odznaczenie?
     }
     connect(pointTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
	     this, SLOT(currentItemChanged()));
}

void MainWindow::linkPoints()
{
     if(!points.size())
	  return;
     LinkDialog ld(this);
     ld.exec();
     setChanged(true);
}

::KeyPoint* MainWindow::getPointByName(const QString& name)
{
     for(int i=0;i<points.size();i++)
	  if(points.at(i).name == name)
	       return &points[i];
     return NULL;
}

::KeyPoint* MainWindow::getPointByName(QList<::KeyPoint>* list, const QString& name)
{
     for(int i=0;i<list->size();i++)
	  if(list->at(i).name == name)
	       return &(*list)[i];
     return NULL;
}

int MainWindow::getPointIndex(const ::KeyPoint* point)
{
     for(int i=0;i<points.size();i++)
	  if(&points.at(i) == point)
	       return i;
     return -1;
}

QString MainWindow::projectToXml()
{

     QString xcontent;
     QXmlStreamWriter stream(&xcontent);
     stream.setAutoFormatting(true);
     stream.writeStartDocument();

     stream.writeStartElement("kpproject");
     stream.writeAttribute("points", QString::number(points.size()));
     //zastepujemy sciezke do projektu przez %project_path% jezeli
     //sciezka do pliku wideo jest w (pod)katalogu projektu
     stream.writeAttribute("sourcevideo", relativeVideoPath());


     for(int i=0;i<points.size();i++)
     {
	  stream.writeStartElement("kp");
	  stream.writeAttribute("index", QString::number(i));
	  stream.writeAttribute("name", points.at(i).name);
	  //stream.writeAttribute("frames", QString::number(points.at(i).framePos.size()));

	  for(int f=0;f<points.at(i).framePos.size();f++)
	  {
	       stream.writeStartElement("framepos");
	       stream.writeAttribute("index", QString::number(f));
	       stream.writeAttribute("x",
				     QString::number(points.at(i).framePos.at(f).x));
	       stream.writeAttribute("y",
				     QString::number(points.at(i).framePos.at(f).y));
	       stream.writeEndElement(); //framepos
	  }

	  for(int l=0;l<points.at(i).linkedPoints.size();l++)
	  {
	       stream.writeStartElement("link");
	       int index = getPointIndex(points.at(i).linkedPoints.at(l));
	       stream.writeAttribute("ref", QString::number(index));
	       stream.writeEndElement(); //link
	  }

	  stream.writeEndElement(); //kp
     }

     stream.writeEndElement(); //kpproject
     stream.writeEndDocument();
     //qDebug() << "xml: " << xcontent;
     return xcontent;
}

bool MainWindow::projectFromXml(const QString& xstr)
{
     //qDebug() << "\n\n parsing xml: \n " << xstr << "\n";
     points.clear();
     player->close();
     clearCache();
     QXmlStreamReader stream(xstr);
     
     int kpindex=0,frameindex=0;
     while (!stream.atEnd()) 
     {
	  stream.readNext();
	  if(stream.isStartElement())
	  {
	       //qDebug() << "START ELEMENT NAME: " << stream.name();
	       if(stream.name() == "kpproject")
	       {
		    QString srcpath = stream.attributes().value("sourcevideo").toString().
			 replace("%project_path%",projectDir());
		    if(!QFile::exists(srcpath) && !srcpath.isEmpty())
			 QMessageBox::warning(
			      this, tr("Warning"),tr("Source video file: ")+srcpath+" not found");
		    else
			 openVideo(srcpath);

		    int s = stream.attributes().value("points").toString().toInt();
		    //qDebug() << "points: " << s;
		    for(int i=0;i<s;i++)
                         points.append(::KeyPoint());
	       }
	       else if(stream.name() == "kp")
	       {
		    kpindex = stream.attributes().value("index").toString().toInt();
		    if(kpindex >= points.size())
		    {
			 QMessageBox::critical(
			      this, tr("Error"),tr("Key point index out of range: ")+
			      QString::number(kpindex));
			 return false;
		    }
		    points[kpindex].name = stream.attributes().value("name").toString();
		    // int fcount = stream.attributes().value("frames").toString().toInt();
		    // for(int i=0;i<fcount;i++)
		    // 	 points[kpindex].framePos.append(KeyPointPos())
			      ;
	       }
	       else if(stream.name() == "framepos")
	       {
		    frameindex = stream.attributes().value("index").toString().toInt();
		    while(points.at(kpindex).framePos.size() <= frameindex)
			 points[kpindex].framePos.append(KeyPointPos());
		    // if(frameindex >= points.at(kpindex).framePos.size())
		    // {
		    // 	 QMessageBox::critical(
		    // 	      this, tr("Error"),tr("Frame index out of range: ")+
		    // 	      QString::number(frameindex));
		    // 	 return false;
		    // }
		    points[kpindex].framePos[frameindex].x = 
			 stream.attributes().value("x").toString().toDouble();
		    points[kpindex].framePos[frameindex].y = 
			 stream.attributes().value("y").toString().toDouble();
	       }
	       else if(stream.name() == "link")
	       {
		    int ref = stream.attributes().value("ref").toString().toInt();
		    points[kpindex].linkedPoints.append(&points[ref]);	 
	       }
	  }
     }
     if (stream.hasError()) 
     {
	  QMessageBox::critical(this, tr("Error"),tr("The project cannot be loaded: ")+stream.errorString());
	  return false;
     }
     
     long maxframes = getMaxFrameCount();
     if(maxframes > player->countFrames())
	  player->setFakeFramesCount(maxframes-player->countRealFrames());

     refreshPointBox();
     redrawPlayerFrame(true);
     redrawPreview();
     return true;
}

QString MainWindow::projectDir()
{
     QString pdir;
#if defined(Q_WS_X11)
     QDir dir("/usr/share/kpedit/");
     if(dir.exists())
	  pdir = set->value("project_path","/usr/share/kpedit/").toString();
     else
#endif
     pdir = set->value("project_path").toString();
     int ls = pdir.lastIndexOf("/");
     return ls != -1 ? pdir.left(ls) : "";
}

QString MainWindow::relativeVideoPath()
{
     QString pdir = projectDir();
     return pdir.isEmpty() ? "" : QString(player->source()).replace(pdir,"%project_path%");
}

void MainWindow::saveProject(const QString& file)
{
     QFile f(file);
     if(f.open(QFile::Text | QFile::WriteOnly))
     {
	  set->setValue("project_path",file);
	  projectFile = file;
	  QTextStream str(&f);
	  str << projectToXml();
	  f.close(); 
     }
     else
	  QMessageBox::critical(this, tr("Error"),tr("Error writing file: ")+file);
}

bool MainWindow::loadProject(const QString& file)
{
     QFile f(file);
     if(f.open(QFile::Text | QFile::ReadOnly))
     {
	  set->setValue("project_path",file);
	  if(projectFromXml(f.readAll()))
	  {
	       projectFile = file;
	       refreshTable();
	       setChanged(false);
	       return true;
	  }
     }
     else
	  QMessageBox::critical(this, tr("Error"),tr("Error reading file: ")+file);
     return false;
}

void MainWindow::openProject()
{
     if(isChanged())
     {
	  switch(QMessageBox::question(
		  this, tr("Question"),tr("The project has changed - do you want to save the changes?"), tr("Yes"), tr("No"), tr("Cancel")))
	  {
	  case 0: saveProject(); if(isChanged()) return; break;
	  case 2: return; break;
	  }
	       
     }
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open project file"), projectDir(),
	  "KPEdit project (*.kpp);;All files (*)");
     if(!path.isEmpty())
	  if(!loadProject(path))
	       QMessageBox::critical(this, tr("Error"),tr("Project was not entirely loaded."));	  
}

void MainWindow::saveProject()
{
     QString path = projectFile.isEmpty() ? QFileDialog::getSaveFileName(
	  this, tr("Save project file"), projectDir(),
	  "KPEdit project (*.kpp);;All files (*.*)") : projectFile;
     if(!path.isEmpty())
     {
	  set->setValue("project_path",path);
	  saveProject(path);	  
	  setChanged(false);
     }
}

void MainWindow::saveProjectAs()
{
     QString path = QFileDialog::getSaveFileName(
	  this, tr("Save project file"), projectDir(),
	  "KPEdit project (*.kpp);;All files (*.*)");
     if(!path.isEmpty())
     {
	  set->setValue("project_path",path);
	  saveProject(path); 
	  setChanged(false);
     }
}

void MainWindow::newProject()
{
     if(isChanged())
	  switch(QMessageBox::question(
		      this, tr("Question"),tr("The project has changed - do you want to save the changes?"), tr("Yes"), tr("No"), tr("Cancel")))
	  {
	  case 0: saveProject(); if(isChanged()) return; break;
	  case 2: return; break;
	  }
     player->close();
     points.clear();
     projectFile.clear();
     refreshPointBox();
     refreshTable();
     player->showDefaultScreen();
     redrawPreview();
     clearCache();
     setChanged(false);
}

void MainWindow::openVideo()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open video file"), projectDir(),
	  "Video files (*.avi *.mp4 *.mpg *.ogv *.flv);;All files (*.*)");
     if(!path.isEmpty())
	  if(!openVideo(path))
	       QMessageBox::critical(this, tr("Error"),tr("The player failed to load the video file!"));
}

void MainWindow::closeVideo(bool quiet)
{
     if(points.size())
	  if(getMaxFrameCount() && player->countRealFrames())
	  {
	       // int a = quiet ? 1 : QMessageBox::question(
	       // 	    this, tr("Question"),
	       // 	    tr("You are trying to close the source video. Do you want to delete all frames based on this video?"),
	       // 	    tr("Yes"), tr("No"), tr("Cancel"));
	       // switch(a)
	       // {
	       // case 0:
	       // 	    //usuwamy pierwsze klatki
	       // 	    for(int i=0;i<points.size();i++)
	       // 		 for(long f=0;f<player->countRealFrames();f++)
	       // 		 {
	       // 		      if(!points.at(i).framePos.isEmpty())
	       // 			   points[i].framePos.removeFirst();
	       // 		      else
	       // 			   break;
	       // 		 }
	       // 	    refreshTable();
	       // 	    player->close();
	       // 	    clearCache();
	       // 	    setChanged(true);
	       // 	    break;
	       // case 1: //przerabiamy prawdziwe klatki na falszywe aby byl do nich dostep
	       // {
		    long fc = player->countFrames();
		    player->close();
		    player->setFakeFramesCount(fc);
		    refreshTable();
		    setChanged(true);
	       // 	    break;
	       // }
	       // case 2: return;
	       // }
	  }
     
}

void MainWindow::loadSettings()
{
     actionPointsOnVideo->setChecked(set->value("points_on_video",false).toBool());
     actionAllPoints->setChecked(set->value("all_points",true).toBool());
     actionPlayerAnti->setChecked(set->value("player_anti",true).toBool());
     player->enableAntialiasing(set->value("player_anti",true).toBool());
     actionPreviewAnti->setChecked(set->value("preview_anti",true).toBool());
     actionHideLinks->setChecked(set->value("hide_links",false).toBool());
     actionHighlightCurrent->setChecked(set->value("highlight_current",false).toBool());
     actionPrevFrameHint->setChecked(set->value("prev_frame_hint",false).toBool());
     actionOutline->setChecked(set->value("outline",false).toBool());

     renderSet.pointSize = set->value("point_size",0.018f).toFloat();
     renderSet.linkSize = set->value("link_size",0.4f).toFloat();
     renderSet.pointColor = QColor(set->value("point_color","#F0F0F0").toString());
     renderSet.highlightColor = QColor(set->value("highlight_color","#DE8800").toString());
     renderSet.linkColor = QColor(set->value("link_color","#F0F0F0").toString());
     renderSet.backgroundColor = QColor(set->value("background_color","#252525").toString());

     QSize s = set->value("mw_size",QSize(-1,-1)).toSize();
     if(s.isValid())
	  resize(s);
     QPoint pos = set->value("mw_pos",QPoint(-1,-1)).toPoint();
     if(pos != QPoint(-1,-1))
	  move(pos);
}

void MainWindow::pointsOnVideo(bool c)
{
     set->setValue("points_on_video",c);
     redrawPlayerFrame(true);
}

void MainWindow::allPoints(bool c)
{
     set->setValue("all_points",c);
     redrawPlayerFrame(true);
     redrawPreview();
}

void MainWindow::playerAnti(bool c)
{
     set->setValue("player_anti",c);
     player->enableAntialiasing(c);
     redrawPlayerFrame();
}

void MainWindow::previewAnti(bool c)
{
     set->setValue("preview_anti",c);
     redrawPreview();
}

void MainWindow::hideLinks(bool c)
{
     set->setValue("hide_links",c);
     redrawPlayerFrame(true);
     redrawPreview();
}

void MainWindow::prevFrameHint(bool c)
{
     set->setValue("prev_frame_hint",c);
     redrawPlayerFrame(true);
     redrawPreview();
}

void MainWindow::highlightCurrent(bool c)
{
     set->setValue("highlight_current",c);
     redrawPlayerFrame(true);
     redrawPreview();
}

void MainWindow::drawOutline(bool c)
{
     set->setValue("outline",c);
     redrawPlayerFrame(true);
     redrawPreview();
}

void MainWindow::exportPng()
{
     
     if(!player->countFrames() || !points.size())
     {
	  QMessageBox::critical(this, tr("Error"),tr("There is no frame or point to export!"));
	  return;
     }

     ExportSettings set;
     ExportDialog ed(&set,ExportDialog::ExportPNG);
     ed.exec();
     if(!set.ok)
	  return;

     player->stop();
     long frames = player->countFrames();
     int len = QString::number(frames-1).size();
     QProgressDialog pd(tr("Rendering frames..."), tr("Abort"), 0, frames , this);
     pd.setWindowModality(Qt::WindowModal);
     for(long f = 0; f < frames; f++)
     {
	  pd.setValue(f);
	  if(pd.wasCanceled())
	       break;
	  QPixmap pix(QSize(set.width,set.height));
	  pix.fill(renderSet.backgroundColor);
	  QImage frame = pix.toImage();
	  if(!actionHideLinks->isChecked() && actionAllPoints->isChecked())
	       drawConnections(&frame, set.antialiasing);
	  drawPoints(&frame, NULL, set.antialiasing);
	  
	  QString filename = QString::number(f);
	  int dif = len - filename.size();
	  for(int i=0;i<dif;i++)
	       filename = "0" + filename;
	  filename = set.fname + "/"+filename+".png";
	  frame.save(filename);

	  player->nextFrame();
     }
     pd.setValue(player->countFrames());
}

void MainWindow::exportAvi()
{
     
     if(!player->countFrames() || !points.size())
     {
	  QMessageBox::critical(this, tr("Error"),tr("There is no frame or point to export!"));
	  return;
     }

     ExportSettings set;
     ExportDialog ed(&set,ExportDialog::ExportAVI);
     ed.exec();
     if(!set.ok)
	  return;

     VideoWriter vw(set.fname.toStdString(),set.codec,set.fps,Size(set.width,set.height),true);
     player->stop();
     QProgressDialog pd(tr("Rendering frames..."), tr("Abort"), 0, player->countFrames(), this);
     pd.setWindowModality(Qt::WindowModal);
     for(long f = 0; f < player->countFrames(); f++)
     {
	  pd.setValue(f);
	  if(pd.wasCanceled())
	       break;
	  QPixmap pix(QSize(set.width,set.height));
	  bool extren = false;
	  if(renderer)
	       extren = renderer->drawBackground(&pix);
	  if(!extren)
	       pix.fill(renderSet.backgroundColor);
	  QImage frame = pix.toImage();
	  if(!actionHideLinks->isChecked() && actionAllPoints->isChecked())
	       drawConnections(&frame, set.antialiasing);
	  drawPoints(&frame, NULL, set.antialiasing);
	  
	  Mat m = qimage2mat(frame);
	  vw << m;
	  player->nextFrame();
     }
     pd.setValue(player->countFrames());
}

Mat MainWindow::qimage2mat(const QImage& qimage) 
{ 
     //qDebug() << "qimage2mat...";

     // videoloop = true;
     cv::Mat mat;
     // mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC3, (uchar*)qimage.bits(), qimage.bytesPerLine());

     // QImage img = qimage.rgbSwapped();
     mat = cv::Mat(qimage.height(), qimage.width(), 
		   CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine()); 
     
     cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 ); 
     int from_to[] = { 0,0,  1,1,  2,2 }; 
     cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 ); 
     //qDebug() << "qimage2mat - zwracam macierz...";
     return mat2; 
/*
  QImage qimg = qimage.rgbSwapped();
  IplImage *imgHeader = cvCreateImageHeader( Size(qimg.width(), qimg.height()), IPL_DEPTH_8U, 4);
  imgHeader->imageData = (char*) qimg.bits();

  uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg.byteCount());
  memcpy(newdata, qimg.bits(), qimg.byteCount());
  imgHeader->imageData = (char*) newdata;
  Mat m = imgHeader;
  return m;
*/
}; 

void MainWindow::exportCsv()
{
     QString path = QFileDialog::getSaveFileName(
	  this, tr("Save CSV file"), projectDir(),
	  "CSV file (*.csv)");
     if(!path.isEmpty())
     {
	  QFile f(path);
	  if(f.open(QFile::Text | QFile::WriteOnly))
	  {
	       QTextStream str(&f);
	       str << pointsToCsv();
	       f.close(); 
	  }
	  else
	       QMessageBox::critical(this, tr("Error"), tr("Error writing file: ")+path);
     }
}

void MainWindow::importCsv()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open CSV file"), projectDir(),
	  "CSV files (*.csv);;All files (*.*)");
     if(!path.isEmpty())
     {
	  QFile f(path);
	  if(f.open(QFile::Text | QFile::ReadOnly))
	       pointsFromCsv(f.readAll().replace("\"","")); //usuwamy " dodawane przez np excell
	  f.close();
     }
}

void MainWindow::drawPoints(QImage* dst, const ::KeyPoint* p, bool antialiasing)
{
     //qDebug() << "drawing points...";
     if(!dst)
	  return;
     if(dst->isNull())
	  return;

     if(renderer)
     {
	  if(renderer->drawPoints(dst,p,antialiasing))
	       return;
     }

     if(p) //tylko jeden punkt
     {
	  long cp = player->getCurrentPos();
	  if(cp >= p->framePos.size() || cp < 0)
	       return;
	  
	  int size = renderSet.pointSize*(dst->height()+dst->width()/2);
	  //qDebug() << "size: " << size << ", " << renderSet.pointSize << " * " << dst->height();

	  QPainter painter(dst);
	  QPen pen = painter.pen();
	  pen.setStyle(actionOutline->isChecked() ? Qt::SolidLine : Qt::NoPen);
	  painter.setPen(pen);
	  QBrush brush(
	       actionHighlightCurrent->isChecked() && p == &points.at(currentPointIndex()) ?
	       renderSet.highlightColor : renderSet.pointColor);
	  painter.setRenderHint(QPainter::Antialiasing,antialiasing);
	  painter.setBrush(brush);

	  if(actionPrevFrameHint->isChecked() && cp > 0 && 
	     player->getStatus() == KPPlayer::Stopped)
	  {
	       QImage hint;
	       hint = dst->copy();
	       QPainter hp(&hint);
	       hp.setPen(pen);
	       hp.setOpacity(0.4);
	       hp.setRenderHint(QPainter::Antialiasing,antialiasing);
	       hp.setBrush(brush);
	       
	       hp.drawEllipse(QPoint(p->framePos.at(cp-1).x*dst->width(),
	  			     p->framePos.at(cp-1).y*dst->height()),size,size);
	       painter.drawImage(0,0,hint);
	  }
	  
	  if(!p->framePos.at(cp).isSet())
	       return;

	  painter.drawEllipse(QPoint(p->framePos.at(cp).x*dst->width(),
				     p->framePos.at(cp).y*dst->height()),size,size);
     }
     else //wszystkie punkty, rekurencyjnie
	  for(int i=0;i<points.size();i++)
	       drawPoints(dst,&points.at(i),antialiasing);
}

void MainWindow::drawConnections(QImage* dst, bool antialiasing, bool allowHint)
{

     if(!dst)
	  return;
     if(dst->isNull())
	  return;

     if(renderer)
     {
	  if(renderer->drawConnections(dst,antialiasing,allowHint))
	       return;
     }
     
     if(actionPrevFrameHint->isChecked() && player->getCurrentPos() > 0
	&& player->getStatus() == KPPlayer::Stopped && allowHint)
     {
	  QImage hint;
	  hint = dst->copy();
	  drawConnections(&hint,antialiasing,false);
	  QPainter hp(dst);
	  hp.setOpacity(0.4);
	  hp.drawImage(0,0,hint);
     }

     
     long cp = player->getCurrentPos();
      //jesli nie pozwolilismy na rysowanie podpowiedzi w rekurencji,
      //to znaczy ze teraz rysujemy podpowiedz:
     if(!allowHint) 
	  cp--;
     if(cp<0) return;
     QPainter painter(dst);
     painter.setRenderHint(QPainter::Antialiasing,antialiasing);
     QPen pen = painter.pen();
     pen.setBrush(renderSet.linkColor);
     pen.setWidth(renderSet.pointSize*2*(dst->height()+dst->width()/2)*renderSet.linkSize);
     painter.setPen(pen);
     for(int i=0;i<points.size();i++)
     {
          const ::KeyPoint& p1 = points.at(i);
	  if(cp >= p1.framePos.size() || !p1.framePos.at(cp).isSet())
		    continue;
	  for(int l=0;l<p1.linkedPoints.size();l++)
	  {
               const ::KeyPoint& p2 = *p1.linkedPoints.at(l);
	       if(cp >= p2.framePos.size() || !p2.framePos.at(cp).isSet())
		    continue;
	       //qDebug() << "drawing link between " << p1.name << " and " << p2.name;
	       painter.drawLine(p1.framePos.at(cp).x*dst->width(),
				p1.framePos.at(cp).y*dst->height(),
				p2.framePos.at(cp).x*dst->width(),
				p2.framePos.at(cp).y*dst->height());
				
	  }
     }
}

void MainWindow::redrawPlayerFrame(bool reseek)
{
     if(player->onFakeFrame())
	  return;
     if(reseek)
	  player->seek(player->getCurrentPos());

     if(!points.size() || !actionPointsOnVideo->isChecked())
	  return;

     if(!actionHideLinks->isChecked() && actionAllPoints->isChecked())
	  drawConnections(player->getCurrentFrame(),actionPlayerAnti->isChecked());
     drawPoints(
	  player->getCurrentFrame(), actionAllPoints->isChecked() ?
	  NULL : &points.at(currentPointIndex()),
	  actionPlayerAnti->isChecked());  
}

void MainWindow::redrawPreview()
{
     QPixmap pix(previewFrame->size());
     bool extren = false;
     if(renderer)
	  extren = renderer->drawBackground(&pix);
     if(!extren)
	  pix.fill(renderSet.backgroundColor);
     QImage img = pix.toImage();
     if(points.size())
     {
	  if(!actionHideLinks->isChecked() && actionAllPoints->isChecked())
	       drawConnections(&img, actionPreviewAnti->isChecked());
	  drawPoints(
	       &img, actionAllPoints->isChecked() ?
	       NULL : &points.at(currentPointIndex()),
	       actionPreviewAnti->isChecked());
     }
     preview->setImage(img);
}


QString MainWindow::pointsToCsv()
{
     //format: point index,point name,frame index,x position,y position,linked index 1;linked index 2;linked index N
     QString csv = "point index,point name,frame index,x position,y position,linked points\n";
     for(int i=0;i<points.size();i++)
     {
	  QString linked = "";
	  for(int l=0;l<points.at(i).linkedPoints.size();l++)
	       linked += QString::number(getPointIndex(points.at(i).linkedPoints.at(l)))+
		    (l == points.at(i).linkedPoints.size()-1 ? "" : ";");
	  for(long f=0;f<points.at(i).framePos.size();f++)
	  {
	       QString line = QString::number(i) + ","+points.at(i).name+","+QString::number(f)+","+
		    QString::number(points.at(i).framePos.at(f).x)+","+
		    QString::number(points.at(i).framePos.at(f).y)+","+linked+"\n";
	       if(!line.toLower().contains("inf"))
		    csv += line;
	  }
     }
     return csv;
}

void MainWindow::pointsFromCsv(const QString& csv)
{
     if(csv.isEmpty())
	  return;

     bool append = false;
     if(points.size())
	  switch(QMessageBox::question(
		      this, tr("Question"),tr("This project already has some points. What do you want to do?"), tr("Replace"), tr("Append"), tr("Cancel")))
	  {
	  case 1: append = true; break;
	  case 2: return; break;
	  }

     QList<::KeyPoint> np; //nowe punkty
     if(!append)
	  points.clear();

     clearCache();
     QStringList lines = csv.split("\n",QString::SkipEmptyParts);

     QHash<int,QString> linkmap; //pary index punktu - indexy;polaczonych;punków
     long maxfindex = 0;
     for(int i=1;i<lines.size();i++) //pierwszy wiersz to nazwy pol
     {
	  
	  QStringList data = lines.at(i).split(",");
	  if(data.size() != 6)
	  {
	       QMessageBox::critical(this, tr("Error"),tr("CSV file format error!"));
	       refreshTable();
	       refreshPointBox();
	       return;
	  }
	  int index = data.at(0).toInt();

	  //nowy punkt jeśli nie istnieje
	  while(np.size()-1 < index) 
               np.append(::KeyPoint());

	  //punkt istnieje ale nie był uzupełniony
	  if(np.at(index).name.isEmpty())
	       np[index].name = data.at(1);

	  long findex = data.at(2).toUInt();
	  if(findex > maxfindex) maxfindex = findex;
	  while( np.at(index).framePos.size()-1 < findex || 
		 np.at(index).framePos.size()-1 < 0)
	       np[index].framePos.append(KeyPointPos());

	  np[index].framePos[findex].x = data.at(3).toDouble();
	  np[index].framePos[findex].y = data.at(4).toDouble();
	  
	  if(!linkmap.contains(index))
	       linkmap.insert(index,data.at(5));
     }

     //polaczenia
     int psize = points.size(); //rozmiar sprzed dołączenia importowanych punktów
     long fmax = getMaxFrameCount();
     qDebug() << "np size: " << np.size();
     qDebug() << "points size: " << points.size();

     //mapa wczytany_index,istniejacy_index dla przyszlego uzupelniania polaczen
     QHash<int,int> trmap; 


     for(int i=0;i<np.size();i++)
     {
          ::KeyPoint* p = getPointByName(np.at(i).name);
	  if(p)
	  {
	       long pfs = p->framePos.size();
	       p->framePos += np.at(i).framePos;
	       completeFrames(p,maxfindex+1+pfs, pfs);
	       trmap.insert(i,getPointIndex(p));
	  }
	  else
	  {
	       qDebug() << "dodaje nowy punkt (" << np.at(i).name << ") i uzupelniam klatki: " << psize;
	       points.append(np.at(i));
	       completeFrames(&points[points.size()-1],fmax+maxfindex+1,0);
	       
	       trmap.insert(i, points.size()-1);
	  }
	  
     }
     qDebug() << "points + np size: " << points.size();

     //polaczenia
     QList<int> keys = linkmap.uniqueKeys();
     for(int i=0;i<keys.size();i++)
     {
	  QStringList ls = linkmap.value(keys.at(i)).split(";",QString::SkipEmptyParts);
	  for(int l=0;l<ls.size();l++)
	  {
	       //kazde polaczenie z listy nowych punktow (np) trzeba przetlumaczyc na 
	       //polaczenia zgodne z tym co mamy w points
               ::KeyPoint* p = &points[trmap.value(keys.at(i))];
	       p->linkedPoints.append(
		    &points[trmap.value(ls.at(l).toInt())]);

	       //przy wielokrotnym imporcie polaczenia moga sie powielac, na razie proste usuwanie:
	       for(int j=0;j<p->linkedPoints.size()-1;j++)
	       	    for(int k=j+1;k<p->linkedPoints.size();k++)
	       		 if(p->linkedPoints.at(j) == p->linkedPoints.at(k))
	       		      p->linkedPoints.removeAt(k--);

	       //points[keys.at(i)+psize].linkedPoints.append(&points[ls.at(l).toInt()+psize]);
	  }
     }

     //usuwamy fałszywe punkty
     // for(int i=0;i<points.size();i++)
     // 	  if(points.at(i).name == "%fake%")
     // 	       points.removeAt(i--);


     completeFrames(NULL,0);
     completeFakeFrames();
     setChanged(true);
     refreshPointBox();
     refreshTable();
     redrawPlayerFrame(true);
     redrawPreview();
}



void MainWindow::addFrame()
{
     int pi = currentPointIndex();
     if(pi < 0) return;
     points[pi].framePos.append(KeyPointPos());
     cacheState(tr("add frame"),KP::Undo);
     if(points.at(pi).framePos.size() > player->countFrames())
	  player->addFakeFrame();
     completeFrames(NULL,points.at(pi).framePos.size());
     refreshTable();
     pointTable->selectRow(pointTable->rowCount()-1);
     setChanged(true);
}

void MainWindow::clearFrame()
{
     int pi = currentPointIndex();
     if(pi < 0 || !player->countFrames()) return;

     cacheState(tr("clear frame"),KP::Undo);
     long cp = player->getCurrentPos();
     points[pi].framePos[cp].x = points[pi].framePos[cp].y = -1;
     //clearCache();
     refreshTable();
     redrawPlayerFrame(true);
     redrawPreview();
     setChanged(true);
}

void MainWindow::deleteFrame()
{
     int pi = currentPointIndex();
     long cp = player->getCurrentPos();
     if(pi < 0 || !player->countFrames() || cp < 0) return;
     if(!player->onFakeFrame())
     {
	  QMessageBox::warning(this, tr("Warning"),tr("This frame is linked to a video frame and cannot be removed. Close the video if you want to manage all the frames."));
	  return;
     }
     // if(QMessageBox::question(
     // 		  this, tr("Question"),
     // 		  tr("This frame will be removed for every existing point. Do you really want to remove this frame?"), 
     // 		  tr("Yes"), tr("No")))
     // 	  return;
     
     cacheState(tr("delete frame"),KP::Undo);
     for(int i=0;i<points.size();i++)
	  if(points.at(i).framePos.size()>cp)
	       points[i].framePos.removeAt(cp);
     player->setFakeFramesCount(player->countFakeFrames()-1);
     refreshTable();
     redrawPlayerFrame(true);
     redrawPreview();
     setChanged(true);
     //clearCache();
}

void MainWindow::duplicateFrame()
{
     int pi = currentPointIndex();
     long cp = player->getCurrentPos();
     if(pi < 0 || !player->countFrames() || cp < 0) return;
     
     if(cp >= player->countRealFrames()-1) //ostatnia prawdziwa mozna zduplikowac jako falszywa
     {
	  qDebug() << "duplicating fake frame " << cp;
	  cacheState(tr("duplicate frame"),KP::Undo);
	  for(int i=0;i<points.size();i++)
	       if(points.at(i).framePos.size()>cp)
		    points[i].framePos.insert(cp+1,KeyPointPos(points.at(i).framePos.at(cp)));
	  player->addFakeFrame();
     }
     else
     {
	  qDebug() << "duplicating real frame " << cp;
	  //wiemy juz ze nie jest to ostatnia klatka, sprawdzamy czy nastepna jest ustawiona
	  if(points.at(pi).framePos.at(cp+1).isSet())
	       if(QMessageBox::question(
		  this, tr("Question"),
		  tr("The next frame is already set, do you want to replace the next frame?"),
		  tr("Yes"), tr("No")))
		    return;

	  cacheState(tr("duplicate frame"),KP::Undo);
	  for(int i=0;i<points.size();i++)
	       if(points.at(i).framePos.size()>cp+1)
		    points[i].framePos[cp+1] = points.at(i).framePos.at(cp);
     }
     refreshTable();
     if(pointTable->currentRow()+1 < pointTable->rowCount())
	  pointTable->selectRow(pointTable->currentRow()+1);
     setChanged(true);
}

void MainWindow::frameUp()
{
     int pi = currentPointIndex();
     long cp = player->getCurrentPos();
     if(pi < 0 || !player->countFrames() || cp <= 0) return;

     cacheState(tr("frame up"),KP::Undo);
     for(int i=0;i<points.size();i++)
	       if(points.at(i).framePos.size()>cp)
		    points[i].framePos.swap(cp,cp-1);
     setChanged(true);
     refreshTable();
     pointTable->selectRow(pointTable->currentRow()-1);
}

void MainWindow::frameDown()
{
     int pi = currentPointIndex();
     long cp = player->getCurrentPos();
     if(pi < 0 || !player->countFrames() || cp < 0 || cp >= player->countFrames()-1) 
	  return;

     cacheState(tr("frame down"),KP::Undo);
     for(int i=0;i<points.size();i++)
	       if(points.at(i).framePos.size()>cp+1)
		    points[i].framePos.swap(cp,cp+1);
     refreshTable();
     pointTable->selectRow(pointTable->currentRow()+1);
     setChanged(true);
}

void MainWindow::frameToBeginning()
{
     setChanged(true);
     int pi = currentPointIndex();
     long cp = player->getCurrentPos();
     if(pi < 0 || !player->countFrames() || cp <= 0)
	  return;

     cacheState(tr("frame to the beginning"),KP::Undo);
     for(int i=0;i<points.size();i++)
	       if(points.at(i).framePos.size()>cp)
	       {
		    KeyPointPos tmp = points.at(i).framePos.at(cp);
		    points[i].framePos.removeAt(cp);
		    points[i].framePos.insert(0,tmp);
	       }
     setChanged(true);
     refreshTable();
     pointTable->selectRow(0);
}

void MainWindow::frameToEnd()
{
     int pi = currentPointIndex();
     const long& cp = player->getCurrentPos();
     qDebug() << "cp: " << cp << ", frames: " << player->countFrames();
     if(pi < 0 || !player->countFrames() || cp < 0 || cp >= player->countFrames()-1) return;

     cacheState(tr("frame to the end"),KP::Undo);
     for(int i=0;i<points.size();i++)
	       if(points.at(i).framePos.size()>cp)
	       {
		    KeyPointPos tmp = points.at(i).framePos.at(cp);
		    points[i].framePos.removeAt(cp);
		    points[i].framePos.insert(points.at(i).framePos.size(),tmp);
	       }
     setChanged(true);
     refreshTable();
     pointTable->selectRow(pointTable->rowCount()-1);
}

int MainWindow::findClosestIndex(double x, double y)
{
     int ci = -1;
     float mindst = 1;
     const long& cp = player->getCurrentPos();
     if(cp < 0) return ci;
     for(int i=0;i<points.size();i++)
	  if(points.at(i).framePos.size() > cp)
	  {
	       const double& px = points.at(i).framePos.at(cp).x;
	       const double& py = points.at(i).framePos.at(cp).y;
	       float dst = sqrt((x-px)*(x-px)+(y-py)*(y-py));
	       if(dst < mindst)
	       {
		    ci = i;
		    mindst = dst;
	       }
	  }
     return ci;
}

void MainWindow::renamePoint()
{
     int ci = currentPointIndex();
     int cbi = pointBox->currentIndex();
     if(ci<0)
	  return;
     QString name = QInputDialog::getText(this, tr("Add point"), tr("Point name:"), 
					  QLineEdit::Normal, points.at(ci).name);
     if(!name.isEmpty() && validateName(name))
     {
	  cacheState(tr("rename point"),KP::Undo);
	  points[ci].name = name;
	  refreshPointBox();
	  pointBox->setCurrentIndex(cbi);
	  setChanged(true);
     }
     
}



bool MainWindow::validateName(const QString& name)
{
     if(name.isEmpty())
	  return false;
     if(name.contains(",") || name.contains(";") || name.contains("%"))
     {
	  QMessageBox::critical(this, tr("Error"), 
				tr("The name contains invalid characters ('%', ';' or ','). Please add a point with a different name."));
	  return false;
     }
     return true;
}

void MainWindow::renderSettings()
{
     RenderDialog rd(&renderSet);
     rd.exec();

     set->setValue("point_size",renderSet.pointSize);
     set->setValue("link_size",renderSet.linkSize);
     set->setValue("point_color",renderSet.pointColor.name());
     set->setValue("link_color",renderSet.linkColor.name());
     set->setValue("highlight_color",renderSet.highlightColor.name());
     set->setValue("background_color",renderSet.backgroundColor.name());

     redrawPlayerFrame(true);
     redrawPreview();
}

void MainWindow::cachePos()
{
     if(lastMove.frame == player->getCurrentPos())
	  uhist.append(lastMove.pos.isSet() ? lastMove : newHistoryItem());
     else
     {
	  if(lastMove.pos.isSet())
	       uhist.append(lastMove);
	  uhist.append(newHistoryItem());
     }
     actionUndo->setText(tr("Undo")+" "+uhist.last().command);
     actionUndo->setEnabled(true);
     actionRedo->setText(tr("Redo"));
     rhist.clear();
     actionRedo->setEnabled(false);
}

void MainWindow::cacheState(const QString& command, KP::CommandType type)
{
     float psize = pointsSize(points);
     while(cacheSize()+psize > CACHE_SIZE && uhist.size())
	  uhist.removeFirst();
     while(cacheSize()+psize > CACHE_SIZE && rhist.size()) //jezeli dalej nie ma miejsca
	  rhist.removeFirst();
     
     if(cacheSize()+psize > CACHE_SIZE)
     {
	  if(!cacheMessage){
	       QMessageBox::warning(this, tr("Warning"), tr("The application cache is out of space. This operation cannot be undone."));
	       cacheMessage = true;
	  }
	  clearCache();
	  
	  return;
     }

     QList<HistoryItem>& hist = (type == KP::Undo ? uhist : rhist);
     HistoryItem item;
     item.command = command;
     item.frame = player->getCurrentPos();
     item.hasCache = true;
     for(int i=0;i<points.size();i++)
	  item.cache.append(points[i]); //deep copy
     //item.cache = QList<KeyPoint>(points);
     hist.append(item);
     if(type == KP::Undo)
     {
	  actionUndo->setEnabled(true);
	  actionUndo->setText(tr("Undo")+" "+command);
     }
     else
     {
	  actionRedo->setEnabled(true);
	  actionRedo->setText(tr("Redo")+" "+command);
     }
}

HistoryItem MainWindow::newHistoryItem()
{
     HistoryItem item;
     item.point = &points[currentPointIndex()];
     item.frame = player->getCurrentPos();
     item.pos = item.point->framePos.at(item.frame);
     item.command = tr("point move");
     item.hasCache = false;
     return item;
}

void MainWindow::undo()
{
     if(uhist.size()<2)
     {
     	  actionUndo->setText(tr("Undo"));
     	  actionUndo->setEnabled(false);
     }
     if(!uhist.size())
	  return;

     HistoryItem item = uhist.takeLast();

     actionRedo->setEnabled(true);
     actionRedo->setText(tr("Redo")+" "+item.command);
     if(uhist.size())
	  actionUndo->setText(tr("Undo")+" "+uhist.last().command);

     if(item.hasCache)
     {
	  cacheState(item.command, KP::Redo);
	  points.clear();
	  for(int i=0;i<item.cache.size();i++)
	       points.append(item.cache[i]); //deep copy
	  //points = item.cache;
	  completeFakeFrames(true);
	  refreshPointBox();
	  refreshTable();
	  qDebug() << "selecting row: " << item.frame;
	  if(item.frame < pointTable->rowCount())
	       pointTable->selectRow(item.frame);
	  else
	  {
	       redrawPreview();
	       redrawPlayerFrame(true);
	  }
	  return;
     }

     item.point->framePos[item.frame] = item.pos;
     updateTable(item.frame, item.pos.x, item.pos.y);

     int pi = getPointIndex(item.point);
     if(pi != currentPointIndex())
	  pointBox->setCurrentIndex(pointBox->findData(QVariant(pi)));
     if(item.frame != player->getCurrentPos())
	  player->seek(item.frame);
     else
     {
	  redrawPlayerFrame(true);
	  redrawPreview();
     }
     rhist.append(lastMove);
     lastMove = item;
}

void MainWindow::redo()
{
     if(rhist.size()<2)
     {
	  actionRedo->setText(tr("Redo"));
	  actionRedo->setEnabled(false);
     }
     if(!rhist.size())
	  return;
     HistoryItem item = rhist.takeLast();

     actionUndo->setEnabled(true);
     actionUndo->setText(tr("Undo")+" "+item.command);

     if(rhist.size())
	  actionRedo->setText(tr("Redo")+" "+rhist.last().command);

     if(item.cache.size())
     {
	  cacheState(item.command, KP::Undo);
	  points.clear();
	  for(int i=0;i<item.cache.size();i++)
	       points.append(item.cache[i]); //deep copy
	  //points = item.cache;
	  completeFakeFrames(true);
	  refreshPointBox();
	  refreshTable();
	  if(item.frame < pointTable->rowCount())
	       pointTable->selectRow(item.frame);
	  else
	  {
	       redrawPreview();
	       redrawPlayerFrame(true);
	  }
	  return;
     }

     item.point->framePos[item.frame] = item.pos;
     updateTable(item.frame, item.pos.x, item.pos.y);
     int pi = getPointIndex(item.point);
     if(pi != currentPointIndex())
	  pointBox->setCurrentIndex(pointBox->findData(QVariant(pi)));
     if(item.frame != player->getCurrentPos())
	  player->seek(item.frame);
     else
     {
	  redrawPlayerFrame(true);
	  redrawPreview();
     }
     uhist.append(lastMove);
     lastMove = item;
     
}

void MainWindow::setChanged(bool c)
{
     actionSave->setEnabled(c);
     changed = c;
     setWindowTitle(projectFile.isEmpty() ? tr("KPEdit - 2D skeletal animation editor") : (projectFile + (c ? tr(" [modified] ") : "") + " - KPEdit"));
}

void MainWindow::saveSplitter(QSplitter* sp, const QString& name)
{
     QList<int> ss = sp->sizes();
     QString ss_str = "";
     for(int i=0;i<ss.size();i++) 
	  ss_str+=QString::number(ss.at(i))+";";
     set->setValue(name,ss_str);
}

void MainWindow::restoreSplitter(QSplitter* sp, const QString& name, const QString& def)
{
     QStringList s = set->value(name, def).toString().split(";",QString::SkipEmptyParts);
     QList<int> sizes;
     for(int i=0;i<s.size();i++)
	  sizes.append(s.at(i).toInt());
     sp->setSizes(sizes);
}

void MainWindow::saveSplitters()
{
     saveSplitter(splitter,"splitter");
     saveSplitter(dockSplitter,"docksplitter");
}

void MainWindow::restoreSplitters()
{
     restoreSplitter(splitter,"splitter",
		     QString::number((int)(width()*0.6))+";"+
		     QString::number((int)(width()*0.4)));
     restoreSplitter(dockSplitter,"docksplitter",
		     QString::number((int)(height()*0.6))+";"+
		     QString::number((int)(height()*0.4)));
}

float MainWindow::cacheSize()
{
     float cs = 0;
     for(int i=0;i<uhist.size();i++)
	  if(uhist.at(i).hasCache)
	       cs+=pointsSize(uhist.at(i).cache);
     for(int i=0;i<rhist.size();i++)
	  if(rhist.at(i).hasCache)
	       cs+=pointsSize(rhist.at(i).cache);
     return cs;
}

float MainWindow::pointsSize(const QList<::KeyPoint>& list)
{
     return list.size() ? 
          (float)(sizeof(list.size())*sizeof(::KeyPoint)*
	  list.at(0).framePos.size()*sizeof(KeyPointPos))/
	  1024/1024 : 0; //10% dodane ze względu nieujete dane
}
