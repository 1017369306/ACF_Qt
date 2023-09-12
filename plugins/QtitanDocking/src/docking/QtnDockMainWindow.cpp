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

#include <QtGui>
#include <QApplication>
#include <QMenuBar>
#include <QToolTip>
#include <QStatusBar>

#include "QtnDockMainWindow.h"
#include "QtnDockToolBarPrivate.h"
#include "QtnDockMainWindowPrivate.h"
#include "QtnDockBarManagerPrivate.h"
#include "QtnDockToolBar.h"
#include "QtnDockBarSite.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif


QTITAN_USE_NAMESPACE

/* \internal */
static void showToolTip(QHelpEvent *helpEvent, QWidget *widget, const QStyleOptionComplex &opt, QStyle::ComplexControl complexControl, QStyle::SubControl subControl)
{
    Q_ASSERT(helpEvent);
    Q_ASSERT(helpEvent->type() == QEvent::ToolTip);
    Q_ASSERT(widget);

    // Convert CC_MdiControls to CC_TitleBar. Sub controls of different complex
    // controls cannot be in the same switch as they might have the same value.
    if (complexControl == QStyle::CC_MdiControls)
    {
        if (subControl == QStyle::SC_MdiMinButton)
            subControl = QStyle::SC_TitleBarMinButton;
        else if (subControl == QStyle::SC_MdiCloseButton)
            subControl = QStyle::SC_TitleBarCloseButton;
        else if (subControl == QStyle::SC_MdiNormalButton)
            subControl = QStyle::SC_TitleBarNormalButton;
        else
            subControl = QStyle::SC_None;
    }

    // Don't change the tooltip for the base widget itself.
    if (subControl == QStyle::SC_None)
        return;

    QString toolTip;

    switch (subControl)
    {
        case QStyle::SC_TitleBarMinButton:
            toolTip = QMdiSubWindow::tr("Minimize");
            break;
        case QStyle::SC_TitleBarMaxButton:
            toolTip = QMdiSubWindow::tr("Maximize");
            break;
        case QStyle::SC_TitleBarUnshadeButton:
            toolTip = QMdiSubWindow::tr("Unshade");
            break;
        case QStyle::SC_TitleBarShadeButton:
            toolTip = QMdiSubWindow::tr("Shade");
            break;
        case QStyle::SC_TitleBarNormalButton:
            if (widget->isMaximized() || !qobject_cast<QMdiSubWindow *>(widget))
                toolTip = QMdiSubWindow::tr("Restore Down");
            else
                toolTip = QMdiSubWindow::tr("Restore");
            break;
        case QStyle::SC_TitleBarCloseButton:
            toolTip = QMdiSubWindow::tr("Close");
            break;
        case QStyle::SC_TitleBarContextHelpButton:
            toolTip = QMdiSubWindow::tr("Help");
            break;
        case QStyle::SC_TitleBarSysMenu:
            toolTip = QMdiSubWindow::tr("Menu");
            break;
        default:
            break;
    }

    const QRect rect = widget->style()->subControlRect(complexControl, &opt, subControl, widget);
    QToolTip::showText(helpEvent->globalPos(), toolTip, widget, rect);
}

/* \internal */
ControllerWidget::ControllerWidget(QMdiArea* mdiArea, QWidget *parent)
    : QWidget(parent)
    , activeControl(QStyle::SC_None)
    , hoverControl(QStyle::SC_None)
    , visibleControls(QStyle::SC_None)
    , mdiArea(mdiArea)
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMouseTracking(true);
}

/* \internal */
QSize ControllerWidget::sizeHint() const
{
    ensurePolished();
    QStyleOptionComplex opt;
    initStyleOption(&opt);
    QSize size(48, 16);
    return style()->sizeFromContents(QStyle::CT_MdiControls, &opt, size, mdiArea);
}

/* \internal */
void ControllerWidget::setControlVisible(WindowStateAction action, bool visible)
{
    QStyle::SubControl subControl = QStyle::SC_None;

    // Map action from QMdiSubWindowPrivate::WindowStateAction to QStyle::SubControl.
    if (action == MaximizeAction)
        subControl = QStyle::SC_MdiNormalButton;
    else if (action == CloseAction)
        subControl = QStyle::SC_MdiCloseButton;
    else if (action == MinimizeAction)
        subControl = QStyle::SC_MdiMinButton;

    if (subControl == QStyle::SC_None)
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    visibleControls.setFlag(subControl, visible && !(visibleControls & subControl));
#else
    if (visible && !(visibleControls & subControl))
        visibleControls |= subControl;
    else if (!visible && (visibleControls & subControl))
        visibleControls &= ~subControl;
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
}

/* \internal */
bool ControllerWidget::hasVisibleControls() const
{
    return (visibleControls & QStyle::SC_MdiMinButton) ||
        (visibleControls & QStyle::SC_MdiNormalButton) ||
        (visibleControls & QStyle::SC_MdiCloseButton);
}

/* \internal */
void ControllerWidget::paintEvent(QPaintEvent* paintEvent)
{
    Q_UNUSED(paintEvent)
    QStyleOptionComplex opt;
    initStyleOption(&opt);
    if (activeControl == hoverControl) 
    {
        opt.activeSubControls = activeControl;
        opt.state |= QStyle::State_Sunken;
    }
    else if (hoverControl != QStyle::SC_None && (activeControl == QStyle::SC_None))
    {
        opt.activeSubControls = hoverControl;
        opt.state |= QStyle::State_MouseOver;
    }
    QPainter painter(this);
    style()->drawComplexControl(QStyle::CC_MdiControls, &opt, &painter, mdiArea);
}

/* \internal */
void ControllerWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    activeControl = getSubControl(event->pos());
    update();
}

/* \internal */
void ControllerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }

    QStyle::SubControl under_mouse = getSubControl(event->pos());
    if (under_mouse == activeControl)
    {
        switch (activeControl)
        {
            case QStyle::SC_MdiCloseButton:
                emit _qtn_close();
                break;
            case QStyle::SC_MdiNormalButton:
                emit _qtn_restore();
                break;
            case QStyle::SC_MdiMinButton:
                emit _qtn_minimize();
                break;
            default:
                break;
        }
    }
    activeControl = QStyle::SC_None;
    update();
}

/* \internal */
void ControllerWidget::mouseMoveEvent(QMouseEvent *event)
{
    QStyle::SubControl under_mouse = getSubControl(event->pos());
    // test if hover state changes
    if (hoverControl != under_mouse)
    {
        hoverControl = under_mouse;
        update();
    }
}

/* \internal */
void ControllerWidget::leaveEvent(QEvent * /*event*/)
{
    hoverControl = QStyle::SC_None;
    update();
}

/* \internal */
bool ControllerWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QStyleOptionComplex opt;
        initStyleOption(&opt);
        QHelpEvent* helpEvent = static_cast<QHelpEvent *>(event);
        showToolTip(helpEvent, this, opt, QStyle::CC_MdiControls, getSubControl(helpEvent->pos()));
    }
    return QWidget::event(event);
}

/*
\internal
*/
void ControllerWidget::initStyleOption(QStyleOptionComplex *option) const
{
    option->initFrom(this);
    option->subControls = visibleControls;
    option->activeSubControls = QStyle::SC_None;
}

QStyle::SubControl ControllerWidget::getSubControl(const QPoint &pos) const
{
    QStyleOptionComplex opt;
    initStyleOption(&opt);
    return style()->hitTestComplexControl(QStyle::CC_MdiControls, &opt, pos, mdiArea);
}


/*DockMainWindowPrivate*/
DockMainWindowPrivate::DockMainWindowPrivate()
  : m_layout(Q_NULL)
{
}

DockMainWindowPrivate::~DockMainWindowPrivate()
{
    m_layout->setDockBarManager(Q_NULL);
    m_layout->setDockPanelManager(Q_NULL);
}

/* \internal */
void DockMainWindowPrivate::initialization()
{
    QTN_P(DockMainWindow);
    p.setAttribute(Qt::WA_CanHostQMdiSubWindowTitleBar);
    m_layout = new DockMainWindowLayout(&p);
    m_layout->setDockBarManager(new DockBarManager(&p));
    m_layout->setDockPanelManager(new DockPanelManager(&p));
}

/*! \internal */
WindowTitleBar* DockMainWindowPrivate::findTitleBar()
{
    QTN_P(DockMainWindow);
    DockMainWindow* mainWindow = &p;
    return p.isWindow() ? WindowTitleBar::find(mainWindow) : Q_NULL;
}

/* \internal */
ControllerWidget* DockMainWindowPrivate::controlWidget() const
{
    QTN_P(const DockMainWindow);
    if (QMenuBar* bar = p.menuBar())
        return qobject_cast<ControllerWidget *>(bar->cornerWidget(Qt::TopRightCorner));
    return Q_NULL;
}

/* \internal */
void DockMainWindowPrivate::setVisibleAction(WindowStateAction action, bool visible)
{
    if (ControllerWidget* ctrlWidget = controlWidget())
        ctrlWidget->setControlVisible(action, visible);
}

/* \internal */
void DockMainWindowPrivate::updateActions(QMdiSubWindow* window)
{
    if (window == Q_NULL)
        return;
    Qt::WindowFlags wndFlags = window->windowFlags();
    // Hide all
    for (int i = 0; i < NumWindowStateActions; ++i)
        setVisibleAction(WindowStateAction(i), false);

    if (wndFlags & Qt::FramelessWindowHint)
        return;

    // CloseAction
    if (wndFlags & Qt::WindowSystemMenuHint)
        setVisibleAction(CloseAction, true);

    // RestoreAction
    if (wndFlags & (Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint))
        setVisibleAction(RestoreAction, true);

    // MinimizeAction
    if (wndFlags & Qt::WindowMinimizeButtonHint)
        setVisibleAction(MinimizeAction, true);

    // MaximizeAction
    if (wndFlags & Qt::WindowMaximizeButtonHint)
        setVisibleAction(MaximizeAction, true);
}

/* \internal */
void DockMainWindowPrivate::subWindowActivated(QMdiSubWindow* window)
{
    if (ControllerWidget* ctrlWidget = controlWidget())
    {
        if (window && window->isMaximized())
        {
            ctrlWidget->show();
            updateActions(window);
        }
        else
            ctrlWidget->hide();
    }
}

/*!
    \internal
    \reimp
*/
bool DockMainWindowPrivate::eventFilter(QObject* watched, QEvent* event)
{
    QTN_P(DockMainWindow);
    if (QMdiSubWindow* subWindow = qobject_cast<QMdiSubWindow *>(watched))
    {
        if (event->type() == QEvent::WindowStateChange)
        {
            QWindowStateChangeEvent *changeEvent = static_cast<QWindowStateChangeEvent*>(event);
            if (changeEvent->isOverride())
                return QObject::eventFilter(watched, event);

            Qt::WindowStates oldState = changeEvent->oldState();
            Qt::WindowStates newState = subWindow->windowState();
            if (oldState != newState)
            {
                subWindowActivated(subWindow);
                if (subWindow->isMaximized())
                {
                    if (QMenuBar* bar = p.menuBar())
                    {
                        QEvent e(QEvent::LayoutDirectionChange);
                        QApplication::sendEvent(bar, &e);
                    }
                }
            }
        }
    }

    if (qobject_cast<QMdiArea*>(watched->parent()))
    {
        switch (event->type())
        {
            case QEvent::ChildAdded:
            {
                if (QObject* addedChild = static_cast<QChildEvent *>(event)->child())
                {
                    addedChild->installEventFilter(this);
                    if (ControllerWidget* ctrlWidget = controlWidget())
                    {
                        connect(ctrlWidget, SIGNAL(_qtn_close()), addedChild, SLOT(close()));
                        connect(ctrlWidget, SIGNAL(_qtn_restore()), addedChild, SLOT(showNormal()));
                        connect(ctrlWidget, SIGNAL(_qtn_minimize()), addedChild, SLOT(showMinimized()));
                    }
                }
            }
            break;
            case QEvent::ChildRemoved:
            {
                if (QObject* removedChild = static_cast<QChildEvent *>(event)->child())
                {
                    removedChild->removeEventFilter(this);
                    if (ControllerWidget* ctrlWidget = controlWidget())
                        ctrlWidget->hide();
                }
            }
            break;
            default:
                break;
        }
    }
    return QObject::eventFilter(watched, event);
}


/*!
\class DockMainWindow
\inmodule QtitanDocking
\brief The main window of an application that has a mechanism of dock panels and toolbars.
It has a customized dock manager and all the necessary functions for adding dockable panels and bars.
By analogy with QMainWindow it has a references to the QMenuBar and to QStatusBar.
\sa DockPanelManager
*/
/*!
Constructor of the class DockMainWindow. Parameter the \c parent by default is NULL and \c flags by default is Qt::WindowFlags().
*/
DockMainWindow::DockMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags.testFlag(Qt::Widget) ? flags : flags | Qt::Window)
{
    QTN_INIT_PRIVATE(DockMainWindow);
    QTN_D(DockMainWindow);
    d.initialization();
}

DockMainWindow::~DockMainWindow()
{
    QTN_FINI_PRIVATE();
}

/*!
Returns title bar for the main window.
*/
WindowTitleBar* DockMainWindow::titleBar() const
{
    return WindowTitleBar::get(const_cast<DockMainWindow *>(this));
}

/*!
Returns QWidget embeded to the title bar. It is equals titleBar()->widget()
*/
QWidget* DockMainWindow::titleBarWidget() const
{
    return titleBar()->widget();
}

/*!
Sets QWidget \a widget to the title bar. It is equals to titleBar()->setWidget(widget)
*/
void DockMainWindow::setTitleBarWidget(QWidget* widget)
{
    titleBar()->setWidget(widget);
}

/*!
Returns the menu bar for the main window.This function creates and returns an empty menu bar if the menu bar does not exist.
*/
QMenuBar* DockMainWindow::menuBar() const
{
    QMenuBar *menuBar = qobject_cast<QMenuBar *>(layout()->menuBar());
    if (!menuBar)
    {
        DockMainWindow* self = const_cast<DockMainWindow *>(this);
        menuBar = new QMenuBar(self);
        self->setMenuBar(menuBar);
    }
    return menuBar;
}

/*!
Sets the menu bar for the main window to menuBar.

Note: DockMainWindow takes ownership of the menuBar pointer and deletes it at the appropriate time.
*/
void DockMainWindow::setMenuBar(QMenuBar* bar)
{
    QLayout* topLayout = layout();

    if (topLayout->menuBar() && topLayout->menuBar() != bar)
    {
        // Reparent corner widgets before we delete the old menu bar.
        QMenuBar* oldMenuBar = qobject_cast<QMenuBar *>(topLayout->menuBar());
        if (bar)
        {
            // TopLeftCorner widget.
            QWidget* cornerWidget = oldMenuBar->cornerWidget(Qt::TopLeftCorner);
            if (cornerWidget)
                bar->setCornerWidget(cornerWidget, Qt::TopLeftCorner);
            // TopRightCorner widget.
            cornerWidget = oldMenuBar->cornerWidget(Qt::TopRightCorner);
            if (cornerWidget)
                bar->setCornerWidget(cornerWidget, Qt::TopRightCorner);
        }
        oldMenuBar->hide();
        oldMenuBar->deleteLater();
    }
    topLayout->setMenuBar(bar);
}

/*!
Returns the status bar for the main window. This function creates and returns an empty status bar QStatusBar if the status bar does not exist.
*/
QStatusBar* DockMainWindow::statusBar() const
{
    QTN_D(const DockMainWindow);

    QStatusBar* statusbar = d.m_layout->statusBar();
    if (!statusbar)
    {
        DockMainWindow* self = const_cast<DockMainWindow *>(this);
        statusbar = new QStatusBar(self);
        statusbar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        self->setStatusBar(statusbar);
    }
    return statusbar;
}

/*!
Sets the status bar for the main window to statusbar. Setting the status bar to 0 will remove it from the main window.

Note: DockMainWindow takes ownership of the statusbar pointer and deletes it at the appropriate time.
*/
void DockMainWindow::setStatusBar(QStatusBar* statusbar)
{
    QTN_D(DockMainWindow);
    if (d.m_layout->statusBar() && d.m_layout->statusBar() != statusbar)
    {
        d.m_layout->statusBar()->hide();
        d.m_layout->statusBar()->deleteLater();
    }
    d.m_layout->setStatusBar(statusbar);
}

/*!
Returns the central widget for the main window. This function returns zero if the central widget has not been set.
*/
QWidget* DockMainWindow::centralWidget() const
{
    return dockPanelManager()->centralWidget();
}

/*!
Sets the given widget to be the main window's central widget.

Note: DockMainWindow takes ownership of the widget pointer and deletes it at the appropriate time.
*/
void DockMainWindow::setCentralWidget(QWidget* widget)
{
    QTN_D(DockMainWindow);
    dockPanelManager()->setCentralWidget(widget);

    if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
    {
        mdiArea->viewport()->installEventFilter(&d);
        if (QMenuBar* bar = menuBar())
        {
            ControllerWidget* controllerWidget = new ControllerWidget(mdiArea, Q_NULL);
            bar->setCornerWidget(controllerWidget, Qt::TopRightCorner);
            controllerWidget->hide();
        }
    }
}

/*!
Returns DockBarManager that manages the main window.
*/
DockBarManager* DockMainWindow::dockBarManager() const
{
    QTN_D(const DockMainWindow);
    DockBarManager* dockBarManager = d.m_layout->dockBarManager();
    Q_ASSERT(dockBarManager != Q_NULL);
    return dockBarManager;
}

/*!
Returns DockPanelManager that manages the main window.
*/
DockPanelManager* DockMainWindow::dockPanelManager() const
{
    QTN_D(const DockMainWindow);
    DockPanelManager* dockPanelManager = d.m_layout->dockPanelManager();
    Q_ASSERT(dockPanelManager != Q_NULL);
    return dockPanelManager;
}

/*!
Forwards the call to DockBarManager that manages the main window.
See DockBarManager::dockBars() description for the details.
*/
const QList<DockBarBase*>& DockMainWindow::dockBars() const
{
    return dockBarManager()->dockBars();
}

/*!
Forwards the call to DockBarManager that manages the main window.
See DockBarManager::addToolBar(const QString& name, DockBarArea position) description for the details.
*/
DockToolBar* DockMainWindow::addToolBar(const QString& name, Qtitan::DockBarArea position)
{
    return dockBarManager()->addToolBar(name, position);
}

/*!
Forwards the call to DockBarManager that manages the main window.
See DockBarManager::insertDockBar(DockBarBase* bar, DockBarArea position) description for the details.
*/
bool DockMainWindow::insertDockBar(DockToolBar* bar, Qtitan::DockBarArea position)
{
    return dockBarManager()->insertDockBar(bar, position);
}

/*!
Forwards the call to DockBarManager that manages the main window.
See DockBarManager::insertDockBar(DockToolBar* before, DockToolBar* bar) description for the details.
*/
void DockMainWindow::insertDockBar(DockToolBar* before, DockToolBar* bar)
{
    dockBarManager()->insertDockBar(before, bar);
}

/*!
Forwards the call to DockBarManager that manages the main window.
See DockBarManager::removeDockBar(DockBarBase* bar) description for the details.
*/
void DockMainWindow::removeDockBar(DockToolBar* bar)
{
    dockBarManager()->removeDockBar(bar);
}

/*!
Forwards the call to DockBarManager that manages the main window.
See DockBarManager:::moveDockBar(DockToolBar* before, DockToolBar* bar) description for the details.
*/
void DockMainWindow::moveDockBar(DockToolBar* before, DockToolBar* bar)
{
    dockBarManager()->moveDockBar(before, bar);
}

/*!
Forwards the call to DockBarManager::clearDockBars() that manages the main window.
\sa DockBarManager::clearDockBars()
*/
void DockMainWindow::clearDockBars()
{
    dockBarManager()->clearDockBars();
}

/*!
Forwards the call to DockPanelManager::addDockWindow() that manages the main window.
\sa DockPanelManager::addDockWindow()
*/
DockWindow* DockMainWindow::addDockWindow(DockPanelBase* panel, const QRect& rect)
{
    return dockPanelManager()->addDockWindow(panel, rect);
}

/*!
Forwards the call to DockPanelManager::addDocumentPanel() that manages the main window.
\sa DockPanelManager::addDocumentPanel()
*/
DockDocumentPanel* DockMainWindow::addDocumentPanel(const QString& title)
{
    return dockPanelManager()->addDocumentPanel(title);
}

/*!
Forwards the call to DockPanelManager::insertDocumentPanel() that manages the main window.
\sa DockPanelManager::insertDocumentPanel()
*/
void DockMainWindow::insertDocumentPanel(DockDocumentPanel* documentPanel)
{
    return dockPanelManager()->insertDocumentPanel(documentPanel);
}

/*!
Forwards the call to DockPanelManager that manages the main window.
See DockPanelManager::addDockPanel(const QString& title, DockPanelArea area, DockPanelBase* before)
description for the details.
*/
DockWidgetPanel* DockMainWindow::addDockPanel(const QString& title, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    return dockPanelManager()->addDockPanel(title, area, targetPanel);
}

/*!
Forwards the call to DockPanelManager that manages the main window.
See DockPanelManager::addDockPanel(const QString& title, const QSize& sizeHint, DockPanelArea area, DockPanelBase* before)
description for the details.
*/
DockWidgetPanel* DockMainWindow::addDockPanel(const QString& title, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    return dockPanelManager()->addDockPanel(title, sizeHint, area, targetPanel);
}

/*!
Forwards the call to DockPanelManager that manages the main window.
See DockPanelManager::insertDockPanel(DockWidgetPanel* panel, DockPanelArea area, DockPanelBase* before) description for the details.
*/
void DockMainWindow::insertDockPanel(DockWidgetPanel* panel, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    dockPanelManager()->insertDockPanel(panel, area, targetPanel);
}

/*!
Forwards the call to DockPanelManager that manages the main window.
See DockPanelManager::insertDockPanel(DockWidgetPanel* panel, const QSize& sizeHint, DockPanelArea area, DockPanelBase* before)
description for the details.
*/
void DockMainWindow::insertDockPanel(DockWidgetPanel* panel, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    dockPanelManager()->insertDockPanel(panel, sizeHint, area, targetPanel);
}

/*!
Forwards the call to DockPanelManager that manages the main window.
See DockPanelManager::removeDockPanel(DockWidgetPanel* panel) description for the details.
*/
void DockMainWindow::removeDockPanel(DockWidgetPanel* panel)
{
    dockPanelManager()->removeDockPanel(panel);
}

/*!
Forwards the call to DockPanelManager that manages the main window.
See DockPanelManager::clearDockPanels() description for the details.
*/
void DockMainWindow::clearDockPanels()
{
    dockPanelManager()->clearDockPanels();
}

/*!
Saves the dock panels and bars state to the \a device.
*/
void DockMainWindow::saveStateToDevice(QIODevice* deviceBars, QIODevice* devicePanels)
{
    if (deviceBars)
        dockBarManager()->saveStateToDevice(deviceBars);
    if (devicePanels)
        dockPanelManager()->saveStateToDevice(devicePanels);
}

/*!
Restores the dock panels and bars state from the \a device.
*/
bool DockMainWindow::loadStateFromDevice(QIODevice* deviceBars, QIODevice* devicePanels)
{
    if (deviceBars && devicePanels)
        return dockBarManager()->loadStateFromDevice(deviceBars) && dockPanelManager()->loadStateFromDevice(devicePanels);
    else if (deviceBars)
        return dockBarManager()->loadStateFromDevice(deviceBars);
    else if (devicePanels)
        return dockPanelManager()->loadStateFromDevice(devicePanels);
    return false;
}

/*! \reimp */
bool DockMainWindow::event(QEvent* event)
{
    QTN_D(DockMainWindow);
    switch (event->type())
    {
        case QEvent::StatusTip:
            if (QStatusBar* sb = d.m_layout->statusBar())
                sb->showMessage(static_cast<QStatusTipEvent*>(event)->tip());
            else
                static_cast<QStatusTipEvent*>(event)->ignore();
            return true;
        default:
            break;
    }
    return QWidget::event(event);
}

/*! \reimp */
void DockMainWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    QTN_D(DockMainWindow);
    d.m_layout->invalidate();
}


