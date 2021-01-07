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

#include "renderdialog.h"

RenderDialog::RenderDialog(RenderSettings* rs)
{
     setupUi(this);
     set = rs;
     pwidthBox->setValue(set->pointSize);
     lwidthBox->setValue(set->linkSize*100);
     pcolorEdit->setText(set->pointColor.name().toUpper());
     lcolorEdit->setText(set->linkColor.name().toUpper());
     hcolorEdit->setText(set->highlightColor.name().toUpper());
     bcolorEdit->setText(set->backgroundColor.name().toUpper());
     
     connect(pcolorButton, SIGNAL(clicked()), this, SLOT(pcolorClicked()));
     connect(lcolorButton, SIGNAL(clicked()), this, SLOT(lcolorClicked()));
     connect(hcolorButton, SIGNAL(clicked()), this, SLOT(hcolorClicked()));
     connect(bcolorButton, SIGNAL(clicked()), this, SLOT(bcolorClicked()));
     connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

RenderDialog::~RenderDialog()
{

}

void RenderDialog::pcolorClicked()
{
     QColor c = QColorDialog::getColor(QColor(pcolorEdit->text()), this, tr("Select point color"));
     if(c.isValid())
	  pcolorEdit->setText(c.name().toUpper());
}

void RenderDialog::lcolorClicked()
{
     QColor c = QColorDialog::getColor(QColor(lcolorEdit->text()), this, tr("Select link color"));
     if(c.isValid())
	  lcolorEdit->setText(c.name().toUpper());
}

void RenderDialog::hcolorClicked()
{
     QColor c = QColorDialog::getColor(QColor(hcolorEdit->text()), this, tr("Select highlighted point color"));
     if(c.isValid())
	  hcolorEdit->setText(c.name().toUpper());
}

void RenderDialog::saveClicked()
{
     QColor tmp(pcolorEdit->text());
     if(!tmp.isValid())
     {
	  QMessageBox::critical(this, tr("Error"),tr("The point color is invalid!"));
	  return;
     }
     set->pointColor = tmp;
     tmp = QColor(lcolorEdit->text());
     if(!tmp.isValid())
     {
	  QMessageBox::critical(this, tr("Error"),tr("The link color is invalid!"));
	  return;
     }
     set->linkColor = tmp;
     tmp = QColor(hcolorEdit->text());
     if(!tmp.isValid())
     {
	  QMessageBox::critical(this, tr("Error"),tr("The highlight color is invalid!"));
	  return;
     }
     set->highlightColor = tmp;
     tmp = QColor(bcolorEdit->text());
     if(!tmp.isValid())
     {
	  QMessageBox::critical(this, tr("Error"),tr("The background color is invalid!"));
	  return;
     }
     set->backgroundColor = tmp;
     
     set->pointSize = pwidthBox->value();
     set->linkSize = (float)lwidthBox->value()/100;
     close();
}

void RenderDialog::bcolorClicked()
{
     QColor c = QColorDialog::getColor(QColor(bcolorEdit->text()), this, tr("Select background color"));
     if(c.isValid())
	  bcolorEdit->setText(c.name().toUpper());
}
