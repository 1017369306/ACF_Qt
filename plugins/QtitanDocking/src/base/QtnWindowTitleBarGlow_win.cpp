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
#include "QtnWindowTitleBarGlow_win.h"
#include "QtnPlatform_win.h"
#include <QApplication>
#include <QHash>
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


QTITAN_BEGIN_NAMESPACE
/* WindowTitleBarGlowWindow */
class WindowTitleBarGlowWindow
{
public:
    WindowTitleBarGlowWindow(WindowTitleBarGlowManager* manager, Qt::WindowFrameSection section);
    ~WindowTitleBarGlowWindow();
public:
    WindowTitleBarGlowManager* manager() const { return m_manager; }
    Qt::WindowFrameSection section() const { return m_section; }
    HWND hWnd() const { return m_hWnd; }
    bool create(HWND parentHWnd);
    void destroyWindow();
    void updateWindowPos(HDWP hDWP);
    void createBitmapShadow();
    void paintGlow(bool updateOnly = false);
    void drawGlow(HDC hDC, const QRect& rect, uint index, byte alpha = 255);
    Qt::WindowFrameSection hitTestToSection(const QPoint& point) const;
    uint hitTest(const QPoint& point) const;
private:
    WindowTitleBarGlowManager* m_manager;
    Qt::WindowFrameSection m_section;
    HWND m_hWnd;
    HBITMAP m_hBitmap;
    QSize m_size;
public:
    static QColor m_clrTransparent;
    static QHash<HWND, WindowTitleBarGlowWindow*> m_hashGlow;
};
QTITAN_END_NAMESPACE

QColor WindowTitleBarGlowWindow::m_clrTransparent = QColor(0xFF, 0xFF, 0xFF, 0x01);
QHash<HWND, WindowTitleBarGlowWindow*> WindowTitleBarGlowWindow::m_hashGlow;
WindowTitleBarGlowWindow::WindowTitleBarGlowWindow(WindowTitleBarGlowManager* manager, Qt::WindowFrameSection section)
    : m_manager(manager)
    , m_section(section)
    , m_hWnd(Q_NULL)
    , m_hBitmap(Q_NULL)
{
}

WindowTitleBarGlowWindow::~WindowTitleBarGlowWindow()
{
    Q_ASSERT(m_hBitmap == Q_NULL);
}

#define qtn_GET_X_LPARAMTOPNT(lp)    ((int)(short)LOWORD(lp))
#define qtn_GET_Y_LPARAMTOPNT(lp)    ((int)(short)HIWORD(lp))
#define qtn_GET_X_PNTTOLPARAM(lp)   (MAKEPOINTS(lp).x)
#define qtn_GET_Y_PNTTOLPARAM(lp)   (MAKEPOINTS(lp).y)
LRESULT CALLBACK GlowWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowTitleBarGlowWindow* glowWnd = WindowTitleBarGlowWindow::m_hashGlow.value(hwnd);
    if (glowWnd == Q_NULL)
        return DefWindowProc(hwnd, message, wParam, lParam);

    LRESULT result = 0;
    switch (message)
    {
        case WM_WINDOWPOSCHANGED:
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
            WINDOWPOS* lpwndpos = (WINDOWPOS*)lParam;
            glowWnd->paintGlow((lpwndpos->flags & SWP_NOSIZE) == SWP_NOSIZE);
            return result;
        }
        case WM_NCACTIVATE:
            return DefWindowProc(hwnd, WM_NCACTIVATE, FALSE, 0L);
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if (::GetForegroundWindow() != glowWnd->manager()->hWnd())
                ::SetForegroundWindow(glowWnd->manager()->hWnd());

            POINT point;
            point.x = (short)qtn_GET_X_LPARAMTOPNT((DWORD)lParam);
            point.y = (short)qtn_GET_Y_LPARAMTOPNT((DWORD)lParam);
            QPoint pnt(point.x, point.y);
            ::ClientToScreen(hwnd, &point);
            glowWnd->manager()->titleBar()->nativeCommand(Q_NULL, glowWnd->hitTestToSection(QPoint(point.x, point.y)));
            glowWnd->manager()->titleBar()->execNativeCommand(pnt);
            return 0;
        }
        case WM_SETCURSOR:
        {
            POINT point;
            ::GetCursorPos(&point);
            QPoint pnt(point.x, point.y);
            UINT hit = glowWnd->hitTest(pnt);

            if (hit == HTNOWHERE || hit == HTBORDER)
                return DefWindowProc(hwnd, message, wParam, lParam);

            LPCTSTR id = Q_NULL;
            switch (hit)
            {
                case HTLEFT:
                case HTRIGHT:
                    id = IDC_SIZEWE;
                    break;

                case HTTOP:
                case HTBOTTOM:
                    id = IDC_SIZENS;
                    break;

                case HTTOPLEFT:
                case HTBOTTOMRIGHT:
                    id = IDC_SIZENWSE;
                    break;

                case HTTOPRIGHT:
                case HTBOTTOMLEFT:
                    id = IDC_SIZENESW;
                    break;
            }
            ::SetCursor(::LoadCursor(Q_NULL, id));
            return true;
        }
        case WM_ERASEBKGND:
            return true;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return result;
}

bool WindowTitleBarGlowWindow::create(HWND parentHWnd)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QString clsName(QStringLiteral("QtitanTitleBarGlowWindow"));
#else
    QString clsName(QStringLiteral("QtitanTitleBarGlowWindow"));
#endif
    const UINT classStyle = CS_VREDRAW | CS_HREDRAW;
    const HINSTANCE appInstance = (HINSTANCE)GetModuleHandle(Q_NULL);

    WNDCLASSW wndcls;
    if (!::GetClassInfoW(appInstance, (wchar_t*)clsName.utf16(), &wndcls))
    {
        wndcls.style = classStyle;
        wndcls.lpfnWndProc = ::GlowWndProc;
        wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
        wndcls.hInstance = appInstance;
        wndcls.hIcon = Q_NULL;
        wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        wndcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wndcls.lpszMenuName = Q_NULL;
        wndcls.lpszClassName = reinterpret_cast<const wchar_t*>(clsName.utf16());

        if (!::RegisterClassW(&wndcls))
            return false;
    }

    m_hWnd = ::CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_LAYERED | 0x08000000L, reinterpret_cast<const wchar_t*>(clsName.utf16()),
        0, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0/*x*/, 0/*y*/, 0/*w*/, 0/*h*/,
        parentHWnd, NULL, appInstance, Q_NULL);

    if (m_hWnd == Q_NULL)
        return false;

    m_hashGlow.insert(m_hWnd, this);
    m_size = QSize(0, 0);

    return true;
}

void WindowTitleBarGlowWindow::destroyWindow()
{
    m_hashGlow.remove(m_hWnd);
    DeleteObject(m_hBitmap);
    m_hBitmap = Q_NULL;
    if (hWnd() != Q_NULL)
        ::DestroyWindow(hWnd());
}

void WindowTitleBarGlowWindow::updateWindowPos(HDWP hDWP)
{
    RECT rect;
    ::GetWindowRect(m_manager->hWnd(), &rect);
    QRect rectWindow(QPoint(rect.left, rect.top), QPoint(rect.right, rect.bottom));
    QSize szBorders(m_manager->borderSize());

    switch (m_section)
    {
        case Qt::LeftSection:
            rectWindow.setRight(rectWindow.left());
            rectWindow.setLeft(rectWindow.left() - (szBorders.width()));
            break;
        case Qt::TopSection:
            rectWindow.setBottom(rectWindow.top());
            rectWindow.setTop(rectWindow.top() - szBorders.height());
            rectWindow.setLeft(rectWindow.left() - szBorders.width());
            rectWindow.setRight(rectWindow.right() + szBorders.width());
            break;
        case Qt::RightSection:
            rectWindow.setLeft(rectWindow.right());
            rectWindow.setRight(rectWindow.right() + szBorders.width());
            break;
        case Qt::BottomSection:
            rectWindow.setTop(rectWindow.bottom());
            rectWindow.setBottom(rectWindow.bottom() + szBorders.height());
            rectWindow.setLeft(rectWindow.left() - szBorders.width());
            rectWindow.setRight(rectWindow.right() + szBorders.width());
            break;
        default:
            break;
    }

    DWORD dwFlags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
    ::DeferWindowPos(hDWP, hWnd(), m_manager->hWnd(), rectWindow.left(),
        rectWindow.top(), rectWindow.width(), rectWindow.height(), dwFlags);
}

static HBITMAP qtn_CreateBitmap_32(const QSize& size, void** pBits)
{
    if (pBits != Q_NULL)
        *pBits = Q_NULL;

    if (size.width() <= 0 || size.height() == 0)
        return Q_NULL;

    BITMAPINFO bi;
    memset(&bi, 0, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size.width();
    bi.bmiHeader.biHeight = size.height();
    bi.bmiHeader.biSizeImage = size.width() * abs(size.height());
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    LPVOID pData = Q_NULL;
    HBITMAP hbmp = ::CreateDIBSection(Q_NULL, &bi, DIB_RGB_COLORS, &pData, Q_NULL, 0);

    if (pData != Q_NULL && hbmp != Q_NULL && pBits != Q_NULL)
        *pBits = pData;
    return hbmp;
}

void WindowTitleBarGlowWindow::createBitmapShadow()
{
    if (m_size.width() == 0 || m_size.height() == 0)
        return;

    BITMAP bmp;
    memset(&bmp, 0, sizeof(BITMAP));

    if (m_hBitmap != Q_NULL)
        ::GetObject(m_hBitmap, sizeof(BITMAP), &bmp);

    if (bmp.bmWidth < m_size.width() || bmp.bmHeight < m_size.height())
    {
        LPBYTE pBits = NULL;
        HBITMAP hBitmap = qtn_CreateBitmap_32(m_size, (void**)&pBits);
        if (hBitmap == NULL)
            return;
        DeleteObject(m_hBitmap);
        m_hBitmap = hBitmap;
    }

    HDC hClientDC = ::GetDC(m_hWnd);
    HDC hDC = ::CreateCompatibleDC(hClientDC);

    HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hDC, m_hBitmap);
    QRect rect(QPoint(0, 0), m_size);
    QSize szBorders(m_manager->borderSize());
    RECT rc = { 0, 0, m_size.width(), m_size.height() };

    ::FillRect(hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));

    switch (m_section)
    {
        case Qt::LeftSection:
            rect.setRight(rect.right() + szBorders.width() * 2 + 1);
            rect.setTop(rect.top() - szBorders.height());
            rect.setBottom(rect.bottom() + szBorders.height());
            break;
        case Qt::TopSection:
            rect.setBottom(rect.bottom() + szBorders.height() * 2 + 1);
            break;
        case Qt::RightSection:
            rect.setLeft(rect.left() - szBorders.width() * 2 + 1);
            rect.setTop(rect.top() - szBorders.height());
            rect.setBottom(rect.bottom() + szBorders.height());
            break;
        case Qt::BottomSection:
            rect.setTop(rect.top() - szBorders.height() * 2 + 1);
            break;
        default:
            break;
    }

    drawGlow(hDC, rect, 0);

//    RECT rc1 = { rect.left(), rect.top(), rect.right(), rect.bottom() };
//    ::SetBkColor(hDC, RGB(0, 255, 0));
//    ::ExtTextOutW(hDC, 0, 0, ETO_OPAQUE, &rc1, Q_NULL, 0, Q_NULL);

    ::SelectObject(hDC, hBitmapOld);
    ::DeleteDC(hDC);
    ::ReleaseDC(m_hWnd, hClientDC);
}

void WindowTitleBarGlowWindow::paintGlow(bool updateOnly)
{
    RECT rect;
    ::GetWindowRect(m_hWnd, &rect);
    QRect rectWindow(QPoint(rect.left, rect.top), QPoint(rect.right - 1, rect.bottom - 1));

    QSize size(rectWindow.size());
    bool sizeChanged = size != m_size && size.width() != 0 && size.height() != 0;

    if (!sizeChanged && !updateOnly)
        return;

    if (sizeChanged)
    {
        if (!size.isNull())
            m_size = size;
        createBitmapShadow();
    }

    if (m_size.isEmpty())
        return;

    HDC hClientDC = ::GetDC(m_hWnd);
    HDC hDC = ::CreateCompatibleDC(hClientDC);

    HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hDC, m_hBitmap);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = m_manager->m_transparency;
    bf.AlphaFormat = LWA_COLORKEY;

    SIZE  sz = { m_size.width(), m_size.height() };
    POINT point = { 0, 0 };
    UpdateLayeredWindow(m_hWnd, NULL, 0, &sz, hDC, &point, 0, &bf, 0x02);

    ::SelectObject(hDC, hBitmapOld);

    ::DeleteDC(hDC);
    ::ReleaseDC(m_hWnd, hClientDC);
}

void WindowTitleBarGlowWindow::drawGlow(HDC hDC, const QRect& rc, uint /*index*/, byte /*alpha*//* = 255*/)
{
    QRect rect(rc);
    ImageHDC paintImage(rect.width(), rect.height(), ImageHDC::systemFormat());
    QPainter p(&paintImage.image());
    if (m_manager->m_color != Qt::transparent)
    {
        QLinearGradient gradient;
        switch (m_section)
        {
            case Qt::LeftSection:
                gradient.setStart(rect.bottomRight());
                gradient.setFinalStop(rect.bottomLeft());
                break;
            case Qt::TopSection:
                gradient.setStart(rect.bottomRight());
                gradient.setFinalStop(rect.topRight());
                break;
            case Qt::RightSection:
                gradient.setStart(rect.bottomLeft());
                gradient.setFinalStop(rect.bottomRight());
                break;
            case Qt::BottomSection:
                gradient.setStart(rect.topRight());
                gradient.setFinalStop(rect.bottomRight());
                break;
            default:
                break;
        }
        QColor clr = m_manager->m_color;
        clr.setAlpha(127);
        gradient.setColorAt(0, clr);
        gradient.setColorAt(1, QColor(255, 255, 255, 0));
        p.fillRect(rect, gradient);
    }
    else
    {
        p.fillRect(rect, m_clrTransparent);
    }
    BitBlt(hDC, rect.left(), rect.top(), rect.width(), rect.height(), paintImage.hdc(), rect.left(), rect.top(), SRCCOPY);
}

Qt::WindowFrameSection WindowTitleBarGlowWindow::hitTestToSection(const QPoint& point) const
{
    switch(hitTest(point))
    {
        case HTLEFT: 
            return Qt::LeftSection;
        case HTRIGHT:
            return Qt::RightSection;
        case HTTOPLEFT:
            return Qt::TopLeftSection;
        case HTTOPRIGHT:
            return Qt::TopRightSection;
        case HTBOTTOMLEFT:
            return Qt::BottomLeftSection;
        case HTBOTTOMRIGHT:
            return Qt::BottomRightSection;
        case HTTOP:
            return Qt::TopSection;
        case HTBOTTOM:
            return Qt::BottomSection;
        default:
            return Qt::NoSection;
    }
}

uint WindowTitleBarGlowWindow::hitTest(const QPoint& point) const
{
    if (!::IsWindowVisible(m_hWnd))
        return HTNOWHERE;

    RECT rect;
    ::GetWindowRect(m_hWnd, &rect);
    QRect rectWindow(QPoint(rect.left, rect.top), QPoint(rect.right - 1, rect.bottom - 1));

    if (!rectWindow.contains(point))
        return HTNOWHERE;

    UINT hit = HTNOWHERE;

    QSize szBorders = m_manager->borderSize();
    szBorders.setWidth(szBorders.width() + m_manager->borderSize().width() - GetSystemMetrics(SM_CXFRAME));
    szBorders.setHeight(szBorders.height() - GetSystemMetrics(SM_CYFRAME));

    switch (m_section)
    {
        case Qt::LeftSection:
        case Qt::RightSection:
            hit = m_section == Qt::LeftSection ? HTLEFT : HTRIGHT;

            if (rectWindow.top() <= point.y() && point.y() <= (rectWindow.top() + szBorders.height()))
                hit = m_section == Qt::LeftSection ? HTTOPLEFT : HTTOPRIGHT;
            else if ((rectWindow.bottom() - szBorders.height()) <= point.y() && point.y() <= rectWindow.bottom())
                hit = m_section == Qt::LeftSection ? HTBOTTOMLEFT : HTBOTTOMRIGHT;
            break;

        case Qt::TopSection:
        case Qt::BottomSection:
            hit = m_section == Qt::TopSection ? HTTOP : HTBOTTOM;

            if (rectWindow.left() <= point.x() && point.x() <= (rectWindow.left() + szBorders.width()))
                hit = m_section == Qt::TopSection ? HTTOPLEFT : HTBOTTOMLEFT;
            else if ((rectWindow.right() - szBorders.width()) <= point.x() && point.x() <= rectWindow.right())
                hit = m_section == Qt::TopSection ? HTTOPRIGHT : HTBOTTOMRIGHT;
            break;
        default:
            break;
    }
    return hit;
}


/* WindowTitleBarGlowManager */
WindowTitleBarGlowManager::WindowTitleBarGlowManager(WindowTitleBarPrivateWin* titleBar)
    : m_visible(false)
    , m_enabled(false)
    , m_transparency(255)
    , m_titleBar(titleBar)
    , m_color(Qt::transparent)
{
    Q_ASSERT(titleBar != Q_NULL);
#if 0
    uint val = qApp->style()->property("qtn_win32_glow").toUInt();
    m_enabled = val == 2 || val == 1;
#endif

    for (int i = 0; i < GLOW_COUNT; i++)
        m_arrayWnd[i] = Q_NULL;
}

WindowTitleBarGlowManager::~WindowTitleBarGlowManager()
{
    deleteGlowWindows();
}

bool WindowTitleBarGlowManager::isVisible() const
{
    return m_visible;
}

void WindowTitleBarGlowManager::setVisible(bool visible)
{
    if (m_visible == visible)
        return;
    m_visible = visible;
    showGlow(m_visible);
}

bool WindowTitleBarGlowManager::isEnabled() const
{
    return m_enabled;
}

void WindowTitleBarGlowManager::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;

    if (m_enabled)
    {
        installWindow();
        setVisible(::IsWindowVisible(hWnd()));
    }
    else
    {
        resetWindow();
        deleteGlowWindows();
    }
}

void WindowTitleBarGlowManager::setColor(const QColor& color)
{
    if (m_color == color)
        return;

    m_color = color;

    uint val = qApp->style()->property("qtn_win32_glow").toUInt();
    if (val == 1)
        m_color = Qt::transparent;

    for (int i = 0; i < GLOW_COUNT; i++)
    {
        WindowTitleBarGlowWindow* glowWnd = m_arrayWnd[i];
        if (glowWnd && glowWnd->hWnd() != Q_NULL)
        {
            glowWnd->createBitmapShadow();
            if (::IsWindowVisible(glowWnd->hWnd()))
                glowWnd->paintGlow(true);
        }
    }
}

bool WindowTitleBarGlowManager::createGlowWindow(int index, Qt::WindowFrameSection section)
{
    bool result = true;
    m_arrayWnd[index] = new WindowTitleBarGlowWindow(this, section);
    result = m_arrayWnd[index]->create(hWnd());
    if (!result)
    {
        delete m_arrayWnd[index];
        m_arrayWnd[index] = Q_NULL;
    }
    return result;
}

void WindowTitleBarGlowManager::deleteGlowWindows()
{
    for (int i = 0; i < GLOW_COUNT; i++)
    {
        delete m_arrayWnd[i];
        m_arrayWnd[i] = Q_NULL;
    }
}

bool WindowTitleBarGlowManager::installWindow()
{
    if (!isEnabled())
        return false;

    bool result = true;
    if (result) result = createGlowWindow(0, Qt::LeftSection);
    if (result) result = createGlowWindow(1, Qt::TopSection);
    if (result) result = createGlowWindow(2, Qt::RightSection);
    if (result) result = createGlowWindow(3, Qt::BottomSection);

    m_transparency = 255;

    return result;
}

void WindowTitleBarGlowManager::resetWindow()
{
    for (int i = 0; i < GLOW_COUNT; i++)
    {
        if (WindowTitleBarGlowWindow* glowWnd = m_arrayWnd[i])
            glowWnd->destroyWindow();
    }
}

void WindowTitleBarGlowManager::updateWindowPos()
{
    if (!isVisible() || !isEnabled())
        return;

    if (m_color != Qt::transparent)
        RedrawWindow(hWnd(), Q_NULL, Q_NULL, RDW_NOFRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);

    HDWP hDWP = ::BeginDeferWindowPos(GLOW_COUNT);
    if (hDWP == Q_NULL)
        return;

    for (int i = 0; i < GLOW_COUNT; i++)
    {
        if (WindowTitleBarGlowWindow* glowWnd = m_arrayWnd[i])
        {
            if (!glowWnd->hWnd())
                continue;
            glowWnd->updateWindowPos(hDWP);
        }
    }
    ::EndDeferWindowPos(hDWP);
}

HWND WindowTitleBarGlowManager::hWnd() const 
{ 
    return (HWND)m_titleBar->winId();
}

WindowTitleBarPrivateWin* WindowTitleBarGlowManager::titleBar() const 
{ 
    return m_titleBar; 
}

void WindowTitleBarGlowManager::showGlow(bool show)
{
    Q_UNUSED(show)
    if (show)
    {
        updateWindowPos();
        return;
    }

    for (int i = 0; i < GLOW_COUNT; i++)
    {
        if (WindowTitleBarGlowWindow* glowWnd = m_arrayWnd[i])
        {
            HWND sideHWnd = glowWnd->hWnd();
            if (sideHWnd != Q_NULL && ::IsWindowVisible(sideHWnd))
                ::SetWindowPos(sideHWnd, hWnd(), 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
        }
    }
}

QSize WindowTitleBarGlowManager::systemBorders(DWORD dwStyle) const
{
    Q_UNUSED(dwStyle)
    const int fwidth = titleBar()->borderThickness();
    QMargins margins = qtn_getWindowFrameMargings(titleBar()->window());
#if (QT_VERSION > QT_VERSION_CHECK(5, 1, 0))
    margins -= qtn_toNativeDPI(QMargins(fwidth, fwidth, fwidth, fwidth), titleBar()->window());
#else
    QMargins marginsDpi = qtn_toNativeDPI(QMargins(fwidth, fwidth, fwidth, fwidth), titleBar()->window());
    margins.setLeft(margins.left() - marginsDpi.left());
    margins.setTop(margins.top() - marginsDpi.top());
    margins.setRight(margins.right() - marginsDpi.right());
    margins.setBottom(margins.bottom() - marginsDpi.bottom());
#endif
    return QSize(margins.left(), margins.right());
}

DWORD WindowTitleBarGlowManager::styleFrame(bool exStyle) const
{
    return hWnd() ? (DWORD)GetWindowLongPtrW(hWnd(), exStyle ? GWL_EXSTYLE : GWL_STYLE) : 0;
}

QSize WindowTitleBarGlowManager::borderSize() const
{
    if (m_size.isNull())
        return m_size;

    QSize size(0, 0);
    if (hWnd())
    {
        DWORD dwStyle = styleFrame();
        size = systemBorders(dwStyle);
    }
    return size;
}
