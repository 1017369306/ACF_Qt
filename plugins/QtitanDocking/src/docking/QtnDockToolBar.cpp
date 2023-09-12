/****************************************************************************
**
** Qtitan Library by Developer Machines (Dock Windows and Bars component for Qt.C++)
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

#include "QtnDockToolBar.h"
#include "QtnDockBarManagerPrivate.h"

#include <QApplication>
#include <qevent.h>
#include <qstylepainter.h>
#include <qtoolbutton.h>
#include <qwidgetaction.h>
#include <qtimer.h>

#include "QtnDockMainWindow.h"
#include "QtnDockToolBarPrivate.h"
#include "QtnDockTitleBarPrivate.h"
#include "QtnDockBarSite.h"
#include "QtnDockMainWindowLayout.h"
#include "QtnDockToolBarLayoutPrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

#define POPUP_TIMER_INTERVAL 500

QTITAN_USE_NAMESPACE


/* DockBarWindow */
DockBarWindow::DockBarWindow(DockBarManager* manager)
    : QWidget(manager->managedWidget()
        /*, Qt::Dialog | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint*/)
    , m_barManager(manager)
    , m_toolBar(Q_NULL)
    , m_titleBar(Q_NULL)
    , m_boxLayout(new QVBoxLayout())
    , m_actCloseButton(Q_NULL)
{
#if 0
    setMouseTracking(true);
#ifdef Q_OS_MAC
    setAttribute(Qt::WA_MacAlwaysShowToolWindow);
    setProperty("_q_platform_MacUseNSWindow", QVariant(true));
#endif /* Q_OS_MAC */
    setAttribute(Qt::WA_X11NetWmWindowTypeToolBar);
#endif

#ifdef Q_OS_WIN
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
#endif
#ifdef Q_OS_LINUX
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
#endif
#ifdef Q_OS_MAC
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
#endif

    m_titleBar = new DockTitleBar(this);
    QObject::connect(m_titleBar, SIGNAL(closeClicked()), this, SLOT(hide()));

    m_actCloseButton = new QAction(QObject::tr("Close"), this);
    m_actCloseButton->setProperty(_qtn_Type_Button, CloseButton);
    m_titleBar->addAction(m_actCloseButton);

    setLayout(m_boxLayout);
    m_boxLayout->setContentsMargins(0, 0, 0, 0);
    m_boxLayout->setSpacing(0);
    m_boxLayout->setSizeConstraint(QLayout::SetFixedSize);
    m_boxLayout->setContentsMargins(0, 0, 0, 0);
    m_boxLayout->addWidget(m_titleBar);

    m_titleBar->setFocusPolicy(Qt::NoFocus);
    m_titleBar->setCloseButtonVisible(true);

    WindowTitleBar* windowTitleBar = WindowTitleBar::get(this);
    windowTitleBar->setResizible(false);
    windowTitleBar->setVisible(true);
}

DockBarWindow::~DockBarWindow()
{
}

DockTitleBar* DockBarWindow::titleBar() const
{ return m_titleBar; }

void DockBarWindow::setDockBar(DockToolBar* dockBar)
{
    if (m_toolBar == dockBar)
        return;

    if (m_toolBar != Q_NULL)
    {
        m_toolBar->setDockBarArea(Qtitan::DockBarNone);
        m_boxLayout->removeWidget(m_toolBar);
        m_toolBar->setParent(Q_NULL);
    }

    m_toolBar = dockBar;

    if (m_toolBar != Q_NULL)
    {
        m_toolBar->setParent(this);
        setFont(m_toolBar->font());
        m_toolBar->setDockBarArea(Qtitan::DockBarFloat);
        QString strTitle = m_toolBar->windowTitle();
        if (strTitle.isEmpty())
            strTitle = m_toolBar->caption();
        titleBar()->setText(strTitle);
        m_boxLayout->addWidget(m_toolBar);
    }
}

DockToolBar* DockBarWindow::toolBar() const
{ return m_toolBar; }

void DockBarWindow::changeEvent(QEvent* event)
{
    switch (event->type())
    {
        case QEvent::StyleChange:
            adjustSize();
            break;
        default:
            break;
    }
    QWidget::changeEvent(event);
}

bool DockBarWindow::event(QEvent* event)
{
    switch (event->type())
    {
        case QEvent::WindowActivate:
        {
            if (m_toolBar != Q_NULL)
            {
                DockToolBarPrivate* p = DockToolBarPrivate::_get(m_toolBar);
                if (p->isDragging())
                    p->activateMainWindow();
            }
        }
        break;
        case QEvent::Show:
            resize(sizeHint());
            break;
        default:
            break;
    }

    return QWidget::event(event);
}

void DockBarWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolBar opt;
    DockToolBarPrivate::_get(m_toolBar)->initStyleOption(&opt);
    opt.rect = rect();
    p.fillRect(opt.rect, palette().window());
    style()->drawControl(QStyle::CE_ToolBar, &opt, &p, this);
    style()->drawPrimitive(QStyle::PE_FrameMenu, &opt, &p, this);
}

void DockBarWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (m_toolBar == Q_NULL)
    {
        Q_ASSERT(false);
        return;
    }
    DockToolBarPrivate* p = DockToolBarPrivate::_get(m_toolBar);
    p->initDragMode(m_toolBar->mapFrom(this, event->pos()), true);
}

void DockBarWindow::moveToMouse(const QPoint& mousePos)
{
    if (m_toolBar == Q_NULL)
    {
        Q_ASSERT(false);
        return;
    }
    QRect rect = geometry();
    QPoint pressedPos = DockToolBarPrivate::_get(m_toolBar)->m_pressedPos;
    pressedPos = mousePos - m_toolBar->mapTo(this, pressedPos);
    rect.moveTopLeft(pressedPos);
    setGeometry(rect);
    DockBarBasePrivate::_get(m_toolBar)->m_windowRect = rect;
}

/* DockToolBarPrivate */
DockToolBarPrivate::DockToolBarPrivate()
    : m_explicitIconSize(false)
    , m_explicitToolButtonStyle(false)
    , m_movable(true)
    , m_allowedAreas(Qtitan::DockBarNone)
    , m_toolButtonStyle(Qt::ToolButtonIconOnly)
    , m_layout(0)
    , m_dockBarWindow(Q_NULL)
    , m_dragging(false)
    , m_pressedPos(cursorPositionInvalid)
    , m_height(0)
{
}

DockToolBarPrivate::~DockToolBarPrivate()
{
}

void DockToolBarPrivate::initialization()
{
    QTN_P(DockToolBar);
    p.setAttribute(Qt::WA_Hover);
    p.setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    p.setBackgroundRole(QPalette::Button);
    p.setFont(QApplication::font("QMenu"));

    QStyle* style = p.style();
    int e = style->pixelMetric(QStyle::PM_ToolBarIconSize, 0, &p);
    m_iconSize = QSize(e, e);

    m_layout = new DockToolBarLayout(&p);
    m_layout->updateMarginAndSpacing();
    m_movable = p.style()->styleHint(QStyle::SH_ToolBar_Movable, 0, &p);
    m_layout->setSpacing(2);
}

QRect DockToolBarPrivate::toolBarHandleRect() const
{
    if (isFloating())
        return QRect();
    QTN_P(const DockToolBar);
    QStyleOptionToolBar opt;
    p.initStyleOption(&opt);
    return p.style()->subElementRect(QStyle::SE_ToolBarHandle, &opt, &p);
}

void DockToolBarPrivate::activateMainWindow()
{
    QTN_P(const DockToolBar);
    QWidget* managedWidget = barManager()->managedWidget();
    Q_ASSERT(managedWidget != 0);
    if (p.dockBarArea() == Qtitan::DockBarFloat)
        managedWidget->activateWindow();
    if (m_dockBarWindow != Q_NULL)
        m_dockBarWindow->titleBar()->setText(p.caption());
}

bool DockToolBarPrivate::isFloating() const
{
    QTN_P(const DockToolBar);
    return p.dockBarArea() == Qtitan::DockBarFloat;
}

bool DockToolBarPrivate::isDragging() const
{
    return m_dragging;
}

DockBarWindow* DockToolBarPrivate::getDockBarWindow()
{
    if (m_dockBarWindow == Q_NULL)
        m_dockBarWindow = new DockBarWindow(barManager());
    return m_dockBarWindow;
}

void DockToolBarPrivate::initStyleOption(QStyleOptionToolBar* option) const
{
    QTN_P(const DockToolBar);
    if (!option)
        return;
    option->initFrom(&p);
    if (orientation() == Qt::Horizontal)
        option->state |= QStyle::State_Horizontal;
    option->lineWidth = p.style()->pixelMetric(QStyle::PM_ToolBarFrameWidth, 0, &p);
    option->features = m_layout->movable() ? QStyleOptionToolBar::Movable : QStyleOptionToolBar::None;
    option->toolBarArea = Qt::NoToolBarArea;
}

void DockToolBarPrivate::initDragMode(const QPoint& pos, bool immediateStart)
{
    QTN_P(DockToolBar);
    if (m_dragging)
    {
        Q_ASSERT(false);
        return;
    }

    activateMainWindow();
    p.grabMouse(Qt::SizeAllCursor);

    m_pressedPos = pos;
    m_dragging = immediateStart;
    if (p.isRightToLeft())
        m_pressedPos = QPoint(p.width() - m_pressedPos.x(), m_pressedPos.y());
}

void DockToolBarPrivate::endDragMode()
{
    QTN_P(DockToolBar);
    m_pressedPos = cursorPositionInvalid;
    m_dragging = false;
    p.releaseMouse();
    if (p.dockBarArea() != Qtitan::DockBarFloat)
    {
        Q_DELETE_AND_NULL(m_dockBarWindow);
    }
    else
        activateMainWindow();
}

bool DockToolBarPrivate::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return true;

    if (!m_layout->movable())
        return true;

    if (toolBarHandleRect().contains(event->pos()))
    {
        initDragMode(event->pos(), false);
        return true;
    }
    return false;
}

bool DockToolBarPrivate::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return false;
    endDragMode();
    return true;
}

bool DockToolBarPrivate::mouseMoveEvent(QMouseEvent* event)
{
    QTN_P(DockToolBar);
    if (m_pressedPos == cursorPositionInvalid)
        return false;

    if (!m_dragging && (event->pos() - m_pressedPos).manhattanLength() > QApplication::startDragDistance())
        m_dragging = true;

    if (!m_dragging)
        return false;

    QRect rect = p.geometry();
    QPoint pos = event->globalPos() - m_pressedPos;
    rect.moveTopLeft(pos);

    bool docked = false;
    bool needGrabbed = false;
    if (DockBarSite* dockSite = DockBarManagerPrivate::_get(barManager())->canDock(event->globalPos(), m_dockSite))
    {
        Qtitan::DockBarArea position = dockSite->dockBarArea();
        if (position == Qtitan::DockBarTop   || position == Qtitan::DockBarLeft || 
            position == Qtitan::DockBarRight || position == Qtitan::DockBarBottom)
        {
            if (p.dockBarArea() == Qtitan::DockBarFloat)
            {
                Q_ASSERT(m_dockBarWindow != Q_NULL);
                if (m_dockBarWindow != Q_NULL)
                {
                    m_dockBarWindow->setDockBar(Q_NULL);
                    needGrabbed = true;
                }
            }

            DockBarSite* oldDockSite = m_dockSite;
            DockBarManagerPrivate::_get(barManager())->addDockBarToSite(&p, &rect, dockSite);
            if (dockSite != oldDockSite)
                p.layout()->activate();

            p.show();
            docked = true;
        }
    }

    if (!docked)
    {
        DockBarWindow* dockBarWindow = getDockBarWindow();
        if (dockBarWindow->toolBar() != &p)
        {
            dockBarWindow->setDockBar(&p);
            needGrabbed = true;
        }
        dockBarWindow->show();
        dockBarWindow->moveToMouse(event->globalPos());
    }

    if (needGrabbed)
        p.grabMouse(Qt::SizeAllCursor);

    return true;
}

/*!
\class DockToolBar
\inmodule QtitanDocking
\brief DockToolBar class provides a movable panel that contains a set of actions or widgets.
*/

/*!
Constructs the toolbar with the given \a parent.
*/
DockToolBar::DockToolBar(QWidget* parent)
    : DockBarBase(*new DockToolBarPrivate, parent, Qt::WindowFlags())
{
    QTN_D(DockToolBar);
    d.initialization();
}

/*!
Constructs the toolbar with the given \a caption and \a parent.
*/
DockToolBar::DockToolBar(const QString& caption, QWidget* parent)
    : DockBarBase(*new DockToolBarPrivate, parent, Qt::WindowFlags())
{
    QTN_D(DockToolBar);
    d.initialization();
    setCaption(caption);
}

DockToolBar::~DockToolBar()
{
}

/*! \reimp */
void DockToolBar::setVisible(bool visible)
{
    QTN_D(const DockToolBar);
    DockBarBase::setVisible(visible);

    if (d.m_dockBarWindow && d.isFloating())
        d.m_dockBarWindow->setVisible(visible);
}

/*!
\property DockToolBar::iconSize
The property contains the default size the icon at the tool buttons. By default it is equals to QStyle::PM_ToolBarIconSize metric.
*/
QSize DockToolBar::iconSize() const
{ 
    QTN_D(const DockToolBar); 
    return d.m_iconSize; 
}

void DockToolBar::setIconSize(const QSize& iconSize)
{
    QTN_D(DockToolBar);
    QSize sz = iconSize;
    if (!sz.isValid()) 
    {
/*
        MainWindow *mw = qobject_cast<MainWindow *>(parentWidget());
        if (mw && mw->m_layout()) {
            QLayout *m_layout = mw->m_layout();
            int i = 0;
            QLayoutItem *item = 0;
            do {
                item = m_layout->itemAt(i++);
                if (item && (item->widget() == this))
                    sz = mw->m_iconSize();
            } while (!sz.isValid() && item != 0);
        }
*/
    }
    if (!sz.isValid()) 
    {
        const int metric = style()->pixelMetric(QStyle::PM_ToolBarIconSize, 0, this);
        sz = QSize(metric, metric);
    }
    if (d.m_iconSize != sz) 
    {
        d.m_iconSize = sz;
        setMinimumSize(0, 0);
        emit iconSizeChanged(d.m_iconSize);
    }
    d.m_explicitIconSize = iconSize.isValid();
    d.m_layout->invalidate();
}

/*!
\property DockToolBar::toolButtonStyle
The property contains the default style for the tool buttons. By default it is Qt::ToolButtonIconOnly.
*/
Qt::ToolButtonStyle DockToolBar::toolButtonStyle() const
{ 
    QTN_D(const DockToolBar); 
    return d.m_toolButtonStyle; 
}

void DockToolBar::setToolButtonStyle(Qt::ToolButtonStyle m_toolButtonStyle)
{
    QTN_D(DockToolBar);
    d.m_explicitToolButtonStyle = true;
    if (d.m_toolButtonStyle == m_toolButtonStyle)
        return;
    d.m_toolButtonStyle = m_toolButtonStyle;
    setMinimumSize(0, 0);
    emit toolButtonStyleChanged(d.m_toolButtonStyle);
}

void DockToolBar::clear()
{
    QList<QAction*> actions = this->actions();
    for(int i = 0; i < actions.size(); i++)
        removeAction(actions.at(i));
}

QAction* DockToolBar::addAction(const QString& text)
{
    QAction* action = new QAction(text, this);
    addAction(action);
    return action;
}

QAction* DockToolBar::addAction(const QIcon& icon, const QString& text)
{
    QAction* action = new QAction(icon, text, this);
    addAction(action);
    return action;
}

QAction* DockToolBar::addAction(const QString& text, const QObject* receiver, const char* member)
{
    QAction* action = new QAction(text, this);
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    addAction(action);
    return action;
}

QAction* DockToolBar::addAction(const QIcon& icon, const QString& text,
                                const QObject* receiver, const char* member)
{
    QAction* action = new QAction(icon, text, this);
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    addAction(action);
    return action;
}

QAction* DockToolBar::addSeparator()
{
    QAction* action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

QAction* DockToolBar::insertSeparator(QAction *before)
{
    QAction* action = new QAction(this);
    action->setSeparator(true);
    insertAction(before, action);
    return action;
}

QAction* DockToolBar::addWidget(QWidget* widget)
{
    QWidgetAction* action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    addAction(action);
    return action;
}

QAction* DockToolBar::insertWidget(QAction* before, QWidget* widget)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    insertAction(before, action);
    return action;
}

QRect DockToolBar::actionGeometry(QAction* action) const
{
    QTN_D(const DockToolBar);
    int index = d.m_layout->getActionIndex(action);
    if (index == -1)
        return QRect();
    return d.m_layout->itemAt(index)->widget()->geometry();
}

QAction* DockToolBar::actionAt(const QPoint& p) const
{
    QTN_D(const DockToolBar);
    QWidget* widget = childAt(p);
    int index = d.m_layout->indexOf(widget);
    if (index == -1)
        return 0;
    QLayoutItem *item = d.m_layout->itemAt(index);
    return static_cast<DockToolBarItem*>(item)->action;
}

QAction* DockToolBar::actionAt(int ax, int ay) const
{
    return actionAt(QPoint(ax, ay));
}

void DockToolBar::actionEvent(QActionEvent* event)
{
    QTN_D(DockToolBar);
    QAction* action = event->action();
    QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action);

    switch (event->type()) 
    {
        case QEvent::ActionAdded: 
        {
            Q_ASSERT_X(widgetAction == 0 || d.m_layout->getActionIndex(widgetAction) == -1,
                        "DockToolBar", "widgets cannot be inserted multiple times");

            if (widgetAction != 0)
                widgetAction->setParent(this);

            int index = d.m_layout->count();
            if (event->before()) 
            {
                index = d.m_layout->getActionIndex(event->before());
                Q_ASSERT_X(index != -1, "DockToolBar::insertAction", "internal error");
            }
            d.m_layout->insertAction(index, action);
            break;
        }

        case QEvent::ActionChanged:
            d.m_layout->invalidate();
            break;

        case QEvent::ActionRemoved: 
        {
            int index = d.m_layout->getActionIndex(action);
            if (index != -1)
                delete d.m_layout->takeAt(index);
            break;
        }
        default:
            Q_ASSERT_X(false, "DockToolBar::actionEvent", "internal error");
    }
    if (d.barManager())
        d.m_height = DockBarManagerPrivate::_get(d.barManager())->calcMaxHeightBars();
}

void DockToolBar::changeEvent(QEvent* event)
{
    QTN_D(DockToolBar);
    switch (event->type()) 
    {
        case QEvent::StyleChange:
            d.m_layout->invalidate();
            if (!d.m_explicitIconSize)
                setIconSize(QSize());
            d.m_layout->updateMarginAndSpacing();
            break;
        case QEvent::LayoutDirectionChange:
            d.m_layout->invalidate();
            break;
        default:
            break;
    }
    QWidget::changeEvent(event);
}

void DockToolBar::paintEvent(QPaintEvent* event)
{
    QTN_D(DockToolBar);
    Q_UNUSED(event);

    QPainter p(this);
    QStyleOptionToolBar opt;
    initStyleOption(&opt);

    if (!d.isFloating())
    {
        p.fillRect(opt.rect, palette().window());
        style()->drawControl(QStyle::CE_ToolBar, &opt, &p, this);

        opt.rect = style()->subElementRect(QStyle::SE_ToolBarHandle, &opt, this);
        if (opt.rect.isValid())
            style()->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, &p, this);
    }
}

static bool waitForPopup(DockToolBar* tb, QWidget* popup)
{
    if (popup == 0 || popup->isHidden())
        return false;

    QWidget* w = popup;
    while (w != 0) 
    {
        if (w == tb)
            return true;
        w = w->parentWidget();
    }

    QMenu* menu = qobject_cast<QMenu*>(popup);
    if (menu == 0)
        return false;

    QAction* action = menu->menuAction();
    QList<QWidget*> widgets = action->associatedWidgets();
    for (int i = 0; i < widgets.count(); ++i) 
    {
        if (waitForPopup(tb, widgets.at(i)))
            return true;
    }
    return false;
}

bool DockToolBar::event(QEvent* event)
{
    QTN_D(DockToolBar);
    switch (event->type()) 
    {
        case QEvent::Timer:
            if (d.waitForPopupTimer.timerId() == static_cast<QTimerEvent*>(event)->timerId()) 
            {
                QWidget *w = QApplication::activePopupWidget();
                if (!waitForPopup(this, w)) 
                {
                    d.waitForPopupTimer.stop();
                    if (!this->underMouse())
                        d.m_layout->setExpanded(false);
                }
            }
            break;
        case QEvent::Hide:
        case QEvent::Show:
            if (DockBarManager* manager = d.barManager())
            {
                if (QLayout* layout = manager->managedWidget()->layout())
                    layout->update();
            }
            break;
        case QEvent::ParentChange:
            d.m_layout->updateMarginAndSpacing();
            d.m_layout->checkUsePopupMenu();
            break;

        case QEvent::MouseButtonPress: 
        {
            if (d.mousePressEvent(static_cast<QMouseEvent*>(event)))
                return true;
            break;
        }
        case QEvent::MouseButtonRelease:
            if (d.mouseReleaseEvent(static_cast<QMouseEvent*>(event)))
                return true;
            break;
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
            return true;
        case QEvent::HoverMove: 
        {
            QHoverEvent *e = static_cast<QHoverEvent*>(event);
            if (d.toolBarHandleRect().contains(e->pos()))
                setCursor(Qt::SizeAllCursor);
            else
                unsetCursor();
            break;
        }
        case QEvent::MouseMove:
            if (d.mouseMoveEvent(static_cast<QMouseEvent*>(event)))
                return true;
            break;
        case QEvent::Leave:
#if 0
            if (d.m_state != 0 && d.m_state->dragging) 
            {
                QPoint pos = QCursor::pos();
                QMouseEvent fake(QEvent::MouseMove, mapFromGlobal(pos), pos, Qt::NoButton,
                                 QApplication::mouseButtons(), QApplication::keyboardModifiers());
                d.mouseMoveEvent(&fake);
            } 
            else 
#endif
            {
                if (!d.m_layout->expanded)
                    break;

                QWidget* w = QApplication::activePopupWidget();
                if (waitForPopup(this, w)) 
                {
                    d.waitForPopupTimer.start(POPUP_TIMER_INTERVAL, this);
                    break;
                }

                d.waitForPopupTimer.stop();
                d.m_layout->setExpanded(false);
                break;
            }
        default:
            break;
    }
    return DockBarBase::event(event);
}

QWidget* DockToolBar::widgetForAction(QAction* action) const
{
    QTN_D(const DockToolBar);
    int index = d.m_layout->getActionIndex(action);
    if (index == -1)
        return 0;
    return d.m_layout->itemAt(index)->widget();
}

void DockToolBar::initStyleOption(QStyleOptionToolBar* option) const
{
    QTN_D(const DockToolBar);
    d.initStyleOption(option);
}


