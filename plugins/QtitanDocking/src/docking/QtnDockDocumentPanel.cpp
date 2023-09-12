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
#include "QtnDockDocumentPanelPrivate.h"
#include "QtnDockPanelManagerPrivate.h"
#include "QtnDockWindowPrivate.h"
#include <QToolButton>
#include <QPainter>
#include <qevent.h>

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

extern int qtn_generate_document_panel_id();

extern DockDocumentLayoutPanel* qtn_parentDocumentLayout(DockLayoutPanel* layoutPanel)
{
    if (DockDocumentLayoutPanel* documentLayoutPanel = qobject_cast<DockDocumentLayoutPanel*>(layoutPanel))
        return documentLayoutPanel;
    if (DockLayoutPanel* parent = qobject_cast<DockLayoutPanel*>(layoutPanel->parentPanel()))
    {
        if (DockDocumentLayoutPanel* documentLayoutPanel = qobject_cast<DockDocumentLayoutPanel*>(parent))
            return documentLayoutPanel;
        else if (DockDocumentLayoutPanel* documentLayoutPanel = qtn_parentDocumentLayout(parent))
            return documentLayoutPanel;
    }
    return Q_NULL;
}


/* DockDocumentAreaTabBar */
DockDocumentAreaTabBar::DockDocumentAreaTabBar(QWidget* parent)
    : DockAreaTabBar(parent)
{
}

DockDocumentAreaTabBar::~DockDocumentAreaTabBar()
{
}

/* DockDocumentTabbedLayoutPanelPrivate */
DockDocumentTabbedLayoutPanelPrivate::DockDocumentTabbedLayoutPanelPrivate(DockLayoutEngine* layoutEngine)
    : DockTabbedLayoutPanelPrivate(layoutEngine, new DockDocumentAreaTabBar())
{
}

DockDocumentTabbedLayoutPanelPrivate::~DockDocumentTabbedLayoutPanelPrivate()
{
}

/*! \internal */
void DockDocumentTabbedLayoutPanelPrivate::repositionTabBar()
{
    QTN_P(DockTabbedLayoutPanel);
    if (!m_tabBar->isHidden())
    {
        QRect rcTab = p.geometry();
        if (rcTab.size().isNull())
            return;
        QSize szTab = m_tabBar->sizeHint();
        QTabBar::Shape shape = m_tabBar->shape();
        if (shape == QTabBar::RoundedSouth)
        {
            rcTab.moveTopLeft(QPoint(0, 0));
            rcTab.setTop(rcTab.height() - szTab.height());
            m_tabBar->setGeometry(rcTab);
        }
        else if (shape == QTabBar::RoundedNorth)
            m_tabBar->setGeometry(QRect(QPoint(0, 0), QSize(rcTab.width(), szTab.height())));
        else if (shape == QTabBar::RoundedWest)
            m_tabBar->setGeometry(QRect(QPoint(0, 0), QSize(rcTab.width(), szTab.height())));
        else if (shape == QTabBar::RoundedEast)
            m_tabBar->setGeometry(QRect(QPoint(rcTab.width() - szTab.width(), 0), QSize(rcTab.width(), szTab.height())));
    }
}

/*! \internal */
void DockDocumentTabbedLayoutPanelPrivate::closePanel(int index)
{
    QVariant variant = m_tabBar->tabData(index);
    if (DockWidgetPanel* widgetPanel = qvariant_cast<DockWidgetPanel*>(variant))
        widgetPanel->closePanel();
}

/*! \internal */
void DockDocumentTabbedLayoutPanelPrivate::setChildrenGeometry(const QRect& rect)
{
    QTN_P(DockTabbedLayoutPanel);
    QTabBar::Shape shape = m_tabBar->shape();
    for (QList<DockPanelBase*>::Iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
    {
        if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(*it))
        {
            if (QWidget* wd = widgetPanel->titleBar())
                wd->setVisible(false);
            QRect rcPanel = p.activePanel() == widgetPanel ? rect : QRect(0, 0, 0, 0);
            rcPanel.moveTopLeft(QPoint(0, 0));
            if (shape == QTabBar::RoundedNorth)
                rcPanel.adjust(0, m_tabBar->height(), 0, 0);
            else if (shape == QTabBar::RoundedSouth)
                rcPanel.adjust(0, 0, 0, -m_tabBar->height());
            else if (shape == QTabBar::RoundedWest)
                rcPanel.adjust(m_tabBar->sizeHint().width(), 0, 0, 0);
            else if (shape == QTabBar::RoundedEast)
                rcPanel.adjust(0, 0, -(m_tabBar->sizeHint().width()), 0);
            widgetPanel->setVisible(!rcPanel.isEmpty());
            DockPanelBasePrivate::_get(widgetPanel)->setPanelGeometry(rcPanel);
        }
    }
}

/* DockDocumentTabbedLayoutPanel */
DockDocumentTabbedLayoutPanel::DockDocumentTabbedLayoutPanel(DockLayoutEngine* layoutEngine)
    : DockTabbedLayoutPanel(*new DockDocumentTabbedLayoutPanelPrivate(layoutEngine))
{
    QTN_D(DockDocumentTabbedLayoutPanel);
    setTabPosition(Qtitan::TopArea);
    d.m_tabBar->setTabsClosable(true);
    connect(d.m_tabBar, SIGNAL(tabCloseRequested(int)), &d, SLOT(closePanel(int)));
}

/*! \internal */
DockDocumentTabbedLayoutPanel::~DockDocumentTabbedLayoutPanel()
{
}

void DockDocumentTabbedLayoutPanel::setFeatures(const DockWidgetPanel::DockPanelFeatures& features){
    m_features = features;
}

void DockDocumentTabbedLayoutPanel::setTabTooltip(const QString& tooltip){
    m_tooltip = tooltip;
}

//void DockDocumentTabbedLayoutPanel::initialization(DockWidgetPanel* panel){
//    DockTabbedLayoutPanel::initialization(panel);
//    QTN_D(DockDocumentTabbedLayoutPanel);
//    if(m_features == DockWidgetPanel::DockPanelClosable || m_features == DockWidgetPanel::AllDockPanelFeatures){
//        d.m_tabBar->setTabsClosable(true);
//    }
//    else{
//        d.m_tabBar->setTabsClosable(false);
//    }
//    d.m_tabBar->setToolTip(m_tooltip);
//}

DockLayoutPanel* DockDocumentTabbedLayoutPanel::createCopyInstance() const
{
    Q_ASSERT(false);
    return Q_NULL;
}

void DockDocumentTabbedLayoutPanel::insertChildPanel(DockPanelBase* panelBase)
{
    DockTabbedLayoutPanel::insertChildPanel(panelBase);
    if (DockDocumentPanel* docPanel = qobject_cast<DockDocumentPanel*>(panelBase))
    {
        DockPanelManagerPrivate* managerPrivate = DockPanelManagerPrivate::_get(dockManager());
        if (!managerPrivate->dockLayoutEngine()->widgetPanelList().contains(docPanel))
            managerPrivate->preservePanelState();
    }
}

void DockDocumentTabbedLayoutPanel::removeChildPanel(DockPanelBase* panelBase)
{
    DockTabbedLayoutPanel::removeChildPanel(panelBase);
}

/*! \internal */
bool DockDocumentTabbedLayoutPanel::isTitleVisible() const
{ return false; }

/*! \internal */
bool DockDocumentTabbedLayoutPanel::isTabsVisible() const
{ return true; }

bool DockDocumentTabbedLayoutPanel::canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool sideCenter)
{
    if (DockTabbedLayoutPanel::canDock(area, dragPanel, sideCenter) || sideCenter)
    {
        if (area == Qtitan::TopDockPanelArea || area == Qtitan::BottomDockPanelArea ||
            area == Qtitan::LeftDockPanelArea || area == Qtitan::RightDockPanelArea)
        {
            if (DockSplitLayoutPanel* dockSplitLayout = dynamic_cast<DockSplitLayoutPanel*>(this->parentPanel()))
            {
                if (sideCenter)
                {
                    QList<DockPanelBase*> lstPanel = dockSplitLayout->findPanels(DockTabbedLayoutPanel::staticMetaObject.className());
                    if (lstPanel.size() > 1)
                    {
                        if (dockSplitLayout->isHorizontal() && (area == Qtitan::TopDockPanelArea || area == Qtitan::BottomDockPanelArea))
                            return false;
                        else if (!dockSplitLayout->isHorizontal() && (area == Qtitan::LeftDockPanelArea || area == Qtitan::RightDockPanelArea))
                            return false;
                    }
                    return true;
                }
                else 
                {
                    QList<DockPanelBase*> lstPanel = dockSplitLayout->findPanels(DockDocumentTabbedLayoutPanel::staticMetaObject.className());
                    int index = lstPanel.indexOf(this);
                    if (index != -1)
                    {
                        if (dockSplitLayout->isHorizontal())
                        {
                            if (area == Qtitan::LeftDockPanelArea)
                                return index == 0;
                            else if (area == Qtitan::RightDockPanelArea)
                                return index == lstPanel.size() - 1;
                            else if (area == Qtitan::BottomDockPanelArea)
                                return true;
                            else if (area == Qtitan::TopDockPanelArea)
                                return true;
                        }
                        else
                        {
                            if (area == Qtitan::TopDockPanelArea)
                                return index == 0;
                            else if (area == Qtitan::BottomDockPanelArea)
                                return index == lstPanel.size() - 1;
                            else if (area == Qtitan::LeftDockPanelArea)
                                return true;
                            else if (area == Qtitan::RightDockPanelArea)
                                return true;
                        }
                    }
                    return dragPanel->findPanels(DockDocumentPanel::staticMetaObject.className()).size() == 0;
                }
            }
        }
        else if (area == Qtitan::InsideDockPanelArea)
            return true;
    }
    if (area == Qtitan::InsideDockPanelArea || sideCenter)
    {
        QList<DockPanelBase*> listPanel;
        listPanel << dragPanel->findPanels(DockDocumentPanel::staticMetaObject.className());
        return listPanel.size() > 0;
    }
    return false;
}

/*! \reimp */
void DockDocumentTabbedLayoutPanel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QStyleOptionFrame opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_FrameDockWidget, &opt, &painter, this);
}


/* DockDocumentLayoutPanelPrivate */
DockDocumentLayoutPanelPrivate::DockDocumentLayoutPanelPrivate(DockLayoutEngine* layoutEngine)
    : DockSplitLayoutPanelPrivate(layoutEngine)
    , m_tabPosition(Qtitan::TopArea)
{
}

/*! \internal */
DockDocumentLayoutPanelPrivate::~DockDocumentLayoutPanelPrivate()
{
}

/*! \internal */
bool DockDocumentLayoutPanelPrivate::isEmpty() const
{
    QTN_P(const DockDocumentLayoutPanel);
    if (DockWindow* dockWindow = qobject_cast<DockWindow*>(p.topDockWindow()))
    {
        if (DockLayoutPanel* layoutPanel = dockWindow->layoutPanel())
        {
            QList<DockPanelBase*> lstWidgetPanels = layoutPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
            if (isEmptyFull())
                return lstWidgetPanels.count() == 0;
        }
        return false;
    }
    return !DockPanelManagerPrivate::_get(dockManager())->isCentralPanelVisible();
}

/*! \internal */
bool DockDocumentLayoutPanelPrivate::isEmptyFull() const
{
    return DockSplitLayoutPanelPrivate::isEmpty();
}

/*! \internal */
void DockDocumentLayoutPanelPrivate::insertDockPanel(DockPanelBase* panelBase)
{
    DockSplitLayoutPanelPrivate::insertDockPanel(panelBase);
}


/* 
DockDocumentLayoutPanel 
One toplevel window can contains only one instance of DockDocumentLayoutPanel.
*/
DockDocumentLayoutPanel::DockDocumentLayoutPanel(DockLayoutEngine* layoutEngine)
    : DockSplitLayoutPanel(*new DockDocumentLayoutPanelPrivate(layoutEngine))
{
}

/*!
Destroys the DocumentLayoutPanel.
*/
DockDocumentLayoutPanel::~DockDocumentLayoutPanel()
{
}

Qtitan::LayoutArea DockDocumentLayoutPanel::tabPosition() const
{
    QTN_D(const DockDocumentLayoutPanel);
    return d.m_tabPosition;
}

DockDocumentTabbedLayoutPanel* DockDocumentLayoutPanel::activTabbedLayout() const
{
    QTN_D(const DockDocumentLayoutPanel)
    DockDocumentTabbedLayoutPanel* tabbedLayoutPanel = Q_NULL;
    QList<DockDocumentTabbedLayoutPanel*> listPanel;
    for (QList<DockPanelBase*>::const_iterator it = d.m_panelList.begin(); it != d.m_panelList.end(); ++it)
    {
        if (DockDocumentTabbedLayoutPanel* tabbedPanel = qobject_cast<DockDocumentTabbedLayoutPanel*>(*it))
            listPanel << tabbedPanel;
    }

    if (DockDocumentPanel* activePanel = qobject_cast<DockDocumentPanel*>(dockManager()->activeDockPanel()))
    {
        if (DockDocumentTabbedLayoutPanel* activeTabbedPanel = qobject_cast<DockDocumentTabbedLayoutPanel*>(activePanel->parentPanel()))
        {
            if (listPanel.contains(activeTabbedPanel))
                tabbedLayoutPanel = activeTabbedPanel;
        }
    }

    if (tabbedLayoutPanel == Q_NULL && listPanel.count() > 0)
        tabbedLayoutPanel = qobject_cast<DockDocumentTabbedLayoutPanel*>(listPanel.at(0));
    return tabbedLayoutPanel;
}

DockLayoutPanel* DockDocumentLayoutPanel::createCopyInstance() const
{
    Q_ASSERT(false);
    return Q_NULL;
}

bool DockDocumentLayoutPanel::canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center)
{ 
    if (DockLayoutPanel::canDock(area, dragPanel, center))
    {
        Qtitan::DockPanelAreas allowedAreas = DockLayoutPanelPrivate::isAllowedAreas(dragPanel, DockDocumentPanel::staticMetaObject.className());
        if (!allowedAreas.testFlag(area))
            return false;

        QList<DockPanelBase*> listDocPanels, listWidgetPanels;
        listWidgetPanels << dragPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
        listDocPanels << dragPanel->findPanels(DockDocumentPanel::staticMetaObject.className());

        if (listDocPanels.size() > 0 && listWidgetPanels.size() > 0 && area == Qtitan::InsideDockPanelArea)
            return true;
        else if (listDocPanels.size() == 0 && listWidgetPanels.size() == 0 && area == Qtitan::InsideDockPanelArea)
            return true;
        else if (listDocPanels.size() == 0 && listWidgetPanels.size() > 0)
            return true;

        QList<DockPanelBase*> listPanels;
        listPanels << findPanels(DockWidgetPanel::staticMetaObject.className()) << findPanels(DockDocumentPanel::staticMetaObject.className());
        if (listPanels.size() == 0 && area == Qtitan::InsideDockPanelArea)
            return true;
    }
    return false;
}

void DockDocumentLayoutPanel::insertChildPanel(DockPanelBase* panel)
{
    DockSplitLayoutPanel::insertChildPanel(panel);
}

void DockDocumentLayoutPanel::removeChildPanel(DockPanelBase* panelBase)
{
    DockSplitLayoutPanel::removeChildPanel(panelBase);
}

/*! \reimp */
void DockDocumentLayoutPanel::paintEvent(QPaintEvent* event)
{
    DockSplitLayoutPanel::paintEvent(event);
#if __DEBUG_PAINTER
    QPainter p(this);
    QRect rect = this->rect();
    QColor clr = QColor(Qt::darkMagenta);
    p.fillRect(rect, clr);
    p.setPen(QPen(Qt::darkYellow, 6 + 7));
    p.drawRect(rect.adjusted(1, 1, -1, -1));
#endif
}

/* DockDocumentPanelPrivate */
DockDocumentPanelPrivate::DockDocumentPanelPrivate(DockPanelManager* manager)
    : DockWidgetPanelPrivate(manager)
{
}

/*! \internal */
DockDocumentPanelPrivate::~DockDocumentPanelPrivate()
{
}

/*! \internal */
void DockDocumentPanelPrivate::initialization()
{
    QTN_P(DockDocumentPanel);
    p.setAllowedAreas(Qtitan::InsideDockPanelArea);
    p.setFocusPolicy(Qt::StrongFocus);
    p.setMouseTracking(true);
}

/*! \internal */
Qtitan::DockPanelAreas DockDocumentPanelPrivate::allowedAreas() const
{
    Qtitan::DockPanelAreas allowedAreas = Qtitan::NoDockPanelArea;
    allowedAreas |= Qtitan::InsideDockPanelArea;
    return allowedAreas;
}

/*! \internal */
void DockDocumentPanelPrivate::setPanelGeometry(const QRect& rect)
{
    QTN_P(DockWidgetPanel);
    DockWidgetPanelPrivate::setPanelGeometry(rect);
    p.setVisible(!rect.isEmpty());
}


/*!
\class DockDocumentPanel
\inmodule QtitanDocking
\brief Special panel that holds QWidget and can be used in dock documnent layout. 
\sa DockPanelManager::addDocumentPanel()
*/
DockDocumentPanel::DockDocumentPanel(DockPanelManager* manager)
    : DockWidgetPanel(*new DockDocumentPanelPrivate(manager))
{
    QTN_D(DockDocumentPanel);
    d.initialization();
    setTitleBar(Q_NULL);
}

DockDocumentPanel::DockDocumentPanel(DockDocumentPanelPrivate& dd)
    : DockWidgetPanel(dd)
{
    dd.initialization();
    setTitleBar(Q_NULL);
}

/*!
Destroys the DocumentPanel.
*/
DockDocumentPanel::~DockDocumentPanel()
{
}

/*! \reimp */
void DockDocumentPanel::setParentPanel(DockPanelBase* parentPanel)
{
    Q_ASSERT(parentPanel == Q_NULL || qobject_cast<DockDocumentTabbedLayoutPanel*>(parentPanel));
    DockPanelBase::setParentPanel(parentPanel);
}

/*! \reimp */
void DockDocumentPanel::resizeEvent(QResizeEvent* event)
{
    QTN_D(DockDocumentPanel);
    DockWidgetPanel::resizeEvent(event);
    if (event->size().isValid() && !event->size().isNull())
        d.m_sizeDocking = event->size();
}
