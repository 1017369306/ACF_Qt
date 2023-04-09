#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QObject>
#include <QUuid>
#include <QVariant>

//一定是唯一的标识符
#define Plugin_iid "IPlugIn"

/**
 * @brief The PlugInType enum 插件的类型（暂时不考虑使用此枚举）
 */
enum PlugInType{
    /**
    * 未指定插件类型
    */
    PlugIn_None = 0,
    /**
    * TCP通讯插件
    */
    PlugIn_TCP,
    /**
    * UDP通讯插件
    */
    PlugIn_UDP,
    /**
    * soap插件
    */
    PlugIn_Soap,
    /**
    * http通讯插件
    */
    PlugIn_Http,
    /**
    * 地图插件
    */
    PlugIn_Map,
    /**
    * Sqlite3数据库插件
    */
    PlugIn_Sqlite3,
    /**
    * 日志插件
    */
    PlugIn_Logger,
    /**
    * 消息提示插件
    */
    PlugIn_Notice,
    /**
    * 消息弹窗插件
    */
    PlugIn_PopupWindow,
    /**
    * 功能模块插件
    */
    PlugIn_Module,
    /**
    * 消息中间件插件
    */
    PlugIn_PassMessage,
    /**
    * 测试插件
    */
    PlugIn_Test
};

/**
 * @brief The PlugInLevel enum 插件等级
 */
enum PlugInLevel{
    //默认、按需加载
    NeedLoad = 0,
    //必装插件、启动就加载
    MustLoad
};

/**
 * @brief The PlugInProperty struct 插件的信息
 */
class PlugInProperty
{
public:
    QString name;//服务端对应的name
    QString displayName;//显示名称
    QString description;//插件的描述
    QString category;//插件的分组
    QString icon;//插件的图标
    QString author;//插件的作者
    QString version;//插件的版本
    QString copyright;//插件的版权信息
    PlugInLevel level;//插件等级：0：默认、按需加载；1：必装插件、启动就加载
    QVariant customData;//自定义数据

};
Q_DECLARE_METATYPE(PlugInProperty)

/**
 * @brief The IPlugIn class 插件模块接口，所有插件必须继承此接口
 * 作者 张洋
 * 创建日期 2022-12-05
 */
class IPlugIn
{
  //接口属性定义
 public:
    /**
    * @brief getGUID 获取唯一识别编号
    * @return 返回唯一识别编号
    */
    virtual QUuid getGUID()=0;

    /**
     * @brief getPlugInProperty 获取插件的信息
     * @return 插件的信息
     */
    PlugInProperty getPlugInProperty() const {return m_plugInProperty;}

    /**
    * @brief setPlugInProperty 设置插件的信息
    * @param property 插件的信息
    * @return
    */
    void setPlugInProperty(const PlugInProperty &property){
        m_plugInProperty = property;
    }

    /**
    * @brief getWidget 获得作用的窗体。注意：作为独立窗体显示：setWindowFlags(Qt::Window);
    * 不阻塞的模态窗体，如果内嵌窗体已经设置了父窗体：setWindowModality(Qt::WindowModal);
    * 不阻塞的模态窗体，如果内嵌窗体未设置了父窗体：setWindowModality(Qt::ApplicationModal);
    * @param parent 父窗体
    * @return
    */
    virtual QWidget *getWidget(QWidget *parent = nullptr) = 0;

    /**
     * @brief createNewPlugin 创建新的插件实例
     * @return
     */
    virtual IPlugIn *createNewPlugin()=0;

    /**
    * @brief initModule 初始化模块
    * @return true：初始化成功；false:初始化失败
    */
    virtual int initModule()=0;

    /**
     * @brief run 运行插件
     * @return
     */
    virtual int run()=0;

    /**
     * @brief stop 停止插件
     * @return
     */
    virtual int stop()=0;

    /**
     * @brief sendData 插件运行起来后，给其发送数据
     * @param data
     */
    virtual void sendData(QVariant data = QVariant::Invalid)=0;

    /**
    * @brief disModule 销毁模块
    */
    virtual void disModule()=0;

    /**
     * @brief cssStyleChanged 样式改变通知
     */
    virtual void cssStyleChanged()=0;

    /**
     * @brief LanguageChanged 语言改变通知
     */
    virtual void LanguageChanged()=0;


private:

    PlugInProperty m_plugInProperty;

};
Q_DECLARE_INTERFACE(IPlugIn, Plugin_iid)

#endif // IPLUGIN_H
