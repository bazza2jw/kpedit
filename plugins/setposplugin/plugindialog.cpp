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

     fromBox->setMinimum(0);
     fromBox->setMaximum(kpedit->countPlayerFrames()-1);
     toBox->setMinimum(0);
     toBox->setMaximum(kpedit->countPlayerFrames()-1);
     fromBox->setValue(kpedit->currentPlayerPos());
     toBox->setValue(kpedit->currentPlayerPos());

     pointBox->addItem(tr("All points"),QVariant(-1));

     QList<KeyPoint>* points = kpedit->getPoints();
     for(int i=0;i<points->size();i++)
	  pointBox->addItem(points->at(i).name,QVariant(i));

     int cp = kpedit->currentPointIndex();
     pointBox->setCurrentIndex(pointBox->findData(cp));
     
     KeyPointPos p = points->at(cp).framePos.at(kpedit->currentPlayerPos());
     if(p.isSet())
     {
	  xBox->setValue(p.x);
	  yBox->setValue(p.y);
     }
     

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

     kpedit->cacheState(tr("set position"),KP::Undo);
     QList<KeyPoint>* points = kpedit->getPoints();
     if(pointBox->currentIndex() == 0)
     {
	  for(int i=0;i<points->size();i++)
	       for(long f=fromBox->value();f<=toBox->value();f++)
		    if(points->at(i).framePos.size() > f)
		    {
			 (*points)[i].framePos[f].x = xBox->value();
			 (*points)[i].framePos[f].y = yBox->value();
		    }
     }
     else
     {
	  KeyPoint* p = &(*points)[pointBox->itemData(pointBox->currentIndex()).toInt()];
	  for(long f=fromBox->value();f<=toBox->value();f++)
	       if(p->framePos.size() > f)
	       {
		    p->framePos[f].x = xBox->value();
		    p->framePos[f].y = yBox->value();
	       }
     }

     kpedit->refreshTable();
     kpedit->redrawPlayerFrame(true);
     kpedit->redrawPreview();
     close();
}
