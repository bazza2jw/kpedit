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

#include "kpplayer.h"

KPPlayer::KPPlayer(QWidget* parent):QWidget(parent),realFrames(0),originalSize(Size(0,0)),
				    currentPos(-1),timerId(-1), status(Stopped),statusbar(NULL),
				    raw(false), fakeFrames(0)
{
     setupUi(this);
     pw = new PreviewWidget(frame);
     gapBox->setFocusPolicy(Qt::ClickFocus);
     defaultImg = QImage(":/res/playerdefault2.png");
     connect(playButton, SIGNAL(clicked()), this, SLOT(play()));
     connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
     connect(pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
     connect(nextButton, SIGNAL(clicked()), this, SLOT(nextFrame()));
     connect(prevButton, SIGNAL(clicked()), this, SLOT(prevFrame()));
     connect(recButton, SIGNAL(clicked()), this, SLOT(record()));
     connect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
     connect(gapBox, SIGNAL(valueChanged(int)), this, SLOT(gapChanged(int)));
     connect(pw, SIGNAL(clicked(double,double)), this, SLOT(pwClicked(double,double)));
}

KPPlayer::~KPPlayer()
{
     //delete pw;
}

bool KPPlayer::openSource(const QString& src)
{
     if(vcap.isOpened())
	  vcap.release();
     vcap.open(src.toStdString());
     originalSize.width = vcap.get(CAP_PROP_FRAME_WIDTH);
     originalSize.height = vcap.get(CAP_PROP_FRAME_HEIGHT);
     killTimer(timerId);
     timerId = -1;
     currentPos = -1;
     if(!vcap.isOpened())
     {
	  qDebug() << "KPPlayer::openSource cannot load video: " << src;
	  close();
	  return false;
     }

     qDebug() << "KPPlayer::openSource new video loaded: " << src;
#if defined(Q_WS_X11)
     QString codec = codecName();
     raw = (codec == "MJPG" || codec == "I420" || codec == "YUV4");
     qDebug() << "codec name: " << codecName();
#endif

     filepath = src;
     realFrames = vcap.get(CAP_PROP_FRAME_COUNT);
     qDebug() << "total frames: " << realFrames;

#if defined(Q_WS_X11)
     if(realFrames > FRAME_LIMIT && !raw)
     {
	  QMessageBox::critical(this, tr("Error"),tr("This file is coded with \"")+codec+tr("\" codec and contains more than ")+QString::number(FRAME_LIMIT)+tr(" frames. This codec is not supported for full-length videos yet, please cut your video or use other codec like motion JPEG or YUV4."));
	  close();
	  return false;
     }
#endif
     gapBox->setValue(estimateGap());
     fpsLabel->setText("("+QString::number(1000/gapBox->value())+" FPS)");
     slider->setMaximum(countFrames()-1);
     //updateSlider(0);
     slider->setValue(0);
     return true;
}

void KPPlayer::close()
{
     stop();
     vcap.release();
     //fakeFrames += realFrames;
     fakeFrames = 0;
     realFrames = 0;
     filepath.clear();
     originalSize = Size(0,0);
     slider->setValue(0);
     showDefaultScreen();
     updateStatus(status);
}

void KPPlayer::nextFrame()
{
     //qDebug() << "KPPlayer::nextFrame(): realFrames: " << realFrames << ", fakeFrames: " << fakeFrames << ", currentPos: " << currentPos;
     if(!vcap.isOpened() && !fakeFrames)
	  return;

     if(vcap.isOpened() && currentPos+1 <= realFrames-1)
     {
	  //qDebug() << "next REAL frame...";
	  Mat m;
	  //vcap.grab();
	  //vcap.retrieve(m);
	  vcap >> m;
	  //currentPos = vcap.get(CV_CAP_PROP_POS_FRAMES); //videocapture zlicza roznie przy roznych kodekach
	  currentPos++;
	  if(currentPos>=realFrames)
	       currentPos = realFrames-1;
	  //qDebug() << "current pos: " << currentPos << "/" << realFrames;
	  //cvtColor(m,m,CV_BGR2RGB);
	  currentFrame = QImage((uchar*)m.data, m.cols, m.rows, m.step, 
				QImage::Format_RGB888).rgbSwapped();
     }
     else if(currentPos+1 <= countFrames()-1 && currentPos+1 >= realFrames)
     {
	  //qDebug() << "next FAKE frame...";
	  showDefaultScreen();
	  currentPos++;
     }
     else 
	  return;

     emit frameChanged(currentPos);
     pw->setImage(currentFrame,frame->size());
     updateSlider(currentPos);
     
     if(status == Recording)
	  updateStatus(Recording);
     else
	  updateStatus(timerId == -1 ? Stopped : Playing);
}

void KPPlayer::play()
{
     if(!countFrames())
	  return;

     if(currentPos == countFrames()-1)
	  seek(0);

     if(timerId==-1)
	  timerId = startTimer(gapBox->value());
     updateStatus(Playing);
}

void KPPlayer::stop()
{
     pause();
     seek(0);
     //nextFrame(); //tymczasowo podwójnie aby zaktualizować podgląd
     //seek(0);
}

void KPPlayer::pause()
{
     killTimer(timerId);
     timerId = -1;
     updateStatus(Stopped);
}

void KPPlayer::record()
{
     if(!vcap.isOpened())
	  return;

     if(currentPos == realFrames-1)
	  seek(0);

     if(timerId==-1)
     {
	  preCounter = PRE_RECORD_TIME;
	  timerId = startTimer(1000);
	  updateStatus(PreRec);
     }
     
}

void KPPlayer::setPos(long pos)
{
     if(!vcap.isOpened())
	  return;

     //------ tymczasowy sposob ze wzgledu na bug gubienia klatek w opencv
      // vcap.release();
      // vcap.open(filepath.toStdString());
      //--------------------------------------

     if(pos < 0) pos = 0;
     if(pos >= realFrames) pos = realFrames-1;
     currentPos = pos;
     vcap.set(CAP_PROP_POS_FRAMES, currentPos);
     currentPos--;
     nextFrame();
}

#if defined(Q_WS_X11)
void KPPlayer::setPosMPEG(long pos)
{
      if(!vcap.isOpened())
	  return;

      //------ tymczasowy sposob ze wzgledu na bug gubienia klatek w opencv
      vcap.release();
      vcap.open(filepath.toStdString());
      //--------------------------------------
      
     if(pos < 0) pos = 0;
     if(pos >= realFrames) pos = realFrames-1;
     currentPos = pos;
     //vcap.set(CV_CAP_PROP_POS_FRAMES, 0);
     while(pos-- > 0)
      	  vcap.grab();
     currentPos--; //aktualizujemy klatkę ale nie pozycję
     nextFrame();
}
#endif

void KPPlayer::seek(int pos)
{
     //qDebug() << "seeking pos: " << pos;
     if(pos < realFrames)
     {
#if defined(Q_WS_X11)
	  if(!raw) 
	       setPosMPEG(pos);
	  else 
#endif
	       setPos(pos);
     }
     else if(pos >= realFrames && pos < countFrames())
     {
	  currentPos = pos-1;
	  nextFrame();
     }
}

void KPPlayer::timerEvent(QTimerEvent*)
{

     if(status == PreRec)
     {
	  preCounter--;
	  if(preCounter == 0)
	  {
	       killTimer(timerId);
	       timerId = startTimer(gapBox->value());
	       updateStatus(Recording);
	       return;
	  }
	  else
	  {
	       updateStatus(PreRec);
	       return;
	  }
     }

     //qDebug() << "tick";
     nextFrame();
     if(currentPos >= countFrames()-1)
	  stop();
}

void KPPlayer::gapChanged(int gap)
{
     fpsLabel->setText("("+QString::number(1000/gapBox->value())+" FPS)");
}

void KPPlayer::updateSlider(int pos)
{
     if(pos < 0) pos = 0;
     disconnect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
     slider->setValue(pos);
     connect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
}

void KPPlayer::updateStatus(Status s)
{
     QString info = "";
     status = s;
     switch(status)
     {
     case Stopped: info = "Stopped"; break;
     case Playing: info = "Playing"; break;
     case Recording: 
	  info = "RECORDING (press escape to pause)";
	  //const QPoint* cp = pw->getCursorPos();
	  if(pw->underMouse())
	       info += ", x: "+QString::number(pw->normalizedX())+
					       ", y: "+QString::number(pw->normalizedY());
	  break;
     case PreRec: info = "Recording will start in: " + QString::number(preCounter) + " sec.";
     }

     playButton->setEnabled(status == Stopped);
     recButton->setEnabled(status == Stopped);
     prevButton->setEnabled(status == Stopped);
     nextButton->setEnabled(status == Stopped);
     slider->setEnabled(status == Stopped);

     long total = countFrames();
     if(total)
	  info += ", frame: " + QString::number(currentPos)+"/"+QString::number(total-1) +
	       " ("+QString::number(total)+" in total)";
     else
	  info += ", no frames";
     slider->setMaximum(total == 0 ? 0 : total-1);
     if(statusbar)
	  statusbar->showMessage(info);
     else
	  statusLabel->setText(info);
     emit statusUpdated(s);
}


void KPPlayer::resizeEvent(QResizeEvent*)
{
     //qDebug() << "player resize event";
     pw->setImage(currentFrame,frame->size());
}

void KPPlayer::setStatusBar(QStatusBar* sb)
{
     statusbar = sb;
     statusLabel->setVisible(sb==NULL);
}

QString KPPlayer::codecName()
{
     if(!vcap.isOpened())
	  return "";
     int ex = static_cast<int>(vcap.get(CAP_PROP_FOURCC));
     char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
     return QString(EXT);
}
