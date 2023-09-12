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


#ifndef QTN_WINDOWTITLEBAR_PRIVATE_H
#define QTN_WINDOWTITLEBAR_PRIVATE_H

#include <QScreen>
#include <QWidget>
#include <QStyleOptionTitleBar>

#include "QtitanDef.h"
#include "QtnPlatform.h"
#include "QtnWindowTitleBar.h"
#include "QtnBackstageWidget.h"

QTITAN_BEGIN_NAMESPACE

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
inline QMargins operator+(const QMargins &m1, const QMargins &m2) Q_DECL_NOTHROW
{
    return QMargins(m1.left() + m2.left(), m1.top() + m2.top(),
        m1.right() + m2.right(), m1.bottom() + m2.bottom());
}

inline QMargins operator-(const QMargins &m1, const QMargins &m2) Q_DECL_NOTHROW
{
    return QMargins(m1.left() - m2.left(), m1.top() - m2.top(),
        m1.right() - m2.right(), m1.bottom() - m2.bottom());
}
#endif


class WindowStyleHelperAbstract
{
public:
    WindowStyleHelperAbstract()
        : m_window(Q_NULL), m_destroyed(false) {}
    virtual ~WindowStyleHelperAbstract() {}
public:
    const QMargins& frameMargins() const { return m_frameMargins; }
    void setFrameMargins(const QMargins& margins) { m_frameMargins = margins; }
    QWidget* window() const { return m_window; }
    virtual void setWindow(QWidget* window) { m_window = window; }
    const QSize& windowSize() const { return m_windowSize; }
    void setWindowSize(const QSize& size) { m_windowSize = size; }
    virtual void update(const QRect& rect) = 0;
    virtual void update(const QRegion& region) = 0;
    virtual bool isActive() const = 0;
    virtual bool underMouse() const = 0;
    virtual void showSystemMenu(const QPoint& pos) = 0;
    bool isDestroyed() const { return m_destroyed; }
    void setDestroyed() { m_destroyed = true; }
protected:
    virtual void nonClientActivateEvent(bool active) = 0;
    virtual void paintNonClientEvent(QPainter* painter, Qt::WindowFrameSection section = Qt::NoSection) = 0;
    virtual void mousePressNonClientEvent(QMouseEvent* event) = 0;
    virtual void mouseReleaseNonClientEvent(QMouseEvent* event) = 0;
    virtual void mouseDoubleClickNonClientEvent(QMouseEvent* event) = 0;
    virtual void mouseMoveNonClientEvent(QMouseEvent* event) = 0;
    virtual void mouseNonClientOver(const QPoint& pos) = 0;
    virtual void mouseNonClientOut(const QPoint& pos) = 0;
    virtual void mouseContextNonClientEvent(QContextMenuEvent* event) = 0;
    virtual void resizeNonClientEvent(const QSize& windowSize) = 0;
private:
    QWidget* m_window;
    QMargins m_frameMargins;
    QSize m_windowSize;
    bool m_destroyed;
};

class FrameStyleOption;

class WindowTitleBarPrivate : public QObject, public WindowStyleHelperAbstract
{
    Q_OBJECT
public:
    friend class ExendedView;
    friend class WindowTitleBar;
    WindowTitleBarPrivate();
    virtual ~WindowTitleBarPrivate();
public:
    bool isInitialized() const { return m_initialized; }
    virtual void setWindow(QWidget* window);
    void setVisible(bool visible);
    bool isVisible() const;
    void setResizible(bool resizible);
    bool isResizible() const;
    virtual bool underMouse() const;
    void setTitleBarWidget(QWidget* widget);
    QWidget* titleBarWidget() const;
    QString titleBarText() const;
    WindowTitleBar::WidgetAlign titleBarWidgetAlign() const;
    void setTitleBarWidgetAlign(WindowTitleBar::WidgetAlign align);
    bool hasFocus() const;
    QRect getClientRect() const;
    QRegion getNonClientRegion() const;
    QRect getTitleBarRect();
    QStyle::SubControl hitTest(const QPoint& pos, Qt::WindowFrameSection& frameSection);
    bool styledFrame() const;
    void setStyledFrame(bool styled);
    bool blurBehindWindowEnabled() const;
    void setBlurBehindWindowEnabled(bool enabled);
    bool extendViewIntoTitleBar() const;
    void setExtendViewIntoTitleBar(bool extend);
    WindowTitleBar::SysButtonKind sysButtonKind() const;
    void setSysButtonKind(WindowTitleBar::SysButtonKind kind);
    const QColor& backgroundColor() const;
    void setBackgroundColor(const QColor& color);
    const QColor& textColor() const;
    void setTextColor(const QColor& color);
    QColor frameColor() const;
    const QMargins& systemFrameMargins() const;
    int borderThickness() const;
    int titleHeight() const;
    bool isMaximized() const;
    bool isFullScreen() const;
    void updateLayout();
    virtual void updateFrame(bool forceLayoutUpdate);
    virtual void update(const QRect& rect);
    virtual void update(const QRegion& region);
    void setCursor(const QCursor& cursor);
    void unsetCursor();
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    void paintMaterialBehindWindow(QPainter* painter, QWidget* w) const;
    void updateWidgetPosition();
    static WindowTitleBarPrivate* get(WindowTitleBar* titleBar);
protected:
    virtual void inputMethodEvent(QInputMethodEvent* event);
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual bool installWindow();
    virtual void resetWindow();
    virtual void nonClientActivateEvent(bool active);
    virtual void paintNonClientEvent(QPainter* painter, Qt::WindowFrameSection section = Qt::NoSection);
    virtual void mousePressNonClientEvent(QMouseEvent* event);
    virtual void mouseReleaseNonClientEvent(QMouseEvent* event);
    virtual void mouseDoubleClickNonClientEvent(QMouseEvent* event);
    virtual void mouseMoveNonClientEvent(QMouseEvent* event);
    virtual void mouseNonClientOver(const QPoint& pos);
    virtual void mouseNonClientOut(const QPoint& pos);
    virtual void mouseContextNonClientEvent(QContextMenuEvent* event);
    virtual void resizeNonClientEvent(const QSize& windowSize);
    virtual bool isGlowEnabled() const;
protected:
    virtual QRect getSubControlRect(QStyle::SubControl sc);
    virtual QStyle::SubControl getHoverSubControl(const QPoint& pos);
    virtual void styleChanged();
    virtual void updateSysButtonsRect();
    virtual QStyle::SubControl getHoverNativeSubControl(const QPoint& pos);
    virtual void paintTitleBarSysButton(QPainter* painter, const QStyleOptionTitleBar& option);
    virtual void paintTitleBarText(QPainter* painter, const QStyleOptionTitleBar& option);
    virtual void nativeCommand(QMouseEvent* event, Qt::WindowFrameSection frameSection) = 0;
    virtual QPoint mapFromGlobal(const QPoint& pos) const = 0;
    virtual QPoint mapToGlobal(const QPoint& pos) const = 0;
    virtual QPoint tranlateToNonClient(const QPoint& clientPos) = 0;
protected:
    virtual bool mouseEventFilter(QObject* object, QEvent* event);
    void updateInputMethod();
    void checkUnderMouse(const QPoint& pos);
    bool installWindowInternal();
    bool useStyleToPaintIconAndText() const;
    void processExtendViewEvent(QEvent* event);
    Qt::WindowFrameSection checkFrameSection(const QPoint& pos);
    void initTitleBarOption(QStyleOptionTitleBar& option);
    void initFrameOption(FrameStyleOption& frameOption, Qt::WindowFrameSection section = Qt::NoSection);
    void updateSubControl(const QPoint& pos);
    void setSubControlPressed(QStyle::SubControl sc);
    void subControlClicked(QMouseEvent* event);
    virtual void processContextMenuEvent(QMouseEvent* event);
    bool defaultResizeHandler();
    void updateTitleBarWidgetGeometry();
    bool containsPoint(const QPoint& pos);
    void updateExtendedViewGeometry();
    void updateCursor();
    void updateMaterialWidgets();
    QCursor getCursorForFrameSection(Qt::WindowFrameSection frameSection);
    int getSysMenuButtonWidth() const;
    bool hasCustomBackground() const;
    bool hasCustomForeground() const;
    const QPixmap& behindWindowBackground() const;
    void syncContentsMargins();
    bool m_initialized;
    bool m_visible;
    bool m_resizible;
    Qt::WindowFlags m_oldFlags;
    BackstageWidget* m_titleBarWidget;
    WindowTitleBar::WidgetAlign m_titleBarWidgetAlign;
    QRect m_textRect;
    Qt::WindowFrameSection m_hoveredFrameSection;
    QStyle::SubControl m_hovered;
    QStyle::SubControl m_pressed;
    bool m_nonClientUnderMouse;
    bool m_styledFrame;
    bool m_blurBehindWindowEnabled;
    bool m_extendViewIntoTitleBar;
    QMargins m_systemFrameMargins;
    QRect m_systemButtonsRect;
    int m_borderThickness;
    int m_titleHeight;
    int m_borderThicknessRequested;
    int m_titleHeightRequested;
    QIcon m_icon;
    bool m_maximized;
    bool m_fullScreen;
    WindowTitleBar::SysButtonKind m_sysButtonKind;
    QColor m_backgroundColor;
    QColor m_textColor;
    QPalette m_titleBarPalette;
    QMargins m_frameWindows10;
    QWidget* m_extendedView;
    QMargins m_origContentsMargins;
    bool m_hasCursor;
    QPixmap m_bluredDesktop;
    int m_eventsCount;
    bool m_QWizardSupports;
private Q_SLOTS:
    void  backstageWidgetUpdate(const QList<QRectF>& region);
    void screenChanged(QScreen* screen);
    friend class WindowTitleBarAutoUpdater;
};

class WindowTitleBarPrivateQt : public WindowTitleBarPrivate
{
public:
    WindowTitleBarPrivateQt();
    virtual ~WindowTitleBarPrivateQt();
    virtual bool isActive() const;
    virtual void showSystemMenu(const QPoint& pos);
protected:
    virtual bool installWindow();
    virtual void resetWindow();
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual void processContextMenuEvent(QMouseEvent* event);
    virtual void resizeNonClientEvent(const QSize& windowSize);
    virtual void nativeCommand(QMouseEvent* event, Qt::WindowFrameSection frameSection);
    virtual QPoint mapFromGlobal(const QPoint& pos) const;
    virtual QPoint mapToGlobal(const QPoint& pos) const;
    virtual QPoint tranlateToNonClient(const QPoint& clientPos);
    virtual bool mouseEventFilter(QObject* object, QEvent* event);
private:
    bool isDragMode() const;
    void startDrag(const QPoint& pos, Qt::WindowFrameSection frameSection);
    void stopDrag();
    void dragMove(const QPoint& pos);
    virtual void updateFrame(bool forceLayoutUpdate);
private:
    Qt::WindowFrameSection m_frameSection;
    QPoint m_startDragPos;
    QRect m_grabGeometry;
};

class ExendedView : public QWidget
{
public:
    ExendedView(WindowTitleBarPrivate* bar);
    virtual ~ExendedView();
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    WindowTitleBarPrivate* m_bar;
};

class WindowTitleBarAutoUpdater
{
public:
    WindowTitleBarAutoUpdater(WindowTitleBarPrivate* p)
        : m_p(p)
    {
        m_p->m_eventsCount++;
    }

    ~WindowTitleBarAutoUpdater()
    {
        m_p->m_eventsCount--;
    }
private:
    WindowTitleBarPrivate* m_p;
};

QTITAN_END_NAMESPACE

#endif //QTN_WINDOWTITLEBAR_PRIVATE_H


