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

#include "rounddialog.h"

RoundDialog::RoundDialog(long frames, RoundSet* rs)
{
     setupUi(this);
     set = rs;
     toBox->setMaximum(frames < 0 ? 0 : frames);
     toBox->setValue(frames < 0 ? 0 : frames);
     fromBox->setMaximum(frames < 0 ? 0 : frames);
     connect(decBox, SIGNAL(valueChanged(int)), this, SLOT(updateExample()));
     connect(roundButton, SIGNAL(clicked()), this, SLOT(roundClicked()));
     connect(floorButton, SIGNAL(clicked()), this, SLOT(floorClicked()));
     connect(ceilButton, SIGNAL(clicked()), this, SLOT(ceilClicked()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
     updateExample();
}

RoundDialog::~RoundDialog()
{

}

void RoundDialog::updateExample()
{
     reLabel->setText("round: 1.234567 -> "+
		      QString::number(RoundPlugin::round(1.234567,decBox->value())));
     feLabel->setText("floor: 1.234567 -> "+
		      QString::number(RoundPlugin::floor(1.234567,decBox->value())));
     ceLabel->setText("ceil: 1.234567 -> "+
		      QString::number(RoundPlugin::ceil(1.234567,decBox->value())));
}

void RoundDialog::roundClicked()
{
     set->operation = OP_ROUND;
     end();
}

void RoundDialog::floorClicked()
{
     set->operation = OP_FLOOR;
     end();
}

void RoundDialog::ceilClicked()
{
     set->operation = OP_CEIL;
     end();
}

void RoundDialog::end()
{
     if(fromBox->value() > toBox->value())
     {
	  QMessageBox::critical(NULL,tr("Error"), tr("The range is invalid!"));
	  return;
     }
     set->startFrame = fromBox->value();
     set->endFrame = toBox->value();
     set->dec = decBox->value();
     set->ok = true;
     close();
}
