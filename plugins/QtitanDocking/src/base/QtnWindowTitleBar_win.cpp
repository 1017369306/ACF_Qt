/****************************************************************************
**
** Qtitan Library by Developer Machines (Components for Qt.C++)
** 
** Copyright (c) 2009-2021 Developer Machines (https://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/

#include <QThread>
#include <qevent.h>
#include <QApplication>

#include "QtnStyleOption.h"
#include "QtnWindowTitleBarGlow_win.h"
#include "QtnWindowTitleBar_win.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#ifdef QTN_PRIVATE_HEADERS
#include <qpa/qplatformwindow_p.h>
using namespace QNativeInterface::Private;
#endif
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
Q_DECLARE_METATYPE(QMargins)
#endif

QTITAN_USE_NAMESPACE

/* NOT USED
static Qt::MouseButton qtn_get_button_from_message(UINT messageId)
{
    const bool mouseSwapped = GetSystemMetrics(SM_SWAPBUTTON);
    Qt::MouseButton result = Qt::NoButton;
    switch (messageId)
    {
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
        case WM_NCLBUTTONDBLCLK:
        case WM_LBUTTONUP:
            result = mouseSwapped ? Qt::RightButton : Qt::LeftButton;
            break;
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_NCRBUTTONDBLCLK:
        case WM_RBUTTONUP:
            result = mouseSwapped ? Qt::LeftButton : Qt::RightButton;
            break;
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        case WM_NCMBUTTONDBLCLK:
        case WM_MBUTTONUP:
            result = Qt::MidButton;
            break;
        default:
            break;
    }
    return result;
}
*/
static Qt::MouseButtons qtn_state_mouse_buttons()
{
    Qt::MouseButtons result = Qt::MouseButtons();
    const bool mouseSwapped = GetSystemMetrics(SM_SWAPBUTTON);
    if (GetAsyncKeyState(VK_LBUTTON) < 0)
        result |= mouseSwapped ? Qt::RightButton : Qt::LeftButton;
    if (GetAsyncKeyState(VK_RBUTTON) < 0)
        result |= mouseSwapped ? Qt::LeftButton : Qt::RightButton;
    if (GetAsyncKeyState(VK_MBUTTON) < 0)
        result |= Qt::MiddleButton;
    if (GetAsyncKeyState(VK_XBUTTON1) < 0)
        result |= Qt::XButton1;
    if (GetAsyncKeyState(VK_XBUTTON2) < 0)
        result |= Qt::XButton2;
    return result;
}
/* NOT USED
static Qt::KeyboardModifiers qtn_state_keyboard_modifiers()
{
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    if (GetKeyState(VK_SHIFT) < 0)
        modifiers |= Qt::ShiftModifier;
    if (GetKeyState(VK_CONTROL) < 0)
        modifiers |= Qt::ControlModifier;
    if (GetKeyState(VK_MENU) < 0)
        modifiers |= Qt::AltModifier;
    if (GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0)
        modifiers |= Qt::MetaModifier;
    return modifiers;
}
*/
/* NOT USED
static UINT qtn_nonClientToClientMouseDownMessage(UINT message)
{
    if (message == WM_NCLBUTTONDOWN || message == WM_LBUTTONDOWN)
        return WM_LBUTTONDOWN;
    else if (message == WM_NCRBUTTONDOWN || message == WM_RBUTTONDOWN)
        return WM_RBUTTONDOWN;
    else if (message == WM_NCMBUTTONDOWN || message == WM_MBUTTONDOWN)
        return WM_MBUTTONDOWN;
    else if (message == WM_NCLBUTTONDBLCLK || message == WM_LBUTTONDBLCLK)
        return WM_LBUTTONDBLCLK;
    else if (message == WM_NCRBUTTONDBLCLK || message == WM_RBUTTONDBLCLK)
        return WM_RBUTTONDBLCLK;
    else if (message == WM_NCMBUTTONDBLCLK || message == WM_MBUTTONDBLCLK)
        return WM_MBUTTONDBLCLK;
#ifdef QTN_WMPOINTER_SUPPORTS
    else if (message == WM_NCPOINTERDOWN || message == WM_POINTERDOWN)
        return WM_POINTERDOWN;
#endif
    Q_ASSERT(false);
    return 0;
}
*/
/* NOT USED
static UINT qtn_nonClientToClientMouseUpMessage(UINT message)
{
    if (message == WM_NCLBUTTONUP || message == WM_LBUTTONUP)
        return WM_LBUTTONUP;
    else if (message == WM_NCRBUTTONUP || message == WM_RBUTTONUP)
        return WM_RBUTTONUP;
    else if (message == WM_NCMBUTTONUP || message == WM_MBUTTONUP)
        return WM_MBUTTONUP;
#ifdef QTN_WMPOINTER_SUPPORTS
    else if (message == WM_NCPOINTERUP || message == WM_POINTERUP)
        return WM_POINTERUP;
#endif
    Q_ASSERT(false);
    return 0;
}
*/
/* NOT USED
static WPARAM qtn_mouseDownKeyState(UINT message)
{
    WPARAM result = 0;
    if (message == WM_LBUTTONDOWN)
        result = MK_LBUTTON;
    else if (message == WM_RBUTTONDOWN)
        result = MK_RBUTTON;
    else if (message == WM_MBUTTONDOWN)
        result = MK_MBUTTON;
    else if (message == WM_LBUTTONDBLCLK)
        result = MK_LBUTTON;
    else if (message == WM_RBUTTONDBLCLK)
        result = MK_RBUTTON;
    else if (message == WM_MBUTTONDBLCLK)
        result = MK_MBUTTON;
    Q_ASSERT(result != 0);
    return result;
}
*/
/* NOT USED
static UINT qtn_mouseUpForMouseDownMessage(UINT message)
{
    if (message == WM_LBUTTONDOWN)
        return WM_LBUTTONUP;
    else if (message == WM_RBUTTONDOWN)
        return WM_RBUTTONUP;
    else if (message == WM_MBUTTONDOWN)
        return WM_MBUTTONUP;
    else if (message == WM_LBUTTONDBLCLK)
        return WM_LBUTTONUP;
    else if (message == WM_RBUTTONDBLCLK)
        return WM_RBUTTONUP;
    else if (message == WM_MBUTTONDBLCLK)
        return WM_MBUTTONUP;
    Q_ASSERT(false);
    return 0;
}
*/
/* NOT USED
static WPARAM qtn_createClientMouseWparam(const Qt::MouseButtons& buttons, Qt::KeyboardModifiers& modifiers)
{
    WPARAM result = 0;
    if (buttons & Qt::LeftButton)
        result |= MK_LBUTTON;
    if (buttons & Qt::RightButton)
        result |= MK_RBUTTON;
    if (buttons & Qt::MiddleButton)
        result |= MK_MBUTTON;
    if (buttons & Qt::XButton1)
        result |= MK_XBUTTON1;
    if (buttons & Qt::XButton2)
        result |= MK_XBUTTON2;
    if (modifiers & Qt::ShiftModifier)
        result |= MK_SHIFT;
    if (buttons & Qt::ControlModifier)
        result |= MK_CONTROL;
    return result;
}
*/
#ifdef QTN_WMPOINTER_SUPPORTS
static WPARAM qtn_createClientPointerWparam(int wid)
{
    POINTER_INFO pi = { NULL };
    GetPointerInfo(wid, &pi);
    const WORD flags = LOWORD(pi.pointerFlags);
    return MAKEWPARAM(wid, flags);
}
#endif

/* WindowTitleBarPrivateWin */
WindowTitleBarPrivateWin::WindowTitleBarPrivateWin()
    : m_isActive(true)
    , m_isFinalized(true)
    , m_aeroGlassEnabled(false)
    , m_lockStyleChange(false)
    , m_fixupQtNeeded(false)
    , m_mouseTracking(false)
    , m_fillClientArea(false)
    , m_glowManager(new WindowTitleBarGlowManager(this))
    , m_ignoreSysCommand(false)
    , m_sysCommandStatus(false)
    , m_sysCommand(-1)
{
    qtn_initializeDrawingResources();
    m_aeroGlassEnabled = isCompositionEnabled();
}

WindowTitleBarPrivateWin::~WindowTitleBarPrivateWin()
{
    setWindow(Q_NULL);
    Q_DELETE_AND_NULL(m_glowManager);
}

void WindowTitleBarPrivateWin::update(const QRect& rect)
{
    if (extendViewIntoTitleBar())
        WindowTitleBarPrivate::update(rect);
    else
    {
        //Update non-client area...
        QRect qr = rect.translated(QPoint(-frameMargins().left(), -frameMargins().top()));
        qr = qtn_toNativeDPI(qr, window(), true);
        RECT r;
        SetRect(&r, qr.left(), qr.top(), qr.right() + 1, qr.bottom() + 1);
        RedrawWindow(winId(), &r, NULL, RDW_FRAME | RDW_INVALIDATE);
    }
}

void WindowTitleBarPrivateWin::update(const QRegion& region)
{
    if (extendViewIntoTitleBar())
        WindowTitleBarPrivate::update(region);
    else
    {
        //Update non-client area...
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        HRGN hrgn = region.isNull() ? Q_NULL : 
            qtn_create_region(qtn_toNativeDPI(
                region.translated(QPoint(-frameMargins().left(), -frameMargins().top())), window(), true));
#else
        HRGN hrgn = region.isEmpty() ? Q_NULL : 
            qtn_create_region(region.translated(QPoint(-frameMargins().left(), -frameMargins().top())));
#endif
        RedrawWindow(winId(), NULL, hrgn, RDW_FRAME | RDW_INVALIDATE);
        DeleteObject(hrgn);
    }
}

bool WindowTitleBarPrivateWin::isActive() const
{
    return m_isActive;
}

LRESULT WindowTitleBarPrivateWin::winProc(HWND hWnd, uint nMessage, WPARAM wParam, LPARAM lParam)
{
    Q_ASSERT(hWnd == winId());
    MSG curMsg;
    curMsg.hwnd = hWnd;
    curMsg.message = nMessage;
    curMsg.wParam = wParam;
    curMsg.lParam = lParam;
    LRESULT lResult = 0;
    if (handleMessage(&curMsg, lResult))
        return lResult;
    return WindowsHook::winProc(hWnd, nMessage, wParam, lParam);
}

void WindowTitleBarPrivateWin::winEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    WindowsHook::winEventProc(hWinEventHook, event, hWnd, idObject, idChild, dwEventThread, dwmsEventTime);
}

bool WindowTitleBarPrivateWin::handleNonClientCreate()
{
    initializeWindowUI();
    return false; 
}

bool WindowTitleBarPrivateWin::handleNonClientDestroy()
{
    return false; 
}

bool WindowTitleBarPrivateWin::handleNonClientPaint(HRGN updateRegion)
{
    if (frameMargins().isNull())
        return true;

    DWORD flags = DCX_WINDOW | DCX_USESTYLE;
    HDC hdc = updateRegion == (HRGN)1 ? GetDCEx(winId(), 0, flags) : GetDCEx(winId(), updateRegion, flags | DCX_INTERSECTRGN);
    Q_ASSERT(hdc != 0);
    paintNonClientArea(hdc);
    ReleaseDC(winId(), hdc);
    return true;
}

bool WindowTitleBarPrivateWin::updateDwmSettings(bool enabled)
{
    if (pDwmExtendFrameIntoClientArea == Q_NULL || pDwmSetWindowAttribute == Q_NULL)
        return false;

    //Negative values extends the aero glass to all non-client area.
    QMargins margins(0, 0, 0, 0);
    if (enabled)
    {
        QMargins nativeFrame = qtn_getWindowFrameMargings(window());
        if (!styledFrame() /*|| !blurBehindWindowEnabled()*/)
        {
            if (isMaximized())
                margins = QMargins(0, nativeFrame.left()/* 0 */, 0, 0);
        }
        else
        {
#if 0
            if (qtn_getWindows10AcrylicSupport())
                margins = QMargins(0, nativeFrame.left()/* 0 */, 0, 0);
            else
            {
                margins = QMargins(-1, -1, -1, -1); //Extend to the full window size
            }
#endif
        }
    }

    HLP_MARGINS hlpMargins;
    hlpMargins.cxLeftWidth = margins.left();
    hlpMargins.cyTopHeight = margins.top();
    hlpMargins.cxRightWidth = margins.right();
    hlpMargins.cyBottomHeight = margins.bottom();
    HRESULT res = pDwmExtendFrameIntoClientArea(winId(), &hlpMargins);

    BOOL value = enabled;
    res = pDwmSetWindowAttribute(winId(), DWMWA_ALLOW_NCPAINT, &value, sizeof(BOOL));
#if 0
    res = pDwmSetWindowAttribute(winId(), DWMWA_NCRENDERING_ENABLED, &value, sizeof(BOOL));
    DWMNCRENDERINGPOLICY policy = (!activated || isAeroGlassEnabled()) ? DWMNCRP_USEWINDOWSTYLE : DWMNCRP_DISABLED;
    res = pDwmSetWindowAttribute(winId(), DWMWA_NCRENDERING_POLICY, &policy, sizeof(DWMNCRENDERINGPOLICY));
#endif
    Q_UNUSED(res);
    return true;
}

void WindowTitleBarPrivateWin::updateSysButtonsRect()
{
    if (styledFrame())
    {
        WindowTitleBarPrivate::updateSysButtonsRect();
    }
    else
    {
        if (pDwmGetWindowAttribute != Q_NULL && isAeroGlassEnabled())
        {
            RECT bounds;
            pDwmGetWindowAttribute(winId(), DWMWA_CAPTION_BUTTON_BOUNDS, &bounds, sizeof(bounds));
            m_systemButtonsRect = qtn_fromNativeDPI(
                QRect(bounds.left, bounds.top, RectWidth(bounds), RectHeight(bounds)), window());
            m_systemButtonsRect.setLeft(windowSize().width() - m_systemButtonsRect.width());
        }
        else
            m_systemButtonsRect = QRect();
    }
}

void WindowTitleBarPrivateWin::updateWindowSize()
{
    RECT rect;
    GetWindowRect(winId(), &rect);
    QSize newSize = qtn_fromNativeDPI(
        QSize(RectWidth(rect), RectHeight(rect)), window());
    QMargins extraMargins = extraFrameMargins();
    newSize -= QSize(extraMargins.left() + extraMargins.right(), extraMargins.top() + extraMargins.bottom());
    if (windowSize() != newSize)
    {
        resizeNonClientEvent(newSize);
    }
}

QMargins WindowTitleBarPrivateWin::calcSystemFrameMargins() const
{
    return qtn_fromNativeDPI(qtn_getWindowFrameMargings(window()), window());
}

QMargins WindowTitleBarPrivateWin::calcWidndows10FrameMargins() const
{
    QMargins margins(0, 0, 0, 0);
#if 0
    if (pDwmGetWindowAttribute == Q_NULL || !m_aeroGlassEnabled)
        return margins;

    RECT rect = { 0,0,0,0 };
    RECT frame = { 0,0,0,0 };
    GetWindowRect(winId(), &rect);
    pDwmGetWindowAttribute(winId(), DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame));
    margins.setLeft(frame.left - rect.left);
    margins.setTop(frame.top - rect.top);
    margins.setRight(rect.right - frame.right);
    margins.setBottom(rect.bottom - frame.bottom);
#endif
    if (qtn_isWindows10())
    {
        QMargins sysmargins = calcSystemFrameMargins();
        int dpi = qtn_getWindowDPI(window());
        const int thickness = 1 * dpi / 96;
        margins = QMargins(sysmargins.left() - thickness, 0, sysmargins.right() - thickness, sysmargins.bottom() - thickness);
    }
    return margins;
}

QStyle::SubControl WindowTitleBarPrivateWin::getHoverNativeSubControl(const QPoint& pos)
{
    LRESULT result = 0;
    QPoint gp = qtn_toNativeDPI(mapToGlobal(pos), window());
    pDwmDefWindowProc(winId(), WM_NCHITTEST, 0, MAKELPARAM(gp.x(), gp.y()), &result);
    if (result != 0)
    {
        QStyle::SubControl sc = QStyle::SC_None;
        switch (result)
        {
        case HTMINBUTTON:
            sc = QStyle::SC_TitleBarMinButton;
            break;
        case HTMAXBUTTON:
            sc = QStyle::SC_TitleBarMaxButton;
            break;
        case HTCLOSE:
            sc = QStyle::SC_TitleBarCloseButton;
            break;
        case HTHELP:
            sc = QStyle::SC_TitleBarContextHelpButton;
            break;
        default:
            break;
        }
        return sc;
    }
    return QStyle::SC_None;
}

void WindowTitleBarPrivateWin::styleChanged()
{
    if (!isVisible())
        return;

    m_frameWindows10 = calcWidndows10FrameMargins();
    m_systemFrameMargins = calcSystemFrameMargins();

    WindowTitleBarPrivate::styleChanged();
    if (!m_isFinalized)
    {
        fixupQtInternals();
        updateWindowSize();
    }
}

bool WindowTitleBarPrivateWin::handlePaint()
{
#if 0
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_winId, &ps);
    EndPaint(m_winId, &ps);
#endif
    return false;
}

#if 0
static bool qtn_isNonClientTrackMouseEnabled(HWND winId)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_QUERY;
    if (!TrackMouseEvent(&tme))
        qWarning("TrackMouseEvent failed.");
    bool ret = tme.hwndTrack == winId && tme.dwFlags == (TME_LEAVE | TME_NONCLIENT);
    return ret;
}
#endif

void WindowTitleBarPrivateWin::setTrackMouseEnabled(bool enabled)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    if (enabled)
        tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
    else
        tme.dwFlags = TME_LEAVE | TME_NONCLIENT | TME_CANCEL;

    tme.hwndTrack = winId();
    tme.dwHoverTime = HOVER_DEFAULT;
    if (!TrackMouseEvent(&tme))
        qWarning("TrackMouseEvent failed.");
}

bool WindowTitleBarPrivateWin::hasMaximizedStyle() const
{
    return GetWindowLong(winId(), GWL_STYLE) & WS_MAXIMIZE;
}

bool WindowTitleBarPrivateWin::hasFullScreenStyle() const
{
    return !(GetWindowLong(winId(), GWL_STYLE) & WS_THICKFRAME);
}

bool WindowTitleBarPrivateWin::hasSysMenuStyle() const
{
    return GetWindowLong(winId(), GWL_STYLE) & WS_SYSMENU;
}

void WindowTitleBarPrivateWin::setSysMenuStyle(bool enabled)
{
    m_lockStyleChange = true;
    if (enabled)
        SetWindowLong(winId(), GWL_STYLE, GetWindowLong(winId(), GWL_STYLE) | WS_SYSMENU);
    else
        SetWindowLong(winId(), GWL_STYLE, GetWindowLong(winId(), GWL_STYLE) & ~WS_SYSMENU);
    m_lockStyleChange = false;
}

bool WindowTitleBarPrivateWin::handleNonClientMouseEvent(MSG* msg, LRESULT &outResult)
{
    QPoint nativeScreenPosition(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));
    switch (msg->message)
    {
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
#ifdef QTN_WMPOINTER_SUPPORTS
    case WM_POINTERDOWN:
    case WM_POINTERUP:
#endif
        {
            POINT pt = { nativeScreenPosition.x() , nativeScreenPosition.y() };
            ClientToScreen(winId(), &pt);
            nativeScreenPosition = QPoint(pt.x, pt.y);
        }
        break;
    default:
        break;
    }
#if 0
    const Qt::MouseButtons buttons = qtn_state_mouse_buttons();
    Qt::KeyboardModifiers modifiers = qtn_state_keyboard_modifiers();
#endif

    bool result = false;
    const QPoint screenPosition = qtn_fromNativeDPI(nativeScreenPosition, window());

    switch (msg->message)
    {
        case WM_NCMOUSELEAVE:
        {
            LRESULT lr;
            pDwmDefWindowProc(winId(), WM_NCMOUSELEAVE, 0, 0, &lr);

            setTrackMouseEnabled(false);
            m_mouseTracking = false;
            const QPoint pos = mapFromGlobal(QCursor::pos());
            checkUnderMouse(pos);
            result = true;
        }
        break;
#ifdef QTN_WMPOINTER_SUPPORTS
        case WM_NCPOINTERUPDATE:
#endif
        case WM_NCMOUSEMOVE:
        {
            if (!m_mouseTracking)
            {
                setTrackMouseEnabled(true);
                m_mouseTracking = true;
            }
#if 0
            QPoint p = window()->mapFromGlobal(globalPosition);
            int clientMessage = msg->message == WM_NCPOINTERUPDATE ? WM_POINTERUPDATE : WM_MOUSEMOVE;
            DWORD wParam = msg->message == WM_NCPOINTERUPDATE ? qtn_createClientPointerWparam(GET_POINTERID_WPARAM(msg->wParam)) :
                qtn_createClientMouseWparam(buttons, modifiers);
            outResult = OldWindowProc(winId(), clientMessage, wParam, MAKELPARAM(p.x(), p.y()));
            //QApplication::processEvents();
            result = true;
#endif
        }
        break;
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCMBUTTONDBLCLK:

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
#ifdef QTN_WMPOINTER_SUPPORTS
    case WM_POINTERDOWN:
    case WM_NCPOINTERDOWN:
#endif
        {
#if 0
            QPoint pos = window()->mapFromGlobal(globalPosition);

            int mouseDownMsg = qtn_nonClientToClientMouseDownMessage(msg->message);
            DWORD wParam = msg->message == WM_NCPOINTERDOWN ?
                qtn_createClientPointerWparam(GET_POINTERID_WPARAM(msg->wParam)) : 
                qtn_createClientMouseWparam(buttons, modifiers);
            DWORD lParam = MAKELPARAM(pos.x(), pos.y());
#else
            int mouseDownMsg = msg->message;
            WPARAM wParam = msg->wParam;
            LPARAM lParam = msg->lParam;
#endif
            if (m_mouseTracking)
                setTrackMouseEnabled(false);

            m_sysCommandStatus = false;
            QPoint ncpos = mapFromGlobal(screenPosition);
            const bool sysButtons = m_systemButtonsRect.contains(ncpos);
            m_ignoreSysCommand = (!sysButtons || styledFrame());
            outResult = OldWindowProc(winId(), mouseDownMsg, wParam, lParam);
            m_ignoreSysCommand = false;
            
            if (defaultResizeHandler() || containsPoint(ncpos))
                QApplication::processEvents();

            if (nativeCommandNeeded())
                execNativeCommand(nativeScreenPosition);

            if (m_sysCommandStatus)
            {
                setSubControlPressed(QStyle::SC_None);
                checkUnderMouse(ncpos);
                m_sysCommandStatus = false;
            }
 
            if (m_mouseTracking && !m_isFinalized)
                setTrackMouseEnabled(true);

            result = true;
        }
        break;

    case WM_NCLBUTTONUP:
    case WM_NCRBUTTONUP:
    case WM_NCMBUTTONUP:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
#ifdef QTN_WMPOINTER_SUPPORTS
    case WM_POINTERUP:
    case WM_NCPOINTERUP:
#endif
    {
#if 0
        QPoint p = window()->mapFromGlobal(globalPosition);
        int clientMouseUpMsg = qtn_nonClientToClientMouseUpMessage(msg->message);
        DWORD wParam = msg->message == WM_NCPOINTERUP ?
            qtn_createClientPointerWparam(GET_POINTERID_WPARAM(msg->wParam)) :
            qtn_createClientMouseWparam(buttons, modifiers);
        DWORD lParam = MAKELPARAM(p.x(), p.y());
#else
        int clientMouseUpMsg = msg->message;
        DWORD wParam = msg->wParam;
        DWORD lParam = msg->lParam;
#endif
        outResult = OldWindowProc(winId(), clientMouseUpMsg, wParam, lParam);
        //QApplication::processEvents();
        //setSubControlPressed(QStyle::SC_None);

        setTrackMouseEnabled(true);
        checkUnderMouse(mapFromGlobal(screenPosition));
        result = true;
    }
    break;
    default:
        {
            Q_ASSERT(false);
        }
        break;
    }
    
    return result;
}

int WindowTitleBarPrivateWin::handleNonClientHitTest(LPPOINT ptHit)
{
    int result = HTERROR;
    const QPoint clientPosition = mapFromGlobal(
        qtn_fromNativeDPI(QPoint(ptHit->x, ptHit->y), window()));

    Qt::WindowFrameSection frameSection;
    QStyle::SubControl sc = hitTest(clientPosition, frameSection);
    Q_UNUSED(sc);
    switch (frameSection)
    {
    case Qt::LeftSection:
        result = HTLEFT;
        break;
    case Qt::RightSection:
        result = HTRIGHT;
        break;
    case Qt::BottomSection:
        result = HTBOTTOM;
        break;
    case Qt::TopSection:
        result = HTTOP;
        break;
    case Qt::TopLeftSection:
        result = HTTOPLEFT;
        break;
    case Qt::BottomRightSection:
        result = HTBOTTOMRIGHT;
        break;
    case Qt::TopRightSection:
        result = HTTOPRIGHT;
        break;
    case Qt::BottomLeftSection:
        result = HTBOTTOMLEFT;
        break;
    case Qt::TitleBarArea:
    default:
        //We should use client mouse events to work correctly the QWidget that we embed into the title bar.
        result = HTCLIENT;
        break;
    }
#if 0
    if (result == HTCAPTION)
    {
        switch (sc)
        {
        case QStyle::SC_TitleBarSysMenu:
            result = HTSYSMENU;
            break;
        case QStyle::SC_TitleBarMinButton:
            result = HTMINBUTTON;
            break;
        case QStyle::SC_TitleBarMaxButton:
            result = HTMAXBUTTON;
            break;
        case QStyle::SC_TitleBarCloseButton:
            result = HTCLOSE;
            break;
        case QStyle::SC_TitleBarNormalButton:
            result = HTZOOM;
            break;
        case QStyle::SC_TitleBarContextHelpButton:
            result = HTHELP;
            break;
        case QStyle::SC_TitleBarShadeButton:
        case QStyle::SC_TitleBarUnshadeButton:
        case QStyle::SC_TitleBarLabel:
        default:
            result = result; //TODO
            break;
        }
    }
#endif
    return result;
}

bool WindowTitleBarPrivateWin::isAeroGlassEnabled() const
{
    return m_aeroGlassEnabled;
}

void WindowTitleBarPrivateWin::setAeroGlassEnabled(bool enabled)
{
    if (m_aeroGlassEnabled == enabled)
        return;

    if (!isCompositionEnabled() && enabled)
        return; //You can't set glass effect on Windows that does not suport composition.

    m_aeroGlassEnabled = enabled;

    if (winId() != 0)
        initializeWindowUI();
}

void WindowTitleBarPrivateWin::showSystemMenu(const QPoint& pos)
{
    if (window() == Q_NULL)
        return;

    Qt::WindowFlags flags = window()->windowFlags();
    HMENU menu = GetSystemMenu(winId(), FALSE);
    if (!menu)
        return;

    const UINT enabled = (MF_BYCOMMAND | MF_ENABLED);
    const UINT disabled = (MF_BYCOMMAND | MF_GRAYED);

    ::EnableMenuItem(menu, SC_MINIMIZE, (flags & Qt::WindowMinimizeButtonHint) ? enabled : disabled);
    bool maximized = IsZoomed(winId());

    ::EnableMenuItem(menu, SC_MAXIMIZE, !(flags & Qt::WindowMaximizeButtonHint) || maximized ? disabled : enabled);
    EnableMenuItem(menu, SC_RESTORE, maximized ? enabled : disabled);

    EnableMenuItem(menu, SC_SIZE, (flags & Qt::MSWindowsFixedSizeDialogHint) || maximized ? disabled : enabled);
    EnableMenuItem(menu, SC_MOVE, maximized ? disabled : enabled);
    EnableMenuItem(menu, SC_CLOSE, enabled);

    MENUITEMINFO closeItem;
    closeItem.cbSize = sizeof(MENUITEMINFO);
    closeItem.fMask = MIIM_STATE;
    closeItem.fState = flags & Qt::WindowCloseButtonHint ? MFS_DEFAULT : MFS_DISABLED;
    SetMenuItemInfo(menu, SC_CLOSE, FALSE, &closeItem);

    QPoint globalPos = qtn_toNativeDPI(pos, window());
    const int ret = TrackPopupMenuEx(menu, TPM_LEFTALIGN | TPM_TOPALIGN |
        TPM_NONOTIFY | TPM_RETURNCMD, globalPos.x(), globalPos.y(), winId(), 0);
    if (ret)
        OldWindowProc(winId(), WM_SYSCOMMAND, WPARAM(ret), 0);
}

void WindowTitleBarPrivateWin::setTransitionsEffectEnabled(bool enabled)
{
    if (pDwmSetWindowAttribute != Q_NULL)
    {
        BOOL value = !enabled;
        HRESULT res = pDwmSetWindowAttribute(winId(), DWMWA_TRANSITIONS_FORCEDISABLED, &value, sizeof(BOOL));
        Q_UNUSED(res);
    }
}

bool WindowTitleBarPrivateWin::isCompositionEnabled() const
{
    return qtn_isCompositionAvailable();
}

bool WindowTitleBarPrivateWin::isCompositionSupported() const
{
    return pDwmIsCompositionEnabled != Q_NULL;
}

void WindowTitleBarPrivateWin::setRedraw(bool redraw)
{ 
    DefWindowProcW(winId(), WM_SETREDRAW, (WPARAM)(BOOL)redraw, 0);
}

bool WindowTitleBarPrivateWin::isIconic() const
{ 
    return IsIconic(winId());
} 

void WindowTitleBarPrivateWin::forceNonClientSizeUpdate() 
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#else
#endif
    // Inform the application about changes in the nc frame.
    RECT rect;
    GetWindowRect(winId(), &rect);
    SetWindowPos(winId(), 
                 NULL, 
                 rect.left, rect.top,
                 RectWidth(rect), RectHeight(rect),
                 SWP_FRAMECHANGED);
}

//#define QTN_NO_SET_WINDOW_THEME

void WindowTitleBarPrivateWin::initializeWindowUI() 
{
    if (!isAeroGlassEnabled())
    {
        RECT rect;
        GetWindowRect(winId(), &rect);
        SendMessageW(winId(), WM_NCCALCSIZE, false, (LPARAM)&rect);
    }

    if (pSetWindowTheme != Q_NULL)
    {
        if (isCompositionSupported())
        {
#ifndef QTN_NO_SET_WINDOW_THEME
            pSetWindowTheme(winId(), L"Explorer", Q_NULL);
#endif
        }
        else
        {
            pSetWindowTheme(winId(), L"", L"");
        }
    }

    m_isActive = GetActiveWindow() == winId();
    m_maximized = hasMaximizedStyle();
    m_fullScreen = hasFullScreenStyle();

    m_isFinalized = false;
}
 
void WindowTitleBarPrivateWin::finalizeWindowUI()
{
    m_isFinalized = true;
    resizeNonClientEvent(QSize());
    if (isDestroyed())
        return;
#ifndef QTN_NO_SET_WINDOW_THEME
    if (pSetWindowTheme != Q_NULL)
        pSetWindowTheme(winId(), L"Explorer", Q_NULL);
#endif
    fixupQtInternals();
}

static bool isNcCalcSizeSupportedByQt()
{
    QStringList version = QString(QLatin1String(qVersion())).split(QLatin1Char('.'));
    Q_ASSERT(version.size() == 3);
    if (version.size() != 3)
        return false;
    int major = version[0].toInt();
    int minor = version[1].toInt();
    int patch = version[2].toInt();
    return QT_VERSION_CHECK(major, minor, patch) >= QT_VERSION_CHECK(5, 9, 0);
}

bool WindowTitleBarPrivateWin::handleNonClientCalcSize(NCCALCSIZE_PARAMS* sizeParams, bool calcRects)
{
    if (!calcRects)
        return false;
#if 0
    RECT oldrect = sizeParams->rgrc[0];
    OldWindowProc(winId(), WM_NCCALCSIZE, calcRects, (LPARAM)sizeParams);
    sizeParams->rgrc[0] = oldrect;
    return true;
#endif
    if (isNcCalcSizeSupportedByQt() || isFullScreen())
        return false;

    const QMargins margins = qtn_toNativeDPI(
        frameMargins() + extraFrameMargins(), window());

    sizeParams->rgrc[0].left += margins.left();
    sizeParams->rgrc[0].top += margins.top();
    sizeParams->rgrc[0].right -= margins.right();
    sizeParams->rgrc[0].bottom -= margins.bottom();

    return true;
}

#define WM_313UNDOCUMENTED 0x313

bool WindowTitleBarPrivateWin::handleMessage(MSG* msg, LRESULT& result)
{
    WindowTitleBarAutoUpdater updater(this);

    if (m_isFinalized)
        return false;

    if ((msg->message == WM_STYLECHANGING || msg->message == WM_STYLECHANGED) && m_lockStyleChange)
    {
        if (msg->message == WM_STYLECHANGED)
            m_lockStyleChange = false;
        return true;
    }
    switch (msg->message)
    {
        case WM_NCCREATE:
            {
                if (handleNonClientCreate())
                    return true;
            }
            break;
        case WM_NCDESTROY:
            m_isFinalized = true;
           break;
        case WM_MOVE:
        case WM_WINDOWPOSCHANGING:
        {
            if (msg->message == WM_WINDOWPOSCHANGING)
            {
                checkMaximized();
                m_glowManager->setVisible(!isMaximized() && !isFullScreen());
            }
            m_glowManager->updateWindowPos();
        }
        break;
        case WM_STYLECHANGED:
        case WM_WINDOWPOSCHANGED:
            {
                if (m_fixupQtNeeded)
                    fixupQtInternals(false);
            }
            break;
        case WM_SYSKEYDOWN :
            if (!hasSysMenuStyle() && quint32(msg->wParam) == VK_SPACE && 
                ((GetKeyState(VK_LMENU) & 0x80) || (GetKeyState(VK_RMENU) & 0x80)))
            {
                showSystemMenu(window()->geometry().topLeft());
                return true;
            }
            break;
        case WM_SHOWWINDOW:
            {
                m_fillClientArea = true;
            }
            break;
        case WM_313UNDOCUMENTED:
            {
                setSysMenuStyle(true);
                result = OldWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
                setSysMenuStyle(false);
#if 0
                POINT pt;
                POINTSTOPOINT(pt, msg->lParam);
                showSystemMenu(qtn_fromNativeDPI(QPoint(pt.x, pt.y), window()));
#endif
                return true;
            }
            break;
        case WM_NCACTIVATE:
        case WM_NCPAINT:
            {
                if (msg->message == WM_NCACTIVATE)
                {
                    m_isActive = (BOOL)msg->wParam;
                    nonClientActivateEvent(m_isActive);
                    if (!isActive())
                    {
                        setTrackMouseEnabled(false);
                    }
                    else
                    {
                        if (underMouse())
                        {
                            Qt::MouseButtons buttons = qtn_state_mouse_buttons();
                            if (buttons & Qt::LeftButton)
                                setTrackMouseEnabled(true);
                        }
                    }
                }

                if (isCompositionSupported() && (msg->message == WM_NCACTIVATE || isAeroGlassEnabled()))
                    result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

                updateWindowSize();

                if (handleNonClientPaint(msg->message == WM_NCACTIVATE ? (HRGN)1 : (HRGN)msg->wParam)) 
                    return true;
                return true;
            }
            break; 
        case WM_NCCALCSIZE:
            {
                if (handleNonClientCalcSize(reinterpret_cast<NCCALCSIZE_PARAMS *>(msg->lParam), (BOOL)msg->wParam))
                    return true;

                //Dirty hack to avoid toolbar paint artifacts on old Windows (without DWM - XP, NT etc.)
                LONG oldStyles = GetWindowLong(winId(), GWL_STYLE);
                LONG newStyles = oldStyles & ~(WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL | WS_BORDER | WS_SIZEBOX);
                if (!isCompositionSupported())
                {
                    m_lockStyleChange = true;
                    SetWindowLong(winId(), GWL_STYLE, newStyles);
                    m_lockStyleChange = false;
                }

                result = OldWindowProc(winId(), msg->message, msg->wParam, msg->lParam);

                if (!isCompositionSupported())
                {
                    m_lockStyleChange = true;
                    SetWindowLong(winId(), GWL_STYLE, oldStyles);
                    m_lockStyleChange = false;
                }
                return true;
            }
            break;
        case WM_SETTEXT:
            {
                result = OldWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
                if (!isCompositionSupported())
                    handleNonClientPaint((HRGN)1);
                return true;
            }
            break;

        case WM_SIZE:
            {
                updateWindowSize();
            }
            break;
        case WM_MOVING:
            {
                updateMaterialWidgets();
            }
            break;
        case WM_PAINT:
            {
                if (handlePaint())
                    return true;
            }
            break;

        case WM_ERASEBKGND:
            {
            }
            break;

        case WM_CLOSE:
            {
                if (isAeroGlassEnabled())
                    setTransitionsEffectEnabled(false);
            }
            break;
        case WM_SYSCOMMAND:
            {
                if (m_ignoreSysCommand)
                    return true;
                m_sysCommandStatus = true;
            }
            break;
        break;
        case WM_NCMOUSELEAVE:
        case WM_NCMOUSEMOVE:

        case WM_NCLBUTTONDOWN:
        case WM_NCRBUTTONDOWN:
        case WM_NCMBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:

#if 0
        case WM_NCPOINTERDOWN:
        case WM_POINTERDOWN:
        case WM_NCPOINTERUPDATE:
        case WM_NCPOINTERUP:
        case WM_POINTERUP:
#endif
#if 1
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_NCLBUTTONUP:
        case WM_NCRBUTTONUP:
        case WM_NCMBUTTONUP:
#endif
            {
            if (handleNonClientMouseEvent(msg, result))
                return true;
            }
            break;
        case WM_SETCURSOR:
            {
#if 0
                if (m_hasCursor && !defaultResizeHandler())
                    return true;
#endif
            }
            break;
        case WM_NCHITTEST:
            {
#if 0
                pDwmDefWindowProc(winId(), WM_NCHITTEST, msg->wParam, msg->lParam, &result);
#endif

                POINT pt;
                POINTSTOPOINT(pt, msg->lParam);
                m_hitTest = handleNonClientHitTest(&pt);
                if (m_hitTest == HTERROR)
                    m_hitTest = OldWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
                result = m_hitTest;
                return true;
            }
        break;
        case WM_SETTINGCHANGE:
            {
                if (msg->wParam == SPI_SETDESKWALLPAPER)
                {
                    m_bluredDesktop = QPixmap();
                }
                if (msg->wParam == 0 && 
                    QString((const QChar *)msg->lParam) == QStringLiteral("ImmersiveColorSet"))
                {
                    styleChanged();
                }
            }
            break;
        case WM_DPICHANGED:
        case WM_THEMECHANGED:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            {
                result = OldWindowProc(winId(), msg->message, msg->wParam, msg->lParam);
                styleChanged();
                return true;
            }
            break;
        default:
            {
            }
            break;
    }
    return false;
}

#define CEF_COLOR_BACKGROUND_ACTIVE         RGB(60, 63, 65)
#define CEF_COLOR_BACKGROUND_INACTIVE       RGB(68, 71, 73) 

void WindowTitleBarPrivateWin::enforceMenuBackground()
{
    static HBRUSH mBackgroundActiveBrush = 0;
    static HBRUSH mBackgroundInactiveBrush = 0; 

    if (mBackgroundActiveBrush == NULL) {                            
        mBackgroundActiveBrush = CreateSolidBrush(CEF_COLOR_BACKGROUND_ACTIVE);
    }

    if (mBackgroundInactiveBrush == NULL) {                            
        mBackgroundInactiveBrush = CreateSolidBrush(CEF_COLOR_BACKGROUND_INACTIVE);
    } 

    HMENU menu = GetSystemMenu(winId(), false);
    MENUINFO mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    GetMenuInfo(menu, &mi);

    if (m_isActive)
    {
        if (mi.hbrBack != mBackgroundActiveBrush)
        {
            mi.hbrBack = mBackgroundActiveBrush;
            SetMenuInfo(menu, &mi);
        }
    }
    else
    {
        if (mi.hbrBack != mBackgroundInactiveBrush)
        {
            mi.hbrBack = mBackgroundInactiveBrush;
            SetMenuInfo(menu, &mi);
        }
    }
} 

#if 0
RECT WindowTitleBarPrivateWin::getLogicalClientRect() const
{
    WINDOWINFO wi;
    ZeroMemory(&wi, sizeof(wi));
    wi.cbSize = sizeof(wi);
    GetWindowInfo(winId(), &wi);
   
    RECT rect;
    rect.top = wi.rcClient.top - wi.rcWindow.top;
    rect.left = wi.rcClient.left - wi.rcWindow.left;
    rect.bottom = rect.top + RectHeight(wi.rcClient);
    rect.right = rect.left + RectWidth(wi.rcClient);

    return rect;
}
#endif

void WindowTitleBarPrivateWin::paintNonClientAreaPart(HDC hdc, const QRegion& clipRegion, const QMargins& margins, Qt::WindowFrameSection section)
{
    Q_UNUSED(clipRegion);
    RECT rect;
    GetWindowRect(winId(), &rect);
    QSize size(RectWidth(rect), RectHeight(rect));

    QRegion regionPart;
    QRect partRect(QPoint(0, 0), size);

    QMargins frame = qtn_toNativeDPI(
        QMargins(borderThickness(), 
            extendViewIntoTitleBar() ? borderThickness() : titleHeight(), 
            borderThickness(), borderThickness()), window());
    if (section == Qt::TitleBarArea)
    {
        int h = frame.top();
        regionPart = QRect(QPoint(0, 0), QSize(size.width(), h));
        partRect.setHeight(h);
    }
    else if (section == Qt::LeftSection)
    {
        regionPart = QRect(QPoint(0, 0), QSize(frame.left(), size.height()));
        partRect.setWidth(frame.left());
    }
    else if (section == Qt::RightSection)
    {
        regionPart = QRect(QPoint(size.width() - frame.right(), 0), QSize(frame.right(), size.height()));
        partRect.setWidth(frame.right());
    }
    else if (section == Qt::BottomSection)
    {
        regionPart = QRect(QPoint(0, size.height() - frame.bottom()), QSize(size.width(), frame.bottom()));
        partRect.setHeight(frame.bottom());
    }
    else if (section == Qt::NoSection)
    {
        regionPart = partRect;
        regionPart -= partRect.adjusted(frame.left(), frame.top(), -frame.right(), -frame.bottom());
    }

    if (partRect.isEmpty())
        return;

    ImageHDC ncpaintptr(partRect.width(), partRect.height(), ImageHDC::systemFormat());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    ncpaintptr.image().setDevicePixelRatio(qtn_pixelRatioDPI(window()));
#endif

    ncpaintptr.image().fill(Qt::transparent);

    QPainter painter(&ncpaintptr.image());
    paintNonClientEvent(&painter, section);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    for (QRegion::const_iterator it = regionPart.begin(); it != regionPart.end(); ++it)
#else
    QVector<QRect> rcs = clipRegionPart.rects();
    for (QVector<QRect>::const_iterator it = rcs.constBegin(); it != rcs.constEnd(); ++it)
#endif
    {
        const QRect& r = *it;
        BitBlt(hdc, r.left() + margins.left(), r.top() + margins.top(),
            r.width(), r.height(), ncpaintptr.hdc(), 0, 0, SRCCOPY);
    }
}

void WindowTitleBarPrivateWin::paintNonClientArea(HDC hdc)
{
    QRegion clipRegion = getNonClientRegion();
    if (clipRegion.isEmpty())
        return;
    const QMargins margins = qtn_toNativeDPI(extraFrameMargins(), window());

    if (m_fillClientArea)
    {
        if (styledFrame())
        {
            QRect rect = qtn_toNativeDPI(getClientRect(), window(), true);
            ImageHDC ncpaintptr(rect.width(), rect.height(), ImageHDC::systemFormat());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            ncpaintptr.image().setDevicePixelRatio(qtn_pixelRatioDPI(window()));
#endif
            ncpaintptr.image().fill(window()->palette().color(isActive() ? QPalette::Active : QPalette::Inactive, window()->backgroundRole()));
            BitBlt(hdc, rect.left(), rect.top(),
                rect.width(), rect.height(), ncpaintptr.hdc(), 0, 0, SRCCOPY);
        }
        m_fillClientArea = false;
    }

    paintNonClientAreaPart(hdc, clipRegion, margins, Qt::LeftSection);
    paintNonClientAreaPart(hdc, clipRegion, margins, Qt::RightSection);
    paintNonClientAreaPart(hdc, clipRegion, margins, Qt::BottomSection);
    if (!extendViewIntoTitleBar()) //Title bar area for extended mode will be painted by the ExtendedView widget.
        paintNonClientAreaPart(hdc, clipRegion, margins, Qt::TitleBarArea);
}

bool WindowTitleBarPrivateWin::eventFilter(QObject* object, QEvent* event)
{
    WindowTitleBarAutoUpdater updater(this);
    if (event->type() == QEvent::WindowStateChange)
        update(getNonClientRegion());
    else if (mouseEventFilter(object, event))
        return true;

    return WindowTitleBarPrivate::eventFilter(object, event);
}

void WindowTitleBarPrivateWin::mouseMoveNonClientEvent(QMouseEvent* event)
{
    WindowTitleBarPrivate::mouseMoveNonClientEvent(event);
}

void WindowTitleBarPrivateWin::mouseContextNonClientEvent(QContextMenuEvent* event)
{
    WindowTitleBarPrivate::mouseContextNonClientEvent(event);
}

void WindowTitleBarPrivateWin::resizeNonClientEvent(const QSize& windowSize)
{
    WindowTitleBarPrivate::resizeNonClientEvent(windowSize);
    updateLayout();
}

void WindowTitleBarPrivateWin::nativeCommand(QMouseEvent* event, Qt::WindowFrameSection frameSection)
{
    m_sysCommand = qtn_get_sys_command_win(window(), event, frameSection);
    if (event != Q_NULL)
        event->setAccepted(m_sysCommand != 1);
}

bool WindowTitleBarPrivateWin::nativeCommandNeeded() const
{
    return m_sysCommand != -1;
}

void WindowTitleBarPrivateWin::execNativeCommand(const QPoint& pos)
{
    qtn_sys_command_win(window(), m_sysCommand, pos);
    m_sysCommand = -1;
}

QMargins WindowTitleBarPrivateWin::invisibleNonClientMaximizeMargins() const
{
    int borderThicknessDef = systemFrameMargins().left();
    return  QMargins(
        borderThicknessDef - borderThickness(),
        borderThicknessDef - borderThickness(),
        borderThicknessDef - borderThickness(),
        borderThicknessDef - borderThickness());
}

QMargins WindowTitleBarPrivateWin::extraFrameMargins() const
{
    //invisibleNonClientMaximizeMargins();
    if (!isMaximized())
        return QMargins(0, 0, 0, 0);
    
    int borderThicknessDef = systemFrameMargins().left();
    return  QMargins(
        borderThicknessDef - borderThickness(),
        borderThicknessDef - borderThickness(),
        borderThicknessDef - borderThickness(),
        borderThicknessDef - borderThickness());
}

QPoint WindowTitleBarPrivateWin::mapFromGlobal(const QPoint& pos) const
{
    RECT rect;
    GetWindowRect(winId(), &rect);
    QRect windowRect = qtn_fromNativeDPI(
        QRect(rect.left, rect.top, RectWidth(rect), RectHeight(rect)), window());
    QMargins margins = extraFrameMargins();
    return pos - QPoint(windowRect.left() + margins.left(), 
        windowRect.top() + margins.top());
}

QPoint WindowTitleBarPrivateWin::tranlateToNonClient(const QPoint& clientPos)
{
    return clientPos + QPoint(frameMargins().left(), frameMargins().top());
}

bool WindowTitleBarPrivateWin::isGlowEnabled() const
{
    return m_glowManager->isEnabled();
}

QPoint WindowTitleBarPrivateWin::mapToGlobal(const QPoint& pos) const
{
    RECT rect;
    GetWindowRect(winId(), &rect);
    QRect windowRect = qtn_fromNativeDPI(
        QRect(rect.left, rect.top, RectWidth(rect), RectHeight(rect)), window());

    QMargins margins = extraFrameMargins();
    return pos + QPoint(windowRect.left() + margins.left(), 
        windowRect.top() + margins.top());
}

bool WindowTitleBarPrivateWin::installWindow()
{
    QWidget* _window = window();
    if (_window == Q_NULL)
    {
        Q_ASSERT(false);
        return false;
    }

    HWND winId = (HWND)_window->internalWinId();
    if (winId == 0 || !WindowTitleBarPrivate::installWindow())
        return false;

    if (GetWindowLong(winId, GWL_STYLE) & WS_CHILD)
    {
        Q_ASSERT(false); //Can't manage child window.
        return false;
    }


    WindowsHookManager::instance()->addHook(winId, this);
    
    m_glowManager->setEnabled(isResizible());

    initializeWindowUI();
    m_oldFlags = _window->windowFlags();
    return true;
}

void WindowTitleBarPrivateWin::resetWindow()
{
    WindowTitleBarPrivate::resetWindow();
    if (winId() == 0)
        return;
    m_oldFlags = Qt::WindowFlags();
    finalizeWindowUI();
    m_glowManager->resetWindow();
    WindowsHookManager::instance()->removeHook(this);
    Q_ASSERT(winId() == 0);
}

void WindowTitleBarPrivateWin::checkMaximized()
{
    bool maximized = hasMaximizedStyle();
    bool fullScreen = hasFullScreenStyle();
    if (m_maximized != maximized)
    {
        m_maximized = maximized;
        m_fixupQtNeeded = true;
    }

    if (m_fullScreen != fullScreen)
    {
        m_fullScreen = fullScreen;
        m_fixupQtNeeded = true;
    }
}

void WindowTitleBarPrivateWin::fixupQtInternals(bool forceUpdate)
{
    m_fixupQtNeeded = false;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWindow* _window = window()->windowHandle();
    Q_ASSERT(_window != Q_NULL);
    if (_window == Q_NULL)
        return;

    QMargins margins = QMargins();
    
    if (!m_isFinalized && !isFullScreen())
    {
        margins = frameMargins() + extraFrameMargins();
        margins = qtn_toNativeDPI(margins, window());
        margins -= qtn_getWindowFrameMargings(window());
    }

#if 0
    QMargins contentsMargins = m_origContentsMargins;
    if (frameMargins().isNull())
    {
        int border = m_isFinalized || !isMaximized() ? 0 : systemFrameMargins().left();
        contentsMargins += QMargins(border, border, border, border);
    }

    if (window()->contentsMargins() != contentsMargins)
        window()->setContentsMargins(contentsMargins);
#endif

    _window->setProperty("_q_windowsCustomMargins", QVariant::fromValue(margins));
    updateDwmSettings(!m_isFinalized);
    updateDwmSettings(!m_isFinalized);

#ifdef QTN_PRIVATE_HEADERS
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (QWindowsWindow* platformWindow = dynamic_cast<QWindowsWindow*>(_window->handle()))
        platformWindow->setCustomMargins(margins);
#else
    QPlatformWindow* platformWindow = _window->handle();
    Q_ASSERT(platformWindow != Q_NULL);
    if (platformWindow == Q_NULL)
        return;
    QGuiApplication::platformNativeInterface()->setWindowProperty(platformWindow, QStringLiteral("WindowsCustomMargins"), QVariant::fromValue(margins));
#endif
#endif
#else
    NONCLIENTMETRICS sysMetrics;
    ZeroMemory(&sysMetrics, sizeof(NONCLIENTMETRICS));
    sysMetrics.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo( SPI_GETNONCLIENTMETRICS, NULL, &sysMetrics, NULL);

    //We have to reset data->fstrut_dirty = 1; in QWidget structure to right client area calculation.
    OldWindowProc(winId(), WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, (LPARAM)&sysMetrics);
#endif
    if (forceUpdate)
        forceNonClientSizeUpdate();
}


