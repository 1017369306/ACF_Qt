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
#include <QEvent>
#include <QStyleOption>
#include <qevent.h>
#include <QMenu>

#include "QtnDockBarSitePrivate.h"
#include "QtnDockBarBasePrivate.h"
#include "QtnDockToolBarPrivate.h"
#include "QtnDockToolBarLayoutPrivate.h"
#include "QtnDockBarManagerPrivate.h"
#include "QtnDockMainWindowLayout.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


DockBarSitePrivate::DockBarSitePrivate()
    : m_manager(Q_NULL)
    , m_layoutArea(Qtitan::TopArea)
    , m_orientation(Qt::Horizontal)
    , m_layout(Q_NULL)
{
}

DockBarSitePrivate::~DockBarSitePrivate()
{
}

void DockBarSitePrivate::initialization(Qtitan::LayoutArea area)
{
    QTN_P(DockBarSite);
    m_listBar.append(Q_NULL);
    m_layoutArea = area;
    m_orientation = (area == Qtitan::TopArea) || (area == Qtitan::BottomArea) ? Qt::Horizontal : Qt::Vertical;

    QString strBarAreaName = area == Qtitan::TopArea ? QStringLiteral("BarTop") : area == Qtitan::LeftArea ?
        QStringLiteral("BarLeft") : area == Qtitan::BottomArea ? QStringLiteral("BarBottom") : QStringLiteral("BarRight");
    p.setObjectName(strBarAreaName);
    m_layout = qobject_cast<DockMainWindowLayout*>(p.parentWidget()->layout());
}

static void calculateSumWidth(DockBarSitePrivate::DockDataVector& vecDockData, int height, bool horizontal)
{
    int sumWidth = 0;
    for (int i = 0, count = vecDockData.size(); i < count; i++)
    {
        DockBarSitePrivate::DockData& data = vecDockData[i];
        data.minWidth = horizontal ? data.bar->sizeHint().width() : (height > 0 ? height : data.bar->sizeHint().height());
        data.totalMinWidth = sumWidth;
        sumWidth += data.minWidth;
    }
}

int DockBarSitePrivate::adjustWidthForLine(QList<DockBarBase*>& listLine, const QPoint& pt, int length, bool horizontal, int& remove)
{
    DockDataVector vecDockData;
    collectSortData(listLine, pt, horizontal, vecDockData);

    int lenResult = calculateLength(vecDockData, length);

    if (lenResult > length)
    {
        ::calculateSumWidth(vecDockData, DockBarManagerPrivate::_get(m_manager)->calcMaxHeightBars(), horizontal);
        lenResult = length;
        for (int i = vecDockData.size() - 1; i >= 0; i--)
        {
            DockData& data = vecDockData[i];

            QRect& rect = data.rectBar;
            int left = horizontal ? data.rectPos.left() : data.rectPos.top();

            if (left > rect.left()) 
                left = rect.left();
            if (left < lenResult - rect.width())
                left = lenResult - rect.width();

            if (left < data.totalMinWidth)
                left = data.totalMinWidth;
            if (lenResult - left < data.minWidth)
                left = lenResult - data.minWidth;

            if ((lenResult - left < data.minWidth || left < data.totalMinWidth) && vecDockData.size() != 1)
            {
                remove = vecDockData[vecDockData.size() - 1].index;
                return -1;
            }
            rect.setRight(lenResult);
            lenResult = qMax(0, left);
            rect.setLeft(qMax(0, left));

            if (data.bar != Q_NULL)
            {
                if (DockToolBarLayout* tblayout = qobject_cast<DockToolBarLayout*>(data.bar->layout()))
                    tblayout->checkUsePopupMenu();
            }
        }
    }

    if (!horizontal)
        vecDockData.invertRects();

    int width = calculateMaximumLength(vecDockData);
    updateGeometryBarsByData(vecDockData);
    return width;
}

int DockBarSitePrivate::adjustWidthForLine(QList<DockBarBase*>& listLine, const QPoint& pnt, int length, bool horz)
{
    QPoint pt = pnt;

    int sumWidth = 0;
    QList<DockBarBase*> arrNext;

    for (;;)
    {
        int remove = -1;
        Q_ASSERT(listLine.size() > 0);
        int width = adjustWidthForLine(listLine, pt, length, horz, remove);

        if (width == -1)
        {
            Q_ASSERT(remove != -1);
            arrNext.append(listLine[remove]);
            listLine.removeAt(remove);
        }
        else
        {
            sumWidth += width;
            if (horz)
                pt.setY(pt.y() + width);
            else
                pt.setX(pt.x() + width);

            if (arrNext.size() > 0)
            {
                listLine = arrNext;
                arrNext.clear();
                continue;
            }
            break;
        }
    }
    return sumWidth;
}

int DockBarSitePrivate::findControlBar(DockBarBase* bar, int indexExclude)
{
    for (int i = 0, count = m_listBar.size(); i < count; i++)
    {
        if (i != indexExclude && m_listBar[i] == bar)
            return i;
    }
    return -1;
}

int DockBarSitePrivate::insertControlBar(DockBarBase* pBarIns, QPoint pntMid)
{
    Q_ASSERT(pBarIns != Q_NULL);
    int width = 0;
    int totalWidth = 0;
    int idxInsertAfter = 0;
    bool enableTopPart = true;//m_layout->menuBar() == Q_NULL/* || findBar(m_layout->menuBar()) == -1*/;

    for (int i = 0, count = m_listBar.size(); count > i; i++)
    {
        DockBarBase* bar = m_listBar.at(i);
        if (bar && bar->isHidden())
            continue;

        if (bar != Q_NULL)
        {
            QRect rectBar = bar->geometry();
            width = qMax(width, isHorizontal() ? rectBar.bottom() : rectBar.right());
        }
        else
        {
            if ((isHorizontal() ? pntMid.y() : pntMid.x()) < width && (enableTopPart || idxInsertAfter > 1))
            {
                if (i == 0 || ((isHorizontal() ? pntMid.y() : pntMid.x()) == totalWidth))
                    m_listBar.insert(idxInsertAfter + 1, (DockBarBase*)Q_NULL);
                m_listBar.insert(idxInsertAfter + 1, pBarIns);
                return idxInsertAfter + 1;
            }
            totalWidth = width;
            width = 0;
            idxInsertAfter = i;
        }
    }
    m_listBar.insert(idxInsertAfter + 1, (DockBarBase*)Q_NULL);
    m_listBar.insert(idxInsertAfter + 1, pBarIns);
    return idxInsertAfter + 1;
}

void DockBarSitePrivate::updateLayout(const QRect& rectLayout)
{
    QTN_P(DockBarSite);
    if (!p.isHidden())
    {
        QRect rect = rectLayout;
        QSize sizeAvail = rect.size();

        int length = isHorizontal() ? sizeAvail.width() : sizeAvail.height();

        QSize size = updateLayout(length, rect);

        size.setWidth(qMin(size.width(), sizeAvail.width()));
        size.setHeight(qMin(size.height(), sizeAvail.height()));

        if (isHorizontal())
        {
            if (m_layoutArea == Qtitan::TopArea)
            {
            }
            else if (m_layoutArea == Qtitan::BottomArea)
            {
                rect.setTop(rect.bottom() - size.height());
            }
        }
        else
        {
            if (m_layoutArea == Qtitan::LeftArea)
            {
            }
            else if (m_layoutArea == Qtitan::RightArea)
            {
                rect.setLeft(rect.right() - size.width());
            }
        }

        rect.setRight(rect.left() + size.width());
        rect.setBottom(rect.top() + size.height());
        rect.setWidth(size.width());
        rect.setHeight(size.height());
        p.setGeometry(rect);
    }
}

QSize DockBarSitePrivate::updateLayout(int length, QRect& rect)
{
    Q_UNUSED(rect)
    bool stretch = 1;
    QSize sizeFixed(stretch && isHorizontal() ? QTN_INT16_MAX : 0, stretch && !isHorizontal() ? QTN_INT16_MAX : 0);

    QPoint pnt(0, 0);
    for (int i = 0, count = m_listBar.count(); i < count; i++)
    {
        QList<DockBarBase*> lstControlBar;
        DockBarBase* bar = m_listBar.at(i);

        while (bar)
        {
            if (!bar->isHidden())
                lstControlBar.append(bar);
            bar = m_listBar.at(++i);
        }

        int width = 0;
        if (lstControlBar.size() != 0)
        {
            width = adjustWidthForLine(lstControlBar, pnt, length, isHorizontal());
            lstControlBar.clear();
        }
        if (bar == Q_NULL && width != 0)
        {
            if (isHorizontal())
            {
                pnt.setY(pnt.y() + width);
                sizeFixed.setHeight(qMax(sizeFixed.height(), pnt.y()));
            }
            else
            {
                pnt.setX(pnt.x() + width);
                sizeFixed.setWidth(qMax(sizeFixed.width(), pnt.x()));
            }
        }
    }
    return sizeFixed;
}

void DockBarSitePrivate::adjustExpandBars()
{
/*
    QTN_P(DockBarSite);
    for (int index = 0, count = m_listBar.size(); index < count; index++)
    {
        DockBarBase* controlBar = p.dockedToolBar(index);
        if (controlBar && controlBar->feature() & flagExtended)
        {
            if (index > 0 && p.dockedToolBar(index - 1) != Q_NULL)
            {
                m_listBar.insert(index, (DockBarBase*)Q_NULL);
                index++;
            }
            if (index < m_listBar.size() - 1 && p.dockedToolBar(index + 1) != Q_NULL)
            {
                m_listBar.insert(index + 1, (DockBarBase*)Q_NULL);
                index++;
            }
        }
    }
*/
}

void DockBarSitePrivate::setDockBarManager(DockBarManager* manager) 
{ 
    m_manager = manager; 
}

void DockBarSitePrivate::collectSortData(const QList<DockBarBase*>& listLine, const QPoint& pt, bool horizontal, DockDataVector& vecDockData)
{
    for (int i = 0, count = listLine.size(); i < count; i++)
    {
        DockBarBase* bar = listLine[i];
        QSize sz = bar->sizeHint();
        if (horizontal)
            sz.setHeight(DockBarManagerPrivate::_get(m_manager)->calcMaxHeightBars());
        DockData dockData(bar, QRect(QPoint(horizontal ? QPoint(DockBarBasePrivate::_get(bar)->m_rectDockPos.left(), pt.y()) :
            QPoint(pt.x(), DockBarBasePrivate::_get(bar)->m_rectDockPos.top())), sz), i);
        vecDockData.append(dockData);
    }
    Q_ASSERT(vecDockData.size() == listLine.size());
    if (!horizontal)
        vecDockData.invertRects();
    vecDockData.sortVector();
}

int DockBarSitePrivate::calculateLength(DockDataVector& vecDockData, int length) const
{
    int index = -1;
    bool move = true;
    int lenResult = 0;
    for (int i = 0, count = vecDockData.size(); i < count; i++)
    {
        DockData& data = vecDockData[i];
        QRect& rect = data.rectBar;

        move = (rect.left() < lenResult && (index < data.index || move));
        if (move)
            rect.translate(lenResult - rect.left(), 0);
        lenResult = rect.right();
        index = data.index;
    }

    lenResult = length;
    index = -1;
    move = true;
    for (int i = vecDockData.size() - 1; i >= 0; i--)
    {
        DockData& data = vecDockData[i];

        QRect& rect = data.rectBar;
        move = (rect.right() - lenResult > 0 && (index < data.index || move));
        if (move)
            rect.translate(lenResult - rect.right(), 0);

        lenResult = rect.left();
        index = data.index;
    }

    lenResult = 0;
    for (int i = 0, count = vecDockData.size(); i < count; i++)
    {
        QRect& rect = vecDockData[i].rectBar;
        if (rect.left() < lenResult)
            rect.translate(lenResult - rect.left(), 0);
        lenResult = rect.left() + rect.width();
    }
    return lenResult;
}

int DockBarSitePrivate::calculateMaximumLength(const DockDataVector& vecDockData) const
{
    int width = 0;
    for (int i = 0, count = vecDockData.size(); i < count; i++)
    {
        QSize sizeBar = vecDockData[i].bar->sizeHint();
        if (isHorizontal())
            sizeBar.setHeight(DockBarManagerPrivate::_get(m_manager)->calcMaxHeightBars());
        width = qMax(width, isHorizontal() ? sizeBar.height() : sizeBar.width());
    }
    return width;
}

void DockBarSitePrivate::updateGeometryBarsByData(const DockDataVector& vecDockData) const
{
    QTN_P(const DockBarSite);
    for (int i = 0, count = vecDockData.size(); i < count; i++)
    {
        const DockData& data = vecDockData[i];
        DockBarBase* bar = data.bar;
        DockBarBasePrivate::_get(bar)->m_dockBarArea = p.dockBarArea();
        if (data.rectBar != bar->geometry())
            bar->setGeometry(data.rectBar);
    }
}


/* DockBarSite */
DockBarSite::DockBarSite(Qtitan::LayoutArea pos, QWidget* parent)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(DockBarSite);
    QTN_D(DockBarSite);
    d.initialization(pos);
}

DockBarSite::~DockBarSite()
{
    QTN_FINI_PRIVATE();
}

QSize DockBarSite::sizeHint() const
{
    if (dockedVisibleCount() > 0)
    {
        QStyleOption opt;
        opt.initFrom(this);
        return opt.rect.size();
    }
    return QSize(0, 0);
}

void DockBarSite::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOptionFrame opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_FrameDockWidget, &opt, &p, this);
#if 0
    p.fillRect(rect(), Qt::red);
#endif
}

void DockBarSite::addDockBar(DockBarBase* bar, QRect* ptRect)
{
    QTN_D(DockBarSite);
    QRect rectBar = bar->geometry();
    DockBarBasePrivate* dd = DockBarBasePrivate::_get(bar);

    if (dd->m_dockSite == this && (ptRect == Q_NULL || rectBar == *ptRect))
        return;

    int index = -1;
    if (ptRect != Q_NULL)
    {
        QRect rect(*ptRect);
        QPoint pos = mapFromGlobal(rect.topLeft());
        rect.moveTopLeft(pos);
        QPoint pntMid = rect.center();
        index = d.insertControlBar(bar, pntMid);
        dd->m_rectDockPos = rect;
    }
    else
    {
        d.m_listBar.append(bar);
        d.m_listBar.append(Q_NULL);
        dd->m_rectDockPos = QRect();
    }

    if (bar->parent() != this)
        bar->setParent(this);

    if (dd->m_dockSite == this)
        dd->m_dockSite->removeDockBar(bar, index);
    else if (dd->m_dockSite != Q_NULL)
        dd->m_dockSite->removeDockBar(bar, -1);
    dd->m_dockSite = this;

    d.adjustExpandBars();
    if (d.m_layout != Q_NULL)
        d.m_layout->invalidate();
}

bool DockBarSite::removeDockBar(DockBarBase* bar, int indexExclude)
{
    QTN_D(DockBarSite);
    Q_ASSERT(bar != Q_NULL);
    if (!bar)
        return false;

    int pos = d.findControlBar(bar, indexExclude);
    Q_ASSERT(pos > 0);
    if (pos <= 0)
        return false;

    d.m_listBar.removeAt(pos);
    if (d.m_listBar[pos-1] == Q_NULL && d.m_listBar[pos] == Q_NULL)
        d.m_listBar.removeAt(pos);
    DockBarBasePrivate::_get(bar)->m_dockSite = Q_NULL;
//    if (d.m_layout != Q_NULL)
//        d.m_layout->invalidate();
    return true;
}

DockBarBase* DockBarSite::dockedToolBar(int index) const
{
    QTN_D(const DockBarSite);
    return d.m_listBar.at(index);
}

int DockBarSite::dockedCount() const
{
    QTN_D(const DockBarSite);
    int count = 0;
    for (int i = 0, size = d.m_listBar.size(); i < size; ++i)
    {
        if (d.m_listBar.at(i) != Q_NULL)
            count++;
    }
    return count;
}

int DockBarSite::dockedVisibleCount() const
{
    QTN_D(const DockBarSite);
    int count = 0;
    for (int i = 0, size = d.m_listBar.size(); i < size; ++i)
    {
        DockBarBase* barBase = d.m_listBar.at(i);
        if (barBase != Q_NULL && !barBase->isHidden())
            count++;
    }
    return count;
}

Qtitan::DockBarArea DockBarSite::dockBarArea() const
{
    QTN_D(const DockBarSite);
    if (d.m_layoutArea == Qtitan::TopArea)    return Qtitan::DockBarTop;
    if (d.m_layoutArea == Qtitan::BottomArea) return Qtitan::DockBarBottom;
    if (d.m_layoutArea == Qtitan::LeftArea)   return Qtitan::DockBarLeft;
    return Qtitan::DockBarRight;
}

Qt::Orientation DockBarSite::orientation() const
{
    QTN_D(const DockBarSite);
    return d.m_orientation;
}

void DockBarSite::contextMenuEvent(QContextMenuEvent* event)
{
    QTN_D(DockBarSite);
    QMenu* menu = d.m_layout ? d.m_layout->createToolBarsContextMenu() : Q_NULL;
    if (!menu)
        return;

    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->popup(event->globalPos());
}

void DockBarSite::visibleToolbars(int pos, QList<DockBarBase*>& listBar)
{
    QTN_D(DockBarSite);
    Q_ASSERT(pos != -1);
    Q_ASSERT(d.m_listBar[pos]);

    while (d.m_listBar[pos] != Q_NULL && pos > 0)
        pos--;
    pos++;

    DockBarSitePrivate::DockDataVector vecDockData;

    while (d.m_listBar[pos] != Q_NULL)
    {
        DockBarBase* bar = d.m_listBar[pos++];
        if (!bar->isHidden())
        {
            DockBarSitePrivate::DockData dockInfo(bar, bar->geometry(), pos);
            vecDockData.append(dockInfo);
        }
    }

    if (d.m_layoutArea == Qtitan::LeftArea || d.m_layoutArea == Qtitan::RightArea)
        vecDockData.invertRects();
    vecDockData.sortVector();

    for (int i = 0; i < vecDockData.size(); i++)
        listBar.append(vecDockData[i].bar);
}
