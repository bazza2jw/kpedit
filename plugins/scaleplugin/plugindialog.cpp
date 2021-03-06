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

#include "plugindialog.h"

PluginDialog::PluginDialog(KPInterface* kp)
{
     setupUi(this);
     kpedit = kp;

     long frames = kpedit->countPlayerFrames()-1;
     toBox->setMaximum(frames);
     toBox->setValue(frames);
     fromBox->setMaximum(frames);

     pointBox->addItem(tr("All points"),QVariant(-1));

     QList<KeyPoint>* points = kpedit->getPoints();
     for(int i=0;i<points->size();i++)
	  pointBox->addItem(points->at(i).name,QVariant(i));

     connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

PluginDialog::~PluginDialog()
{

}



void PluginDialog::okClicked()
{
     if(fromBox->value() > toBox->value())
     {
	  QMessageBox::critical(NULL,tr("Error"), tr("The range is invalid!"));
	  return;
     }
     
     kpedit->cacheState(tr("scale points"),KP::Undo);
     QList<KeyPoint>* points = kpedit->getPoints();
     int start = pointBox->itemData(pointBox->currentIndex()).toInt();
     int stop = start;
     if(start == -1)
     {
	  start = 0;
	  stop = points->size()-1;
     }

     for(int i=start;i<=stop;i++)
     {
	 
	  QList<KeyPointPos>& p = (*points)[i].framePos;
	  if(p.size() <= toBox->value())
	       continue;

	   qDebug() << "scaling point " << points->at(i).name;
	  for(long f=fromBox->value();f<=toBox->value();f++)
	  {
	       if(!p[f].isSet())
		    continue;
	       p[f].x *= xBox->value();
	       p[f].y *= yBox->value();
	       
	       if(p[f].x == -1 || p[f].y == -1)
		    p[f].x += 0.000001; //aby isSet() nie zwrocilo false
	  }
     }

     kpedit->refreshTable();
     kpedit->redrawPlayerFrame(true);
     kpedit->redrawPreview();
     close();
}
