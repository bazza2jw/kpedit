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
     QAction* a = new QAction(tr("Export to JSON..."),this);
     connect(a, SIGNAL(triggered()), this, SLOT(activate()));
     return a;
}

void Plugin::activate()
{
     // qDebug() << "json:\n" << pointsToJSON();
     // return;

     QSettings set("KPEdit","KPEdit");
     QString path = QFileDialog::getSaveFileName(
     	  NULL, tr("Save JSON file"),
     	  set.value("project_path",qApp->applicationDirPath()).toString(),
     	  "JSON file (*.json)");
     if(!path.isEmpty())
     {
     	  QFile f(path);
     	  if(f.open(QFile::Text | QFile::WriteOnly))
     	  {
     	       QTextStream str(&f);
     	       str << pointsToJSON();
     	       f.close(); 
     	  }	  
     }
}


QString Plugin::pointsToJSON()
{
// template:
//  {
//  "points":
//     [
//         {
//             "index": 0,
//             "name": "nazwa",
//             "frames":
//             [
//                 {"index":0, "x":0.00, "y":0.00},
//                 {"index":1, "x":0.01, "y":0.40},
//                 {"index":2, "x":0.00, "y":0.00}
//             ]
// 	},
// 	{
// 	  "index": 1,
// 	  "name": "nazwa2",
// 	  "frames":
// 	  [
// 	  {"index":0, "x":0.00, "y":0.00},
// 	  {"index":1, "x":0.01, "y":0.40},
// 	  {"index":2, "x":0.00, "y":0.00}
// 	  ]
// 	}
//     ],

    
//     "links":
//     [
// 	 {
// 	      "point": 0,
// 		   "link":
// 		   [
// 			{"with":1},
// 			{"with":3}
// 		   ]
// 	 },
// 	 {
// 	      "point": 1,
// 		   "link":
// 		   [
// 			{"with":0}
// 		   ]
// 	 }
//     ]
// }
     
     
	  
     QString jp = "\t\"points\":\n\t[\n",jl = "\n\n\t\"links\":\n\t[\n";
     QList<KeyPoint>* points = kpedit->getPoints();

     int lastLinked = -1;
     for(int i=points->size()-1;i>=0;i--)
	  if(points->at(i).linkedPoints.size())
	  {
	       lastLinked = i;
	       break;
	  }

     for(int i=0;i<points->size();i++)
     {
	  jp += "\t\t{\n\t\t\t\"index\": " + QString::number(i) + 
	       ",\n\t\t\t\"name\": \""+points->at(i).name + 
	       "\",\n\t\t\t\"frames\": \n\t\t\t[\n";

	  const QList<KeyPointPos>& p = points->at(i).framePos;
	  for(long f=0;f<p.size();f++)
	  {
	       
	       jp += "\t\t\t\t{\"index\": "+QString::number(f)+
		    ", \"x\": "+QString::number(p.at(f).x)+
		    ", \"y\": "+QString::number(p.at(f).y)+(f==p.size()-1 ? "}\n" : "},\n");
	  }
	  jp += "\t\t\t]\n\t\t";
	  jp += (i==points->size()-1) ? "}\n" : "},\n";

	  if(!points->at(i).linkedPoints.size())
	       continue;

	  jl += "\t\t{\n\t\t\t\"point\": "+QString::number(i) + 
	       ",\n\t\t\t\"link\":\n\t\t\t[\n";
	  for(int l=0;l<points->at(i).linkedPoints.size();l++)
	  {
	       jl += "\t\t\t\t{\"with\": "+QString::number(
		    kpedit->getPointIndex(points->at(i).linkedPoints.at(l)))+
		    (l == points->at(i).linkedPoints.size()-1 ? "}\n" : "},\n");
	  }
	  jl += "\t\t\t]\n\t\t";
	  jl += (i==lastLinked) ? "}\n" : "},\n";
     }
     jp += "\t],\n";
     jl += "\t]\n";
     return "{\n"+jp+jl+"}\n";
	       
}





