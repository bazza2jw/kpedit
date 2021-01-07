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

#ifndef POLINTERP_H
#define POLINTERP_H

#include <iostream>
#include <Eigen/Dense>
#include <QList>
#include <QString>
#include "../../kpedit/src/interfaces.h" //KeyPointPos i KPInterface dla PIDialog
using namespace std;
using namespace Eigen;


class Polinter
{
public:
     Polinter();
     virtual ~Polinter();
     static QList<KeyPointPos> interpolate(const QList<KeyPointPos>& nodes, int deg, int steps);
     static QList<KeyPointPos> interpolate(const QList<KeyPointPos>& nodes, int steps);
     static double polyval(double x, VectorXd c);
     static void print(QString label, const QList<KeyPointPos>& points);
private:


};

#endif
