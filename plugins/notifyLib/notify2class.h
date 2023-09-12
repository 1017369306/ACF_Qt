#ifndef NOTIFY2CLASS_H
#define NOTIFY2CLASS_H

#include <QObject>
#include <globalInfoEnum.h>
#include <globalEnums.h>
#include <loggerBase.h>
#include <defaultplugin.h>
// 包含头文件
#include "notifymanager.h"

#define def_notifyLevel "level"
#define def_notifyTitle "title"
#define def_notifyBody "body"

class Notify2Class : public DefaultPlugin
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
     * @brief sendData 插件运行起来后，给其发送数据
     * @param data
     */
    virtual void sendData(QVariant data = QVariant::Invalid) override {
        if(data.isValid() && data.canConvert<NotifyStruct>()){
            NotifyStruct notify = data.value<NotifyStruct>();
            // 追加扩展数据
            switch (notify.level) {
            case NotifyLevel::InfoLevel:
            {
                notify.data.insert("icon", "./appPics/svgs/theme/info.svg"); // 自定义消息图标，也可传入QPixmap
                ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::INFO_LogLevel, notify.body.toStdString())));
//                LOG_INFO(notify.body.toStdString());
                break;
            }
            case NotifyLevel::SuccessLevel:
            {
                notify.data.insert("icon", "./appPics/svgs/theme/success.svg"); // 自定义消息图标，也可传入QPixmap
                ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::INFO_LogLevel, notify.body.toStdString())));
//                LOG_INFO(notify.body.toStdString());
                break;
            }
            case NotifyLevel::WarnLevel:
            {
                notify.data.insert("icon", "./appPics/svgs/theme/warn.svg"); // 自定义消息图标，也可传入QPixmap
                ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::WARN_LogLevel, notify.body.toStdString())));
//                LOG_WARN(notify.body.toStdString());
                break;
            }
            case NotifyLevel::ErrorLevel:
            {
                notify.data.insert("icon", "./appPics/svgs/theme/error.svg"); // 自定义消息图标，也可传入QPixmap
                ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::ERROR_LogLevel, notify.body.toStdString())));
//                LOG_ERROR(notify.body.toStdString());
                break;
            }
            case NotifyLevel::FatalLevel:
            {
                notify.data.insert("icon", "./appPics/svgs/theme/fatal.svg"); // 自定义消息图标，也可传入QPixmap
                ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::FATAL_LogLevel, notify.body.toStdString())));
//                LOG_FATAL(notify.body.toStdString());
                break;
            }
            default:{
                notify.data.insert("icon", "./appPics/svgs/theme/trace.svg"); // 自定义消息图标，也可传入QPixmap
                ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::TRACE_LogLevel, notify.body.toStdString())));
//                LOG_TRACE(notify.body.toStdString());
                break;
            }
            }
            if(manager != nullptr){
                //TODO 根据不同等级显示不同的图标
                notify.data.insert(def_notifyLevel, (int)notify.level);
                manager->notify(notify.title, notify.body, notify.data);
            }
        }
    }

     /**
     * @brief disModule 销毁模块
     */
    void disModule() override;

    /**
     * @brief ThemeChanged 主题改变通知
     */
    void cssStyleChanged() override;

    /**
     * @brief LanguageChanged 语言改变通知
     */
    void LanguageChanged() override {}

private:

    void slot_notifyDetail(const QVariantMap &data);

private:

    NotifyManager *manager = nullptr;

};

#endif // NOTIFY2CLASS_H
