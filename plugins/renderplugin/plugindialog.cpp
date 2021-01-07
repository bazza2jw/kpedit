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

PluginDialog::PluginDialog(KPInterface* kp, QSettings* s)
{
     setupUi(this);
     kpedit = kp;
     set = s;

     rbBox->setChecked(set->value("render_background",false).toBool());
     rpBox->setChecked(set->value("render_points",false).toBool());
     rlBox->setChecked(set->value("render_links",false).toBool());

     rbEdit->setText(set->value("background_texture","").toString());
     rpEdit->setText(set->value("point_texture","").toString());
     rlEdit->setText(set->value("link_texture","").toString());

     connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(rbButton, SIGNAL(clicked()), this, SLOT(rbSelect()));
     connect(rpButton, SIGNAL(clicked()), this, SLOT(rpSelect()));
     connect(rlButton, SIGNAL(clicked()), this, SLOT(rlSelect()));
}

PluginDialog::~PluginDialog()
{

}

void PluginDialog::okClicked()
{
     if(rbBox->isChecked() && (rbEdit->text().isEmpty() || !QFile::exists(rbEdit->text())))
     {
	  QMessageBox::warning(this, tr("Warning"), tr("The background file path is invalid!"));
	  return;
     }
     if(rpBox->isChecked() && (rpEdit->text().isEmpty() || !QFile::exists(rpEdit->text())))
     {
	  QMessageBox::warning(this, tr("Warning"), tr("The point texture file path is invalid!"));
	  return;
     }
     if(rlBox->isChecked() && (rlEdit->text().isEmpty() || !QFile::exists(rlEdit->text())))
     {
	  QMessageBox::warning(this, tr("Warning"), tr("The link texture file path is invalid!"));
	  return;
     }
     set->setValue("render_background",rbBox->isChecked());
     set->setValue("render_points",rpBox->isChecked());
     set->setValue("render_links",rlBox->isChecked());
     set->setValue("background_texture",rbEdit->text());
     set->setValue("point_texture",rpEdit->text());
     set->setValue("link_texture",rlEdit->text());
     close();
}

void PluginDialog::rbSelect()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open background image"), rbEdit->text(),
	  "Image files (*.png *.jpg *.jpeg *.bmp *.tiff *.xpm)");
     if(!path.isEmpty())
	  rbEdit->setText(path);
}

void PluginDialog::rpSelect()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open texture file"), rpEdit->text(),
	  "Image files (*.png *.jpg *.jpeg *.bmp *.tiff *.xpm)");
     if(!path.isEmpty())
	  rpEdit->setText(path);
}

void PluginDialog::rlSelect()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open texture file"), rlEdit->text(),
	  "Image files (*.png *.jpg *.jpeg *.bmp *.tiff *.xpm)");
     if(!path.isEmpty())
	  rlEdit->setText(path);
}
