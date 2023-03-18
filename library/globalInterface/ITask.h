#ifndef ITASK_H
#define ITASK_H

#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QDateTime>

/**
 * @brief The TaskStatus 任务状态
 */
enum TaskStatus
{
    Stoped = 0,
    Started,
    Paused
};

/**
 * @brief The StopReason
 */
enum StopReason
{
    //用户点击停止
    ClickByUser = 0,
    //监测站已经下线
    StationDown,
    //数据库处理出现错误
    DBError,
    //绘制/统计数据出现错误
    ShowDataError,
    //测量窗口将被关闭
    WindowClosing,
    //计时器到期，自动停止
    TimedEnd,
    //服务端错误
    ServerThrow,
    //服务端退出addCallBack
    ServerClose,
    //主任务已经停止
    MainTaskStop,
    //临时停止
    HoldOn,
    //RePlay完成
    RePlayCompleted
};

/**
 * @brief The CommandType enum 命令项定义
 */
enum CommandType
{
    Unknown = 0,
    /**
     * 开始测量
    */
    Measure_Run,
    /**
     * 停止测量
    */
    Measure_Stop,
    /**
     * 开始测量
    */
    Measure_Pause,
    /**
     * 停止测量
    */
    Measure_Continue,
    /**
     * 截图保存
    */
    ScreenShot,
    /**
     * 清空重置
    */
    ClearReset,
    /**
    * 改变任务参数
    */
    ChangeParam
};


/**
 * @brief The ITask class 定义测量任务接口
 * 作者 张洋
 * 创建日期 2022-12-04
 */
class ITask
{
  //接口属性定义
 public:
    virtual ~ITask(){}

    /**
    * @brief getGUID 获取唯一识别编号
    * @return 返回唯一识别编号
    */
    virtual QUuid getGUID()=0;

    /**
    * @brief getTaskParam 获取功能参数
    * @return 返回功能参数
    */
    virtual QVariant getTaskParam()=0;

    /**
    * @brief getStartTime 获取任务开始的时间
    * @return 任务开始的时间
    */
    virtual QDateTime getStartTime()=0;

    /**
    * @brief getTaskState 获取任务的运行状态
    * @return 任务的运行状态
    */
    virtual TaskStatus getTaskState()=0;

//    virtual TaskType getTaskType() = 0;

    //接口方法定义
public:

    /**
     * @brief startTask 开始任务
     * @return
     */
    virtual int startTask()=0;

    /**
     * @brief stopTask 停止任务
     * @return
     */
    virtual int stopTask(const StopReason &stopReason)=0;

    /**
     * @brief pauseTask 暂停任务
     * @return
     */
    virtual int pauseTask()=0;

    /**
     * @brief continueTask 继续任务
     * @return
     */
    virtual int continueTask()=0;

    /**
     * @brief otherOrdor 其他任务命令
     * @param type 任务命令项
     * @param tag 自定义参数
     * @return
     */
    virtual int otherOrder(const CommandType &type, const QVariant &tag)=0;

};


#endif // ITASK_H
