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

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc,argv);

	// KPPlayer cp;
	// cp.show();
	// if(cp.openSource("/home/chodak/rec/test.mp4"))
	// {
	//      cp.nextFrame();
	//      //cp.setPosMPEG(cp.countFrames()/2);
	//      qDebug() << "wszystkich klatek: " << cp.countFrames();
	//      qDebug() << "szacowana przerwa: " << cp.estimateGap();
	// }
	// else
	//      qDebug() << "nie mozna otworzyc pliku wideo";
	app.setWindowIcon(QIcon(":/res/icon.png"));
	MainWindow mw;
	mw.show();

	return app.exec();
}
