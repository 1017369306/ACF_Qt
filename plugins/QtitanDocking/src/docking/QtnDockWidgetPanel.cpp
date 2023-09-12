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
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <qevent.h>

#include "QtnPlatform.h"
#include "QtnDockWindow.h"
#include "QtnDockTitleBar.h"
#include "QtnDockTitleBarPrivate.h"
#include "QtnCommonStyle.h"
#include "QtnDockLayoutPrivate.h"
#include "QtnDockWindowPrivate.h"
#include "QtnDockWidgetDragDrop.h"
#include "QtnDockPanelManagerPrivate.h"
#include "QtnDockWidgetPanelPrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

static inline int pick(bool vertical, const QSize& size) { return vertical ? size.height() : size.width(); }
static inline int perp(bool vertical, const QSize& size) { return vertical ? size.width() : size.height(); }


/* TitleBarWrapper */
TitleBarWrapper::TitleBarWrapper(DockPanelManager* panelManager, QWidget* parent)
    : QWidget(parent)
    , m_titleBar(Q_NULL)
    , m_panelManager(panelManager)
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

TitleBarWrapper::~TitleBarWrapper()
{
    Q_DELETE_AND_NULL(m_titleBar);
}

void TitleBarWrapper::setTitleBar(QWidget* titleBar) 
{ 
    if (m_titleBar)
    {
        m_titleBar->removeEventFilter(this);
        m_titleBar->hide();
    }

    m_titleBar = titleBar; 

    if (m_titleBar)
    {
        m_titleBar->setParent(this);
        m_titleBar->installEventFilter(this);
        QRect rectTemp = QRect(QPoint(0, 0), QSize(width(), height()));
        m_titleBar->setGeometry(rectTemp);
        m_titleBar->show();
    }
}

#ifdef Q_OS_LINUX //See QtnPlatform_linux : qtn_X11EmulateButtonRelease(w, pos);
bool TitleBarWrapper::isNativeMouseButtons_linux()
{
    Qt::MouseButtons buttons = qtn_get_mouseButtons_linux();
    return (buttons & Qt::LeftButton);
}
#endif

QSize TitleBarWrapper::minimumSizeHint() const
{
    if (m_titleBar != Q_NULL && !isHidden())
    {
        QSize sizeTemp = m_titleBar->minimumSizeHint();
        return sizeTemp;
    }
//        return m_titleBar->minimumSizeHint();
    return QSize(0, 0);
}

/*! \reimp */
bool TitleBarWrapper::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != m_titleBar || m_titleBar == Q_NULL || isHidden())
        return QWidget::eventFilter(watched, event);

    if (event->type() == QEvent::MouseButtonPress)
        return handleMousePressEvent(static_cast<QMouseEvent*>(event));
#if 0
    else if (event->type() == QEvent::MouseMove)
        return handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
    else if (event->type() == QEvent::MouseButtonRelease)
        return handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
#endif
    else if (event->type() == QEvent::MouseButtonDblClick)
        return handleMouseDoubleClickEvent(static_cast<QMouseEvent*>(event));
    return QWidget::eventFilter(watched, event);
}

/*! \reimp */
bool TitleBarWrapper::handleMousePressEvent(QMouseEvent* event)
{
    DockWidgetDragDrop* dragDropObject = DockPanelManagerPrivate::_get(m_panelManager)->dockingMasterDrag();

    QWidget* parentWidget = this->parentWidget();
    if (parentWidget == Q_NULL || m_titleBar == Q_NULL || event->button() != Qt::LeftButton)
        return false;

    DockTabbedLayoutPanel* tabbedLayoutPanel = qobject_cast<DockTabbedLayoutPanel*>(parentWidget->parentWidget());

    if (tabbedLayoutPanel != Q_NULL)
        DockTabbedLayoutPanelPrivate::_get(tabbedLayoutPanel)->restoreFocus();

    DockWindow* dockWindow = qobject_cast<DockWindow*>(parentWidget->topLevelWidget());
    if (dockWindow && (DockWindowPrivate::_get(dockWindow)->titleBarWrapper()->isHidden() || 
        DockWindowPrivate::_get(dockWindow)->titleBarWrapper() == this))
    {
        dragDropObject->startDrag();
        dragDropObject->execDrag(dockWindow->layoutPanel(), event->pos());
        if (dragDropObject->isStartDrag())
            dragDropObject->acceptDrag();
        dragDropObject->stopDrag();
        return true;
    }

    DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(parentWidget);
    if (widgetPanel == Q_NULL)
        return false;
#if 0
    if (widgetPanel != Q_NULL && !widgetPanel->features().testFlag(DockWidgetPanel::DockPanelFloatable))
        return false;
#endif
    if (tabbedLayoutPanel != Q_NULL)
    {
        const QList<DockPanelBase*>& lstPanel = tabbedLayoutPanel->panels();
        for (QList<DockPanelBase*>::const_iterator it = lstPanel.begin(); it != lstPanel.end(); ++it)
        {
            DockWidgetPanel* panel = qobject_cast<DockWidgetPanel*>(*it);
            if (panel != Q_NULL && !panel->features().testFlag(DockWidgetPanel::DockPanelFloatable))
            {
                return false;
            }
        }
    }

    DockWidgetPanelPrivate* p = DockWidgetPanelPrivate::_get(widgetPanel);
    p->mousePress(mapTo(widgetPanel, event->pos()), tabbedLayoutPanel);
    return true;
}

/*! \reimp */
bool TitleBarWrapper::handleMouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return false;

    if (DockTabbedLayoutPanel* tabbedLayoutPanel = qobject_cast<DockTabbedLayoutPanel*>(parentWidget()->parentWidget()))
    {
        if (!isHidden())
        {
            if (DockWidgetPanel* selectedPanel = tabbedLayoutPanel->activePanel())
            {
                if (DockTabbedLayoutPanelPrivate::_get(tabbedLayoutPanel)->isAutoHide() && ((selectedPanel->features() & DockWidgetPanel::DockPanelHideable) == 0))
                    return false;
                if (DockTabbedLayoutPanelPrivate::_get(tabbedLayoutPanel)->isAutoHide() && ((selectedPanel->features() & DockWidgetPanel::DockPanelFloatableByCaptionDoubleClick) == 0))
                    return false;
                DockPanelManagerPrivate::_get(m_panelManager)->switchDocking(tabbedLayoutPanel);
                return true;
            }
        }
    }
    return false;
}

/*! \reimp */
void TitleBarWrapper::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_titleBar != Q_NULL)
    {
        double widthTemp = width();
        m_titleBar->setGeometry(QRect(QPoint(0, 0), QSize(width(), height())));
    }
}


/* DockWidgetPanelLayout */
DockWidgetPanelLayout::DockWidgetPanelLayout(QWidget* parent)
    : QLayout(parent)
    , m_itemList(RoleCount, Q_NULL)
    , m_verticalTitleBar(false)
{
}

DockWidgetPanelLayout::~DockWidgetPanelLayout()
{
    qDeleteAll(m_itemList);
}

void DockWidgetPanelLayout::addItem(QLayoutItem*)
{
    qWarning("DockWidgetPanelLayout::addItem(): please use DockWidgetPanelLayout::setWidget()");
    return;
}

/*! \internal */
QLayoutItem* DockWidgetPanelLayout::itemAt(int index) const
{
    int cnt = 0;
    for (int i = 0; i < m_itemList.count(); ++i) 
    {
        QLayoutItem *item = m_itemList.at(i);
        if (item == Q_NULL)
            continue;
        if (index == cnt++)
            return item;
    }
    return Q_NULL;
}

/*! \internal */
QLayoutItem* DockWidgetPanelLayout::takeAt(int index)
{
    int j = 0;
    for (int i = 0; i < m_itemList.count(); ++i) 
    {
        QLayoutItem *item = m_itemList.at(i);
        if (item == Q_NULL)
            continue;
        if (index == j) 
        {
            m_itemList[i] = Q_NULL;
            invalidate();
            return item;
        }
        ++j;
    }
    return Q_NULL;
}

/*! \internal */
int DockWidgetPanelLayout::count() const
{
    int result = 0;
    for (int i = 0; i < m_itemList.count(); ++i) 
    {
        if (m_itemList.at(i))
            ++result;
    }
    return result;
}

/*! \internal */
QSize DockWidgetPanelLayout::sizeFromContent(const QSize& content) const
{
    QSize result = content;
    if (m_verticalTitleBar)
    {
        result.setHeight(qMax(result.height(), minimumTitleWidth()));
        result.setWidth(qMax(content.width(), 0));
    }
    else
    {
        result.setHeight(qMax(result.height(), 0));
        result.setWidth(qMax(content.width(), minimumTitleWidth()));
    }

    DockWidgetPanel* w = qobject_cast<DockWidgetPanel*>(parentWidget());
    const int fw = 0;
    const int th = titleHeight();

    if (m_verticalTitleBar)
        result += QSize(th + 2 * fw, 2 * fw);
    else
        result += QSize(2 * fw, th + 2 * fw);

    result.setHeight(qMin(result.height(), static_cast<int>(QWIDGETSIZE_MAX)));
    result.setWidth(qMin(result.width(), static_cast<int>(QWIDGETSIZE_MAX)));

    if (content.width() < 0)
        result.setWidth(-1);
    if (content.height() < 0)
        result.setHeight(-1);

    QMargins margins = w->contentsMargins();
    // we need to subtract the contents margin (it will be added by the caller)
    QSize min = w->minimumSize() - QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    QSize max = w->maximumSize() - QSize(margins.left() + margins.right(), margins.top() + margins.bottom());

    uint explicitMin = 0;
    uint explicitMax = 0;
    explicitMin = (w->minimumWidth() ? Qt::Horizontal : 0) | (w->minimumHeight() ? Qt::Vertical : 0);
    explicitMax = (w->maximumWidth() != QWIDGETSIZE_MAX ? Qt::Horizontal : 0) | (w->maximumHeight() != QWIDGETSIZE_MAX ? Qt::Vertical : 0);

    if (!(explicitMin & Qt::Horizontal) || min.width() == 0)
        min.setWidth(-1);
    if (!(explicitMin & Qt::Vertical) || min.height() == 0)
        min.setHeight(-1);

    if (!(explicitMax & Qt::Horizontal))
        max.setWidth(QWIDGETSIZE_MAX);
    if (!(explicitMax & Qt::Vertical))
        max.setHeight(QWIDGETSIZE_MAX);

    return result.boundedTo(max).expandedTo(min);
}

/*! \internal */
QSize DockWidgetPanelLayout::sizeHint() const
{
    QSize content(-1, -1);
    if (m_itemList[Content] != Q_NULL)
        content = m_itemList[Content]->sizeHint();
    return sizeFromContent(content);
}

/*! \internal */
QSize DockWidgetPanelLayout::maximumSize() const
{
    if (m_itemList[Content] != Q_NULL)
    {
        const QSize content = m_itemList[Content]->maximumSize();
        return sizeFromContent(content);
    } 
    return parentWidget()->maximumSize();
}

/*! \internal */
QSize DockWidgetPanelLayout::minimumSize() const
{
    QSize content(0, 0);
    if (m_itemList[Content] != Q_NULL)
        content = m_itemList[Content]->minimumSize();

    return sizeFromContent(content);
}

/*! \internal */
QWidget* DockWidgetPanelLayout::widgetForRole(Role r) const
{
    QLayoutItem *item = m_itemList.at(r);
    return item == Q_NULL ? Q_NULL : item->widget();
}

/*! \internal */
QLayoutItem* DockWidgetPanelLayout::itemForRole(Role r) const
{
    return m_itemList.at(r);
}

/*! \internal */
void DockWidgetPanelLayout::setWidgetForRole(Role r, QWidget* w)
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

/*! \internal */
int DockWidgetPanelLayout::titleHeight() const
{
    if (QWidget* title = widgetForRole(TitleBar))
        return perp(m_verticalTitleBar, title->sizeHint());
    return 0;
}

/*! \internal */
int DockWidgetPanelLayout::minimumTitleWidth() const
{
    if (QWidget* title = widgetForRole(TitleBar))
        return pick(m_verticalTitleBar, title->minimumSizeHint());
    return 0;
}

/*! \internal */
void DockWidgetPanelLayout::setGeometry(const QRect& geometry)
{
    DockWidgetPanel* w = qobject_cast<DockWidgetPanel*>(parentWidget());
    if (w == Q_NULL)
        return;
    const int fw = w->style()->pixelMetric(QStyle::PM_DockWidgetFrameWidth, Q_NULL, w);
    int titleHeight = this->titleHeight();
    QRect titleArea;
    if (m_verticalTitleBar) 
        titleArea = QRect(QPoint(fw, fw), QSize(titleHeight, geometry.height() - (fw * 2)));
    else
        titleArea = QRect(QPoint(fw, fw), QSize(geometry.width() - (fw * 2), titleHeight));

    if (QLayoutItem* item = m_itemList[TitleBar])
        item->setGeometry(titleArea);

    if (QLayoutItem* item = m_itemList[Content]) 
    {
        QRect r = geometry;
        if (m_verticalTitleBar) 
        {
            r.adjust(titleArea.width(), 0, 0, 0);
            r.adjust(0, fw, -fw, -fw);
        }
        else 
        {
            r.adjust(0, titleArea.height(), 0, 0);
            r.adjust(fw, fw, -fw, -fw);
        }
        item->setGeometry(r);
    }
}


/* DockWidgetPanelPrivate */
DockWidgetPanelPrivate::DockWidgetPanelPrivate(DockPanelManager* manager)
    : DockPanelBasePrivate(DockPanelManagerPrivate::_get(manager)->dockLayoutEngine())
    , m_actAutoHide(Q_NULL)
    , m_actFloat(Q_NULL)
    , m_actDock(Q_NULL)
    , m_actHide(Q_NULL)
    , m_actVisible(Q_NULL)
    , m_menuButton(Q_NULL)
    , m_features(DockWidgetPanel::DockPanelClosable | DockWidgetPanel::DockPanelHideable |
        DockWidgetPanel::DockPanelFloatable | DockWidgetPanel::DockPanelMenuButton |
        DockWidgetPanel::DockPanelDockable | DockWidgetPanel::DockPanelFloatableByCaptionDoubleClick)
    , m_allowedAreas(Qtitan::AllDockPanelAreas)
    , m_dragFrameRect()
    , m_pressedPos(cursorPositionInvalid)
    , m_dragPanel(Q_NULL)
{
}

DockWidgetPanelPrivate::~DockWidgetPanelPrivate()
{
}

/*! \internal */
void DockWidgetPanelPrivate::init()
{
    QTN_P(DockWidgetPanel);

    DockPanelBasePrivate::init();
    p.setFocusPolicy(Qt::StrongFocus);

    DockWidgetPanelLayout* layout = new DockWidgetPanelLayout(&p);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout->setWidgetForRole(DockWidgetPanelLayout::TitleBar, new TitleBarWrapper(dockManager(), &p));

    m_frameStyle = QFrame::StyledPanel | QFrame::Plain;

    m_actMenuButton = new QAction(QObject::tr("Window Position"), &p);
    m_actMenuButton->setProperty(_qtn_Type_Button, MenuButton);
    m_menuButton = new QMenu(&p);
    m_actMenuButton->setMenu(m_menuButton);
    
    m_actAutoButton = new QAction(QObject::tr("Auto Hide"), &p);
    m_actAutoButton->setProperty(_qtn_Type_Button, AutoHideButton);

    m_actFloat = new QAction(QObject::tr("&Float"), &p);
    QObject::connect(m_actFloat, SIGNAL(triggered(bool)), this, SLOT(floatingChanged(bool)));

    m_actDock = new QAction(QObject::tr("Doc&k"), &p);
    QObject::connect(m_actDock, SIGNAL(triggered(bool)), this, SLOT(dockableChanged(bool)));

    m_actAutoHide = new QAction(QObject::tr("&Auto Hide"), &p);
    QObject::connect(m_actAutoHide, SIGNAL(triggered(bool)), this, SLOT(autoHideChanged(bool)));

    m_actHide = new QAction(QObject::tr("&Hide"), &p);
    QObject::connect(m_actHide, SIGNAL(triggered(bool)), this, SLOT(hideChanged(bool)));

    m_actCloseButton = new QAction(QObject::tr("Close"), &p);
    m_actCloseButton->setProperty(_qtn_Type_Button, CloseButton);
    
    m_actVisible = new QAction(p.caption(), &p);
    m_actVisible->setMenuRole(QAction::NoRole);
    m_actVisible->setCheckable(true);
    m_actVisible->setChecked(!p.isClosed());
    QObject::connect(m_actVisible, SIGNAL(triggered(bool)), this, SLOT(visibleChanged(bool)));

    m_menuButton->addAction(m_actFloat);
    m_menuButton->addAction(m_actDock);
    m_menuButton->addAction(m_actAutoHide);
    m_menuButton->addAction(m_actHide);
    connect(m_menuButton, SIGNAL(aboutToShow()), this, SLOT(updateActions()));

    DockTitleBar* titleBar = new DockTitleBar(&p);
    p.setTitleBar(titleBar);
    addActionToTitleBar();
    titleBar->updateFeatures(m_features);
}

void DockWidgetPanelPrivate::setPanelId(const QString& id)
{
    QTN_P(DockWidgetPanel);
    QString oldId = p.id();
    if (layoutEngine()->setPanelId(&p, oldId, id))
        DockPanelBasePrivate::setPanelId(id);
}

void DockWidgetPanelPrivate::addActionToTitleBar()
{
    QTN_P(DockWidgetPanel);
    if (DockTitleBar* titleBar = qobject_cast<DockTitleBar*>(p.titleBar()))
    {
        titleBar->addAction(m_actMenuButton);
        titleBar->addAction(m_actAutoButton);
        titleBar->addAction(m_actCloseButton);
    }
}

/*! \internal */
QWidget* DockWidgetPanelPrivate::lastOwner(bool floating)
{
    return floating ? panelInfo().floatingOwner : panelInfo().dockingOwner;
}

void DockWidgetPanelPrivate::mousePress(const QPoint& pos, DockPanelBase* dragPanel)
{
    QTN_P(DockWidgetPanel);
    DockWidgetDragDrop* dragDropObject = DockPanelManagerPrivate::_get(dockManager())->dockingMasterDrag();
    if (dragDropObject->isStartDrag())
    {
        Q_ASSERT(false);
        return;
    }
    m_pressedPos = pos;
    m_dragPanel = dragPanel;
    p.grabMouse();
    dragDropObject->startDrag();
}

void DockWidgetPanelPrivate::mouseRelease()
{
    QTN_P(DockWidgetPanel);

    if (m_pressedPos != cursorPositionInvalid)
    {
        m_pressedPos = cursorPositionInvalid;
        m_dragPanel = Q_NULL;
        p.releaseMouse();
    }

    DockWidgetDragDrop* dragDropObject = DockPanelManagerPrivate::_get(dockManager())->dockingMasterDrag();
    if (!dragDropObject->isStartDrag())
        return;
    dragDropObject->stopDrag();
}

DockPanelInfo& DockWidgetPanelPrivate::panelInfo() const
{
    QTN_P(const DockWidgetPanel);
    const_cast<DockWidgetPanelPrivate *>(this)->idNeeded();
    return layoutEngine()->getPanelInfo(&p, m_id);
}

void DockWidgetPanelPrivate::idNeeded()
{
    QTN_P(DockWidgetPanel);
    if (!m_id.isEmpty())
        return;
    m_id = layoutEngine()->generatePanelId(p.caption());
}

void DockWidgetPanelPrivate::setLayoutEngine(DockLayoutEngine* layoutEngine)
{
    QTN_P(DockWidgetPanel);
    if (m_layoutEngine != Q_NULL)
        m_layoutEngine->removeWidgetPanel(&p);
    m_layoutEngine = layoutEngine;
    if (m_layoutEngine != Q_NULL)
        m_layoutEngine->addWidgetPanel(&p);
}

void DockWidgetPanelPrivate::mouseMove(const QPoint& pos)
{
    QTN_P(DockWidgetPanel);
    DockWidgetDragDrop* dragDropObject = DockPanelManagerPrivate::_get(dockManager())->dockingMasterDrag();
    if (m_pressedPos == cursorPositionInvalid || !dragDropObject->isStartDrag())
        return;
    QPoint offset = m_pressedPos - pos;
    if (offset.manhattanLength() > QApplication::startDragDistance())
    {
        DockPanelBase* panelForDragging = m_dragPanel != Q_NULL ? m_dragPanel : &p;
        p.releaseMouse();
        QPoint pos = m_pressedPos;
        m_pressedPos = cursorPositionInvalid;
        m_dragPanel = Q_NULL;

        dragDropObject->execDrag(panelForDragging, pos);
        if (dragDropObject->isStartDrag())
            dragDropObject->acceptDrag();
        dragDropObject->stopDrag();
    }
}

/*! \internal */
void DockWidgetPanelPrivate::updateActions()
{
    QTN_P(DockWidgetPanel);
    m_actFloat->setEnabled(!p.isFloat() && !p.isAutoHide() && !p.isClosed());
    m_actDock->setEnabled((p.isFloat() || p.isAutoHide()) && !p.isClosed());
    m_actAutoHide->setEnabled((!p.isAutoHide() && !p.isFloat()) && !p.isClosed());
    m_actHide->setEnabled(!p.isClosed());
    m_actFloat->setVisible(p.features().testFlag(DockWidgetPanel::DockPanelFloatable));
    m_actHide->setVisible(p.features().testFlag(DockWidgetPanel::DockPanelClosable));
    m_actAutoHide->setVisible(p.features().testFlag(DockWidgetPanel::DockPanelHideable));
}

/*! \internal */
void DockWidgetPanelPrivate::checkActions()
{
    QTN_P(DockWidgetPanel);
    m_actVisible->setChecked(!p.isClosed() || p.isAutoHide());
}

/*! \internal */
bool DockWidgetPanelPrivate::isAutoHide() const
{
    QTN_P(const DockWidgetPanel);
    DockPanelBase* panelBase = p.parentPanel();
    if (panelBase == Q_NULL)
        return false;
    return DockPanelBasePrivate::_get(panelBase)->isAutoHide();
}

/*! \internal */
void DockWidgetPanelPrivate::getMinMaxData(qtnMinMaxSize& minmaxSize) const
{
    QTN_P(const DockWidgetPanel);
    minmaxSize.minTrackSize = p.minimumSize();
    minmaxSize.maxTrackSize = p.maximumSize();
}

/*! \internal */
Qtitan::DockPanelAreas DockWidgetPanelPrivate::allowedAreas() const
{ return m_allowedAreas; }

/*! \internal */
void DockWidgetPanelPrivate::autoHideChanged(bool)
{
    QTN_P(DockWidgetPanel);
    if (dockManager())
        dockManager()->setDockPanelAutoHide(&p, true);
}

/*! \internal */
void DockWidgetPanelPrivate::floatingChanged(bool)
{
    QTN_P(DockWidgetPanel);
    if (dockManager())
        dockManager()->setDockPanelFloat(&p, true);
}

/*! \internal */
void DockWidgetPanelPrivate::dockableChanged(bool)
{
    QTN_P(DockWidgetPanel);
    if (dockManager())
        dockManager()->setDockPanelFloat(&p, false);
}

/*! \internal */
void DockWidgetPanelPrivate::hideChanged(bool)
{
    QTN_P(DockWidgetPanel);
    p.closePanel();
}

/*! \internal */
void DockWidgetPanelPrivate::visibleChanged(bool checked)
{
    QTN_P(DockWidgetPanel);
    if (isAutoHide() || p.isHidden())
        checked = true;

    if (checked)
        p.showPanel();
    else
        p.closePanel();
}

/*!
\class DockWidgetPanel
\inmodule QtitanDocking
\brief Implements the panel for docking that contains QWidget.
\sa DockLayoutPanel
*/
DockWidgetPanel::DockWidgetPanel(DockPanelManager* manager)
    : DockWidgetPanel(*new DockWidgetPanelPrivate(manager))
{
}

DockWidgetPanel::DockWidgetPanel(const QString& caption, DockPanelManager* manager)
    : DockWidgetPanel(*new DockWidgetPanelPrivate(manager))
{
    setCaption(caption);
}

DockWidgetPanel::DockWidgetPanel(DockWidgetPanelPrivate& dd)
    : DockPanelBase(dd)
{
    QTN_D(DockWidgetPanel);
    if (d.layoutEngine() != Q_NULL)
        d.layoutEngine()->addWidgetPanel(this);
}

DockWidgetPanel::~DockWidgetPanel()
{
    QTN_D(DockWidgetPanel);
    DockPanelManagerPrivate::_get(dockManager())->internal_detachDockPanel(this);
    if (d.layoutEngine() != Q_NULL)
        d.layoutEngine()->removeWidgetPanel(this);
}

QWidget* DockWidgetPanel::widget() const
{
    DockWidgetPanelLayout* layout = qobject_cast<DockWidgetPanelLayout*>(this->layout());
    return layout->widgetForRole(DockWidgetPanelLayout::Content);
}

void DockWidgetPanel::setWidget(QWidget* widget)
{
    QTN_D(DockWidgetPanel);
    if (QFrame* frame = qobject_cast<QFrame*>(widget))
    {
        if (frame->frameShape() != QFrame::NoFrame)
            d.m_frameStyle = QFrame::NoFrame | QFrame::Plain;
    }
    else
        d.m_frameStyle = QFrame::StyledPanel | QFrame::Plain;
    widget->setParent(this);
    widget->show();
    setFocusProxy(widget);
    DockWidgetPanelLayout* layout = qobject_cast<DockWidgetPanelLayout*>(this->layout());
    layout->setWidgetForRole(DockWidgetPanelLayout::Content, widget);
}

QIcon DockWidgetPanel::icon() const
{
    QTN_D(const DockWidgetPanel);
    return d.m_icon;
}

void DockWidgetPanel::setIcon(const QIcon& icon)
{
    QTN_D(DockWidgetPanel);
    d.m_icon = icon;
    if (DockTitleBar* titleBar = qobject_cast<DockTitleBar*>(this->titleBar()))
        titleBar->setIcon(d.m_icon);

    if (DockPanelManager* panelManager = dockManager())
        DockPanelManagerPrivate::_get(panelManager)->updatePanelProperties(this);
}

QMenu* DockWidgetPanel::menuButton() const
{
    QTN_D(const DockWidgetPanel);
    return d.m_menuButton;
}

void DockWidgetPanel::setMenuButton(QMenu* menu)
{
    QTN_D(DockWidgetPanel);
    if (menu == Q_NULL)
        return;

    if (d.m_menuButton && d.m_menuButton != menu)
    {
        d.m_menuButton->hide();
        d.m_menuButton->deleteLater();
    }
    d.m_menuButton = menu;
    d.m_menuButton->setParent(this);
    d.m_actMenuButton->setMenu(d.m_menuButton);
//    if (DockTitleBar* _titleBar = qobject_cast<DockTitleBar*>(titleBar()))
//        _titleBar->setButtonMenu(d.m_menuButton);
}

DockWidgetPanel::DockPanelFeatures DockWidgetPanel::features() const
{
    QTN_D(const DockWidgetPanel);
    DockPanelManager* panelManager = dockManager();
    return panelManager != Q_NULL ? (panelManager->defaultPaneFeatures() | d.m_features) : DockWidgetPanel::NoDockPanelFeatures;
}

void DockWidgetPanel::setFeatures(DockWidgetPanel::DockPanelFeatures features)
{
    QTN_D(DockWidgetPanel);
    features &= DockPanelFeatureMask;
    if (d.m_features == features)
        return;

    d.m_features = features;

    DockWidgetPanelLayout* layoutPanel = qobject_cast<DockWidgetPanelLayout*>(this->layout());
    Q_ASSERT(layoutPanel != Q_NULL);
    if (d.m_features.testFlag(DockWidgetPanel::DockPanelNoCaption) && titleBar() != Q_NULL)
    {
        QWidget* titleBar = this->titleBar();
        if (DockTabbedLayoutPanel* parentPanel = qobject_cast<DockTabbedLayoutPanel*>(this->parentPanel()))
            DockTabbedLayoutPanelPrivate::_get(parentPanel)->connectToTitleBar(this, false);

        setTitleBar(Q_NULL);
        Q_DELETE_AND_NULL(titleBar);
    }
    else if (!d.m_features.testFlag(DockWidgetPanel::DockPanelNoCaption) && titleBar() == Q_NULL)
    {
        DockTitleBar* titleBar = new DockTitleBar(this);
        setTitleBar(titleBar);
        titleBar->setIcon(d.m_icon);
        d.addActionToTitleBar();

        if (DockTabbedLayoutPanel* parentPanel = qobject_cast<DockTabbedLayoutPanel*>(this->parentPanel()))
            DockTabbedLayoutPanelPrivate::_get(parentPanel)->connectToTitleBar(this, true);

        titleBar->setText(caption());
        DockWidgetPanel* widgetPanel = dockManager()->activeDockPanel();
        titleBar->setTitleActive(widgetPanel == this);
    }

    if (DockTitleBar* bar = qobject_cast<DockTitleBar*>(titleBar()))
        bar->updateFeatures(d.m_features);
    emit featuresChanged(d.m_features);
    update();
}

bool DockWidgetPanel::isValid() const
{ return widget() != Q_NULL; }

void DockWidgetPanel::showPanel()
{
    if (dockManager())
        dockManager()->showDockPanel(this);
}

bool DockWidgetPanel::isClosed() const
{
    return parentPanel() == Q_NULL;
}

void DockWidgetPanel::closePanel()
{
    if (dockManager())
        dockManager()->closeDockPanel(this);
}

const QString& DockWidgetPanel::caption() const
{
    QTN_D(const DockWidgetPanel);
    return d.m_caption;
}

void DockWidgetPanel::setCaption(const QString& caption)
{
    QTN_D(DockWidgetPanel);
    if (d.m_caption != caption)
    {
        d.m_caption = caption;
//        d.m_actVisible->setText(d.m_caption);

        if (DockTitleBar* titleBar = qobject_cast<DockTitleBar*>(this->titleBar()))
            titleBar->setText(d.m_caption);

        if (DockPanelManager* panelManager = dockManager())
            DockPanelManagerPrivate::_get(panelManager)->updatePanelProperties(this);
    }
}

bool DockWidgetPanel::isAutoHide() const
{
    QTN_D(const DockWidgetPanel);
    return d.isAutoHide();
}

void DockWidgetPanel::setAutoHide(bool set)
{
    if (dockManager())
        dockManager()->setDockPanelAutoHide(this, set);
}

/*!
Returns true if the panel is active, otherwise returns false.
*/
bool DockWidgetPanel::isActive() const
{
    if (parentPanel() == Q_NULL)
        return false;
    Q_ASSERT(qtn_checkLayoutType(parentPanel(), DockLayoutPanel::Tabbed));
    return qobject_cast<DockTabbedLayoutPanel*>(parentPanel())->activePanel() == this;
}

/*!
Activates the panel and move the focus to the first QWidget on this panel.
*/
void DockWidgetPanel::activate()
{
    if (dockManager())
        dockManager()->showDockPanel(this);
}

/*!
Deactivates the panel and blurs the focus.
*/
void DockWidgetPanel::deactivate()
{
    clearFocus();
    if (dockManager())
        DockPanelManagerPrivate::_get(dockManager())->changedActiveDockPanel(this, false);
}

/*!
Returns true if the panel is float.
*/
bool DockWidgetPanel::isFloat() const
{
    if (parentPanel() == Q_NULL)
        return false;
    if (isAutoHide())
        return false;
    return qobject_cast<DockWindow*>(topDockWindow()) != Q_NULL;
}

/*!
Determines whether the panel is floating or not. If the parameter \a _float is true the panel should be floating.
In floating mode, the panel is placed on a separate top-level window.
*/
void DockWidgetPanel::setFloat(bool _float)
{
    if (dockManager() != Q_NULL)
        dockManager()->setDockPanelFloat(this, _float);
}

/*!
Returns the bit mask of the flags in which areas the docking of this panel is allowed.
*/
Qtitan::DockPanelAreas DockWidgetPanel::allowedAreas() const
{
    QTN_D(const DockWidgetPanel);
    return d.allowedAreas();
}

/*!
Returns true if the panel can be docked to \a area otherwise returns false.
*/
bool DockWidgetPanel::isAreaAllowed(Qtitan::DockPanelAreas area) const
{
    return (allowedAreas() & area) == area;
}

/*!
Sets the bit mask of the flags in which areas the docking of this panel is allowed.
*/
void DockWidgetPanel::setAllowedAreas(Qtitan::DockPanelAreas areas)
{
    QTN_D(DockWidgetPanel);
    areas &= Qtitan::DockPanelArea_Mask;
    if (areas == d.m_allowedAreas)
        return;
    d.m_allowedAreas = areas;
    emit allowedAreasChanged(d.m_allowedAreas);
}

/*!
Returns QAction that can be used to control this panel visibility. 
*/
QAction* DockWidgetPanel::visibleAction() const
{
    QTN_D(const DockWidgetPanel);
    return d.m_actVisible;
}

QWidget* DockWidgetPanel::titleBar() const
{
    DockWidgetPanelLayout* layout = qobject_cast<DockWidgetPanelLayout*>(this->layout());
    if (TitleBarWrapper* wrapper = qobject_cast<TitleBarWrapper*>(layout->widgetForRole(DockWidgetPanelLayout::TitleBar)))
        return wrapper->titleBar();
    return Q_NULL;
}

void DockWidgetPanel::setTitleBar(QWidget* widget)
{
    DockWidgetPanelLayout* layout = qobject_cast<DockWidgetPanelLayout*>(this->layout());
    if (TitleBarWrapper* wrapper = qobject_cast<TitleBarWrapper*>(layout->widgetForRole(DockWidgetPanelLayout::TitleBar)))
    {
        wrapper->setTitleBar(widget);
        layout->invalidate();
    }
}

void DockWidgetPanel::setParentPanel(DockPanelBase* parentPanel)
{
    Q_ASSERT(parentPanel == Q_NULL || qtn_checkLayoutType(parentPanel, DockLayoutPanel::Tabbed));
    DockPanelBase::setParentPanel(parentPanel);
}

/*! \reimp */
bool DockWidgetPanel::event(QEvent* event)
{
    QTN_D(DockWidgetPanel);
    switch (event->type())
    {
        case QEvent::Hide:
            d.checkActions();
            break;
        case QEvent::Show:
            d.checkActions();
            break;
        default:
            break;
    }
    return DockPanelBase::event(event);
}

/*! \reimp */
void DockWidgetPanel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    if (widget() == Q_NULL)
    {
        QPainter painter(this);
        painter.setPen(Qt::gray);
        painter.setFont(QFont(QStringLiteral("Arial")));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QStyleOptionFrame opt;
#else
        QStyleOptionFrameV3 opt;
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        initStyleOption(&opt);
        style()->drawControl(QStyle::CE_ShapedFrame, &opt, &painter, this);

        QPixmap icon(QStringLiteral(":res/qtitanlogo32x32.png"));
        QString text = tr("The panel is not configured.");
        QSize iconSize = QSize(5 + icon.width() + 10, icon.height());
        QSize textSize = painter.fontMetrics().boundingRect(text).size();
        textSize.rwidth() += 5;
        if (textSize.width() > width() - iconSize.width())
        {
            text = painter.fontMetrics().elidedText(text, Qt::ElideRight, width() - iconSize.width());
            textSize.rwidth() = width() - iconSize.width();
        }

        QSize s(iconSize.width() + textSize.width(), qMax(iconSize.height(), textSize.height()));
        QRect r = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, s, rect());
        r.adjust(5, 0, 0, 0);
        painter.drawPixmap(QRect(r.topLeft(), QSize(icon.width(), r.height())), icon);
        r.adjust(icon.width() + 10, 0, 0, 0);
        painter.drawText(r, text, QTextOption(Qt::AlignVCenter));
    }
}

void DockWidgetPanel::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(DockWidgetPanel);
    DockPanelBase::mouseMoveEvent(event);
    d.mouseMove(event->pos());
}

void DockWidgetPanel::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(DockWidgetPanel);
    DockPanelBase::mouseReleaseEvent(event);
#ifdef Q_OS_LINUX //See QtnPlatform_linux : qtn_X11EmulateButtonRelease(w, pos);
    if (TitleBarWrapper::isNativeMouseButtons_linux())
    {
        event->ignore();
        return;
    }
#endif
    d.mouseRelease();
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void DockWidgetPanel::initStyleOption(QStyleOptionFrame* option) const
#else
void DockWidgetPanel::initStyleOption(QStyleOptionFrameV3* option) const
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
{
    QTN_D(const DockWidgetPanel);
    if (!option)
        return;

    option->initFrom(this);

    int frameShape = d.m_frameStyle & QFrame::Shape_Mask;
    int frameShadow = d.m_frameStyle & QFrame::Shadow_Mask;
    option->frameShape = QFrame::Shape(int(option->frameShape) | frameShape);
    option->rect = contentsRect();
    option->rect.adjust(1, 1, -1, -1);
    switch (frameShape)
    {
        case QFrame::Box:
        case QFrame::HLine:
        case QFrame::VLine:
        case QFrame::StyledPanel:
        case QFrame::Panel:
            option->lineWidth = 1;
            option->midLineWidth = 0;
            break;
        default:
            option->lineWidth = 1;
            break;
    }

    if (frameShadow == QFrame::Sunken)
        option->state |= QStyle::State_Sunken;
    else if (frameShadow == QFrame::Raised)
        option->state |= QStyle::State_Raised;
}

