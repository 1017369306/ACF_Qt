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
#include <QHash>
#include <QLabel>
#include <QStylePainter>
#include <QTextDocument>
#include <QToolTip>
#include <QMouseEvent>
#include <QToolButton>

#include "QtnToolTip.h"
#include "QtnCommonStylePrivate.h"
#include "QtnStyleHelperPrivate.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif


QTITAN_USE_NAMESPACE

struct QEffects
{
    enum Direction {
        LeftScroll  = 0x0001,
        RightScroll = 0x0002,
        UpScroll    = 0x0004,
        DownScroll  = 0x0008
    };

    typedef uint DirFlags;
};

extern void Q_GUI_EXPORT qScrollEffect(QWidget*, QEffects::DirFlags dir = QEffects::DownScroll, int time = -1);
extern void Q_GUI_EXPORT qFadeEffect(QWidget*, int time = -1);


QTITAN_BEGIN_NAMESPACE
/* ToolTipPrivate */
class ToolTipPrivate
{
    QTN_DECLARE_PUBLIC(ToolTip)
public:
    explicit ToolTipPrivate();
public:
    void updateTool();
public:
    int m_indent;
    int m_margin;
    int m_maxTipWidth;
    static bool m_showIcon;
    bool m_fadingOut;
    QLabel* m_label;
    QWidget* m_widget;
    static ToolTip* m_instance;
    static ToolTip::WrapMode m_wrapMode;
    QRect m_rect;
    QString m_text;
    QString m_textTitle;
    QIcon m_icon;
    QBasicTimer m_hideTimer, m_expireTimer;
};
QTITAN_END_NAMESPACE

ToolTip* ToolTipPrivate::m_instance = Q_NULL;
ToolTip::WrapMode ToolTipPrivate::m_wrapMode = ToolTip::NoWrap;
bool ToolTipPrivate::m_showIcon = true;

ToolTipPrivate::ToolTipPrivate()
    : m_indent(-1)
    , m_margin(0)
    , m_maxTipWidth(160)
    , m_fadingOut(false)
    , m_label(Q_NULL)
    , m_widget(Q_NULL)
{
}

static QString removeMnemonics(const QString &original)
{
    QString returnText(original.size(), QChar());
    int finalDest = 0;
    int currPos = 0;
    int l = original.length();
    while (l) {
        if (original.at(currPos) == QLatin1Char('&')
            && (l == 1 || original.at(currPos + 1) != QLatin1Char('&'))) {
            ++currPos;
            --l;
            if (l == 0)
                break;
        }
        else if (original.at(currPos) == QLatin1Char('(') && l >= 4 &&
            original.at(currPos + 1) == QLatin1Char('&') &&
            original.at(currPos + 2) != QLatin1Char('&') &&
            original.at(currPos + 3) == QLatin1Char(')')) {
            /* remove mnemonics its format is "\s*(&X)" */
            int n = 0;
            while (finalDest > n && returnText.at(finalDest - n - 1).isSpace())
                ++n;
            finalDest -= n;
            currPos += 4;
            l -= 4;
            continue;
        }
        returnText[finalDest] = original.at(currPos);
        ++currPos;
        ++finalDest;
        --l;
    }
    returnText.truncate(finalDest);
    return returnText;
}

void ToolTipPrivate::updateTool()
{
    QTN_P(ToolTip);
    if (m_label)
    {
        m_label->setText(::removeMnemonics(m_text));
        m_label->setWordWrap(/*Qt::mightBeRichText(m_text)*/true);
        m_label->adjustSize();
    }

    p.adjustSize();
    p.updateGeometry();
    p.update(p.contentsRect());
}

/*!
\class ToolTip
\inmodule QtitanBase
\brief Class ToolTip provides tool tips (balloon help) for any widget.
*/
/*!
Constructs ToolTip object with the given \a parent. Parameter \a title - is a title of the tooltip, \a text - is a text of the tooltip.
*/ 
ToolTip::ToolTip(const QString& title, const QString& text, const QIcon& icon, QWidget* parent)
    : QFrame(parent, Qt::WindowFlags(Qt::ToolTip | Qt::BypassGraphicsProxyWidget))
{
    delete ToolTipPrivate::m_instance;
    ToolTipPrivate::m_instance = this;

    QTN_INIT_PRIVATE(ToolTip);
    QTN_D(ToolTip);

    if (wordWrap() == NativeWrap)
        d.m_label = new QLabel(this);
    
    if (d.m_label)
    {
        d.m_label->setForegroundRole(QPalette::ToolTipText);
        d.m_label->setBackgroundRole(QPalette::ToolTipBase);
        d.m_label->setPalette(QToolTip::palette());
        d.m_label->setFont(QToolTip::font());
        d.m_label->ensurePolished();
        d.m_label->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, Q_NULL, this));
        d.m_label->setFrameStyle(QFrame::NoFrame);
    }

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
//    setPalette(QToolTip::palette());

    setFont(QToolTip::font());
    ensurePolished();
    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, Q_NULL, this));
    setFrameStyle(QFrame::NoFrame);
    qApp->installEventFilter(this);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, Q_NULL, this) / 255.0);
    setMouseTracking(true);
    d.m_fadingOut = false;
    setIcon(isIconVisible() ? icon : QIcon());
    reuseTip(title, text);
}

/*!
Destructor of the ToolTip object.
*/ 
ToolTip::~ToolTip()
{
    ToolTipPrivate::m_instance = Q_NULL;
    QTN_FINI_PRIVATE();
}

ToolTip* ToolTip::instance()
{
    return ToolTipPrivate::m_instance;
}

/*!
Shows tooltip in \a pos position. Parameter \a title - is a title of the tooltip, \a text - is a text of the tooltip, \a icon - the icon of the tooltip. 
Parameter \a parent - reference to the widget for calculating the position of tooltip.
*/ 
void ToolTip::showToolTip(const QPoint &pos, const QString& title, const QString& text, const QIcon& icon, QWidget* parent)
{
    QRect rect;
    // a tip does already exist
    if (ToolTipPrivate::m_instance && ToolTipPrivate::m_instance->isVisible())
    {
        // empty text means hide current tip
        if (text.isEmpty())
        {
            ToolTipPrivate::m_instance->hideTip();
            return;
        }
        else if (!ToolTipPrivate::m_instance->fadingOut())
        {
            // If the tip has changed, reuse the one
            // that is showing (removes flickering)
            QPoint localPos = pos;
            if (parent != Q_NULL)
                localPos = parent->mapFromGlobal(pos);
            if (ToolTipPrivate::m_instance->tipChanged(localPos, text, title, parent))
            {
                ToolTipPrivate::m_instance->setIcon(isIconVisible() ? icon : QIcon());
                ToolTipPrivate::m_instance->reuseTip(title, text);
                ToolTipPrivate::m_instance->setTipRect(parent, rect);
                ToolTipPrivate::m_instance->placeTip(pos, parent);
            }
            return;
        }
    }

    if (!text.isEmpty())
    {
        new ToolTip(title, text, icon, parent);
        ToolTipPrivate::m_instance->setTipRect(parent, rect);
        ToolTipPrivate::m_instance->placeTip(pos, parent);

#if !defined(QT_NO_EFFECTS) && !defined(Q_WS_MAC)
        if (QApplication::isEffectEnabled(Qt::UI_FadeTooltip))
            qFadeEffect(ToolTipPrivate::m_instance);
        else if (QApplication::isEffectEnabled(Qt::UI_AnimateTooltip))
            qScrollEffect(ToolTipPrivate::m_instance);
        else
            ToolTipPrivate::m_instance->showNormal();
#else
        ToolTipPrivate::m_instance->showNormal();
#endif
    }
}

/*!
Hides the tooltip window.
*/ 
void ToolTip::hideToolTip() 
{ 
    showToolTip(QPoint(), QString(), QString(), QIcon()); 
}

/*!
Returns visibility of the tooltip at the moment.
*/ 
bool ToolTip::isToolTipVisible()
{
    return (ToolTipPrivate::m_instance != Q_NULL && ToolTipPrivate::m_instance->isVisible());
}
/*
QString ToolTip::textToolTip()
{
    if (ToolTipPrivate::m_instance)
        return ToolTipPrivate::m_instance->text();
    return QString();
}
*/
/*!
Sets the wrap \a mode for the tolltip's text.
*/ 
void ToolTip::setWrapMode(WrapMode mode)
{
    ToolTipPrivate::m_wrapMode = mode;
}

/*!
Returns the wrap mode for the tooltip's text.
*/
ToolTip::WrapMode ToolTip::wordWrap()
{
    return ToolTipPrivate::m_wrapMode;
}

/*!
Sets the visibility of the tooltip's icon to \a visible. Method is static.
*/ 
void ToolTip::setIconVisible(bool visible)
{
    ToolTipPrivate::m_showIcon = visible;
}

/*!
Returns the visibility of the tooltip's icon. Method is static.
*/ 
bool ToolTip::isIconVisible()
{
    return ToolTipPrivate::m_showIcon;
}

/*!
Returns the margin \a mode for the tolltip's text in pixels.
*/ 
int ToolTip::margin() const
{
    QTN_D(const ToolTip);
    return d.m_margin;
}

/*!
Sets the margin \a mode for the tolltip's text in in pixels.
*/ 
void ToolTip::setMargin(int margin)
{
    QTN_D(ToolTip);
    if (d.m_margin == margin)
        return;
    d.m_margin = margin;
    d.updateTool();
}

void ToolTip::setTitle(const QString& text)
{
    QTN_D(ToolTip);
    if (d.m_textTitle == text)
        return;
    d.m_textTitle = text;
    d.updateTool();
}

const QString& ToolTip::title() const
{
    QTN_D(const ToolTip);
    return d.m_textTitle;
}

void ToolTip::setText(const QString& text)
{
    QTN_D(ToolTip);
    if (d.m_text == text)
        return;
    d.m_text = text;
    d.updateTool();
}

const QString& ToolTip::text() const
{
    QTN_D(const ToolTip);
    return d.m_text;
}

void ToolTip::setIcon(const QIcon& icon)
{
    QTN_D(ToolTip);
    d.m_icon = icon;
    d.updateTool();
}

const QIcon& ToolTip::icon() const
{
    QTN_D(const ToolTip);
    return d.m_icon;
}

void ToolTip::restartExpireTimer()
{
    QTN_D(ToolTip);
    int time = 10000 + 40 * qMax(0, text().length()-100);
    d.m_expireTimer.start(time, this);
    d.m_hideTimer.stop();
}

void ToolTip::reuseTip(const QString& textTitle, const QString& text)
{
    setText(text);
    setTitle(textTitle);
    QFontMetrics fm(font());
    QSize extra(1, 0);
    // Make it look good with the default ToolTip font on Mac, which has a small descent.
    if (fm.descent() == 2 && fm.ascent() >= 11)
        ++extra.rheight();

    resize(sizeHint() + extra);
    restartExpireTimer();
}

/*! \reimp */
QSize ToolTip::sizeHint() const
{
    QTN_D(const ToolTip);

    QFontMetrics fm = fontMetrics();

    QRect rcMargin(margin(), margin(), margin(), margin());
    QSize szMargin(3 + rcMargin.left() + rcMargin.right() + 3, 3 + rcMargin.top() + rcMargin.bottom() + 3);

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d.m_textTitle.isEmpty();
    bool drawImage = !d.m_icon.isNull();
    bool drawImageTop = true;
    QSize szImage(0, 0);
    QSize szTitle(0, 0);

    if (drawTitle)
    {
        QFont fontBold = font();
        fontBold.setBold(true);
        QFontMetrics fmBold(fontBold);
        int w = fmBold.averageCharWidth() * 80;
        QRect rcTitle = fmBold.boundingRect(0, 0, w, 2000, flags | Qt::TextSingleLine, d.m_textTitle);
        szTitle = QSize(rcTitle.width(), rcTitle.height() + 15);
    }

    QSize szIcon(0, 0);
    if (drawImage)
    {
        QStyleOptionFrame opt;
        opt.initFrom(this);

        if (QToolButton* button = qobject_cast<QToolButton*>(d.m_widget))
        {
            drawImageTop = button->toolButtonStyle() != Qt::ToolButtonTextUnderIcon;
            const int sz = style()->pixelMetric(drawImageTop ? QStyle::PM_SmallIconSize : QStyle::PM_LargeIconSize, Q_NULL, button);
            szIcon = QSize(sz, sz);
        }
        else
        {
            szIcon = d.m_icon.actualSize(opt.rect.adjusted(3, 3, -3, -3).size());
            drawImageTop = szIcon.height() <= 16;
        }

        if (drawImageTop)
        {
            if (!drawTitle)
                szImage.setWidth(szIcon.height() + 3);
            else
                szTitle.setWidth(szTitle.width() + szIcon.width() + 1);
        }
        else
            szImage.setWidth(szIcon.width() + 5);
        szImage.setHeight(szIcon.height());
    }

    QSize szText;
    if (!d.m_label)
    {
        int maxTipWidth = d.m_maxTipWidth;
        QRect rcText(QPoint(0, 0), QSize(maxTipWidth - szMargin.width(), 0));
        rcText = fm.boundingRect(rcText.left(), rcText.top(), rcText.width(), rcText.height(), flags | Qt::TextWordWrap, d.m_text);
        szText = rcText.size();
    }
    else
    {
        szText = d.m_label->sizeHint();
    }

    QSize sz(0, 0);
    sz.setHeight(qMax(szImage.height(), szText.height()));
    sz.setWidth(szImage.width() + szText.width());

    if (drawTitle)
    {
        const int siz = style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULL, this);
        sz.setWidth(qMax(sz.width(), szTitle.width()) + siz);
        sz.setHeight(sz.height() + szTitle.height());
    }
    sz += szMargin;
    return sz;
}

/*! \reimp */
void ToolTip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(ToolTip);

    QStylePainter p(this);
    drawFrame(&p);

    QStyleOptionFrame opt;
    opt.initFrom(this);
    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    
    QRect rc = contentsRect();
    rc.adjust(margin(), margin(), -margin(), -margin());
    rc.adjust(3, 3, -3, -3);

    QRect rcTitle(rc.left() + 2, rc.top() + 2, rc.right() - 2, rc.bottom());

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d.m_textTitle.isEmpty();
    bool drawImage = !d.m_icon.isNull();
    bool drawImageTop = true;

    QSize szImage(0, 0), szDrvImage(0, 0);
    if (drawImage)
    {
        if (QToolButton* button = qobject_cast<QToolButton*>(d.m_widget))
        {
            drawImageTop = button->toolButtonStyle() != Qt::ToolButtonTextUnderIcon;
            const int sz = style()->pixelMetric(drawImageTop ? QStyle::PM_SmallIconSize : QStyle::PM_LargeIconSize, Q_NULL, button);
//            const int dpi = CommonStylePrivate::dpiScaled(1, this) / 100;
//            szImage = QSize(sz / dpi, sz / dpi);
            szDrvImage = QSize(sz, sz);
        }
        else
        {
            szImage = d.m_icon.actualSize(opt.rect.adjusted(3, 3, -3, -3).size());
            szDrvImage = szImage;
            drawImageTop = szImage.height() <= 16;
        }

        if (drawImageTop)
        {
            QPoint ptIcon = rc.topLeft();
            ptIcon.setY(rcTitle.y());
            QPixmap pm = CommonStylePrivate::preparePixmap(d.m_icon, szDrvImage, QIcon::Normal, QIcon::On, this);
            style()->drawItemPixmap(&p, QRect(ptIcon, szDrvImage), Qt::AlignCenter, pm);
            rcTitle.setLeft(rcTitle.left() + szDrvImage.width() + 1);
            rc.setLeft(rc.left() + szDrvImage.width() + 3);
        }
    }
    if (drawTitle)
    {
        QFont oldFont = p.font();
        QFont fnt = oldFont;
        fnt.setBold(true);
        p.setFont(fnt);
        style()->drawItemText(&p, rcTitle, flags | Qt::TextSingleLine, opt.palette, isEnabled(), d.m_textTitle, QPalette::ToolTipText);
        p.setFont(oldFont);
        rc.setTop(rc.top() + p.boundingRect(rcTitle, flags | Qt::TextSingleLine, d.m_textTitle).height());
        rc.adjust(0, 12, 0, 0);
    }

    if (drawImage && !drawImageTop)
    {
        QPoint ptIcon = rc.topLeft();
        QPixmap pm = CommonStylePrivate::preparePixmap(d.m_icon, szDrvImage, QIcon::Normal, QIcon::On, this);
        style()->drawItemPixmap(&p, QRect(ptIcon, szDrvImage), Qt::AlignCenter, pm);
        rc.setLeft(rc.left() + szDrvImage.width() + 4);
    }

    if (d.m_label)
        d.m_label->move(rc.topLeft());
    else
        style()->drawItemText(&p, rc, flags | Qt::TextWordWrap, opt.palette, isEnabled(), d.m_text, QPalette::ToolTipText);
    p.end();
}

/*! \reimp */
void ToolTip::resizeEvent(QResizeEvent* event)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.initFrom(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);

    QFrame::resizeEvent(event);
}

/*! \reimp */
void ToolTip::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(ToolTip);
    if (d.m_rect.isNull())
        return;
    QPoint pos = event->globalPos();
    if (d.m_widget)
        pos = d.m_widget->mapFromGlobal(pos);
    if (!d.m_rect.contains(pos))
        hideTip();
    QFrame::mouseMoveEvent(event);
}

void ToolTip::hideTip()
{
    QTN_D(ToolTip);
    if (!d.m_hideTimer.isActive())
        d.m_hideTimer.start(300, this);
}

void ToolTip::closeToolTip()
{
    close(); // to trigger QEvent::Close which stops the animation
    deleteLater();
}

void ToolTip::setTipRect(QWidget* w, const QRect& r)
{
    QTN_D(ToolTip);
    bool update = false;
    if (!d.m_rect.isNull() && !w)
        qWarning("ToolTip::setTipRect: Cannot pass null widget if rect is set");
    else
    {
        update = d.m_widget != w;
        d.m_widget = w;
        d.m_rect = r;
    }
    if (update)
        d.updateTool();
}

/*! \reimp */
void ToolTip::timerEvent(QTimerEvent* event)
{
    QTN_D(ToolTip);
    if (event->timerId() == d.m_hideTimer.timerId() || event->timerId() == d.m_expireTimer.timerId())
    {
        d.m_hideTimer.stop();
        d.m_expireTimer.stop();
        closeToolTip();
    }
}

/*! \reimp */
bool ToolTip::event(QEvent* event)
{
    QTN_D(ToolTip);
    bool result = QFrame::event(event);
    if (event->type() == QEvent::PaletteChange)
    {
        if (d.m_label)
            d.m_label->setPalette(palette());
    }
    return result;
}

/*! \reimp */
bool ToolTip::eventFilter(QObject *o, QEvent* event)
{
    QTN_D(ToolTip);
    switch (event->type()) 
    {
        case QEvent::Leave:
                hideTip();
            break;
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Wheel:
                closeToolTip();
            break;
        case QEvent::MouseMove:
            if (o == d.m_widget && !d.m_rect.isNull() && !d.m_rect.contains(static_cast<QMouseEvent*>(event)->pos()))
                hideTip();
            break;
        default:
            break;
    }
    return false;
}

void ToolTip::placeTip(const QPoint& pos, QWidget* w)
{
    QRect screenRC = qtn_screenGeometry(pos, w);
    QPoint p = pos;
    if (p.x() + this->width() > screenRC.x() + screenRC.width())
        p.rx() -= 4 + this->width();
    if (p.y() + this->height() > screenRC.y() + screenRC.height())
        p.ry() -= 24 + this->height();
    if (p.y() < screenRC.y())
        p.setY(screenRC.y());
    if (p.x() + this->width() > screenRC.x() + screenRC.width())
        p.setX(screenRC.x() + screenRC.width() - this->width());
    if (p.x() < screenRC.x())
        p.setX(screenRC.x());
    if (p.y() + this->height() > screenRC.y() + screenRC.height())
        p.setY(screenRC.y() + screenRC.height() - this->height());

    this->move(p);
}

bool ToolTip::tipChanged(const QPoint& pos, const QString& text, const QString& textTitle, QObject* o)
{
    QTN_D(ToolTip);
    if (ToolTipPrivate::m_instance->text() != text)
        return true;

    if (ToolTipPrivate::m_instance->title() != textTitle)
        return true;

    if (o != d.m_widget)
        return true;

    if (!d.m_rect.isNull())
        return !d.m_rect.contains(pos);
    else
        return false;
}

bool ToolTip::fadingOut() const
{
    QTN_D(const ToolTip);
    return d.m_fadingOut;
}



