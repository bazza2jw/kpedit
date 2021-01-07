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

#include "previewwidget.h"

PreviewWidget::PreviewWidget(QWidget* parent, Qt::WindowFlags f):
     QWidget(parent,f),smooth(false),imsize(-1,-1)
{
     setFixedSize(480,360);
     show();
     setMouseTracking(true);

     //wlasny kursor
     QPixmap cr(7,7);
     cr.fill(Qt::transparent);
     QPainter p(&cr);
     p.setRenderHint(QPainter::Antialiasing, true);
     p.setBrush(QColor("yellow"));
     p.drawEllipse(QPoint(4,4),3,3);
     setCursor(QCursor(cr));
}

PreviewWidget::~PreviewWidget()
{
     qDebug() << "preview destroyed...";
}

void PreviewWidget::setImage(const QImage& img, const QSize& size)
{
     if(img.isNull())
	  return;
     imsize = size; 
     image = img;
     if(!imsize.isValid())
	  setFixedSize(img.size());
     else
     {
	  image = image.scaled(
	       imsize, Qt::IgnoreAspectRatio, (
		    smooth ? Qt::SmoothTransformation : Qt::FastTransformation));
	  setFixedSize(imsize);
     }
     repaint();
}

void PreviewWidget::paintEvent(QPaintEvent*)
{
     if(image.isNull())
	  return;
     QPainter p(this);
     p.drawImage(QRect(0,0,width(),height()),image);
}


void PreviewWidget::mouseMoveEvent(QMouseEvent* e)
{
     cpos.setX(e->x());
     cpos.setY(e->y());
     //qDebug() << "mouse x,y: " << cpos;
}

void PreviewWidget::clear()
{
     QPixmap p(QSize(1,1));
     p.fill(Qt::transparent);
     setImage(p.toImage());
}
