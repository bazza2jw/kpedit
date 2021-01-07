#ifndef ROUND_H
#define ROUND_H

#include <QObject>
#include <QString>
#include <QtPlugin>
#include <QAction>
#include <QDebug>
#include "plugindialog.h"
#include "../../kpedit/src/interfaces.h"

class Plugindialog;

class Plugin : public QObject, public KPPluginInterface
{
     Q_OBJECT;
     Q_INTERFACES(KPPluginInterface);
     
public:
     Plugin();
     virtual ~Plugin();
     QString pluginVersion() const {return "0.1";}
     void onLoad(KPInterface*);
     void onUnload();
     QAction* createAction();


     public slots:
	  void activate();
	  
private:
	  KPInterface* kpedit;
     private slots:

};


#endif
