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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <QDebug>


class PreviewWidget : public QWidget
{
     Q_OBJECT;
public:
     PreviewWidget(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~PreviewWidget();
     void setImage(const QImage& img, const QSize& size = QSize(-1,-1));
     const QImage* getImage() const {return &image;}
     const QPoint* getCursorPos() const {return underMouse() ? &cpos : NULL;}
     double normalizedX() const
     {return image.isNull() ? -1 : (double)cpos.x()/(double)image.width();}
     double normalizedY() const
     {return image.isNull() ? -1 : (double)cpos.y()/(double)image.height();}
     void enableAntialiasing(bool enable) {smooth = enable;}
     void clear();
private:
     void paintEvent(QPaintEvent*);
     void mouseMoveEvent(QMouseEvent*);
     void mouseReleaseEvent(QMouseEvent*) {emit clicked(normalizedX(),normalizedY());}
     QSize imsize;
     QImage image;
     QPoint cpos;
     bool smooth;
     

     private slots:
	  
signals:
     void clicked(double x, double y);

};

#endif


