#include "plugin.h"

Plugin::Plugin()
{
    
}

Plugin::~Plugin()
{
     
}

void Plugin::onLoad(KPInterface* kp)
{
     kpedit = kp;
}

void Plugin::onUnload()
{

}

QAction* Plugin::createAction()
{
     QAction* a = new QAction(tr("Plugins menu entry..."),this);
     connect(a, SIGNAL(triggered()), this, SLOT(activate()));
     return a;
}

void Plugin::activate()
{
     PluginDialog pd(kpedit);
     pd.exec();
}

Q_EXPORT_PLUGIN2(kp_some_plugin, Plugin);

