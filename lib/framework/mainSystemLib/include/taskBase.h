#ifndef TASKBASE_H
#define TASKBASE_H

#include "mainSystemLib_global.h"
#include <QObject>
#include <ITask.h>
#include <globalInfoEnum.h>

/**
 * @brief The taskBase class 任务基类
 * 作者 张洋
 * 日期 2022-12-04
 */
class MAINSYSTEMLIB_EXPORT taskBase : public ITask
{
public:
    /**
    * @brief Task 构造函数
    * @param funcinfo 任务的类型
    * @param parent 所属的管理上级
    */
    explicit taskBase();
    virtual ~taskBase();

    //接口属性定义

    /**
    * @brief getGUID 获取唯一识别编号
    * @return 返回唯一识别编号
    */
    QUuid getGUID() override {return m_guid;}

    /**
    * @brief getParam 获取功能参数
    * @return 返回功能参数
    */
    QVariant getTaskParam() override {return m_taskParam;}

    /**
    * @brief getStartTime 获取任务开始的时间
    * @return 任务开始的时间
    */
    QDateTime getStartTime() override {return m_taskStartTime;}

    /**
    * @brief getGUID 获取任务的执行状态,true:正在执行任务
    * @return 任务的执行状态
    */
    TaskStatus getTaskState() override {return m_taskStatus;}

//    TaskType getTaskType() override {return m_type;}

    //接口方法定义
    /**
    * @brief runTask 开始任务
    * @return
    */
    int startTask() override;

    /**
    * @brief stopTask 停止任务
    * @return
    */
    int stopTask(const StopReason &stopReason)  override;

    /**
    * @brief pauseTask 暂停任务
    * @return
    */
    int pauseTask() override;

    /**
    * @brief continueTask 继续任务
    * @return
    */
    int continueTask() override;

    /**
    * @brief otherOrdor 其他任务命令
    * @param type 任务命令项
    * @param tag 自定义参数
    * @return
    */
    int otherOrder(const CommandType &type, const QVariant &tag) override;

protected:

    /**
    * @brief starting
    * @return
    */
    virtual int starting()=0;
    /**
    * @brief started
    * @return
    */
    virtual void started()=0;
    /**
    * @brief stoping
    * @return
    */
    virtual int stoping()=0;
    /**
    * @brief stoped
    * @return
    */
    virtual void stoped()=0;
    /**
    * @brief pausing
    * @return
    */
    virtual int pausing()=0;
    /**
    * @brief paused
    * @return
    */
    virtual void paused()=0;
    /**
    * @brief continuing
    * @return
    */
    virtual int continuing()=0;
    /**
    * @brief continued
    * @return
    */
    virtual void continued()=0;

    virtual int otherOrdering(const CommandType &type, const QVariant &tag) = 0;

    virtual int otherOrdered(const CommandType &type, const QVariant &tag) = 0;

private:

    QUuid m_guid;

    TaskStatus m_taskStatus = TaskStatus::Stoped;

    QDateTime m_taskStartTime;

    QVariant m_taskParam;

};


#endif // TASKBASE_H
