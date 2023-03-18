#ifndef NOTIFY2CLASS_H
#define NOTIFY2CLASS_H

#include <QObject>
#include <notifybase.h>
// 包含头文件
#include "notifymanager.h"

class Notify2Class : public NotifyBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "notifyLib.json")
    Q_INTERFACES(IPlugIn)
public:
    Notify2Class();
    ~Notify2Class();

     /**
     * @brief initModule 初始化模块
     * @return true：初始化成功；false:初始化失败
     */
    int initModule() override;

     /**
     * @brief disModule 销毁模块
     */
    void disModule() override;

    /**
    * @brief notify 实现具体的弹窗消息
    * @param level 弹窗等级
    * @param title 消息标题
    * @param body 消息内容体
    * @param data 消息的其余属性
    */
    void notify(const NotifyLevel &level, const QString &title, const QString &body, QVariantMap data = QVariantMap()) override;

    IPlugIn *createNewPlugin() override {return this;}

    QWidget *getWidget(QWidget *parent = nullptr) override {return nullptr;}

private:

    void slot_notifyDetail(const QVariantMap &data);

private:

    NotifyManager *manager = nullptr;

};

#endif // NOTIFY2CLASS_H
