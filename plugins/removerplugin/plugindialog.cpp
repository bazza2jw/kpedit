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

     fromBox->setMinimum(kpedit->countPlayerRealFrames());
     fromBox->setMaximum(kpedit->countPlayerFrames()-1);
     toBox->setMinimum(kpedit->countPlayerRealFrames());
     toBox->setMaximum(kpedit->countPlayerFrames()-1);
     toBox->setValue(toBox->maximum());

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

     kpedit->cacheState(tr("remove frames"),KP::Undo);
     QList<KeyPoint>* points = kpedit->getPoints();
     long fmax = 0;
     for(int i=0;i<points->size();i++)
     {
	  QList<KeyPointPos>& p = (*points)[i].framePos;
	  if(toBox->value() < p.size())
	  {
	       long del = 0;
	       long index = fromBox->value();
	       while(index <= toBox->value())
	       {
		    if(index%everyBox->value() == 0)
		    {
			 p.removeAt(index-del);
			 del++;
		    }
		    index++;
	       }
	       if(p.size() > fmax)
		    fmax = p.size();
	  }
     }
     kpedit->setPlayerFakeFrames(fmax-kpedit->countPlayerRealFrames());
     kpedit->refreshTable();
     kpedit->redrawPlayerFrame(true);
     kpedit->redrawPreview();
     close();
}
