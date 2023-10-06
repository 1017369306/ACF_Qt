#ifndef LOADTOOL_H
#define LOADTOOL_H

#include <QObject>

#include "loginLib_global.h"

/**
 * @brief 登录的帮助类（单例）
 */
class LOGINLIB_EXPORT LoginTool : public QObject
{
    Q_OBJECT

public:

    static LoginTool *instance(){
        if(m_instance == nullptr)
            m_instance = new LoginTool();
        return m_instance;
    }

    /**
     * @brief 获取当前的详细进度值（0-100）
     * @return
     */
    int getCurrentProcess() const {return m_currentProcess;}
    /**
     * @brief 设置当前的详细进度值
     * @param 进度
     */
    void setCurrentProcess(const int& value){
        if(value > m_currentProcess){
            m_currentProcess = value;
            emit currentProcessChanged(m_currentProcess);
        }
    }

    /**
     * @brief 获取里程碑进度（0-100）
     * @return
     */
    int getMilestoneProcess() const {return m_milestoneProcess;}
    /**
     * @brief 设置里程碑进度（0-100）
     * @param 进度
     */
    void setMilestoneProcess(const int& value){
        if(value > m_milestoneProcess){
            m_milestoneProcess = value;
            emit milestoneProcessChanged(m_milestoneProcess);
        }
    }

    /**
     * @brief 重置当前的进度
     */
    void resetProcess(){
        m_currentProcess = 0;
        m_milestoneProcess = 0;
    }

//信号
public:
Q_SIGNALS:

    /**
     * @brief 当前的详细进度值改变信号
     */
    void currentProcessChanged(const int& value);

    /**
     * @brief 里程碑进度改变信号
     */
    void milestoneProcessChanged(const int& value);

private:

    LoginTool();

private:

    static LoginTool *m_instance;

    int m_currentProcess = 0;
    int m_milestoneProcess = 0;

};

#endif // LOADTOOL_H
