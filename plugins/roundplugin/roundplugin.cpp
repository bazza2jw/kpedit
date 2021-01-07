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

#include "roundplugin.h"

RoundPlugin::RoundPlugin()
{
    
}

RoundPlugin::~RoundPlugin()
{
     
}

void RoundPlugin::onLoad(KPInterface* kp)
{
     kpedit = kp;
}

void RoundPlugin::onUnload()
{

}

QAction* RoundPlugin::createAction()
{
     QAction* a = new QAction(tr("Round positions..."),this);
     connect(a, SIGNAL(triggered()), this, SLOT(activate()));
     return a;
}

void RoundPlugin::activate()
{

     long frames = kpedit->countPlayerFrames()-1;
     if(frames < 0)
     {
	  QMessageBox::information(NULL,tr("Information"),tr("There are no frames to process."));
	  return;
     }
     RoundSet rs;
     RoundDialog rd(frames,&rs);
     rd.exec();
     if(rs.ok)
     {
	  kpedit->cacheState(tr("round positions"),KP::Undo);
	  QList<KeyPoint>* points = kpedit->getPoints();
	  for(int i=0;i<points->size();i++)
	  {
	       if(rs.startFrame > points->at(i).framePos.size())
		    continue;
	       long stop = rs.endFrame;
	       if(stop >= points->at(i).framePos.size())
		    stop = points->at(i).framePos.size()-1;
	       for(long f=rs.startFrame;f<=stop;f++)
		    switch(rs.operation)
		    {
		    case OP_ROUND:
			 (*points)[i].framePos[f].x = round((*points)[i].framePos[f].x, rs.dec);
			 (*points)[i].framePos[f].y = round((*points)[i].framePos[f].y, rs.dec);
			 break;
		    case OP_FLOOR:
			 break; 
		    case OP_CEIL:
			 break;
		    }
	  }
	  kpedit->refreshTable();
	  kpedit->redrawPlayerFrame(true);
	  kpedit->redrawPreview();
     }
}



