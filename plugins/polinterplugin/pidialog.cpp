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

#include "pidialog.h"

PIDialog::PIDialog(KPInterface* kp)
{
     setupUi(this);
     kpedit = kp;
     toBox->setMaximum(kpedit->countPlayerFrames()-1);
     toBox->setValue(toBox->maximum());
     fromBox->setMaximum(toBox->maximum());
     fromBox->setValue(0);

     QList<KeyPointPos> points;
     //points << KeyPointPos(0.06,0.19) << KeyPointPos(0.23,0.67) << KeyPointPos(0.41,0.3) << KeyPointPos(0.55, 0.19) << KeyPointPos(0.67, 0.54) << KeyPointPos(0.89, 0.24);

     points << KeyPointPos(0.05,0.43) << KeyPointPos(0.25,0.19) << KeyPointPos(0.3,0.66) << KeyPointPos(0.45, 0.85) << KeyPointPos(0.52, 0.55) << KeyPointPos(0.72, 0.12) << KeyPointPos(0.91, 0.62) << KeyPointPos(0.68, 0.9) << KeyPointPos(0.78, 0.41);

     // for(int i=0;i<10;i++)
     // 	  points << KeyPointPos((double)(rand()%100)/100,(double)(rand()%100)/100);

     setNodes(points);
     //nodes << KeyPointPos(1,1) << KeyPointPos(3,3) << KeyPointPos(2,1);

     //connect(refreshButton, SIGNAL(clicked()), this, SLOT(refresh()));
     connect(stepBox, SIGNAL(valueChanged(int)), this, SLOT(refresh()));
     connect(degBox, SIGNAL(valueChanged(int)), this, SLOT(refresh()));
     connect(okButton, SIGNAL(clicked()), this, SLOT(interpolate()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
       
}

PIDialog::~PIDialog()
{

}

void PIDialog::refresh(bool lastonly)
{
     if(lastonly)
     {
	  previewLabel->setPixmap(last.scaled(previewLabel->size(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	  return;
     }
     QPixmap pix(previewLabel->size());
     pix.fill(QColor("white"));
     //QList<KeyPointPos> pt = Polinter::interpolate(nodes.mid(0,3),degBox->value(),stepBox->value());
     //pt += Polinter::interpolate(nodes.mid(2,2),degBox->value(),stepBox->value());
     QList<KeyPointPos> pt = Polinter::interpolate(nodes,degBox->value(),stepBox->value());
     //QList<KeyPointPos> pt = Polinter::interpolate(nodes,stepBox->value());
     for(int i=0;i<pt.size();i++)
     	  drawDot(&pix,pt.at(i).x,pt.at(i).y,1);

     for(int i=0;i<nodes.size();i++)
     	  drawDot(&pix,nodes.at(i).x,nodes.at(i).y,5,QColor("red"));
     last = pix;
     previewLabel->setPixmap(pix.scaled(previewLabel->size(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void PIDialog::drawDot(QPixmap* pix, double nx, double ny, int r, QColor c)
{
     QPainter p(pix);
     p.setRenderHint(QPainter::Antialiasing, true);
     p.setBrush(c);
     p.drawEllipse(QPoint(nx*pix->width(),ny*pix->height()), r, r);
}

void PIDialog::setNodes(QList<KeyPointPos> n)
{
     nodes = n;
     degBox->setMaximum(nodes.size()-1);
}

bool PIDialog::areFramesSet()
{
     QList<KeyPoint>* points = kpedit->getPoints();
     for(int i=0;i<points->size();i++)
	  for(long f=fromBox->value(); f <= toBox->value(); f++)
	       if(!points->at(i).framePos.at(f).isSet())
		    return false;
     return true;
}

void PIDialog::interpolate()
{
     if(fromBox->value() > toBox->value())
     {
	  QMessageBox::critical(NULL,tr("Error"), tr("The range is invalid!"));
	  return;
     }

     if(!areFramesSet())
     {
	  QMessageBox::critical(NULL, tr("Error"), 
				   tr("There are empty frames in given range. All the frames need to be set before interpolation!"));
	  return;
     }
     
     long fcount = kpedit->countPlayerFrames();
     if(degBox->value() >= fcount-(fcount-toBox->value())-fromBox->value()+1)
     {
	  QMessageBox::critical(NULL, tr("Error"), 
				tr("There is not enough frames! Chosen frame range is too narrow or degree of polynomial is too high."));
	  return;
     }
     
     // if(QMessageBox::question(
     // 	     NULL,tr("Are you sure?"),
     // 	     tr("This operation cannot be undone. Would you like to proceed?"), 
     // 	     tr("Yes"), tr("Cancel")))
     // 	  return;
     

     kpedit->cacheState(tr("interpolation"),KP::Undo);
     QList<KeyPoint>* points = kpedit->getPoints();
     long fmax = 0;
     QProgressDialog pd(tr("Interpolating points coordinates..."), 
			tr("Abort"), 0, points->size() , this);
     pd.setWindowModality(Qt::WindowModal);
     for(int i=0;i<points->size();i++)
     {
	  pd.setValue(i);
	  QList<KeyPointPos>& p = (*points)[i].framePos;
	  long midlen = p.size()-(p.size()-toBox->value())-fromBox->value()+1;
	  
	  QList<KeyPointPos> newpos = Polinter::interpolate( 
	       p.mid(fromBox->value(),  midlen),
		     degBox->value(), stepBox->value());

	  Polinter::print("koncowka", p.mid(toBox->value()+1));
	  p = p.mid(0,fromBox->value())+newpos+p.mid(toBox->value()+1);
	  if(p.size() > fmax)
	       fmax = p.size();
     }
     pd.setValue(points->size());
     kpedit->closeVideo(true);
     kpedit->setPlayerFakeFrames(fmax);
     kpedit->refreshTable();
     kpedit->redrawPlayerFrame(true);
     kpedit->redrawPreview();
     close();
}

