#include "pluginManager.h"
#include <QDir>
#include <QCoreApplication>
#include <QJsonArray>
#include <globalhelper.h>
#include <QDebug>

PluginManager* PluginManager::m_instance=nullptr;

class PluginManager::PluginsManagerPrivate
{
public:
    PluginsManagerPrivate()
    {
        m_names.clear();
        m_versions.clear();
        m_dependencies.clear();
        m_loaders.clear();
        m_propertys.clear();
    }
    ~PluginsManagerPrivate(){}

    QHash<QString, QVariant> m_names;               //插件路径--插件名称
    QHash<QString, QVariant> m_versions;            //插件路径--插件版本
    QHash<QString, QVariantList> m_dependencies;    //插件路径--插件额外依赖的其他插件
    QHash<QString, QPluginLoader *> m_loaders;      //插件路径--QPluginLoader实例
    QHash<QString, PlugInProperty> m_propertys;      //插件路径--插件信息

    bool check(const QString &filepath)             //插件依赖检测
    {
        bool status = true;

        QVariantList dependencies = m_dependencies.value(filepath);
        foreach (QVariant item, dependencies) {
            QVariantMap map = item.toMap();
            // 依赖的插件名称、版本、路径
            QVariant name = map.value("name");
            QVariant version = map.value("version");
            QString path = m_names.key(name);

            /********** 检测插件是否依赖于其他插件 **********/
            // 先检测插件名称
            if (!m_names.values().contains(name)) {
                qDebug() << Q_FUNC_INFO << "  Missing dependency:" << name.toString() << "for plugin" << path;
                status = false;
                continue;
            }

            // 再检测插件版本
            if (m_versions.value(path) != version) {
                qDebug() << Q_FUNC_INFO << "    Version mismatch:" << name.toString() << "version"
                         << m_versions.value(m_names.key(name)).toString() << "but" << version.toString() << "required for plugin" << path;
                status = false;
                continue;
            }

            // 然后，检测被依赖的插件是否还依赖于另外的插件
            if (!check(path)) {
                qDebug() << Q_FUNC_INFO << "Corrupted dependency:" << name.toString() << "for plugin" << path;
                status = false;
                continue;
            }
        }

        return status;
    }

};

PluginManager::PluginManager(QObject *parent) : QObject(parent)
{
    m_managerPrivate = new PluginsManagerPrivate;
}

PluginManager::~PluginManager()
{
    delete m_managerPrivate;
}

void PluginManager::loadAllPlugins()
{
    QDir pluginsDir(m_pluginDir);    //pluginsDir: "../build-xxx-debug/debug"
    if(!pluginsDir.exists()){
        return;
    }

//    QFileInfoList pluginsInfo = pluginsDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    //递归遍历下面的文件夹
    QFileInfoList pluginsInfo = GlobalMethods::findAllFiles(m_pluginDir, QStringList());

    //初始化插件中的元数据
    for(QFileInfo fileinfo : pluginsInfo){
        scanMetaData(fileinfo.absoluteFilePath());
    }

    //加载插件
    for(QFileInfo fileinfo : pluginsInfo)
        loadSystemPlugin(fileinfo.absoluteFilePath());
}

void PluginManager::unloadAllPlugins()
{
    for(QString filepath : m_managerPrivate->m_loaders.keys())
        unloadPlugin(filepath);
}

void PluginManager::scanMetaData(const QString &filepath)
{
    //判断是否为库（后缀有效性）
    if(!QLibrary::isLibrary(filepath))
        return ;

    qDebug() << "scanMetaData:" << filepath;
    //获取元数据
    QPluginLoader *loader = new QPluginLoader(filepath);

    qDebug()<<loader->metaData().keys();
    QJsonObject json = loader->metaData().value("MetaData").toObject();
    if(json.length() > 0){
        PlugInProperty property;
        if(json.contains(PluginJson_Name)){
            property.name = json.value(PluginJson_Name).toString();
        }
        if(json.contains(PluginJson_Type)){
            property.type = json.value(PluginJson_Type).toString();
        }
        if(json.contains(PluginJson_DisplayName)){
            property.displayName = json.value(PluginJson_DisplayName).toString();
        }
        if(json.contains(PluginJson_Description)){
            property.description = json.value(PluginJson_Description).toString();
        }
        if(json.contains(PluginJson_Category)){
            property.category = json.value(PluginJson_Category).toString();
        }
        if(json.contains(PluginJson_Icon)){
            property.icon = json.value(PluginJson_Icon).toString();
        }
        if(json.contains(PluginJson_Author)){
            property.author = json.value(PluginJson_Author).toString();
        }
        if(json.contains(PluginJson_Version)){
            property.version = json.value(PluginJson_Version).toString();
        }
        if(json.contains(PluginJson_Copyright)){
            property.copyright = json.value(PluginJson_Copyright).toString();
        }
        if(json.contains(PluginJson_Level)){
            property.level = (PlugInLevel)json.value(PluginJson_Level).toInt();
        }
        if(json.contains(PluginJson_CustomData)){
            property.customData = json.value(PluginJson_CustomData).toVariant();
        }
        if(json.contains(PluginJson_Dependencies)){
            m_managerPrivate->m_dependencies.insert(filepath, json.value(PluginJson_Dependencies).toArray().toVariantList());
        }

        m_managerPrivate->m_names.insert(filepath, property.name);
        m_managerPrivate->m_versions.insert(filepath, property.version);
        m_managerPrivate->m_propertys.insert(filepath, property);
    }

    delete loader;
    loader = nullptr;
}

QString PluginManager::getPluginDir() const
{
    return m_pluginDir;
}

void PluginManager::setPluginDir(const QString &pluginDir)
{
    m_pluginDir = pluginDir;
}

QList<QVariant> PluginManager::allPluginsName()
{
    return m_managerPrivate->m_names.values();
}

QList<PlugInProperty> PluginManager::allPluginsProperty(){
    return m_managerPrivate->m_propertys.values();
}

PlugInProperty PluginManager::getPluginProperty(const QString &name){
    QString filePath = m_managerPrivate->m_names.key(name);
    if(!filePath.isEmpty() && m_managerPrivate->m_propertys.contains(filePath)){
        return m_managerPrivate->m_propertys.value(filePath);
    }
    return PlugInProperty();
}

QString PluginManager::getPath(const QString &name){
    return m_managerPrivate->m_names.key(name);
}

QPluginLoader *PluginManager::getPlugin(const QString &name)
{
    return m_managerPrivate->m_loaders.value(m_managerPrivate->m_names.key(name));
}

IPlugIn *PluginManager::getIPlugin(const QString &name){
    QPluginLoader *plugin = this->getPlugin(name);
    if(plugin != nullptr){
        IPlugIn *iplugin = qobject_cast<IPlugIn *>(plugin->instance());
        return iplugin;
    }
    return nullptr;
}

void PluginManager::loadPlugin(const QString &filepath)
{
    if(!QLibrary::isLibrary(filepath))
        return;

    //如果没有扫描过此插件的元数据，扫描JSON文件中的插件元数据
    if(!m_managerPrivate->m_propertys.contains(filepath)){
        scanMetaData(filepath);
    }

    if(!m_managerPrivate->m_propertys.contains(filepath)){
        return;
    }

    //检测依赖
    if(!m_managerPrivate->check(filepath))
        return;

    //加载插件
    QPluginLoader *loader = new QPluginLoader(filepath);
    if(loader->load())
    {
        IPlugIn *plugin = qobject_cast<IPlugIn *>(loader->instance());
        if(plugin)
        {
            PlugInProperty property = m_managerPrivate->m_propertys.value(filepath);
            plugin->setPlugInProperty(property);
            plugin->initModule();
            m_managerPrivate->m_loaders.insert(filepath, loader);

            emit pluginStatusChanged(property, true);
        }
        else{
            loader->unload();
            delete loader;
            loader = nullptr;
        }
    }
    else{
        qDebug() << "loadPlugin:" << filepath << loader->errorString();
    }
}

void PluginManager::loadSystemPlugin(const QString &filepath)
{
    if(!QLibrary::isLibrary(filepath))
        return;

    //如果没有扫描过此插件的元数据，扫描JSON文件中的插件元数据
    if(!m_managerPrivate->m_propertys.contains(filepath)){
        scanMetaData(filepath);
    }

    //检测依赖
    if(!m_managerPrivate->check(filepath))
        return;

    if(m_managerPrivate->m_propertys.contains(filepath) && !m_managerPrivate->m_loaders.contains(filepath)){
        PlugInProperty property = m_managerPrivate->m_propertys.value(filepath);
        //如果是初始化就加载的插件，这里直接加载
        if(property.level == PlugInLevel::MustLoad){
            //加载插件
            QPluginLoader *loader = new QPluginLoader(filepath);
            if(loader->load())
            {
                IPlugIn *plugin = qobject_cast<IPlugIn *>(loader->instance());
                if(plugin)
                {
                    plugin->setPlugInProperty(property);
                    plugin->initModule();
                    m_managerPrivate->m_loaders.insert(filepath, loader);

                    emit pluginStatusChanged(property, true);
                }
                else{
                    loader->unload();
                    delete loader;
                    loader = nullptr;
                }
            }
            else{
                qDebug() << "loadPlugin:" << filepath << loader->errorString();
            }
        }
    }
}

void PluginManager::unloadPlugin(const QString &filepath)
{
    QPluginLoader *loader = m_managerPrivate->m_loaders.value(filepath);
    //卸载插件，并从内部数据结构中移除
    if(loader->unload())
    {
        m_managerPrivate->m_loaders.remove(filepath);
        delete loader;
        loader = nullptr;

        if(m_managerPrivate->m_propertys.contains(filepath)){
            emit pluginStatusChanged(m_managerPrivate->m_propertys.value(filepath), false);
        }
    }
}

