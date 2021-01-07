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

#ifndef KEYPOINT_H
#define KEYPOINT_H

#include <QList>
#include <QString>

struct KeyPointPos
{
KeyPointPos():x(-1),y(-1){}
KeyPointPos(double nx, double ny):x(nx),y(ny){}
     bool isSet() const {return x != -1 && y != 1;}
     double x;
     double y;
};

struct KeyPoint
{
     QString name; ///< Nazwa punktu
     QList<KeyPointPos> framePos; ///< Pozycja punktu w określonej klatce (wg indeksu)
     QList<KeyPoint*> linkedPoints; ///< Adresy połączonych punktów
};

#endif
