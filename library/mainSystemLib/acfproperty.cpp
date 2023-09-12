#include "acfproperty.h"

#include "defaultplugin.h"
#include "pluginManager.h"

ACFProperty *ACFProperty::m_instance = nullptr;

ACFProperty::ACFProperty()
{
    connect(PluginManager::instance(), &PluginManager::pluginStatusChanged, this, &ACFProperty::slot_pluginStatusChanged);
}

ACFProperty::~ACFProperty(){

}

void ACFProperty::slot_pluginStatusChanged(const PlugInProperty& property, const bool& isLoad){
    if(property.type == LogPlugin_Name){
        m_logPlugin = PluginManager::instance()->getIPlugin(property.name);
        if(m_logPlugin == nullptr){
            m_logPlugin = new DefaultPlugin();
        }

    }
    else if(property.type == NotifyPlugin_Name){
        m_notifyPlugin = PluginManager::instance()->getIPlugin(property.name);
        if(m_notifyPlugin == nullptr){
            m_notifyPlugin = new DefaultPlugin();
        }
    }

}

