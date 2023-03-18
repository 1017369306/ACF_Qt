#ifndef GLOBALINFOENUM_H
#define GLOBALINFOENUM_H

#include <QObject>
#include <QHash>
#include <QMetaObject>

class GlobalInfo : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief The ClientDataType enum 客户端的所有字符串信息定义
     */
    enum GlobalInfoEnum
    {
        /**
         * @brief 操作成功
         */
        DealSuccess = 0,
        /**
         * @brief 参数改变
         */
        TaskChanged = 1,
        /**
         * @brief 任务状态不是开始
         */
        TaskIsNotStart = 2,
        /**
         * @brief 任务状态不是停止
         */
        TaskIsNotStop = 3,
        /**
         * @brief 任务状态不是暂停
         */
        TaskIsNotPause = 4,
        /**
         * @brief 任务状态不是开始
         */
        TaskIsStart = 5,
        /**
         * @brief 任务状态不是停止
         */
        TaskIsStop = 6,
        /**
         * @brief 任务状态不是暂停
         */
        TaskIsPause = 7,

        /**
          * 操作失败
          */
        DealFatal = 9999
    };
    Q_ENUM(GlobalInfoEnum);

public:

    static GlobalInfo *instance(){
        if(m_globalInfo == nullptr){
            m_globalInfo = new GlobalInfo();
        }
        return m_globalInfo;
    }

    ~GlobalInfo();

    QString getInfo(const int &value);

private:

    GlobalInfo();

private:

    static GlobalInfo *m_globalInfo;

    QHash<int, QString> m_allInfos;

};

// global object
//extern GlobalInfo globalInfo;

#define INFO(value)			GlobalInfo::instance->getInfo(value)

#endif // GLOBALINFOENUM_H
