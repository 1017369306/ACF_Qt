#ifndef NOTIFYBASE_H
#define NOTIFYBASE_H

#include <QObject>
#include <IPlugIn.h>

class NotifyBase : public QObject, public IPlugIn
{
    Q_OBJECT
public:

    /**
     * @brief 弹窗等级
     */
    enum NotifyLevel
    {
        InfoLevel = 0,
        SuccessLevel,
        WarnLevel,
        ErrorLevel,
        FatalLevel
    };
    Q_ENUM(NotifyLevel)

    NotifyBase(){
        this->m_guid = QUuid::createUuid(); //new guid
    }

    ~NotifyBase(){}

    static NotifyBase *instance(){
        return m_notify;
    }

    /**
     * @brief getGUID 获取唯一识别编号
     * @return 返回唯一识别编号
     */
    QUuid getGUID() override {return m_guid;}

    /**
     * @brief getPlugInType 获取此插件的类型
     * @return 插件的类型
     */
    PlugInType getPlugInType() override {return PlugInType::PlugIn_Notice;}

    /**
    * @brief initModule 初始化模块
    * @return true：初始化成功；false:初始化失败
    */
    virtual int initModule() override {return 0;}

    /**
    * @brief disModule 销毁模块
    */
    virtual void disModule() override {}

    /**
     * @brief ThemeChanged 主题改变通知
     */
    virtual void ThemeChanged() override {}

    /**
     * @brief LanguageChanged 语言改变通知
     */
    virtual void LanguageChanged() override {}

public:

    /**
    * @brief notify 实现具体的弹窗消息
    * @param level 弹窗等级
    * @param title 消息标题
    * @param body 消息内容体
    * @param data 消息的其余属性
    */
    virtual void notify(const NotifyLevel &level, const QString &title, const QString &body, QVariantMap data = QVariantMap());

protected:

    static NotifyBase *m_notify;

private:

    /**
     * @brief m_str_GUID 功能组件的唯一识别ID
     */
    QUuid m_guid;

};


#endif // NOTIFYBASE_H
