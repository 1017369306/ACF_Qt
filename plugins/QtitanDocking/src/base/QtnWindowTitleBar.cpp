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

#include <QApplication>
#include <QPainter>
#include <QAbstractScrollArea>
#include <QStyle>
#include <QWizard>
#include <QScreen>
#include "QtnStyleOption.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#ifndef Q_OS_WIN
#define QTN_HAS_DRAG_LOOP
#endif
#endif

#ifdef QTN_HAS_DRAG_LOOP
#include <QBasicTimer>
#include <QAbstractNativeEventFilter>
#endif

#ifdef QTN_PRIVATE_HEADERS
#include <qpa/qplatformtheme.h>
#include <private/qguiapplication_p.h>
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QBackingStore>
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
#include <QOperatingSystemVersion>
#endif

#include "QtnPlatform.h"
#include "QtnWindowTitleBarPrivate.h"
#include "QtnCommonStyle.h"
#include "QtnMaterialWidget.h"
#ifdef Q_OS_WIN
#include "QtnWindowTitleBar_win.h"
#endif

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

/*!
    \class BaseEventArgs
    \brief Base class for the arguments raised via signal functions.
    \inmodule QtitanBase
*/

/*!
    \fn BaseEventArgs::BaseEventArgs()
    Constructs BaseEventArgs class.
*/

/*!
    \fn bool BaseEventArgs::isHandled() const
    Returns whether the event is processed or not.
*/

/*!
    \fn void BaseEventArgs::setHandled(bool handled)
    Sets whether the event is processed or not to \a handled.
*/

/*!
    \namespace Qtitan
    \inmodule QtitanBase
    \brief The Qtitan namespace contains miscellaneous identifiers used throughout the Qtitan components.
*/

#ifndef QTITAN_NO_PROCESS
/*!
Returns system environment variable by \a key from current process.
*/
QString Qtitan::getSystemEnvironmentVariable(const QString& key)
{
    QStringList all = QProcess::systemEnvironment();
    for (QStringList::const_iterator it = all.constBegin(); it != all.constEnd(); ++it)
    {
        QStringList pair = it->split(QStringLiteral("="));
        if (pair.size() != 2)
            continue;
        if (key.compare(pair[0], Qt::CaseInsensitive) == 0)
            return pair[1];
    }
    return QString();
}

/*!
Returns environment variable by \a key from \a process.
*/
QString Qtitan::getEnvironmentVariable(QProcess* process, const QString& key)
{
    QStringList all = process->environment();
    for (QStringList::const_iterator it = all.constBegin(); it != all.constEnd(); ++it)
    {
        QStringList pair = it->split(QStringLiteral("="));
        if (pair.size() != 2)
            continue;
        if (key.compare(pair[0], Qt::CaseInsensitive) == 0)
            return pair[1];
    }
    return QString();
}

/*!
Sets environment variable using \a key and \a value for \a process.
*/
void Qtitan::setEnvironmentVariable(QProcess* process, const QString& key, const QString& value)
{
    QStringList all = process->environment();
    QStringList::iterator it = all.begin();
    for (; it != all.end(); ++it)
    {
        QStringList pair = it->split(QStringLiteral("="));
        if (pair.size() != 2)
            continue;
        if (key.compare(pair[0], Qt::CaseInsensitive) == 0)
            break;
    }
    if (it != all.end())
        *it = QStringLiteral("%1=%2").arg(key).arg(value);
    else
        all.push_back(QStringLiteral("%1=%2").arg(key).arg(value));
    process->setEnvironment(all);
}
#endif

/*!
\class WindowTitleBar
\inmodule QtitanBase
\brief Provides customization APIs for the top-level window title bar.

The customization allows you specify the colors for title bar buttons,
control the thickness of the frame or the height of the title bar,
add custom QWidget to the title bar and even extend window content into the title bar area.
\code 

QMainWindow window;

QLineEdit* edit = new QLineEdit();
edit->setAttribute(Qt::WA_NoSystemBackground, true);
edit->setContentsMargins(0, 3, 0, 3);
edit->setPlaceholderText("Search here...");
#ifdef Q_OS_WIN
edit->setFrame(false);
#endif

Qtitan::WindowTitleBar* titleBar = Qtitan::WindowTitleBar::get(&window);
titleBar->setSysButtonKind(WindowTitleBar::BackButton);
titleBar->setWidget(edit, WindowTitleBar::AlignRight);
titleBar->show();

window.show();

\endcode

*/
WindowTitleBar::WindowTitleBar(QWidget* window)
    : QObject(window), m_p(Q_NULL)
{
    Q_ASSERT(window != Q_NULL && window->isWindow());
    CommonStyle::ensureStyle();
#ifdef Q_OS_WIN
    m_p = new WindowTitleBarPrivateWin();
#else
    m_p = new WindowTitleBarPrivateQt();
#endif
    m_p->setWindow(qobject_cast<QWidget *>(parent()));
    connect(window, SIGNAL(destroyed()), this, SLOT(windowDestroyed()));
}

WindowTitleBar* WindowTitleBar::get(QWidget* window)
{
    WindowTitleBar* ret = WindowTitleBar::find(window);
    if (ret == Q_NULL)
        ret = new WindowTitleBar(window);
    return ret;
}

void QTITAN_PREPEND_NAMESPACE(qtn_updateTitleBarWidgetPosition)(QWidget* window)
{
    WindowTitleBar* titleBar = WindowTitleBar::find(window);
    if (titleBar != Q_NULL)
        WindowTitleBarPrivate::get(titleBar)->updateWidgetPosition();
}

WindowTitleBar* WindowTitleBar::find(const QWidget* window)
{
    if (window == Q_NULL)
        return Q_NULL;
    Q_ASSERT(window->isWindow());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    WindowTitleBar* ret = window->findChild<WindowTitleBar *>(QString(), Qt::FindDirectChildrenOnly);
#else
    WindowTitleBar* ret = window->findChild<WindowTitleBar *>(QString());
#endif
    return ret;
}

WindowTitleBar::~WindowTitleBar()
{
    Q_DELETE_AND_NULL(m_p);
}

void WindowTitleBar::setVisible(bool visible)
{
    m_p->setVisible(visible);
}

bool WindowTitleBar::isVisible() const
{
    return m_p->isVisible();
}

void WindowTitleBar::setResizible(bool resizible)
{
    m_p->setResizible(resizible);
}

bool WindowTitleBar::isResizible() const
{
    return m_p->isResizible();
}

void WindowTitleBar::show()
{
    setVisible(true);
}

void WindowTitleBar::hide()
{
    setVisible(false);
}

/*!
Updates for repaint the non-client area of the window.
*/
void WindowTitleBar::update()
{
    m_p->update(m_p->getNonClientRegion());
}

void WindowTitleBar::setWidget(QWidget* widget, WindowTitleBar::WidgetAlign align)
{
    m_p->setTitleBarWidget(widget);
    m_p->setTitleBarWidgetAlign(align);
    update();
}

QWidget* WindowTitleBar::widget() const
{
    return m_p->titleBarWidget();
}

bool WindowTitleBar::styledFrame() const
{
    return m_p->styledFrame();
}

void WindowTitleBar::setStyledFrame(bool styled)
{
    m_p->setStyledFrame(styled);
}

bool WindowTitleBar::extendViewIntoTitleBar() const
{
    return m_p->extendViewIntoTitleBar();
}

void WindowTitleBar::setExtendViewIntoTitleBar(bool extend)
{
    m_p->setExtendViewIntoTitleBar(extend);
}

/*!
Sets the \a height of the title bar (top part of the window non-client area). 
If the value is not specified, then the height of the title bar will be equal 
to the height of the system title bar. If the styledFrame() property returns true, 
the height of the title bar will be retrieved from the window style using the QStyle::PM_TitleBarHeight metric.
*/
void WindowTitleBar::setTitleHeight(int height)
{
    m_p->m_titleHeightRequested = height;
}

/*!
Returns the height of the title bar.
*/
int WindowTitleBar::titleHeight() const
{
    return m_p->m_titleHeight;
}

/*!
Sets the frame \a thickness of the window non-client area.
If the value is not specified, then the thickness will be equal to the thickness of the system window frame.
If the styledFrame() property returns true, the border thickness will be retrieved from the window style using the QStyle::PM_MdiSubWindowFrameWidth metric.
*/
void WindowTitleBar::setBorderThickness(int thickness)
{
    if (m_p->m_borderThicknessRequested != thickness)
    {
        m_p->m_borderThicknessRequested = thickness;
        m_p->styleChanged();
    }
}

/*!
Returns the window non-client area frame thickness.
*/
int WindowTitleBar::borderThickness() const
{
    return m_p->m_borderThickness;
}

bool WindowTitleBar::isSysMenuButtonVisible() const
{
    return sysButtonKind() == SysMenuButton;
}

void WindowTitleBar::setSysMenuButtonVisible(bool visible)
{
    m_p->setSysButtonKind(visible ? SysMenuButton : SysButtonHidden);
}

WindowTitleBar::SysButtonKind WindowTitleBar::sysButtonKind() const
{
    return m_p->sysButtonKind();
}

void WindowTitleBar::setSysButtonKind(SysButtonKind kind)
{
    m_p->setSysButtonKind(kind);
}

const QColor& WindowTitleBar::backgroundColor() const
{
    return m_p->backgroundColor();
}

void WindowTitleBar::setBackgroundColor(const QColor& color)
{
    m_p->setBackgroundColor(color);
}

const QColor& WindowTitleBar::textColor() const
{
    return m_p->textColor();
}

void WindowTitleBar::setTextColor(const QColor& color)
{
    m_p->setTextColor(color);
}

QColor WindowTitleBar::frameColor() const
{
    return m_p->frameColor();
}

QPalette WindowTitleBar::palette() const
{
    QStyleOptionTitleBar option;
    initStyleOption(&option);
    return option.palette;
}

void WindowTitleBar::initStyleOption(QStyleOptionTitleBar* option) const
{
    if (option == Q_NULL)
    {
        Q_ASSERT(false);
        return;
    }
    m_p->initTitleBarOption(*option);
}

bool WindowTitleBar::isQWizardSupports() const
{
    return m_p->m_QWizardSupports;
}

void WindowTitleBar::setQWizardSupports(bool supports) const
{
    m_p->m_QWizardSupports = supports;
}

void WindowTitleBar::removeAndDelete()
{
    m_p->setWindow(Q_NULL);
    if (m_p->m_eventsCount > 0)
        deleteLater();
    else
        delete this;
}

QRect WindowTitleBar::systemButtonsRect() const
{
    if (m_p->m_systemButtonsRect.isEmpty())
        m_p->updateSysButtonsRect();
    return m_p->m_systemButtonsRect;
}

QTITAN_BEGIN_NAMESPACE
class UnixDragEventLoop;
class QWidgetEx : public QWidget
{
public:
    friend class QTITAN_PREPEND_NAMESPACE(WindowTitleBarPrivateQt);
    friend class QTITAN_PREPEND_NAMESPACE(WindowTitleBarPrivate);
    friend class QTITAN_PREPEND_NAMESPACE(UnixDragEventLoop);
};

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QWindowEx : public QWindow
{
public:
    friend class QTITAN_PREPEND_NAMESPACE(WindowTitleBarPrivate);
};
#endif

#ifdef QTN_HAS_DRAG_LOOP
static QEventLoop* g_dragLoop = Q_NULL;

class UnixDragEventLoop: public QEventLoop, QAbstractNativeEventFilter
{
public:
    UnixDragEventLoop(QWidget* window) : m_window(window), m_init(true), m_moveEventsCount(0)
    {
        m_currentPos = QCursor::pos();
        qApp->installNativeEventFilter(this);
        m_window->installEventFilter(this);
        m_timer.start(60, this);
    }

    ~UnixDragEventLoop() override
    {
        qApp->removeNativeEventFilter(this);
        m_window->removeEventFilter(this);
        m_window = Q_NULL;
    }

    void timerEvent(QTimerEvent *event) override
    {
        if (event->timerId() == m_timer.timerId())
        {
            QPoint pos = QCursor::pos();
            if (m_currentPos == pos)
                return;
            QMoveEvent event(m_currentPos, pos);
            m_currentPos = pos;
            static_cast<QWidgetEx *>(m_window)->moveEvent(&event);
        }
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == static_cast<QObject *>(m_window))
        {
            if (event->type() == QEvent::Move)
            {
                if (m_moveEventsCount < 2)
                    m_moveEventsCount++;
                else
                    m_timer.stop();
            }
        }
        return QEventLoop::eventFilter(watched, event);
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override
#else
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override
#endif
    {
        Q_UNUSED(eventType);
        Q_UNUSED(message);
        Q_UNUSED(result);
        checkMouseButtons();
        return false;
    }

    void checkMouseButtons()
    {
        if (m_init)
        {
            Qt::MouseButtons buttons = qtn_getMouseButtons();
            if (!(buttons & Qt::LeftButton))
            {
                quit();
                m_init = false;
            }
        }
    }
private:
    QWidget* m_window;
    bool m_init;
    int m_moveEventsCount;
    QBasicTimer m_timer;
    QPoint m_currentPos;
};
#endif

#ifdef Q_OS_MAC

class MACOSWantsLayer
{
public:
    MACOSWantsLayer()
    {
        qputenv("QT_MAC_WANTS_LAYER", "1");
    }
};
static  MACOSWantsLayer macWantsLayer;

static void qtn_updateNSView(QWidget* window)
{
    if (window == Q_NULL)
        return;
    qtn_set_window_hook(window);
    qtn_set_window_frameless_mac(window);
}
#endif

QTITAN_END_NAMESPACE

void WindowTitleBar::startDrag(QWidget* window, const QPoint& pos)
{
    Q_ASSERT(window != Q_NULL && window->isWindow());
#ifdef Q_OS_WASM
    Q_UNUSED(window);
    Q_UNUSED(pos);
#endif

#ifdef QTN_HAS_DRAG_LOOP
    Q_ASSERT(g_dragLoop == Q_NULL);
    if (g_dragLoop != Q_NULL)
        return;
#endif
#ifdef Q_OS_WIN
    QMouseEvent event(QEvent::MouseButtonPress, window->mapFromGlobal(pos), pos,
        Qt::MouseButton(), Qt::MouseButtons(), Qt::KeyboardModifiers());
    int command = qtn_get_sys_command_win(window, &event, Qt::TitleBarArea);
    if (command != -1)
        qtn_sys_command_win(window, command, pos, true);
#endif
#ifdef Q_OS_MAC
    QApplication::processEvents();
    bool ret = qtn_window_start_native_drag_mac(window, pos);
    Q_UNUSED(ret);
#endif
#ifdef Q_OS_LINUX
    bool ret = qtn_window_start_native_drag_linux(window, pos, Qt::TitleBarArea);
    Q_UNUSED(ret);
#endif
#ifdef QTN_HAS_DRAG_LOOP
    UnixDragEventLoop loop(window);
    QObject::connect(qApp, SIGNAL(aboutToQuit()), &loop, SLOT(quit()));
    g_dragLoop = &loop;
    loop.exec(); 
    g_dragLoop = Q_NULL;
#endif
}

bool WindowTitleBar::blurBehindWindowEnabled() const
{
    return m_p->blurBehindWindowEnabled();
}

void WindowTitleBar::setBlurBehindWindowEnabled(bool enabled)
{
    m_p->setBlurBehindWindowEnabled(enabled);
}

void WindowTitleBar::windowDestroyed()
{
    m_p->setDestroyed();
    m_p->setWindow(Q_NULL);
    if (m_p->m_eventsCount > 0)
    {
        setParent(Q_NULL);
        deleteLater();
    }
}

/* WindowTitleBarPrivate */
WindowTitleBarPrivate::WindowTitleBarPrivate() 
    : WindowStyleHelperAbstract(),
    m_initialized(false),
    m_visible(false),
    m_resizible(true),
    m_oldFlags(Qt::WindowFlags()),
    m_titleBarWidget(Q_NULL),
    m_hovered(QStyle::SC_None),
    m_pressed(QStyle::SC_None),
    m_nonClientUnderMouse(false),
    m_styledFrame(true),
    m_blurBehindWindowEnabled(false),
    m_extendViewIntoTitleBar(false),
    m_systemFrameMargins(QMargins(0, 0, 0, 0)),
    m_borderThickness(-1),
    m_titleHeight(-1),
    m_borderThicknessRequested(-1),
    m_titleHeightRequested(-1),
    m_maximized(false),
    m_fullScreen(false),
    m_sysButtonKind(WindowTitleBar::SysMenuButton),
    m_backgroundColor(),
    m_frameWindows10(QMargins(0, 0, 0, 0)),
    m_extendedView(Q_NULL),
    m_hasCursor(false),
    m_eventsCount(0),
    m_QWizardSupports(true)
{
}

WindowTitleBarPrivate::~WindowTitleBarPrivate()
{
    Q_DELETE_AND_NULL(m_titleBarWidget);
    Q_DELETE_AND_NULL(m_extendedView);
}

bool WindowTitleBarPrivate::underMouse() const
{
    return m_nonClientUnderMouse;
}

bool WindowTitleBarPrivate::mouseEventFilter(QObject* object, QEvent* event)
{
    if (QWidget::mouseGrabber() != Q_NULL && QWidget::mouseGrabber() != window())
        return false;

    //if (QWidget::mouseGrabber() != Q_NULL)
    //    return false;

    bool result = false;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (object == window()->windowHandle())
#else
    if (object == window())
#endif
    {
        switch (event->type())
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        case QEvent::Leave:
#else
        case QEvent::HoverLeave:
#endif
        {
            const QPoint pos = mapFromGlobal(QCursor::pos());
            checkUnderMouse(pos);
        }
        break;
        case QEvent::NonClientAreaMouseMove:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        case QEvent::MouseMove:
#else
        case QEvent::HoverMove:
#endif
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QMouseEvent* origevent = static_cast<QMouseEvent*>(event);
            MouseEventQt me(*origevent, tranlateToNonClient(origevent->pos()));
#else
            QHoverEvent* he = static_cast<QHoverEvent *>(event);
            QMouseEvent me(QEvent::MouseMove, tranlateToNonClient(he->pos()), 
                Qt::NoButton, Qt::NoButton,
                qApp->queryKeyboardModifiers());
#endif

            checkUnderMouse(me.pos());
            if (containsPoint(me.pos()))
            {
                mouseMoveNonClientEvent(&me);
                event->setAccepted(me.isAccepted());
                result = true;
            }
        }
        break;
        
        case QEvent::NonClientAreaMouseButtonPress:
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* origevent = static_cast<QMouseEvent*>(event);
            MouseEventQt me(*origevent, tranlateToNonClient(origevent->pos()));

            setSubControlPressed(QStyle::SC_None);
            checkUnderMouse(me.pos());

            if (defaultResizeHandler())
            {
                //The size of window resize grip can be larger than the frame width, 
                //so first we check that we can resize the window.
                nativeCommand(&me, m_hoveredFrameSection); //Native resize processing.
                event->accept();
                result = true;
            }
            else if (containsPoint(me.pos()))
            {
                mousePressNonClientEvent(&me);
                event->setAccepted(me.isAccepted());
                if (!me.isAccepted())
                {
                    nativeCommand(&me, m_hoveredFrameSection); //Native move and other commands processing.
                    event->accept();
                }
                result = true;
            }
        }
        break;
        case QEvent::NonClientAreaMouseButtonDblClick:
        case QEvent::MouseButtonDblClick:
        {
            QMouseEvent* origevent = static_cast<QMouseEvent*>(event);
            MouseEventQt me(*origevent, tranlateToNonClient(origevent->pos()));
            if (underMouse())
            {
                mouseDoubleClickNonClientEvent(&me);
                event->setAccepted(me.isAccepted());
                if (!me.isAccepted())
                    nativeCommand(&me, m_hoveredFrameSection); //Native dbl-click processing.
                result = true;
            }
        }
        break;
        case QEvent::NonClientAreaMouseButtonRelease:
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent* origevent = static_cast<QMouseEvent*>(event);
            MouseEventQt me(*origevent, tranlateToNonClient(origevent->pos()));
            if (underMouse())
            {
                mouseReleaseNonClientEvent(&me);
                event->setAccepted(me.isAccepted());
                result = true;
            }
        }
        break;
        default:
            break;
        }
    }
    return result;
}

void WindowTitleBarPrivate::updateMaterialWidgets()
{
    if (blurBehindWindowEnabled() && !qtn_blurBehindSupported())
    {
        QList<MaterialWidget *> list = window()->findChildren<MaterialWidget *>();
        for (QList<MaterialWidget *>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
        {
            MaterialWidget* w = *it;
            if (w->blendType() == MaterialWidget::BackgroundBlend)
            {
                w->repaint();
                //w->update();
            }
        }
    }
}

bool WindowTitleBarPrivate::eventFilter(QObject* object, QEvent* event)
{
    if (window() == Q_NULL)
    {
        Q_ASSERT(false);
        return QObject::eventFilter(object, event);
    }

    if (object == window())
    {
        switch (event->type())
        {
#if 1
        case QEvent::UpdateRequest:
        {
            if (extendViewIntoTitleBar() && m_extendedView != Q_NULL)
                m_extendedView->raise();
        }
        break;
#endif
        case QEvent::WinIdChange:
        case QEvent::ShowToParent:
            if (event->type() == QEvent::WinIdChange)
            {
                installWindow();
#ifdef Q_OS_MAC
                qtn_unset_window_hook(window(), false);
#endif
            }
#if 1
            if (event->type() == QEvent::ShowToParent)
            {
                if (blurBehindWindowEnabled())
                    qtn_setBlurBehindWindow(window(), true);
            }
#endif
#ifdef Q_OS_MAC
            if (isVisible())
            {
                qtn_updateNSView(window());
            }
#endif
            break;
        case QEvent::Show:
            styleChanged();
            break;
        case QEvent::PaletteChange:
            styleChanged();
            break;
        case QEvent::StyleChange:
            styleChanged();
            break;
        case QEvent::ShortcutOverride:
        {
            if (m_titleBarWidget != Q_NULL)
            {
                m_titleBarWidget->send_shortcutOverrideEvent(static_cast<QKeyEvent *>(event));
                if (event->isAccepted())
                    return true;
            }
        }
        break;
        case QEvent::InputMethod:
             inputMethodEvent(static_cast<QInputMethodEvent *>(event));
            return true;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        case QEvent::InputMethodQuery:
            if (window()->testAttribute(Qt::WA_InputMethodEnabled))
            {
                QInputMethodQueryEvent *query = static_cast<QInputMethodQueryEvent *>(event);
                Qt::InputMethodQueries queries = query->queries();
                for (uint i = 0; i < 32; ++i) {
                    Qt::InputMethodQuery q = (Qt::InputMethodQuery)(int)(queries & (1 << i));
                    if (q) {
                        QVariant v = inputMethodQuery(q);
                        if (q == Qt::ImEnabled && !v.isValid() && window()->isEnabled())
                            v = QVariant(true); // special case for Qt4 compatibility
                        query->setValue(q, v);
                    }
                }
                query->accept();
            }
            return true;
#endif
        case QEvent::KeyPress:
        {
            if (m_titleBarWidget != Q_NULL)
            {
                QPoint globalPos = mapToGlobal(QPoint(0, 0) + m_titleBarWidget->geometry().topLeft());
                m_titleBarWidget->send_keyPressEvent(static_cast<QKeyEvent *>(event), globalPos);
                if (event->isAccepted())
                    return true;
           }
        }
        break;
        case QEvent::KeyRelease:
        {
            if (m_titleBarWidget != Q_NULL)
            {
                m_titleBarWidget->send_keyReleaseEvent(static_cast<QKeyEvent *>(event));
                if (event->isAccepted())
                    return true;
            }
        }
        break;
        case QEvent::FocusIn:
        {
            if (m_titleBarWidget != Q_NULL)
                m_titleBarWidget->send_focusInEvent(static_cast<QFocusEvent *>(event)->reason());
            updateInputMethod();
        }
        break;
        case QEvent::FocusOut:
        {
            if (m_titleBarWidget != Q_NULL)
                m_titleBarWidget->send_focusOutEvent(static_cast<QFocusEvent *>(event)->reason());
        }
        break;
        default:
        break;
        }
    }

    return QObject::eventFilter(object, event);
}

void WindowTitleBarPrivate::setTitleBarWidget(QWidget* widget)
{
    if (widget == Q_NULL)
    {
        if (m_titleBarWidget != Q_NULL)
            m_titleBarWidget->setWidget(Q_NULL, QRect(1000, 0, 100, 25));
        Q_DELETE_AND_NULL(m_titleBarWidget);
        return;
    }

    if (m_titleBarWidget == Q_NULL)
    {
        m_titleBarWidget = new BackstageWidget(window());
        QObject::connect(m_titleBarWidget, SIGNAL(changed(const QList<QRectF> &)), 
            this, SLOT(backstageWidgetUpdate(const QList<QRectF> &)));
    }
    
    m_titleBarWidget->setWidget(widget, QRect());
    if (isActive())
    {
        updateTitleBarWidgetGeometry();
        m_titleBarWidget->activate();
    }
}

QWidget* WindowTitleBarPrivate::titleBarWidget() const
{
    return m_titleBarWidget != Q_NULL ? m_titleBarWidget->widget() : Q_NULL;
}

QString WindowTitleBarPrivate::titleBarText() const
{
    if (window() == Q_NULL)
        return QString();
    QString ret = window()->windowTitle();
    if (ret.isEmpty())
        ret = qApp->applicationName();
    return ret;
}

WindowTitleBar::WidgetAlign WindowTitleBarPrivate::titleBarWidgetAlign() const
{
    return m_titleBarWidgetAlign;
}

void WindowTitleBarPrivate::setTitleBarWidgetAlign(WindowTitleBar::WidgetAlign align)
{
    if (m_titleBarWidgetAlign == align)
        return;
    m_titleBarWidgetAlign = align;
    updateTitleBarWidgetGeometry();
}

#if 0
void WindowTitleBarPrivate::setTitleBarMargins(const QMargins& margins)
{
    if (m_titleBarMargins == margins)
        return;
    m_titleBarMargins = margins;
    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->setMargins(margins);
}
#endif

bool WindowTitleBarPrivate::hasFocus() const
{
    return window() != Q_NULL && window()->hasFocus();
}

QRect WindowTitleBarPrivate::getClientRect() const
{
    QRect rect(0, 0, windowSize().width(), windowSize().height());
    rect = rect.adjusted(borderThickness(), titleHeight(), -borderThickness(), -borderThickness());
    return rect;
}

QRegion WindowTitleBarPrivate::getNonClientRegion() const
{
    QRect rect = QRect(QPoint(0, 0), windowSize());
    QRegion region(rect);
    region -= rect.adjusted(borderThickness(), titleHeight(), -borderThickness(), -borderThickness());
    return region;
}

QRect WindowTitleBarPrivate::getTitleBarRect()
{
    return QRect(0, 0, windowSize().width(), titleHeight());
}

QStyle::SubControl WindowTitleBarPrivate::hitTest(const QPoint& pos, Qt::WindowFrameSection& frameSection)
{
    QStyle::SubControl ret = QStyle::SC_None;
    frameSection = checkFrameSection(pos);
    if (frameSection != Qt::NoSection)
    {
        ret = getHoverSubControl(pos);
        if (ret == QStyle::SC_None)
            ret = (QStyle::SubControl)CommonStyle::SC_TitleBarFrame;
    }
    return ret;
}

bool WindowTitleBarPrivate::styledFrame() const
{
#ifdef Q_OS_WIN
    return m_styledFrame || extendViewIntoTitleBar() || !qtn_isCompositionAvailable();
#else
    return true; //Native title bar rendering on MacOSX and Linux is not implemented yet.
#endif
}

void WindowTitleBarPrivate::setStyledFrame(bool styled)
{
    if (m_styledFrame == styled)
        return;
    m_styledFrame = styled;
    styleChanged();
}

bool WindowTitleBarPrivate::blurBehindWindowEnabled() const
{
    return m_blurBehindWindowEnabled;
}

void WindowTitleBarPrivate::setBlurBehindWindowEnabled(bool enabled)
{
    if (m_blurBehindWindowEnabled == enabled)
        return;
    m_blurBehindWindowEnabled = enabled;
}

bool WindowTitleBarPrivate::extendViewIntoTitleBar() const
{
    return m_extendViewIntoTitleBar;
}

void WindowTitleBarPrivate::setExtendViewIntoTitleBar(bool extend)
{
    if (m_extendViewIntoTitleBar == extend)
        return;
    m_extendViewIntoTitleBar = extend;
    styleChanged();
}

WindowTitleBar::SysButtonKind WindowTitleBarPrivate::sysButtonKind() const
{
    return m_sysButtonKind;
}

void WindowTitleBarPrivate::setSysButtonKind(WindowTitleBar::SysButtonKind kind)
{
    if (m_sysButtonKind == kind)
        return;
    m_sysButtonKind = kind;
    styleChanged();
}

const QColor& WindowTitleBarPrivate::backgroundColor() const
{
    return m_backgroundColor;
}

void WindowTitleBarPrivate::setBackgroundColor(const QColor& color)
{
    if (m_backgroundColor == color)
        return;
    m_backgroundColor = color;
    styleChanged();
}

const QColor& WindowTitleBarPrivate::textColor() const
{
    return m_textColor;
}

void WindowTitleBarPrivate::setTextColor(const QColor& color)
{
    if (m_textColor == color)
        return;
    m_textColor = color;
    styleChanged();
}

QColor WindowTitleBarPrivate::frameColor() const
{
    if (window() == Q_NULL)
        return QColor();
    return m_titleBarPalette.color(isActive() ? QPalette::Active : QPalette::Inactive, QPalette::Shadow);
}

const QMargins& WindowTitleBarPrivate::systemFrameMargins() const
{
    return m_systemFrameMargins;
}

int WindowTitleBarPrivate::borderThickness() const
{
    if (isFullScreen())
        return 0;
    return m_borderThickness;
}

int WindowTitleBarPrivate::titleHeight() const
{
    if (isFullScreen())
        return 0;
    return m_titleHeight;
}

bool WindowTitleBarPrivate::useStyleToPaintIconAndText() const
{
    return false;/* styledFrame() &&
        sysButtonKind() != TitleBarBackButton &&
        m_titleBarWidget == Q_NULL && 
        !extendViewIntoTitleBar();*/
}

bool WindowTitleBarPrivate::isMaximized() const
{
    return m_maximized;
}

bool WindowTitleBarPrivate::isFullScreen() const
{
    return m_fullScreen;
}

#if 0
static QRegion qtn_createRoundedRegion(const QSize& size, int radius)
{
#if 1
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, size.width(), size.height()), radius, radius);
    QRegion region(path.toFillPolygon().toPolygon());
#else
    QRegion verticalRegion(0, radius, size.width(), size.height() - 2 * radius);
    QRegion horizontalRegion(radius, 0, size.width() - 2 * radius, size.height());
    QRegion circle(0, 0, 2 * radius, 2 * radius, QRegion::Ellipse);

    QRegion region = verticalRegion.united(horizontalRegion);
    region = region.united(circle);
    region = region.united(circle.translated(size.width() - 2 * radius, 0));
    region = region.united(circle.translated(size.width() - 2 * radius, size.height() - 2 * radius));
    region = region.united(circle.translated(0, size.height() - 2 * radius));
#endif
    return region;
}
#endif

void WindowTitleBarPrivate::updateLayout()
{
    if (isDestroyed())
        return;
    updateSysButtonsRect();
    if (extendViewIntoTitleBar())
        updateExtendedViewGeometry();
    updateTitleBarWidgetGeometry();
}

void WindowTitleBarPrivate::updateFrame(bool forceLayoutUpdate)
{
    Q_UNUSED(forceLayoutUpdate);
    updateLayout();
}

void WindowTitleBarPrivate::update(const QRect& rect)
{
    QRect r = rect.adjusted(-1, -1, 1, 1);
    if (extendViewIntoTitleBar())
        m_extendedView->update(QRect(m_extendedView->mapFrom(window(), r.topLeft()), r.size()));
    else if (window() != Q_NULL)
        window()->update(r);
}

void WindowTitleBarPrivate::update(const QRegion& region)
{
    if(window() != Q_NULL)
        window()->update(region);
}

void WindowTitleBarPrivate::setCursor(const QCursor& cursor)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (window() == Q_NULL || window()->windowHandle() == Q_NULL)
        return;
    window()->windowHandle()->setCursor(cursor);
#else
    Q_UNUSED(cursor);
#endif
    m_hasCursor = true;
}

void WindowTitleBarPrivate::unsetCursor()
{
    if (!m_hasCursor)
        return;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (window() == Q_NULL || window()->windowHandle() == Q_NULL)
        return;
    window()->windowHandle()->unsetCursor();
#endif
    m_hasCursor = false;
}

QVariant WindowTitleBarPrivate::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (m_titleBarWidget == Q_NULL)
        return window() != Q_NULL ? window()->inputMethodQuery(query) : QVariant();
    QVariant retval = m_titleBarWidget->inputMethodQuery(query);
    return retval;
}

void WindowTitleBarPrivate::paintMaterialBehindWindow(QPainter* painter, QWidget* w) const
{
    if (!qtn_blurBehindEnabled(w))
        return;

    if (w != Q_NULL)
    {
        if (qtn_blurBehindSupported())
        {
            const QRect rect = QRect(0, 0, w->width(), w->height());
            QPainter::CompositionMode mode = painter->compositionMode();
            painter->setCompositionMode(QPainter::CompositionMode_Clear);
            painter->fillRect(rect, Qt::white);
            painter->setCompositionMode(mode);
        }
        else
        {
            // We emulate blur behind window feature by blurring the desktop wallpaper.
            QPoint p = w->mapToGlobal(QPoint(0, 0));
            painter->drawPixmap(QPoint(0, 0), behindWindowBackground(), QRect(p, w->size()));
        }
    }
}

WindowTitleBarPrivate* WindowTitleBarPrivate::get(WindowTitleBar* titleBar)
{
    return titleBar->m_p;
}

void WindowTitleBarPrivate::updateWidgetPosition()
{
    if (m_titleBarWidget == Q_NULL || 
        m_titleBarWidget->viewWidget() == Q_NULL)
        return;

    QPoint globalPos = m_titleBarWidget->viewWidget()->mapToGlobal(-tranlateToNonClient(QPoint(0, 0)));
    globalPos += m_titleBarWidget->geometry().topLeft();
    m_titleBarWidget->setGlobalPos(globalPos);
}

const QPixmap& WindowTitleBarPrivate::behindWindowBackground() const
{
    if (m_bluredDesktop.isNull())
    {
        WindowTitleBarPrivate* p = const_cast<WindowTitleBarPrivate *>(this);
        QSize size = qtn_availableGeometry().size();

        p->m_bluredDesktop = QPixmap(size);
        p->m_bluredDesktop.fill(Qt::transparent);
        QImage image = qtn_getDesktopImage();
        if (!image.isNull())
        {
            DesktopImageAspectStyle aspectStyle = qtn_getDesktopAspectStyle();
            QImage buffer(size, QImage::Format_ARGB32_Premultiplied);
#ifdef Q_OS_WIN
            if (aspectStyle == AspectStyleCentral)
                buffer.fill(Qt::black);
#endif
            if (aspectStyle == AspectStyleIgnoreRatio ||
                aspectStyle == AspectStyleKeepRatio ||
                aspectStyle == AspectStyleKeepRatioByExpanding)
            {
                Qt::AspectRatioMode mode = Qt::KeepAspectRatioByExpanding;
                if (aspectStyle == AspectStyleIgnoreRatio)
                    mode = Qt::IgnoreAspectRatio;
                else if (aspectStyle == AspectStyleKeepRatio)
                    mode = Qt::KeepAspectRatio;

                QSize newSize = image.size();
                newSize.scale(size, mode);
                image = image.scaled(newSize, Qt::IgnoreAspectRatio, Qt::FastTransformation);
            }

            if (aspectStyle == AspectStyleTiled)
            {
                QPainter painterBuffer(&buffer);
                QBrush brush(image);
                painterBuffer.fillRect(QRect(QPoint(0, 0), size), brush);
            }
            else
            {
                QPainter painterBuffer(&buffer);
                QRect r = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                    image.size(), QRect(QPoint(0, 0), size));
                painterBuffer.drawImage(r.topLeft(), image);
            }

            QPainter painter(&p->m_bluredDesktop);
#if 1
            qt_blurImage(&painter, buffer, 128, false, false);
#else
            painter.drawImage(QPoint(0, 0), buffer);
#endif
        }
    }
    return m_bluredDesktop;
}

void WindowTitleBarPrivate::syncContentsMargins()
{
    if (window() == Q_NULL)
        return;

    const QMargins margins = window()->contentsMargins();

    if (m_QWizardSupports)
    {
        QWizard* wizard = qobject_cast<QWizard *>(window());
        if (wizard == Q_NULL)
            return;
        QList<QWidget *> list = wizard->findChildren<QWidget *>();
        if (list.size() > 0)
        {
            QWidget* w = list[0];
            if (w->contentsMargins() != margins)
                w->setContentsMargins(margins);
        }
    }
}

void WindowTitleBarPrivate::inputMethodEvent(QInputMethodEvent* event)
{
    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->send_inputMethodEvent(event);
}

void WindowTitleBarPrivate::updateInputMethod()
{
    if (window() == Q_NULL)
        return;
    if (window()->testAttribute(Qt::WA_InputMethodEnabled))
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QTransform t;
        QPoint p = window()->mapTo(window()->topLevelWidget(), QPoint(0, 0));
        t.translate(p.x(), p.y());
        QGuiApplication::inputMethod()->setInputItemTransform(t);
#endif
    }
}

void WindowTitleBarPrivate::processExtendViewEvent(QEvent* event)
{
    if (!extendViewIntoTitleBar())
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (window() != Q_NULL && !event->isAccepted())
        static_cast<QWindowEx *>(window()->windowHandle())->event(event);
#else
    Q_UNUSED(event);
#endif
}

#ifdef Q_OS_MAC
#define QTN_IGNORE_NATIVE_RESIZE_BORDER
#endif
Qt::WindowFrameSection WindowTitleBarPrivate::checkFrameSection(const QPoint& pos)
{
    int border = 0;
#ifndef QTN_IGNORE_NATIVE_RESIZE_BORDER
    //For MacOSX and Linux use native resize border.
    const int sysBorder = qMax(systemFrameMargins().left(), qtn_DPIScaled(5, window()));
    if (!isMaximized() && !isFullScreen())
    {
        border = borderThickness();
        if (!isGlowEnabled())
            border = qMax(border, sysBorder);
        border = qMin(border, sysBorder);
        if (!isResizible())
            border = 0;
    }
#endif 
    QRect windowRect(0, 0, windowSize().width(), windowSize().height());

    if (windowRect.contains(pos))
    {
        QPoint mappedPos = pos - windowRect.topLeft();
        if (QRect(0, 0, border, border).contains(mappedPos))
            return Qt::TopLeftSection;

        if (QRect(border, 0, windowRect.width() - border * 2, border).contains(mappedPos))
            return Qt::TopSection;

        if (QRect(windowRect.width() - border, 0, border, border).contains(mappedPos))
            return Qt::TopRightSection;

        if (QRect(windowRect.width() - border, border, border, windowRect.height() - border * 2).contains(mappedPos))
            return Qt::RightSection;

        if (QRect(windowRect.width() - border, windowRect.height() - border, border, border).contains(mappedPos))
            return Qt::BottomRightSection;

        if (QRect(border, windowRect.height() - border, windowRect.width() - border * 2, border).contains(mappedPos))
            return Qt::BottomSection;

        if (QRect(0, windowRect.height() - border, border, border).contains(mappedPos))
            return Qt::BottomLeftSection;

        if (QRect(0, border, border, windowRect.height() - border * 2).contains(mappedPos))
            return Qt::LeftSection;

        if (containsPoint(pos))
            return Qt::TitleBarArea;
    }
    return Qt::NoSection;
}

void WindowTitleBarPrivate::initTitleBarOption(QStyleOptionTitleBar& option)
{
    option.initFrom(window());

    option.subControls = QStyle::SC_All;
    option.activeSubControls = QStyle::SC_None;

    option.titleBarFlags = m_oldFlags;
    option.titleBarState = window()->windowState();

    option.palette = m_titleBarPalette;
    if (extendViewIntoTitleBar())
        QPalette_setColor(option.palette, QPalette::Window, Qt::transparent);

    option.palette.setCurrentColorGroup(isActive() ? QPalette::Active : QPalette::Inactive);

    if (hasCustomBackground())
    {
        QColor bgcolor = isActive() ? backgroundColor() : window()->palette().color(QPalette::Inactive, QPalette::Window);
        if (!extendViewIntoTitleBar())
            QPalette_setColor(option.palette, QPalette::Window, bgcolor);
        QPalette_setColor(option.palette, QPalette::Shadow, bgcolor);
    }

    if (hasCustomForeground())
    {
        QColor _textColor = isActive() ? textColor() : window()->palette().color(QPalette::Inactive, QPalette::WindowText);
        QPalette_setColor(option.palette, QPalette::WindowText, _textColor);
    }

    if (isActive())
    {
        option.state |= QStyle::State_Active;
        option.titleBarState |= QStyle::State_Active;
    }
    else
    {
        option.state &= ~QStyle::State_Active;
    }
    //For back button painting we need intialize QPalette::ButtonText
    //QPalette_setColor(option.palette, QPalette::ButtonText, option.palette.color(QPalette::WindowText));

    if (window()->isMinimized())
        option.titleBarState |= Qt::WindowMinimized;
    else if (window()->isMaximized())
        option.titleBarState |= Qt::WindowMaximized;

    option.text = titleBarText();
    option.icon = m_icon;

    if (m_hovered != QStyle::SC_None && m_hovered != QStyle::SC_TitleBarLabel)
        option.state |= QStyle::State_MouseOver;
    else
        option.state &= ~QStyle::State_MouseOver;

    if (m_pressed != QStyle::SC_None)
    {
        option.state |= QStyle::State_Sunken;
        if (m_pressed == m_hovered)
            option.activeSubControls = m_pressed;
    }
    else
    {
        option.state &= ~QStyle::State_Sunken;
        option.activeSubControls = m_hovered;
    }

    option.rect = QRect(0, 0, windowSize().width(), titleHeight());
}

void WindowTitleBarPrivate::styleChanged()
{
    if (!isVisible())
        return;

    m_borderThickness = m_borderThicknessRequested;
    m_titleHeight = m_titleHeightRequested;

    m_titleBarPalette = window()->palette();
    qtn_initTitleBarPalette(m_titleBarPalette);

    if (styledFrame())
    {
        QStyleOptionTitleBar option;
        initTitleBarOption(option);
        QStyleHintReturnVariant ret;
        if (window()->style()->styleHint((QStyle::StyleHint)CommonStyle::SH_WindowTitleBarPalette, &option, window(), &ret))
            m_titleBarPalette = qvariant_cast<QPalette>(ret.variant);

        if (m_borderThickness == -1)
            m_borderThickness = window()->style()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, &option, window());
        if (m_titleHeight == -1)
            m_titleHeight = window()->style()->pixelMetric(QStyle::PM_TitleBarHeight, &option, window());
    }
    else
    {
        QMargins systemMargins = systemFrameMargins() - m_frameWindows10;
        if (m_borderThickness == -1)
            m_borderThickness = systemMargins.left();
        if (m_titleHeight == -1)
            m_titleHeight = systemMargins.top();
    }
#if 0
    m_icon = qtn_getTitleBarIcon(window());
#else
    m_icon = window()->windowIcon();
#endif
    if (m_icon.isNull())
        m_icon =  QPixmap(QStringLiteral(":/res/qtitanlogo32x32.png"));

    QMargins margins = QMargins(m_borderThickness, m_titleHeight, m_borderThickness, m_borderThickness);

    if (extendViewIntoTitleBar())
    {
        margins = QMargins(m_borderThickness, 0, m_borderThickness, m_borderThickness);
        if (m_extendedView == Q_NULL)
            m_extendedView = new ExendedView(this);
        m_extendedView->show();
    }
    else
    {
        if (m_extendedView != Q_NULL)
            m_extendedView->hide();
    }

    setFrameMargins(margins);
    updateFrame(true);
}

static bool isButtonVisible(const QStyle::SubControl sc, const QStyleOptionTitleBar *titleBarOption)
{
    bool isMinimized = titleBarOption->titleBarState & Qt::WindowMinimized;
    bool isMaximized = titleBarOption->titleBarState & Qt::WindowMaximized;
    const uint flags = titleBarOption->titleBarFlags;
    bool retVal = false;
    switch (sc) {
    case QStyle::SC_TitleBarContextHelpButton:
        if (flags & Qt::WindowContextHelpButtonHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarMinButton:
        if (!isMinimized && (flags & Qt::WindowMinimizeButtonHint))
            retVal = true;
        break;
    case QStyle::SC_TitleBarNormalButton:
        if (isMinimized && (flags & Qt::WindowMinimizeButtonHint))
            retVal = true;
        else if (isMaximized && (flags & Qt::WindowMaximizeButtonHint))
            retVal = true;
        break;
    case QStyle::SC_TitleBarMaxButton:
        if (!isMaximized && (flags & Qt::WindowMaximizeButtonHint))
            retVal = true;
        break;
    case QStyle::SC_TitleBarShadeButton:
        if (!isMinimized &&  flags & Qt::WindowShadeButtonHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarUnshadeButton:
        if (isMinimized && flags & Qt::WindowShadeButtonHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarCloseButton:
        if (flags & Qt::WindowSystemMenuHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarSysMenu:
        if (flags & Qt::WindowSystemMenuHint)
            retVal = true;
        break;
    default:
        retVal = true;
    }
    return retVal;
}

void WindowTitleBarPrivate::updateSysButtonsRect()
{
    m_systemButtonsRect = QRect();
    if (window() == Q_NULL)
        return;

    Q_ASSERT(styledFrame());
    QStyleOptionTitleBar titleBarOption;
    initTitleBarOption(titleBarOption);

    QRegion region;
    if (isButtonVisible(QStyle::SC_TitleBarContextHelpButton, &titleBarOption))
        region += window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, QStyle::SC_TitleBarContextHelpButton, window());
    if (isButtonVisible(QStyle::SC_TitleBarMinButton, &titleBarOption))
        region += window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, QStyle::SC_TitleBarMinButton, window());
    if (isButtonVisible(QStyle::SC_TitleBarNormalButton, &titleBarOption))
        region += window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, QStyle::SC_TitleBarNormalButton, window());
    if (isButtonVisible(QStyle::SC_TitleBarMaxButton, &titleBarOption))
        region += window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, QStyle::SC_TitleBarMaxButton, window());
    if (isButtonVisible(QStyle::SC_TitleBarShadeButton, &titleBarOption))
        region += window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, QStyle::SC_TitleBarShadeButton, window());
    if (isButtonVisible(QStyle::SC_TitleBarUnshadeButton, &titleBarOption))
        region += window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, QStyle::SC_TitleBarUnshadeButton, window());
    if (isButtonVisible(QStyle::SC_TitleBarCloseButton, &titleBarOption))
        region += window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, QStyle::SC_TitleBarCloseButton, window());

    m_systemButtonsRect = region.boundingRect().adjusted(-2, 0, 2, 0);
}

QStyle::SubControl WindowTitleBarPrivate::getHoverNativeSubControl(const QPoint& pos)
{
    Q_UNUSED(pos);
    return QStyle::SC_None;
}

QRect WindowTitleBarPrivate::getSubControlRect(QStyle::SubControl sc)
{
    if (window() == Q_NULL)
        return QRect();
    const bool sysButtons = (sc != QStyle::SC_TitleBarSysMenu && sc != QStyle::SC_TitleBarLabel);
    if ((sysButtons && styledFrame()) || (!sysButtons && useStyleToPaintIconAndText()))
    {
        QStyleOptionTitleBar titleBarOption;
        initTitleBarOption(titleBarOption);
        return window()->style()->subControlRect(QStyle::CC_TitleBar, &titleBarOption, sc, window());
    }

    switch ((int)sc)
    {
    case QStyle::SC_TitleBarSysMenu:
    {
        QRect r = getTitleBarRect();
        r.adjust(borderThickness(), 0, 0, 0);

        if (isMaximized())
            r.adjust(0, borderThickness(), 0, 0);
        else
            r.adjust(0, qMin(borderThickness(), 5), 0, 0);

        const bool alignSysButtonsLeft = m_systemButtonsRect.left() - r.left() < r.right() - m_systemButtonsRect.right();
        if (alignSysButtonsLeft)
            r.setLeft(m_systemButtonsRect.right());
        
        int sysMenuButtonWidth = getSysMenuButtonWidth();
        r.setWidth(sysMenuButtonWidth);
        return r;
    }
    case QStyle::SC_TitleBarLabel:
    {
        QRect r = getTitleBarRect();
        r.adjust(borderThickness(), 0, 0, 0);
        
        if (isMaximized())
            r.adjust(0, borderThickness(), 0, 0);
        else
            r.adjust(0, qMin(borderThickness(), 5), 0, 0);

        const bool alignSysButtonsLeft = m_systemButtonsRect.left() - r.left() < r.right() - m_systemButtonsRect.right();

        int sysMenuButtonWidth = getSysMenuButtonWidth();
        if (alignSysButtonsLeft)
            r.setLeft(m_systemButtonsRect.right() + sysMenuButtonWidth);
        else
        {
            r.setLeft(r.left() + sysMenuButtonWidth);
            r.setRight(m_systemButtonsRect.left());
        }
        return r;
    }
    case QStyle::SC_TitleBarMinButton:
    case QStyle::SC_TitleBarMaxButton:
    case QStyle::SC_TitleBarCloseButton:
    case QStyle::SC_TitleBarContextHelpButton:
    case QStyle::SC_TitleBarNormalButton:
    case QStyle::SC_TitleBarShadeButton:
    case QStyle::SC_TitleBarUnshadeButton:
    case CommonStyle::SC_TitleBarFrame:
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    return QRect();
}

QStyle::SubControl WindowTitleBarPrivate::getHoverSubControl(const QPoint& pos)
{
    if (window() == Q_NULL)
        return QStyle::SC_None;
    QStyle::SubControl result = QStyle::SC_None;
    const bool sysButtons = m_systemButtonsRect.contains(pos);
    if ((sysButtons && styledFrame()) || (!sysButtons && useStyleToPaintIconAndText()))
    {
        QStyleOptionTitleBar titleBarOption;
        initTitleBarOption(titleBarOption);
        result = window()->style()->hitTestComplexControl(QStyle::CC_TitleBar, &titleBarOption, pos, window());
    }
    else
    {
        QRect r = getSubControlRect(QStyle::SC_TitleBarSysMenu);
        if (r.contains(pos))
            result = QStyle::SC_TitleBarSysMenu;

        if (result == QStyle::SC_None)
        {
            r = getSubControlRect(QStyle::SC_TitleBarLabel);
            if (r.contains(pos))
                result = QStyle::SC_TitleBarLabel;
        }

        if (result == QStyle::SC_None && !styledFrame())
            result = getHoverNativeSubControl(pos);
    }

    if (result == QStyle::SC_TitleBarMaxButton && window()->isMaximized())
        result = QStyle::SC_TitleBarNormalButton;
    return result;
}

void WindowTitleBarPrivate::setSubControlPressed(QStyle::SubControl sc)
{
    if (m_pressed != QStyle::SC_None)
    {
        QRect r = getSubControlRect(m_pressed);
        update(r);
    }

    m_pressed = sc;

    if (m_pressed != QStyle::SC_None)
    {
        QRect r = getSubControlRect(m_pressed);
        update(r);
    }
}

void WindowTitleBarPrivate::subControlClicked(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (window() == Q_NULL)
        return;

    switch (m_pressed)
    {
    case QStyle::SC_TitleBarSysMenu:
        {
            QPoint p = window()->geometry().topLeft();
            p += QPoint(window()->contentsMargins().left(),
                window()->contentsMargins().top());
            if (sysButtonKind() == WindowTitleBar::SysMenuButton)
                showSystemMenu(p);
            else if (sysButtonKind() == WindowTitleBar::BackButton)
            {
                WindowTitleBar* bar = WindowTitleBar::get(window());
                if (bar != Q_NULL)
                    emit bar->backRequested();
            }
        }
        break;
    case QStyle::SC_TitleBarMinButton:
        window()->showMinimized();
        break;
    case QStyle::SC_TitleBarMaxButton:
        window()->showMaximized();
        break;
    case QStyle::SC_TitleBarCloseButton:
        if (window()->windowFlags() & Qt::WindowCloseButtonHint)
            window()->close();
        break;
    case QStyle::SC_TitleBarNormalButton:
        window()->showNormal();
        break;
    case QStyle::SC_TitleBarShadeButton:
        //window()->showShaded();
        break;
    case QStyle::SC_TitleBarUnshadeButton:
        window()->showNormal();
        break;
    case QStyle::SC_TitleBarContextHelpButton:
        {
            WindowTitleBar* bar = WindowTitleBar::get(window());
            if (bar != Q_NULL)
                emit bar->showHelp();
        }
        break;
    case QStyle::SC_TitleBarLabel:
    default:
        break;
    }
}

void WindowTitleBarPrivate::updateSubControl(const QPoint& pos)
{
    QStyle::SubControl oldhovered = m_hovered;
    m_hovered = hitTest(pos, m_hoveredFrameSection);
    if (m_hovered == oldhovered)
        return;
#ifdef Q_OS_MAC
        update(getTitleBarRect());
        return;
#else
    if (oldhovered != QStyle::SC_None && oldhovered != QStyle::SC_TitleBarLabel)
    {
        QRect r = getSubControlRect(oldhovered);
        update(r);
    }

    if (m_hovered != QStyle::SC_None && m_hovered != QStyle::SC_TitleBarLabel)
    {
        QRect r = getSubControlRect(m_hovered);
        update(r);
    }
#endif
}

void WindowTitleBarPrivate::checkUnderMouse(const QPoint& pos)
{
    updateSubControl(pos);

    if (m_pressed != QStyle::SC_None)
        return;

    bool underMouse = containsPoint(pos);

    if (m_nonClientUnderMouse && !underMouse)
        mouseNonClientOut(pos);
    else if (!m_nonClientUnderMouse && underMouse)
        mouseNonClientOver(pos);

    m_nonClientUnderMouse = underMouse;

    updateCursor();
}

void WindowTitleBarPrivate::setVisible(bool visible)
{
    if (m_visible == visible)
        return;
    m_visible = visible;
    if (visible)
        installWindowInternal();
    else
        resetWindow();
}

bool WindowTitleBarPrivate::isVisible() const
{
    return m_visible && window() != Q_NULL && !window()->isHidden();
}

void WindowTitleBarPrivate::setResizible(bool resizible)
{
    if (m_resizible == resizible)
        return;
    m_resizible = resizible;
    styleChanged();
}

bool WindowTitleBarPrivate::isResizible() const
{
    return m_resizible;
}

bool WindowTitleBarPrivate::installWindow()
{
    QWidget* _window = window();
    if (isInitialized() || _window == Q_NULL
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            || _window->windowHandle() == 0
        #endif
            )
        return false;

    m_initialized = true;
    m_origContentsMargins = _window->contentsMargins();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#if 0
    _window->createWinId(); //We need valid windowHandle here. Don't use winId() to create a handle.
#endif
     Q_ASSERT(_window->windowHandle() != Q_NULL);
    _window->windowHandle()->installEventFilter(this);
    connect(window()->windowHandle(), SIGNAL(screenChanged(QScreen*)), 
        this, SLOT(screenChanged(QScreen*)));
#endif

#if 0
    if (blurBehindWindowEnabled()/* && !qtn_blurBehindEnabled(window())*/)
        qtn_setBlurBehindWindow(window(), true);
#endif
    return true;
}

void WindowTitleBarPrivate::resetWindow()
{
    QWidget* _window = window();
    if (_window != Q_NULL)
    {
        _window->removeEventFilter(this);
        if (!isDestroyed())
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            if (_window->windowHandle() != Q_NULL)
            {
                _window->windowHandle()->removeEventFilter(this);
                disconnect(_window->windowHandle(), SIGNAL(screenChanged(QScreen*)),
                    this, SLOT(screenChanged(QScreen*)));
            }
#endif
            _window->setContentsMargins(m_origContentsMargins);
            syncContentsMargins();
#ifndef Q_OS_WIN
            if (blurBehindWindowEnabled()/* && !qtn_blurBehindEnabled(window())*/)
                qtn_setBlurBehindWindow(window(), false);
#endif
            updateCursor();
        }
    }
    m_initialized = false;
    m_origContentsMargins = QMargins();
    m_nonClientUnderMouse = false;
    setFrameMargins(QMargins());
    m_icon = QPixmap();
    m_titleHeight = -1;
    m_borderThickness = -1;
}

void WindowTitleBarPrivate::setWindow(QWidget* _window)
{
    if (_window == window())
        return;
    m_visible = false;
    Q_DELETE_AND_NULL(m_extendedView);
#if 0
    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->setViewWidget(_window);
#endif
    resetWindow();
    WindowStyleHelperAbstract::setWindow(_window);
}

bool WindowTitleBarPrivate::installWindowInternal()
{
    QWidget* _window = window();
    if (_window == Q_NULL)
    {
        Q_ASSERT(false);
        return false;
    }

    _window->installEventFilter(this);

    if (!installWindow())
        return false;

    styleChanged();
    return true;
}

void WindowTitleBarPrivate::nonClientActivateEvent(bool active)
{
    if (window() == Q_NULL)
        return;

    if (active)
    {
#ifdef Q_OS_WIN
        if (!styledFrame() && hasCustomBackground())
        {
            qtn_setWindowColorization(window(), backgroundColor());
        }
#endif

        if (m_titleBarWidget != Q_NULL)
            m_titleBarWidget->activate();
    }
    else
    {
#ifdef Q_OS_WIN
        if (!styledFrame() && hasCustomBackground())
        {
            qtn_setWindowColorization(window(), window()->palette().color(QPalette::Inactive, QPalette::Window));
        }
#endif

        if (m_titleBarWidget != Q_NULL)
            m_titleBarWidget->deactivate();

        setSubControlPressed(QStyle::SC_None);
    }
    window()->update(getNonClientRegion());
}

void WindowTitleBarPrivate::processContextMenuEvent(QMouseEvent* event)
{
#ifdef QTN_PRIVATE_HEADERS
    static const QEvent::Type contextMenuTrigger =
        QGuiApplicationPrivate::platformTheme()->themeHint(QPlatformTheme::ContextMenuOnMouseRelease).toBool() ?
        QEvent::MouseButtonRelease : QEvent::MouseButtonPress;
#else
    static const QEvent::Type contextMenuTrigger = QEvent::MouseButtonRelease;
#endif

    if (event->type() != contextMenuTrigger)
    {
        return;
    }

    QRegion region = getNonClientRegion();
    if (event->type() == contextMenuTrigger && event->button() == Qt::RightButton
        && region.contains(event->pos()))
    {
        QContextMenuEvent e(QContextMenuEvent::Mouse, event->pos(), event->globalPos(), event->modifiers());
        mouseContextNonClientEvent(&e);
    }
}

void WindowTitleBarPrivate::paintTitleBarSysButton(QPainter* painter, const QStyleOptionTitleBar& option)
{
    if (sysButtonKind() == WindowTitleBar::SysMenuButton)
    {
        if (!styledFrame())
        {
            qtn_paintTitleBarIcon(painter, option.icon, option.rect);
        }
        else
        {
            QPixmap pixmap = option.icon.pixmap(window()->style()->pixelMetric(QStyle::PM_SmallIconSize, &option, window())/*option.rect.size()*/);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            const int pixw = pixmap.width() / pixmap.devicePixelRatio();
            const int pixh = pixmap.height() / pixmap.devicePixelRatio();
#else
            const int pixw = pixmap.width();
            const int pixh = pixmap.height();
#endif
            QRect r = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(pixw, pixh)/*pixmap.size()*/, option.rect);
            painter->drawPixmap(r, pixmap);
        }
    }
    else if (sysButtonKind() == WindowTitleBar::BackButton)
    {
        window()->style()->drawControl((QStyle::ControlElement)CommonStyle::CE_NavigationBackButton, &option, painter, window());
    }
}

void WindowTitleBarPrivate::paintTitleBarText(QPainter* painter, const QStyleOptionTitleBar& option)
{
    if (option.rect.width() == 0)
        return;

    QColor _textColor;
    if (hasCustomForeground())
        _textColor = isActive() ? textColor() : window()->palette().color(QPalette::Inactive, QPalette::WindowText);
    else
        _textColor = option.palette.color(isActive() ? QPalette::Active : QPalette::Inactive, QPalette::WindowText);

    if (!styledFrame())
    {
        qtn_paintTitleBarText(painter, option.text, option.rect, isActive(), _textColor);
    }
    else
    {
        QString text = option.text;
#ifdef Q_OS_WIN
        painter->setFont(qtn_getWindowCaptionFont(window()));
#endif
        QRect textBoundingRect = painter->fontMetrics().boundingRect(text);
        QTextOption textOption(Qt::AlignCenter);
        if (textBoundingRect.width() > option.rect.width())
        {
            textOption.setAlignment(Qt::AlignVCenter);
            text = painter->fontMetrics().elidedText(text, Qt::ElideRight, option.rect.width());
        }
        textOption.setWrapMode(QTextOption::NoWrap);
        painter->setPen(_textColor);
        painter->drawText(option.rect, text, textOption);
#if 0
        painter->drawRect(option.rect.adjusted(0, 0, -1, -1));
#endif
    }
}

void WindowTitleBarPrivate::paintNonClientEvent(QPainter* painter, Qt::WindowFrameSection section)
{
    QStyleOptionTitleBar titleBarOption;
    initTitleBarOption(titleBarOption);
    if (styledFrame())
    {
        if (section == Qt::NoSection || section == Qt::TitleBarArea)
        {
            if (/*extendViewIntoTitleBar() ||*/ hasCustomBackground())
                titleBarOption.subControls &= ~QStyle::SC_TitleBarLabel; //Remove title bar background.

            if (hasCustomBackground())
            {
                QColor bgcolor = isActive() ? backgroundColor() : window()->palette().color(QPalette::Inactive, QPalette::Window);
                QRegion clip = getNonClientRegion();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
                for (QRegion::const_iterator it = clip.begin(); it != clip.end(); ++it)
#else
                QVector<QRect> rcs = clip.rects();
                for (QVector<QRect>::const_iterator it = rcs.constBegin(); it != rcs.constEnd(); ++it)
#endif
                    painter->fillRect(*it, bgcolor);
            }

            if (!useStyleToPaintIconAndText())
            {
                titleBarOption.subControls &= ~QStyle::SC_TitleBarSysMenu; //Remove title bar icon.
                titleBarOption.icon = QIcon();
                titleBarOption.text = QString();
            }

            painter->save();
#ifdef Q_OS_MAC
            if (QLatin1String(window()->style()->metaObject()->className()) == QStringLiteral("QMacStyle"))
                qtn_paint_title_bar_mac(&titleBarOption, painter, window());
            else
#endif
            window()->style()->drawComplexControl(QStyle::CC_TitleBar, &titleBarOption, painter, window());
            painter->restore();
#if 0
            QRect temp = getSubControlRect(QStyle::SC_TitleBarCloseButton);
            painter->drawRect(temp);
            temp = getSubControlRect(QStyle::SC_TitleBarMinButton);
            painter->drawRect(temp);
            temp = getSubControlRect(QStyle::SC_TitleBarMaxButton);
            painter->drawRect(temp);
#endif
        }
        if (!hasCustomBackground() && (section != Qt::TitleBarArea || section == Qt::NoSection))
        {
            FrameStyleOption frameOption;
            initFrameOption(frameOption, section);
            frameOption.palette = titleBarOption.palette;
            painter->save();
            bool hasBorder = false;
            if (!hasBorder && section == Qt::NoSection)
                painter->setClipRect(frameOption.rect.adjusted(0, titleBarOption.rect.height(), 0, 0), Qt::IntersectClip);
            window()->style()->drawPrimitive(QStyle::PE_FrameWindow, &frameOption, painter, window());
            painter->restore();
        }
    }
    else if (extendViewIntoTitleBar())
        qtn_paintAirEffect(painter, m_systemButtonsRect, m_systemButtonsRect);

    if (section == Qt::NoSection || section == Qt::TitleBarArea)
    {
        if (m_titleBarWidget != Q_NULL)
            m_titleBarWidget->paintContent(*painter);

        if (!useStyleToPaintIconAndText() && sysButtonKind() != WindowTitleBar::SysButtonHidden)
        {
            //Draw icon.
            titleBarOption.rect = getSubControlRect(QStyle::SC_TitleBarSysMenu);
            titleBarOption.icon = m_icon;
            if (titleBarOption.activeSubControls != QStyle::SC_TitleBarSysMenu)
                titleBarOption.state = QStyle::State_None;
            //if (sysButtonKind() == TitleBarBackButton)
            //    titleBarOption.state |= QStyle::State_MouseOver;
            paintTitleBarSysButton(painter, titleBarOption);

            //Draw title.
            titleBarOption.rect = m_textRect;
            titleBarOption.text = titleBarText();
            titleBarOption.titleBarFlags = Qt::WindowFlags();
            titleBarOption.titleBarState = 0;
            paintTitleBarText(painter, titleBarOption);
        }
    }
}

void WindowTitleBarPrivate::initFrameOption(FrameStyleOption& frameOption, Qt::WindowFrameSection section)
{
    frameOption.initFrom(window());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    frameOption.features = QStyleOptionFrame::Flat;
    frameOption.frameShape = QFrame::StyledPanel;
#endif
    frameOption.lineWidth = window()->style()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, 0, window());
    frameOption.midLineWidth = borderThickness() - frameOption.lineWidth;
    frameOption.rect = QRect(QPoint(0, 0), windowSize());
    if (isActive())
        frameOption.state |= QStyle::State_Active;
    else
        frameOption.state &= ~QStyle::State_Active;

    const int fwidth = frameOption.lineWidth + frameOption.midLineWidth;
    frameOption.section = section;
    switch (section)
    {
        case Qt::LeftSection:
            frameOption.rect.setWidth(fwidth);
            break;
        case Qt::RightSection:
            frameOption.rect.setWidth(fwidth);
            break;
        case Qt::BottomSection:
            frameOption.rect.setHeight(fwidth);
            break;
        default: break;
    }
}

void WindowTitleBarPrivate::mousePressNonClientEvent(QMouseEvent* event)
{
    event->ignore();

    if (!defaultResizeHandler() && m_hovered != QStyle::SC_None)
    {
        if (m_hovered == QStyle::SC_TitleBarLabel)
        {
            if (m_titleBarWidget != Q_NULL)
            {
                if (m_titleBarWidget->hasMouseGrabber())
                {
                    QMouseEvent moveEvent(QEvent::MouseMove, event->pos(), 
                        Qt::MouseButton(), Qt::MouseButtons(), Qt::KeyboardModifier());
                    m_titleBarWidget->send_mouseMoveEvent(&moveEvent);
                }

                m_titleBarWidget->send_mousePressEvent(event);

                if (event->isAccepted())// && m_titleBarWidget->hasFocusItem())
                {
                    if (window() != Q_NULL)
                        window()->setFocus();
                }
            }
        }
        else
        {
            event->accept();
        }
    }

    setSubControlPressed(m_hovered);

    if (event->isAccepted() || event->button() != Qt::LeftButton)
    {
        processContextMenuEvent(event);
        event->accept();
    }

    processExtendViewEvent(event);
}

void WindowTitleBarPrivate::mouseReleaseNonClientEvent(QMouseEvent* event)
{
    event->ignore();

    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->send_mouseReleaseEvent(event);

    if (!event->isAccepted() && event->button() == Qt::LeftButton &&
        m_pressed != QStyle::SC_None && m_pressed == m_hovered)
        subControlClicked(event);

    processContextMenuEvent(event);
    setSubControlPressed(QStyle::SC_None);

    processExtendViewEvent(event);
}

void WindowTitleBarPrivate::mouseDoubleClickNonClientEvent(QMouseEvent* event)
{
    event->ignore();

    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->send_mouseDoubleClickEvent(event);

    processExtendViewEvent(event);

    if (!event->isAccepted())
        event->setAccepted(m_pressed != QStyle::SC_TitleBarLabel);
}

void WindowTitleBarPrivate::mouseMoveNonClientEvent(QMouseEvent* event)
{
    event->ignore();

    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->send_mouseMoveEvent(event);
    processExtendViewEvent(event);
}

void WindowTitleBarPrivate::mouseNonClientOver(const QPoint& pos)
{
    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->send_mouseOverEvent(pos);
}

void WindowTitleBarPrivate::mouseNonClientOut(const QPoint& pos)
{
    setSubControlPressed(QStyle::SC_None);

    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->send_mouseOutEvent(pos);
}

void WindowTitleBarPrivate::mouseContextNonClientEvent(QContextMenuEvent* event)
{
    event->ignore();
    if (m_titleBarWidget != Q_NULL)
        m_titleBarWidget->send_contextMenuEvent(event);

    if (!event->isAccepted())
    {
        if (m_pressed == QStyle::SC_TitleBarSysMenu || m_pressed == QStyle::SC_TitleBarLabel)
        {
            showSystemMenu(event->globalPos());
            event->accept();
        }
    }
}

void WindowTitleBarPrivate::resizeNonClientEvent(const QSize& windowSize)
{
    if (windowSize == this->windowSize())
        return;
    setWindowSize(windowSize);
}

bool WindowTitleBarPrivate::isGlowEnabled() const
{
    return false;
}

bool WindowTitleBarPrivate::defaultResizeHandler()
{
    return m_hoveredFrameSection == Qt::LeftSection ||
        m_hoveredFrameSection == Qt::TopLeftSection ||
        m_hoveredFrameSection == Qt::TopSection ||
        m_hoveredFrameSection == Qt::TopRightSection ||
        m_hoveredFrameSection == Qt::RightSection ||
        m_hoveredFrameSection == Qt::BottomRightSection ||
        m_hoveredFrameSection == Qt::BottomSection ||
        m_hoveredFrameSection == Qt::BottomLeftSection;
}

static inline QMargins qtn_margins_from_rects(const QRect& r1, const QRect& r2)
{
    return QMargins(r2.left() - r1.left(), r2.top() - r1.top(), r1.right() - r2.right(), r1.bottom() - r2.bottom());
}

void WindowTitleBarPrivate::updateTitleBarWidgetGeometry()
{
    m_textRect = QRect();
    if (window() == Q_NULL)
        return;

    m_textRect = getSubControlRect(QStyle::SC_TitleBarLabel);

    if (m_titleBarWidget != Q_NULL)
    {
        WindowTitleBar::WidgetAlign align = titleBarWidgetAlign();
        bool alignLeft = align == WindowTitleBar::AlignLeft;

        const QRect titleAreaRect = m_textRect;

        QString text = titleBarText();
        QSize textSize = QFontMetrics(window()->font()).size(Qt::TextSingleLine, text);

        QRect widgetRect = alignLeft ? titleAreaRect.adjusted(0, 0, -textSize.width(), 0) : titleAreaRect.adjusted(textSize.width(), 0, 0, 0);
        widgetRect.setWidth(qMax(widgetRect.width(), m_titleBarWidget->widget()->minimumWidth()));
        widgetRect.setWidth(qMin(widgetRect.width(), m_titleBarWidget->widget()->maximumWidth()));

        QSize widgetSizeHint = QSize(m_titleBarWidget->widget()->sizeHint().width(), titleAreaRect.height());

        widgetRect = QStyle::alignedRect(Qt::LeftToRight, 
            alignLeft ? Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter) : 
            Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter), widgetSizeHint, widgetRect);

        if (align == WindowTitleBar::AlignClient)
        {
            widgetRect.setLeft(titleAreaRect.left());
            widgetRect.setWidth(m_textRect.width());
        }

        widgetRect = widgetRect.intersected(titleAreaRect);
        
        QRect rect = getTitleBarRect();
        m_titleBarWidget->setMargins(qtn_margins_from_rects(rect, widgetRect));
        m_titleBarWidget->setGeometry(rect);

//        textSize.rwidth() = qMin(textSize.width(), qMax(0, geometry.width() - widgetRect.width()));
        textSize.rwidth() = qMax(0, titleAreaRect.width() - widgetRect.width());

        if (alignLeft)
            m_textRect.setLeft(m_textRect.left() + (m_textRect.width() - textSize.width() - 1));
        else
            m_textRect.setRight(m_textRect.left() + textSize.width() - 1);
    }
}

bool WindowTitleBarPrivate::containsPoint(const QPoint& pos)
{
    QRegion region = getNonClientRegion();
    return region.contains(pos);
}

void WindowTitleBarPrivate::updateExtendedViewGeometry()
{
    if (m_extendedView != Q_NULL)
    {
        QMargins margins = window()->contentsMargins();
        m_extendedView->setGeometry(QRect(QPoint(margins.left(), margins.top()), 
            windowSize() - QSize(margins.left() + margins.right(), margins.top() + margins.bottom())));
    }
}

void WindowTitleBarPrivate::updateCursor()
{
    if (window() == Q_NULL)
    {
        unsetCursor();
        return;
    }

    if (defaultResizeHandler())
    {
        setCursor(getCursorForFrameSection(m_hoveredFrameSection));
    }
    else
    {
        bool hc = underMouse() && m_titleBarWidget != Q_NULL && m_titleBarWidget->hasCursor();
        if (hc)
            setCursor(m_titleBarWidget->cursor());
        else
            unsetCursor();
    }
}

QCursor WindowTitleBarPrivate::getCursorForFrameSection(Qt::WindowFrameSection frameSection)
{
    Qt::CursorShape cursorShape = Qt::ArrowCursor;
    switch (frameSection)
    {
    case Qt::LeftSection:
    case Qt::RightSection:
        cursorShape = Qt::SizeHorCursor;
        break;
    case Qt::BottomSection:
    case Qt::TopSection:
        cursorShape = Qt::SizeVerCursor;
        break;
    case Qt::TopLeftSection:
    case Qt::BottomRightSection:
        cursorShape = Qt::SizeFDiagCursor;
        break;
    case Qt::TopRightSection:
    case Qt::BottomLeftSection:
        cursorShape = Qt::SizeBDiagCursor;
        break;
    case Qt::TitleBarArea:
        cursorShape = Qt::ArrowCursor;
        break;
    default:
        break;
    }
    return QCursor(cursorShape);
}

int WindowTitleBarPrivate::getSysMenuButtonWidth() const
{
    if (window() == Q_NULL || sysButtonKind() == WindowTitleBar::SysButtonHidden)
        return 0;

    int ret = 0;
    if (sysButtonKind() == WindowTitleBar::SysMenuButton)
        ret = window()->style()->pixelMetric(QStyle::PM_SmallIconSize, 0, window()) + 4;
    else
        ret = qtn_DPIScaled(50, window());
#if 0
    ret = qMax(ret, !m_icon.isNull() ? m_icon.availableSizes()[0].width() + 4 : 0);
#endif
    return ret;
}

bool WindowTitleBarPrivate::hasCustomBackground() const
{
    return m_backgroundColor.isValid();
}

bool WindowTitleBarPrivate::hasCustomForeground() const
{
    return m_textColor.isValid();
}

void WindowTitleBarPrivate::backstageWidgetUpdate(const QList<QRectF>& region)
{
    if (m_titleBarWidget == Q_NULL)
        return;

    for (QList<QRectF>::const_iterator it = region.constBegin(); it != region.constEnd(); ++it)
    {
        QRect r = (*it).toRect();
        if (r.width() == 0 || r.height() == 0)
            continue;
        update(r.translated(m_titleBarWidget->geometry().topLeft()));
    }

    //Dirty hack to fix the bug in QGraphicsScene - QWidget::scroll()
    QList<QAbstractScrollArea *> areas = m_titleBarWidget->widget()->findChildren<QAbstractScrollArea *>();
    for (QList<QAbstractScrollArea *>::const_iterator it = areas.constBegin(); it != areas.constEnd(); ++it)
        (*it)->viewport()->update();
}

void WindowTitleBarPrivate::screenChanged(QScreen* screen)
{
    Q_UNUSED(screen);
    styleChanged();
}

static const QPoint invalidDragPosConst = QPoint(-10000, -10000);

/* WindowTitleBarPrivateQt */
WindowTitleBarPrivateQt::WindowTitleBarPrivateQt()
    : WindowTitleBarPrivate(),
      m_frameSection(Qt::NoSection), m_startDragPos(invalidDragPosConst)
{
}

WindowTitleBarPrivateQt::~WindowTitleBarPrivateQt()
{
    setWindow(Q_NULL);
}

bool WindowTitleBarPrivateQt::installWindow()
{
    if (!WindowTitleBarPrivate::installWindow())
        return false;

    QWidget* _window = window();
    Q_ASSERT(_window != Q_NULL);
    if (_window == Q_NULL)
        return false;

    bool visible = isVisible();

    m_oldFlags = _window->windowFlags();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QRect saveRect = _window->windowHandle()->geometry();
#else
    QRect saveRect = _window->geometry();
#endif
#ifdef Q_OS_MAC
    _window->setWindowFlags(Qt::Window);
#else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    _window->setWindowFlags(_window->windowFlags() | Qt::FramelessWindowHint);
#else
    _window->setWindowFlags(Qt::WindowFlags(Qt::Window | Qt::FramelessWindowHint));
#endif
#endif
    _window->setAttribute(Qt::WA_Hover, true);
    _window->setMouseTracking(true);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    _window->windowHandle()->setGeometry(saveRect);
#else
    _window->setGeometry(saveRect);
#endif
    resizeNonClientEvent(saveRect.size());
    _window->setVisible(visible);
#ifdef Q_OS_MAC
    if (visible)
    {
        QApplication::setActiveWindow(Q_NULL);
        QApplication::setActiveWindow(_window);
    }
#endif
    return true;
}

void WindowTitleBarPrivateQt::resetWindow()
{
    WindowTitleBarPrivate::resetWindow();
    if (m_oldFlags == 0)
        return;
    QWidget* _window = window();
    Q_ASSERT(_window != Q_NULL);
    if (_window == Q_NULL)
        return;
#ifdef Q_OS_MAC
    qtn_unset_window_hook(_window, !isDestroyed());
#endif
    resizeNonClientEvent(QSize());
    if (!isDestroyed())
    {
        bool shouldBeVisible = _window->isVisible();
        _window->setWindowFlags(m_oldFlags);
        _window->setAttribute(Qt::WA_Hover, false);
        _window->clearMask();
        if (shouldBeVisible)
            _window->show();
    }
    m_oldFlags = Qt::WindowFlags();
}

bool WindowTitleBarPrivateQt::isActive() const
{
    return window() != Q_NULL ? window()->isActiveWindow() : false;
}

void WindowTitleBarPrivateQt::showSystemMenu(const QPoint& pos)
{
    Q_UNUSED(pos);
}

bool WindowTitleBarPrivateQt::mouseEventFilter(QObject* object, QEvent* event)
{
    if (isDragMode())
    {
        if (event->type() == QEvent::MouseMove)
        {
            dragMove(static_cast<QMouseEvent *>(event)->globalPos());
            return true;
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            stopDrag();
        }
        return false;
    }

    return WindowTitleBarPrivate::mouseEventFilter(object, event);
}

bool WindowTitleBarPrivateQt::eventFilter(QObject* object, QEvent* event)
{
    WindowTitleBarAutoUpdater updater(this);
    bool result = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (object == window()->windowHandle())
#else
    if (object == window())
#endif
    {
        switch (event->type())
        {
        case QEvent::Move:
        {
            updateMaterialWidgets();
        }
        break;
        case QEvent::Resize:
        {
            QResizeEvent* re = static_cast<QResizeEvent *>(event);
            resizeNonClientEvent(re->size());
            window()->update();
        }
        break;
        default:
            break;
        }
    }

    if (object == window())
    {
        switch (event->type())
        {
        case QEvent::WindowStateChange:
            updateFrame(false);
            break;
        case QEvent::WindowActivate:
            nonClientActivateEvent(true);
            break;
        case QEvent::WindowDeactivate:
            nonClientActivateEvent(false);
            break;
        case QEvent::Paint:
        {
            QPaintEvent* pe = static_cast<QPaintEvent *>(event);
            {
                PaintEventQt peq(*pe);
                peq.setRegion(QRegion(getClientRect()).intersected(pe->region()));
                peq.setRect(getClientRect().intersected(pe->rect()));
                static_cast<QWidgetEx *>(window())->paintEvent(&peq);
            }
            result = true;

            if (extendViewIntoTitleBar())
                break;

            QRegion clip = getNonClientRegion().intersected(pe->region());
            if (clip.isEmpty())
                break; //Nothing to paint.

            QPainter painter(window());
            QPainter::CompositionMode mode = painter.compositionMode();
            painter.setCompositionMode(QPainter::CompositionMode_Clear);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
            for (QRegion::const_iterator it = clip.begin(); it != clip.end(); ++it)
#else
            QVector<QRect> rcs = clip.rects();
            for (QVector<QRect>::const_iterator it = rcs.constBegin(); it !=  rcs.constEnd(); ++it)
#endif
            {
                const QRect& r = *it;
                painter.fillRect(r, Qt::white);
            }
            painter.setCompositionMode(mode);
            painter.setClipRegion(clip);
            paintNonClientEvent(&painter);
        }
        break;

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        case QEvent::ContextMenu:
        {
            QContextMenuEvent* ce = static_cast<QContextMenuEvent *>(event);
            QRegion region = getNonClientRegion();
            if (region.contains(ce->pos()))
            {
                mouseContextNonClientEvent(ce);
                result = true;
            }
        }
        break;
#endif
        default:
        break;
        }
    }

    if (result || mouseEventFilter(object, event))
        return true;

    return WindowTitleBarPrivate::eventFilter(object, event);
}

bool WindowTitleBarPrivateQt::isDragMode() const
{
    return m_startDragPos != invalidDragPosConst;
}

void WindowTitleBarPrivateQt::startDrag(const QPoint& pos, Qt::WindowFrameSection frameSection)
{
    m_startDragPos = pos;
    m_frameSection = frameSection;
    m_grabGeometry = window()->geometry();
    window()->grabMouse();
    setCursor(getCursorForFrameSection(m_frameSection));
}

void WindowTitleBarPrivateQt::stopDrag()
{
    m_frameSection = Qt::NoSection;
    m_startDragPos = invalidDragPosConst;
    window()->releaseMouse();
    updateCursor();
}

void WindowTitleBarPrivateQt::dragMove(const QPoint& pos)
{
    QRect geometry = m_grabGeometry;

    QPoint offset = pos - m_startDragPos;

    if (m_frameSection == Qt::TitleBarArea)
    {
        geometry.moveTo(geometry.topLeft() + offset);
    }
    else
    {
        if (m_frameSection == Qt::RightSection || m_frameSection == Qt::TopRightSection || m_frameSection == Qt::BottomRightSection)
        {
            int newWidth = geometry.width() + offset.x();
            newWidth = qMin(qMax(newWidth, window()->minimumWidth()), window()->maximumWidth());
            geometry.adjust(0, 0, newWidth - geometry.width(), 0);
        }

        if (m_frameSection == Qt::LeftSection || m_frameSection == Qt::TopLeftSection || m_frameSection == Qt::BottomLeftSection)
        {
            int newWidth = geometry.width() - offset.x();
            newWidth = qMin(qMax(newWidth, window()->minimumWidth()), window()->maximumWidth());
            geometry.adjust(geometry.width() - newWidth, 0, 0, 0);
        }

        if (m_frameSection == Qt::BottomSection || m_frameSection == Qt::BottomLeftSection || m_frameSection == Qt::BottomRightSection)
        {
            int newHeight = geometry.height() + offset.y();
            newHeight = qMin(qMax(newHeight, window()->minimumHeight()), window()->maximumHeight());
            geometry.adjust(0, 0, 0, newHeight - geometry.height());
        }

        if (m_frameSection == Qt::TopSection || m_frameSection == Qt::TopLeftSection || m_frameSection == Qt::TopRightSection)
        {
            int newHeight = geometry.height() - offset.y();
            newHeight = qMin(qMax(newHeight, window()->minimumHeight()), window()->maximumHeight());
            geometry.adjust(0, geometry.height() - newHeight, 0, 0);
        }
    }
    if (window()->geometry() != geometry)
        window()->setGeometry(geometry);
}

void WindowTitleBarPrivateQt::updateFrame(bool forceLayoutUpdate)
{
    if (window() == Q_NULL)
        return;

    Qt::WindowStates states = window()->windowState();

    bool changed = false;
    bool m = states & Qt::WindowMaximized;
    bool fs = states & Qt::WindowFullScreen;
    if (m_maximized != m)
    {
        m_maximized = m;
        changed = true;
    }
    if (m_fullScreen != fs)
    {
        m_fullScreen = fs;
        changed = true;
    }

    QMargins contentsMargins = m_origContentsMargins;
    
    if (!isFullScreen())
        contentsMargins = frameMargins() + m_origContentsMargins;

    if (contentsMargins != window()->contentsMargins())
    {
        window()->setContentsMargins(contentsMargins);
        syncContentsMargins();
        changed = true;
    }

    if (changed || forceLayoutUpdate)
        updateLayout();
}

void WindowTitleBarPrivateQt::processContextMenuEvent(QMouseEvent* event)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    WindowTitleBarPrivate::processContextMenuEvent(event);
#else
    Q_UNUSED(event);
#endif
}

void WindowTitleBarPrivateQt::resizeNonClientEvent(const QSize& windowSize)
{
#ifdef Q_OS_MAC
    qtn_window_resize_mac(window());
#endif
    WindowTitleBarPrivate::resizeNonClientEvent(windowSize);
    updateFrame(true);
}

void WindowTitleBarPrivateQt::nativeCommand(QMouseEvent* event, Qt::WindowFrameSection frameSection)
{
    if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
    {
        if (isMaximized())
            window()->showNormal();
        else
            window()->showMaximized();
        event->accept();
        return;
    }

#ifdef Q_OS_MAC
    if (qtn_window_start_native_drag_mac(window(), event->globalPos()))
    {
        event->accept();
        return;
    }
#endif
#ifdef Q_OS_LINUX
    if (qtn_window_start_native_drag_linux(window(), event->globalPos(), frameSection))
    {
        event->accept();
        return;
    }
#endif
    startDrag(event->globalPos(), frameSection);
    event->accept();
}

QPoint WindowTitleBarPrivateQt::mapFromGlobal(const QPoint& pos) const
{
    if (window() != Q_NULL)
        return window()->mapFromGlobal(pos);
    else
        return QPoint();
}


QPoint WindowTitleBarPrivateQt::mapToGlobal(const QPoint& pos) const
{
    if (window() != Q_NULL)
        return window()->mapToGlobal(pos);
    else
        return QPoint();
}

QPoint WindowTitleBarPrivateQt::tranlateToNonClient(const QPoint& clientPos)
{
    return clientPos;
}

/* ExendedView */
ExendedView::ExendedView(WindowTitleBarPrivate* bar)
    : QWidget(bar->window()), m_bar(bar)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

ExendedView::~ExendedView()
{
}

void ExendedView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPoint offset = QPoint(m_bar->frameMargins().left(), m_bar->frameMargins().top());
    painter.translate(-offset);
    QRegion clip = m_bar->getNonClientRegion().intersected(event->region().translated(offset));
    painter.setClipRegion(clip);
    if (!clip.isEmpty())
        m_bar->paintNonClientEvent(&painter, Qt::TitleBarArea);
}

bool QTITAN_PREPEND_NAMESPACE(qtn_image_copy(QImage& dest, const QPoint& p, const QImage& src))
{

    if (dest.format() != src.format() ||
        (dest.format() != QImage::Format_ARGB32_Premultiplied && dest.format() != QImage::Format_ARGB32))
    {
        Q_ASSERT(false);
        return false;
    }

    int w = src.width();
    int h = src.height();
    if (w <= 0 || h <= 0)
    {
        Q_ASSERT(false);
        return false;
    }

    int x = p.x();
    int y = p.y();

    int dx = 0;
    int dy = 0;

    if (x < 0 || y < 0 || x + w > dest.width() || y + h > dest.height())
    {
        if (x < 0) {
            dx = -x;
            x = 0;
        }
        if (y < 0) {
            dy = -y;
            y = 0;
        }
    }

    int pixels_to_copy = qMax(w - dx, 0);
    if (x > dest.width())
        pixels_to_copy = 0;
    else if (pixels_to_copy > dest.width() - x)
        pixels_to_copy = dest.width() - x;
    int lines_to_copy = qMax(h - dy, 0);
    if (y > dest.height())
        lines_to_copy = 0;
    else if (lines_to_copy > dest.height() - y)
        lines_to_copy = dest.height() - y;

    bool aligned = true;
    bool changed = false;
    if (aligned)
    {
        uchar *dest_bits = dest.bits() + ((x * dest.depth()) >> 3) + y * dest.bytesPerLine();
        const uchar *src_bits = src.bits() + ((dx * src.depth()) >> 3) + dy * src.bytesPerLine();
        const int bytes_to_copy = (pixels_to_copy * dest.depth()) >> 3;

        for (int i = 0; i < lines_to_copy; ++i)
        {
            changed = changed || memcmp(dest_bits, src_bits, bytes_to_copy) != 0;
            if (changed)
                memcpy(dest_bits, src_bits, bytes_to_copy);
            dest_bits += dest.bytesPerLine();
            src_bits += src.bytesPerLine();
        }
    }
    return changed;
}

bool QTITAN_PREPEND_NAMESPACE(qtn_blurBehindEnabled(QWidget* widget))
{
    QWidget* window = widget->isWindow() ? widget : widget->nativeParentWidget();
    if (window == Q_NULL)
        return false;
    return window->property("qtn_blurBehind").toBool();
}

bool QTITAN_PREPEND_NAMESPACE(qtn_blurBehindSupported())
{
    static const bool supported =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows10 ||
        QOperatingSystemVersion::current() >= QOperatingSystemVersion::OSXYosemite;
#else
#if defined(Q_OS_WIN)
        QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS10;
#elif defined(Q_OS_MAC)
        QSysInfo::macVersion() >= QSysInfo::MV_YOSEMITE;
#else
        true;
#endif
#endif
    return supported;
}
