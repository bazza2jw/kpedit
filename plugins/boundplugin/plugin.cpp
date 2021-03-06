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

Plugin::Plugin()
{
    
}

Plugin::~Plugin()
{
     
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
     QAction* a = new QAction(tr("Set boundaries..."),this);
     connect(a, SIGNAL(triggered()), this, SLOT(activate()));
     return a;
}

void Plugin::activate()
{
     if(kpedit->countPlayerFrames() < 1)
     {
	  QMessageBox::information(NULL,tr("Information"),tr("There are no frames to process."));
	  return;
     }
     PluginDialog pd(kpedit);
     pd.exec();
}




