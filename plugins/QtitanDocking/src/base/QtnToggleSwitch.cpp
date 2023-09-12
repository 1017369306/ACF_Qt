/****************************************************************************
**
** Qtitan Library by Developer Machines (Components for Qt.C++)
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
#include <QPainterPath>
#include <QMouseEvent>
#include <QAction>
#include <QVariantAnimation>

#include "QtnStyleOption.h"
#include "QtnToggleSwitch.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE
class SwitchAnimation;
class ToggleSwitchPrivate
{
    QTN_DECLARE_PUBLIC(ToggleSwitch)
public:
    explicit ToggleSwitchPrivate();
    ~ToggleSwitchPrivate();
public:
    void init();
    void clickEvent(const QPoint& pnt);
    void startAnimation(qreal startValue, qreal finalValue, int duration);
    void animationStopped();
    void animationOffset(qreal offset);
    bool isVertical() const { return m_orientation == Qt::Horizontal; }
    void initStyleOption(ToggleSwitchStyleOption* option) const;
    void qtn_actionTriggered();
    void actionChanged();
public:
    bool m_value;
    bool m_animated;
    bool m_dontDrawThumbOn;
    bool m_dontDrawThumbOff;
    bool m_drawTextLabels;
    int m_animationDuration;
    qreal m_animationOffset;
    Qt::Orientation m_orientation;
    Qt::Alignment m_alignment;
    QPointF m_pntClick;
    QString m_strLabelOff;
    QString m_strLabelOn;
    QSize m_sizeHint;

    ToggleSwitch::Shape m_shape;
    SwitchAnimation* m_animation;
    QAction* m_defaultAction;
private:
    Q_DISABLE_COPY(ToggleSwitchPrivate)
};

/* DragTabAnimation */
class SwitchAnimation : public QVariantAnimation
{
public:
    SwitchAnimation(ToggleSwitchPrivate* box) : m_switchBox(box)
    {
        setEasingCurve(QEasingCurve::Linear);
    }
    ~SwitchAnimation() {}
    virtual void updateCurrentValue(const QVariant& current)
    {
        m_switchBox->animationOffset(current.toReal());
    }
    virtual void updateState(State newState, State)
    {
        if (newState == Stopped) m_switchBox->animationStopped();
    }
private:
    ToggleSwitchPrivate* m_switchBox;
};
QTITAN_END_NAMESPACE

ToggleSwitchPrivate::ToggleSwitchPrivate()
    : m_value(false)
    , m_animated(true)
    , m_dontDrawThumbOn(false)
    , m_dontDrawThumbOff(false)
    , m_drawTextLabels(true)
    , m_animationDuration(200)
    , m_animationOffset(0.0)
    , m_orientation(Qt::Horizontal)
    , m_alignment(Qt::AlignLeft)
    , m_strLabelOff(QObject::tr("Off"))
    , m_strLabelOn(QObject::tr("On"))
    , m_sizeHint(QSize())
    , m_shape(ToggleSwitch::Circle)
    , m_animation(Q_NULL)
    , m_defaultAction(Q_NULL)
{
}

ToggleSwitchPrivate::~ToggleSwitchPrivate()
{
    Q_DELETE_AND_NULL(m_animation);
}

void ToggleSwitchPrivate::init()
{
    QTN_P(ToggleSwitch);
    p.setFocusPolicy(Qt::FocusPolicy(p.style()->styleHint(QStyle::SH_Button_FocusPolicy)));
    p.setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    p.setAttribute(Qt::WA_WState_OwnSizePolicy, false);
    p.setForegroundRole(QPalette::ButtonText);
    p.setBackgroundRole(QPalette::Button);
}

void ToggleSwitchPrivate::clickEvent(const QPoint& pnt)
{
    QTN_P(ToggleSwitch);
    p.setOn(!m_value);
    if (p.isAnimated())
    {
        m_pntClick = pnt;
        return;
    }
}

void ToggleSwitchPrivate::startAnimation(qreal startValue, qreal finalValue, int duration)
{
    if (m_animation == Q_NULL)
        m_animation = new SwitchAnimation(this);
    m_animation->setStartValue(startValue);
    m_animation->setEndValue(finalValue);
    m_animation->setDuration(duration);
    m_animation->start();
}

void ToggleSwitchPrivate::animationStopped()
{
    if (m_animation != Q_NULL)
        m_animation->stop();
    m_animationOffset = 0.0;
}

void ToggleSwitchPrivate::animationOffset(qreal offset)
{
    QTN_P(ToggleSwitch);
    m_animationOffset = offset;
    p.update();
}

void ToggleSwitchPrivate::initStyleOption(ToggleSwitchStyleOption* option) const
{
    QTN_P(const ToggleSwitch);
    if (option == Q_NULL)
        return;

    option->initFrom(&p);
    if (p.isOn())
    {
        option->state |= QStyle::State_On;
        option->statusText = m_strLabelOn;
    }
    else
    { 
        option->state |= QStyle::State_Off;
        option->statusText = m_strLabelOff;
    }
#if 0
    bool keyboard = option->state & QStyle::State_KeyboardFocusChange;
    bool keyboard = p.style()->styleHint(QStyle::SH_UnderlineShortcut, option);
#endif

    option->subControls = CommonStyle::SC_ToggleSwitchThumb | CommonStyle::SC_ToggleSwitchGroove;
    if (m_drawTextLabels)
        option->subControls |= CommonStyle::SC_ToggleSwitchText;

    QFontMetrics fm = option->fontMetrics;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int widthX = fm.horizontalAdvance(QLatin1Char('x'));
#else
    int widthX = fm.width(QLatin1Char('x'));
#endif
    QSize sizeLabelOn = p.style()->itemTextRect(fm, QRect(), Qt::TextShowMnemonic, false, m_strLabelOn).size();
    sizeLabelOn.setWidth(sizeLabelOn.width() + widthX);
    QSize sizeLabelOff = p.style()->itemTextRect(fm, QRect(), Qt::TextShowMnemonic, false, m_strLabelOff).size();
    sizeLabelOff.setWidth(sizeLabelOff.width() + widthX);

    qreal width = qMax(sizeLabelOn.width(), sizeLabelOff.width());
    qreal height = qMax(sizeLabelOn.height(), sizeLabelOff.height());

    qreal thumbsSize = (m_shape != ToggleSwitch::Circle && m_shape != ToggleSwitch::CircleSmallGroove) ? width * 2.0 / 3.0 : width;
    int h = p.style()->pixelMetric(QStyle::PM_IndicatorHeight, Q_NULL, &p);
    height = qMax(height, static_cast<qreal>(h));

    option->sizeIndicator.setWidth(width + thumbsSize);
    option->sizeIndicator.setHeight(height + widthX);

    option->shape = m_shape;
    option->isAnimated = m_animated;
    option->animationOffset = m_animationOffset;
    option->dontDrawThumbOn = m_dontDrawThumbOn;
    option->dontDrawThumbOff = m_dontDrawThumbOff;
    option->orientation = m_orientation;
    option->alignment = m_alignment;
    option->text = p.text();
}

void ToggleSwitchPrivate::qtn_actionTriggered()
{
//    QTN_P(ToggleSwitch);
//    if (QAction *action = qobject_cast<QAction *>(p.sender()))
//        emit q->triggered(action);
}

void ToggleSwitchPrivate::actionChanged()
{
    QTN_P(ToggleSwitch);
    if (m_defaultAction == Q_NULL)
        return;
    p.setText(m_defaultAction->iconText());
    p.setToolTip(m_defaultAction->toolTip());
    p.setStatusTip(m_defaultAction->statusTip());
    p.setOn(m_defaultAction->isChecked());
    p.setEnabled(m_defaultAction->isEnabled());
    p.setFont(m_defaultAction->font());
    if (!p.isWindow())
        p.setVisible(m_defaultAction->isVisible());
}

/* ToggleSwitch */
ToggleSwitch::ToggleSwitch(QWidget* widget)
    : QAbstractButton(widget)
{
    QTN_INIT_PRIVATE(ToggleSwitch);
    QTN_D(ToggleSwitch);
    d.init();
}

ToggleSwitch::~ToggleSwitch()
{
    QTN_FINI_PRIVATE();
}

bool ToggleSwitch::isOn() const
{
    QTN_D(const ToggleSwitch);
    return d.m_value;
}

void ToggleSwitch::setOn(bool set)
{
    QTN_D(ToggleSwitch);
    if (d.m_value == set)
        return;

    d.m_value = set;
    if (isAnimated())
        d.animationStopped();
    if (isAnimated() && d.m_animationDuration != 0)
        d.startAnimation(1.0, 0.0, d.m_animationDuration);
    d.m_sizeHint = QSize();
    update();

    if (d.m_defaultAction != Q_NULL)
        d.m_defaultAction->setChecked(d.m_value);
//      d.m_defaultAction->trigger();

    emit toggled(d.m_value);
}

Qt::Alignment ToggleSwitch::alignmentText() const
{
    QTN_D(const ToggleSwitch);
    return d.m_alignment;
}

void ToggleSwitch::setAlignmentText(Qt::Alignment alignment)
{
    QTN_D(ToggleSwitch);
    if (d.m_alignment == alignment)
        return;
    d.m_alignment = alignment;
    d.m_sizeHint = QSize();
    update();
    updateGeometry();
}

QString ToggleSwitch::labelOff() const
{
    QTN_D(const ToggleSwitch);
    return d.m_strLabelOff;
}

void ToggleSwitch::setLabelOff(const QString& off)
{
    QTN_D(ToggleSwitch);
    d.m_strLabelOff = off;
    d.m_sizeHint = QSize();
}

QString ToggleSwitch::labelOn() const
{
    QTN_D(const ToggleSwitch);
    return d.m_strLabelOn;
}

void ToggleSwitch::setLabelOn(const QString& on)
{
    QTN_D(ToggleSwitch);
    d.m_strLabelOn = on;
    d.m_sizeHint = QSize();
}

bool ToggleSwitch::isAnimated() const
{
    QTN_D(const ToggleSwitch);
    return d.m_animated;
}

void ToggleSwitch::setAnimated(bool animated)
{
    QTN_D(ToggleSwitch);
    d.m_animated = animated;
}

bool ToggleSwitch::isVisibleLabels() const
{
    QTN_D(const ToggleSwitch);
    return d.m_drawTextLabels;
}

void ToggleSwitch::setVisibleLabels(bool visible)
{
    QTN_D(ToggleSwitch);
    if (d.m_drawTextLabels != visible)
    {
        d.m_drawTextLabels = visible;
        d.m_sizeHint = QSize();
        update();
    }
}

ToggleSwitch::Shape ToggleSwitch::shape() const
{
    QTN_D(const ToggleSwitch);
    return d.m_shape;
}

void ToggleSwitch::setShape(ToggleSwitch::Shape shape)
{
    QTN_D(ToggleSwitch);
    if (d.m_shape != shape)
    {
        d.m_shape = shape;
        update(); 
    }
}

QAction* ToggleSwitch::defaultAction() const
{
    QTN_D(const ToggleSwitch);
    return d.m_defaultAction;
}

void ToggleSwitch::setDefaultAction(QAction* action)
{
    QTN_D(ToggleSwitch);
    d.m_defaultAction = action;
    if (action == Q_NULL)
        return;
    if (!actions().contains(action))
        addAction(action);
    d.actionChanged();
}

/*! \reimp */
QSize ToggleSwitch::sizeHint() const
{
    QTN_D(const ToggleSwitch);
    if (!d.m_drawTextLabels)
        return QSize(50, 20);
    if (d.m_sizeHint.isValid())
        return d.m_sizeHint;

    ensurePolished();
    QFont font = this->font();
//    font.setBold(true);
    QFontMetrics fm(font);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int widthX = fm.horizontalAdvance(QLatin1Char('x'));
#else
    int widthX = fm.width(QLatin1Char('x'));
#endif
    QSize sizeLabelOn = style()->itemTextRect(fm, QRect(), Qt::TextShowMnemonic, false, d.m_strLabelOn).size();
    sizeLabelOn.setWidth(sizeLabelOn.width() + widthX /* 2*/);
    QSize sizeLabelOff = style()->itemTextRect(fm, QRect(), Qt::TextShowMnemonic, false, d.m_strLabelOff).size();
    sizeLabelOff.setWidth(sizeLabelOff.width() + widthX/* 2*/);

    qreal width  = qMax(sizeLabelOn.width(), sizeLabelOff.width());
    qreal height = qMax(sizeLabelOn.height(), sizeLabelOff.height());

    if (!text().isEmpty())
    {
        QSize sizeLabelText = style()->itemTextRect(fm, QRect(), Qt::TextShowMnemonic, false, text()).size();
        int spacing = style()->pixelMetric((QStyle::PixelMetric)CommonStyle::PM_ToggleSwitchLabelSpacing, Q_NULL, this);
        width = width + sizeLabelText.width() + widthX * 2 + spacing;
    }

    int w = style()->pixelMetric(QStyle::PM_IndicatorWidth, Q_NULL, this);
    int h = style()->pixelMetric(QStyle::PM_IndicatorHeight, Q_NULL, this);
    height = qMax(height, static_cast<qreal>(h));

    ToggleSwitch* ts = (ToggleSwitch*)this;
    int margin = style()->pixelMetric((QStyle::PixelMetric)CommonStyle::PM_ToggleSwitchMargin, Q_NULL, this);
    ts->qtn_d().m_sizeHint = QSizeF(width + w, height + margin).toSize();
    return d.m_sizeHint;
}

/*! \reimp */
QSize ToggleSwitch::minimumSizeHint() const
{
    return sizeHint();
}

/*! \reimp */
bool ToggleSwitch::event(QEvent* event)
{
    if (!isEnabled()) 
    {
        switch (event->type()) 
        {
            case QEvent::TabletPress:
            case QEvent::TabletRelease:
            case QEvent::TabletMove:
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseMove:
            case QEvent::HoverMove:
            case QEvent::HoverEnter:
            case QEvent::HoverLeave:
            case QEvent::ContextMenu:
            case QEvent::Wheel:
                return true;
            default:
                break;
        }
    }
    return QWidget::event(event);
}

/*! \reimp */
void ToggleSwitch::changeEvent(QEvent* event)
{
    QTN_D(ToggleSwitch);
    d.m_sizeHint = QSize();
    QWidget::changeEvent(event);
}

/*! \reimp */
void ToggleSwitch::actionEvent(QActionEvent* event)
{
    QTN_D(ToggleSwitch);
    QAction* action = event->action();
    switch (event->type()) 
    {
        case QEvent::ActionChanged:
            if (action == d.m_defaultAction)
                d.actionChanged(); // update toggleSwitch state
            break;
        case QEvent::ActionAdded:
            //connect(action, SIGNAL(triggered()), this, SLOT(qtn_actionTriggered()));
            break;
        case QEvent::ActionRemoved:
            if (d.m_defaultAction == action)
                d.m_defaultAction = Q_NULL;
            action->disconnect(this);
            break;
        default:
            ;
    }
    QWidget::actionEvent(event);
}

/*! \reimp */
void ToggleSwitch::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(ToggleSwitch);
    QPainter p(this);
    ToggleSwitchStyleOption opt;
    d.initStyleOption(&opt);
    style()->drawComplexControl(static_cast<QStyle::ComplexControl>(CommonStyle::CC_ToggleSwitch), &opt, &p, this);
#if 0
    p.setPen(Qt::red);
/*
    QRect rc = opt.rect.adjusted(0, 0, -opt.sizeIndicator.width(), -1);
    p.drawRect(rc);
    p.setPen(Qt::blue);
    rc = opt.rect.adjusted(rc.width(), 1, -2, -2);
*/
    QRect rc = opt.rect.adjusted(0, 0, -1, -1);
    p.drawRect(rc);
#endif
}

/*! \reimp */
void ToggleSwitch::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) 
    {
        event->ignore();
        return;
    }

    if (rect().contains(event->pos()))
    {
//        setDown(true);
////        d.m_pressed = true;
//        repaint();
//        d.emitPressed();
        event->accept();
    }
    else 
        event->ignore();
}

/*! \reimp */
void ToggleSwitch::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(ToggleSwitch);
    if ((event->buttons() & Qt::LeftButton))
        return;
    d.clickEvent(event->pos());
}

/*! \reimp */
void ToggleSwitch::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}
