#ifndef WINBREAKPAD_H
#define WINBREAKPAD_H

#pragma once

#if defined(_WIN64)

#include <Windows.h>
#include "IHook.h"
//#include <ntsecapi.h>
#include <TlHelp32.h>
//#include <tchar.h>

typedef struct tagWNDINFO
{
    DWORD dwProcessId;
    HWND hWnd;
} WNDINFO, *LPWNDINFO;

#define SHIFT_STR "Shift"
#define CTRL_STR "Ctrl"
#define ALT_STR "Alt"

/**
 * @brief windows平台下的用户行为记录类（普通非低级鼠标键盘钩子，需要注入）。普通的低级键盘钩子可以监控所有窗口的按键消息，对于qq登陆界面-qq密码框会触发保护，只要光标停留在qq的密码框，就随机返回一些乱码来保护密码
 *
 * 代码使用方式如下：
 */
class HOOKPLUS_EXPORT WinHook : public IHook
{
public:
    explicit WinHook();
    ~WinHook();

    static WinHook *instance() {
        if(!m_winHook){
            m_winHook = new WinHook();
        }
        return m_winHook;
    }

    vector<HookModel> getModels() const{
        return m_models;
    }

    /**
     * @brief 安装钩子函数
     * @param model 钩子类型
     */
    bool installHook(vector<HookModel> models) override;

    /**
     * @brief 卸载钩子
     * @param model 钩子类型
     */
    void unInstallHook(vector<HookModel> models) override;

//    /**
//     * @brief 提高钩子dll的权限
//     * @return
//     */
//    bool AddDebugPrivilege();

    /**
     * @brief 远程线程注入
     * @param 远程进程的PID
     * @param 注入dll的全路径
     * @return
     */
    bool RemoteInject(DWORD _Pid, LPCWSTR DllName);

    /**
     * @brief 钩子消息函数，系统消息队列信息会返回到该函数中
     * @param nCode
     * @param wParam
     * @param lParam
     * @return
     */
    LRESULT CALLBACK keyHookHandle(int nCode, WPARAM wParam, LPARAM lParam);

    /**
     * @brief 钩子消息函数，系统消息队列信息会返回到该函数中
     * @param hook 钩子对象
     * @param nCode
     * @param wParam
     * @param lParam
     * @return
     */
    LRESULT CALLBACK hookHandle(HHOOK hook, int nCode, WPARAM wParam, LPARAM lParam);

    /**
     * @brief 通过进程ID获取进程句柄（PROCESS_ALL_ACCESS 获取所有权限，PROCESS_QUERY_INFORMATION 获取进程的令牌、退出码和优先级等信息）
     * @param 进程ID
     * @return
     */
    static HANDLE GetProcessHandle(int nID);

    /**
    * @brief 通过进程快照获取PID（未打开进程获取句柄）
    * @param 程序名，如：test.exe
    * @return
    */
    static DWORD GetProcessPID(LPCWSTR lpProcessName);

    /**
     * @brief 获取特殊码的状态（指示灯开启、处于按下状态？）
     * @return
     */
    static SpecialCodeStatus getSpecialCode();

    /**
     * @brief 识别按键是否大小写或特殊字符
     * @param code 按键虚拟码,即键盘上每个按键对应一个虚拟码，不区分大小写，微软官方文档：https://docs.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
     * @param caps 是否按下大写按键，TRUE代表按下
     * @param shift 是否按住shift按键，TRUE代表正按住
     * @return 函数返回的按键字符
     */
    static std::string HookCode(DWORD code, BOOL caps, BOOL shiftOn, BOOL shiftDown);

    /**
     * @brief 枚举所有进程
     * @param hWnd
     * @param lParam
     * @return
     */
    static BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam)
    {
        DWORD dwProcId;
        GetWindowThreadProcessId(hWnd, &dwProcId);
        LPWNDINFO pInfo = (LPWNDINFO)lParam;
        if(dwProcId == pInfo->dwProcessId)
        {
            pInfo->hWnd = hWnd;
            return FALSE;
        }
        return TRUE;
    }

    /**
     * @brief 获取给定进程ID的窗口handle
     * @param dwProcessId
     * @return
     */
    static HWND GetProcessMainWnd(DWORD dwProcessId)
    {
        WNDINFO wi;
        wi.dwProcessId = dwProcessId;
        wi.hWnd = NULL;
        EnumWindows(WinHook::MyEnumProc, (LPARAM)&wi);
        return wi.hWnd;
    }

private:

    /**
     * @brief OnHook 已过时
     */
    attribute_deprecated  void OnHook();

    /**
     * @brief OnHook 已过时
     */
    attribute_deprecated  void OnUnHook();

private:

    static WinHook *m_winHook;

    vector<HookModel> m_models;

    //用户行为记录的完整信息
    BehaviorInfo m_hookInfo;

};

#endif

#endif // WINBREAKPAD_H
