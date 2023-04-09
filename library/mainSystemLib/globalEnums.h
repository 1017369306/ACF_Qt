#ifndef GLOBALENUMS_H
#define GLOBALENUMS_H

#include <QObject>
#include <QMetaType>
#include <QVariant>

/**
 * @brief The LoggerBaseLevel enum 日志等级
 */
enum LoggerBaseLevel
{
    NULL_LogLevel = 0,
    TRACE_LogLevel,
    DEBUG_LogLevel,
    INFO_LogLevel,
    WARN_LogLevel,
    ERROR_LogLevel,
    FATAL_LogLevel
};
Q_DECLARE_METATYPE(LoggerBaseLevel)

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
Q_DECLARE_METATYPE(NotifyLevel)

/**
 * @brief The LoggerBaseStruct struct 基类的日志结构体
 */
struct LoggerBaseStruct{

public:

    //日志等级
    LoggerBaseLevel level;
    //日志信息
    std::basic_string<char> message;

    LoggerBaseStruct(){}

    LoggerBaseStruct(LoggerBaseLevel level, std::basic_string<char> message)
        : level(level), message(message)
    {
    }
};
Q_DECLARE_METATYPE(LoggerBaseStruct)

/**
 * @brief The NotifyStruct struct 通知消息的结构体
 */
struct NotifyStruct{

public:

    //消息等级
    NotifyLevel level;
    //标题
    QString title;
    //消息内容
    QString body;
    //自定义数据
    QVariantMap data;

    NotifyStruct(){}

    NotifyStruct(NotifyLevel level, QString title, QString body, QVariantMap data = QVariantMap())
        : level(level), title(title), body(body), data(data)
    {
    }
};
Q_DECLARE_METATYPE(NotifyStruct)

#endif // GLOBALENUMS_H
