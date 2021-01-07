#include "plugindialog.h"

PluginDialog::PluginDialog(KPInterface* kp)
{
     setupUi(this);
     kpedit = kp;

     connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

PluginDialog::~PluginDialog()
{

}



void PluginDialog::okClicked()
{
     
}
