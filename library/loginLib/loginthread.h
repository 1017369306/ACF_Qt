#ifndef LOADTHREAD_H
#define LOADTHREAD_H

#include <QObject>
#include <QThread>

#include "loginLib_global.h"
#include "logintool.h"

/**
 * @brief 登录过程中的详细进度更新线程
 */
class LOGINLIB_EXPORT LoginThread : public QThread
{
    Q_OBJECT
public:
    Q_PROPERTY(int stepMs READ getStepMs WRITE setStepMs)

    /**
     * @brief 获取计时器间隔时间，单位：ms；默认50ms
     * @return
     */
    int getStepMs() const {return m_stepMs;}
    /**
     * @brief 设置计时器间隔时间，单位：ms
     * @param 间隔时间，单位：ms
     */
    void setStepMs(const int& value){m_stepMs = value;}

    /**
     * @brief 获取当前的详细进度值（0-100）
     * @return
     */
    int getCurrentProcess() const {return m_currentProcess;}

    /**
     * @brief 获取里程碑进度（0-100）
     * @return
     */
    int getMilestoneProcess() const {return m_milestoneProcess;}

public:
    LoginThread();
    ~LoginThread();

public:
signals:
    /**
     * @brief 当前的详细进度值改变信号
     * @param value
     */
    void currentProcessChanged(const int& value);

protected:
    void run() override;

private slots:
    void slot_milestoneProcessChanged(const int& value);

private:
    void setCurrentProcess(const int& value);

private:
    int m_stepMs = 50;
    int m_currentProcess = 0;
    int m_milestoneProcess = 0;

};

#endif // LOADTHREAD_H
