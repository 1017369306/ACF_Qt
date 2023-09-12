#ifndef WINBREAKPAD_H
#define WINBREAKPAD_H

#pragma once

#if defined(_WIN64)

#include "breakpadPlus_global.h"
#include <Windows.h>
#include "IBreakpad.h"

/**
 * @brief windows平台下的异常分析、定位类，代码使用方式如下：
 * 1、IBreakpad *breakpad = new winBreakpad(appPath.toStdString());
 * 2、breakpad->instance()->setExceptionCrashedCallBack(std::bind(onExceptionCrashed, std::placeholders::_1));
 * 3、把addr2line.exe拷贝到主程序运行目录下
 *
 * 有以下几点情况时，CRT 不会通知被注册的 Unhandled Exception Filter：
 * 1、调用了 abort() 并且设置 abort 的行为为 _CALL_REPORTFAULT（Release 版本默认使用此设置）
 * 2、Security Checks 失败时，具体来说就是检查到一些会引发安全问题的堆栈溢出时不会通知被注册的 Unhandled Exception Filter，
 * 会引发安全问题的堆栈溢出包括：覆盖了函数的返回值，覆盖了 Exception handler 的地址，覆盖了某些类型的参数。关于编译器的
 * Security Checks 的内容，详细参考：http://msdn.microsoft.com/en-us/library/Aa290051（注意，此文章谈到的是
 * Visual Studio .NET 2003，其中 _set_security_error_handler 函数在 VC++2005 以及以上版本已经无法使用）
 * 3、如果没有调用 _set_invalid_parameter_handler 设置 Invalid parameter handler 时，检查到了非法的参数
 */
class BREAKPADPLUS_EXPORT winBreakpad : public IBreakpad
{
public:
    explicit winBreakpad(std::string appPath);
    ~winBreakpad();

    IBreakpad *instance() override {
//        if(m_winBreakpad.getAppPath().length() <= 0){
//            m_winBreakpad = winBreakpad();
//        }
        return &m_winBreakpad;
    }

public:

    /**
     * @brief TrimString 截取最后面的文件名("\\"方式 )
     * @param psz
     * @return
     */
    static char* TrimString(char* psz){
        char szTmp[512] = { 0 };
        char* pszDot = strrchr(psz, '\\');
        if (pszDot)
        {
            pszDot++;   // Advance past the '\\'
            strcpy(szTmp, pszDot);
            ZeroMemory(psz, strlen(psz));
            strcpy(psz, szTmp);
        }
        return psz;
    }

    /**
     * @brief formatString 可变参数的格式化函数，依次对每个参数进行格式化
     * @param format 例如：%d %f
     * @return
     */
    static std::string formatString(const char *format, ...){
        char buffer[1024] = "";
        int retValue;
        va_list argptr;

        va_start(argptr, format);
        retValue = vsprintf(buffer, format, argptr);
        va_end(argptr);

        return std::string(buffer);
    }

    /**
     * @brief ExceptionFilter 异常触发时的第一入口函数（必须为静态）
     * @param e
     * @return
     */
    static LONG ExceptionFilter(LPEXCEPTION_POINTERS e);

private:

    /**
     * @brief createDump 生成dump文件
     * @param e
     * @return
     */
    long __stdcall createDump(LPEXCEPTION_POINTERS e);

    /**
     * @brief HandleException 处理异常的主函数
     * @param pExceptionInfo
     * @return
     */
    LONG HandleException(LPEXCEPTION_POINTERS pExceptionInfo);

    /**
     * @brief GenerateExceptionReport 获取异常头信息
     * @param pExceptionInfo
     */
    void GenerateExceptionReport(LPEXCEPTION_POINTERS pExceptionInfo);

    const char* GetExceptionString(DWORD dwCode);

    /**
     * @brief TraceCallStack 跟踪每一个异常地址对应的原始代码信息
     * @param pContext
     */
    void TraceCallStack(CONTEXT* pContext);

    /**
     * @brief PrintTraceLog 打印每一个异常地址解析出的代码信息
     * @param format
     * @return
     */
    int PrintTraceLog(const char * format, ...);

    /**
     * @brief runCmd cmd方式运行指令
     * @param command 指令
     * @param info 管道输出的信息
     * @return
     */
    bool runCmd(std::string command, std::string &info);

    /**
     * @brief WChar2ANSI 解决了项目放在中文目录下，通过SymGetLineFromAddr64无法正确地查看调用堆栈的字符串内容
     * @param szDest
     * @param nMaxDestSize
     * @param szSrc
     */
    void WChar2ANSI(char* szDest, size_t nMaxDestSize, const PWSTR szSrc);

    /**
     * @brief DisableSetUnhandledExceptionFilter 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效
     * 但是调用后发现程序要崩溃，根本运行不起来
     */
    void DisableSetUnhandledExceptionFilter(){
        void * addr = (void*)GetProcAddress(LoadLibraryA("kernel32.dll"),"SetUnhandledExceptionFilter");
        if (addr)
        {
            unsigned char code[16];
            int size = 0;
            code[size++] = 0x33;
            code[size++] = 0xC0;
            code[size++] = 0xC2;
            code[size++] = 0x04;
            code[size++] = 0x00;

            DWORD oldFlag, tmpFlag;
            VirtualProtect(addr, size, PAGE_READWRITE, &oldFlag);
            WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
            VirtualProtect(addr, size, oldFlag, &tmpFlag);
        }
    }

private:

    //注意：这里不能用指针，否则CRT 不会通知被注册的 Unhandled Exception Filter
//    static winBreakpad *m_winBreakpad;

    static winBreakpad m_winBreakpad;

    static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;

    // Machine type matters when trace the call stack (StackWalk64)
    DWORD m_dwMachineType;
    HANDLE m_hReportFile;
    TCHAR m_szLogFileName[MAX_PATH];

    //分析、定位出来的完整异常信息
    ExceptionInfo m_exceptionInfo;

};

#endif

#endif // WINBREAKPAD_H
