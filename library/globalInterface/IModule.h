#ifndef IUIMODULE_H
#define IUIMODULE_H

#include <globalInterface_global.h>
#include <QMenu>
#include <QObject>
#include <QUuid>
#include <ITask.h>

//一定是唯一的标识符
#define IModule_iid "library.GlobalInterface.IModule"

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
class ModuleProperty
{
public:
    QString name;//服务端对应的name
    QString displayname;//显示名称
    QString libName;//模块的库名
    QString icon;//模块图标
    QString description;//模块的描述
    QString author;//模块的作者
    QString version;//模块的版本
    int multipleInstance;//模块支持几个实例
    bool canClose;//模块是否可关闭
    ModuleGroup group;//模块的分组
    int index;//排序
    QList<int> moduleToolBarItems;//模块的工具栏具有哪些工具
    QVariant tag;//自定义数据

public:

    bool operator ==(const ModuleProperty &other){
        return (name == other.name && displayname == other.displayname &&
                libName == other.libName && author == other.author && version == other.version);
    }

    bool operator !=(const ModuleProperty &other){
        return (name != other.name || displayname != other.displayname ||
                libName != other.libName || author != other.author || version != other.version);
    }

    ModuleProperty Clone()
    {
        ModuleProperty moduleProperty;
        moduleProperty.name = this->name;
        moduleProperty.libName = this->libName;
        moduleProperty.displayname = this->displayname;
        moduleProperty.icon = this->icon;
        moduleProperty.description = this->description;
        moduleProperty.author = this->author;
        moduleProperty.version = this->version;
        moduleProperty.multipleInstance = this->multipleInstance;
        moduleProperty.canClose = this->canClose;
        moduleProperty.group = this->group;
        moduleProperty.index = this->index;
        moduleProperty.moduleToolBarItems = this->moduleToolBarItems;
        moduleProperty.tag = this->tag;
        return moduleProperty;
    }
};
Q_DECLARE_METATYPE(ModuleProperty)

/**
 * @brief The IModule class 定义客户端功能模块接口
 * 作者 zhangyang
 * 创建日期 2022-12-02
 */
class IModule
{

    //接口的公共方法定义
public:
    /**
     * @brief getGUID 获取唯一识别编号
     * @return 返回唯一识别编号
     */
    virtual QUuid getGUID()=0;

    /**
    * @brief getModuleProperty 返回此模块的描述信息
    * @return
    */
    virtual ModuleProperty getModuleProperty()=0;

    /**
     * @brief getTaskObject 获取任务控制对象
     * @return 返回任务控制对象
     */
    virtual ITask *getTaskObject()=0;

    /**
     * @brief getRightMenu 此功能对应的右键菜单
     * @return
     */
    virtual QMenu getRightMenu()=0;

    /**
     * @brief initModule 初始化模块
     * @param tasktype true:回放任务；false:实时任务
     */
    virtual void initModule(int taskType)=0;

    /**
     * @brief uiOrder 执行UI界面层下发的指令，如启动测量、停止测量、抓图、过程记录、结果记录、清空重置等等
     * @param order 命令
     * @return 命令是否执行成功
     */
    virtual int uiOrder(const CommandType &order, QVariant tag = QVariant::Invalid)=0;

    /**
    * @brief getTag 用户自定义数据
    * @return
    */
    virtual QObject getTag()=0;

    //接口的保护方法定义
protected:

    /**
    * @brief getModuleProperty 返回此模块的描述信息
    * @return
    */
   virtual ModuleProperty getConstModuleProperty()=0;

    /**
    * @brief OnModelInitializing 基类在开始初始化之前，这个事件被触发。
    */
    virtual void OnModelInitializing()=0;

    /**
    * @brief OnWindowsOpening 窗体打开中
    */
    virtual void OnWindowsOpening()=0;
    /**
    * @brief OnWindowsOpened 窗体打开完成
    * @param event
    */
    virtual void OnWindowsOpened(QShowEvent *event)=0;

    /**
    * @brief OnWindowsCloseing 窗体关闭中
    */
    virtual void OnWindowsCloseing()=0;

    /**
    * @brief OnWindowsClosed 窗体关闭完成
    * @param event
    */
    virtual void OnWindowsClosed(QCloseEvent *event)=0;

    /**
    * @brief OnSelectedChanged 窗体选择事件
    */
    virtual void OnSelectedChanged()=0;

    /**
     * @brief disposeing 释放资源
     */
    virtual void disposeing() = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(IModule, IModule_iid)
QT_END_NAMESPACE

#endif // IUIMODULE_H
