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

     kpedit->cacheState(tr("set boundaries"),KP::Undo);
     QList<KeyPoint>* points = kpedit->getPoints();
     
     for(int i=0;i<points->size();i++)
     {
	  QList<KeyPointPos>& p = (*kpedit->getPoints())[i].framePos;
	  
	  for(long f=fromBox->value();f<=toBox->value();f++)
	  {
	       if(!p[f].isSet() || f >= p.size())
		    continue;
	       
	       //lewo
	       if(p[f].x < leftBox->value())
		    translate(f,leftBox->value()-p[f].x,0);
	       //prawo
	       if(p[f].x > rightBox->value())
		    translate(f,rightBox->value()-p[f].x,0);
	       //dol
	       if(p[f].y > bottomBox->value())
		    translate(f,0,bottomBox->value()-p[f].y);
	       //gora
	       if(p[f].y < topBox->value())
		    translate(f,0,topBox->value()-p[f].y);
	  }
	  
     }
     kpedit->refreshTable();
     kpedit->redrawPlayerFrame(true);
     kpedit->redrawPreview();
     close();

}

void PluginDialog::translate(long frame, double x, double y)
{
     for( int i=0;i<kpedit->getPoints()->size();i++)
     {
	  QList<KeyPointPos>& p = (*kpedit->getPoints())[i].framePos;
     
	  if(!p[frame].isSet() || frame >= p.size())
	       continue;
	  bool changed = false;
	  p[frame].x += x;
	  p[frame].y += y;
	  if(changed && (p[frame].x == -1 || p[frame].y == -1))
	       p[frame].x += 0.000001; //aby isSet() nie zwrocilo false
     }
}
