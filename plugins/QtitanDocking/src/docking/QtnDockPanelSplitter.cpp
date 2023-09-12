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
#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>

#include "QtnDockPanelSplitter.h"
#include "QtnDockWindowPrivate.h"
#include "QtnDockPanelManagerPrivate.h"
#include "QtnDockWidgetPanelPrivate.h"
#include "QtnDockMainWindowLayout.h"
#include "QtnDockLayoutPrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE
/* DockPanelSplitterPrivate */
class DockPanelSplitterPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(DockPanelSplitter)
public:
    explicit DockPanelSplitterPrivate();
    virtual ~DockPanelSplitterPrivate();
public:
    void initialization();
    void reposition(QRect rc, QRect rcAllowed);
    bool calcAllowedRect(QRect& rcAllowed, QRect& rcUnion);
public:
    DockSplitLayoutPanel* m_layoutPanel;
    DockPanelBase* m_first;
    DockPanelBase* m_second;
    DockPanelManager* m_manager;
    QRect m_rcAllowed;
    QRect m_rcUnion;
    bool m_horiz;
    QPoint m_pressedPos;
    QRect m_pressedGeometry;
    bool m_hover;
private:
    Q_DISABLE_COPY(DockPanelSplitterPrivate)
};
QTITAN_END_NAMESPACE

DockPanelSplitterPrivate::DockPanelSplitterPrivate()
    : m_layoutPanel(Q_NULL)
    , m_first(Q_NULL)
    , m_second(Q_NULL)
    , m_manager(Q_NULL)
    , m_rcAllowed(QRect())
    , m_rcUnion(QRect())
    , m_horiz(false)
    , m_pressedPos(cursorPositionInvalid)
    , m_hover(false)
{
}

DockPanelSplitterPrivate::~DockPanelSplitterPrivate()
{
}

void DockPanelSplitterPrivate::initialization()
{
    QTN_P(DockPanelSplitter);
    p.setAttribute(Qt::WA_Hover, true);
    p.setMouseTracking(true);
    p.setCursor(!m_horiz ? Qt::SplitHCursor : Qt::SplitVCursor);
    p.setFocusPolicy(Qt::NoFocus);
    p.setGeometry(QRect(0, 0, 0, 0));
}

void DockPanelSplitterPrivate::reposition(QRect rc, QRect rcAllowed)
{
    QTN_P(DockPanelSplitter);
    const int splitterIndent = 0;
    const int splitterWidth = p.style()->pixelMetric(QStyle::PM_SplitterWidth, 0, &p);
    
    if (m_layoutPanel->isHorizontal())
    {
        qreal d = static_cast<qreal>(rc.left() + splitterIndent - rcAllowed.left()) / rcAllowed.width();
        int sum = DockPanelBasePrivate::_get(m_first)->m_sizeDocking.width() + DockPanelBasePrivate::_get(m_second)->m_sizeDocking.width() + splitterWidth;
        DockPanelBasePrivate::_get(m_first)->m_sizeDocking.setWidth(QApplication::isRightToLeft() ? sum - static_cast<int>(d * sum) - splitterWidth : static_cast<int>(d * sum));
        DockPanelBasePrivate::_get(m_second)->m_sizeDocking.setWidth(sum - DockPanelBasePrivate::_get(m_first)->m_sizeDocking.width() - splitterWidth);
    }
    else
    {
        qreal d = static_cast<qreal>(rc.top() + splitterIndent - rcAllowed.top()) / rcAllowed.height();
        int sum = DockPanelBasePrivate::_get(m_first)->m_sizeDocking.height() + DockPanelBasePrivate::_get(m_second)->m_sizeDocking.height() + splitterWidth;
        DockPanelBasePrivate::_get(m_first)->m_sizeDocking.setHeight(static_cast<int>(d * sum));
        DockPanelBasePrivate::_get(m_second)->m_sizeDocking.setHeight(sum - DockPanelBasePrivate::_get(m_first)->m_sizeDocking.height() - splitterWidth);
    }
}

bool DockPanelSplitterPrivate::calcAllowedRect(QRect& rcAllowed, QRect& rcUnion)
{
    QTN_P(DockPanelSplitter);
    if (!m_manager)
        return false;

    const int splitterWidth = p.style()->pixelMetric(QStyle::PM_SplitterWidth, 0, &p);

    bool horiz = m_layoutPanel->isHorizontal();
    Q_ASSERT(m_first && m_second);
    if (!m_first || !m_second)
        return false;

    QRect rcFirst = m_first->geometry();
    QRect rcSecond = m_second->geometry();

    qtnMinMaxSize mmiFirst, mmiSecond;
    DockPanelBasePrivate::_get(m_first)->getMinMaxData(mmiFirst);
    DockPanelBasePrivate::_get(m_second)->getMinMaxData(mmiSecond);

    rcAllowed = rcFirst.united(rcSecond);
    rcUnion = rcAllowed;

    int gap = DockPanelManagerPrivate::_get(m_manager)->m_splitterGap;
    if (horiz)
    {
        rcAllowed.adjust(qMax(mmiFirst.minTrackSize.width(), gap), 0, -qMax(mmiSecond.minTrackSize.width() + splitterWidth, gap), 0);

        if (rcUnion.width() > mmiFirst.maxTrackSize.width())
            rcAllowed.setRight(qMin(rcAllowed.right(), rcUnion.left() + mmiFirst.maxTrackSize.width()));

        if (rcUnion.width() > mmiSecond.maxTrackSize.width())
            rcAllowed.setLeft(qMax(rcAllowed.left(), rcUnion.right() - mmiSecond.maxTrackSize.width() - splitterWidth));

        if (rcAllowed.left() >= rcAllowed.right())
            return false;
    }
    else
    {
        rcAllowed.adjust(0, qMax(mmiFirst.minTrackSize.height(), gap), 0, -qMax(mmiSecond.minTrackSize.height() + splitterWidth, gap));

        if (rcUnion.height() > mmiFirst.maxTrackSize.height())
            rcAllowed.setBottom(qMin(rcAllowed.bottom(), rcUnion.top() + mmiFirst.maxTrackSize.height()));

        if (rcUnion.height() > mmiSecond.maxTrackSize.height())
            rcAllowed.setTop(qMax(rcAllowed.top(), rcUnion.bottom() - mmiSecond.maxTrackSize.height() - splitterWidth));

        if (rcAllowed.top() >= rcAllowed.bottom())
            return false;
    }
    return true;
}

/* DockPanelSplitter */
DockPanelSplitter::DockPanelSplitter(DockSplitLayoutPanel* layoutPanel, 
    DockPanelBase* first, DockPanelBase* second, QWidget* parent)
    : QSplitter(parent)
{
    QTN_INIT_PRIVATE(DockPanelSplitter);
    QTN_D(DockPanelSplitter);
    d.m_first = first;
    d.m_second = second;
    d.m_layoutPanel = layoutPanel;
    d.m_horiz = !layoutPanel->isHorizontal();
    d.m_manager = layoutPanel->dockManager();
    d.initialization();
}

DockPanelSplitter::~DockPanelSplitter()
{
    QTN_FINI_PRIVATE();
}

bool DockPanelSplitter::isHorizontal() const
{
    QTN_D(const DockPanelSplitter);
    return d.m_horiz;
}

bool DockPanelSplitter::isPressed() const
{
    QTN_D(const DockPanelSplitter);
    return d.m_pressedPos != cursorPositionInvalid;
}

DockSplitLayoutPanel* DockPanelSplitter::layoutPanel() const
{
    QTN_D(const DockPanelSplitter);
    return d.m_layoutPanel;
}

/*! \reimp */
bool DockPanelSplitter::event(QEvent* event)
{
    QTN_D(DockPanelSplitter);
    switch(event->type()) 
    {
        case QEvent::HoverEnter:
            d.m_hover = true;
            update();
            break;
        case QEvent::HoverLeave:
            d.m_hover = false;
            update();
            break;
        default:
            break;
    }
    return QWidget::event(event);
}

/*! \reimp */
void DockPanelSplitter::paintEvent(QPaintEvent* event)
{
    QTN_D(DockPanelSplitter);
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    opt.palette = palette();

    if (isHorizontal())
        opt.state = QStyle::State_Horizontal;
    else
        opt.state = QStyle::State_None;

    if (d.m_hover)
        opt.state |= QStyle::State_MouseOver;
    if (isPressed())
        opt.state |= QStyle::State_Sunken;
    if (isEnabled())
        opt.state |= QStyle::State_Enabled;

    style()->drawControl(QStyle::CE_Splitter, &opt, &p, this);
}

/*! \reimp */
void DockPanelSplitter::mousePressEvent(QMouseEvent* event)
{
    QTN_D(DockPanelSplitter);
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (d.m_layoutPanel == Q_NULL)
        return;

    d.m_rcAllowed = QRect();
    d.m_rcUnion = QRect();
    if (!d.calcAllowedRect(d.m_rcAllowed, d.m_rcUnion))
        return;

    d.m_pressedPos = QCursor::pos();
    d.m_pressedGeometry = geometry();
    d.m_layoutPanel->adjustDockingSize();

    if (DockPanelManagerPrivate::_get(d.m_manager)->isSplitterTrackerUsed())
    {
    }
    else
    {
        grabMouse(!d.m_horiz ? Qt::SplitHCursor : Qt::SplitVCursor);
    }
}

/*! \reimp */
void DockPanelSplitter::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(DockPanelSplitter);
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (d.m_pressedPos != cursorPositionInvalid)
    {
        QPoint offset = QCursor::pos() - d.m_pressedPos;
        if (offset.x() == 0 && offset.y() == 0)
            return;
        if (!d.calcAllowedRect(d.m_rcAllowed, d.m_rcUnion))
            return;

        bool horiz = d.m_layoutPanel->isHorizontal();

        QRect rect = geometry();
        QPoint point = d.m_pressedGeometry.topLeft() + offset;

        point.setX(qMax(qMin(point.x(), d.m_rcAllowed.right()), d.m_rcAllowed.left()));
        point.setY(qMax(qMin(point.y(), d.m_rcAllowed.bottom()), d.m_rcAllowed.top()));

        if (horiz)
        {
            if (rect.left() == point.x())
                return;
            rect.translate(point.x() - rect.left(), 0);
        }
        else
        {
            if (rect.top() == point.y())
                return;
            rect.translate(0, point.y() - rect.top());
        }
        d.reposition(rect, d.m_rcUnion);

        if (DockWindow* dockWindow = qobject_cast<DockWindow*>(d.m_layoutPanel->topDockWindow()))
        {
            DockWindowLayout* windowLayout = qobject_cast<DockWindowLayout*>(dockWindow->layout());
            Q_ASSERT(windowLayout != Q_NULL);
            windowLayout->updateGeometry();
        }
        else
            DockPanelManagerPrivate::_get(d.m_manager)->recalcLayout();
    }
}

/*! \reimp */
void DockPanelSplitter::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    QTN_D(DockPanelSplitter);
    if (isPressed())
    {
        d.m_pressedPos = cursorPositionInvalid;
        QWidget* grWidget = QWidget::mouseGrabber();
        if (grWidget == this)
            releaseMouse();
    }
}


