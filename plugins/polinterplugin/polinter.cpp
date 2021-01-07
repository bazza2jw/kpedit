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

#include "polinter.h"

Polinter::Polinter()
{

}

Polinter::~Polinter()
{

}

QList<KeyPointPos>Polinter::interpolate(const QList<KeyPointPos>& nodes, int deg, int steps)
{
     QList<KeyPointPos> ipoints;
     if(nodes.size() < deg+1)
	  return ipoints;

     //print("nodes",nodes);

     if(nodes.size() == deg+1)
	  return interpolate(nodes,steps);

     //dzielimy listę na równe części
     ipoints += interpolate(nodes.mid(0,deg+1), steps);
     //print("first ipoints", ipoints);
     int c = (nodes.size()-(deg+1))/deg; //całych podlist
     int m = (nodes.size()-(deg+1))%deg; //elementy pozostałe

     // cout << "deg: " << deg << endl;
     // cout << "c: " << c << endl;
     // cout << "m: " << m << endl;

     for(int i=0;i<c;i++)
     {
	  QList<KeyPointPos> tmp = interpolate(nodes.mid(deg+i*deg,deg+1),steps);
	  //print("new ipoints",tmp);
	  //usuwamy punkt wspolny aby go nie powielic
	  tmp.takeFirst();
	  ipoints += tmp;
     }

     
     if(m) //jeżeli zostaną węzły nieinterpolowane
     {
	  QList<KeyPointPos> tmp;
	  tmp = interpolate(nodes.mid(nodes.size()-1-deg),steps); //interpolujemy z węzłami wstecz tak aby stopień wielomianu się zgadzał, w niektórych przypadkach to może popsuć krzywą :/ na razie nie mam pomysłu jak ją poprawić nie zwiększając stopnia wielomianu
	  tmp = tmp.mid(tmp.size()-m-steps*m); //wycinamy tylko część dla jeszcze nie interpolowanych węzłów

	  //print("last ipoints",tmp);
	  ipoints += tmp;
     }

     //print("final ipoints", ipoints);

     return ipoints;
}

QList<KeyPointPos>Polinter::interpolate(const QList<KeyPointPos>& nodes, int steps)
{
     QList<KeyPointPos> ipoints;
     if(!nodes.size())
	  return ipoints;

     //print("nodes", nodes);

     //dla x:

     MatrixXd M(nodes.size(),nodes.size());
     VectorXd xc(nodes.size());
     for(int i=0;i<nodes.size();i++)
     {
	  for(long d=0;d<nodes.size();d++)
	       M(i,d) = pow(i,nodes.size()-d-1);
	  xc(i) = nodes.at(i).x;
     }
     //cout << "matrix M:\n" << M << endl;
     //cout << "vector xc:\n" << xc << endl;
     VectorXd xsolv = M.colPivHouseholderQr().solve(xc);
     //cout << "xsolv: \n" << xsolv << endl;
     
     //dla y:
     VectorXd yc(nodes.size());
     for(long i=0;i<nodes.size();i++)
     	  yc(i) = nodes.at(i).y;
     //cout << "vector yc:\n" << yc << endl;
     VectorXd ysolv = M.colPivHouseholderQr().solve(yc);
     //cout << "ysolv: \n" << ysolv << endl;

     //obliczamy punkty posrednie:
     double s = 1/((double)steps+1); 
     //cout << "s: " << s << endl;
     
     for(int i=0;i<nodes.size()-1;i++)
     {
	  for(int j=0;j<steps+1;j++)
	  {
	       KeyPointPos p;
	       double x = i+j*s;
	       p.x = polyval(x, xsolv);
	       //cout << "fx(" << x << ") = " << p.x << endl;
	       p.y = polyval(x, ysolv);
	       ipoints.append(p);
	  }
     }
     ipoints << nodes.at(nodes.size()-1);
     //print("ipoints", ipoints);
     return ipoints;
}

double Polinter::polyval(double x, VectorXd c)
{
     double r = 0;
     for(int i = 0; i < c.size(); i++)
	  r += c(i)*pow(x,c.size()-i-1);
     return r;
}

void Polinter::print(QString label, const QList<KeyPointPos>& points)
{
     cout << "\n" << label.toStdString() << ":";
     for(int i=0;i<points.size();i++)
	  cout << " (" << points.at(i).x << ", " << points.at(i).y << ")";
     cout << endl;
}
