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
#include "QtnDockWidgetDragDrop.h"

#include <QApplication>
#include <QtGui>
#include <QTabBar>
#include <QResizeEvent>

#include "QtnStyleOption.h"
#include "QtnDockWindow.h"
#include "QtnDockTitleBar.h"
#include "QtnCommonStylePrivate.h"
#include "QtnDockWindowPrivate.h"
#include "QtnDockLayoutPrivate.h"
#include "QtnDockDocumentPanelPrivate.h"
#include "QtnDockPanelManagerPrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE
static qreal borderWidth = 5.0;
static int offsetTab   = 10;
extern DockDocumentLayoutPanel* qtn_parentDocumentLayout(DockLayoutPanel* layoutPanel);

//!
static QSize qtn_sizeHintTabBar(QWidget* parent)
{
    QTabBar tabBar;
    tabBar.setFont(parent->font());
    tabBar.setAttribute(Qt::WA_MacSmallSize);
    tabBar.insertTab(0, QStringLiteral("TEXT - TEXT"));
    return tabBar.sizeHint();
}

//!
static QRect qtn_mapPanelToGlobalRect(const DockPanelBase* panel)
{
    Q_ASSERT(panel != Q_NULL);
    QRect rc = panel->rect();
    QPoint pnt = panel->mapToGlobal(QPoint(0, 0));
    rc.moveTopLeft(pnt);
    return rc;
}

static void qtn_adjustRectangle(QRect& rect, const QPoint& pt, const QWidget* widget)
{
    int value6 = CommonStylePrivate::dpiScaled(6.0, widget);
    int xOffset = (pt.x() < rect.left()) ? (pt.x() - rect.left() - value6) :
        (pt.x() > rect.right()) ? (pt.x() - rect.right() + value6) : 0;
    int yOffset = (pt.y() < rect.top()) ? (pt.y() - rect.top() - value6) :
        (pt.y() > rect.bottom()) ? (pt.y() - rect.bottom() + value6) : 0;
    rect.translate(xOffset, yOffset);
}

QTITAN_BEGIN_NAMESPACE
/* HighlightFrameMessages */
class HighlightFrameMessages : public QWidget
{
public:
    explicit HighlightFrameMessages(QWidget* parent, DockWidgetDragDrop* masterDrag);
    virtual ~HighlightFrameMessages();
protected:
    virtual void moveEvent(QMoveEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
private:
    DockWidgetDragDrop* m_masterDrag;
private:
    Q_DISABLE_COPY(HighlightFrameMessages);
};
QTITAN_END_NAMESPACE

/* HighlightFrameMessages */
HighlightFrameMessages::HighlightFrameMessages(QWidget* parent, DockWidgetDragDrop* masterDrag)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_masterDrag(masterDrag)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_MouseTracking);
    show();
}

HighlightFrameMessages::~HighlightFrameMessages()
{
}

void HighlightFrameMessages::moveEvent(QMoveEvent* event)
{
    Q_UNUSED(event);
    if (!m_masterDrag->isDragInitialized())
        return;
    m_masterDrag->moveDrag(QCursor::pos(), Qt::NoModifier);
    m_masterDrag->m_pntSnap = QPoint(0, 0);
}

void HighlightFrameMessages::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        m_masterDrag->stopDrag();
}

/* DockWidgetPhantomWidget */
DockWidgetPhantomWidget::DockWidgetPhantomWidget(DockWidgetDragDrop* masterDrag, bool drawFrame)
    : QWidget(masterDrag->m_dockManager->managedWidget(),
              Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
              #ifndef Q_OS_WIN
              | Qt::X11BypassWindowManagerHint
              #endif
              )
    , m_masterDrag(masterDrag)
    , m_frameMessages(Q_NULL)
    , m_drawFrame(drawFrame)
    , m_drawTab(false)
    , m_posTab(Qtitan::TopArea)
{
    setFocusPolicy(Qt::NoFocus);
    if (!masterDrag->m_dockManager->isDockPanelFullContentsWhileDraggingShown())
        m_frameMessages = new HighlightFrameMessages(masterDrag->m_dockManager->managedWidget(), masterDrag);

    if (m_drawFrame)
        setAttribute(Qt::WA_TranslucentBackground, true);
    else
        setWindowOpacity(0.50);
}

DockWidgetPhantomWidget::~DockWidgetPhantomWidget()
{
    Q_DELETE_AND_NULL(m_frameMessages);
}

void DockWidgetPhantomWidget::startDrag(const QPoint& pnt)
{
    Q_ASSERT(m_frameMessages != Q_NULL);
    WindowTitleBar::startDrag(m_frameMessages, pnt);
}

void DockWidgetPhantomWidget::setStartRect(const QRect& rect)
{
    if (m_frameMessages != Q_NULL)
        m_frameMessages->setGeometry(rect);
}

void DockWidgetPhantomWidget::setGeometryFrame(const QRect& rect)
{
    Q_ASSERT(m_frameMessages != Q_NULL);
    if (rect.size() != m_frameMessages->size())
        setGeometry(rect);
    else
        setGeometry(m_frameMessages->geometry());
    setVisible(true);
}

void DockWidgetPhantomWidget::createMask(const QRect& rcDockWidget)
{
    clearMask();
    if (m_drawTab)
    {
        const qreal value = CommonStylePrivate::dpiScaled(4.0, this);
        const qreal valueHeight = CommonStylePrivate::dpiScaled(24.0, this);
        const qreal valueWidth = CommonStylePrivate::dpiScaled(70.0, this);
        QRectF rc1(QPoint(0, 0), rcDockWidget.size());
        QRectF rc(QPoint(0, 0), rcDockWidget.size());
        rc.adjust(value, value, -value, -value);

        QSizeF szTab(qMin(valueWidth, rc.width()), qMin(valueHeight, rc.height() / 2));
        static int indent = 10;
        QPolygonF polygon(18);
        polygon.append(QPointF(rc1.left(), rc1.top()));
        polygon.append(QPointF(rc1.left(), rc1.bottom() - szTab.height()));
        polygon.append(QPointF(rc1.left() + indent, rc1.bottom() - szTab.height()));
        polygon.append(QPointF(rc1.left() + indent, rc1.bottom()));
        polygon.append(QPointF(rc1.left() + indent + szTab.width(), rc1.bottom()));
        polygon.append(QPointF(rc1.left() + indent + szTab.width(), rc1.bottom() - szTab.height()));
        polygon.append(QPointF(rc1.right(), rc1.bottom() - szTab.height()));
        polygon.append(QPointF(rc1.right(), rc1.top()));
        polygon.append(QPointF(rc1.left(), rc1.top()));

        polygon.append(QPointF(rc.left(), rc.top()));
        polygon.append(QPointF(rc.left(), rc.bottom() - szTab.height()));
        polygon.append(QPointF(rc.left() + indent, rc.bottom() - szTab.height()));
        polygon.append(QPointF(rc.left() + indent, rc.bottom()));
        polygon.append(QPointF(rc.left() + indent + szTab.width(), rc.bottom()));
        polygon.append(QPointF(rc.left() + indent + szTab.width(), rc.bottom() - szTab.height()));
        polygon.append(QPointF(rc.right(), rc.bottom() - szTab.height()));
        polygon.append(QPointF(rc.right(), rc.top()));
        polygon.append(QPointF(rc.left(), rc.top()));
        setMask(polygon.toPolygon());
    }
    else
    {
        const qreal value = CommonStylePrivate::dpiScaled(4.0, this);
        QRectF rc1(QPointF(0.0, 0.0), rcDockWidget.size());
        QRectF rc(QPointF(0.0, 0.0), rcDockWidget.size());
        rc.adjust(value, value, -value, -value);

        QPolygonF polygon(10);
        polygon.append(QPointF(rc1.left(), rc1.top()));
        polygon.append(QPointF(rc1.right(), rc1.top()));
        polygon.append(QPointF(rc1.right(), rc1.bottom()));
        polygon.append(QPointF(rc1.left(), rc1.bottom()));
        polygon.append(QPointF(rc1.left(), rc1.top()));

        polygon.append(QPointF(rc.left(), rc.top()));
        polygon.append(QPointF(rc.right(), rc.top()));
        polygon.append(QPointF(rc.right(), rc.bottom()));
        polygon.append(QPointF(rc.left(), rc.bottom()));
        polygon.append(QPointF(rc.left(), rc.top()));
        setMask(polygon.toPolygon());
    }
}

void DockWidgetPhantomWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    DockPhantomStyleOption opt;
    opt.initFrom(this);
    opt.palette = this->palette();
    opt.drawFrame = m_drawFrame;
    opt.posTab = m_posTab;
    opt.borderWidth = CommonStylePrivate::dpiScaled(borderWidth, this);
    opt.rectTab = m_rectTab;
    style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(CommonStyle::PE_DockWidgetPhantom), &opt, &p, this);
}

void DockWidgetPhantomWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_drawFrame && event->size().isValid() && !event->size().isNull())
        createMask(rect());
}


#define INDEX_MARKER_TOP              0
#define INDEX_MARKER_LEFT             1
#define INDEX_MARKER_BOTTOM           2
#define INDEX_MARKER_RIGHT            3

#define INDEX_MARKER_CENTER_LEFT      4
#define INDEX_MARKER_CENTER_RIGHT     5
#define INDEX_MARKER_CENTER_TOP       6
#define INDEX_MARKER_CENTER_BOTTOM    7

#define INDEX_MARKER_CENTER           8
#define INDEX_MARKER_BASIS            9

//---------------------------------------
#define INDEX_EX_MARKER_TOP              10
#define INDEX_EX_MARKER_LEFT             11
#define INDEX_EX_MARKER_BOTTOM           12
#define INDEX_EX_MARKER_RIGHT            13

#define INDEX_EX_MARKER_CENTER_LEFT      14
#define INDEX_EX_MARKER_CENTER_RIGHT     15
#define INDEX_EX_MARKER_CENTER_TOP       16
#define INDEX_EX_MARKER_CENTER_BOTTOM    17

#define INDEX_EX_MARKER_CENTER_CENTER    18
#define INDEX_EX_MARKER_CENTER_BASIS     19


static QPointF arrMarkersPosition[] =
{
    QPoint(36.0,   0.0),    // 0   INDEX_MARKER_TOP
    QPoint(0.0,   36.0),    // 1   INDEX_MARKER_LEFT
    QPoint(36.0,  72.0),    // 2   INDEX_MARKER_BOTTOM 
    QPoint(72.0,  36.0),    // 3   INDEX_MARKER_RIGHT 

    QPoint(0.0,    36.0),   // 8   INDEX_MARKER_CENTER_LEFT
    QPoint(73.0,   36.0),   // 9   INDEX_MARKER_CENTER_RIGHT
    QPoint(36.0,   0.0),    // 10  INDEX_MARKER_CENTER_TOP
    QPoint(36.0,   73.0),   // 11  INDEX_MARKER_CENTER_BOTTOM

    QPoint(36.0,   36.0),   // 16  INDEX_MARKER_CENTER

    QPoint(0.0,     0.0),   // 18  Basis

    QPoint(73.0,    0.0),   // 19  INDEX_EX_MARKER_TOP
    QPoint(0.0,    73.0),   // 20  INDEX_EX_MARKER_LEFT
    QPoint(73.0,  146.0),   // 21  INDEX_EX_MARKER_BOTTOM 
    QPoint(146.0,  73.0),   // 22  INDEX_EX_MARKER_RIGHT 

    QPoint(36.0,   73.0),   // 23  INDEX_EX_MARKER_CENTER_LEFT
    QPoint(110.0,  73.0),   // 24  INDEX_EX_MARKER_CENTER_RIGHT
    QPoint(73.0,   36.0),   // 25  INDEX_EX_MARKER_CENTER_TOP
    QPoint(73.0,  110.0),   // 26  INDEX_EX_MARKER_CENTER_BOTTOM

    QPoint(73.0,   73.0),   // 27  INDEX_EX_MARKER_CENTER_CENTER

    QPoint(0.0,    0.0),    // 28  basisCenter

    QPoint(0.0, 0.0),
    QPoint(0.0, 0.0),
    QPoint(0.0, 0.0),
    QPoint(0.0, 0.0),

    QPoint(0.0, 0.0),
    QPoint(0, 0.0),
    QPoint(0.0, 0.0),
    QPoint(0.0, 0.0),
};


/* DockWidgetMarkerWidget */
DockWidgetMarkerWidget::DockWidgetMarkerWidget(DockWidgetDragDrop* masterDrag)
    : QWidget(masterDrag->m_dockManager->managedWidget(),
              Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
    #ifndef Q_OS_WIN
    | Qt::X11BypassWindowManagerHint
    #endif
    )
    , m_masterDrag(masterDrag)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

DockWidgetMarkerWidget::~DockWidgetMarkerWidget()
{
}

DockWidgetMarkerWidget::DockingWidgetMarkerType DockWidgetMarkerWidget::hitTest(const QPoint& pnt)
{
    if (isHidden())
        return MarkerNone;

    QPoint pntHit = mapFromGlobal(pnt);
    QRect rc(geometry());

    if (!rc.contains(pnt))
        return MarkerNone;

    QPointF* position = arrMarkersPosition;
    const bool basis = (m_typeMarker & MarkerBasis) == MarkerBasis;
    const bool basisCenter = (m_typeMarker & MarkerCenterBasis) == MarkerCenterBasis;

    if (m_typeMarker & MarkerTop)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerTop));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_TOP], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_TOP], this));
        if (rc.contains(pntHit))
            return MarkerTop;
    }
    if (m_typeMarker & MarkerBottom)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerBottom));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_BOTTOM], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_BOTTOM], this));
        if (rc.contains(pntHit))
            return MarkerBottom;
    }
    if (m_typeMarker & MarkerLeft)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerLeft));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_LEFT], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_LEFT], this));
        if (rc.contains(pntHit))
            return MarkerLeft;
    }
    if (m_typeMarker & MarkerRight)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerRight));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_RIGHT], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_RIGHT], this));
        if (rc.contains(pntHit))
            return MarkerRight;
    }
    if (m_typeMarker & MarkerCenter)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerCenter));
        if (basis)
            rc.translate(qtn_DPIScaledF(position[INDEX_MARKER_CENTER], this));
        if (rc.contains(pntHit))
            return MarkerCenter;
    }

    if (m_typeMarker & MarkerCenterCenter)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerCenterCenter));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_CENTER_CENTER], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_CENTER], this));
        if (rc.contains(pntHit))
            return MarkerCenterCenter;
    }
    if (m_typeMarker & MarkerCenterLeft)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerCenterLeft));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_CENTER_LEFT], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_CENTER_LEFT], this));
        if (rc.contains(pntHit))
            return MarkerCenterLeft;
    }
    if (m_typeMarker & MarkerCenterRight)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerCenterRight));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_CENTER_RIGHT], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_CENTER_RIGHT], this));
        if (rc.contains(pntHit))
            return MarkerCenterRight;
    }
    if (m_typeMarker & MarkerCenterTop)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerCenterRight));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_CENTER_TOP], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_CENTER_TOP], this));
        if (rc.contains(pntHit))
            return MarkerCenterTop;
    }
    if (m_typeMarker & MarkerCenterBottom)
    {
        QRectF rc = rect();
        rc.setSize(m_masterDrag->markerSize(MarkerCenterBottom));
        if (basisCenter)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_EX_MARKER_CENTER_BOTTOM], this));
        else if (basis)
            rc.moveTo(qtn_DPIScaledF(position[INDEX_MARKER_CENTER_BOTTOM], this));
        if (rc.contains(pntHit))
            return MarkerCenterBottom;
    }
    return MarkerNone;
}

void DockWidgetMarkerWidget::drawMarker(QPainter* p, QPointF* position, CommonStyle::StandardPixmapEx sp, bool active, bool basis) const
{
    DockMarkerStyleOption opt;
    opt.initFrom(this);
    if (active)
    {
        opt.state |= QStyle::State_Active;
        opt.palette.setCurrentColorGroup(QPalette::Active);
    }
    else
    {
        opt.state &= ~QStyle::State_Active;
        opt.palette.setCurrentColorGroup(QPalette::Inactive);
    }
    opt.sp = static_cast<QStyle::StandardPixmap>(sp);
    opt.position = *position;
    opt.basis = basis;
    style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(CommonStyle::PE_DockMarker), &opt, p, this);
}

void DockWidgetMarkerWidget::drawMarkers(QPainter* p)
{
    const bool basis = (m_typeMarker & MarkerBasis) == MarkerBasis;
    const bool basisCenter = (m_typeMarker & MarkerCenterBasis) == MarkerCenterBasis;

    QPointF* position = arrMarkersPosition;
    int basisMarker = basis ? 0 : 11;
    int basisCenterMarker = basisCenter ? 0 : 11;

    p->save();
    p->setBackgroundMode(Qt::TransparentMode);
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    if (basisCenter)
    {
        drawMarker(p, &position[INDEX_EX_MARKER_CENTER_BASIS], CommonStyle::SP_DockMarkerBasisEx, false);

        if (m_typeMarker & MarkerCenterCenter)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_CENTER_CENTER], CommonStyle::SP_DockMarkerCenter, m_selectedMarker == MarkerCenterCenter, basisCenter);

        if (m_typeMarker & MarkerLeft)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_LEFT], CommonStyle::SP_DockMarkerCenterLeft, m_selectedMarker == MarkerLeft, basisCenter);

        if (m_typeMarker & MarkerCenterLeft)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_CENTER_LEFT], CommonStyle::SP_DockMarkerCenterSplitLeft, m_selectedMarker == MarkerCenterLeft, basisCenter);

        if (m_typeMarker & MarkerTop)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_TOP], CommonStyle::SP_DockMarkerCenterTop, m_selectedMarker == MarkerTop, basisCenter);

        if (m_typeMarker & MarkerCenterTop)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_CENTER_TOP], CommonStyle::SP_DockMarkerCenterSplitTop, m_selectedMarker == MarkerCenterTop, basisCenter);

        if (m_typeMarker & MarkerRight)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_RIGHT], CommonStyle::SP_DockMarkerCenterRight, m_selectedMarker == MarkerRight, basisCenter);

        if (m_typeMarker & MarkerCenterRight)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_CENTER_RIGHT], CommonStyle::SP_DockMarkerCenterSplitRight, m_selectedMarker == MarkerCenterRight, basisCenter);

        if (m_typeMarker & MarkerBottom)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_BOTTOM], CommonStyle::SP_DockMarkerCenterBottom, m_selectedMarker == MarkerBottom, basisCenter);

        if (m_typeMarker & MarkerCenterBottom)
            drawMarker(p, &position[basisCenterMarker + INDEX_EX_MARKER_CENTER_BOTTOM], CommonStyle::SP_DockMarkerCenterSplitBottom, m_selectedMarker == MarkerCenterBottom, basisCenter);
    }
    else if (basis)
    {
        drawMarker(p, &position[INDEX_MARKER_BASIS], CommonStyle::SP_DockMarkerBasis, false);

        if (m_typeMarker & MarkerCenter)
            drawMarker(p, &position[INDEX_MARKER_CENTER], CommonStyle::SP_DockMarkerCenter, m_selectedMarker == MarkerCenter, basis);

        if (m_typeMarker & MarkerTop)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_TOP], CommonStyle::SP_DockMarkerBasisTop, m_selectedMarker == MarkerTop, basis);

        if (m_typeMarker & MarkerLeft)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_LEFT], CommonStyle::SP_DockMarkerBasisLeft, m_selectedMarker == MarkerLeft, basis);

        if (m_typeMarker & MarkerBottom)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_BOTTOM], CommonStyle::SP_DockMarkerBasisBottom, m_selectedMarker == MarkerBottom, basis);

        if (m_typeMarker & MarkerRight)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_RIGHT], CommonStyle::SP_DockMarkerBasisRight, m_selectedMarker == MarkerRight, basis);

        // CenterCenter
        if (m_typeMarker & MarkerCenterCenter)
            drawMarker(p, &position[INDEX_MARKER_CENTER], CommonStyle::SP_DockMarkerCenter, m_selectedMarker == MarkerCenterCenter, basis);

        if (m_typeMarker & MarkerCenterLeft)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_CENTER_LEFT], CommonStyle::SP_DockMarkerCenterSplitLeft, m_selectedMarker == MarkerCenterLeft, basis);

        if (m_typeMarker & MarkerCenterRight)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_CENTER_RIGHT], CommonStyle::SP_DockMarkerCenterSplitRight, m_selectedMarker == MarkerCenterRight, basis);

        if (m_typeMarker & MarkerCenterTop)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_CENTER_TOP], CommonStyle::SP_DockMarkerCenterSplitTop, m_selectedMarker == MarkerCenterTop, basis);

        if (m_typeMarker & MarkerCenterBottom)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_CENTER_BOTTOM], CommonStyle::SP_DockMarkerCenterSplitBottom, m_selectedMarker == MarkerCenterBottom, basis);
    }
    else
    {
        if (m_typeMarker & MarkerTop)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_TOP], CommonStyle::SP_DockMarkerTop, m_selectedMarker == MarkerTop, basis);

        if (m_typeMarker & MarkerLeft)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_LEFT], CommonStyle::SP_DockMarkerLeft, m_selectedMarker == MarkerLeft, basis);

        if (m_typeMarker & MarkerBottom)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_BOTTOM], CommonStyle::SP_DockMarkerBottom, m_selectedMarker == MarkerBottom, basis);

        if (m_typeMarker & MarkerRight)
            drawMarker(p, &position[basisMarker + INDEX_MARKER_RIGHT], CommonStyle::SP_DockMarkerRight, m_selectedMarker == MarkerRight, basis);
    }
    p->restore();
}

void DockWidgetMarkerWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    drawMarkers(&p);
}

void DockWidgetMarkerWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (isVisible())
        updateMask(event->size());
}

void DockWidgetMarkerWidget::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    updateMask(size());
}

void DockWidgetMarkerWidget::updateMask(const QSize& size)
{
    QPixmap bg(size.width(), size.height());
    QColor maskColor = Qt::magenta;
    maskColor.setAlpha(1);
    bg.fill(maskColor);
    {
        QPainter p(&bg);
        drawMarkers(&p);
    }
    setMask(bg.createMaskFromColor(maskColor));
}

/* DockWidgetDragDrop */
DockWidgetDragDrop::DockWidgetDragDrop(DockPanelManager* dockManager)
    : m_dockManager(dockManager)
    , m_markerDockLayout(Q_NULL)
    , m_lastMarkerDockLayout(Q_NULL)
    , m_dockPanel(Q_NULL)
    , m_layoutPanel(Q_NULL)
    , m_panelPhantomWidget(Q_NULL)
    , m_framePhantomWidget(Q_NULL)
    , m_startDrag(false)
    , m_attachLast(false)
    , m_attach(false)
    , m_sideDock(false)
    , m_centerDock(false)
    , m_floatable(true)
    , m_dockable(true)
    , m_useDockingMarkers(false)
    , m_usePanelTransparentDragDrop(false)
    , m_szTab(qtn_sizeHintTabBar(dockManager->managedWidget()))
    , m_containDirection(Qtitan::NoDockPanelArea)
{
}

DockWidgetDragDrop::~DockWidgetDragDrop()
{
    Q_DELETE_AND_NULL(m_panelPhantomWidget);
    Q_DELETE_AND_NULL(m_framePhantomWidget);
}

void DockWidgetDragDrop::execDrag(DockPanelBase* dockPanel, const QPoint& pos)
{
    QRect initializeRect = qtn_mapPanelToGlobalRect(dockPanel);
    DockWindow* dockWindow = qobject_cast<DockWindow*>(dockPanel->dockWindow());
    m_usePanelTransparentDragDrop = m_dockManager->isDockPanelTransparentWhileDragging();
    m_useDockingMarkers = m_dockManager->isArrowMarkersShown() && m_usePanelTransparentDragDrop;
    const QPoint cursorPos = QCursor::pos();
    {
        DockManagerUpdater managerUpdater(m_dockManager);

        if (m_dockManager->isDockPanelFullContentsWhileDraggingShown() && 
            (dockWindow == Q_NULL || !(qtn_checkLayoutType(dockPanel, DockLayoutPanel::Split) ||
            qtn_checkLayoutType(dockPanel, DockLayoutPanel::Document))))
        {
            initializeRect.moveTopLeft(cursorPos - pos);
            dockWindow = m_dockManager->addDockWindow(dockPanel, initializeRect);
            if (dockWindow == Q_NULL)
            {
                stopDrag();
                return;
            }
            dockPanel = dockWindow->layoutPanel();
        }
    }
#ifdef Q_OS_WIN // System "Show contents when dragging" option is off
    DWORD value = 0;
    SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &value, SPIF_SENDCHANGE);
    if (value == 0)
        SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, 0, 0);
#endif
    if (m_dockManager->isDockPanelFullContentsWhileDraggingShown())
    {
        Q_ASSERT(dockWindow != Q_NULL);
        initDrag(dockPanel, pos, dockWindow != Q_NULL? dockWindow->geometry() : dockPanel->rect());
        WindowTitleBar::startDrag(dockWindow, cursorPos);
    }
    else
    {
        initializeRect.moveTopLeft(cursorPos - pos);
        initDrag(dockPanel, pos, dockWindow ? dockWindow->geometry() : initializeRect);
        m_framePhantomWidget->startDrag(cursorPos);
    }
#ifdef Q_OS_WIN
    if (value == 0)
        SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, 0, 0);
#endif
}

void DockWidgetDragDrop::initDrag(DockPanelBase* dockPanel, const QPoint& point, const QRect& initializeRect)
{
    Q_ASSERT(m_dockPanel == Q_NULL);
    m_dockPanel = dockPanel;

    QRect rect = initializeRect;
    m_pntSnap = m_pntLast = point;
    if (m_usePanelTransparentDragDrop)
        qtn_adjustRectangle(rect, point, m_dockManager->managedWidget());

    m_rectDragFrame = rect;
    m_layoutPanel = Q_NULL;
    m_attach = m_attachLast = false;
    m_floatable = true;
    m_dockable = true;
    m_sideDock = false;
    m_centerDock = false;

    QList<DockPanelBase*> lst = dockPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
    for (QList<DockPanelBase*>::iterator it = lst.begin(); it != lst.end(); ++it)
    {
        DockWidgetPanel* pDockWidget = (DockWidgetPanel*)(*it);
        if ((pDockWidget->features() & DockWidgetPanel::DockPanelFloatable) == 0)
            m_floatable = false;
        if ((pDockWidget->features() & DockWidgetPanel::DockPanelDockable) == 0)
            m_dockable = false;
        Qtitan::DockPanelAreas allowedAreas = DockLayoutPanelPrivate::isAllowedAreas(pDockWidget, DockWidgetPanel::staticMetaObject.className());
        if (allowedAreas.testFlag(Qtitan::NoDockPanelArea))
            m_dockable = false;
    }

    if (!m_dockManager->isDockPanelFullContentsWhileDraggingShown())
    {
        m_framePhantomWidget = new DockWidgetPhantomWidget(this, !m_usePanelTransparentDragDrop);
        m_framePhantomWidget->setStartRect(initializeRect);
    }
    destroyMarkers();
}

static QRect nonVisibleWindowRect = { -1, -1, 1, 1 };

void DockWidgetDragDrop::startDrag()
{
    Q_ASSERT(!m_startDrag);

    m_startDrag = true;
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    m_rectLast = QRect();
    m_rectMarkerDockWidget = QRect();
    m_markerDockLayout = Q_NULL;
    m_lastMarkerDockLayout = Q_NULL;

    Q_ASSERT(m_panelPhantomWidget == Q_NULL);
    if (m_dockManager->isDockPanelFullContentsWhileDraggingShown())
    {
        m_panelPhantomWidget = new DockWidgetPhantomWidget(this);
        m_panelPhantomWidget->show();
        m_panelPhantomWidget->setGeometry(nonVisibleWindowRect);
    }
}

void DockWidgetDragDrop::stopDrag()
{
    Q_ASSERT(m_startDrag);

    QApplication::restoreOverrideCursor();
    m_dockPanel = Q_NULL;
    m_startDrag = false;
    m_rectDragFrame = QRect();

    destroyMarkers();
    Q_DELETE_AND_NULL(m_panelPhantomWidget);
    Q_DELETE_AND_NULL(m_framePhantomWidget);

    if (!m_dockManager->isDockPanelFullContentsWhileDraggingShown())
    {
        updateRect(true);
    }
}

void DockWidgetDragDrop::acceptDrag()
{
    if (m_pntSnap == QPoint(0, 0))
    {
        DockManagerUpdater managerUpdater(m_dockManager);
        if (m_attach)
        {
            Q_ASSERT(qtn_checkLayoutType(m_layoutPanel, DockLayoutPanel::Tabbed) || qtn_checkLayoutType(m_layoutPanel, DockLayoutPanel::Document));
            DockPanelManagerPrivate::_get(m_dockManager)->tabifyDockPanel(m_dockPanel, m_layoutPanel);
        }
        else if (m_sideDock && m_layoutPanel != Q_NULL && m_layoutPanel == centralPanel())
        {
            QRect rectSide(m_rectLayout);
            rectSide.translate(-m_layoutPanel->geometry().topLeft());
        }
        else if (m_layoutPanel)
        {
            if (m_centerDock)
                DockPanelManagerPrivate::_get(m_dockManager)->insertCenterDockPanel(m_dockPanel, m_containDirection, m_layoutPanel);
            else
            {
                if (!m_dockManager->isDockPanelFullContentsWhileDraggingShown())
                {
                    QList<DockPanelBase*> listPanel = m_dockPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
                    for (QList<DockPanelBase*>::iterator it = listPanel.begin(); it != listPanel.end(); ++it)
                        DockWidgetPanelPrivate::_get((DockWidgetPanel*)(*it))->setDragFrameRect(m_rectDragFrame);
                }
                DockPanelManagerPrivate::_get(m_dockManager)->insertDockPanelBefore(m_dockPanel, m_containDirection, m_layoutPanel);
            }
            DockPanelManagerPrivate::_get(m_dockManager)->ensureVisible(m_layoutPanel);
        }
        else if (m_floatable)
        {
            if (m_framePhantomWidget != Q_NULL && !m_dockManager->isDockPanelFullContentsWhileDraggingShown())
                m_rectDragFrame = m_framePhantomWidget->geometry();

            if (qtn_checkLayoutType(m_dockPanel, DockLayoutPanel::Split) || qtn_checkLayoutType(m_dockPanel, DockLayoutPanel::Document))
                m_dockPanel->parentWidget()->setGeometry(m_rectDragFrame);
            else
                m_dockManager->addDockWindow(m_dockPanel, m_rectDragFrame);
        }
    }
}


bool DockWidgetDragDrop::isDragInitialized() const
{
    return m_startDrag && m_dockPanel != Q_NULL;
}


bool DockWidgetDragDrop::isStartDrag() const
{
    return m_startDrag;
}

QSize DockWidgetDragDrop::markerSize(DockWidgetMarkerWidget::DockingWidgetMarkerType type) const
{
    int value40 = CommonStylePrivate::dpiScaled(40.0, m_dockManager->managedWidget());
    int value112 = CommonStylePrivate::dpiScaled(112.0, m_dockManager->managedWidget());
    int value186 = CommonStylePrivate::dpiScaled(186.0, m_dockManager->managedWidget());

    const int cx = value40;
    const int cy = value40;
    int sz = value112;
    if (type == DockWidgetMarkerWidget::MarkerCenterBasis)
        sz = value186;
    if (type == DockWidgetMarkerWidget::MarkerCenter || type == DockWidgetMarkerWidget::MarkerCenterCenter)
        return QSize(cy, cx);
    if (type == DockWidgetMarkerWidget::MarkerTop || type == DockWidgetMarkerWidget::MarkerCenterTop)
        return QSize(cy, cx);
    if (type == DockWidgetMarkerWidget::MarkerLeft || type == DockWidgetMarkerWidget::MarkerCenterLeft)
        return QSize(cx, cy);
    if (type == DockWidgetMarkerWidget::MarkerBottom || type == DockWidgetMarkerWidget::MarkerCenterBottom)
        return QSize(cy, cx);
    if (type == DockWidgetMarkerWidget::MarkerRight || type == DockWidgetMarkerWidget::MarkerCenterRight)
        return QSize(cx, cy);
    return QSize(sz, sz);
}

DockPanelBase* DockWidgetDragDrop::centralPanel() const
{ return DockPanelManagerPrivate::_get(m_dockManager)->centralPanel(); }

void DockWidgetDragDrop::updateMarkers()
{
    if (m_rectMarkerDockWidget.isEmpty())
    {
        m_lastMarkerDockLayout = Q_NULL;
        setInvisibleMarkers();
        return;
    }

    if (m_lastMarkerDockLayout != m_markerDockLayout || m_lstMarkers.isEmpty())
    {
        setInvisibleMarkers();

        if (m_markerDockLayout)
        {
            DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(m_markerDockLayout);
            const bool documentLayout = layoutPanel && qtn_parentDocumentLayout(layoutPanel);

            DockWidgetMarkerWidget::DockingWidgetMarkerType allowMarkers = DockWidgetMarkerWidget::MarkerNone;
            if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::LeftDockPanelArea))   allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerLeft);
            if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::RightDockPanelArea))  allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerRight);
            if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::BottomDockPanelArea)) allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerBottom);
            if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::TopDockPanelArea))    allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerTop);
            const bool saveAllowMarkers = allowMarkers == DockWidgetMarkerWidget::MarkerNone;

            QSize sz(markerSize(documentLayout && !saveAllowMarkers ? DockWidgetMarkerWidget::MarkerCenterBasis : DockWidgetMarkerWidget::MarkerBasis));
            QRect rc(m_rectMarkerDockWidget.center(), sz);
            rc.translate(-sz.width() / 2, -sz.height() / 2);

            if (documentLayout)
            {
                if (qobject_cast<DockDocumentTabbedLayoutPanel* > (m_markerDockLayout))
                {
                    if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::LeftDockPanelArea,   true)) allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerCenterLeft);
                    if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::RightDockPanelArea,  true)) allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerCenterRight);
                    if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::TopDockPanelArea,    true)) allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerCenterTop);
                    if (isDockToAreaEnabled(m_markerDockLayout, Qtitan::BottomDockPanelArea, true)) allowMarkers = DockWidgetMarkerWidget::DockingWidgetMarkerType(allowMarkers + DockWidgetMarkerWidget::MarkerCenterBottom);
                }

                if (saveAllowMarkers)
                    createMarker(rc, !isAppendEnabled(m_markerDockLayout) ? DockWidgetMarkerWidget::DockingWidgetMarkerType(DockWidgetMarkerWidget::MarkerBasis + allowMarkers) :
                        DockWidgetMarkerWidget::DockingWidgetMarkerType(DockWidgetMarkerWidget::MarkerBasis + allowMarkers + DockWidgetMarkerWidget::MarkerCenterCenter));
                else
                    createMarker(rc, !isAppendEnabled(m_markerDockLayout) ?
                        DockWidgetMarkerWidget::DockingWidgetMarkerType(DockWidgetMarkerWidget::MarkerCenterBasis + allowMarkers) :
                        DockWidgetMarkerWidget::DockingWidgetMarkerType(DockWidgetMarkerWidget::MarkerCenterBasis + allowMarkers + DockWidgetMarkerWidget::MarkerCenterCenter));
            }
            else

            {
                createMarker(rc, m_markerDockLayout == centralPanel() || !isAppendEnabled(m_markerDockLayout) ?
                    DockWidgetMarkerWidget::DockingWidgetMarkerType(DockWidgetMarkerWidget::MarkerBasis + allowMarkers) :
                    DockWidgetMarkerWidget::DockingWidgetMarkerType(DockWidgetMarkerWidget::MarkerBasis + allowMarkers + DockWidgetMarkerWidget::MarkerCenter));
            }
        }

        DockPanelBase* topPanel = m_dockManager->layoutPanel();
        if (m_markerDockLayout)
        {
            if (const DockWindow* dockWindow = qobject_cast<const DockWindow*>(m_markerDockLayout->topDockWindow()))
            {
                if (qobject_cast<DockDocumentTabbedLayoutPanel*>(m_markerDockLayout))
                    topPanel = dockWindow->layoutPanel();
                else
                {
                    m_lastMarkerDockLayout = m_markerDockLayout;
                    return;
                }
            }
        }

        QRect rcWindow = topPanel->geometry();
        QPoint pnt = topPanel->parentWidget()->mapToGlobal(rcWindow.topLeft());
        rcWindow.moveTopLeft(pnt);

        if (isDockToAreaEnabled(topPanel, Qtitan::TopDockPanelArea))
            createMarker(QRect(QPoint(rcWindow.center().x() - markerSize(DockWidgetMarkerWidget::MarkerTop).width() / 2, rcWindow.top() + 16), 
                markerSize(DockWidgetMarkerWidget::MarkerTop)), DockWidgetMarkerWidget::MarkerTop);
        if (isDockToAreaEnabled(topPanel, Qtitan::LeftDockPanelArea))
            createMarker(QRect(QPoint(rcWindow.left() + 16, rcWindow.center().y() - markerSize(DockWidgetMarkerWidget::MarkerLeft).height() / 2), 
                markerSize(DockWidgetMarkerWidget::MarkerLeft)), DockWidgetMarkerWidget::MarkerLeft);
        if (isDockToAreaEnabled(topPanel, Qtitan::BottomDockPanelArea))
            createMarker(QRect(QPoint(rcWindow.center().x() - markerSize(DockWidgetMarkerWidget::MarkerBottom).width() / 2, 
                rcWindow.bottom() - 16 - markerSize(DockWidgetMarkerWidget::MarkerBottom).height()), markerSize(DockWidgetMarkerWidget::MarkerBottom)), DockWidgetMarkerWidget::MarkerBottom);
        if (isDockToAreaEnabled(topPanel, Qtitan::RightDockPanelArea))
            createMarker(QRect(QPoint(rcWindow.right() - markerSize(DockWidgetMarkerWidget::MarkerRight).width() - 16, rcWindow.center().y() - markerSize(DockWidgetMarkerWidget::MarkerRight).height() / 2), 
                markerSize(DockWidgetMarkerWidget::MarkerRight)), DockWidgetMarkerWidget::MarkerRight);
        m_lastMarkerDockLayout = m_markerDockLayout;
    }
}

void DockWidgetDragDrop::destroyMarkers()
{
    QListIterator<DockWidgetMarkerWidget*> itMarker(m_lstMarkers.values());
    while (itMarker.hasNext()) 
    {
        DockWidgetMarkerWidget* marker = itMarker.next();
        delete marker;
    }
    m_lstMarkers.clear();
}

void DockWidgetDragDrop::setInvisibleMarkers()
{
    QListIterator<DockWidgetMarkerWidget*> itMarker(m_lstMarkers.values());
    while (itMarker.hasNext())
        itMarker.next()->setVisible(false);

    if (m_dockPanel != Q_NULL && m_dockPanel->dockWindow() != Q_NULL)
        m_dockPanel->dockWindow()->raise();
}

DockWidgetMarkerWidget* DockWidgetDragDrop::createMarker(const QRect& rect, DockWidgetMarkerWidget::DockingWidgetMarkerType typeMarker)
{
    if (typeMarker == DockWidgetMarkerWidget::MarkerNone || typeMarker == DockWidgetMarkerWidget::MarkerBasis)
        return Q_NULL;

    DockWidgetMarkerWidget* marker = Q_NULL;
    if (m_lstMarkers.contains(typeMarker))
    {
        marker = m_lstMarkers.value(typeMarker);
    }
    else
    {
        marker = new DockWidgetMarkerWidget(this);
        marker->setMarkerType(typeMarker);
        m_lstMarkers.insert(typeMarker, marker);
    }
    marker->setGeometry(rect);
    marker->show();
    //marker->raise();
    return marker;
}

void DockWidgetDragDrop::updateRect(bool hide)
{
    if (m_dockManager->isDockPanelFullContentsWhileDraggingShown() && m_layoutPanel == Q_NULL &&
        m_floatable && !m_attach && !m_rectDragFrame.isEmpty())
    {
        QRect rect = m_rectDragFrame;
        Q_ASSERT(m_panelPhantomWidget != Q_NULL);
        m_panelPhantomWidget->setGeometry(nonVisibleWindowRect);

        if (qtn_checkLayoutType(m_dockPanel, DockLayoutPanel::Split) || qtn_checkLayoutType(m_dockPanel, DockLayoutPanel::Document))
        {
            if (m_rectDragFrame != m_rectLast)
                m_rectDragFrame = m_dockPanel->dockWindow()->geometry();
        }
        else if (!hide)
        {
            if (m_rectDragFrame.top() < 0)
                m_rectDragFrame.moveTopLeft(QPoint(0, -m_rectDragFrame.top()));
        }
        m_rectLast = rect;
        m_attachLast = false;
        return;
    }
    else if (m_dockManager->isDockPanelFullContentsWhileDraggingShown() && m_layoutPanel != Q_NULL && m_floatable && !m_rectDragFrame.isEmpty())
    {
        if (qtn_checkLayoutType(m_dockPanel, DockLayoutPanel::Split) || qtn_checkLayoutType(m_dockPanel, DockLayoutPanel::Document))
        {
            QRect rect = m_rectDragFrame;
            m_rectLast = rect;
        }
    }

    if (m_dockManager->isDockPanelFullContentsWhileDraggingShown())
    {
        Q_ASSERT(!hide);
        QRect rect = (m_layoutPanel == Q_NULL) ? m_rectDragFrame : m_rectLayout;
        m_rectLast = rect;

        if (!m_floatable && m_layoutPanel == Q_NULL)
            rect = QRect();

        if (m_attach != m_attachLast)
        {
            m_attachLast = m_attach;
            Q_ASSERT(m_panelPhantomWidget != Q_NULL);
            m_panelPhantomWidget->setGeometry(nonVisibleWindowRect);
        }
        
        QRect rcTab;
        Qtitan::LayoutArea pos = Qtitan::DefaultArea;
        if (m_attach)
        {
            QSize szTab(::qMin(m_szTab.width(), rect.width() - offsetTab), ::qMin(m_szTab.height(), rect.height() / 2));
            pos = Qtitan::BottomArea;
            if (DockDocumentLayoutPanel* documentLayoutPanel = qobject_cast<DockDocumentLayoutPanel*>(m_markerDockLayout))
                pos = documentLayoutPanel->tabPosition();
            else if (DockDocumentTabbedLayoutPanel* tabbedLayoutPanel = qobject_cast<DockDocumentTabbedLayoutPanel*>(m_markerDockLayout))
                pos = tabbedLayoutPanel->tabPosition();

            if (pos == Qtitan::TopArea && m_layoutPanel && qtn_checkLayoutType(m_layoutPanel, DockLayoutPanel::Tabbed))
            {
                if (((DockTabbedLayoutPanel*)m_layoutPanel)->isTitleVisible())
                {
                    int captionHeight = ((DockTabbedLayoutPanel*)m_layoutPanel)->titleHeight();
                    rect.setTop(rect.top() + captionHeight + 3);
                }
                rect.setTop(rect.top());
                rcTab = QRect(QPoint(rect.left() + offsetTab, rect.top() - szTab.height()), 
                    QPoint(rect.left() + offsetTab + szTab.width(), rect.top() - 1));
            }
            else
            {
                if (pos == Qtitan::TopArea)
                {
                    rect.setTop(rect.top() + szTab.height());
                    rcTab = QRect(QPoint(rect.left() + offsetTab, rect.top() - szTab.height()), 
                        QPoint(rect.left() + offsetTab + szTab.width(), rect.top()));
                }
                else
                {
                    rcTab = QRect(QPoint(rect.left() + offsetTab, rect.bottom() + 1), 
                        QPoint(rect.left() + offsetTab + szTab.width(), rect.bottom() + szTab.height()));
                }
            }
        }

        if (m_rectDragFrame.isNull())
            return;

        if (m_panelPhantomWidget == Q_NULL)
        {
            Q_ASSERT(false);
            return;
        }

        if (m_panelPhantomWidget->size() != rect.size())
        {
            m_panelPhantomWidget->setPositionTab(pos);
            if (m_attach)
            {
                m_panelPhantomWidget->setDrawTab(true);
                m_panelPhantomWidget->setRectTab(rcTab);
            }
        }
        m_panelPhantomWidget->setGeometry(rect);
        //m_panelPhantomWidget->show();
        //m_panelPhantomWidget->lower();
    }
}

void DockWidgetDragDrop::moveDrag(const QPoint& pt, Qt::KeyboardModifiers modifiers)
{
    QPoint pntOffset = pt - m_pntLast;
    m_rectDragFrame.translate(pntOffset);

    m_attach = false;
    m_centerDock = false;
    m_sideDock = false;
    m_rectMarkerDockWidget = QRect();
    m_layoutPanel = Q_NULL;
    m_markerDockLayout = Q_NULL;

    if (!(modifiers & Qt::ControlModifier) && m_dockable)
        findLayout(pt);

    m_pntLast = pt;

    if (m_useDockingMarkers)
    {
        updateMarkers();

        bool found = false;
        QListIterator<DockWidgetMarkerWidget*> itMarker(m_lstMarkers.values());
        while (itMarker.hasNext()) 
        {
            DockWidgetMarkerWidget* marker = itMarker.next();
            DockWidgetMarkerWidget::DockingWidgetMarkerType selectedMarker = DockWidgetMarkerWidget::MarkerNone;

            if (!found)
            {
                DockWidgetMarkerWidget::DockingWidgetMarkerType hit = marker->hitTest(pt);
                if (hit != DockWidgetMarkerWidget::MarkerNone)
                {
                    m_attach = false;
                    if ((marker->m_typeMarker & DockWidgetMarkerWidget::MarkerBasis) == DockWidgetMarkerWidget::MarkerBasis)
                    {
                        if (m_layoutPanel && !m_markerDockLayout)
                        {
                            m_markerDockLayout = m_layoutPanel;
                            m_rectMarkerDockWidget = m_rectLayout = qtn_mapPanelToGlobalRect(m_layoutPanel);
                        }
                        m_layoutPanel = m_markerDockLayout;
                        m_rectLayout = m_rectMarkerDockWidget;
                    }
                    else if ((marker->m_typeMarker & DockWidgetMarkerWidget::MarkerCenterBasis) == DockWidgetMarkerWidget::MarkerCenterBasis)
                    {
                        if (hit != DockWidgetMarkerWidget::MarkerNone && m_layoutPanel && !m_markerDockLayout)
                        {
                            m_markerDockLayout = m_layoutPanel;
                            m_rectMarkerDockWidget = m_rectLayout = qtn_mapPanelToGlobalRect(m_layoutPanel);
                        }

                        if (hit == DockWidgetMarkerWidget::MarkerTop  || hit == DockWidgetMarkerWidget::MarkerBottom ||
                            hit == DockWidgetMarkerWidget::MarkerLeft || hit == DockWidgetMarkerWidget::MarkerRight)
                        {
                            m_layoutPanel = m_dockManager->layoutPanel();
                            if (m_markerDockLayout)
                            {
                                if (const DockWindow* dockWindow = qobject_cast<const DockWindow*>(m_markerDockLayout->topDockWindow()))
                                {
                                    if (qobject_cast<DockDocumentTabbedLayoutPanel*>(m_markerDockLayout))
                                        m_layoutPanel = dockWindow->layoutPanel();
                                }
                                else if (qobject_cast<DockDocumentTabbedLayoutPanel*>(m_markerDockLayout) || qobject_cast<DockDocumentLayoutPanel*>(m_markerDockLayout))
                                {
                                    m_layoutPanel = m_dockManager->createDocumentLayout();
                                }
                            }
                            if (m_layoutPanel)
                                m_rectLayout = qtn_mapPanelToGlobalRect(m_layoutPanel);
                        }
                        else
                        {
                            m_layoutPanel = m_markerDockLayout;
                            m_rectLayout = m_rectMarkerDockWidget;
                        }
                    }   
                    else
                    {
                        m_layoutPanel = m_dockManager->layoutPanel();
                        if (const DockWindow* dockWindow = qobject_cast<const DockWindow*>(m_markerDockLayout ? m_markerDockLayout->topDockWindow() : Q_NULL))
                            m_layoutPanel = dockWindow->layoutPanel();
                        m_rectLayout = qtn_mapPanelToGlobalRect(m_layoutPanel);
                    }

                    switch (hit)
                    {
                        case DockWidgetMarkerWidget::MarkerTop:
                        case DockWidgetMarkerWidget::MarkerCenterTop:
                            m_rectLayout.setBottom(m_rectLayout.center().y());
                            m_containDirection = Qtitan::TopDockPanelArea;
                            break;
                        case DockWidgetMarkerWidget::MarkerBottom:
                        case DockWidgetMarkerWidget::MarkerCenterBottom:
                            m_rectLayout.setTop(m_rectLayout.center().y());
                            m_containDirection = Qtitan::BottomDockPanelArea;
                            break;
                        case DockWidgetMarkerWidget::MarkerLeft:
                        case DockWidgetMarkerWidget::MarkerCenterLeft:
                            m_rectLayout.setRight(m_rectLayout.center().x());
                            m_containDirection = Qtitan::LeftDockPanelArea;
                            break;
                        case DockWidgetMarkerWidget::MarkerRight:
                        case DockWidgetMarkerWidget::MarkerCenterRight:
                            m_rectLayout.setLeft(m_rectLayout.center().x());
                            m_containDirection = Qtitan::RightDockPanelArea;
                            break;
                        case DockWidgetMarkerWidget::MarkerCenter:
                            m_attach = true;
                            break;
                        case DockWidgetMarkerWidget::MarkerCenterCenter:
                            m_containDirection = Qtitan::InsideDockPanelArea;
                            m_attach = true;
                            break;
                        default:
                            Q_ASSERT(false);
                            break;
                    }

                    bool enabled = true;
                    if ((hit != DockWidgetMarkerWidget::MarkerCenter       && 
                         hit != DockWidgetMarkerWidget::MarkerCenterCenter &&
                         hit != DockWidgetMarkerWidget::MarkerCenterLeft   &&
                         hit != DockWidgetMarkerWidget::MarkerCenterRight  &&
                         hit != DockWidgetMarkerWidget::MarkerCenterTop    &&
                         hit != DockWidgetMarkerWidget::MarkerCenterBottom) &&
                        !isDockToAreaEnabled(m_layoutPanel, m_containDirection))
                    {
                        m_layoutPanel = Q_NULL;
                        enabled = false;
                    }

                    if (hit == DockWidgetMarkerWidget::MarkerCenter && !isAppendEnabled(m_layoutPanel))
                    {
                        m_layoutPanel = Q_NULL;
                        m_attach = false;
                        enabled = false;
                    }

                    if (hit == DockWidgetMarkerWidget::MarkerCenterLeft || hit == DockWidgetMarkerWidget::MarkerCenterRight || 
                        hit == DockWidgetMarkerWidget::MarkerCenterTop || hit == DockWidgetMarkerWidget::MarkerCenterBottom )
                    {
                        m_centerDock = true;
                        m_attach = false;
                    }

                    if (enabled)
                    {
                        if (hit != DockWidgetMarkerWidget::MarkerCenter && hit != DockWidgetMarkerWidget::MarkerCenterCenter && 
                            hit != DockWidgetMarkerWidget::MarkerCenterLeft && hit != DockWidgetMarkerWidget::MarkerCenterRight &&
                            hit != DockWidgetMarkerWidget::MarkerCenterTop && hit != DockWidgetMarkerWidget::MarkerCenterBottom)
                            m_rectLayout = DockPanelManagerPrivate::_get(m_dockManager)->calculateResultDockingRect(m_dockPanel, m_containDirection, m_layoutPanel);
                        selectedMarker = hit;
                    }
                    found = true;
                }
            }
            if (marker->m_selectedMarker != selectedMarker)
            {
                marker->m_selectedMarker = selectedMarker;
                marker->update();
            }
        }
    }

    {
        QRect rect = (m_layoutPanel == Q_NULL) ? m_rectDragFrame : m_rectLayout;
        if (m_framePhantomWidget != Q_NULL)
        {
            m_framePhantomWidget->setDrawTab(m_attach);
            m_framePhantomWidget->setGeometryFrame(rect);
        }
    }

    updateRect(false);
    m_pntSnap = QPoint(0, 0);
}

static DockLayoutPanel* qtn_childAtRecursiveHelper(DockLayoutPanel* layoutPanel, const QPoint& pt)
{
    const QList<DockPanelBase*>& panels = layoutPanel->panels();
    for (QList<DockPanelBase*>::const_iterator it = panels.begin(); it != panels.end(); ++it)
    {
        if (DockLayoutPanel* layPanel = qobject_cast<DockLayoutPanel*>(*it))
        {
            if (DockLayoutPanel* findLayoutPanel = qtn_childAtRecursiveHelper(layPanel, pt))
                return findLayoutPanel;
        }
    }

    DockDocumentLayoutPanel* layPanel = qobject_cast<DockDocumentLayoutPanel*>(layoutPanel);
    if (layPanel == Q_NULL)
        layPanel = qobject_cast<DockDocumentLayoutPanel*>(layoutPanel->parentPanel());

    if (layPanel != Q_NULL)
    {
        if (!layPanel->isHidden() && layPanel->childCount() == 0)
        {
            QRect rc = qtn_mapPanelToGlobalRect(layoutPanel);
            if (rc.contains(pt))
                return layPanel;
        }
    }
    return Q_NULL;
}

bool DockWidgetDragDrop::checkCurrentPanelDocked()
{
    if (m_dockPanel == Q_NULL)
        return true;
    QList<DockPanelBase*> lstDocumentLayoutPanels = m_dockPanel->findPanels(DockDocumentLayoutPanel::staticMetaObject.className());
    if (lstDocumentLayoutPanels.count() > 0)
    {
        int countDocPanel = 0, countWidgetPanel = 0;
        QList<DockPanelBase*> lstWidgetPanels = m_dockPanel->findPanels(DockWidgetPanel::staticMetaObject.className());
        for (QList<DockPanelBase*>::const_iterator it = lstWidgetPanels.begin(); it != lstWidgetPanels.end(); ++it)
        {
            if (qobject_cast<DockDocumentPanel*>(*it))
                countDocPanel++;
            else
                countWidgetPanel++;
        }
        return countDocPanel > 0 && countWidgetPanel == 0;
    }
    return true;
}

DockLayoutPanel* DockWidgetDragDrop::childAtHelper(const QPoint& pt, bool& inadmissible)
{
    if (m_dockPanel == Q_NULL)
    {
        Q_ASSERT(false);
        return Q_NULL;
    }
    DockLayoutEngine* layoutEngine = DockPanelManagerPrivate::_get(m_dockManager)->dockLayoutEngine();
    Q_ASSERT(layoutEngine != Q_NULL);

    DockLayoutPanel* findLayoutPanel = Q_NULL;
    const QList<DockWindow*>& windowList = layoutEngine->windowList();
    for (QList<DockWindow*>::const_iterator it = windowList.begin(); it != windowList.end() && findLayoutPanel == Q_NULL; ++it)
    {
        DockWindow* window = (*it);
        if (window == Q_NULL || m_dockPanel->dockWindow() == window)
            continue;

        if (QWidget* findWidget = window->childAt(window->mapFromGlobal(pt)))
        {
            if (qobject_cast<DockPanelSplitter*>(findWidget) != Q_NULL || 
                qobject_cast<DockTitleBar*>(findWidget) != Q_NULL)
            {
                inadmissible = true;
                break;
            }
        }

        if (window->isHidden() || checkZOrderDockWindow(window->layoutPanel(), pt))
            continue;

        if (window->layoutPanel() != Q_NULL)
        {
            if (m_dockPanel != window->layoutPanel())
                findLayoutPanel = qtn_childAtRecursiveHelper(window->layoutPanel(), pt);
        }
    }
    return Q_NULL;
}

bool DockWidgetDragDrop::checkZOrderDockWindow(const DockPanelBase* panelBase, const QPoint& pt)
{
    DockWindow* dockWindow = qobject_cast<DockWindow*>(panelBase->topLevelWidget());
    if (dockWindow != Q_NULL)
    {
        DockLayoutEngine* layoutEngine = DockPanelManagerPrivate::_get(m_dockManager)->dockLayoutEngine();
        Q_ASSERT(layoutEngine != Q_NULL);

        const QList<DockWindow*>& windowList = layoutEngine->windowList();
        if (windowList.count() > 1 && windowList.at(1) != dockWindow && dockWindow->geometry().contains(pt))
            return true;
    }
    return false;
}

//#define QTN_NEXTGENDOCKENGINE

#ifdef QTN_NEXTGENDOCKENGINE
static DockPanelBase* qtn_panelHitTest2(DockPanelBase* panel, const QPoint& mousePos)
{
    Q_ASSERT(panel != Q_NULL);
    if (panel->isHidden())
        return Q_NULL;

    QRect rect = qtn_mapPanelToGlobalRect(panel);
    if (!rect.contains(mousePos))
        return Q_NULL;

    if (DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel *>(panel))
    {
        const QList<DockPanelBase*>& panels = layoutPanel->panels();
        for (QList<DockPanelBase*>::const_iterator it = panels.begin(); it != panels.end(); ++it)
        {
            panel = qtn_panelHitTest2(*it, mousePos);
            if (panel != Q_NULL)
                return panel;
        }
    }
    return panel;
}
#endif
bool DockWidgetDragDrop::checkTargetPanel(DockPanelBase* rootPanel, const QPoint& mousePos)
{
    Q_UNUSED(rootPanel);
    Q_UNUSED(mousePos);
#ifdef QTN_NEXTGENDOCKENGINE
    DockPanelBase* panelUnderMouse = qtn_panelHitTest2(rootPanel, mousePos);
    while (panelUnderMouse != Q_NULL)
    {
        if (qobject_cast<DockLayoutPanel *>(panelUnderMouse) != Q_NULL || qobject_cast<DockCentralPanel *>(panelUnderMouse) != Q_NULL)
          break;
        panelUnderMouse = panelUnderMouse->parentPanel();
    }

    if (panelUnderMouse == Q_NULL)
        panelUnderMouse = rootPanel;

    if (panelUnderMouse != Q_NULL)
    {
        QRect screenRect = qtn_mapPanelToGlobalRect(panelUnderMouse);

        if (DockTabbedLayoutPanel* tabbedPanel = qobject_cast<DockTabbedLayoutPanel *>(panelUnderMouse))
        {
            if (tabbedPanel->canDock(screenRect, mousePos) &&
                isAppendEnabled(panelUnderMouse))
            {
                m_attach = true;
                m_layoutPanel = panelUnderMouse;
                m_rectLayout = screenRect;
                return true;
            }
        }

        bool inside = panelUnderMouse != rootPanel;
        if (canDock(screenRect, mousePos, panelUnderMouse, inside))
        {
            m_layoutPanel = panelUnderMouse;
            m_rectLayout = DockPanelManagerPrivate::_get(m_dockManager)->calculateResultDockingRect(m_dockPanel, m_containDirection, m_layoutPanel);
            return true;
        }
    }
#endif
    return false;
}

void DockWidgetDragDrop::findLayout(const QPoint& pt)
{
    if (!checkCurrentPanelDocked())
        return;
    bool inadmissible = true;
    m_markerDockLayout = m_useDockingMarkers ? childAtHelper(pt, inadmissible) : Q_NULL;
    if (!inadmissible)
        return;

    if (m_markerDockLayout)
        m_rectMarkerDockWidget = qtn_mapPanelToGlobalRect(m_markerDockLayout);

    DockPanelBase* floatingWidget = Q_NULL;
    if (m_markerDockLayout == Q_NULL)
    {
#ifdef QTN_NEXTGENDOCKENGINE
        DockLayoutEngine* layoutEngine = DockPanelManagerPrivate::_get(m_dockManager)->dockLayoutEngine();
        const QList<DockWindow*>& windowList = layoutEngine->windowList();
        for (QList<DockWindow*>::ConstIterator it = windowList.constBegin(); it != windowList.constEnd(); ++it)
        {
            DockWindow* dockWindow = *it;
            if (dockWindow->isHidden() || m_dockPanel->topDockWindow() == dockWindow)
                continue;
            if (checkTargetPanel(dockWindow->layoutPanel(), pt))
                return;
        }
        if (checkTargetPanel(m_dockManager->layoutPanel(), pt))
        {
        }
        return;
#endif
        const QList<DockWidgetPanel*>& list = DockPanelManagerPrivate::_get(m_dockManager)->dockLayoutEngine()->widgetPanelList();
        for (QList<DockWidgetPanel*>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
        {
            DockWidgetPanel* widgetPanel = *it;
            DockPanelBase* dockPanel = widgetPanel->parentPanel();
            if (dockPanel == Q_NULL)
                continue;

            Q_ASSERT(qtn_checkLayoutType(dockPanel, DockLayoutPanel::Tabbed));

            DockPanelBase* parentPanel = dockPanel->parentPanel();
            if (dockPanel->isHidden())
                continue;

            if (parentPanel == Q_NULL || !(qtn_checkLayoutType(parentPanel, DockLayoutPanel::Split) || 
                qtn_checkLayoutType(parentPanel, DockLayoutPanel::Document)))
                continue;

            if (DockPanelBasePrivate::_get(m_dockPanel)->containsPanel(dockPanel) != -1)
                continue;

            if (checkZOrderDockWindow(widgetPanel, pt))
                continue;

            QRect rcClient = qtn_mapPanelToGlobalRect(dockPanel);

            if (rcClient.contains(pt) && widgetPanel->isFloat())
            {
                if (floatingWidget == Q_NULL || isAfter(floatingWidget, dockPanel))
                    floatingWidget = dockPanel;
            }
        }

        for (QList<DockWidgetPanel*>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
        {
            DockWidgetPanel* widgetPanel = *it;
            if ((widgetPanel->features() & DockWidgetPanel::DockPanelDockable) == 0)
                continue;

            DockPanelBase* dockPanel = widgetPanel->parentPanel();
            if (dockPanel == Q_NULL)
                continue;

            if (floatingWidget != Q_NULL && floatingWidget != dockPanel)
                continue;

            Q_ASSERT(qtn_checkLayoutType(dockPanel, DockLayoutPanel::Tabbed));

            DockPanelBase* parentPanel = dockPanel->parentPanel();
            if (parentPanel == Q_NULL || !(qtn_checkLayoutType(parentPanel, DockLayoutPanel::Split) || 
                qtn_checkLayoutType(parentPanel, DockLayoutPanel::Document)))
                continue;

            if (dockPanel->isHidden())
                continue;

            if (DockPanelBasePrivate::_get(m_dockPanel)->containsPanel(dockPanel) != -1)
                continue;

            QRect rcClient = m_rectLayout = qtn_mapPanelToGlobalRect(dockPanel);
            if (((DockTabbedLayoutPanel*)dockPanel)->canDock(rcClient, pt))
            {
                if (isAppendEnabled(dockPanel))
                {
                    m_attach = true;
                    m_layoutPanel = dockPanel;
                    m_markerDockLayout = dockPanel;
                    m_rectMarkerDockWidget = m_rectLayout;
                    return;
                }
                else
                    rcClient = m_rectLayout;
            }

            if (m_useDockingMarkers)
            {
                if (rcClient.contains(pt))
                {
                    if (m_markerDockLayout == Q_NULL || isAfter(m_markerDockLayout, dockPanel))
                    {
                        m_rectMarkerDockWidget = m_rectLayout;
                        m_markerDockLayout = dockPanel;
                    }
                }
            }
            else if (canDock(rcClient, pt, dockPanel))
                return;
        }
    }

    if (floatingWidget && m_useDockingMarkers && m_markerDockLayout == Q_NULL && m_rectMarkerDockWidget.isEmpty())
    {
        DockPanelBase* dockPanel = m_dockManager->layoutPanel();
        QRect rect = qtn_mapPanelToGlobalRect(dockPanel);
        if (rect.contains(pt))
            m_rectMarkerDockWidget = rect;
    }

    if (floatingWidget)
        return;

    if (m_useDockingMarkers)
    {
        if (m_markerDockLayout)
            return;

        if (m_rectMarkerDockWidget.isEmpty())
        {
            DockPanelBase* panelBase = centralPanel();
            QRect rect = qtn_mapPanelToGlobalRect(panelBase);
            if (rect.contains(pt))
            {
                m_rectMarkerDockWidget = rect;
                m_markerDockLayout = panelBase;
                return;
            }

            panelBase = m_dockManager->layoutPanel();
            rect = qtn_mapPanelToGlobalRect(panelBase);
            if (rect.contains(pt))
            {
                m_rectMarkerDockWidget = rect;
                return;
            }
        }
    }
    else
    {
        if (DockPanelBase* panelBase = centralPanel())
        {
            m_rectLayout = qtn_mapPanelToGlobalRect(panelBase);
            if (canDock(m_rectLayout, pt, panelBase))
                return;
        }

        if (DockPanelBase* panelBase = m_dockManager->layoutPanel())
        {
            m_rectLayout = qtn_mapPanelToGlobalRect(panelBase);
            if (canDock(m_rectLayout, pt, panelBase, false))
                return;
        }
    }
}

bool DockWidgetDragDrop::canDock(const QRect& rc, const QPoint& pt, DockPanelBase* dockWidget, bool inside)
{
    qreal size = CommonStylePrivate::dpiScaled(20.0, m_dockManager->managedWidget());
    qreal ratio = CommonStylePrivate::dpiScaled(1.0, m_dockManager->managedWidget());

    if (inside && !m_floatable)
    {
        if (rc.width() > 1 && rc.height() > 1) 
            ratio = static_cast<qreal>(rc.height() / rc.width());
        size = 32000.0;
    }

    if (rc.height() == 0 || rc.width() == 0)
        return false;

    if (inside && !rc.contains(pt))
        return false;

    const int value = CommonStylePrivate::dpiScaled(20.0, /*this*/m_dockManager->managedWidget());

    if (!inside && !QRect(rc.left() - value, rc.top() - value, rc.right() + value, rc.bottom() + value).contains(pt))
        return false;

    int valInside = CommonStylePrivate::dpiScaled(inside ? 1 : -1, /*this*/m_dockManager->managedWidget());
    bool found = false;

    if (static_cast<qreal>(::qAbs(pt.y() - rc.top())) < size && valInside * (pt.y() - rc.top()) >= 0)
    {
        m_containDirection = Qtitan::TopDockPanelArea;
        size = static_cast<qreal>(::qAbs(pt.y() - rc.top()));
        found = true;
    }

    if (static_cast<qreal>(::qAbs(rc.bottom() - pt.y())) < size && valInside * (rc.bottom() - pt.y()) >= 0)
    {
        m_containDirection = Qtitan::BottomDockPanelArea;
        size = static_cast<qreal>(::qAbs(rc.bottom() - pt.y()));
        found = true;
    }

    if (static_cast<qreal>(::qAbs(pt.x() - rc.left())) * ratio < size && valInside * (pt.x() - rc.left()) >= 0)
    {
        m_containDirection = Qtitan::LeftDockPanelArea;
        size = static_cast<qreal>(::qAbs(pt.x() - rc.left()) * ratio);
        found = true;
    }

    if (static_cast<qreal>(::qAbs(rc.right() - pt.x())) * ratio < size && valInside * (rc.right() - pt.x()) >= 0)
    {
        m_containDirection = Qtitan::RightDockPanelArea;
        found = true;
    }

    if (found)
    {
        if (!isDockToAreaEnabled(dockWidget, m_containDirection))
            return false;

        m_rectLayout = DockPanelManagerPrivate::_get(m_dockManager)->calculateResultDockingRect(m_dockPanel, m_containDirection, dockWidget);
        m_layoutPanel = dockWidget;
    }
    return found;
}

bool DockWidgetDragDrop::isDockToAreaEnabled(DockPanelBase* panelBase, Qtitan::DockPanelArea area, bool center)
{
    Qtitan::DockPanelAreas allowedAreas = DockLayoutPanelPrivate::isAllowedAreas(m_dockPanel, DockWidgetPanel::staticMetaObject.className());
    if (!allowedAreas.testFlag(area) && ((allowedAreas & Qtitan::InsideDockPanelArea) == 0))
        return false;

    DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(panelBase);
    if (layoutPanel == Q_NULL && panelBase == centralPanel())
        return true;
    if (layoutPanel == Q_NULL)
        return false;
    return layoutPanel->canDock(area, m_dockPanel, center);
}

bool DockWidgetDragDrop::isAppendEnabled(DockPanelBase* widget)
{
    if (DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(widget))
        return layoutPanel->canDock(Qtitan::InsideDockPanelArea, m_dockPanel, false);
    return true;
}

bool DockWidgetDragDrop::isAfter(DockPanelBase* widgetBase, DockPanelBase* widgetCheck) const
{
    Q_UNUSED(widgetBase);
    Q_UNUSED(widgetCheck);
    return false;
}
