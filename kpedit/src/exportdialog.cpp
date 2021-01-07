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

#include "exportdialog.h"

ExportDialog::ExportDialog(ExportSettings* es, ExportMode m)
{
     setupUi(this);
     set = es;
     mode = m;

     fpsLabel->setVisible(mode == ExportAVI);
     fpsBox->setVisible(mode == ExportAVI);
     codecBox->setVisible(mode == ExportAVI);
     codecLabel->setVisible(mode == ExportAVI);
     
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(chooseButton, SIGNAL(clicked()), this, SLOT(chooseClicked()));
     connect(exportButton, SIGNAL(clicked()), this, SLOT(exportClicked()));
     typeLabel->setText(mode == ExportAVI ? tr("File:") : tr("Directory:"));
     lineEdit->setText(QDir::home().absolutePath() + (mode == ExportAVI ? "/kpedit_video.avi" : ""));
     exportButton->setFocus();
}

ExportDialog::~ExportDialog()
{

}

void ExportDialog::exportClicked()
{
     if(lineEdit->text().isEmpty())
     {
	  QMessageBox::critical(this, tr("Error"),tr("The path cannot be empty!"));
	  return;
     }
     QDir d = (mode == ExportAVI) ? QFileInfo(lineEdit->text()).absoluteDir() : lineEdit->text();
     if(!d.exists())
     {
	  QMessageBox::critical(this, tr("Error"),tr("Chosen directory does not exist."));
	  return;
     }
     set->fps = fpsBox->value();
     set->width = widthBox->value();
     set->height = heightBox->value();
     set->fname = lineEdit->text();
     set->antialiasing = antiBox->isChecked();
     set->codec = codecBox->currentIndex() ? cv::VideoWriter::fourcc('X', 'V', 'I', 'D') :
          cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
     set->ok = true;
     close();
}

void ExportDialog::chooseClicked()
{
     if(mode == ExportAVI)
     {
	  QString path = QFileDialog::getSaveFileName(
	       this, tr("Save video file"), lineEdit->text(),"AVI video file (*.avi)");
	  if(!path.isEmpty())
	       lineEdit->setText(path);
     }
     else
     {
	  QString path = QFileDialog::getExistingDirectory(
	       this, tr("Choose directory"), lineEdit->text());
	  if(!path.isEmpty())
	       lineEdit->setText(path);
     }
}
