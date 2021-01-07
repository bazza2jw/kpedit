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

#include "polinterplugin.h"

PolinterPlugin::PolinterPlugin()
{

}

PolinterPlugin::~PolinterPlugin()
{

}

void PolinterPlugin::onLoad(KPInterface* kp)
{
     kpedit = kp;
}

void PolinterPlugin::onUnload()
{

}

QAction* PolinterPlugin::createAction()
{
     QAction* a = new QAction(tr("Interpolation..."),this);
     connect(a, SIGNAL(triggered()), this, SLOT(activate()));
     return a;
}

void PolinterPlugin::activate()
{
     if(kpedit->countPlayerFrames() < 2 || !kpedit->getPoints()->size())
     {
	  QMessageBox::information(NULL, tr("Warning"), 
				   tr("There is not enough frames to interpolate!"));
	  return;
     }
     PIDialog pid(kpedit);
     pid.exec();
}


