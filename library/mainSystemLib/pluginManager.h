#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "mainSystemLib_global.h"
#include <QObject>
#include <QPluginLoader>
#include <QVariant>
#include <IPlugIn.h>

#define PluginJson_Name "Name"
#define PluginJson_Type "Type"
#define PluginJson_DisplayName "DisplayName"
#define PluginJson_Description "Description"
#define PluginJson_Category "Category"
#define PluginJson_Icon "Icon"
#define PluginJson_Author "Author"
#define PluginJson_Version "Version"
#define PluginJson_Copyright "Copyright"
#define PluginJson_Level "Level"
#define PluginJson_Dependencies "Dependencies"
#define PluginJson_CustomData "CustomData"

class MAINSYSTEMLIB_EXPORT PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager();

public:
    static PluginManager *instance(){
        if(m_instance == nullptr)
            m_instance = new PluginManager();
        return m_instance;
    }

    //获取所有插件的名字
    QList<QVariant> allPluginsName();

    //获取所有插件的属性
    QList<PlugInProperty> allPluginsProperty();

    //获取某个插件的属性
    PlugInProperty getPluginProperty(const QString &name);

    //获取某个插件的Path
    QString getPath(const QString &name);

    //根据插件名获取插件
    QPluginLoader *getPlugin(const QString &name);

    //根据插件名获取插件
    IPlugIn *getIPlugin(const QString &name);

    QString getPluginDir() const;
    void setPluginDir(const QString &pluginDir);

    //加载所有的插件
    void loadAllPlugins();

    //卸载所有插件
    void unloadAllPlugins();

    //加载某个插件
    void loadPlugin(const QString &filePath);

    //卸载某个插件
    void unloadPlugin(const QString &filepath);

signals:

    void pluginStatusChanged(const PlugInProperty& property, const bool& isLoad);

private:

    //扫描JSON文件中的插件元数据
    void scanMetaData(const QString &filepath);

    //加载系统必要插件
    void loadSystemPlugin(const QString &filePath);

private:

    static PluginManager *m_instance;

    class PluginsManagerPrivate;
    PluginsManagerPrivate *m_managerPrivate;

    QString m_pluginDir;

};
#endif // PLUGINMANAGER_H
