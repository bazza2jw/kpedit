#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "ui_plugindialog.h"
#include "../../kpedit/src/interfaces.h"

class PluginDialog : public QDialog, public Ui::PluginDialog
{
     Q_OBJECT;
public:
     PluginDialog(KPInterface* kp);
     virtual ~PluginDialog();
private:
     KPInterface* kpedit;

     private slots:
	  void okClicked();
};

#endif
