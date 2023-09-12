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
#include <QApplication>
#include <qevent.h>

#include "QtnDockWidgetPanel.h"
#include "QtnDockWindowPrivate.h"
#include "QtnDockDocumentPanelPrivate.h"
#include "QtnDockLayoutPrivate.h"
#include "QtnDockLayoutPanelPrivate.h"
#include "QtnDockWidgetDragDrop.h"
#include "QtnDockPanelManagerPrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

extern int qtn_areaToIndex(Qtitan::LayoutArea area);
extern DockDocumentLayoutPanel* qtn_parentDocumentLayout(DockLayoutPanel* layoutPanel);

DockLayoutPanelPrivate::~DockLayoutPanelPrivate() 
{
    Q_ASSERT(m_panelList.size() == 0);
    m_panelList.clear();
}

bool DockLayoutPanelPrivate::isEmpty() const
{
    for (QList<DockPanelBase*>::const_iterator it = m_panelList.constBegin(); it != m_panelList.constEnd(); ++it)
    {
        if (!DockPanelBasePrivate::_get(*it)->isEmpty())
            return false;
    }
    return true;
}

/*!
\class DockLayoutPanel
\inmodule QtitanDocking
\brief Implements the layout panel. The panel is used to form the docking tree's layout. It can be in four states: Split, Tabbed, AutoHide, Document.
Can contain other layout panels or widget panels inside.
\sa DockWidgetPanel
*/
DockLayoutPanel::DockLayoutPanel(DockLayoutPanelPrivate& dd)
    : DockPanelBase(dd)
{
    QTN_D(DockLayoutPanel);
    if (d.layoutEngine() != Q_NULL)
        d.layoutEngine()->addLayoutPanel(this);
}

DockLayoutPanel::~DockLayoutPanel() 
{
    QTN_D(DockLayoutPanel);
    Q_ASSERT(childCount() == 0);
    if (d.layoutEngine() != Q_NULL)
        d.layoutEngine()->removeLayoutPanel(this);
}

Qtitan::DockPanelAreas DockLayoutPanelPrivate::isAllowedAreas(DockPanelBase* panelBase, const char* className)
{
    Qtitan::DockPanelAreas allowedAreas = Qtitan::AllDockPanelAreas;
    QList<DockPanelBase*> list = panelBase->findPanels(className);
    for (QList<DockPanelBase*>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        DockWidgetPanel* wp = qobject_cast<DockWidgetPanel*>(*it);
        if (wp != Q_NULL)
        {
            if (!wp->isAreaAllowed(Qtitan::InsideDockPanelArea))
                allowedAreas &= ~Qtitan::InsideDockPanelArea;
            if (!wp->isAreaAllowed(Qtitan::LeftDockPanelArea))
                allowedAreas &= ~Qtitan::LeftDockPanelArea;
            if (!wp->isAreaAllowed(Qtitan::RightDockPanelArea))
                allowedAreas &= ~Qtitan::RightDockPanelArea;
            if (!wp->isAreaAllowed(Qtitan::TopDockPanelArea))
                allowedAreas &= ~Qtitan::TopDockPanelArea;
            if (!wp->isAreaAllowed(Qtitan::BottomDockPanelArea))
                allowedAreas &= ~Qtitan::BottomDockPanelArea;
        }
    }
    return allowedAreas;
}

/*! \internal */
DockLayoutPanel* DockLayoutPanel::createCopyInstance() const
{
    Q_ASSERT(false);
    return Q_NULL;
}

bool DockLayoutPanel::isEmpty() const
{
    QTN_D(const DockLayoutPanel);
    return d.isEmpty();
}

int DockLayoutPanel::childCount() const
{
    QTN_D(const DockLayoutPanel);
    return d.m_panelList.count();
}

bool DockLayoutPanel::canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center)
{
    Q_UNUSED(center);
    Qtitan::DockPanelAreas allowedAreas = DockLayoutPanelPrivate::isAllowedAreas(dragPanel, DockWidgetPanel::staticMetaObject.className());
    if (!allowedAreas.testFlag(area))
        return false;
    return true;
}

const QList<DockPanelBase*>& DockLayoutPanel::panels() const
{
    QTN_D(const DockLayoutPanel);
    return d.m_panelList;
}

void DockLayoutPanel::childLayoutChanged()
{
    if (parentPanel() != Q_NULL)
        parentPanel()->childLayoutChanged();
    else if (DockWindow* parentWindow = qobject_cast<DockWindow*>(dockWindow()))
        DockWindowPrivate::_get(parentWindow)->childLayoutChanged();
    DockPanelBase::childLayoutChanged();
}

void DockLayoutPanel::parentLayoutChanged()
{
    for (QList<DockPanelBase*>::const_iterator it = panels().constBegin(); it != panels().constEnd(); ++it)
        (*it)->parentLayoutChanged();
    DockPanelBase::parentLayoutChanged();
}

void DockLayoutPanel::updatePanelProperties(DockWidgetPanel* widgetPanel)
{
    if (DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(parentPanel()))
        layoutPanel->updatePanelProperties(widgetPanel);
}

/* DockSplitLayoutPanelPrivate */
DockSplitLayoutPanelPrivate::DockSplitLayoutPanelPrivate(DockLayoutEngine* layoutEngine)
    : DockLayoutPanelPrivate(layoutEngine)
    , m_next(Q_NULL)
    , m_after(true)
    , m_replaceParent(false)
    , m_horizontal(false)
    , m_recalcLayout(false)
{
}

DockSplitLayoutPanelPrivate::~DockSplitLayoutPanelPrivate()
{
    while (!m_lstSpliters.isEmpty())
        delete m_lstSpliters.takeFirst();
}

/*! \internal */
int DockSplitLayoutPanelPrivate::containsPanel(DockPanelBase* panelBase) const
{
    QTN_P(const DockSplitLayoutPanel);
    if (panelBase == (DockPanelBase*)&p)
        return 1;

    for (int i = 0, count = m_panelList.count(); i < count; i++)
    {
        DockPanelBase* curPanelBase = m_panelList.at(i);
        if (DockPanelBasePrivate::_get(curPanelBase)->containsPanel(panelBase) != -1)
            return i;
    }
    return -1;
}

/*! \internal */
void DockSplitLayoutPanelPrivate::getMinMaxData(qtnMinMaxSize& minmaxSize) const
{
    QTN_P(const DockSplitLayoutPanel);
    DockPanelBasePrivate::getMinMaxData(minmaxSize);
    int size = 0;
    for (QList<DockPanelBase*>::const_iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
    {
        DockPanelBase* curPanel = (*it);
        if (DockPanelBasePrivate::_get(curPanel)->isEmpty())
            continue;
        size++;
        qtnMinMaxSize info;
        DockPanelBasePrivate::_get(curPanel)->getMinMaxData(info);

        if (size == 1)
            minmaxSize = info;
        else
        {
            getMinSize(minmaxSize, m_horizontal) += getMinSize(info, m_horizontal);
            getMaxSize(minmaxSize, m_horizontal) += getMaxSize(info, m_horizontal);

            getMinSize(minmaxSize, !m_horizontal) = qMax(getMinSize(minmaxSize, !m_horizontal), getMinSize(info, !m_horizontal));
            getMaxSize(minmaxSize, !m_horizontal) = qMin(getMaxSize(minmaxSize, !m_horizontal), getMaxSize(info, !m_horizontal));
        }
    }
    if (size > 0)
    {
        const int splitterWidth = p.style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, Q_NULL);
        getMinSize(minmaxSize, m_horizontal) += splitterWidth * (size - 1);
        getMaxSize(minmaxSize, m_horizontal) += splitterWidth * (size - 1);
    }
}

/*! \internal */
void DockSplitLayoutPanelPrivate::setPanelGeometry(const QRect& rect)
{
    QTN_P(DockSplitLayoutPanel);
    DockPanelBasePrivate::setPanelGeometry(rect);
    if (!rect.isValid())
        return;
    DockPanelManagerPrivate* managerPrivate = DockPanelManagerPrivate::_get(dockManager());
    DockPanelBase* centralPanel = managerPrivate->centralPanel();

    bool centralPanelVisible = managerPrivate->isCentralPanelVisible();
    QList<DockPanelBase*> lstFilledPanel, lstEmptyPanel;
    for (QList<DockPanelBase*>::iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
    {
        DockPanelBasePrivate* curWidgetPrivate = DockPanelBasePrivate::_get(*it);
        if (!curWidgetPrivate->isEmpty())
            lstFilledPanel.append(*it);
        else
            lstEmptyPanel.append(*it);
        if (!centralPanelVisible && curWidgetPrivate->containsPanel(centralPanel) != -1)
            centralPanelVisible = true;
    }

    if (!lstFilledPanel.isEmpty() && centralPanelVisible && !managerPrivate->isCentralPanelVisible())
        DockPanelBasePrivate::_get(centralPanel)->setPanelGeometry(QRect(0, 0, 0, 0));

    for (QList<DockPanelBase*>::iterator it = lstEmptyPanel.begin(); it != lstEmptyPanel.end(); ++it)
        DockPanelBasePrivate::_get(*it)->setPanelGeometry(QRect(0, 0, 0, 0));

    if (lstFilledPanel.count() == 0)
        return;

    if (lstFilledPanel.count() == 1)
    {
        DockPanelBasePrivate::_get(lstFilledPanel[0])->setPanelGeometry(p.rect());
        p.setVisible(true);
        return;
    }
    updateSplittersAndPanelsLength(p.rect(), lstFilledPanel);
    p.setVisible(true);
}

int& DockSplitLayoutPanelPrivate::getMinSize(qtnMinMaxSize& minMaxInfo, bool horiz) const 
{ return horiz ? minMaxInfo.minTrackSize.rwidth() : minMaxInfo.minTrackSize.rheight(); }
int& DockSplitLayoutPanelPrivate::getMaxSize(qtnMinMaxSize& minMaxInfo, bool horiz) const 
{ return horiz ? minMaxInfo.maxTrackSize.rwidth() : minMaxInfo.maxTrackSize.rheight(); }

/*! \internal */
void DockSplitLayoutPanelPrivate::extraInsertParam(DockPanelBase* next, bool after)
{
    m_next = next;
    m_after = after;
}

/*! \internal */
void DockSplitLayoutPanelPrivate::replaceChildToParent()
{
    QTN_P(DockSplitLayoutPanel);
    if (DockSplitLayoutPanel* layoutParentPanel = qobject_cast<DockSplitLayoutPanel*>(p.parentPanel()))
    {
        if (p.panels().count() != 1 || layoutParentPanel->panels().count() != 1)
        {
            Q_ASSERT(false);
            return;
        }
        DockPanelBase* childPanel = m_panelList.at(0);
        layoutParentPanel->qtn_d().m_replaceParent = true;
        layoutParentPanel->panels().at(0)->setParentPanel(Q_NULL);
        childPanel->setParentPanel(layoutParentPanel);
        p.setParentPanel(Q_NULL);
        layoutParentPanel->qtn_d().m_replaceParent = false;
    }
}

/*! \internal */
void DockSplitLayoutPanelPrivate::insertDockPanel(DockPanelBase* panelBase)
{
    Q_ASSERT(panelBase != Q_NULL);
    if (m_replaceParent)
    {
        m_panelList.insert(0, panelBase);
        return;
    }
    else
    {
        int index = m_next ? m_panelList.indexOf(m_next) : -1;
        if (m_after)
        {
            if (index == -1)
                index = m_panelList.count() - 1;
            m_panelList.insert(index + 1, panelBase);
        }
        else
        {
            if (index == -1)
                index = 0;
            m_panelList.insert(index, panelBase);
        }
    }

    m_next = Q_NULL;
    m_after = true;
}

/*! \internal */
void DockSplitLayoutPanelPrivate::updateSplittersAndPanelsLength(const QRect& rect, const QList<DockPanelBase*>& lstPanel)
{
    if (m_recalcLayout)
        return;

    DockPanelManager* manager = dockManager();
    const int splitterIndent = 0;
    const int splitterWidth = manager->managedWidget()->style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, Q_NULL);
    m_recalcLayout = true;

    QMap<DockPanelSplitter*, QRect> mapSplitter;

    int totalLength = 0, lengthAvail = 0;
    DockSplitLayoutPanel::adjustPanesLength(manager, lstPanel, rect, m_horizontal, true, totalLength, lengthAvail);

    int posSplitter = 0;
    QRect rcPanel = rect;
    for (int i = 0, count = lstPanel.count(); count > i; i++)
    {
        DockPanelBasePrivate* curPanelPrivate = DockPanelBasePrivate::_get(lstPanel.at(i));
        int length = -curPanelPrivate->m_length;
        if (curPanelPrivate->m_length > 0)
        {
            length = totalLength == 0 ? 0 : static_cast<int>((lengthAvail * curPanelPrivate->m_length) / totalLength);
            totalLength -= curPanelPrivate->m_length;
            lengthAvail = qMax(lengthAvail - length, 0);
        }

        QRect rcSplitter;
        if (m_horizontal)
        {
            if (lstPanel.count() - 1 == i)
                rcPanel.setRight(rect.right());
            else
                rcPanel.setWidth(length);
            rcSplitter = QRect(QPoint(rcPanel.right(), rect.top()), QSize(splitterWidth + splitterIndent, rect.height()));
        }
        else
        {
            if (lstPanel.count() - 1 == i)
                rcPanel.setBottom(rect.bottom());
            else
                rcPanel.setHeight(length);
            rcSplitter = QRect(QPoint(rect.left(), rcPanel.bottom() - splitterIndent), QSize(rect.width(), splitterWidth + splitterIndent));
        }

        if (m_lstSpliters.count() - 1 >= posSplitter)
        {
            if (DockPanelSplitter* splitter = m_lstSpliters.at(posSplitter))
                mapSplitter.insert(splitter, rcSplitter.intersected(rect));
            posSplitter++;
        }

        curPanelPrivate->setPanelGeometry(rcPanel);

        if (m_horizontal)
            rcPanel.setLeft(rcPanel.right() + splitterWidth);
        else
            rcPanel.setTop(rcPanel.bottom() + splitterWidth);
    }

    for (QMap<DockPanelSplitter*, QRect>::iterator it = mapSplitter.begin(); it != mapSplitter.end(); ++it)
    {
        DockPanelSplitter* splitter = it.key();
        QRect rectSplitter = it.value();
        splitter->setGeometry(rectSplitter);
        splitter->setVisible(true);
    }
    m_recalcLayout = false;
}

/*! \internal */
void DockSplitLayoutPanelPrivate::updateSplitters()
{
    QTN_P(DockSplitLayoutPanel);
    if (m_recalcLayout || layoutEngine()->isDestroying())
        return;

    while (!m_lstSpliters.isEmpty())
        delete m_lstSpliters.takeLast();

    if (m_panelList.count() > 1)
    {
        int index = 0;
        DockPanelBase* firstWidget = Q_NULL;
        for (QList<DockPanelBase*>::iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
        {
            firstWidget = (*it); index++;
            if (!DockPanelBasePrivate::_get(firstWidget)->isEmpty())
                break;
        }

        for (int i = index, count = m_panelList.count(); count > i; i++)
        {
            DockPanelBase* secondWidget = m_panelList.at(i);
            if (DockPanelBasePrivate::_get(secondWidget)->isEmpty())
                continue;
            DockPanelSplitter* splitter = new DockPanelSplitter(&p, firstWidget, secondWidget, &p);
            m_lstSpliters.append(splitter);
            firstWidget = secondWidget;
        }
    }
}

/* DockSplitLayoutPanel */
DockSplitLayoutPanel::DockSplitLayoutPanel(DockLayoutEngine* layoutEngine)
    : DockLayoutPanel(*new DockSplitLayoutPanelPrivate(layoutEngine))
{
    setFocusPolicy(Qt::NoFocus);
}

DockSplitLayoutPanel::DockSplitLayoutPanel(DockSplitLayoutPanelPrivate& d)
    : DockLayoutPanel(d)
{
    setFocusPolicy(Qt::NoFocus);
}

DockSplitLayoutPanel::~DockSplitLayoutPanel()
{
    if (DockWindow* window = qobject_cast<DockWindow*>(dockWindow()))
        DockWindowPrivate::_get(window)->checkToFinalize();
}

void DockSplitLayoutPanel::initialization(DockPanelBase* panelBase, bool horiz)
{
    QTN_D(DockSplitLayoutPanel);
    Q_ASSERT(panelBase != Q_NULL);
    if (panelBase == Q_NULL)
        return;
    d.m_horizontal = horiz;
    panelBase->setParentPanel(this);
    d.m_sizeDocking = DockPanelBasePrivate::_get(panelBase)->m_sizeDocking;
}

/*!
Adjust the size of the dock panels in the split container. It is necessary that the values of the left edge
are less than the right and the value of the top edge is less than the bottom.
*/
void DockSplitLayoutPanel::adjustDockingSize()
{
    QTN_D(DockSplitLayoutPanel);
    if (d.m_horizontal)
        d.m_sizeDocking.setWidth(rect().width());
    else
        d.m_sizeDocking.setHeight(rect().height());

    for (QList<DockPanelBase*>::iterator it = d.m_panelList.begin(); it != d.m_panelList.end(); ++it)
    {
        DockPanelBasePrivate* panelPrivate = DockPanelBasePrivate::_get(*it);
        if (!panelPrivate->isEmpty())
        {
            if (d.m_horizontal)
                panelPrivate->m_sizeDocking.setWidth((*it)->width());
            else
                panelPrivate->m_sizeDocking.setHeight((*it)->height());
        }
    }
}

void DockSplitLayoutPanel::insertChildPanel(DockPanelBase* panelBase)
{
    QTN_D(DockSplitLayoutPanel);
    d.insertDockPanel(panelBase);
}

void DockSplitLayoutPanel::removeChildPanel(DockPanelBase* panelBase)
{
    QTN_D(DockSplitLayoutPanel);
    if (d.m_replaceParent)
    {
        d.m_panelList.takeAt(0);
        return;
    }
    else
    {
        const int index = d.m_panelList.indexOf(panelBase);
        Q_ASSERT(index != -1);
        d.m_panelList.removeAt(index);
    }
#if 0
    if (d.m_panelList.isEmpty())
    {
        if (DockWindow* dockWindow = qobject_cast<DockWindow*>(this->dockWindow()))
        {
            //if (!d.m_replaceParent)
                dockWindow->finalizationWindow(this);
        }
        else if (qobject_cast<DockDocumentLayoutPanel*>(this) == Q_NULL)
            setParentPanel(Q_NULL);
    }
#endif
}

void DockSplitLayoutPanel::childLayoutChanged()
{
    QTN_D(DockSplitLayoutPanel);
    DockLayoutPanel::childLayoutChanged();
    d.updateSplitters();
}

void DockSplitLayoutPanel::parentLayoutChanged()
{
    DockLayoutPanel::parentLayoutChanged();
}

/*! \internal */
DockLayoutPanel* DockSplitLayoutPanel::createCopyInstance() const
{
    QTN_D(const DockSplitLayoutPanel);
    Q_ASSERT(dockManager() != Q_NULL);
    DockSplitLayoutPanel* layoutPanel = new DockSplitLayoutPanel(d.layoutEngine());
    DockSplitLayoutPanelPrivate* layoutPanelPrivate = DockSplitLayoutPanelPrivate::_get(layoutPanel);

    layoutPanelPrivate->m_horizontal = isHorizontal();
    layoutPanel->setGeometry(geometry());

    QList<DockPanelBase*> panelList = panels();
    for (QList<DockPanelBase*>::const_iterator it = panelList.begin(); it != panelList.end(); ++it)
    {
        DockLayoutPanel* curLayoutPanel = qobject_cast<DockLayoutPanel*>(*it);
        Q_ASSERT(curLayoutPanel != Q_NULL);
        if (!DockLayoutPanelPrivate::_get(curLayoutPanel)->isEmpty())
        {
            if (DockLayoutPanel* newLayoutBase = curLayoutPanel->createCopyInstance())
                newLayoutBase->setParentPanel(layoutPanel);
        }
    }
    layoutPanelPrivate->m_sizeDocking = d.m_sizeDocking;
    return layoutPanel;
}

DockLayoutPanel::LayoutType DockSplitLayoutPanel::layoutType() const
{ return DockLayoutPanel::Split; }

bool DockSplitLayoutPanel::canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center)
{
    bool result = false;
    if (DockLayoutPanel::canDock(area, dragPanel, center))
    {
        result = true;
        if (area != Qtitan::InsideDockPanelArea)
        {
            QList<DockPanelBase*> listDocs = dragPanel->findPanels(DockDocumentPanel::staticMetaObject.className());
            if (listDocs.size() > 0)
                result = false;
        }
    }
    return result;
}

bool DockSplitLayoutPanel::isHorizontal() const
{
    QTN_D(const DockSplitLayoutPanel);
    return d.m_horizontal;
}

void DockSplitLayoutPanel::setHorizontal(bool horiz)
{
    QTN_D(DockSplitLayoutPanel);
    d.m_horizontal = horiz;
}

/*! \reimp */
void DockSplitLayoutPanel::paintEvent(QPaintEvent* event)
{
    DockLayoutPanel::paintEvent(event);
#if __DEBUG_PAINTER
    QPainter p(this);
    p.fillRect(rect(), Qt::red);
#endif
}

bool DockSplitLayoutPanel::beforeByPosition(const DockPanelBase* panelBase, int pos) const
{
    QTN_D(const DockSplitLayoutPanel);
    for (int i = 0, count = d.m_panelList.count(); count > i; i++)
    {
        if (i == pos)
            return false;
        if (d.m_panelList.at(i) == panelBase)
            return true;
    }
    Q_ASSERT(false);
    return false;
}

void DockSplitLayoutPanel::replacePanel(DockPanelBase* panelBefore, DockPanelBase* panelBase)
{
    QTN_D(DockSplitLayoutPanel);
    Q_ASSERT(panelBefore != Q_NULL && panelBase != Q_NULL);
    panelBase->setParentPanel(this);
    int indexBefore = d.m_panelList.indexOf(panelBefore);
    Q_ASSERT(indexBefore != -1);
    int indexBase = d.m_panelList.indexOf(panelBase);
    Q_ASSERT(indexBase != -1);
    d.m_panelList.move(indexBase, indexBefore);
    panelBefore->setParentPanel(Q_NULL);

    //Call these layout chages directly!
    childLayoutChanged();
    panelBase->parentLayoutChanged();
}

QRect DockSplitLayoutPanel::calculatePanelsRectSelf(DockPanelBase* panelBase, Qtitan::DockPanelArea panelArea, DockPanelBase* adjacent)
{
    Q_ASSERT(adjacent != Q_NULL);
    QTN_D(DockSplitLayoutPanel);

    const bool after = (panelArea == Qtitan::RightDockPanelArea || panelArea == Qtitan::BottomDockPanelArea);

    QList<DockPanelBase*> lstPanel;
    for (QList<DockPanelBase*>::iterator it = d.m_panelList.begin(); it != d.m_panelList.end(); ++it)
    {
        DockPanelBase* curPanel = (*it);
        if (DockPanelBasePrivate::_get(curPanel)->isEmpty() || (curPanel == panelBase))
            continue;

        lstPanel.append(curPanel);

        if (curPanel == adjacent)
        {
            int index = lstPanel.indexOf(curPanel);
            if (after)
                lstPanel.insert(index + 1, panelBase);
            else
                lstPanel.insert(index, panelBase);
        }
    }

    QRect rcResult = calculatePanelsRect(d.m_horizontal, lstPanel, rect(), panelBase);
    QPoint pnt = mapToParent(rcResult.topLeft());
    pnt = parentWidget()->mapToGlobal(pnt);
    rcResult.moveTopLeft(pnt);
    return rcResult;
}

QRect DockSplitLayoutPanel::calculatePanelsRectChild(DockPanelBase* splitter, DockPanelBase* panel, Qtitan::DockPanelArea area)
{
    bool after = (area == Qtitan::RightDockPanelArea || area == Qtitan::BottomDockPanelArea);
    bool horiz = (area == Qtitan::LeftDockPanelArea || area == Qtitan::RightDockPanelArea);

    QList<DockPanelBase*> lstPanel;
    lstPanel.append(splitter);
    if (after)
        lstPanel.append(panel);
    else
        lstPanel.prepend(panel);

    QRect rcResult = splitter->rect();
    QPoint pnt = splitter->mapToParent(rcResult.topLeft());
    pnt = splitter->parentWidget()->mapToGlobal(pnt);
    rcResult.moveTopLeft(pnt);

    return calculatePanelsRect(horiz, lstPanel, rcResult, panel);
}

QRect DockSplitLayoutPanel::calculatePanelsRect(bool horiz, const QList<DockPanelBase*>& lstPanel, QRect rect, DockPanelBase* panel)
{
    DockPanelManager* manager = panel->dockManager();
    const int splitterWidth = panel->style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, Q_NULL);

    int totalLength = 0;
    int lengthAvail = 0;
    adjustPanesLength(manager, lstPanel, rect, horiz, false, totalLength, lengthAvail);

    QRect rcPanel(rect);
    for (int i = 0, count = lstPanel.count(); count > i; ++i)
    {
        DockPanelBase* curWidget = lstPanel.at(i);
        int length = -DockPanelBasePrivate::_get(curWidget)->m_length;

        if (DockPanelBasePrivate::_get(curWidget)->m_length > 0)
        {
            length = totalLength == 0 ? 0 : int((lengthAvail * DockPanelBasePrivate::_get(curWidget)->m_length) / totalLength);
            totalLength -= DockPanelBasePrivate::_get(curWidget)->m_length;
            lengthAvail = qMax(lengthAvail - length, 0);
        }

        if (horiz)
        {
            rcPanel.setRight(lstPanel.count() - 1 == i ? rect.right() : rcPanel.left() + length);
            if (panel == curWidget)
                break;
            rcPanel.setLeft(rcPanel.right() + splitterWidth);
        }
        else
        {
            rcPanel.setBottom(lstPanel.count() - 1 == i ? rect.bottom() : rcPanel.top() + length);
            if (panel == curWidget)
                break;
            rcPanel.setTop(rcPanel.bottom() + splitterWidth);
        }
    }
    return rcPanel;
}

void DockSplitLayoutPanel::adjustPanesLength(DockPanelManager* manager, const QList<DockPanelBase*>& lstPanel, 
    const QRect& rect, bool horiz, bool apply, int& totalLength, int& lengthAvail)
{
    totalLength = 0;
    lengthAvail = 0;
    DockPanelBasePrivate* curCentralPanelPrivate = Q_NULL;
    DockPanelBase* centralPanel = DockPanelManagerPrivate::_get(manager)->centralPanel();
    const int splitterWidth = centralPanel->dockManager()->managedWidget()->style()->pixelMetric(QStyle::PM_SplitterWidth, Q_NULL, Q_NULL);
    for (int i = 0, count = lstPanel.count(); count > i; ++i)
    {
        DockPanelBasePrivate* panelPrivate = DockPanelBasePrivate::_get(lstPanel.at(i));
        panelPrivate->m_length = horiz ? panelPrivate->m_sizeDocking.width() : panelPrivate->m_sizeDocking.height();

        qtnMinMaxSize mmi;
        panelPrivate->getMinMaxData(mmi);
        panelPrivate->m_length = qMax(minSize(mmi, horiz), qMin(maxSize(mmi, horiz), panelPrivate->m_length));

        if (curCentralPanelPrivate == Q_NULL && panelPrivate->containsPanel(centralPanel) != -1)
        {
            if (DockPanelManagerPrivate::_get(manager)->isCentralPanelVisible() || panelPrivate->m_sizeDocking == QSize(0, 0))
            {
                curCentralPanelPrivate = DockPanelBasePrivate::_get(lstPanel.at(i));
                if (apply)
                    curCentralPanelPrivate->m_sizeDocking = QSize(0, 0);
                curCentralPanelPrivate->m_length = 0;
            }
        }
        totalLength += panelPrivate->m_length;
    }
    lengthAvail = (horiz ? rect.width() : rect.height()) - (splitterWidth * (lstPanel.count() - 1));

    if (curCentralPanelPrivate != Q_NULL && totalLength < lengthAvail)
    {
        curCentralPanelPrivate->m_length = lengthAvail - totalLength;
        if (apply)
        {
            if (horiz)
                curCentralPanelPrivate->m_sizeDocking.setWidth(curCentralPanelPrivate->m_length);
            else
                curCentralPanelPrivate->m_sizeDocking.setHeight(curCentralPanelPrivate->m_length);
        }
        totalLength = lengthAvail;
    }

    if (lengthAvail > 0)
    {
        for (int i = 0, count = lstPanel.count(); count > i; i++)
        {
            DockPanelBasePrivate* panelPrivate = DockPanelBasePrivate::_get(lstPanel.at(i));
            if ((panelPrivate->m_length < 0) || (totalLength == 0))
                continue;

            qtnMinMaxSize mmi;
            panelPrivate->getMinMaxData(mmi);

            int length = panelPrivate->m_length;
            if (lengthAvail * length / totalLength < minSize(mmi, horiz))
                panelPrivate->m_length = -minSize(mmi, horiz);
            else if (lengthAvail * length / totalLength > maxSize(mmi, horiz))
                panelPrivate->m_length = -maxSize(mmi, horiz);

            if (panelPrivate->m_length < 0)
            {
                lengthAvail += panelPrivate->m_length;
                totalLength -= length;

                if (lengthAvail < 0)
                {
                    panelPrivate->m_length -= lengthAvail;
                    lengthAvail = 0;
                    break;
                }
                i = 0;
            }
        }
    }
}

int DockSplitLayoutPanel::minSize(qtnMinMaxSize& minmaxSize, bool horiz)
{ return horiz ? minmaxSize.minTrackSize.width() : minmaxSize.minTrackSize.height(); }

int DockSplitLayoutPanel::maxSize(qtnMinMaxSize& minmaxSize, bool horiz)
{ return horiz ? minmaxSize.maxTrackSize.width() : minmaxSize.maxTrackSize.height(); }


/* DockAreaTabBar */
DockAreaTabBar::DockAreaTabBar(QWidget* parent)
    : QTabBar(parent), m_pressedPos(cursorPositionInvalid)
{
}

DockAreaTabBar::~DockAreaTabBar()
{
}

/*! \reimp */
void DockAreaTabBar::mousePressEvent(QMouseEvent* event)
{
    QTabBar::mousePressEvent(event);
    if (event->button() != Qt::LeftButton)
        return;
    m_pressedPos = event->pos();
    grabMouse();
}

void DockAreaTabBar::mouseMoveEvent(QMouseEvent* event)
{
    if (m_pressedPos == cursorPositionInvalid)
        return;

    int distance = QApplication::startDragDistance();
    QRect r = rect().adjusted(-distance, -distance, distance, distance);
    if (r.contains(event->pos()))
    {
        QTabBar::mouseMoveEvent(event);
        event->accept();
        return;
    }
    releaseMouse();
    QPoint pos = m_pressedPos;
    m_pressedPos = cursorPositionInvalid;

    DockWidgetPanel* widgetPanel = qvariant_cast<DockWidgetPanel*>(tabData(currentIndex()));
    if (widgetPanel == Q_NULL || !widgetPanel->features().testFlag(DockWidgetPanel::DockPanelFloatable))
        return;

    DockWidgetPanelPrivate* p = DockWidgetPanelPrivate::_get(widgetPanel);
    //pos = widgetPanel->mapFromGlobal(mapToGlobal(pos));
    p->mousePress(pos, Q_NULL);
    event->accept();
}

void DockAreaTabBar::mouseReleaseEvent(QMouseEvent* event)
{
    QTabBar::mouseReleaseEvent(event);
    releaseMouse();
    m_pressedPos = cursorPositionInvalid;
}

/*! \reimp */
void DockAreaTabBar::paintEvent(QPaintEvent* event)
{
    QTabBar::paintEvent(event);
}

/* DockTabbedLayoutPanelPrivate */
DockTabbedLayoutPanelPrivate::DockTabbedLayoutPanelPrivate(DockLayoutEngine* layoutEngine, DockAreaTabBar* tabBar)
    : DockLayoutPanelPrivate(layoutEngine)
    , m_activePanel(Q_NULL)
    , m_tabBar(tabBar)
    , m_titleVisible(true)
    , m_active(false)
    , m_maximized(false)
    , m_ensureSelectedTab(false)
    , m_preserved(false)
{
}

/*! \internal */
DockTabbedLayoutPanelPrivate::~DockTabbedLayoutPanelPrivate()
{
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::init()
{
    QTN_P(DockTabbedLayoutPanel);
    DockLayoutPanelPrivate::init();
    p.setFocusPolicy(Qt::NoFocus);
    if (m_tabBar != Q_NULL)
        m_tabBar->setParent(&p);
    else
        m_tabBar = new DockAreaTabBar(&p);
    m_tabBar->setExpanding(false);
    m_tabBar->setMovable(true);
    m_tabBar->setElideMode(Qt::ElideRight);
    m_tabBar->setTabsClosable(false);
    p.setTabPosition(Qtitan::BottomArea);
    QObject::connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(slotSetCurrentTab(int)));
    QObject::connect(m_tabBar, SIGNAL(tabBarClicked(int)), this, SLOT(slotSetCurrentTab(int)));
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::updateButtons()
{
    if (layoutEngine()->isDestroying())
        return;

    if (m_panelList.count() > 0)
    {
        DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(m_panelList[0]);
        if (widgetPanel && widgetPanel->features().testFlag(DockWidgetPanel::DockPanelHideable))
        {
            bool floating = widgetPanel->isFloat();
            emit sigVisibilityAutohide(!floating);
        }
    }
    emit sigStateAutohide(isAutoHide());
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::repositionTabBar()
{
    QTN_P(DockTabbedLayoutPanel);
    if (!m_tabBar->isHidden())
    {
        const int frame = p.style()->pixelMetric(QStyle::PM_DockWidgetFrameWidth, Q_NULL, &p);
        QSize szTab = m_tabBar->sizeHint();
        QRect rcTabbed = p.rect();
        rcTabbed.moveTopLeft(QPoint(frame, 0));
        rcTabbed.setTop(rcTabbed.height() - szTab.height());
        m_tabBar->setGeometry(rcTabbed);
    }
    else
        m_tabBar->setGeometry(QRect(0, 0, 0, 0));
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::restoreFocus()
{
    QTN_P(DockTabbedLayoutPanel);
    if (m_activePanel)
        m_activePanel->setFocus();
    else
        p.setFocus();
    DockPanelManagerPrivate::_get(p.dockManager())->updateFocusPanels();
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::makeCurrentTabVisible()
{
}

/*! \internal */
bool DockTabbedLayoutPanelPrivate::isAllowMaximize() const
{
    return false;
}

/*! \internal */
bool DockTabbedLayoutPanelPrivate::isPanelMaximized() const
{
    if (!isAllowMaximize())
        return false;
    return m_maximized;
}

/*! \internal */
bool DockTabbedLayoutPanelPrivate::isPanelRestored() const
{
    if (!isAllowMaximize())
        return false;
    return !m_maximized;
}

/*! \internal */
bool DockTabbedLayoutPanelPrivate::isPanelMinimized() const
{ return false; }

/*! \internal */
void DockTabbedLayoutPanelPrivate::updatePanel(bool activateChanged)
{
    QTN_P(DockTabbedLayoutPanel);
    if (activateChanged)
    {
        QRect rect = p.rect();
        for (QList<DockPanelBase*>::Iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
        {
            DockPanelBase* curPanel = (*it);
            QRect rcPanel = m_activePanel == curPanel ? rect : QRect(0, 0, 0, 0);
            if (!rcPanel.isNull())
            {
                rcPanel.moveTopLeft(QPoint(0, 0));
                rcPanel.adjust(0, 0, 0, -m_tabBar->height() + 1);
            }
            curPanel->setVisible(!rcPanel.isEmpty());
            DockPanelBasePrivate::_get(curPanel)->setPanelGeometry(rcPanel);
        }
    }
    if (DockPanelBase* panelBase = p.parentPanel())
        DockPanelBasePrivate::_get(panelBase)->updatePanel(activateChanged);
    else if (DockWindow* dockWindow = qobject_cast<DockWindow*>(p.dockWindow()))
        DockWindowPrivate::_get(dockWindow)->updatePanel(activateChanged);
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::setPanelGeometry(const QRect& rect)
{
    QTN_P(DockTabbedLayoutPanel);
    if (m_preserved)
    {
        DockLayoutPanelPrivate::setPanelGeometry(rect);
        return;
    }
    DockLayoutPanelPrivate::setPanelGeometry(rect);
    m_tabBar->setVisible(p.isTabsVisible());
    repositionTabBar();
    setChildrenGeometry(rect);
    if (m_ensureSelectedTab)
    {
        m_ensureSelectedTab = false;
        makeCurrentTabVisible();
    }

    if (m_panelList.count() > 0)
        p.setVisible(true);
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::setChildrenGeometry(const QRect& rc)
{
    Q_UNUSED(rc);
    QTN_P(DockTabbedLayoutPanel);
    for (QList<DockPanelBase*>::const_iterator it = m_panelList.constBegin(); it != m_panelList.constEnd(); ++it)
    {
        DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(*it);
        QRect rcPanel = p.activePanel() == widgetPanel ? p.rect() : QRect(0, 0, 0, 0);
        if (!rcPanel.isNull())
            rcPanel.adjust(0, 0, 0, -m_tabBar->height() + 1);
        widgetPanel->setVisible(!rcPanel.isEmpty());
        DockPanelBasePrivate::_get(widgetPanel)->setPanelGeometry(rcPanel);
    }
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::restore()
{
    QTN_P(DockTabbedLayoutPanel);
    if (p.parentPanel() == Q_NULL)
        return;
    m_maximized = false;
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::adjustDockingSize()
{
    QTN_P(DockTabbedLayoutPanel);
    const Qtitan::LayoutArea panelArea = DockPanelManagerPrivate::_get(p.dockManager())->dockPanelArea(&p);
    const bool horiz = (panelArea == Qtitan::LeftArea) || (panelArea == Qtitan::RightArea);

    if (horiz)
        m_sizeDocking.setWidth(p.width());
    else
        m_sizeDocking.setHeight(p.height());

    for (QList<DockPanelBase*>::Iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
    {
        DockPanelBasePrivate* panelPrivate = DockPanelBasePrivate::_get(*it);
        if (!panelPrivate->isEmpty())
        {
            if (horiz)
                panelPrivate->m_sizeDocking.setWidth(m_sizeDocking.width());
            else
                panelPrivate->m_sizeDocking.setHeight(m_sizeDocking.height());
        }
    }
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::closeDockPanel(DockWidgetPanel* panel)
{
    QTN_P(DockTabbedLayoutPanel);
    p.dockManager()->closeDockPanel(panel);
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::insertDockPanel(DockPanelBase* panelBase, bool setFocus)
{
    QTN_P(DockTabbedLayoutPanel);
    m_panelList.append(panelBase);
    m_preserved = m_panelList.isEmpty();
    p.rebuildTabs();
    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panelBase))
    {
        p.setActivePanel(widgetPanel, setFocus);
        connectToTitleBar(widgetPanel, true);
    }
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::connectToTitleBar(DockWidgetPanel* widgetPanel, bool connect)
{
    QWidget* titleWidget = widgetPanel->titleBar();
    if (titleWidget == Q_NULL)
        return;
    if (connect)
    {
        QObject::connect(titleWidget, SIGNAL(closeClicked()), this, SLOT(slotCloseDockPanel()));
        QObject::connect(titleWidget, SIGNAL(autoHideClicked()), this, SLOT(slotAutoHideDockPanel()));

        QObject::connect(this, SIGNAL(sigVisibilityAutohide(bool)), titleWidget, SLOT(setAutoHideButtonVisible(bool)));
        QObject::connect(this, SIGNAL(sigStateAutohide(bool)), titleWidget, SLOT(setAutoHideButtonState(bool)));
        QObject::connect(this, SIGNAL(sigActivate(bool)), titleWidget, SLOT(setTitleActive(bool)));
        updateButtons();
    }
    else
    {
        QObject::disconnect(titleWidget, SIGNAL(closeClicked()), this, SLOT(slotCloseDockPanel()));
        QObject::disconnect(titleWidget, SIGNAL(autoHideClicked()), this, SLOT(slotAutoHideDockPanel()));

        QObject::disconnect(this, SIGNAL(sigVisibilityAutohide(bool)), titleWidget, SLOT(setAutoHideButtonVisible(bool)));
        QObject::disconnect(this, SIGNAL(sigStateAutohide(bool)), titleWidget, SLOT(setAutoHideButtonState(bool)));
        QObject::disconnect(this, SIGNAL(sigActivate(bool)), titleWidget, SLOT(setTitleActive(bool)));
    }
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::slotCloseDockPanel()
{
    QTN_P(DockTabbedLayoutPanel);
    if (DockPanelManagerPrivate::_get(dockManager())->m_closeGroupOnButtonClick)
    {
        for (QList<DockPanelBase*>::Iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
            closeDockPanel((DockWidgetPanel*)(*it));
    }
    else if (p.activePanel() != Q_NULL)
        closeDockPanel(p.activePanel());
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::slotAutoHideDockPanel()
{
    QTN_P(DockTabbedLayoutPanel);
    DockPanelManager* manager = dockManager();
    DockWidgetPanel* activePanel = p.activePanel();
    if (activePanel == Q_NULL || manager == Q_NULL)
        return;

    if (!isAutoHide())
    {
        p.parentWidget()->setFocus();
        adjustDockingSize();
        manager->setDockPanelAutoHide(activePanel, true);
    }
    else
        DockPanelManagerPrivate::_get(manager)->switchDocking(DockPanelManagerPrivate::_get(manager)->m_hideGroupOnButtonClick ? 
            static_cast<DockPanelBase *>(&p) : activePanel);
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::slotMenuButtonDockPanel()
{
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::slotSetCurrentTab(int index)
{
    QTN_P(DockTabbedLayoutPanel);
    if (m_tabBar != Q_NULL)
    {
        if (DockWidgetPanel* wPanel = qvariant_cast<DockWidgetPanel*>(m_tabBar->tabData(index)))
            p.setActivePanel(wPanel, true, true);
    }
}

/*! \internal */
bool DockTabbedLayoutPanelPrivate::isAutoHide() const
{
    QTN_P(const DockTabbedLayoutPanel);
    return qtn_checkLayoutType(p.parentPanel(), DockLayoutPanel::AutoHide);
}

/*! \internal */
int DockTabbedLayoutPanelPrivate::containsPanel(DockPanelBase* panelBase) const
{
    QTN_P(const DockTabbedLayoutPanel);
    if (panelBase == &p)
        return 1;

    for (int i = 0, count = m_panelList.count(); i < count; i++)
    {
        DockPanelBase* curPanel = m_panelList.at(i);
        if (DockPanelBasePrivate::_get(curPanel)->containsPanel(panelBase) != -1)
            return i;
    }
    return -1;
}

/*! \internal */
void DockTabbedLayoutPanelPrivate::getMinMaxData(qtnMinMaxSize& minmaxSize) const
{
    DockPanelBasePrivate::getMinMaxData(minmaxSize);
    QTN_P(const DockTabbedLayoutPanel);
    if (DockPanelBasePrivate::_get(&p)->isEmpty())
        return;

    if (isPanelMinimized())
    {
        DockSplitLayoutPanel* parentLayout = qobject_cast<DockSplitLayoutPanel*>(p.parentPanel());
        if (parentLayout && parentLayout->isHorizontal())
            minmaxSize.maxTrackSize.setWidth(0);
        else
            minmaxSize.maxTrackSize.setHeight(0);
        p.adjustMinMaxClientSize(minmaxSize, false);
    }
    else
    {
        int minTabs = 0;
        if (p.isTabsVisible())
        {
            const int vframe = p.style()->pixelMetric(QStyle::PM_TabBarTabVSpace, Q_NULL, Q_NULL);
            minTabs = m_tabBar->height() - vframe;
        }

        for (QList<DockPanelBase*>::const_iterator it = m_panelList.begin(); it != m_panelList.end(); ++it)
        {
            qtnMinMaxSize info;
            DockPanelBasePrivate::_get(*it)->getMinMaxData(info);

            minmaxSize.minTrackSize.setWidth(qMax(minmaxSize.minTrackSize.width(), info.minTrackSize.width()));
            minmaxSize.minTrackSize.setHeight(qMax(minmaxSize.minTrackSize.height(), info.minTrackSize.height() + minTabs));

            minmaxSize.maxTrackSize.setWidth(qMin(minmaxSize.maxTrackSize.width(), info.maxTrackSize.width()));
            minmaxSize.maxTrackSize.setHeight(qMin(minmaxSize.maxTrackSize.height(), info.maxTrackSize.height()));
        }
        p.adjustMinMaxClientSize(minmaxSize);
    }
}

/* DockTabbedLayoutPanel */
DockTabbedLayoutPanel::DockTabbedLayoutPanel(DockLayoutEngine* layoutEngine)
    : DockLayoutPanel(*new DockTabbedLayoutPanelPrivate(layoutEngine))
{
}

DockTabbedLayoutPanel::DockTabbedLayoutPanel(DockTabbedLayoutPanelPrivate& dd)
    : DockLayoutPanel(dd)
{
}

DockTabbedLayoutPanel::~DockTabbedLayoutPanel()
{
}

Qtitan::LayoutArea DockTabbedLayoutPanel::tabPosition() const
{
    QTN_D(const DockTabbedLayoutPanel);
    switch (d.m_tabBar->shape())
    {
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            return Qtitan::LeftArea;
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            return Qtitan::BottomArea;
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            return Qtitan::TopArea;
        default:
            return Qtitan::RightArea;
    }
}

bool DockTabbedLayoutPanel::isPreserved() const
{
    QTN_D(const DockTabbedLayoutPanel);
    return d.m_preserved;
}

void DockTabbedLayoutPanel::setPreserved(bool preserved)
{
    QTN_D(DockTabbedLayoutPanel);
    if (d.m_preserved == preserved)
        return;
    d.m_preserved = preserved;

    if (d.m_preserved)
        setGeometry(QRect(0, 0, 0, 0));

    //Call these layout chages directly!
    if (parentPanel() != Q_NULL)
        parentPanel()->childLayoutChanged();
    parentLayoutChanged();
}

void DockTabbedLayoutPanel::setTabPosition(Qtitan::LayoutArea pos)
{
    QTN_D(DockTabbedLayoutPanel);
    QTabBar::Shape shape = QTabBar::RoundedSouth;
    switch (pos)
    {
        case Qtitan::TopArea:
            shape = QTabBar::RoundedNorth;
            break;
        case Qtitan::RightArea:
            shape = QTabBar::RoundedWest;
            break;
        case Qtitan::LeftArea:
            shape = QTabBar::RoundedEast;
            break;
        default:
            break;
    }
    d.m_tabBar->setShape(shape);
}

void DockTabbedLayoutPanel::initialization(DockWidgetPanel* widgetPanel)
{
    QTN_D(DockTabbedLayoutPanel);
    Q_ASSERT(widgetPanel != Q_NULL);
    if (widgetPanel == Q_NULL)
        return;
    d.m_sizeDocking = DockPanelBasePrivate::_get(widgetPanel)->m_sizeDocking;
    widgetPanel->setParentPanel(this);
    setActivePanel(widgetPanel);
}

DockLayoutPanel::LayoutType DockTabbedLayoutPanel::layoutType() const
{ 
    return DockLayoutPanel::Tabbed;
}

bool DockTabbedLayoutPanel::canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center)
{
    if (DockLayoutPanel::canDock(area, dragPanel, center))
    {
        DockLayoutPanel* layoutPanel = this;
        const bool documentLayout = layoutPanel && qtn_parentDocumentLayout(layoutPanel);
        if ((area == Qtitan::TopDockPanelArea || area == Qtitan::BottomDockPanelArea ||
             area == Qtitan::LeftDockPanelArea || area == Qtitan::RightDockPanelArea) && documentLayout && center)
            return true;

        QList<DockPanelBase*> listPanel;
        listPanel << dragPanel->findPanels(DockDocumentPanel::staticMetaObject.className());
        return listPanel.size() == 0;
    }
    return false;
}

bool DockTabbedLayoutPanel::isTabsVisible() const
{
    QTN_D(const DockTabbedLayoutPanel);
    DockPanelBase* parentPanel = this->parentPanel();
    DockWindowBase* dockWindow = this->dockWindow();
    return parentPanel != Q_NULL && !qobject_cast<DockAutoHideWindow*>(dockWindow) && d.m_tabBar->count() > 1;
}

void DockTabbedLayoutPanel::focusChanged()
{
    QTN_D(DockTabbedLayoutPanel);
    if (d.isEmpty() || dockManager() == Q_NULL)
        return;

    QWidget* focusWidget = QApplication::focusWidget();
    bool active = (focusWidget && (focusWidget == this || isAncestorOf(focusWidget) || 
        (focusWidget->parentWidget() && isAncestorOf(focusWidget->parentWidget()))));

    QWidget* activePopup = QApplication::activePopupWidget();
    if (activePopup && isAncestorOf(activePopup->parentWidget()))
        active = true;

    if (d.m_active != active)
    {
        d.m_active = active;
        DockPanelManagerPrivate::_get(dockManager())->changedActiveDockPanel(activePanel(), active);
    }
    else if (d.m_active && dockManager()->activeDockPanel() != activePanel())
    {
        DockPanelManagerPrivate::_get(dockManager())->changedActiveDockPanel(activePanel(), active);
    }
    emit d.sigActivate(d.m_active);
}

void DockTabbedLayoutPanel::rebuildTabs()
{
    QTN_D(DockTabbedLayoutPanel);
    if (d.layoutEngine()->isDestroying())
        return;

    const bool isBlockSignals = d.m_tabBar->blockSignals(true);
    const int curIndex = currentIndexTab();

    for (int tab_idx = d.m_tabBar->count() - 1; tab_idx >= 0; --tab_idx)
        d.m_tabBar->removeTab(tab_idx);

    for (int i = 0, count = d.m_panelList.count(); i < count; i++)
    {
        if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(d.m_panelList.at(i)))
        {
            d.m_tabBar->addTab(widgetPanel->caption());
            d.m_tabBar->setTabIcon(i, widgetPanel->icon());
            d.m_tabBar->setTabToolTip(i, widgetPanel->caption());
            d.m_tabBar->setTabData(i, QVariant::fromValue<QObject *>(widgetPanel));
        }
    }

    if (curIndex != -1)
        d.m_tabBar->setCurrentIndex(curIndex);
    d.m_tabBar->blockSignals(isBlockSignals);
}

int DockTabbedLayoutPanel::currentIndexTab() const
{
    QTN_D(const DockTabbedLayoutPanel);
    for (int i = 0, count = d.m_tabBar->count(); count > i; i++)
    {
        QVariant variant = d.m_tabBar->tabData(i);
        if (DockWidgetPanel* wp = qvariant_cast<DockWidgetPanel*>(variant))
        {
            if (wp == activePanel())
                return i;
        }
    }
    return -1;
}

void DockTabbedLayoutPanel::insertChildPanel(DockPanelBase* panel)
{
    QTN_D(DockTabbedLayoutPanel);
    d.insertDockPanel(panel);
}

void DockTabbedLayoutPanel::removeChildPanel(DockPanelBase* panelBase)
{
    QTN_D(DockTabbedLayoutPanel);
    Q_ASSERT(panelBase->parentPanel() == this);
    int index = d.m_panelList.indexOf(panelBase);
    Q_ASSERT(index != -1);
    d.m_panelList.removeAt(index);

    rebuildTabs();

    if (d.m_panelList.isEmpty())
    {
        if (d.m_active)
        {
            if (QWidget* widget = dockManager()->managedWidget())
                widget->setFocus();
            d.m_active = false;
            DockPanelManagerPrivate::_get(dockManager())->changedActiveDockPanel(activePanel(), false);
        }
        setPreserved(true);
    }
    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panelBase))
        d.connectToTitleBar(widgetPanel, false);

    if (panelBase == activePanel())
        setActivePanel(d.m_panelList.isEmpty() ? Q_NULL : qobject_cast<DockWidgetPanel*>(d.m_panelList[d.m_panelList.count() - 1]));
}

void DockTabbedLayoutPanel::childLayoutChanged()
{
    DockLayoutPanel::childLayoutChanged();
    QTN_D(DockTabbedLayoutPanel);
    d.updateButtons();
}

void DockTabbedLayoutPanel::parentLayoutChanged()
{
    DockLayoutPanel::parentLayoutChanged();
    QTN_D(DockTabbedLayoutPanel);
    d.m_titleVisible = true;
    d.updateButtons();
}

void DockTabbedLayoutPanel::updatePanelProperties(DockWidgetPanel* widgetPanel)
{
    DockLayoutPanel::updatePanelProperties(widgetPanel);
    rebuildTabs();
}

/*! \reimp */
void DockTabbedLayoutPanel::paintEvent(QPaintEvent* event)
{
    DockLayoutPanel::paintEvent(event);
#if __DEBUG_PAINTER
    QPainter p(this);
    p.fillRect(rect(), Qt::green);
#endif
}

/*! \reimp */
void DockTabbedLayoutPanel::focusInEvent(QFocusEvent* event)
{
    focusChanged();
    DockPanelBase::focusInEvent(event);
}

/*! \reimp */
void DockTabbedLayoutPanel::focusOutEvent(QFocusEvent* event)
{
    const Qt::FocusReason reason = event->reason();
    if (reason != Qt::ActiveWindowFocusReason)
        focusChanged();
    DockPanelBase::focusOutEvent(event);
}

void DockTabbedLayoutPanel::setVisibleTitle(bool visible)
{
    QTN_D(DockTabbedLayoutPanel);
    if (d.m_titleVisible != visible)
        d.m_titleVisible = visible;
}

int DockTabbedLayoutPanel::titleHeight() const
{
    if (DockWidgetPanel* widgetPanel = activePanel())
    {
        if (QWidget* titleBar = widgetPanel->titleBar())
            return titleBar->height();
    }
    return 0;
}

bool DockTabbedLayoutPanel::isTitleVisible() const
{
    QTN_D(const DockTabbedLayoutPanel);
    return d.m_titleVisible && (activePanel() && ((activePanel()->features() & DockWidgetPanel::DockPanelNoCaption) == 0));
}

void DockTabbedLayoutPanel::adjustMinMaxClientSize(qtnMinMaxSize& minmaxSize, bool caption) const
{
    Q_UNUSED(caption);
    QRect rc(0, 0, 1000, 1000);
    minmaxSize.minTrackSize.setWidth(minmaxSize.minTrackSize.width() + (1000 - rc.width()));
    minmaxSize.minTrackSize.setHeight(minmaxSize.minTrackSize.height() + (1000 - rc.height()));

    minmaxSize.maxTrackSize.setWidth(minmaxSize.maxTrackSize.width() + (1000 - rc.width()));
    minmaxSize.maxTrackSize.setHeight(minmaxSize.maxTrackSize.height() + (1000 - rc.height()));
}

/*! \internal */
DockLayoutPanel* DockTabbedLayoutPanel::createCopyInstance() const
{
    QTN_D(const DockTabbedLayoutPanel);
    Q_ASSERT(dockManager() != Q_NULL);
    DockTabbedLayoutPanel* layoutPanel = new DockTabbedLayoutPanel(d.layoutEngine());
    DockTabbedLayoutPanelPrivate* layoutPanelPrivate = DockTabbedLayoutPanelPrivate::_get(layoutPanel);

    layoutPanelPrivate->m_active = false;
    layoutPanelPrivate->m_activePanel = Q_NULL;

    layoutPanel->setGeometry(geometry());
    layoutPanelPrivate->m_sizeDocking = d.m_sizeDocking;
    layoutPanelPrivate->m_titleVisible = true;

    if (d.layoutEngine() != layoutPanelPrivate->layoutEngine())
        layoutPanelPrivate->m_maximized = d.m_maximized;

    DockWidgetPanel* activePanel = Q_NULL;
    QList<DockPanelBase*> panelList = panels();
    for (QList<DockPanelBase*>::const_iterator it = panelList.begin(); it != panelList.end(); ++it)
    {
        DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(*it);
        Q_ASSERT(widgetPanel != Q_NULL);
        if (this->activePanel() == widgetPanel && activePanel == Q_NULL)
            activePanel = widgetPanel;

        if ((widgetPanel->features() & DockWidgetPanel::NoDockPanelFeatures) == 0)
            widgetPanel->setParentPanel(layoutPanel);
    }
    if (activePanel)
        layoutPanel->setActivePanel(activePanel, false);
    return layoutPanel;
}

QString DockTabbedLayoutPanel::title() const
{
    QTN_D(const DockTabbedLayoutPanel);
    return d.m_activePanel ? d.m_activePanel->caption() : QObject::tr("");
}

bool DockTabbedLayoutPanel::isActive() const
{
    QTN_D(const DockTabbedLayoutPanel);
    return d.m_active;
}

void DockTabbedLayoutPanel::setActivePanel(DockWidgetPanel* panel, bool focus, bool update)
{
    QTN_D(DockTabbedLayoutPanel);
    if (d.m_activePanel != panel)
    {
        d.m_activePanel = panel;
        if (update)
        {
            d.m_ensureSelectedTab = true;
            d.setChildrenGeometry(rect());
        }
        else
        {
            d.updatePanel(true);
            d.makeCurrentTabVisible();
        }

        int curIndex = currentIndexTab();
        if (curIndex != -1)
            d.m_tabBar->setCurrentIndex(curIndex);
    }

    if (focus)
    {
        if (d.m_active)
            DockPanelManagerPrivate::_get(dockManager())->changedActiveDockPanel(d.m_activePanel, true);
        if (panel && !isHidden() && !panel->isAncestorOf(QApplication::focusWidget()))
            panel->setFocus();
    }
}

DockWidgetPanel* DockTabbedLayoutPanel::activePanel() const
{
    QTN_D(const DockTabbedLayoutPanel);
    return d.m_activePanel;
}

bool DockTabbedLayoutPanel::canDock(QRect& rcClient, const QPoint& pt) const
{
    QTN_D(const DockTabbedLayoutPanel);
    if (qobject_cast<DockDocumentPanel*>(d.m_activePanel))
        return false;

    QRect rcTitle(rcClient);
    if (d.m_activePanel)
    {
        QRect rcTitleBar = d.m_activePanel->titleBar() ? d.m_activePanel->titleBar()->geometry() : QRect();
        rcClient.setTop(rcClient.top() + rcTitleBar.height() + 3);
    }
    else if (QWidget* widget = parentWidget())
        rcTitle = widget->geometry();

    rcTitle.setBottom(rcClient.top());

    if (rcTitle.contains(pt))
        return true;

    if (isTabsVisible())
    {
        QRect rcTabs(rcClient);
        rcTabs.setTop(rcClient.bottom() - d.m_tabBar->height());
        if (rcTabs.contains(pt))
            return true;
    }
    return false;
}

void DockTabbedLayoutPanel::showDockPanel(DockWidgetPanel* panel, bool focus)
{
    Q_ASSERT(panel != Q_NULL);
    if (qtn_checkLayoutType(parentPanel(), DockLayoutPanel::AutoHide))
    {
        DockAutoHideLayoutPanel* autoHidePanel = static_cast<DockAutoHideLayoutPanel*>(parentPanel());
        autoHidePanel->showDockPanel(panel, focus);
    }
    else
    {
        setActivePanel(panel, focus);
        if (DockWindow* dockWindow = qobject_cast<DockWindow*>(topDockWindow()))
            dockWindow->setVisible(true); // TODO dockWindow->expand();
    }
}

/* DockAutoHideLayoutPanelPrivate */
bool DockAutoHideLayoutPanelPrivate::m_closeActiveWindow = true;
int DockAutoHideLayoutPanelPrivate::m_hoverTimeout = 250;
DockAutoHideLayoutPanelPrivate::DockAutoHideLayoutPanelPrivate(DockLayoutEngine* layoutEngine)
    : DockLayoutPanelPrivate(layoutEngine)
    , m_layoutArea(Qtitan::LeftArea)
    , m_activeWindow(Q_NULL)
    , m_tabBar(Q_NULL)
    , m_widgetPanel(Q_NULL)
    , m_rcFrameRect()
    , m_itemsLength(0)
    , m_currentIndexTab(-1)
    , m_collapseWindow(false)
{
}

DockAutoHideLayoutPanelPrivate::~DockAutoHideLayoutPanelPrivate()
{
}

void DockAutoHideLayoutPanelPrivate::init()
{
    QTN_P(DockAutoHideLayoutPanel);
    DockLayoutPanelPrivate::init();
    m_layoutArea = Qtitan::LeftArea;
    m_activeWindow = Q_NULL;
    m_tabBar = new TabWidgetAutoHideBar(&p);
    m_tabBar->setDrawBase(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    connect(m_tabBar, SIGNAL(tabBarClicked(int)), this, SLOT(activeWindow(int)));
#else
    connect(m_tabBar, SIGNAL(qtn_tabBarClicked(int)), this, SLOT(activeWindow(int)));
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
}

/*! \internal */
void DockAutoHideLayoutPanelPrivate::initWindowTimer(DockWidgetPanel* widgetPanel)
{
    m_widgetPanel = widgetPanel;
    m_timerMouseHoverDelay.stop();
    if (widgetPanel && widgetPanel->features().testFlag(DockWidgetPanel::DockPanelHoverShow))
        m_timerMouseHoverDelay.start(m_hoverTimeout, this);
}

/*! \internal */
void DockAutoHideLayoutPanelPrivate::updatePanel(bool activateChanged)
{
    QTN_P(DockAutoHideLayoutPanel);
    Q_UNUSED(activateChanged);
    p.update();
}

/*! \internal */
void DockAutoHideLayoutPanelPrivate::activeWindow(int index)
{
    QTN_P(DockAutoHideLayoutPanel);
    if (m_activeWindow == Q_NULL || m_currentIndexTab != index)
    {
        if (m_activeWindow != Q_NULL && m_currentIndexTab != index)
            p.closeActiveWindow(true);

        QVariant val = m_tabBar->tabData(index);
        if (val.isValid())
        {
            if (DockWidgetPanel* wp = qvariant_cast<DockWidgetPanel*>(val))
                p.showDockPanel(wp);
        }
    }
    else if (m_activeWindow != Q_NULL)
    {
        p.closeActiveWindow(true);
        DockPanelManager* manager = dockManager();
        manager->managedWidget()->setFocus();
    }
    m_currentIndexTab = index;
}

/*! \reimp */
void DockAutoHideLayoutPanelPrivate::timerEvent(QTimerEvent* timerEvent)
{
    QTN_P(DockAutoHideLayoutPanel);
    if (timerEvent->timerId() != m_timerMouseHoverDelay.timerId())
        return;
    m_timerMouseHoverDelay.stop();
    if (m_widgetPanel != Q_NULL)
        p.showDockPanel(m_widgetPanel);
    m_widgetPanel = Q_NULL;
}

/*! \reimp */
void TabWidgetAutoHideBar::mouseMoveEvent(QMouseEvent* event)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTabBar::mousePressEvent(event);
    const int index = tabAt(event->pos());
    emit qtn_tabBarClicked(index);
#else
    QTabBar::mouseMoveEvent(event);
    if (!event->isAccepted())
    {
        DockWidgetPanel* widgetPanel = Q_NULL;
        int index = tabAt(event->pos());
        if (index != -1)
        {
            QVariant val = tabData(index);
            if (val.isValid())
                widgetPanel = qvariant_cast<DockWidgetPanel*>(val);
        }
        DockAutoHideLayoutPanelPrivate::_get(m_autoHideLayout)->initWindowTimer(widgetPanel);
    }
#endif // (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
}

/*! \reimp */
void TabWidgetAutoHideBar::leaveEvent(QEvent* event)
{
    QTabBar::leaveEvent(event);
    DockAutoHideLayoutPanelPrivate::_get(m_autoHideLayout)->initWindowTimer(Q_NULL);
}


/* DockAutoHideLayoutPanel */
DockAutoHideLayoutPanel::DockAutoHideLayoutPanel(DockLayoutEngine* layoutEngine)
    : DockLayoutPanel(*new DockAutoHideLayoutPanelPrivate(layoutEngine))
{
}

DockAutoHideLayoutPanel::~DockAutoHideLayoutPanel()
{
}

DockAutoHideWindow* DockAutoHideLayoutPanel::activeWindow() const
{
    QTN_D(const DockAutoHideLayoutPanel);
    return d.m_activeWindow;
}

void DockAutoHideLayoutPanel::setActiveWindow(DockAutoHideWindow* window)
{
    QTN_D(DockAutoHideLayoutPanel);
    d.m_activeWindow = window;
}

Qtitan::LayoutArea DockAutoHideLayoutPanel::layoutArea() const
{
    QTN_D(const DockAutoHideLayoutPanel);
    return d.m_layoutArea;
}

void DockAutoHideLayoutPanel::setLayoutArea(Qtitan::LayoutArea area)
{
    QTN_D(DockAutoHideLayoutPanel);
    d.m_layoutArea = area;
}

DockLayoutPanel::LayoutType DockAutoHideLayoutPanel::layoutType() const
{
    return DockLayoutPanel::AutoHide; 
}

bool DockAutoHideLayoutPanel::canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center)
{ 
    return DockLayoutPanel::canDock(area, dragPanel, center);
}

void DockAutoHideLayoutPanel::updatePanelProperties(DockWidgetPanel* widgetPanel)
{
    DockLayoutPanel::updatePanelProperties(widgetPanel);
    rebuildTabs();
}

void DockAutoHideLayoutPanel::insertChildPanel(DockPanelBase* panelBase)
{
    QTN_D(DockAutoHideLayoutPanel);
    if (d.m_collapseWindow)
        return;

    d.m_panelList.append(panelBase);
 
    if (qtn_checkLayoutType(panelBase, DockLayoutPanel::Tabbed))
    {
        QWidget* focus = focusWidget();
        if (focus == ((DockTabbedLayoutPanel*)panelBase) || ((DockTabbedLayoutPanel*)panelBase)->isAncestorOf(focus))
            dockManager()->managedWidget()->setFocus();
        panelBase->setVisible(false);
    }
// TODO  rebuildTabs() is called from setParentPanel()
}

void DockAutoHideLayoutPanel::removeChildPanel(DockPanelBase* panelBase)
{
    QTN_D(DockAutoHideLayoutPanel);
    if (d.m_collapseWindow)
        return;
    d.m_panelList.removeOne(panelBase);
    rebuildTabs();
}

/*! \internal */
void DockAutoHideLayoutPanel::childLayoutChanged()
{
    DockLayoutPanel::childLayoutChanged();
    QTN_D(DockAutoHideLayoutPanel);
    rebuildTabs();
    if (d.m_activeWindow != Q_NULL)
    {
        if (!d.m_closeActiveWindow)
            d.m_activeWindow->recalcLayout();
        else
            closeActiveWindow(DockPanelManagerPrivate::_get(dockManager())->attachingDockWidget());
    }
}

void DockAutoHideLayoutPanel::setCollapseWindow(DockTabbedLayoutPanel* tabbedPanel)
{
    QTN_D(DockAutoHideLayoutPanel);
    Q_ASSERT(tabbedPanel != Q_NULL);
    d.m_collapseWindow = true;
    tabbedPanel->setDockWindow(Q_NULL);
    tabbedPanel->setParentPanel(this);
    tabbedPanel->setHidden(true);
    d.m_collapseWindow = false;
}

void DockAutoHideLayoutPanel::closeActiveWindow(bool delayed)
{
    QTN_D(DockAutoHideLayoutPanel);
    if (!DockAutoHideLayoutPanelPrivate::m_closeActiveWindow)
        return;

    if (d.m_activeWindow != Q_NULL)
    {
        if (d.m_activeWindow->widgetTabbed() && d.m_activeWindow->widgetTabbed()->activePanel() != Q_NULL)
        {
            // signal Collapsing
            // signal Collapsed
        }

        if (delayed)
        {
            if (DockTabbedLayoutPanel* widgetTabbed = d.m_activeWindow->widgetTabbed())
            {
                widgetTabbed->setDockWindow(Q_NULL);
                d.m_activeWindow->clearWidgetTabbed();
            }
            d.m_activeWindow->startCloseTimer();
        }
        else
            d.m_activeWindow->closeWindow();
        d.m_activeWindow = Q_NULL;
    }
}

void DockAutoHideLayoutPanel::closeActiveWindows()
{
    QTN_D(DockAutoHideLayoutPanel);
    for (int i = 0; i < 4; i++)
    {
        DockLayoutEngine* layout = d.layoutEngine();
        if (layout->m_wndAutoHide[i])
            layout->m_wndAutoHide[i]->closeActiveWindow();
    }
}

void DockAutoHideLayoutPanel::showDockPanel(DockWidgetPanel* panel, bool focus)
{
    QTN_D(DockAutoHideLayoutPanel);
    if (d.m_activeWindow != Q_NULL && 
        d.m_activeWindow->widgetTabbed() != Q_NULL && 
        d.m_activeWindow->widgetTabbed()->activePanel() == panel)
    {
        if (focus)
            d.m_activeWindow->widgetTabbed()->setActivePanel(panel, true, false);

        if (d.m_activeWindow->isSlideOut())
            d.m_activeWindow->timerCheckActive();
        return;
    }

    closeActiveWindows();

    Q_ASSERT(qtn_checkLayoutType(panel->parentPanel(), DockLayoutPanel::Tabbed));
    DockTabbedLayoutPanel* tabbedLayout = static_cast<DockTabbedLayoutPanel*>(panel->parentPanel());
    d.m_activeWindow = new DockAutoHideWindow(this, tabbedLayout);
    tabbedLayout->setActivePanel(panel, focus, true);
}

int DockAutoHideLayoutPanel::dockHeight() const
{
    QTN_D(const DockAutoHideLayoutPanel);
    int height = 0;
    QSize sz = d.m_tabBar->sizeHint();
    if (d.m_layoutArea == Qtitan::LeftArea || d.m_layoutArea == Qtitan::RightArea)
        height = sz.width();
    else if (d.m_layoutArea == Qtitan::TopArea || d.m_layoutArea == Qtitan::BottomArea)
        height = sz.height();
    return height;
}

bool DockAutoHideLayoutPanel::isCloseActiveWindow()
{
    return DockAutoHideLayoutPanelPrivate::m_closeActiveWindow;
}

QRect DockAutoHideLayoutPanel::setPanelGeometryEx(const QRect& rect)
{
    QTN_D(DockAutoHideLayoutPanel);
    QRect retRect = rect;
    if (d.isEmpty())
    {
        setVisible(false);
        return retRect;
    }

    if (isHidden())
    {
        if (parentWidget() == Q_NULL)
            setParent(dockManager()->managedWidget());
        setVisible(true);
    }

    if (d.m_rcFrameRect != retRect)
    {
        d.m_rcFrameRect = retRect;
        if (d.m_activeWindow != Q_NULL)
            d.m_activeWindow->updateLayout();
    }

    int height = dockHeight();
    QRect rcClient(retRect);
    switch (d.m_layoutArea)
    {
        case Qtitan::LeftArea:
            rcClient.setRight(retRect.left() + height);
            retRect.setLeft(retRect.left() + height);
            break;
        case Qtitan::RightArea:
            rcClient.setLeft(retRect.right() - height);
            retRect.setRight(retRect.right() - height);
            break;
        case Qtitan::TopArea:
            rcClient.setBottom(retRect.top() + height);
            retRect.setTop(retRect.top() + height);
            break;
        case Qtitan::BottomArea:
            rcClient.setTop(retRect.bottom() - height);
            retRect.setBottom(retRect.bottom() - height);
            break;
        default:
            break;
    }
    QRect rcPanel(rcClient);
    rcPanel.translate(-rcPanel.topLeft());
    updateGeometryTabBar(rcPanel);
    d.setPanelGeometry(rcClient);
    return retRect;
}

void DockAutoHideLayoutPanel::updateGeometryTabBar(const QRect& rc)
{
    QTN_D(DockAutoHideLayoutPanel);
    QRect rcClient = rc;
    DockPanelManager* manager = dockManager();
    DockLayoutEngine* engine = DockPanelManagerPrivate::_get(manager)->dockLayoutEngine();
    bool vertical = d.m_layoutArea == Qtitan::LeftArea || d.m_layoutArea == Qtitan::RightArea;
    bool indent = vertical && (engine->m_wndAutoHide[qtn_areaToIndex(Qtitan::TopArea)] != Q_NULL &&
        !DockPanelBasePrivate::_get(engine->m_wndAutoHide[qtn_areaToIndex(Qtitan::TopArea)])->isEmpty());

    int widgetIndent = indent ? dockHeight() : 0;
    rcClient.setTop(rcClient.top() + widgetIndent);
    rcClient.setLeft(rcClient.left());

    if (vertical)
        rcClient.setTop(rcClient.top() + manager->margin());
    else
        rcClient.setLeft(rcClient.left() + manager->margin());
    d.m_tabBar->setGeometry(rcClient);
}

void DockAutoHideLayoutPanel::rebuildTabs()
{
    QTN_D(const DockAutoHideLayoutPanel);
    QTabBar::Shape shape = QTabBar::RoundedWest;
    switch (d.m_layoutArea)
    {
        case Qtitan::LeftArea:
            shape = QTabBar::RoundedEast;
            break;
        case Qtitan::BottomArea:
            shape = QTabBar::RoundedNorth;
            break;
        case Qtitan::TopArea:
            shape = QTabBar::RoundedSouth;
            break;
        default:
            break;
    }
    d.m_tabBar->setShape(shape);
    d.m_tabBar->setExpanding(false);
    d.m_tabBar->setMovable(true);
    d.m_tabBar->setElideMode(Qt::ElideRight);
    for (int i = (int)d.m_tabBar->count() - 1; i >= 0; i--)
        d.m_tabBar->removeTab(i);

    for (int i = 0, count = d.m_panelList.count(); count > i; i++)
    {
        DockTabbedLayoutPanel* tabbed = (DockTabbedLayoutPanel*)d.m_panelList.at(i);
        if (DockPanelBasePrivate::_get(tabbed)->isEmpty())
            continue;
        for (int j = 0, countJ = tabbed->panels().count(); countJ > j; j++)
        {
            DockWidgetPanel* panel = (DockWidgetPanel*)tabbed->panels().at(j);
            int index = d.m_tabBar->addTab(panel->caption());
            d.m_tabBar->setTabIcon(index, panel->icon());
            d.m_tabBar->setTabData(index, QVariant::fromValue<QObject*>(panel));
        }
    }
    if (d.m_currentIndexTab != -1)
        d.m_tabBar->setCurrentIndex(d.m_currentIndexTab);
}

/*! \reimp */
void DockAutoHideLayoutPanel::paintEvent(QPaintEvent* event)
{
    DockLayoutPanel::paintEvent(event);
#if __DEBUG_PAINTER
    QPainter p(this);
    p.fillRect(rect(), Qt::blue);
#endif
}
