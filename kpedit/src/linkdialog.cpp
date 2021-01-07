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

#include "linkdialog.h"

LinkDialog::LinkDialog(KPInterface* kp):changed(false)
{
     setupUi(this);
     pointTree->hideColumn(1);
 
     kpedit = kp;
     points = kpedit->getPoints();
    
     connect(linkButton, SIGNAL(clicked()), this, SLOT(linkClicked()));
     connect(unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkClicked()));
     connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(pointTree, SIGNAL(itemSelectionChanged()), this, SLOT(validateLink()));
     connect(pointList, SIGNAL(itemSelectionChanged()), this, SLOT(validateLink()));

     refreshTree();
     refreshList();
}

LinkDialog::~LinkDialog()
{

}



void LinkDialog::refreshTree()
{
     pointTree->clear();
     for(int i=0;i<points->size();i++)
     {
	  QTreeWidgetItem* top = new QTreeWidgetItem();
	  top->setText(0,points->at(i).name);
	  top->setText(1,QString::number(i));
	  for(int j=0;j<points->at(i).linkedPoints.size();j++)
	  {
	       QTreeWidgetItem* child = new QTreeWidgetItem();
	       child->setText(0,points->at(i).linkedPoints.at(j)->name);
	       child->setText(1,QString::number(j));
	       top->addChild(child);
	  }
	  pointTree->addTopLevelItem(top);
	  pointTree->expandItem(top);
     }

     pointTree->setCurrentItem(pointTree->itemAt(0,0));
}

void LinkDialog::refreshList()
{
     pointList->clear();
     for(int i=0;i<points->size();i++)
	  pointList->addItem(points->at(i).name);
     pointList->setCurrentRow(0);
}

void LinkDialog::linkClicked()
{
     //liczymy ze validateLink dopuscil do kliniecia
     //--------------------------------------------
     changed = true;
     KeyPoint* p1 = &(*points)[pointTree->currentItem()->text(1).toInt()];
     KeyPoint* p2 = &(*points)[pointList->currentRow()];
     qDebug() << "linking " << p1->name << " point to " << p2->name << " point";

     //czy sam ze soba
     if(p1 == p2)
     {
	  QMessageBox::warning(this, tr("Warning"), 
			       tr("The point cannot be linked to itself!"));
	  return;
     }
     
     //czy takie polaczenie istnieje
     for(int i=0;i<p1->linkedPoints.size();i++)
	  if(p1->linkedPoints.at(i) == p2)
	  {
	       QMessageBox::warning(this, tr("Warning"), 
				    tr("Those points are already linked that way!"));
	       return;
	  }
     p1->linkedPoints.append(p2);
     refreshTree();
}

void LinkDialog::unlinkClicked()
{
     changed = true;
     KeyPoint* top = &(*points)[pointTree->currentItem()->parent()->text(1).toInt()];
     top->linkedPoints.removeAt(pointTree->currentItem()->text(1).toInt());
     refreshTree();
}

void LinkDialog::validateLink()
{
     if(pointTree->currentItem())
     {
	  linkButton->setEnabled(!pointTree->currentItem()->parent());
	  unlinkButton->setEnabled(pointTree->currentItem()->parent());
     }
}


void LinkDialog::closeEvent(QCloseEvent* e)
{
     if(changed)
     {
	  //kpedit->cacheState(tr("link points"),KP::Undo);
	  kpedit->clearCache(); //do poki nie ma glebokiej kopii calosci, bezpiczeniej wyczyscic
	  kpedit->redrawPlayerFrame(true);
	  kpedit->redrawPreview();
     }
     e->accept();
}
