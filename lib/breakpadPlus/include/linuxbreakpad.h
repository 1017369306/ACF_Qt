#ifndef LINUXBREAKPAD_H
#define LINUXBREAKPAD_H

#if defined(__linux__)

#include "breakpadPlus_global.h"
#include "IBreakpad.h"
#include <signal.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>
#include <vector>
#include <dlfcn.h>
#include <execinfo.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

class BREAKPADPLUS_EXPORT LinuxBreakpad : public IBreakpad
{
public:
    explicit LinuxBreakpad(std::string appPath);
    ~LinuxBreakpad();

    IBreakpad *instance() override {
        return &m_linuxBreakpad;
    }

    static void ExceptionFilter(int sig);

private:

    /**
     * @brief HandleException 处理异常的主函数
     * @param sig
     * @return
     */
    void HandleException(int sig);

    /**
     * @brief TraceCallStack 跟踪每一个异常地址对应的原始代码信息
     * @param stack
     * @param traces
     * @param size
     */
    void TraceCallStack(void **stack, char **traces, int size);

    bool findAddr(std::string str, std::string& filePath, std::string& functionName, std::string& functionOffset);

    bool getAddress(const void *stackTrace, std::string& filePath, std::string& functionOffset, std::string& address, std::string& error);

    std::string getCodeLineByAddr2Line(const std::string& filePath, const std::string& address, std::string& error);

private:

    //注意：这里不能用指针，否则CRT 不会通知被注册的 Unhandled Exception Filter
    //    static winBreakpad *m_winBreakpad;

    static LinuxBreakpad m_linuxBreakpad;

    //分析、定位出来的完整异常信息
    ExceptionInfo m_exceptionInfo;

};

#endif

#endif // LINUXBREAKPAD_H
