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

#include <QPainter>
#include <QTimer>
#include <QBoxLayout>
#include <QMouseEvent>

#include "QtnDockWindowPrivate.h"
#include "QtnDockTitleBarPrivate.h"
#include "QtnDockDocumentPanelPrivate.h"
#include "QtnDockPanelManagerPrivate.h"
#include "QtnDockWidgetDragDrop.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

/* DockWindowBase */
DockWindowBase::DockWindowBase(DockPanelManager* manager)
    : QWidget(manager->managedWidget())
{
}

DockWindowBase::~DockWindowBase()
{
}

/* DockWindowLayout */
DockWindowLayout::DockWindowLayout(QWidget* parent)
    : QLayout(parent)
    , m_verticalTitleBar(false)
    , m_itemList(RoleCount, 0)
{
    setSpacing(0);
    setContentsMargins(QMargins());
}

DockWindowLayout::~DockWindowLayout()
{
    qDeleteAll(m_itemList);
}

void DockWindowLayout::addItem(QLayoutItem* item) { Q_UNUSED(item); }
QLayoutItem* DockWindowLayout::itemAt(int index) const { Q_UNUSED(index); return 0; }
QLayoutItem* DockWindowLayout::takeAt(int index) { Q_UNUSED(index); return 0; }
int DockWindowLayout::count() const { return 0; }
QSize DockWindowLayout::sizeHint() const { return QSize(0, 0); }

/*! \internal */
QSize DockWindowLayout::maximumSize() const
{
    if (DockPanelBase* panelBase = qobject_cast<DockPanelBase*>(widgetForRole(Content)))
    {
        qtnMinMaxSize minmaxSize;
        DockPanelBasePrivate::_get(panelBase)->getMinMaxData(minmaxSize);
        return minmaxSize.maxTrackSize;
    }
    return parentWidget()->maximumSize();
}

/*! \internal */
QSize DockWindowLayout::minimumSize() const
{
    if (DockPanelBase* panelBase = qobject_cast<DockPanelBase*>(widgetForRole(Content)))
    {
        qtnMinMaxSize minmaxSize;
        DockPanelBasePrivate::_get(panelBase)->getMinMaxData(minmaxSize);
        return minmaxSize.minTrackSize;
    }
    return parentWidget()->minimumSize();
}

/*! \internal */
void DockWindowLayout::setGeometry(const QRect& geometry)
{
    QLayout::setGeometry(geometry);
    updateGeometry();
}

/*! \internal */
void DockWindowLayout::updateGeometry()
{
    QRect geometry = this->geometry();
    const int fw = 0;
    int titleHeight = 0;
    if (QLayoutItem* item = m_itemList[TitleBar])
    {
        if (item->widget())
        {
            titleHeight = item->widget()->sizeHint().height();
            m_titleArea = QRect(geometry.topLeft(), QSize(geometry.width(), titleHeight));
            item->setGeometry(m_titleArea);
        }
    }

    if (QLayoutItem* item = m_itemList[Content])
    {
        if (item->widget() != Q_NULL)
        {
            QRect rect = geometry;
            if (m_verticalTitleBar)
            {
                if (!m_titleArea.isEmpty())
                    rect.adjust(m_titleArea.right(), 0, 0, 0);
                rect.adjust(0, fw, -fw, -fw);
            }
            else
            {
                if (!m_titleArea.isEmpty())
                    rect.adjust(0, m_titleArea.bottom(), 0, 0);
                rect.adjust(fw, 0, -fw, -fw);
            }
            item->setGeometry(rect);
            if (DockPanelBase* panel = qobject_cast<DockPanelBase*>(item->widget()))
            {
                DockPanelBasePrivate::_get(panel)->setPanelGeometry(rect);
                DockPanelManagerPrivate::_get(panel->dockManager())->preservePanelState();
            }
        }
    }
}

/*! \internal */
QWidget* DockWindowLayout::widgetForRole(Role r) const
{
    QLayoutItem* item = m_itemList.at(r);
    return item == Q_NULL ? Q_NULL : item->widget();
}

/*! \internal */
void DockWindowLayout::setWidgetForRole(Role r, QWidget* w)
{
    QWidget* old = widgetForRole(r);
    if (old != Q_NULL)
    {
        old->hide();
        removeWidget(old);
    }

    if (w != Q_NULL)
    {
        addChildWidget(w);
        m_itemList[r] = new QWidgetItemV2(w);
        w->show();
    }
    else
        m_itemList[r] = Q_NULL;
    invalidate();
}

/* DockWindowPrivate */
DockWindowPrivate::DockWindowPrivate(DockLayoutEngine* layoutEngine)
    : QObject()
    , m_layoutEngine(layoutEngine)
    , m_rootPanel(Q_NULL)
    , m_internalLayout(Q_NULL)
    , m_actCloseButton(Q_NULL)
    , m_sizeDocking(QSize(100, 100))
    , m_active(false)
    , m_hasGeometrySet(false)
{
}

/*! \internal */
DockWindowPrivate::~DockWindowPrivate()
{
    QTN_P(DockWindow);
    if (DockLayoutEngine* engine = layoutEngine())
        engine->resetDockWindow(&p);
}

/*! \internal */
void DockWindowPrivate::checkToFinalize()
{
    QTN_P(DockWindow);
    if (m_rootPanel == Q_NULL)
        return;

    bool isEmptyDoc = false;
    DockDocumentLayoutPanel* documentLayoutPanel = m_rootPanel->childCount() > 0 ? qobject_cast<DockDocumentLayoutPanel*>(m_rootPanel->panels().at(0)) : Q_NULL;
    if (documentLayoutPanel && DockDocumentLayoutPanelPrivate::_get(documentLayoutPanel)->isEmptyFull())
        isEmptyDoc = true;

    if ((m_rootPanel->isEmpty()) || isEmptyDoc)
        p.finalizationWindow(); // Hide the float dock window if we have no stuff in it.

    updatePanel(false);
}

/*! \internal */
void DockWindowPrivate::initialization()
{
    QTN_P(DockWindow);
    layoutEngine()->attachDockWindow(&p);
    p.setFocusPolicy(Qt::NoFocus);

    QStyleOptionTitleBar options;
    options.initFrom(&p);

    int height = p.style()->pixelMetric(QStyle::PM_TitleBarHeight, &options);
    int width = p.style()->subElementRect(QStyle::SE_DockWidgetCloseButton, &options, &p).width();
    int frameWidth = p.style()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, Q_NULL, &p) + 8;
    QSize sz(width * 3 + frameWidth, height + frameWidth);
    p.setMinimumSize(sz);

    m_internalLayout = new DockWindowLayout(&p);
    m_internalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    TitleBarWrapper* titleBarWrapper = new TitleBarWrapper(dockManager(), &p);
    m_actCloseButton = new QAction(QObject::tr("Close"), &p);
    m_actCloseButton->setProperty(_qtn_Type_Button, CloseButton);
    DockTitleBar* titleBarWidget = new DockTitleBar(titleBarWrapper);

    QString text = dockManager()->managedWidget()->windowTitle();
    if (text.isEmpty())
        text = qApp->applicationName();

    titleBarWidget->setText(text);
    titleBarWidget->setNoDrawGrip(false);
    titleBarWidget->setFocusPolicy(Qt::NoFocus);
    titleBarWidget->setCloseButtonVisible(true);
    titleBarWrapper->setTitleBar(titleBarWidget);
    m_internalLayout->setWidgetForRole(DockWindowLayout::TitleBar, titleBarWrapper);
    titleBarWrapper->setVisible(false);
    QObject::connect(titleBarWidget, &DockTitleBar::closeClicked, this, &DockWindowPrivate::closeDockPanel);
    titleBarWidget->addAction(m_actCloseButton);
    updateButtons();

    WindowTitleBar* windowTitleBar = WindowTitleBar::get(&p);
    windowTitleBar->setSysMenuButtonVisible(false);
}

/*! \internal */
void DockWindowPrivate::showOnScreen(const QRect& rect)
{
    QTN_P(DockWindow);
    if (!m_hasGeometrySet)
        p.setGeometry(rect);
    p.show();
}

/*! \internal */
void DockWindowPrivate::focusChanged() 
{
    QTN_P(DockWindow);
    QWidget* focus = QApplication::focusWidget();
    bool active = (focus && (focus == &p || p.isAncestorOf(focus) || (focus->parentWidget() && p.isAncestorOf(focus->parentWidget()))));
    if (active != m_active)
    {
        m_active = active;
        QList<DockPanelBase*> lstWidgetPanels = m_rootPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
        if (lstWidgetPanels.size() == 1)
        {
            if (DockWidgetPanel* wp = qobject_cast<DockWidgetPanel*>(lstWidgetPanels.at(0)))
            {
                if (DockTitleBar* titleWidget = qobject_cast<DockTitleBar*>(wp->titleBar()))
                    titleWidget->setTitleActive(m_active);
            }
        }
    }
}

/*! \internal */
void DockWindowPrivate::childLayoutChanged()
{
    Q_ASSERT(m_rootPanel != Q_NULL);
    m_hasGeometrySet = false;
    DockDocumentLayoutPanel* documentLayoutPanel = qobject_cast<DockDocumentLayoutPanel*>(m_rootPanel);
    if (m_rootPanel == Q_NULL || m_rootPanel->isEmpty() || (documentLayoutPanel && 
        DockDocumentLayoutPanelPrivate::_get(documentLayoutPanel)->isEmptyFull()))
    {
// TODO:       QMetaObject::invokeMethod(&p, "setVisible", Qt::QueuedConnection, Q_ARG(bool, false));
    }
    else
    {
        updatePanel(true);
        updatePosition();
    }
#ifdef Q_OS_WIN
    checkSizeConstraint();
#endif // Q_OS_WIN
}

/*! \internal */
void DockWindowPrivate::updatePanel(bool activateChanged)
{
    QTN_P(DockWindow);
    Q_UNUSED(activateChanged)
    if (m_rootPanel == Q_NULL)
        return;

    QList<DockPanelBase*> lstTabbed = m_rootPanel->findPanels(DockTabbedLayoutPanel::staticMetaObject.className());
    int count = lstTabbed.count();
    for (QList<DockPanelBase*>::iterator it = lstTabbed.begin(); it != lstTabbed.end(); ++it)
    {
        DockPanelBase* dock = (*it);
        Q_ASSERT(qtn_checkLayoutType(dock, DockLayoutPanel::Tabbed));
        if (DockTabbedLayoutPanel* tabbad = qobject_cast<DockTabbedLayoutPanel*>(dock))
            tabbad->setVisibleTitle(count != 1);
    }

    QList<DockPanelBase*> lstDocPanels = m_rootPanel->findPanels(DockDocumentLayoutPanel::staticMetaObject.className());
    if (lstDocPanels.count() == 0)
    {
        QList<DockPanelBase*> lstWidgetPanels = m_rootPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
        for (QList<DockPanelBase*>::iterator it = lstWidgetPanels.begin(); it != lstWidgetPanels.end(); ++it)
        {
            if (DockWidgetPanel* wp = qobject_cast<DockWidgetPanel*>(*it))
            {
                if (DockTitleBar* titleBar = qobject_cast<DockTitleBar*>(wp->titleBar()))
                    titleBar->setVisible(true);
            }
        }
    }

    if (TitleBarWrapper* wrapper = qobject_cast<TitleBarWrapper*>(m_internalLayout->widgetForRole(DockWindowLayout::TitleBar)))
        wrapper->setVisible(count > 1 || lstDocPanels.count() > 0);

    if (count > 0)
    {
        m_internalLayout->update();
        if (p.isHidden())
            QMetaObject::invokeMethod(&p, "setVisible", Qt::QueuedConnection, Q_ARG(bool, true));
    }
}

/*! \internal */
void DockWindowPrivate::updatePosition()
{
    QTN_P(DockWindow);
    QRect rc = p.geometry();
    if (m_assignRect.isValid())
        rc = m_assignRect;

    if (!dockManager()->isDockPanelFullContentsWhileDraggingShown())
    {
        QRect rectDragFrame;
        QList<DockPanelBase*> listPanel = m_rootPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
        for (QList<DockPanelBase*>::iterator it = listPanel.begin(); it != listPanel.end(); ++it)
        {
            DockWidgetPanel* dockPanel = (DockWidgetPanel*)(*it);
            rectDragFrame = DockWidgetPanelPrivate::_get(dockPanel)->dragFrameRect();
            if (rectDragFrame.isValid())
                DockWidgetPanelPrivate::_get(dockPanel)->setDragFrameRect(QRect(0, 0, 0, 0));
        }

        if (rectDragFrame.isValid())
        {
            rc = rectDragFrame;
            m_hasGeometrySet = true;
        }
    }

    QSize sz = rc.size();
    if (isThemed())
    {
        qtnMinMaxSize mmi;
        mmi.maxTrackSize.setWidth(QWIDGETSIZE_MAX);
        mmi.maxTrackSize.setHeight(QWIDGETSIZE_MAX);

        minMaxInfo(&mmi);
        sz.setWidth(qMax(qMin(sz.width(), mmi.maxTrackSize.width()), mmi.minTrackSize.width()));
        sz.setHeight(qMax(qMin(sz.height(), mmi.maxTrackSize.height()), mmi.minTrackSize.height()));
    }
    m_assignRect = QRect(0, 0, 0, 0);
    rc.setSize(sz);
    p.setGeometry(rc);
}

/*! \internal */
void DockWindowPrivate::updateButtons()
{
//    QTN_P(DockWindow);
//    const bool customTitleBar = m_internalLayout->widgetForRole(DockWindowLayout::TitleBar) != Q_NULL;
//    const bool nativeDeco = false;
//    const bool hideButtons = nativeDeco || customTitleBar;
//    const bool canClose = true;
//    const bool canFloat = false;
//    p.setAttribute(Qt::WA_ContentsPropagated, (canFloat || canClose) && !hideButtons);
    m_internalLayout->invalidate();
}

/*! \internal */
bool DockWindowPrivate::isThemed() const 
{ return DockPanelManagerPrivate::_get(dockManager())->isUseThemedFloatingFrames(); }

/*! \internal */
#ifdef Q_OS_WIN
bool DockWindowPrivate::checkSizeConstraint()
{
    QTN_P(DockWindow);
    QList<DockWidgetPanel*> lstPanel = p.findChildren<DockWidgetPanel*>();
    if (lstPanel.size() == 0)
        return false;

    const int sizeLayout = QLAYOUTSIZE_MAX;
    const int sizeWidget = QWIDGETSIZE_MAX;

    bool findLimitedWidget = false;
    foreach(DockWidgetPanel* panel, lstPanel)
    {
        QSize maxSize = panel->maximumSize();
        findLimitedWidget = !((maxSize.width() == sizeLayout || maxSize.width() == sizeWidget) && 
            (maxSize.height() == sizeLayout || maxSize.height() == sizeWidget));
        if (findLimitedWidget)
            break;
    }

    Qt::WindowFlags flags = p.windowFlags();
    if (findLimitedWidget)
        flags &= ~Qt::WindowMaximizeButtonHint;
    else
        flags |= Qt::WindowMaximizeButtonHint;

    if (flags != p.windowFlags())
    {
        p.setWindowFlags(flags | Qt::CustomizeWindowHint);
        return true;
    }
    return false;
}
#endif // Q_OS_WIN

/*! \internal */
void DockWindowPrivate::minMaxInfo(qtnMinMaxSize* minmaxSize) const
{ Q_UNUSED(minmaxSize); }

/*! \internal */
DockWidgetPanel* DockWindowPrivate::selectedPanel() const
{
    if (m_rootPanel == Q_NULL)
        return Q_NULL;
    QList<DockPanelBase*> lst = m_rootPanel->findPanels(DockTabbedLayoutPanel::staticMetaObject.className());
    int count = lst.count();
    if (count != 1)
        return Q_NULL;
    return qobject_cast<DockTabbedLayoutPanel*>(lst.at(0))->activePanel();
}

/*! \internal */
QWidget* DockWindowPrivate::titleBarWrapper() const
{
    return m_internalLayout->widgetForRole(DockWindowLayout::TitleBar);
}

/*! \internal */
void DockWindowPrivate::closeDockPanel()
{
    if (m_rootPanel != Q_NULL)
    {
        QList<DockPanelBase*> lstPanels;
        lstPanels << m_rootPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
        for (int i = 0, count = lstPanels.count(); count > i; i++)
        {
            DockWidgetPanel* panel = qobject_cast<DockWidgetPanel*>(lstPanels.at(i));
            if ((panel->features() & DockWidgetPanel::DockPanelClosable) == 0)
                continue;
            panel->closePanel();
        }
    }
}

/*!
\class DockWindow
\inmodule QtitanDocking
\brief Window that used to host the panel in floating (un-dock) state.
*/
DockWindow::DockWindow(DockLayoutEngine* layoutEngine)
    : DockWindowBase(layoutEngine->dockManager())
{
    Q_ASSERT(layoutEngine != Q_NULL);
#ifdef Q_OS_WIN
    setWindowFlags(Qt::Window);
#endif
#ifdef Q_OS_LINUX
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
#endif
#ifdef Q_OS_MAC
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
#endif
    setPalette(QApplication::palette("QDockWidget"));
    setBackgroundRole(QPalette::Button);
#ifdef Q_OS_MAC
    setAttribute(Qt::WA_MacAlwaysShowToolWindow);
    setProperty("_q_platform_MacUseNSWindow", QVariant(true));
#endif // Q_OS_MAC
    qtn_d_ptr = new DockWindowPrivate(layoutEngine);
    qtn_d_ptr->setPublic(this);
    qtn_d_ptr->initialization();

    WindowTitleBar* windowTitleBar = WindowTitleBar::get(this);
    windowTitleBar->setVisible(true);
}

DockWindow::~DockWindow()
{
    Q_ASSERT(qtn_d_ptr->m_rootPanel == Q_NULL);
    QTN_FINI_PRIVATE();
}

void DockWindow::initializationWindow(DockPanelBase* panelBase)
{
    QTN_D(DockWindow);
    DockSplitLayoutPanel* splitLayoutPanel = new DockSplitLayoutPanel(d.layoutEngine());
    if (DockDocumentPanel* documentPanel = qobject_cast<DockDocumentPanel*>(panelBase))
    {
        DockDocumentLayoutPanel* documentLayoutPanel = new DockDocumentLayoutPanel(d.layoutEngine());
        DockDocumentTabbedLayoutPanel* tabbedLayoutPanel = new DockDocumentTabbedLayoutPanel(d.layoutEngine());
        tabbedLayoutPanel->initialization(documentPanel);
        tabbedLayoutPanel->setParentPanel(documentLayoutPanel);
        panelBase = documentLayoutPanel;
    }
    else if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panelBase))
    {
        DockTabbedLayoutPanel* tabbedLayoutPanel = new DockTabbedLayoutPanel(d.layoutEngine());
        tabbedLayoutPanel->initialization(widgetPanel);
        panelBase = tabbedLayoutPanel;
    }

    d.m_rootPanel = splitLayoutPanel;
    splitLayoutPanel->setDockWindow(this);
    splitLayoutPanel->initialization(panelBase, true);
    d.m_internalLayout->setWidgetForRole(DockWindowLayout::Content, splitLayoutPanel);
}

void DockWindow::finalizationWindow()
{
    QTN_D(DockWindow);
    setVisible(false);
    if (d.m_rootPanel != Q_NULL && d.m_rootPanel->childCount() == 0)
    {
        Q_ASSERT(d.m_rootPanel->parentPanel() == Q_NULL);
        d.m_rootPanel->setDockWindow(Q_NULL);
        d.m_rootPanel = Q_NULL;
    }
}

DockPanelManager* DockWindow::dockManager() const
{
    QTN_D(const DockWindow);
    return d.dockManager();
}

DockLayoutPanel* DockWindow::layoutPanel() const
{
    QTN_D(const DockWindow);
    return d.m_rootPanel;
}

/*! \reimp */
bool DockWindow::event(QEvent* event)
{
    QTN_D(DockWindow);
#ifdef Q_OS_WIN
    if (event->type() == QEvent::Enter)
    {
        if (d.checkSizeConstraint())
        {
            QList<DockWidgetPanel*> lstPanel = findChildren<DockWidgetPanel*>();
            if (lstPanel.size() > 0)
                show();
        }
    }
    else
#endif // Q_OS_WIN
    if (event->type() == QEvent::WindowActivate)
    {
        DockLayoutEngine* layoutEngine = d.layoutEngine();
        Q_ASSERT(layoutEngine != Q_NULL);
        layoutEngine->stackUnderDockWindow(this);
    }
    return DockWindowBase::event(event);
}

/*! \reimp */
void DockWindow::moveEvent(QMoveEvent* event)
{
    Q_UNUSED(event);
    DockWidgetDragDrop* dragDropObject = DockPanelManagerPrivate::_get(dockManager())->dockingMasterDrag();
    if (!dragDropObject->isDragInitialized())
        return;
    dragDropObject->moveDrag(QCursor::pos(), Qt::NoModifier);
}


qreal DockAutoHideWindowPrivate::m_animationDelay = -1;
int DockAutoHideWindowPrivate::m_animationInterval = 16;
int DockAutoHideWindowPrivate::m_animationDuration = 128;
uint DockAutoHideWindowPrivate::m_inactiveCollapseDelay = 100;

/* DockAutoHideWindowPrivate */
DockAutoHideWindowPrivate::DockAutoHideWindowPrivate()
    : m_tabbedPanel(Q_NULL)
    , m_autoHidePanel(Q_NULL)
    , m_layoutArea(Qtitan::TopArea)
    , m_defaultCursor(Qt::ArrowCursor)
    , m_slideStep(0)
    , m_stepsCount(0)
    , m_deactivationCount(0)
    , m_slideOut(false)
    , m_horiz(false)
    , m_tracking(false)
{
}

DockAutoHideWindowPrivate::~DockAutoHideWindowPrivate()
{
}

/*! \internal */
void DockAutoHideWindowPrivate::init(DockAutoHideLayoutPanel* autoHidePanel, DockTabbedLayoutPanel* tabbedPanel)
{
    QTN_P(DockAutoHideWindow);
    m_tabbedPanel = tabbedPanel;
    m_autoHidePanel = autoHidePanel;
    p.setMouseTracking(true);
    m_layoutArea = autoHidePanel->layoutArea();
    m_defaultCursor = m_tabbedPanel->cursor();
    m_horiz = (m_layoutArea == Qtitan::TopArea || m_layoutArea == Qtitan::BottomArea);
    m_tabbedPanel->setDockWindow(&p);
    p.setVisible(true);
}

/* DockAutoHideWindow */
DockAutoHideWindow::DockAutoHideWindow(DockAutoHideLayoutPanel* autoHidePanel, DockTabbedLayoutPanel* tabbedPanel)
    : DockWindowBase(autoHidePanel->dockManager())
{
    QTN_INIT_PRIVATE(DockAutoHideWindow);
    QTN_D(DockAutoHideWindow);
    d.init(autoHidePanel, tabbedPanel);
    Q_ASSERT(tabbedPanel != Q_NULL);
    if (tabbedPanel == Q_NULL)
        return;

    Q_ASSERT(tabbedPanel->activePanel());
    if (!tabbedPanel->activePanel())
        return;

    updateLayout();

    d.m_slideStep = 0; d.m_stepsCount = 1;
    if (DockAutoHideWindowPrivate::m_animationDelay != 0.0 && DockAutoHideWindowPrivate::m_animationInterval != 0)
    {
        if (DockAutoHideWindowPrivate::m_animationDelay > 0)
            d.m_stepsCount = int((d.m_horiz ? m_rcWindow.width() : m_rcWindow.height()) * DockAutoHideWindowPrivate::m_animationDelay * 50 / DockAutoHideWindowPrivate::m_animationInterval);
        else
            d.m_stepsCount = DockAutoHideWindowPrivate::m_animationDuration / DockAutoHideWindowPrivate::m_animationInterval;
    }

    if (d.m_stepsCount < 1)
        d.m_stepsCount = 1;

    d.m_slideOut = false;
    if (d.m_stepsCount > 1)
    {
        d.m_timerSlidein.setInterval(DockAutoHideWindowPrivate::m_animationInterval);
        connect(&d.m_timerSlidein, SIGNAL(timeout()), this, SLOT(timerSlidein()));
        d.m_timerSlidein.start();
    }

    slideStep();
    d.m_tabbedPanel->setCursor(d.m_defaultCursor);
    recalcLayout();

    connect(&d.m_timerSlideout, SIGNAL(timeout()), this, SLOT(timerSlideout()));
    connect(&d.m_closeTimer, SIGNAL(timeout()), this, SLOT(closeTimerTimeout()));

    d.m_timerCheckActive.setInterval(static_cast<int>(DockAutoHideWindowPrivate::m_inactiveCollapseDelay));
    connect(&d.m_timerCheckActive, SIGNAL(timeout()), this, SLOT(timerCheckActive()));
    d.m_timerCheckActive.start();
    d.m_deactivationCount = 6;
}

DockAutoHideWindow::~DockAutoHideWindow()
{
    QTN_FINI_PRIVATE();
}

DockPanelManager* DockAutoHideWindow::dockManager() const
{
    QTN_D(const DockAutoHideWindow);
    Q_ASSERT(d.m_autoHidePanel);
    return d.m_autoHidePanel ? d.m_autoHidePanel->dockManager() : Q_NULL;
}

void DockAutoHideWindow::updateLayout()
{
    QTN_D(DockAutoHideWindow);
    const int splitterWidth = style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, this);

    DockPanelManager* manager = dockManager();
    DockPanelBase* rootLayoutPanel = manager->layoutPanel();

    QRect rcClient = rootLayoutPanel->geometry();

    qtnMinMaxSize mmInfo;
    calcMinMaxInfo(mmInfo);

    QRect rcAvail, rc;
    bool resizable = allowedRect(rcAvail, rc);

    QSize size = DockPanelBasePrivate::_get(d.m_tabbedPanel)->m_sizeDocking + QSize(splitterWidth, splitterWidth);

    switch (DockPanelManagerPrivate::_get(manager)->rtlPosition(d.m_layoutArea))
    {
        case Qtitan::LeftArea:
            size.setWidth(qMin(size.width(), rcAvail.right() + splitterWidth - rcClient.left()));
            break;
        case Qtitan::RightArea:
            size.setWidth(qMin(size.width(), rcClient.right() - rcAvail.left()));
            break;
        case Qtitan::TopArea:
            size.setHeight(qMin(size.height(), rcAvail.bottom() + splitterWidth - rcClient.top()));
            break;
        case Qtitan::BottomArea:
            size.setHeight(qMin(size.height(), rcClient.bottom() - rcAvail.top()));
            break;
        default:
            break;
    }

    size.setWidth(qMax(mmInfo.minTrackSize.width(), qMin(mmInfo.maxTrackSize.width(), size.width())));
    size.setHeight(qMax(mmInfo.minTrackSize.height(), qMin(mmInfo.maxTrackSize.height(), size.height())));

    switch (DockPanelManagerPrivate::_get(manager)->rtlPosition(d.m_layoutArea))
    {
        case Qtitan::LeftArea:
            rcClient.setRight(rcClient.left() + size.width());
            break;

        case Qtitan::RightArea:
            rcClient.setLeft(rcClient.right() - size.width());
            break;

        case Qtitan::TopArea:
            rcClient.setBottom(rcClient.top() + size.height());
            break;

        case Qtitan::BottomArea:
            rcClient.setTop(rcClient.bottom() - size.height());
            break;
        default:
            break; 
    }

    QCursor cursor(!resizable || DockPanelManagerPrivate::_get(manager)->isSplittersLocked() ? 
        Qt::ArrowCursor : !d.m_horiz ? Qt::SplitHCursor : Qt::SplitVCursor);
    setCursor(cursor);

    m_rcWindow = rcClient;
    setGeometry(m_rcWindow);
}

void DockAutoHideWindow::calcMinMaxInfo(qtnMinMaxSize& minmaxSize, bool includeSplitter /*= true*/) const
{
    QTN_D(const DockAutoHideWindow);
    minmaxSize.maxTrackSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    if (!d.m_tabbedPanel || !d.m_tabbedPanel->activePanel())
        return;

    DockPanelBasePrivate::_get(d.m_tabbedPanel->activePanel())->getMinMaxData(minmaxSize);
    d.m_tabbedPanel->adjustMinMaxClientSize(minmaxSize, true);

    if (!includeSplitter)
        return;

    const int splitterWidth = style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, this);
    if (d.m_layoutArea == Qtitan::LeftArea || d.m_layoutArea == Qtitan::RightArea)
    {
        minmaxSize.minTrackSize.setWidth(minmaxSize.minTrackSize.width() + splitterWidth);
        minmaxSize.maxTrackSize.setWidth(minmaxSize.maxTrackSize.width() + splitterWidth);
    }
    else
    {
        minmaxSize.minTrackSize.setHeight(minmaxSize.minTrackSize.height() + splitterWidth);
        minmaxSize.maxTrackSize.setHeight(minmaxSize.maxTrackSize.height() + splitterWidth);
    }
}

void DockAutoHideWindow::slideStep()
{
    QTN_D(DockAutoHideWindow);
    QRect rc(m_rcWindow);
    switch (d.m_layoutArea)
    {
        case Qtitan::LeftArea:
            rc.setRight(rc.left() + (d.m_slideStep + 1) * (m_rcWindow.right() - m_rcWindow.left()) / d.m_stepsCount);
            break;

        case Qtitan::RightArea:
            rc.setLeft(rc.right() - (d.m_slideStep + 1) * (m_rcWindow.right() - m_rcWindow.left()) / d.m_stepsCount);
            break;

        case Qtitan::TopArea:
            rc.setBottom(rc.top() + (d.m_slideStep + 1) * (m_rcWindow.bottom() - m_rcWindow.top()) / d.m_stepsCount);
            break;

        case Qtitan::BottomArea:
            rc.setTop(rc.bottom() - (d.m_slideStep + 1) * (m_rcWindow.bottom() - m_rcWindow.top()) / d.m_stepsCount);
            break;
        default:
            break;
    }
    setGeometry(rc);
}

void DockAutoHideWindow::recalcLayout()
{
    QTN_D(DockAutoHideWindow);
    if (!d.m_tabbedPanel)
        return;

    QRect rcClient(rect());
    QRect rcPane(QPoint(0, 0), m_rcWindow.size());
    const int splitterWidth = style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, this);
    switch (d.m_layoutArea)
    {
        case Qtitan::LeftArea:
            rcPane.translate(-m_rcWindow.width() + rcClient.width(), 0);
            rcPane.setRight(rcPane.right() - splitterWidth);
            break;

        case Qtitan::TopArea:
            rcPane.translate(0, -m_rcWindow.height() + rcClient.height());
            rcPane.setBottom(rcPane.bottom() - splitterWidth);
            break;

        case Qtitan::RightArea:
            rcPane.setLeft(rcPane.left() + splitterWidth);
            break;

        case Qtitan::BottomArea:
            rcPane.setTop(rcPane.top() + splitterWidth);
            break;
        default:
            break;
    }
    DockPanelBasePrivate::_get(d.m_tabbedPanel)->setPanelGeometry(rcPane);
}

void DockAutoHideWindow::closeWindow()
{
    QTN_D(DockAutoHideWindow);
    setVisible(false);
    if (d.m_tabbedPanel && d.m_tabbedPanel->parent() == this)
        d.m_autoHidePanel->setCollapseWindow(d.m_tabbedPanel);

    d.m_tabbedPanel = Q_NULL;

    if (d.m_autoHidePanel && d.m_autoHidePanel->activeWindow() == this)
        d.m_autoHidePanel->setActiveWindow(Q_NULL);
    deleteLater();
}

void DockAutoHideWindow::startCloseTimer()
{
    QTN_D(DockAutoHideWindow);
    d.m_closeTimer.setInterval(50);
    d.m_closeTimer.start();
}

void DockAutoHideWindow::clearWidgetTabbed()
{
    QTN_D(DockAutoHideWindow);
    d.m_tabbedPanel = Q_NULL;
}

DockTabbedLayoutPanel* DockAutoHideWindow::widgetTabbed() const
{
    QTN_D(const DockAutoHideWindow);
    return d.m_tabbedPanel;
}

DockAutoHideLayoutPanel* DockAutoHideWindow::widgetAutoHide() const
{
    QTN_D(const DockAutoHideWindow);
    return d.m_autoHidePanel;
}

/*! \reimp */
void DockAutoHideWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(DockAutoHideWindow);
    if (d.m_autoHidePanel)
    {
        QPainter p(this);
        AutoHideWindowStyleOption opt;
        opt.initFrom(this);
        opt.position = d.m_layoutArea;
        style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(CommonStyle::PE_DockAutoHideFrameWindow), &opt, &p, this);
    }
}

/*! \reimp */
void DockAutoHideWindow::mousePressEvent(QMouseEvent* event)
{
    QTN_D(DockAutoHideWindow);
    if (event->button() == Qt::LeftButton)
    {
        if (d.m_tabbedPanel == Q_NULL)
            return;

        if (childAt(event->pos()) != Q_NULL)
            return;

        DockPanelManager* manager = dockManager();
        if (!manager || DockPanelManagerPrivate::_get(manager)->isSplittersLocked())
            return;

        d.m_rc = d.m_rcClient = d.m_rcCatch = QRect();
        d.m_tracking = true;

        if (DockPanelManagerPrivate::_get(manager)->isSplitterTrackerUsed())
        {
        }
        else
        {
            grabMouse();
        }
    }
    DockWindowBase::mousePressEvent(event);
}

/*! \reimp */
void DockAutoHideWindow::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(DockAutoHideWindow);
    if (d.m_slideOut)
    {
        timerCheckActive();
        return;
    }

    DockPanelManager* manager = dockManager();
    if (!manager || DockPanelManagerPrivate::_get(manager)->isSplittersLocked())
        return;

    if (d.m_tracking && !DockPanelManagerPrivate::_get(manager)->isSplitterTrackerUsed())
    {
        if (d.m_rc.isEmpty() || d.m_rcClient.isEmpty() || d.m_rcCatch.isEmpty())
        {
            d.m_rc = rect();
            d.m_rc.moveTo(mapToGlobal(d.m_rc.topLeft()));
            d.m_rcClient = manager->layoutPanel()->geometry();
            d.m_rcClient.moveTo(manager->layoutPanel()->parentWidget()->mapToGlobal(d.m_rcClient.topLeft()));
            if (!allowedRect(d.m_rcCatch, d.m_rc))
                return;
        }

        QPoint point = event->globalPos();
        point.setX(qMax(qMin(point.x(), d.m_rcCatch.right()), d.m_rcCatch.left()));
        point.setY(qMax(qMin(point.y(), d.m_rcCatch.bottom()), d.m_rcCatch.top()));

        if (!d.m_horiz)
        {
            if (d.m_rc.left() == point.x())
                return;
            d.m_rc.translate(point.x() - d.m_rc.left(), 0);
        }
        else
        {
            if (d.m_rc.top() == point.y())
                return;
            d.m_rc.translate(0, point.y() - d.m_rc.top());
        }
        reposition(d.m_rc, d.m_rcClient);
    }
}

/*! \reimp */
void DockAutoHideWindow::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(DockAutoHideWindow);
    if (d.m_tracking)
    {
        d.m_tracking = false;
        QWidget* grWidget = DockWindowBase::mouseGrabber();
        if (grWidget == this)
            releaseMouse();
    }
    DockWindowBase::mouseReleaseEvent(event);
}

/*! \reimp */
void DockAutoHideWindow::resizeEvent(QResizeEvent* event)
{
    DockWindowBase::resizeEvent(event);
    recalcLayout();
}

void DockAutoHideWindow::reposition(QRect rc, QRect rcClient)
{
    QTN_D(DockAutoHideWindow);
    switch (DockPanelManagerPrivate::_get(dockManager())->rtlPosition(d.m_layoutArea))
    {
        case Qtitan::LeftArea:
            rc.setLeft(rcClient.left());
            break;
        case Qtitan::RightArea:
            rc.setRight(rcClient.right());
            break;
        case Qtitan::TopArea:
            rc.setTop(rcClient.top());
            break;
        case Qtitan::BottomArea:
            rc.setBottom(rcClient.bottom());
            break;
        default:
            break;
    }

    if (!d.m_horiz)
        DockPanelBasePrivate::_get(d.m_tabbedPanel->activePanel())->m_sizeDocking.setWidth(rc.width());
    else
        DockPanelBasePrivate::_get(d.m_tabbedPanel->activePanel())->m_sizeDocking.setHeight(rc.height());

    QPoint pnt = parentWidget() ? parentWidget()->mapFromGlobal(rc.topLeft()) : 
        dockManager()->managedWidget()->mapFromGlobal(rc.topLeft());
    rc.moveTo(pnt);
    m_rcWindow = rc;

    setGeometry(m_rcWindow);
}

bool DockAutoHideWindow::allowedRect(QRect& rcAvail, QRect& rc) const
{
    QTN_D(const DockAutoHideWindow);
    DockPanelManager* manager = dockManager();
    if (!manager)
        return false;

    QRect rcClient = manager->layoutPanel()->geometry();

    if (d.m_tracking)
        rcClient.moveTo(manager->layoutPanel()->parentWidget()->mapToGlobal(rcClient.topLeft()));

    const int splitterWidth = style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, this);
    const int spacing = DockPanelManagerPrivate::_get(manager)->m_splitterGap + splitterWidth;

    rcAvail = rcClient;

    qtnMinMaxSize mmInfo;
    calcMinMaxInfo(mmInfo, d.m_layoutArea == Qtitan::RightArea || d.m_layoutArea == Qtitan::BottomArea);

    switch (d.m_layoutArea)
    {
        case Qtitan::LeftArea:
            rcAvail.adjust(qMax(mmInfo.minTrackSize.width(), spacing), 0, -spacing, 0);

            if (rcAvail.right() - rcClient.left() > mmInfo.maxTrackSize.width())
                rcAvail.setRight(mmInfo.maxTrackSize.width() + rcClient.left());

            if (rcAvail.left() >= rcAvail.right())
                return false;

            rc.setLeft(rc.right() - splitterWidth);
            break;

        case Qtitan::RightArea:
            rcAvail.adjust(spacing, 0, -qMax(mmInfo.minTrackSize.width(), spacing), 0);

            if (rcClient.right() - rcAvail.left() > mmInfo.maxTrackSize.width())
                rcAvail.setLeft(rcClient.right() - mmInfo.maxTrackSize.width());

            if (rcAvail.left() >= rcAvail.right())
                return false;

            rc.setRight(rc.left() + splitterWidth);
            break;

        case Qtitan::TopArea:
            rcAvail.adjust(0, qMax(mmInfo.minTrackSize.height(), spacing), 0, spacing);

            if (rcAvail.bottom() - rcClient.top() > mmInfo.maxTrackSize.height())
                rcAvail.setBottom(mmInfo.maxTrackSize.height() + rcClient.top());

            if (rcAvail.top() >= rcAvail.bottom())
                return false;

            rc.setTop(rc.bottom() - splitterWidth);
            break;

        case Qtitan::BottomArea:
            rcAvail.adjust(0, spacing, 0, -qMax(mmInfo.minTrackSize.height(), spacing));

            if (rcClient.bottom() - rcAvail.top() > mmInfo.maxTrackSize.height())
                rcAvail.setTop(rcClient.bottom() - mmInfo.maxTrackSize.height());

            if (rcAvail.top() >= rcAvail.bottom())
                return false;

            rc.setBottom(rc.top() + splitterWidth);
            break;
        default:
            break;
    }
    return true;
}

bool DockAutoHideWindow::isSlideOut() const
{
    QTN_D(const DockAutoHideWindow);
    return d.m_slideOut;
}

void DockAutoHideWindow::closeTimerTimeout()
{
    closeWindow();
}

void DockAutoHideWindow::timerSlidein()
{
    QTN_D(DockAutoHideWindow);
    if (!d.m_tabbedPanel)
        return;

    if (d.m_autoHidePanel && d.m_autoHidePanel->activeWindow() != this)
        return;

    if (!d.m_slideOut)
    {
        d.m_slideStep++;
        if (d.m_slideStep < d.m_stepsCount)
            slideStep();
        else
        {
            d.m_slideStep = d.m_stepsCount - 1;
            d.m_timerSlidein.stop();
        }
    }
}

void DockAutoHideWindow::timerSlideout()
{
    QTN_D(DockAutoHideWindow);
    if (!d.m_tabbedPanel)
        return;

    if (d.m_autoHidePanel && d.m_autoHidePanel->activeWindow() != this)
        return;

    if (d.m_slideOut)
    {
        d.m_slideStep--;
        if (d.m_slideStep > -1)
            slideStep();
        else
        {
            d.m_slideOut = false;
            d.m_timerSlideout.stop();
            closeWindow();
        }
        return;
    }
}

void DockAutoHideWindow::timerCheckActive()
{
    QTN_D(DockAutoHideWindow);
    if (!d.m_tabbedPanel)
        return;

    if (d.m_autoHidePanel && d.m_autoHidePanel->activeWindow() != this)
        return;

    if (d.m_tracking)
        return;

    QPoint pt(QCursor::pos());
    QRect rect = geometry();
    QRect rectDock = d.m_autoHidePanel->geometry();
    if (!d.m_tabbedPanel->isActive() && !rect.contains(pt) && !rectDock.contains(pt))
    {
        if (!d.m_slideOut)
        {
            if (--d.m_deactivationCount <= 0)
            {
                d.m_deactivationCount = 0;
                if (!DockAutoHideLayoutPanel::isCloseActiveWindow())
                {
                    d.m_deactivationCount = 6;
                }
                else
                {
                    d.m_slideOut = true;
                    d.m_timerSlideout.setInterval(DockAutoHideWindowPrivate::m_animationInterval);
                    d.m_timerSlideout.start();
                }
            }
        }
    }
    else
    {
        d.m_deactivationCount = 6;
        if (d.m_slideOut)
        {
            d.m_slideOut = false;
            d.m_timerSlideout.stop();
            d.m_timerSlideout.setInterval(DockAutoHideWindowPrivate::m_animationInterval);
            d.m_timerSlidein.start();
        }
    }
}
