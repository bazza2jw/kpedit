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

#include "plugin.h"
#include "math.h"
Plugin::Plugin()
{
     kpset = new QSettings("KPEdit","KPEdit");
     set = new QSettings("KPEdit", "RenderPlugin");
     reloadTextures();
}

Plugin::~Plugin()
{
     delete kpset;
     delete set;
}

void Plugin::onLoad(KPInterface* kp)
{
     kpedit = kp;
}

void Plugin::onUnload()
{

}

QAction* Plugin::createAction()
{
     QAction* a = new QAction(tr("Advanced rendering..."),this);
     connect(a, SIGNAL(triggered()), this, SLOT(activate()));
     return a;
}

void Plugin::activate()
{
     PluginDialog pd(kpedit,set);
     pd.exec();
     reloadTextures();
     kpedit->redrawPlayerFrame(true);
     kpedit->redrawPreview();
}

bool Plugin::drawBackground(QPixmap* bkg)
{
     if(background.isNull() || !set->value("render_background",false).toBool())
	  return false;
     *bkg = QPixmap(background.scaled(bkg->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
     return true;
}


bool Plugin::drawConnections(QImage* dst, bool antialiasing, bool allowHint)
{

     if(link.isNull() || !set->value("render_links",false).toBool())
	  return false;

     long cp = kpedit->currentPlayerPos();

     if(kpset->value("prev_frame_hint",false).toBool() && cp > 0
	/*&& player->getStatus() == KPPlayer::Stopped*/ && allowHint)
     {
	  QImage hint;
	  hint = dst->copy();
	  drawConnections(&hint,antialiasing,false);
	  QPainter hp(dst);
	  hp.setOpacity(0.4);
	  hp.drawImage(0,0,hint);
     }

     const QList<KeyPoint>& points = *kpedit->getPoints();
      //jesli nie pozwolilismy na rysowanie podpowiedzi w rekurencji,
      //to znaczy ze teraz rysujemy podpowiedz:
     if(!allowHint) 
	  cp--;
     if(cp<0) return false;
     QPainter painter(dst);
     painter.setRenderHint(QPainter::Antialiasing,antialiasing);
     painter.setRenderHint(QPainter::SmoothPixmapTransform,true);
     QPen pen = painter.pen();
     pen.setBrush(QColor("red"));
     float pointsize = kpset->value("point_size",0.018f).toFloat();
     float linksize = kpset->value("link_size",0.4f).toFloat();
     int width = pointsize*2*(dst->height()+dst->width()/2)*linksize;
     //pen.setWidth(pointsize*2*(dst->height()+dst->width()/2)*linksize);
     painter.setPen(pen);
     for(int i=0;i<points.size();i++)
     {
	  const KeyPoint& p1 = points.at(i);
	  if(cp >= p1.framePos.size() || !p1.framePos.at(cp).isSet())
		    continue;
	  for(int l=0;l<p1.linkedPoints.size();l++)
	  {
	       const KeyPoint& p2 = *p1.linkedPoints.at(l);
	       if(cp >= p2.framePos.size() || !p2.framePos.at(cp).isSet())
		    continue;
	       //qDebug() << "drawing link between " << p1.name << " and " << p2.name;
	       drawLink(&painter,p1.framePos.at(cp).x*dst->width(),
			p1.framePos.at(cp).y*dst->height(),
			p2.framePos.at(cp).x*dst->width(),
			p2.framePos.at(cp).y*dst->height(),width);
	       // painter.drawLine(p1.framePos.at(cp).x*dst->width(),
	       // 			p1.framePos.at(cp).y*dst->height(),
	       // 			p2.framePos.at(cp).x*dst->width(),
	       // 			p2.framePos.at(cp).y*dst->height());
				
	  }
     }
     return true;
}



bool Plugin::drawPoints(QImage* dst, const KeyPoint* p, bool antialiasing)
{
     if(point.isNull() || !set->value("render_points",false).toBool())
	  return false;

     const QList<KeyPoint>& points = *kpedit->getPoints();
     if(p) //tylko jeden punkt
     {
     	  long cp = kpedit->currentPlayerPos();
     	  if(cp >= p->framePos.size() || cp < 0)
     	       return false;
	
	  float pointsize = kpset->value("point_size",0.018f).toFloat();
     	  int size = pointsize*(dst->height()+dst->width()/2);
     	  //qDebug() << "size: " << size << ", " << renderSet.pointSize << " * " << dst->height();

     	  QPainter painter(dst);
     	  painter.setRenderHint(QPainter::Antialiasing,antialiasing);
	  painter.setRenderHint(QPainter::SmoothPixmapTransform,true);

     	  if(kpset->value("prev_frame_hint",false).toBool() && cp > 0 
	     /*&& player->getStatus() == KPPlayer::Stopped*/)
     	  {
     	       QImage hint;
     	       hint = dst->copy();
     	       QPainter hp(&hint);
     	       hp.setOpacity(0.4);
     	       hp.setRenderHint(QPainter::Antialiasing,antialiasing);
	       hp.setRenderHint(QPainter::SmoothPixmapTransform,true);
	       
	       drawPoint(&hp,p->framePos.at(cp-1).x*dst->width(),
			 p->framePos.at(cp-1).y*dst->height(),size,size,
			 kpset->value("highlight_current",false).toBool() && p == &points.at(kpedit->currentPointIndex()));
     	       // hp.drawEllipse(QPoint(p->framePos.at(cp-1).x*dst->width(),
     	       // 			     p->framePos.at(cp-1).y*dst->height()),size,size);
     	       painter.drawImage(0,0,hint);
     	  }
	  
     	  if(!p->framePos.at(cp).isSet())
     	       return false;

	  drawPoint(&painter, p->framePos.at(cp).x*dst->width(),
		    p->framePos.at(cp).y*dst->height(),size,size,
		    kpset->value("highlight_current",false).toBool() && p == &points.at(kpedit->currentPointIndex()));
     	  //painter.drawEllipse(QPoint(p->framePos.at(cp).x*dst->width(),
	  //p->framePos.at(cp).y*dst->height()),size,size);
     }
     else //wszystkie punkty, rekurencyjnie
     	  for(int i=0;i<points.size();i++)
     	       drawPoints(dst,&points.at(i),antialiasing);

     return true;
}

void Plugin::reloadTextures()
{
     background.load(set->value("background_texture").toString());
     point.load(set->value("point_texture").toString());
     link.load(set->value("link_texture").toString());
}

void Plugin::drawPoint(QPainter* painter, int x, int y, int rw, int rh, bool hl)
{
     if(hl)
     {
	  QImage p = point.scaled(rw*2,rh*2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	  QPixmap pix(p.size());
	  pix.fill(Qt::transparent);
	  QImage hlp = pix.toImage();
	  
	  QColor c = QColor(kpset->value("highlight_color","#DE8800").toString());
	  for(int i=0;i<p.width();i++)
	       for(int j=0;j<p.height();j++)
	       {
		    int alpha = QColor::fromRgba(p.pixel(i,j)).alpha();
		    if(alpha > 128)
			 hlp.setPixel(i,j,c.rgb());
	       }

	  painter->drawImage(x-rw, y-rh, hlp);
     }
     else
	  painter->drawImage(x-rw, y-rh, point.scaled(rw*2,rh*2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
}

void Plugin::drawLink(QPainter* painter, int x1, int y1, int x2, int y2, int width)
{
     float vx = x2-x1;
     float vy = y2-y1;
     float d = sqrt(vx*vx + vy*vy);
     //qDebug() << "dlugosc polaczenia: " << d;

     qreal angle = atan2(vy,vx)*(180/M_PI);
     //qDebug() << "kat obrotu polaczenia: " << angle;
 
     painter->translate(x1,y1);
     painter->rotate(angle);
     QImage img = link.scaled(d,width,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	  //.transformed(tr);

     int psize = (kpset->value("point_size",0.018f).toFloat())*
	  (painter->device()->height()+painter->device()->width()/2);
     painter->drawImage(0,-1*psize/2,img);
     painter->resetTransform();
}




