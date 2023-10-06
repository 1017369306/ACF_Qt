#include "acfproperty.h"

#include "defaultplugin.h"
#include "pluginManager.h"

ACFProperty *ACFProperty::m_instance = nullptr;

ACFProperty::ACFProperty()
{
    m_logPlugin = new DefaultPlugin();
    m_notifyPlugin = new DefaultPlugin();

    connect(PluginManager::instance(), &PluginManager::pluginStatusChanged, this, &ACFProperty::slot_pluginStatusChanged);
}

ACFProperty::~ACFProperty(){
    if(m_logPlugin != nullptr){
        delete m_logPlugin;
        m_logPlugin = nullptr;
    }
    if(m_notifyPlugin != nullptr){
        delete m_notifyPlugin;
        m_notifyPlugin = nullptr;
    }
}

void ACFProperty::slot_pluginStatusChanged(const PlugInProperty& property, const bool& isLoad){
    if(property.type == LogPlugin_Name){
        if(m_logPlugin != nullptr){
            delete m_logPlugin;
            m_logPlugin = nullptr;
        }
        m_logPlugin = PluginManager::instance()->getIPlugin(property.name);
    }
    else if(property.type == NotifyPlugin_Name){
        if(m_notifyPlugin != nullptr){
            delete m_notifyPlugin;
            m_notifyPlugin = nullptr;
        }
        m_notifyPlugin = PluginManager::instance()->getIPlugin(property.name);
    }

}

