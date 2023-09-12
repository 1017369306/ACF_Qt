#ifndef ACFPROPERTY_H
#define ACFPROPERTY_H

#include "mainSystemLib_global.h"
#include <IPlugIn.h>
#include <QTextEdit>
#include <globalEnums.h>

#define LogPlugin_Name "log"
#define NotifyPlugin_Name "notify"

/**
 * @brief 框架级公共参数
 */
class MAINSYSTEMLIB_EXPORT ACFProperty : public QObject
{
    Q_OBJECT
public:
    explicit ACFProperty();
    ~ACFProperty();

    static ACFProperty *instance(){
        if(m_instance == nullptr)
            m_instance = new ACFProperty();
        return m_instance;
    }

    /**
     * @brief 获取系统的日志等级
     * @return
     */
    LoggerBaseLevel getLogLevel() const {
        return m_logLevel;
    }

    /**
     * @brief 获取日志插件
     * @return
     */
    IPlugIn *getLogPlugIn() const {
        return m_logPlugin;
    }

    /**
     * @brief 获取消息弹窗插件
     * @return
     */
    IPlugIn *getNotifyPlugIn() const {
        return m_notifyPlugin;
    }

    QTextEdit *getOutputText() const {
        return m_outputText;
    }

    void setOutputText(QTextEdit *edit) {
        m_outputText = edit;
    }

private:

    void slot_pluginStatusChanged(const PlugInProperty& property, const bool& isLoad);

private:

    static ACFProperty *m_instance;

    LoggerBaseLevel m_logLevel = LoggerBaseLevel::TRACE_LogLevel;

    IPlugIn *m_logPlugin = nullptr;
    IPlugIn *m_notifyPlugin = nullptr;

    //日志输出的容器
    QTextEdit *m_outputText = nullptr;

};

#endif // ACFPROPERTY_H
