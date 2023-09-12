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

#include <qaction.h>
#include <qwidgetaction.h>
#include <qstyleoption.h>
#include <qmenu.h>
#include <qdebug.h>
#include <qmath.h>
#include <QPainter>
#include <QApplication>

#include "QtnDockMainWindow.h"
#include "QtnDockToolBar.h"
#include "QtnDockBarSite.h"
#include "QtnDockToolBarPrivate.h"
#include "QtnDockTitleBar.h"
#include "QtnDockToolBarExtensionPrivate.h"
#include "QtnDockToolBarLayoutPrivate.h"
#include "QtnDockMainWindowLayout.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

static inline qint64 toFixed(int i) { return static_cast<qint64>(i) * 256; }
static inline int fRound(qint64 i) {
    return (i % 256 < 128) ? i / 256 : 1 + i / 256;
}

static inline int pick(Qt::Orientation o, const QPoint& pos)
{ return o == Qt::Horizontal ? pos.x() : pos.y(); }

static inline int pick(Qt::Orientation o, const QSize& size)
{ return o == Qt::Horizontal ? size.width() : size.height(); }

static inline int& rpick(Qt::Orientation o, QPoint& pos)
{ return o == Qt::Horizontal ? pos.rx() : pos.ry(); }

static inline int& rpick(Qt::Orientation o, QSize& size)
{ return o == Qt::Horizontal ? size.rwidth() : size.rheight(); }

static inline int perp(Qt::Orientation o, const QPoint &pos)
{ return o == Qt::Vertical ? pos.x() : pos.y(); }

static inline int perp(Qt::Orientation o, const QSize &size)
{ return o == Qt::Vertical ? size.width() : size.height(); }

static inline int &rperp(Qt::Orientation o, QPoint &pos)
{ return o == Qt::Vertical ? pos.rx() : pos.ry(); }

static inline int &rperp(Qt::Orientation o, QSize &size)
{ return o == Qt::Vertical ? size.rwidth() : size.rheight(); }

static QVarLengthArray<int, 32> qtnGatherMinimumSizes(QVector<GeomLayout>& chain, int start, int count )
{
    QVarLengthArray<int, 32> minimumSizes;
    minimumSizes.reserve(count);
    for (int i = start; i < start + count; i++)
        minimumSizes << chain.at(i).minimumSize;
    std::sort(minimumSizes.begin(), minimumSizes.end());
    return minimumSizes;
}

static void qtnGeomCompute(QVector<GeomLayout>& chain, int start, int count, int pos, int space, int spacer)
{
    int szHint = 0;
    int szMin = 0;
    int szMax = 0;
    int sumStretch = 0;
    int sumSpacing = 0;
    int expandingCount = 0;

    bool allEmptyNonstretch = true;
    int pendingSpacing = -1;
    int spacerCount = 0;
    int i = 0;

    for (i = start; i < start + count; i++) 
    {
        GeomLayout* data = &chain[i];
        data->done = false;
        szHint += data->suitableSizeHint();
        szMin += data->minimumSize;
        szMax += data->maximumSize;
        sumStretch += data->stretch;
        if (!data->empty) 
        {
            if (pendingSpacing >= 0) 
            {
                sumSpacing += pendingSpacing;
                ++spacerCount;
            }
            pendingSpacing = data->bestSpacer(spacer);
        }
        if (data->expansive)
            expandingCount++;
        allEmptyNonstretch = allEmptyNonstretch && data->empty && !data->expansive && data->stretch <= 0;
    }

    int extraspace = 0;

    if (space < szMin + sumSpacing) 
    {
        int minSize = szMin + sumSpacing;
        if (spacer >= 0) 
        {
            spacer = minSize > 0 ? spacer * space / minSize : 0;
            sumSpacing = spacer * spacerCount;
        }
        QVarLengthArray<int, 32> minimumSizes = qtnGatherMinimumSizes(chain, start, count);
        int spaceLeft = space - sumSpacing;

        int sum = 0;
        int index = 0;
        int spaceUsed = 0;
        int current = 0;
        while (index < count && spaceUsed < spaceLeft) 
        {
            current = minimumSizes.at(index);
            spaceUsed = sum + current * (count - index);
            sum += current;
            ++index;
        }
        --index;
        int deficit = spaceUsed - spaceLeft;
        int items = count - index;
        int deficitPerItem = deficit / items;
        int remainder = deficit % items;
        int maxval = current - deficitPerItem;

        int rest = 0;
        for (i = start; i < start + count; i++) 
        {
            int maxv = maxval;
            rest += remainder;
            if (rest >= items) 
            {
                maxv--;
                rest -= items;
            }
            GeomLayout* data = &chain[i];
            data->size = qMin(data->minimumSize, maxv);
            data->done = true;
        }
    }
    else if (space < szHint + sumSpacing) 
    {
        int n = count;
        int spaceLeft = space - sumSpacing;
        int overdraft = szHint - spaceLeft;

        for (i = start; i < start + count; i++) 
        {
            GeomLayout* data = &chain[i];
            if (!data->done && data->minimumSize >= data->suitableSizeHint())
            {
                data->size = data->suitableSizeHint();
                data->done = true;
                spaceLeft -= data->suitableSizeHint();
                n--;
            }
        }
        bool finished = n == 0;
        while (!finished) 
        {
            finished = true;
            qint64 fp_over = toFixed(overdraft);
            qint64 fixedWidth = 0;

            for (i = start; i < start + count; i++) 
            {
                GeomLayout* data = &chain[i];
                if (data->done)
                    continue;
                fixedWidth += fp_over / n;
                int w = fRound(fixedWidth);
                data->size = data->suitableSizeHint() - w;
                fixedWidth -= toFixed(w);
                if (data->size < data->minimumSize) 
                {
                    data->done = true;
                    data->size = data->minimumSize;
                    finished = false;
                    overdraft -= data->suitableSizeHint() - data->minimumSize;
                    n--;
                    break;
                }
            }
        }
    }
    else 
    {
        int n = count;
        int spaceLeft = space - sumSpacing;
        for (i = start; i < start + count; i++) 
        {
            GeomLayout* data = &chain[i];
            if (!data->done && (data->maximumSize <= data->suitableSizeHint() ||
                (!allEmptyNonstretch && data->empty && !data->expansive && data->stretch == 0))) 
            {
                data->size = data->suitableSizeHint();
                data->done = true;
                spaceLeft -= data->size;
                sumStretch -= data->stretch;
                if (data->expansive)
                    expandingCount--;
                n--;
            }
        }
        extraspace = spaceLeft;

        int surplus, deficit;
        do 
        {
            surplus = deficit = 0;
            qint64 fixedSpace = toFixed(spaceLeft);
            qint64 fixedWidth = 0;
            for (i = start; i < start + count; i++) 
            {
                GeomLayout *data = &chain[i];
                if (data->done)
                    continue;
                extraspace = 0;
                if (sumStretch > 0)
                    fixedWidth += (fixedSpace * data->stretch) / sumStretch;
                else if (expandingCount > 0) 
                    fixedWidth += (fixedSpace * (data->expansive ? 1 : 0)) / expandingCount;
                else
                    fixedWidth += fixedSpace * 1 / n;
                int w = fRound(fixedWidth);
                data->size = w;
                fixedWidth -= toFixed(w);
                if (w < data->suitableSizeHint())
                    deficit += data->suitableSizeHint() - w;
                else if (w > data->maximumSize)
                    surplus += w - data->maximumSize;
            }
            if (deficit > 0 && surplus <= deficit) 
            {
                for (i = start; i < start + count; i++) 
                {
                    GeomLayout* data = &chain[i];
                    if (!data->done && data->size < data->suitableSizeHint())
                    {
                        data->size = data->suitableSizeHint();
                        data->done = true;
                        spaceLeft -= data->suitableSizeHint();
                        sumStretch -= data->stretch;
                        if (data->expansive)
                            expandingCount--;
                        n--;
                    }
                }
            }
            if (surplus > 0 && surplus >= deficit) 
            {
                for (i = start; i < start + count; i++) 
                {
                    GeomLayout* data = &chain[i];
                    if (!data->done && data->size > data->maximumSize) 
                    {
                        data->size = data->maximumSize;
                        data->done = true;
                        spaceLeft -= data->maximumSize;
                        sumStretch -= data->stretch;
                        if (data->expansive)
                            expandingCount--;
                        n--;
                    }
                }
            }
        } while (n > 0 && surplus != deficit);

        if (n == 0)
            extraspace = spaceLeft;
    }

    int extra = extraspace / (spacerCount + 2);
    int p = pos + extra;
    for (i = start; i < start + count; i++) 
    {
        GeomLayout* data = &chain[i];
        data->pos = p;
        p += data->size;
        if (!data->empty)
            p += data->bestSpacer(spacer) + extra;
    }
}


DockToolBarItem::DockToolBarItem(QWidget *widget)
    : QWidgetItem(widget), action(0), customWidget(false)
{
}

bool DockToolBarItem::isEmpty() const
{
    return action == 0 || !action->isVisible();
}

DockToolBarLayout::DockToolBarLayout(QWidget* parent)
    : QLayout(parent)
    , expanded(false)
    , animating(false)
    , m_dirty(true)
    , m_expanding(false)
    , m_empty(true)
    , m_expandFlag(false)
    , m_popupMenu(Q_NULL)
    , m_heightItem(-1)
{
    DockToolBar* tb = qobject_cast<DockToolBar*>(parent);
    if (tb == Q_NULL)
        return;
    m_extension = new DockToolBarExtension(tb);
    m_extension->setFocusPolicy(Qt::NoFocus);
    m_extension->hide();
    QObject::connect(DockBarBasePrivate::_get(tb), SIGNAL(orientationChanged(Qt::Orientation)), m_extension, SLOT(setOrientation(Qt::Orientation)));
    setUsePopupMenu(qobject_cast<DockMainWindow *>(tb->parentWidget()) == Q_NULL);
}

DockToolBarLayout::~DockToolBarLayout()
{
    while (!m_items.isEmpty()) 
    {
        DockToolBarItem *item = m_items.takeFirst();
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction*>(item->action)) 
        {
            if (item->customWidget)
                widgetAction->releaseWidget(item->widget());
        }
        delete item;
    }
}

void DockToolBarLayout::updateMarginAndSpacing()
{
    DockToolBar *tb = qobject_cast<DockToolBar*>(parentWidget());
    if (!tb)
        return;
    QStyle *style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);

    const int margin = style->pixelMetric(QStyle::PM_ToolBarItemMargin, &opt, tb) + style->pixelMetric(QStyle::PM_ToolBarFrameWidth, &opt, tb);
    setContentsMargins(margin, margin, margin, margin);
    setSpacing(style->pixelMetric(QStyle::PM_ToolBarItemSpacing, &opt, tb));
}

bool DockToolBarLayout::hasExpandFlag() const
{
    return m_expandFlag;
}

void DockToolBarLayout::setUsePopupMenu(bool set)
{
    if (!m_dirty && ((m_popupMenu == Q_NULL) == set))
        invalidate();

    if (!set) 
    {
        QObject::connect(m_extension, SIGNAL(clicked(bool)), this, SLOT(setExpanded(bool)), Qt::UniqueConnection);
        m_extension->setPopupMode(QToolButton::DelayedPopup);
        m_extension->setMenu(0);
        Q_DELETE_AND_NULL(m_popupMenu);
    } 
    else 
    {
        QObject::disconnect(m_extension, SIGNAL(clicked(bool)), this, SLOT(setExpanded(bool)));
        m_extension->setPopupMode(QToolButton::InstantPopup);
        if (m_popupMenu == Q_NULL)
            m_popupMenu = new QMenu(m_extension);
        m_extension->setMenu(m_popupMenu);
    }
}

void DockToolBarLayout::checkUsePopupMenu()
{
    DockToolBar* tb = static_cast<DockToolBar*>(parent());
    QWidget* mw = qobject_cast<QWidget*>(tb/*->parent()*/);
    QSize size = mw != Q_NULL ? mw->size() : QSize();
    Qt::Orientation orientation = DockBarBasePrivate::_get(tb)->orientation();
    setUsePopupMenu(!mw || DockBarBasePrivate::_get(tb)->m_dockBarArea == Qtitan::DockBarFloat || 
                    perp(orientation, expandedSize(size)) >= perp(orientation, size));
//    setUsePopupMenu(!mw || DockBarBasePrivate::_get(tb)->m_barPosition == Qtitan::DockBarFloat);
}

void DockToolBarLayout::addItem(QLayoutItem*)
{
    qWarning() << "DockToolBarLayout::addItem(): please use addAction() instead";
    return;
}

QLayoutItem* DockToolBarLayout::itemAt(int index) const
{
    if (index < 0 || index >= m_items.count())
        return 0;
    return m_items.at(index);
}

QLayoutItem* DockToolBarLayout::takeAt(int index)
{
    if (index < 0 || index >= m_items.count())
        return Q_NULL;

    DockToolBarItem* item = m_items.takeAt(index);
    if (m_popupMenu != Q_NULL)
        m_popupMenu->removeAction(item->action);

    QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(item->action);
    if (widgetAction != 0 && item->customWidget) 
    {
        widgetAction->releaseWidget(item->widget());
    } 
    else 
    {
        item->widget()->hide();
        item->widget()->deleteLater();
    }

    invalidate();
    return item;
}

void DockToolBarLayout::insertAction(int index, QAction *action)
{
    index = qMax(0, index);
    index = qMin(m_items.count(), index);

    DockToolBarItem *item = createItem(action);
    if (item) 
    {
        m_items.insert(index, item);
        invalidate();
    }
}

int DockToolBarLayout::getActionIndex(QAction *action) const
{
    for (int i = 0; i < m_items.count(); ++i) 
    {
        if (m_items.at(i)->action == action)
            return i;
    }
    return -1;
}

int DockToolBarLayout::count() const
{
    return m_items.count();
}

bool DockToolBarLayout::isEmpty() const
{
    if (m_dirty)
        updateGeomArray();
    return m_empty;
}

void DockToolBarLayout::invalidate()
{
    m_dirty = true;
    QLayout::invalidate();
}

Qt::Orientations DockToolBarLayout::expandingDirections() const
{
    if (m_dirty)
        updateGeomArray();
    DockToolBar* tb = qobject_cast<DockToolBar*>(parentWidget());
    if (tb == Q_NULL)
        return Qt::Orientations(0);
    Qt::Orientation orientation = DockBarBasePrivate::_get(tb)->orientation();
    return m_expanding ? Qt::Orientations(orientation) : Qt::Orientations(0);
}

bool DockToolBarLayout::movable() const
{
    DockToolBar* toolBar = qobject_cast<DockToolBar*>(parentWidget());
    if (!toolBar)
        return false;
    DockBarSite* win = qobject_cast<DockBarSite*>(toolBar->parentWidget() ? toolBar->parentWidget() : Q_NULL);
    return DockToolBarPrivate::_get(toolBar)->m_movable && win != Q_NULL;
}

void DockToolBarLayout::updateGeomArray() const
{
    if (!m_dirty)
        return;

    DockToolBarLayout* that = const_cast<DockToolBarLayout*>(this);
    DockToolBar* tb = qobject_cast<DockToolBar*>(parentWidget());
    if (tb == Q_NULL)
        return;

    QStyle* style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);
    const int handleExtent = movable() ? style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb) : 0;

    int margin = 0;
    int left, top, right, bottom;
    this->getContentsMargins(&left, &top, &right, &bottom);
    if (left == top && top == right && right == bottom)
        margin = left;

    const int spacing = this->spacing();
    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);
    Qt::Orientation orientation = DockBarBasePrivate::_get(tb)->orientation();

    that->m_szMinSize = QSize(0, 0);
    that->m_szHint = QSize(0, 0);
    rperp(orientation, that->m_szMinSize) = style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb);
    rperp(orientation, that->m_szHint) = style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb);

    that->m_expanding = false;
    that->m_empty = false;

    QVector<GeomLayout> a(m_items.count() + 1);
    int count = 0;
    for (int i = 0; i < m_items.count(); ++i) 
    {
        DockToolBarItem* item = m_items.at(i);

        QSize max = item->maximumSize();
        QSize min = item->minimumSize();
        QSize m_szHint = item->sizeHint();

        if (!item->action->isSeparator() &&  m_szHint.height() < m_heightItem)
            m_szHint.setHeight(m_heightItem);

        Qt::Orientations exp = item->expandingDirections();
        bool m_empty = item->isEmpty();

        that->m_expanding = m_expanding || exp & orientation;

        if (item->widget() && (item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag))
            that->m_expandFlag = true;

        if (!m_empty) 
        {
            if (count == 0)
                rpick(orientation, that->m_szMinSize) += pick(orientation, min);
            int s = perp(orientation, m_szMinSize);
            rperp(orientation, that->m_szMinSize) = qMax(s, perp(orientation, min));

            rpick(orientation, that->m_szHint) += (count == 0 ? 0 : spacing) + pick(orientation, m_szHint);
            s = perp(orientation, that->m_szHint);
            rperp(orientation, that->m_szHint) = qMax(s, perp(orientation, m_szHint));
            ++count;
        }

        a[i].sizeHint = pick(orientation, m_szHint);
        a[i].maximumSize = pick(orientation, max);
        a[i].minimumSize = pick(orientation, min);
        a[i].expansive = exp & orientation;
        if (orientation == Qt::Horizontal)
            a[i].stretch = item->widget()->sizePolicy().horizontalStretch();
        else
            a[i].stretch = item->widget()->sizePolicy().verticalStretch();
        a[i].empty = m_empty;
    }

    that->m_geomArray = a;
    that->m_empty = count == 0;

    rpick(orientation, that->m_szMinSize) += handleExtent;
    that->m_szMinSize += QSize(2 * margin, 2 * margin);
    if (m_items.count() > 1)
        rpick(orientation, that->m_szMinSize) += spacing + extensionExtent;

    rpick(orientation, that->m_szHint) += handleExtent;
    that->m_szHint += QSize(2 * margin, 2 * margin);
    that->m_dirty = false;
}

static bool defaultWidgetAction(DockToolBarItem *item)
{
    QWidgetAction* action = qobject_cast<QWidgetAction*>(item->action);
    return action != Q_NULL && action->defaultWidget() == item->widget();
}

void DockToolBarLayout::setGeometry(const QRect& rect)
{
    DockToolBar* tb = qobject_cast<DockToolBar*>(parentWidget());
    if (tb == Q_NULL)
        return;

    if (m_heightItem == -1)
        m_heightItem = CustomLayout::calcItemHeight(tb);

    QStyle* style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);

    int margin = 0;
    int left, top, right, bottom;
    this->getContentsMargins(&left, &top, &right, &bottom);
    if (left == top && top == right && right == bottom)
        margin = left;

    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);
    Qt::Orientation orientation = DockBarBasePrivate::_get(tb)->orientation();

    QLayout::setGeometry(rect);

    bool ranOutOfSpace = false;
    if (!animating)
        ranOutOfSpace = layoutActions(rect.size());

    if (expanded || animating || ranOutOfSpace) 
    {
        Qtitan::DockBarArea position = Qtitan::DockBarTop;
        if (tb->parentWidget() && qobject_cast<QWidget*>(tb->parentWidget()->parentWidget()))
        {
            DockBarSite* dockSite = DockBarBasePrivate::_get(tb)->m_dockSite;
            position = dockSite != Q_NULL ? dockSite->dockBarArea() : Qtitan::DockBarNone;
        }

        QPoint pos;
        QSize szHint = sizeHint();

        rpick(orientation, pos) = pick(orientation, rect.bottomRight()) - margin - extensionExtent + 2;
        if (position == Qtitan::DockBarLeft || position == Qtitan::DockBarTop)
            rperp(orientation, pos) = perp(orientation, rect.topLeft()) + margin;
        else
            rperp(orientation, pos) = perp(orientation, rect.bottomRight()) - margin - (perp(orientation, szHint) - 2 * margin) + 1;
        QSize size;
        rpick(orientation, size) = extensionExtent;
        rperp(orientation, size) = perp(orientation, szHint) - 2 * margin;

        QRect r(pos, size);
        if (orientation == Qt::Horizontal)
            r = QStyle::visualRect(parentWidget()->layoutDirection(), rect, r);
        m_extension->setGeometry(r);

        if (m_extension->isHidden())
            m_extension->show();
    } 
    else 
    {
        if (!m_extension->isHidden())
            m_extension->hide();
    }
}

bool DockToolBarLayout::layoutActions(const QSize& size)
{
    if (m_dirty)
        updateGeomArray();

    DockToolBar* tb = qobject_cast<DockToolBar*>(parentWidget());
    if (!tb)
        return false;

    QRect rect(QPoint(0, 0), size);
    QList<QWidget*> showWidgets, hideWidgets;

    QStyle* style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);

    int margin = 0;
    int left, top, right, bottom;
    this->getContentsMargins(&left, &top, &right, &bottom);
    if (left == top && top == right && right == bottom)
        margin = left;

    const int spacing = this->spacing();
    int handleExtent = movable() ? style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb) : 0;
    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);
    Qt::Orientation orientation = DockBarBasePrivate::_get(tb)->orientation();
    bool extensionMenuContainsOnlyWidgetActions = true;

    int space = pick(orientation, rect.size()) - 2 * margin - handleExtent;
    if (space <= 0)
        return false;  // nothing to do.

    if (m_popupMenu != Q_NULL)
        m_popupMenu->clear();

    if (tb->dockBarArea() == Qtitan::DockBarFloat)
    {
        if (orientation == Qt::Horizontal)
            handleExtent = margin * 2;
        else
            handleExtent = margin;// + sz.height();
    }

    bool ranOutOfSpace = false;
    int rows = 0;
    int rowPos = perp(orientation, rect.topLeft()) + margin;
    int i = 0;
    while (i < m_items.count()) 
    {
        QVector<GeomLayout> g_data = m_geomArray;
        int start = i;
        int size = 0;
        int prev = -1;
        int rowHeight = 0;
        int count = 0;
        int maximumSize = 0;
        bool expansiveRow = false;
        for (; i < m_items.count(); ++i) 
        {
            if (g_data[i].empty)
                continue;
            int newSize = size + (count == 0 ? 0 : spacing) + g_data[i].minimumSize;
            if (prev != -1 && newSize > space) 
            {
                if (rows == 0)
                    ranOutOfSpace = true;
                if (count > 1 && size + spacing + extensionExtent > space)
                    i = prev;
                break;
            }

            if (expanded)
                rowHeight = qMax(rowHeight, perp(orientation, m_items.at(i)->sizeHint()));

            expansiveRow = expansiveRow || g_data[i].expansive;
            size = newSize;
            maximumSize += spacing + (g_data[i].expansive ? g_data[i].maximumSize : g_data[i].suitableSizeHint());
            prev = i;
            ++count;
        }
        g_data[i].sizeHint = 0;
        g_data[i].maximumSize = QWIDGETSIZE_MAX;
        g_data[i].minimumSize = 0;
        g_data[i].expansive = true;
        g_data[i].stretch = 0;
        g_data[i].empty = true;

        if (expansiveRow && maximumSize < space) 
        {
            expansiveRow = false;
            g_data[i].maximumSize = space - maximumSize;
        }

        qtnGeomCompute(g_data, start, i - start + (expansiveRow ? 0 : 1), 0, space - (ranOutOfSpace ? (extensionExtent + spacing) : 0), spacing);

        for (int j = start; j < i; ++j) 
        {
            DockToolBarItem* item = m_items.at(j);

            if (g_data[j].empty)
            {
                if (!item->widget()->isHidden())
                    hideWidgets << item->widget();
                continue;
            }

            QPoint pos;
            rpick(orientation, pos) = margin + handleExtent + g_data[j].pos;
            rperp(orientation, pos) = rowPos;
            QSize size;
            rpick(orientation, size) = g_data[j].size;
            if (expanded)
                rperp(orientation, size) = rowHeight;
            else
                rperp(orientation, size) = perp(orientation, rect.size()) - 2*margin;

            QRect r(pos, size);
            if (orientation == Qt::Horizontal)
            {
                r = QStyle::visualRect(parentWidget()->layoutDirection(), rect, r);
                r.setY(0);

                if (item->action->isSeparator())
                    r.setBottom(r.bottom() - margin);
            }

            item->setGeometry(r);

            if (item->widget()->isHidden())
                showWidgets << item->widget();
        }

        if (!expanded) 
        {
            for (int j = i; j < m_items.count(); ++j) 
            {
                DockToolBarItem* item = m_items.at(j);
                if (!item->widget()->isHidden())
                    hideWidgets << item->widget();
                if (m_popupMenu != Q_NULL) 
                {
                    if (!defaultWidgetAction(item)) 
                    {
                        m_popupMenu->addAction(item->action);
                        extensionMenuContainsOnlyWidgetActions = false;
                    }
                }
            }
            break;
        }
        rowPos += rowHeight + spacing;
        ++rows;
    }

    m_extension->setEnabled(m_popupMenu == Q_NULL || !extensionMenuContainsOnlyWidgetActions);

    for (int i = 0; i < showWidgets.count(); ++i)
        showWidgets.at(i)->show();
    for (int i = 0; i < hideWidgets.count(); ++i)
        hideWidgets.at(i)->hide();

    return ranOutOfSpace;
}

QSize DockToolBarLayout::expandedSize(const QSize& size) const
{
    if (m_dirty)
        updateGeomArray();

    DockToolBar* tb = qobject_cast<DockToolBar*>(parentWidget());
    if (!tb)
        return QSize(0, 0);

    QWidget* win = qobject_cast<QWidget*>(tb->parentWidget());
    Qt::Orientation o = DockBarBasePrivate::_get(tb)->orientation();
    QStyle* style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);
    const int handleExtent = movable() ? style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb) : 0;

    int margin = 0;
    int left, top, right, bottom;
    this->getContentsMargins(&left, &top, &right, &bottom);
    if (left == top && top == right && right == bottom)
        margin = left;

    const int spacing = this->spacing();
    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);

    int total_w = 0;
    int count = 0;
    for (int x = 0; x < m_items.count(); ++x) 
    {
        if (!m_geomArray[x].empty) 
        {
            total_w += (count == 0 ? 0 : spacing) + m_geomArray[x].minimumSize;
            ++count;
        }
    }
    if (count == 0)
        return QSize(0, 0);

    int min_w = pick(o, size);
    int rows = (int)qSqrt(qreal(count));
    if (rows == 1)
        ++rows;
    int space = total_w/rows + spacing + extensionExtent;
    space = qMax(space, min_w - 2*margin - handleExtent);
    if (win != 0)
        space = qMin(space, pick(o, win->size()) - 2*margin - handleExtent);

    int w = 0;
    int h = 0;
    int i = 0;
    while (i < m_items.count()) 
    {
        int count = 0;
        int size = 0;
        int prev = -1;
        int rowHeight = 0;
        for (; i < m_items.count(); ++i) 
        {
            if (m_geomArray[i].empty)
                continue;

            int newSize = size + (count == 0 ? 0 : spacing) + m_geomArray[i].minimumSize;

            QSize szItems = m_items.at(i)->sizeHint();
            rowHeight = qMax(rowHeight, perp(o, szItems));

            if (prev != -1 && newSize > space) 
            {
                if (count > 1 && size + spacing + extensionExtent > space) 
                {
                    size -= spacing + m_geomArray[prev].minimumSize;
                    i = prev;
                }
                break;
            }

            size = newSize;
            prev = i;
            ++count;
        }

        w = qMax(size, w);
        h += rowHeight + spacing;
    }

    w += 2*margin + handleExtent + spacing + extensionExtent;
    w = qMax(w, min_w);
    if (win != 0)
        w = qMin(w, pick(o, win->size()));
    h += 2*margin - spacing;

    QSize result;
    rpick(o, result) = w;
    rperp(o, result) = h;
    return result;
}

void DockToolBarLayout::setExpanded(bool exp)
{
    if (exp == expanded)
        return;
    expanded = exp;
    m_extension->setChecked(expanded);
}

QSize DockToolBarLayout::minimumSize() const
{
    if (m_dirty)
        updateGeomArray();
    return m_szMinSize;
}

QSize DockToolBarLayout::sizeHint() const
{
    if (m_dirty)
        updateGeomArray();
    return m_szHint;
}

DockToolBarItem* DockToolBarLayout::createItem(QAction* action)
{
    bool customWidget = false;
    bool standardButtonWidget = false;
    QWidget* widget = 0;
    DockToolBar* tb = qobject_cast<DockToolBar*>(parentWidget());
    if (!tb)
        return (DockToolBarItem *)0;

    if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction *>(action)) 
    {
        widget = widgetAction->requestWidget(tb);
        if (widget != 0) 
        {
            widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            customWidget = true;
        }
    } 
    else if (action->isSeparator()) 
    {
        DockToolBarSeparator* sep = new DockToolBarSeparator(tb);
        QObject::connect(DockBarBasePrivate::_get(tb), SIGNAL(orientationChanged(Qt::Orientation)), sep, SLOT(setOrientation(Qt::Orientation)));
        widget = sep;
    }

    if (!widget) 
    {
        ToolButton *button = new ToolButton(tb);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setIconSize(tb->iconSize());
        button->setToolButtonStyle(tb->toolButtonStyle());
        QObject::connect(tb, SIGNAL(iconSizeChanged(QSize)), button, SLOT(setIconSize(QSize)));
        QObject::connect(tb, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)), button, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
        button->setDefaultAction(action);
        QObject::connect(button, SIGNAL(triggered(QAction*)), tb, SIGNAL(actionTriggered(QAction*)));
        widget = button;
        standardButtonWidget = true;
    }

    widget->hide();
    DockToolBarItem* result = new DockToolBarItem(widget);
    if (standardButtonWidget)
        result->setAlignment(Qt::AlignJustify);
    result->customWidget = customWidget;
    result->action = action;
    return result;
}

/*! \reimp */
QSize ToolButton::sizeHint() const
{
    ensurePolished();

    int w = 0, h = 0;
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QFontMetrics fm = fontMetrics();
    if (opt.toolButtonStyle != Qt::ToolButtonTextOnly)
    {
        QSize icon = opt.iconSize;
        w = icon.width();
        h = icon.height();
    }

    if (opt.toolButtonStyle != Qt::ToolButtonIconOnly)
    {
        QSize textSize = fm.size(Qt::TextShowMnemonic, text());
        textSize.setWidth(textSize.width() + fm.horizontalAdvance(QLatin1Char(' '))*2);
        if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
        {
            h += 4 + textSize.height();
            if (textSize.width() > w)
                w = textSize.width();
        }
        else if (opt.toolButtonStyle == Qt::ToolButtonTextBesideIcon)
        {
            w += 4 + textSize.width();
            if (textSize.height() > h)
                h = textSize.height();
        }
        else
        { // TextOnly
            w = textSize.width();
            h = textSize.height();
        }
    }

    opt.rect.setSize(QSize(w, h)); // PM_MenuButtonIndicator depends on the height
    if (popupMode() == MenuButtonPopup)
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);

    return style()->sizeFromContents(QStyle::CT_ToolButton, &opt, QSize(w, h), this);
}

/*! \reimp */
void ToolButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
#ifdef Q_OS_MAC
    if (opt.state & (/*QStyle::State_Sunken | QStyle::State_On |*/ QStyle::State_Raised))
        opt.subControls = QStyle::SubControls();
#endif
    style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}
