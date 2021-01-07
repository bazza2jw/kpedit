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

#ifndef KPPLAYER_H
#define KPPLAYER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QWidget>
#include <QImage>
#include <QStatusBar>
#include <QPixmap>
#include <QMessageBox>
#include "previewwidget.h"
#include "ui_kpplayer.h"

#define PRE_RECORD_TIME 2
#define FRAME_LIMIT 1800

using namespace cv;

class KPPlayer : public QWidget, public Ui::KPPlayerForm
{
     Q_OBJECT;

public:
     enum Status{Stopped,Playing,Recording,PreRec};
     
     KPPlayer(QWidget* parent);
     virtual ~KPPlayer();

     bool openSource(const QString& src);

     long countFrames() const {return realFrames+fakeFrames;}
     const long& countRealFrames() const {return realFrames;}
     const long& countFakeFrames() const {return fakeFrames;}
     const long& getCurrentPos() const {return currentPos;}
     void setFakeFramesCount(long ffc){fakeFrames = ffc;updateStatus(status);}
     void addFakeFrame() {fakeFrames++;updateStatus(status);}
     bool onFakeFrame() const {return currentPos >= realFrames && currentPos < countFrames();}
     QImage* getCurrentFrame() {return &currentFrame;}
     int estimateGap()
     {return vcap.isOpened() ? round(1000.0/vcap.get(CAP_PROP_FPS)) : 0;}
     void setStatusBar(QStatusBar* sb);

     /**
      * Ustawia wskaźnik odczytu na podanej klatce.
      * Metoda ta używa właściwości CV_CAP_PROP_POS_FRAMES, która nie
      * działa właściwie przy większości kodeków, wspierane kodeki
      * wg nazw fourcc to:
      * I420 (raw video)
      * MJPG (motion jpeg)
      * YUV4
      * 
      * @param pos Numer nowej bieżącej klatki
      */
     void setPos(long pos);

#if defined(Q_WS_X11)
     /**
      * Ustawia wskaźnk odczytu na podanej klatce.
      * Metoda jest używana dla kodeków analizujących poprzednie
      * klatki, przez co wymga cofnięcia wskaźnika. Ze względu
      * na czas potrzebny na ponowne wczytanie klatek, FRAME_LIMIT
      * wyznacza maksymalną liczbę klatek filmu dla tych kodeków.
      * 
      * @param pos Numer nowej bieżącej klatki
      */
     void setPosMPEG(long pos);
#endif

     Status getStatus() const {return status;}
     const PreviewWidget* previewWidget() const {return pw;}
     const QString& source() const {return filepath;}
     void setDefaultImage(const QImage& img) 
     {defaultImg = img.copy();}
     void enableAntialiasing(bool enable) 
     {pw->enableAntialiasing(enable);pw->setImage(currentFrame,frame->size());}
     QString codecName();
     void showDefaultScreen()
     {currentFrame = defaultImg;pw->setImage(defaultImg.scaled(frame->size()));}

private:
     VideoCapture vcap;
     QString filepath;
     Status status;
     Size originalSize;
     QImage currentFrame;
     QImage defaultImg;
     PreviewWidget* pw;
     long realFrames;
     long fakeFrames;
     long currentPos;
     int timerId;
     int preCounter;
     QStatusBar* statusbar;
     bool raw;
     void timerEvent(QTimerEvent*);
     void updateSlider(int pos);
     void updateStatus(Status s);
     void resizeEvent(QResizeEvent*); 

public slots:
     void nextFrame();
     void prevFrame() {seek(currentPos-1);}
     void play();
     void stop();
     void pause();
     void record();
     void seek(int pos);
     void gapChanged(int gap);
     void close();
     void pwClicked(double x, double y)
     {emit previewClicked(x,y);}

signals:
     void frameChanged(long);
     void previewClicked(double x, double y);
     void statusUpdated(KPPlayer::Status status);
};

#endif
