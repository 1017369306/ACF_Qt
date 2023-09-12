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
#ifndef QTN_WINDOWTITLEBAR_WIN_H
#define QTN_WINDOWTITLEBAR_WIN_H


#include "QtnWindowTitleBarPrivate.h"

#ifdef QTN_PRIVATE_HEADERS
#include <qpa/qplatformwindow.h>
#include <qpa/qplatformnativeinterface.h>
#endif

QTITAN_BEGIN_NAMESPACE

class WindowTitleBarGlowManager;
class WindowsStyleMouseHandlerWin;

/* WindowTitleBarPrivateWin */
class WindowTitleBarPrivateWin : public WindowTitleBarPrivate,
                                 public WindowsHook
{
public:
    WindowTitleBarPrivateWin();
    virtual ~WindowTitleBarPrivateWin();
public:
    virtual void update(const QRect& rect);
    virtual void update(const QRegion& region);
    virtual bool isActive() const;
    virtual void showSystemMenu(const QPoint& pos);
public:
    bool isAeroGlassEnabled() const;
    void setAeroGlassEnabled(bool enabled);
    void setTransitionsEffectEnabled(bool enabled);

    virtual void nativeCommand(QMouseEvent* event, Qt::WindowFrameSection frameSection);
    void execNativeCommand(const QPoint& pos);
protected:
    bool isCompositionEnabled() const;
    bool isCompositionSupported() const;
    virtual LRESULT winProc(HWND hWnd, uint nMessage, WPARAM wParam, LPARAM lParam);
    virtual void winEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
    void initializeWindowUI();
    void finalizeWindowUI();
    void forceNonClientSizeUpdate();
    bool updateDwmSettings(bool enabled);
    void updateWindowSize();
    virtual void styleChanged();
    virtual void updateSysButtonsRect();
    virtual QStyle::SubControl getHoverNativeSubControl(const QPoint& pos);

    bool handleMessage(MSG* msg, LRESULT& result);
    bool handleNonClientCreate();
    bool handleNonClientDestroy();
    bool handleNonClientPaint(HRGN updateRegion);
    bool handleNonClientCalcSize(NCCALCSIZE_PARAMS* sizeParams, bool calcRects);
    bool handlePaint(); 
    bool handleNonClientMouseEvent(MSG* msg, LRESULT &outResult);
    int handleNonClientHitTest(LPPOINT ptHit);

    void setRedraw(bool redraw);
    bool isIconic() const;
    void enforceMenuBackground();
    void paintNonClientAreaPart(HDC hdc, const QRegion& clipRegion, const QMargins& margins, Qt::WindowFrameSection section);
    void paintNonClientArea(HDC hdc);
protected:
    virtual bool installWindow();
    virtual void resetWindow();
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual void mouseMoveNonClientEvent(QMouseEvent* event);
    virtual void mouseContextNonClientEvent(QContextMenuEvent* event);
    virtual void resizeNonClientEvent(const QSize& windowSize);
    virtual QPoint mapFromGlobal(const QPoint& pos) const;
    virtual QPoint mapToGlobal(const QPoint& pos) const;
    virtual QPoint tranlateToNonClient(const QPoint& clientPos);
    virtual bool isGlowEnabled() const;
private:
    void checkMaximized();
    void fixupQtInternals(bool forceUpdate = true);
    void setTrackMouseEnabled(bool enabled);
    bool hasMaximizedStyle() const;
    bool hasFullScreenStyle() const;
    bool hasSysMenuStyle() const;
    void setSysMenuStyle(bool enabled);
    QMargins extraFrameMargins() const;
    QMargins invisibleNonClientMaximizeMargins() const;
    QMargins calcSystemFrameMargins() const;
    QMargins calcWidndows10FrameMargins() const;
    bool nativeCommandNeeded() const;
private:
    bool m_isActive;
    bool m_isFinalized;
    bool m_aeroGlassEnabled;
    bool m_lockStyleChange;
    bool m_fixupQtNeeded;
    bool m_mouseTracking;
    bool m_fillClientArea;

    WindowTitleBarGlowManager* m_glowManager;
    LRESULT m_hitTest;
    bool m_ignoreSysCommand;
    bool m_sysCommandStatus;
    int m_sysCommand;
};

QTITAN_END_NAMESPACE

#endif //QTN_WINDOWTITLEBAR_WIN_H

