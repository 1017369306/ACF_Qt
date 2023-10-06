#ifndef MODULEBASE_H
#define MODULEBASE_H

#include "mainSystemLib_global.h"
#include <IPlugIn.h>
#include <ITask.h>
#include <QObject>
#include <QMetaType>
#include <QWidget>
#include <QMenu>
#include <globalInfoEnum.h>

#define PluginJson_MultipleInstance "MultipleInstance"
#define PluginJson_CanClose "CanClose"
#define PluginJson_Group "Group"
#define PluginJson_Index "Index"

enum ModuleGroup{
    /**
    * 未指定模块类型
    */
    Module_None = 0,
    /**
    * 常规模块（非测量模块）
    */
    Module_Normal,
    /**
    * 常规测量模块
    */
    Module_Measure,
    /**
    * 测试模块
    */
    Module_Test
};

/**
 * @brief The ModuleProperty struct 模块的描述信息
 */
class MAINSYSTEMLIB_EXPORT ModuleProperty
{
public:

    ModuleProperty(QObject *parent = 0){
        this->multipleInstance = 0;
        this->canClose = true;
        this->group = ModuleGroup::Module_None;
        this->index = -1;
    }

    QString name;//服务端对应的name
    QString displayName;//显示名称
    QString description;//模块的描述
    QString icon;//模块图标
    int multipleInstance;//模块支持几个实例
    bool canClose;//模块是否可关闭
    ModuleGroup group;//模块的分组
    int index;//排序
    QVariant tag;//自定义数据

public:

    bool operator ==(const ModuleProperty &other){
        return (name == other.name && displayName == other.displayName);
    }

    bool operator !=(const ModuleProperty &other){
        return (name != other.name || displayName != other.displayName);
    }

    static bool compareIndex(const ModuleProperty &module1, const ModuleProperty &module2){
        return module1.index < module2.index;
    }

    ModuleProperty Clone()
    {
        ModuleProperty moduleProperty;
        moduleProperty.name = this->name;
        moduleProperty.displayName = this->displayName;
        moduleProperty.icon = this->icon;
        moduleProperty.description = this->description;
        moduleProperty.multipleInstance = this->multipleInstance;
        moduleProperty.canClose = this->canClose;
        moduleProperty.group = this->group;
        moduleProperty.index = this->index;
        moduleProperty.tag = this->tag;
        return moduleProperty;
    }
};
Q_DECLARE_METATYPE(ModuleProperty)

/**
 * @brief The ModuleBase class 功能模块的基类
 */
class MAINSYSTEMLIB_EXPORT ModuleBase : public QWidget,  public IPlugIn
{ 
    Q_OBJECT
public:
    explicit ModuleBase(QWidget *parent = nullptr);
    virtual ~ModuleBase();

public:
    //重写接口属性实现
    /**
     * @brief getGUID 获取唯一识别编号
     * @return 返回唯一识别编号
     */
    QUuid getGUID() override {return m_guid;}

    /**
     * @brief initModule 初始化模块
     */
    int initModule() override;

    /**
     * @brief run 运行插件
     * @return
     */
    int run() override {return GlobalInfo::GlobalInfoEnum::DealSuccess;}

    /**
     * @brief stop 停止插件
     * @return
     */
    int stop() override {return GlobalInfo::GlobalInfoEnum::DealSuccess;}

    /**
     * @brief sendData 插件运行起来后，给其发送数据
     * @param data
     */
    void sendData(QVariant data = QVariant::Invalid) override {}

    /**
     * @brief ThemeChanged 主题改变通知
     */
    virtual void cssStyleChanged() override {}

    /**
     * @brief LanguageChanged 语言改变通知
     */
    virtual void LanguageChanged() override {}

    static ModuleProperty convertFrom(const PlugInProperty &plugInProperty);

public:

    /**
    * @brief getModuleProperty 返回此模块的描述信息
    * @return
    */
    ModuleProperty getModuleProperty() {return m_moduleProperty;}

    /**
     * @brief getTaskObject 获取任务控制对象
     * @return 返回任务控制对象
     */
    ITask* getTaskObject() {return m_taskobj;}

    /**
     * @brief getTaskRuning
     * @return 是否正在执行任务
     */
    TaskStatus getTaskRuning();

    /**
     * @brief uiOrder 执行UI界面层下发的指令，如启动测量、停止测量、抓图、过程记录、结果记录、清空重置等等
     * @param order 命令
     * @return 命令是否执行成功
     */
    int uiOrder(const CommandType &order, QVariant tag = QVariant::Invalid);

protected:

    /**
     * @brief starting
     * @return
     */
    virtual int starting();
    /**
     * @brief started
     * @return
     */
    virtual void started();
    /**
     * @brief stoping
     * @return
     */
    virtual int stoping();
    /**
     * @brief stoped
     * @return
     */
    virtual void stoped();
    /**
     * @brief pausing
     * @return
     */
    virtual int pausing();
    /**
     * @brief paused
     * @return
     */
    virtual void paused();
    /**
     * @brief continuing
     * @return
     */
    virtual int continuing();
    /**
     * @brief continued
     * @return
     */
    virtual void continued();

    /**
     * @brief otherOrdering
     * @return
     */
    virtual int otherOrdering();
    /**
     * @brief continued
     * @return
     */
    virtual void otherOrdered();

private:
    /**
     * @brief StartTask 开始测量任务
     * @return 是否执行成功
     */
    int StartTask();
    /**
     * @brief StopTask 停止测量任务
     * @return 是否执行成功
     */
    int StopTask(StopReason stopReason);

    /**
     * @brief PauseTask Pause测量任务
     * @return 是否执行成功
     */
    int PauseTask();

    /**
     * @brief ContinueTask Continue测量任务
     * @return 是否执行成功
     */
    int ContinueTask();

private:
    /**
     * @brief m_str_GUID 功能组件的唯一识别ID
     */
    QUuid m_guid;

    /**
     * @brief m_moduleProperty 模块的描述信息
     */
    ModuleProperty m_moduleProperty;

    /**
     * @brief m_PopMenu 功能项的右键菜单
     */
    QMenu *m_PopMenu = nullptr;

    /**
     * @brief m_taskobj 任务模块变量
     */
    ITask* m_taskobj = nullptr;

};

#endif // MODULEBASE_H
