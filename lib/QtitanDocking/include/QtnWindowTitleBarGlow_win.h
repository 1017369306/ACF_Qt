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
#ifndef QTN_WINDOWTITLEBARGLOWMANAGER_WIN_H
#define QTN_WINDOWTITLEBARGLOWMANAGER_WIN_H

#include <QSize>
#include <QColor>
#include <qt_windows.h>
#include "QtnWindowTitleBar_win.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

#define GLOW_COUNT 4
class WindowTitleBarGlowWindow;
/* WindowTitleBarGlowManager  */
class WindowTitleBarGlowManager
{
public:
    WindowTitleBarGlowManager(WindowTitleBarPrivateWin* titleBar);
    ~WindowTitleBarGlowManager();
public:
    bool isVisible() const;
    void setVisible(bool visible);
    bool isEnabled() const;
    void setEnabled(bool enabled);
    void setColor(const QColor& color);
    bool installWindow();
    void resetWindow();
    void updateWindowPos();
    HWND hWnd() const;
    WindowTitleBarPrivateWin* titleBar() const;
protected:
    void showGlow(bool show);
    bool createGlowWindow(int index, Qt::WindowFrameSection section);
    void deleteGlowWindows();
    DWORD styleFrame(bool exStyle = false) const;
    QSize systemBorders(DWORD dwStyle) const;
    QSize borderSize() const;
protected:
    bool m_visible;
    bool m_enabled;
    byte m_transparency;
    WindowTitleBarPrivateWin* m_titleBar;
    WindowTitleBarGlowWindow* m_arrayWnd[GLOW_COUNT];
    QSize m_size;
    QColor m_color;
private:
    friend class WindowTitleBarGlowWindow;
};

QTITAN_END_NAMESPACE

#endif // QTN_WINDOWTITLEBARGLOWMANAGER_WIN_H
