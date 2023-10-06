#include "loginthread.h"

LoginThread::LoginThread()
{
    QObject::connect(LoginTool::instance(), &LoginTool::currentProcessChanged, this, &LoginThread::setCurrentProcess);
    QObject::connect(LoginTool::instance(), &LoginTool::milestoneProcessChanged, this, &LoginThread::slot_milestoneProcessChanged);

}

LoginThread::~LoginThread()
{
    quit();
}

void LoginThread::setCurrentProcess(const int& value){
    if(value > m_currentProcess){
        m_currentProcess = value;
        if(value > m_milestoneProcess){
            m_currentProcess = m_milestoneProcess;
        }

        if(m_currentProcess >= 100){
            quit();
        }

        emit currentProcessChanged(m_currentProcess);
    }
}

void LoginThread::slot_milestoneProcessChanged(const int& value){
    m_milestoneProcess = value;
}

void LoginThread::run(){
    m_currentProcess = 0;
    m_milestoneProcess = 0;

    while (isRunning()) {
        //是否可以显示下一步加载的信息
        bool canNext = m_milestoneProcess > m_currentProcess;
        if (canNext)
        {
            setCurrentProcess(m_currentProcess + 1);

            if(m_currentProcess >= 100){
                break;
            }
            else{
                QThread::msleep(m_stepMs);
            }
        }
        else{
            QThread::msleep(m_stepMs);
        }
    }
}
