/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
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
#include <QCoreApplication>
#include <QComboBox>
#include <QScrollBar>
#include <QListView>
#include <QTreeView>
#include <QMdiArea>
#include <QDockWidget>
#include <QToolBar>
#include <QMenu>
#include <QPainterPath>
#include <qdrawutil.h>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif /* Q_OS_WIN*/
#include "QtnSegoeMDL2Assets.h"
#include "QtnOffice2013StylePrivate.h"
#include "QtnStyleOption.h"
#include "QtnStyleHelperPrivate.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

static const int iTextMargin = 3;


/* Office2013StylePrivate */
Office2013StylePrivate::Office2013StylePrivate()
    : OfficeStylePrivate(), m_themeType(Office2013Style::White)
{
}

Office2013StylePrivate::~Office2013StylePrivate()
{
}

void Office2013StylePrivate::init()
{
    OfficeStylePrivate::init();
#if 0
    QTN_P(OfficeStyle)
    QFont font = p.font(Q_NULL);
    QApplication::setFont(font, "QMenuBar");
    QApplication::setFont(font, QTITAN_META_CLASSNAME("RibbonBar"));
#endif
}

/*! \internal */
QColor Office2013StylePrivate::accentIndexToColor(OfficeStyle::AccentColor accentcolor) const
{
    QColor clr = QColor(43, 87, 154);
    switch (accentcolor)
    {
        case OfficeStyle::AccentColorBlue:    clr = QColor(43, 87, 154); break;
        case OfficeStyle::AccentColorBrown:   clr = QColor(161, 53, 55); break;
        case OfficeStyle::AccentColorGreen:   clr = QColor(33, 115, 70); break;
        case OfficeStyle::AccentColorLime:    clr = QColor(137, 164, 48); break;
        case OfficeStyle::AccentColorMagenta: clr = QColor(216, 0, 115); break;
        case OfficeStyle::AccentColorOrange:  clr = QColor(208, 69, 37); break;
        case OfficeStyle::AccentColorPink:    clr = QColor(230, 113, 184); break;
        case OfficeStyle::AccentColorPurple:  clr = QColor(126, 56, 120); break;
        case OfficeStyle::AccentColorRed:     clr = QColor(229, 20, 0); break;
        case OfficeStyle::AccentColorTeal:    clr = QColor(7, 114, 101); break;
    }
    return clr;
}

/*! \internal */
void Office2013StylePrivate::initStandardPalette(QPalette& palette) const
{
    OfficeStylePrivate::initStandardPalette(palette);
    const QColor clrDisabledHighlight = QColor(145, 145, 145);
    const QColor clrHighlight = StyleHelper::colorMakePale(m_clrAccent.rgb(), .9);
    QPalette_setBrush(palette, QPalette::Active, QPalette::Highlight, clrHighlight);
    QPalette_setBrush(palette, QPalette::Inactive, QPalette::Highlight, clrHighlight);
    QPalette_setBrush(palette, QPalette::Disabled, QPalette::Highlight, clrDisabledHighlight);
}

// draw PrimitiveElement
/*! \internal */
bool Office2013StylePrivate::drawFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    const QPen savePen = painter->pen();
    QPen thePen(opt->palette.shadow().color().darker(140));
    thePen.setCosmetic(false);
    painter->setPen(thePen);
    painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
    painter->setPen(savePen);
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawFrameWindow(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (const QStyleOptionFrame* frm = qstyleoption_cast<const QStyleOptionFrame *>(option))
    {
        if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstagePage")))
        {
            painter->fillRect(frm->rect, option->palette.color(QPalette::Base));
            return true;
        }
    }
    else if (const QStyleOptionTabWidgetFrame* frm = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>(option))
    {
        painter->fillRect(frm->rect, frm->palette.window());
        painter->setPen(frm->palette.shadow().color().darker(140));
        painter->drawRect(frm->rect.adjusted(0, 0, -1, -1));
        return true;
    }
    return OfficeStylePrivate::drawFrameWindow(option, painter, widget);
}

/*! \internal */
bool Office2013StylePrivate::drawFrameMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (qobject_cast<const QToolBar*>(w))
    {
        StyleHelper::draw3dRectEx(*painter, opt->rect, opt->palette.shadow().color().darker(140), opt->palette.shadow().color().darker(140));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawPanelMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (OfficeStylePrivate::drawPanelMenu(opt, painter, w))
        return true;
    painter->save();
    QColor menuBackground = opt->palette.window().color();
    QColor borderColor = opt->palette.light().color();
    painter->setPen(borderColor);
    painter->setBrush(menuBackground);
    painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
    painter->restore();
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawPanelStatusBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const OfficeStyle)
    if (m_themeType == Office2013Style::Dark)
        painter->fillRect(opt->rect, opt->palette.window().color());
    else
        painter->fillRect(opt->rect, p.accentColor());
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawIndicatorToolBarSeparator(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (widget && widget->inherits("QStatusBar"))
    {
        QPoint p1, p2;
        if (option->state & QStyle::State_Horizontal) 
        {
            p1 = QPoint(option->rect.width() / 2, 0);
            p2 = QPoint(p1.x(), option->rect.height());
        }
        else 
        {
            p1 = QPoint(0, option->rect.height() / 2);
            p2 = QPoint(option->rect.width(), p1.y());
        }
        painter->setPen(QPen(option->palette.dark().color()));
        painter->drawLine(p1, p2);
        return true;
    }

    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonGroup")))
    {
        if (const QStyleOptionGroupBox* optGroup = qstyleoption_cast<const QStyleOptionGroupBox*>(option))
        {
            QRect rect = option->rect;
            QColor clrRibbonSeparator = option->palette.shadow().color();
            if (optGroup->text.isEmpty())
            {
                QPainterPath path;
                path.moveTo(rect.center().x(), rect.top());
                path.lineTo(rect.center().x(), rect.bottom());

                const QPen savePen = painter->pen();
                painter->setPen(clrRibbonSeparator);
                painter->drawPath(path);
                painter->setPen(savePen);
            }
            else
            {
                QString titleText = optGroup->text;
                QRect rectText = optGroup->fontMetrics.boundingRect(titleText);
                if (optGroup->textAlignment == Qt::AlignBottom)
                {
                    QPainterPath path;
                    path.moveTo(rect.center().x(), rect.top());
                    path.lineTo(rect.center().x(), rect.bottom() - rectText.width() - 4);

                    const QPen savePen = painter->pen();
                    painter->setPen(clrRibbonSeparator);
                    painter->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    painter->translate(rect.left() - 1, rect.top() + rect.width());
                    painter->rotate(-90);
                    painter->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette palette;
                    QPalette_setColor(palette, QPalette::WindowText, clrRibbonSeparator.darker());
                    p.proxy()->drawItemText(painter, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    painter->setPen(savePen);
                }
                else if (optGroup->textAlignment == Qt::AlignTop)
                {
                    QPainterPath path;
                    path.moveTo(rect.center().x(), rect.top() + rectText.width());
                    path.lineTo(rect.center().x(), rect.bottom());

                    const QPen savePen = painter->pen();
                    painter->setPen(clrRibbonSeparator);
                    painter->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    painter->translate(rect.left() - 1, rect.top() + rect.width());
                    painter->rotate(-90);
                    painter->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette palette;
                    QPalette_setColor(palette, QPalette::WindowText, clrRibbonSeparator.darker());
                    p.proxy()->drawItemText(painter, rect.adjusted(indent + rect.width() - rectText.width() - 2, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    painter->setPen(savePen);
                }
                else if (optGroup->textAlignment == Qt::AlignVCenter)
                {
                    const QPen savePen = painter->pen();
                    painter->setPen(clrRibbonSeparator);
                    painter->drawLine(rect.center().x(), rect.top() + rectText.width() + rectText.width() + 4, rect.center().x(), rect.bottom());
                    painter->drawLine(rect.center().x(), rect.top(), rect.center().x(), rect.bottom() - (rectText.width() + rectText.width() + 4));

                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    painter->translate(rect.left() - 1, rect.top() + rect.width());
                    painter->rotate(-90);
                    painter->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette palette;
                    QPalette_setColor(palette, QPalette::WindowText, clrRibbonSeparator.darker());
                    p.proxy()->drawItemText(painter, rect.adjusted(indent + (rect.width() - rectText.width()) / 2 - 2, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    painter->setPen(savePen);
                }
            }
        }
        return true;
    }
    else
    {
        QPen pen = painter->pen();
        const int margin = 3;
        painter->setPen(option->palette.window().color().darker(114));
        if (option->state & QStyle::State_Horizontal)
        {
            const int x1 = option->rect.center().x();
            painter->drawLine(QPoint(x1, option->rect.top() + margin), QPoint(x1, option->rect.bottom() - margin));
        }
        else
        {
            const int y1 = option->rect.center().y();
            painter->drawLine(QPoint(option->rect.left() + margin, y1), QPoint(option->rect.right() - margin, y1));
        }
        painter->setPen(pen);
    }
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawFrameGroupBox(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QtStyleOptionFrame* frame = qstyleoption_cast<const QtStyleOptionFrame *>(opt))
    {
        QPen oldPen = painter->pen();
        painter->setPen(frame->palette.light().color());
        if (frame->features & QtStyleOptionFrame::Flat)
        {
            QPolygon a;
            QRect fr = frame->rect;
            QPoint p1(fr.x(), fr.y() + 1);
            QPoint p2(fr.x() + fr.width(), p1.y());
            int x1 = p1.x(), y1 = p2.y(), x2 = p2.x();
            // total line width
            int tlw = frame->lineWidth * 2 + frame->midLineWidth;
            int y = y1 - tlw / 2;
            for (int i = 0; i < frame->lineWidth; i++)
            {
                a.setPoints(3, x1 + i, y + tlw - i - 1, x2 - i, y + tlw - i - 1, x2 - i, y + i + 1);
                painter->drawPolyline(a);
            }
        }
        else
        {
            int x = frame->rect.x(), y = frame->rect.y(), w = frame->rect.width(), h = frame->rect.height();
            int x1 = x, y1 = y;
            painter->drawRect(x1, y1, w - 2, h - 2);
        }
        painter->setPen(oldPen);
    }
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawIndicatorCheckBox(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const OfficeStyle)
    if (const QStyleOptionButton* checkbox = qstyleoption_cast<const QStyleOptionButton*>(option)) 
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->translate(0.5, 0.5);

        QRect rect = option->rect.adjusted(0, 0, -1, -1);
        QColor clrAccentText = StyleHelper::pixelAlpha(p.accentColor().rgb(), 75);
        QColor clrHighlight = option->palette.color(QPalette::Highlight);
        QColor clrHighlightDn = StyleHelper::colorMakeDarker(clrHighlight.rgb(), .14);

        const bool enabled = option->state & QStyle::State_Enabled;
        const bool highlighted = option->state & QStyle::State_MouseOver;
        const bool pressed = option->state & QStyle::State_Sunken;

        QColor pressedColor = option->palette.color(QPalette::Window);

        if (pressed)
            pressedColor = clrHighlightDn;
        else if (pressed || highlighted)
            pressedColor = clrHighlight;
        painter->fillRect(rect, pressedColor);

        QColor clrBorder = !enabled ? option->palette.color(QPalette::Text) : highlighted ? StyleHelper::colorMakeDarker(clrHighlight.rgb(), .14) :
            (pressed ? clrHighlightDn : option->palette.color(QPalette::Text));

        painter->setPen(clrBorder);
        painter->drawRect(rect);

        QColor checkMarkColor = highlighted && enabled ? clrAccentText :
            (qtn_isColorLight(option->palette.color(QPalette::Window)) ? StyleHelper::colorMakeDarker(option->palette.color(QPalette::Text).rgb(), 0.4) :
            StyleHelper::colorMakeLighter(option->palette.color(QPalette::Text).rgb(), 0.4));
        if (highlighted && enabled)
            checkMarkColor = checkMarkColor.lighter();

        const qreal checkMarkPadding = 1 + rect.width() * 0.13; // at least one pixel padding

        if (checkbox->state & QStyle::State_NoChange) 
        {
            // Gradient fill
            QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
            gradient.setColorAt(0, pressed ? pressedColor : option->palette.base().color().darker(115));
            gradient.setColorAt(0.15, pressed ? pressedColor : option->palette.base().color());
            gradient.setColorAt(1, pressed ? pressedColor : option->palette.base().color());

            gradient = QLinearGradient(rect.topLeft(), rect.bottomLeft());
            checkMarkColor.setAlpha(80);
            gradient.setColorAt(0, checkMarkColor);
            checkMarkColor.setAlpha(140);
            gradient.setColorAt(1, checkMarkColor);
            checkMarkColor.setAlpha(180);
            painter->setPen(QPen(checkMarkColor, 1));
            painter->setBrush(gradient);
            painter->drawRect(rect.adjusted(qRound(checkMarkPadding), qRound(checkMarkPadding), 
                qRound(-checkMarkPadding), qRound(-checkMarkPadding)));
        }
        else if (checkbox->state & QStyle::State_On) 
        {
            qreal penWidth = CommonStylePrivate::dpiScaledF(1.5, w);
            penWidth = qMax<qreal>(penWidth, 0.13 * rect.height());
            penWidth = qMin<qreal>(penWidth, 0.20 * rect.height());
            QPen checkPen = QPen(checkMarkColor, penWidth);
            checkMarkColor.setAlpha(210);
            painter->translate(CommonStylePrivate::dpiScaledF(-0.8, w), CommonStylePrivate::dpiScaledF(0.5, w));
            painter->setPen(checkPen);
            painter->setBrush(Qt::NoBrush);

            // Draw checkmark
            QPainterPath path;
            const qreal rectHeight = rect.height(); // assuming height equals width
            path.moveTo(checkMarkPadding + rectHeight * 0.11, rectHeight * 0.47);
            path.lineTo(rectHeight * 0.5, rectHeight - checkMarkPadding);
            path.lineTo(rectHeight - checkMarkPadding, checkMarkPadding);
            painter->drawPath(path.translated(rect.topLeft()));
        }
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawIndicatorRadioButton(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const OfficeStyle)
    const bool enabled = option->state & QStyle::State_Enabled;
    const bool highlighted = option->state & QStyle::State_MouseOver;
    const bool pressed = option->state & QStyle::State_Sunken;

    QRect rc = option->rect;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPainterPath circle;
    const QPointF circleCenter = rc.center() + QPoint(1, 1);
    const qreal outlineRadius = (rc.width() + (rc.width() + 1) % 2) / 2.0 - 1;
    circle.addEllipse(circleCenter, outlineRadius, outlineRadius);

    QColor clrAccentText = StyleHelper::pixelAlpha(p.accentColor().rgb(), 75);
    QColor clrHighlight = option->palette.color(QPalette::Highlight);
    QColor clrHighlightDn = StyleHelper::colorMakeDarker(clrHighlight.rgb(), .14);

    if (pressed)
        painter->setBrush(clrHighlightDn);
    else if (pressed || highlighted)
        painter->setBrush(clrHighlight);
    else
        painter->setBrush(option->palette.color(QPalette::Window));

    QColor clrBorder = !enabled ? option->palette.color(QPalette::Text) : highlighted ? StyleHelper::colorMakeDarker(clrHighlight.rgb(), .14) :
        (pressed ? clrHighlightDn : option->palette.color(QPalette::Text));

    painter->setPen(clrBorder);
    painter->drawPath(circle);

    if (option->state & (QStyle::State_Sunken | QStyle::State_On))
    {
        circle = QPainterPath();
        const qreal checkmarkRadius = outlineRadius / 2.32;
        circle.addEllipse(circleCenter, checkmarkRadius, checkmarkRadius);

        QColor checkMarkColor = highlighted && enabled ? clrAccentText :
            (qtn_isColorLight(option->palette.color(QPalette::Window)) ? StyleHelper::colorMakeDarker(option->palette.color(QPalette::Text).rgb(), 0.4) :
                StyleHelper::colorMakeLighter(option->palette.color(QPalette::Text).rgb(), 0.4));
        if (highlighted && enabled)
            checkMarkColor = checkMarkColor.lighter();

        checkMarkColor.setAlpha(200);
        painter->setPen(checkMarkColor);
        checkMarkColor.setAlpha(180);
        painter->setBrush(checkMarkColor);
        painter->drawPath(circle);
    }
    painter->restore();
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QRect rect = opt->rect;
    QPixmap px = standardPixmap(QStringLiteral("Image_MenuCheckMark"), (opt && opt->state & QStyle::State_Enabled) ? CommonStylePrivate::Black : CommonStylePrivate::Gray, w);

    const QSize sizeImage = px.size();
    QPoint ptImage(rect.left() + (rect.width() - sizeImage.width()) / 2 + ((rect.width() - sizeImage.width()) % 2),
        rect.top() + (rect.height() - sizeImage.height()) / 2 + ((rect.height() - sizeImage.height()) % 2));

    QRect rectImage(ptImage, sizeImage);
    bool wasSmoothPixmapTransform = painter->renderHints() & QPainter::SmoothPixmapTransform;
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->drawPixmap(rectImage, px);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, wasSmoothPixmapTransform);
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawIndicatorButtonDropDown(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    const bool enabled = opt->state & QStyle::State_Enabled;
    const bool isHighlight = opt->state & QStyle::State_MouseOver;
    const bool isPress = opt->state & QStyle::State_Sunken;
    QBrush fill = opt->palette.color(QPalette::Button);
    if (isPress && enabled)
        fill = buttonHighlightColor(opt->palette, true, false/*border*/);
    else if (isHighlight && enabled)
        fill = buttonHighlightColor(opt->palette, false, false/*border*/);
    painter->fillRect(opt->rect, fill);
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawPanelButtonBevel(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    painter->translate(opt->rect.x(), opt->rect.y());
    QRect rect(opt->rect);
    if (opt->state & QStyle::State_Horizontal)
    {
        for (int y = 4; y < rect.height() - 4; y += 4)
        {
            painter->fillRect(QRect(QPoint(3, y + 1), QPoint(4, y + 2)), opt->palette.window().color());
            painter->fillRect(QRect(QPoint(2, y), QPoint(3, y + 1)), opt->palette.midlight().color());
        }
    }
    else
    {
        for (int x = 4; x < opt->rect.width() - 4; x += 4)
        {
            painter->fillRect(QRect(QPoint(x + 1, 3), QPoint(x + 2, 4)), opt->palette.window().color());
            painter->fillRect(QRect(QPoint(x, 2), QPoint(x + 1, 3)), opt->palette.midlight().color());
        }
    }
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawFrameTabBarBase(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (const QStyleOptionTabBarBase* tbb = qstyleoption_cast<const QStyleOptionTabBarBase *>(opt))
    {
        painter->save();
        QColor outline = opt->palette.light().color();
        painter->setPen(outline.lighter(110));
        switch (tbb->shape)
        {
            case QTabBar::RoundedNorth:
                painter->drawLine(tbb->rect.topLeft(), tbb->rect.topRight());
            break;
            case QTabBar::RoundedWest:
                painter->drawLine(tbb->rect.topLeft(), tbb->rect.bottomLeft());
                break;
            case QTabBar::RoundedSouth:
                painter->drawLine(tbb->rect.bottomLeft(), tbb->rect.bottomRight());
                break;
            case QTabBar::RoundedEast:
                painter->drawLine(tbb->rect.topRight(), tbb->rect.bottomRight());
                break;
            case QTabBar::TriangularNorth:
            case QTabBar::TriangularEast:
            case QTabBar::TriangularWest:
            case QTabBar::TriangularSouth:
                painter->restore();
                p.QProxyStyle::drawPrimitive(QStyle::PE_FrameTabBarBase, opt, painter, widget);
                return true;
        }
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawFrameFocusRect(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && (widget->inherits(QTITAN_META_CLASSNAME("StatusButton")) || (widget->parentWidget() && widget->parentWidget()->inherits("QStatusBar"))))
    {
        const bool isOn = opt->state & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised);
        const bool isPress = opt->state & QStyle::State_Sunken;
        const bool isHighlight = opt->state & QStyle::State_MouseOver;

        QColor fill = opt->palette.color(QPalette::Active, QPalette::Button);
        if (isPress)
            fill = opt->palette.color(QPalette::Active, QPalette::Button).lighter(80);
        else if (isHighlight)
            fill = opt->palette.color(QPalette::Active, QPalette::Button).lighter(120);
        else if (isOn)
            fill = opt->palette.color(QPalette::Active, QPalette::Button).lighter(80);

        painter->fillRect(opt->rect, fill);
        return true;
    }
    else if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar*>(opt))
    {
        const bool isPress = tb->state & QStyle::State_Sunken;
        const bool isHighlight = tb->state & QStyle::State_MouseOver;
        QBrush fill = Qt::transparent;
        if (isPress)
            fill = buttonHighlightColor(tb->palette, true, false, widget);
        else if (isHighlight)
            fill = buttonHighlightColor(tb->palette, false, false, widget);
        painter->fillRect(tb->rect, fill);
        return true;
    }
    else if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonGroup")))
    {
        if (const QStyleOptionButton* optGroup = qstyleoption_cast<const QStyleOptionButton*>(opt))
        {
            QRect rcEntry(opt->rect);
            if (optGroup->features & QStyleOptionButton::HasMenu)
                painter->fillRect(rcEntry.adjusted(0, 0, -1, 0), opt->palette.mid().color());
            else if (opt->state & QStyle::State_MouseOver)
                painter->fillRect(rcEntry, StyleHelper::pixelAlpha(opt->palette.mid().color().rgb(), 103));

            const QPen savePen = painter->pen();
            painter->setPen(opt->palette.midlight().color());
            painter->drawLine(QPoint(opt->rect.right() - 1, opt->rect.top()), QPoint(opt->rect.right() - 1, opt->rect.bottom()));
            painter->setPen(savePen);
            return true;
        }
    }
    else
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool isHasFocus = opt->state & QStyle::State_HasFocus;
        const bool isHighlight = opt->state & QStyle::State_MouseOver;
        const bool isPress = (opt->state & QStyle::State_Sunken) || (opt->state & QStyle::State_On);
        QBrush colorFill = opt->palette.color(QPalette::Button);
        QColor colorBorder = opt->palette.color(QPalette::Dark);
        if (isPress && enabled)
        {
            colorFill = buttonHighlightColor(opt->palette, true, false, widget);
            colorBorder = buttonHighlightColor(opt->palette, true, true, widget);
        }
        else if (isHighlight && enabled)
        {
            colorFill = buttonHighlightColor(opt->palette, false, false, widget);
            colorBorder = buttonHighlightColor(opt->palette, false, true, widget);
        }
        else if (isHasFocus && enabled)
            colorBorder = m_clrAccent;

        painter->fillRect(opt->rect, colorFill);
        painter->setPen(colorBorder);
        qtn_drawRect(painter, opt->rect);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawFrameLineEdit(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const OfficeStyle)
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
    {
        const bool enabled = panel->state & QStyle::State_Enabled;
        const bool highlighted = panel->state & QStyle::State_MouseOver;
        const bool hasFocus = panel->state & QStyle::State_HasFocus;

        painter->save();
        QRegion clipRegion = opt->rect;
        clipRegion -= panel->rect.adjusted(2, 2, -2, -2);
        painter->setClipRegion(clipRegion);

        QColor colorBorder = panel->palette.dark().color();
        if (enabled && (highlighted || hasFocus))
            colorBorder = p.accentColor();

        QtnPen pen(painter, colorBorder);
        qtn_drawRect(painter, panel->rect);
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawPanelButtonCommand(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(opt))
    {
        bool usePalette = btn->palette.isBrushSet(btn->palette.currentColorGroup(), QPalette::Base);
        if (usePalette)
            return false;
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool checked = opt->state & QStyle::State_On;
        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;
        const bool isHasFocus = opt->state & QStyle::State_HasFocus;

        QBrush colorFill = opt->palette.color(QPalette::Button);
        QColor colorBorder = opt->palette.color(QPalette::Dark);
        if ((pressed || checked) && enabled)
        {
            colorFill = buttonHighlightColor(opt->palette, true, false, w);
            colorBorder = buttonHighlightColor(opt->palette, true, true, w);
        }
        else if (selected && enabled)
        {
            colorFill = buttonHighlightColor(opt->palette, false, false, w);
            colorBorder = buttonHighlightColor(opt->palette, false, true, w);
        }
        else if (isHasFocus && enabled)
            colorBorder = m_clrAccent;

        painter->fillRect(opt->rect, colorFill);
        painter->setPen(colorBorder);
        qtn_drawRect(painter, opt->rect);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonFileButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle);
    if (const SystemToolButtonStyleOption* toolbutton = qstyleoption_cast<const SystemToolButtonStyleOption*>(opt))
    {
        QColor fillColor = buttonRibbonFileColor(toolbutton);
        painter->fillRect(toolbutton->rect.adjusted(0, 0, 0, -1), fillColor);
        if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
        {
            int alignment = Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, toolbutton, widget))
                alignment |= Qt::TextHideMnemonic;
            alignment |= Qt::AlignCenter;
            alignment |= Qt::TextSingleLine;

            QString text = toolbutton->text;
            if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), opt, widget))
                text = text.toUpper();
            QPalette palette = toolbutton->palette;
            QPalette_setBrush(palette, QPalette::All, QPalette::ButtonText, isDarkColor(fillColor) ? Qt::white : Qt::black);
            p.proxy()->drawItemText(painter, toolbutton->rect, alignment, palette,
                toolbutton->state & QStyle::State_Enabled, text, QPalette::ButtonText);
        }
    }
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonTitleBarWidget(const QStyleOptionComplex* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const OfficeStyle)
    if (const TitleBarStyleOption* styleOption = qstyleoption_cast<const TitleBarStyleOption*>(option))
    {
        if (styleOption->isBackstageVisible && p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonBackstageHideTabs)))
        {
            QRect rect = styleOption->rect;
            rect.setWidth(styleOption->menuWidth);
            painter->fillRect(rect, fillBackstageFrameColor(styleOption->paletteBackstageFrameMenu));
        }

        for (QVector<TitleBarStyleOption::ContextualTabInfo>::const_iterator it = styleOption->contextualTabs.constBegin();
            it != styleOption->contextualTabs.constEnd(); ++it)
        {
            QRect rc = it->rect;
            const QColor color = CommonStyle::contextColorToColor(it->color);
            const QColor clrLight = StyleHelper::colorMakePale(color.rgb());

            QColor clrContextTab = colorSubControl(QStringLiteral("RibbonTitleBarWidget"), QStringLiteral("ContextTab"), QPalette::Active, QPalette::Window);
            if (clrContextTab.isValid())
                painter->fillRect(rc, clrContextTab);
            else
                painter->fillRect(rc, clrLight);

            QRect rectTop = rc;
            rectTop.setBottom(rectTop.top() + 4);
            painter->fillRect(rectTop, color);

            QString text = it->strTitle;
            if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), option, w))
                text = text.toUpper();

            painter->setPen(StyleHelper::colorMakeDarker(color.rgb(), 0.3));
            painter->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine, painter->fontMetrics().elidedText(text, Qt::ElideRight, rc.width()));
        }
        painter->setFont(styleOption->fontTextTitle);
        QString text = QFontMetrics(styleOption->fontTextTitle).elidedText(styleOption->text, Qt::ElideRight, styleOption->rcTitleText.width());
        p.proxy()->drawItemText(painter, styleOption->rcTitleText, styleOption->alignment, styleOption->palette, true/*enabled*/, text, QPalette::WindowText);
        return true;
    }
    return false;
}

QWidget* Office2013StylePrivate::qtn_findRibbonBar(const QWidget* parent)
{
    if (parent == Q_NULL)
        return Q_NULL;

    QList<QWidget*> wds = parent->findChildren<QWidget*>();
    for (QList<QWidget*>::ConstIterator it = wds.constBegin(); it != wds.constEnd(); ++it)
    {
        if ((*it)->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
            return (*it);
    }
    return Q_NULL;
}

/*! \internal */
bool Office2013StylePrivate::qtn_isBackstageVisible(const QWidget* widget)
{
    const QWidget* wd = Q_NULL;
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
        wd = widget;
    if (wd == Q_NULL)
        wd = qtn_findRibbonBar(widget);
    return wd && wd->property("backstageVisible").toBool();
}

static const double marginRatio = 3.2;
// draw ComplexControl
/*! \internal */
bool Office2013StylePrivate::drawTitleBar(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
    {
        QRect ir;
        const bool isActive = tb->state & QStyle::State_Active;
        const QWidget* wdRibbon = qtn_findRibbonBar(widget);
        const bool isBackstageVisible = qtn_isBackstageVisible(wdRibbon);
        if (option->subControls & QStyle::SC_TitleBarLabel)
        {
            QColor frameColor = titleBarFrameColor(option->palette, isActive, widget);
            QColor windowColor = fillTitleBarColor(option->palette, isBackstageVisible); // TODO: titleBarWindowColor(option->palette, isActive, widget)
            if (!windowColor.isValid())
                windowColor = widget ? widget->palette().color(QPalette::Window) : option->palette.color(QPalette::Window);
            drawfillTitleBar(option, painter, widget, frameColor, windowColor);
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarLabel, widget);
            painter->setPen(isActive ? option->palette.text().color() : option->palette.color(QPalette::Disabled, QPalette::Text));
            painter->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, tb->text);
        }
#if 0 //TODO remove me
        if (wdRibbon != Q_NULL)
        {
            const int fwidth = p.proxy()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, Q_NULL, widget);
            RibbonBackgroundStyleOption ob;
            ob.rect = tb->rect;
            ob.rect.translate(0, fwidth);
            ob.pixTitleBackground = wdRibbon->property("imageBackground").value<QPixmap>();
            if (!ob.pixTitleBackground.isNull())
                p.proxy()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(CommonStyle::PE_RibbonImageBackground), &ob, painter, widget);
        }
#endif
        bool down = false;
        bool highlight = false;
        QPixmap pm;
        QPalette savePalette = tb->palette;
        QStyleOptionTitleBar tool = *tb;
        if (tb->subControls & QStyle::SC_TitleBarCloseButton && tb->titleBarFlags & Qt::WindowSystemMenuHint)
        {
            const bool closeHint = tb->titleBarFlags & Qt::WindowCloseButtonHint;
            QColor clrText = brushFromConfig(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"), closeHint ? QPalette::Active : QPalette::Disabled,
                QPalette::WindowText, option->state, isBackstageVisible).color();
            QColor clrHighlightedText = brushFromConfig(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"),
                QPalette::Active, QPalette::HighlightedText, option->state, isBackstageVisible).color();
            QColor clrButton = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarCloseButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarCloseButton) && (option->state & QStyle::State_Sunken);
            highlight = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight && closeHint) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarCloseButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        tool.palette = savePalette;
        if (tb->subControls & QStyle::SC_TitleBarMaxButton && tb->titleBarFlags & Qt::WindowMaximizeButtonHint && !(tb->titleBarState & Qt::WindowMaximized))
        {
            QColor clrText = brushFromConfig(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMaxButton"), QPalette::Active, QPalette::WindowText,
                option->state, isBackstageVisible).color();
            QColor clrButton = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMaxButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMaxButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarMaxButton) && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarMaxButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarMaxButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        if (tb->subControls & QStyle::SC_TitleBarMinButton && tb->titleBarFlags & Qt::WindowMinimizeButtonHint && !(tb->titleBarState & Qt::WindowMinimized))
        {
            QColor clrText = brushFromConfig(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMinButton"), QPalette::Active, QPalette::WindowText,
                option->state, isBackstageVisible).color();
            QColor clrButton = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMinButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMinButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarMinButton) && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarMinButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarMinButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        bool drawNormalButton = (tb->subControls & QStyle::SC_TitleBarNormalButton) && (((tb->titleBarFlags & Qt::WindowMinimizeButtonHint) &&
            (tb->titleBarState & Qt::WindowMinimized)) || ((tb->titleBarFlags & Qt::WindowMaximizeButtonHint) && (tb->titleBarState & Qt::WindowMaximized)));
        if (drawNormalButton)
        {
            QColor clrText = brushFromConfig(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarNormalButton"), QPalette::Active, QPalette::WindowText,
                option->state, isBackstageVisible).color();
            QColor clrButton = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarNormalButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarNormalButton, widget);
            down = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarNormalButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarNormalButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }
        if (tb->subControls & QStyle::SC_TitleBarShadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint && !(tb->titleBarState & Qt::WindowMinimized))
        {
            QColor clrText = brushFromConfig(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarShadeButton"), QPalette::Active, QPalette::WindowText,
                option->state, isBackstageVisible).color();
            QColor clrButton = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarShadeButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarShadeButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarShadeButton && (option->state & QStyle::State_Sunken));
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarShadeButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarNormalButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        if (tb->subControls & QStyle::SC_TitleBarContextHelpButton && tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarContextHelpButton, widget);

            down = tb->activeSubControls & QStyle::SC_TitleBarContextHelpButton && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarContextHelpButton) && (option->state & QStyle::State_MouseOver);

            tool.rect = ir;

            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, Qt::black);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, Qt::black);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarContextHelpButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        if (tb->subControls & QStyle::SC_TitleBarUnshadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint && tb->titleBarState & Qt::WindowMinimized)
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarUnshadeButton, widget);

            down = tb->activeSubControls & QStyle::SC_TitleBarUnshadeButton && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarUnshadeButton) && (option->state & QStyle::State_MouseOver);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            pm = p.proxy()->standardIcon(QStyle::SP_TitleBarUnshadeButton, &tool, widget).pixmap(QtnPrivate::qtn_getWindow(widget), QSize(10, 10));
#else
            pm = p.proxy()->standardIcon(QStyle::SP_TitleBarUnshadeButton, &tool, widget).pixmap(QSize(10, 10));
#endif
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);
            // TODO:
            painter->save();
            if (down)
                painter->translate(p.proxy()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, tb, widget), p.proxy()->pixelMetric(QStyle::PM_ButtonShiftVertical, tb, widget));
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, pm);
            painter->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarSysMenu && tb->titleBarFlags & Qt::WindowSystemMenuHint)
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarSysMenu, widget);
            if (!tb->icon.isNull())
                tb->icon.paint(painter, ir);
            else
            {
                int iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, tb, widget);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                pm = p.proxy()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, widget).pixmap(QtnPrivate::qtn_getWindow(widget), QSize(iconSize, iconSize));
#else
                pm = p.proxy()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, widget).pixmap(QSize(iconSize, iconSize));
#endif
                tool.rect = ir;
                painter->save();
                p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, pm);
                painter->restore();
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawScrollBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (p.isScrollBarsIgnored())
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        // Make a copy here and reset it for each primitive.
        QStyleOptionSlider newScrollbar(*scrollbar);
        if (scrollbar->minimum == scrollbar->maximum)
            newScrollbar.state &= ~QStyle::State_Enabled; //do not draw the slider.

        QStyle::State saveFlags = scrollbar->state;

        newScrollbar.state = saveFlags;
        newScrollbar.rect = opt->rect;

        painter->fillRect(scrollbar->rect, scrollbar->palette.shadow().color());

        const QPen savePen = painter->pen();
        painter->setPen(scrollbar->palette.shadow().color());
        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine)
        {
            QRect rc = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubLine, widget);
            if (opt->state & QStyle::State_Horizontal)
                painter->drawRect(scrollbar->rect.adjusted(rc.width() + 1, 0, -1, -1));
            else
                painter->drawRect(scrollbar->rect.adjusted(0, rc.height() + 1, -1, -1));
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine)
        {
            QRect rc = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddLine, widget);
            if (opt->state & QStyle::State_Horizontal)
                painter->drawRect(scrollbar->rect.adjusted(0, 0, -(rc.width() + 1), -1));
            else
                painter->drawRect(scrollbar->rect.adjusted(0, 0, -1, -(rc.height() + 1)));
        }
        painter->setPen(savePen);

        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubLine, widget);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 0, 0, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, 0);

            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarSubLine, &newScrollbar, painter, widget);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddLine, widget);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 0, -1, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, -1);

            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarAddLine, &newScrollbar, painter, widget);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSlider)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSlider, widget);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 0, 0, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, 0);

            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarSlider, &newScrollbar, painter, widget);
        }

        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawComboBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const OfficeStyle)
    if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt))
    {
        QStyle::State flags = cmb->state;
        QStyle::SubControls sub = cmb->subControls;

        const bool enabled = flags & QStyle::State_Enabled;
        const bool highlighted = flags & QStyle::State_MouseOver;
        const bool selected = flags & QStyle::State_Selected;
        const bool dropped = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && ((flags & QStyle::State_On) || (flags & QStyle::State_Sunken));
        const bool dropButtonHot = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && (flags & QStyle::State_MouseOver);
        const bool hasFocus = flags & QStyle::State_HasFocus;

        if (cmb->frame)
        {
            QRect r = cmb->rect.adjusted(0, 0, -1, -1);
            bool usePalette = cmb->palette.isBrushSet(cmb->palette.currentColorGroup(), QPalette::Base);

            painter->fillRect(r, !usePalette && enabled && (highlighted || selected || hasFocus) ? cmb->palette.brush(QPalette::Base) :
                enabled ? cmb->palette.window().color() : cmb->palette.brush(QPalette::Base));

            QColor colorBorder = cmb->palette.dark().color();
            if (enabled && (highlighted || hasFocus || selected || dropped))
                colorBorder = p.accentColor();

            QtnPen pen(painter, colorBorder);
            qtn_drawRect(painter, cmb->rect);
        }

        if (sub & QStyle::SC_ComboBoxArrow)
        {
            QRect rcBtn = p.proxy()->subControlRect(QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxArrow, w);
            rcBtn.adjust(0, 1, -1, -1);
            if (enabled)
            {
                QColor clrFill = cmb->palette.window().color();
                if (dropped)
                    clrFill = buttonHighlightColor(cmb->palette, dropped/*highlightDn*/, false/*border*/);
                else if (dropButtonHot || hasFocus)
                    clrFill = buttonHighlightColor(cmb->palette, false/*highlightDn*/, false/*border*/);
                painter->fillRect(rcBtn, clrFill);
            }

            QStyleOption optArray = *opt;
            QPoint pt = rcBtn.center();
            int size = CommonStylePrivate::dpiScaled(10, w);
            QRect rc(QPoint(pt.x() - size / 2, pt.y() - size / 2), QSize(size, size));
            optArray.rect = rc;
            drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, &optArray, painter, w);
        }

        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawSlider(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSliderSlider")))
    {
        if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider*>(opt))
        {
            if (slider->orientation == Qt::Horizontal)
            {
                QRect groove = p.proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderGroove, widget);
                QRect handle = p.proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderHandle, widget);

                QRect rcTrackDest = groove;
                rcTrackDest.setHeight(1);
                painter->fillRect(rcTrackDest, opt->palette.color(QPalette::Shadow));

                if ((opt->subControls & QStyle::SC_SliderHandle))
                {
                    const bool enabled = opt->state & QStyle::State_Enabled;
                    const bool selected = slider->activeSubControls & QStyle::SC_SliderHandle && opt->state & QStyle::State_MouseOver;
                    const bool pressetHt = slider->state & QStyle::State_Sunken;

                    QColor clrLine = opt->palette.color(QPalette::Shadow);
                    QColor clrFill = opt->palette.color(QPalette::Base);

                    if (enabled && (pressetHt || selected))
                    {
                        QColor clrAccentLight = StyleHelper::colorMakePale(p.accentColor().rgb(), .9);
                        clrFill = clrAccentLight;
                        QColor clrHighlightDn = StyleHelper::colorMakeDarker(p.accentColor().rgb(), .14);
                        clrLine = clrHighlightDn;
                    }

                    handle.adjust(0, 0, -1, -1);
                    painter->fillRect(handle, clrFill);
                    painter->setPen(clrLine);
                    painter->drawRect(handle);
                }
            }
        }
        return true;
    }

    p.QProxyStyle::drawComplexControl(QStyle::CC_Slider, opt, painter, widget);
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawSpinBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (const QStyleOptionSpinBox* sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) 
    {
        QStyle::State flags = sb->state;
        const bool enabled = flags & QStyle::State_Enabled;
        const bool hasFocus = flags & QStyle::State_HasFocus;
        const bool highlighted = flags & QStyle::State_MouseOver;
        const bool selected = flags & QStyle::State_Selected;

        QStyleOptionSpinBox copy = *sb;
        if (sb->frame && (sb->subControls & QStyle::SC_SpinBoxFrame)) 
        {
            if (sb->frame)
            {
                QRect r = p.proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxFrame, widget);
                bool usePalette = sb->palette.isBrushSet(sb->palette.currentColorGroup(), QPalette::Base);
                painter->fillRect(r, !usePalette && enabled && (highlighted || selected || hasFocus) ? sb->palette.brush(QPalette::Base) :
                    enabled ? sb->palette.window().color() : sb->palette.brush(QPalette::Base));

                QColor colorBorder = sb->palette.dark().color();
                if (!enabled)
                    colorBorder = sb->palette.shadow().color();
                else if (selected)
                    colorBorder = p.accentColor();
                else if (highlighted || hasFocus)
                    colorBorder = p.accentColor();

                QtnPen pen(painter, colorBorder);
                qtn_drawRect(painter, r);
            }
        }

        if (sb->subControls & QStyle::SC_SpinBoxUp) 
        {
            copy.subControls = QStyle::SC_SpinBoxUp;
            QPalette pal2 = sb->palette;
            if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled)) 
            {
                pal2.setCurrentColorGroup(QPalette::Disabled);
                copy.state &= ~QStyle::State_Enabled;
            }
            copy.palette = pal2;
            bool press = false;
            if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_Sunken)) 
            {
                copy.state |= QStyle::State_On;
                copy.state |= QStyle::State_Sunken;
                press = true;
            }
            else 
            {
                copy.state |= QStyle::State_Raised;
                copy.state &= ~QStyle::State_Sunken;
            }
            copy.rect = p.proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxUp, widget);
            QColor clrFill = sb->palette.base().color();
            if (enabled && press && hasFocus)
                clrFill = StyleHelper::colorMakeDarker(sb->palette.highlight().color().rgb(), 0.14);
            else if (enabled && (sb->activeSubControls == QStyle::SC_SpinBoxUp) && (sb->state & QStyle::State_MouseOver))
                clrFill = sb->palette.highlight().color();

            painter->fillRect(copy.rect, clrFill);
            copy.rect.adjust(3, 0, -4, 0);
            drawIndicatorArrow(QStyle::PE_IndicatorArrowUp, &copy, painter, widget);
        }

        if (sb->subControls & QStyle::SC_SpinBoxDown) 
        {
            copy.subControls = QStyle::SC_SpinBoxDown;
            copy.state = sb->state;
            QPalette pal2 = sb->palette;
            if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled)) 
            {
                pal2.setCurrentColorGroup(QPalette::Disabled);
                copy.state &= ~QStyle::State_Enabled;
            }
            copy.palette = pal2;
            bool press = false;
            if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_Sunken)) 
            {
                copy.state |= QStyle::State_On;
                copy.state |= QStyle::State_Sunken;
                press = true;
            }
            else 
            {
                copy.state |= QStyle::State_Raised;
                copy.state &= ~QStyle::State_Sunken;
            }
            copy.rect = p.proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxDown, widget);
            QColor clrFill = sb->palette.base().color();
            if (enabled && press && hasFocus)
                clrFill = StyleHelper::colorMakeDarker(sb->palette.highlight().color().rgb(), 0.14);
            else if (enabled && (sb->activeSubControls == QStyle::SC_SpinBoxDown) && (sb->state & QStyle::State_MouseOver))
                clrFill = sb->palette.highlight().color();

            painter->fillRect(copy.rect, clrFill);
            copy.rect.adjust(3, 0, -4, 0);
            drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, &copy, painter, widget);
        }
        return true;
    }
    return false;
}

// draw ControlElement
/*! \internal */
bool Office2013StylePrivate::drawRibbonTabShapeLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const OfficeStyle)
    if (const RibbonOptionHeaderStyleOption* optTab = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        const bool selected = opt->state & QStyle::State_Selected;
        const bool highlighted = opt->state & QStyle::State_MouseOver;
        int flags = static_cast<int>(optTab->textAlignment);

        flags |= Qt::TextShowMnemonic;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;

        QColor textColor = opt->palette.color(QPalette::WindowText);

        if (selected && optTab->pageColor != Qtitan::ContextColorNone)
        {
            textColor = CommonStyle::contextColorToColor(optTab->pageColor);
            textColor = StyleHelper::colorMakeDarker(textColor.rgb(), .4);
        }
        else if (selected || highlighted)
            textColor = textRibbonTabBarColor(opt);

        if (m_isActiveTabAccented && selected)
            textColor = opt->palette.color(QPalette::Window);

        QString text = optTab->text;
        if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), opt, w))
            text = text.toUpper();

        // draw text
        QPalette palette = optTab->palette;
        QPalette_setColor(palette, QPalette::WindowText, textColor);
        p.proxy()->drawItemText(painter, optTab->rect, flags, palette, opt->state & QStyle::State_Enabled, text, QPalette::WindowText);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const RibbonOptionHeaderStyleOption* optTab = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        QRect rectTab = optTab->rect;
        if (optTab->state & QStyle::State_Selected)
        {
            // fill the background of the tab
            painter->fillRect(rectTab, optTab->palette.button().color());
            QPainterPath path;
            if (optTab->tabBarPosition == 1)
            {
                path.moveTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.bottom());
            }
            else if (optTab->tabBarPosition == 2)
            {
                path.moveTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.top());
            }
            else
                return true;

            const QPen savePen = painter->pen();
            const QColor color = CommonStyle::contextColorToColor(optTab->pageColor);
            painter->setPen(color.isValid() ? color : optTab->palette.midlight().color());
            painter->drawPath(path);
            painter->setPen(savePen);
        }

        if (optTab->state & QStyle::State_Small)
        {
            QRect rectTab = optTab->rect;
            QPainterPath path;
            path.moveTo(rectTab.right(), rectTab.top());
            path.lineTo(rectTab.right(), rectTab.bottom());
            const QPen savePen = painter->pen();
            painter->setPen(optTab->palette.midlight().color());
            painter->drawPath(path);
            painter->setPen(savePen);
        }

        if (optTab->pageColor != Qtitan::ContextColorNone && !(optTab->state & QStyle::State_Selected))
        {
            QColor clrContextTab = colorSubControl(QStringLiteral("RibbonTitleBarWidget"), QStringLiteral("ContextTab"), QPalette::Active, QPalette::Window);
            if (!clrContextTab.isValid())
            {
                const QColor color = CommonStyle::contextColorToColor(optTab->pageColor);
                clrContextTab = StyleHelper::colorMakePale(color.rgb());
            }

            QRect rc = optTab->rcFirst | optTab->rcLast;
            painter->fillRect(rc.adjusted(0, 0, 0, -1), clrContextTab);
        }
    }
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonGalleryItem(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2013Style)
    if (const RibbonGalleryItemStyleOption* optItem = qstyleoption_cast<const RibbonGalleryItemStyleOption*>(opt))
    {
        bool enabled = opt->state & QStyle::State_Enabled;
        QRect rectItem = optItem->rectItem;
        if (optItem->separator)
        {
            painter->fillRect(rectItem, opt->palette.mid().color());
            painter->fillRect(rectItem.left(), rectItem.bottom() - 1, rectItem.width(), 1, outlineMidlight(optItem->palette));

            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter;

            QPalette palette = optItem->palette;
            QColor colText = textGalleryItem(optItem->palette);
            QPalette_setColor(palette, QPalette::WindowText, colText);

            rectItem.adjust(10, 0, 0, 0);
            const QFont& saveFont = painter->font();
            QFont font(saveFont);
            font.setBold(true);
            painter->setFont(font);
            p.proxy()->drawItemText(painter, rectItem, alignment, palette, true, optItem->caption, QPalette::WindowText);
            painter->setFont(saveFont);
            return true;
        }
        if (!optItem->icon.isNull())
        {
            QStyleOption optTmp = *optItem;
            optTmp.rect = optItem->rectItem;
            drawHighlightItemGallery(&optTmp, painter, w);

            QList<QSize> lstSz = optItem->icon.availableSizes(enabled ? QIcon::Normal : QIcon::Disabled);
            QSize iconSize = lstSz.size() > 0 ? lstSz[0] : rectItem.size();
            iconSize = QSize(CommonStylePrivate::dpiScaled(iconSize.width(), w), CommonStylePrivate::dpiScaled(iconSize.height(), w));
            QPixmap pm = preparePixmap(optItem->icon, iconSize, enabled ? QIcon::Normal : QIcon::Disabled, QIcon::Off, w);
            p.proxy()->drawItemPixmap(painter, rectItem, Qt::AlignCenter, pm);
        }
        else
        {
            QStyleOption optTmp = *optItem;
            optTmp.rect = optItem->rectItem;
            drawHighlightButton(&optTmp, painter, w);

            QRect rcText(rectItem);
            rcText.setLeft(rcText.left() + 3);
            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter | Qt::TextHideMnemonic;
            QPalette palette;
            p.proxy()->drawItemText(painter, rcText, alignment, palette, enabled, optItem->caption, QPalette::WindowText);
        }
    }
    return true;
}

/*! \internal */
void Office2013StylePrivate::drawMenuItemSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        if (menuitem->text.isEmpty())
        {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);

            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = p.proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
            int yoff = (y - 1 + h / 2);
            int xIcon = iconSize;
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu")))
                xIcon = 0;

            QPen penSave = painter->pen();
            painter->setPen(opt->palette.light().color());
            painter->drawLine(x + 2 + xIcon, yoff, x + w - 4, yoff);
            painter->setPen(penSave);
        }
        else
        {
            painter->save();
            painter->fillRect(menuitem->rect, menuitem->palette.shadow().color());

            QRect rectText = menuitem->rect;
            rectText.adjust(iTextMargin, 0, -iTextMargin, -iTextMargin);

            QFont font = menuitem->font;
            font.setBold(true);
            painter->setFont(font);
            painter->setPen(opt->state & QStyle::State_Enabled ? menuitem->palette.windowText().color() : menuitem->palette.windowText().color());

            int alignment = Qt::AlignVCenter | Qt::TextSingleLine;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                alignment |= Qt::TextHideMnemonic;

            painter->drawText(rectText, alignment, menuitem->text);
            painter->restore();
        }
    }
}

/*! \internal */
bool Office2013StylePrivate::drawShapedFrame(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageSeparator")))
    {
        if (const QtStyleOptionFrame* f = qstyleoption_cast<const QtStyleOptionFrame*>(opt))
        {
            painter->setPen(QPen(opt->palette.shadow().color()));
            if (f->frameShape == QFrame::HLine)
                painter->drawLine(opt->rect.topLeft(), opt->rect.topRight());
            else
                painter->drawLine(opt->rect.topLeft(), opt->rect.bottomLeft());
            return true;
        }
    }

    bool ret = false;
    if (const QMdiArea* mdiArea = qobject_cast<const QMdiArea*>(widget))
    {
        if (const QtStyleOptionFrame* f = qstyleoption_cast<const QtStyleOptionFrame*>(opt))
        {
            int frameShape = f->frameShape;
            int frameShadow = QFrame::Plain;
            if (f->state & QStyle::State_Sunken)
                frameShadow = QFrame::Sunken;
            else if (f->state & QStyle::State_Raised)
                frameShadow = QFrame::Raised;

            switch (frameShape)
            {
                case QFrame::Panel:
                {
                    if (frameShadow == QFrame::Sunken)
                    {
                        QRect rect = opt->rect;
                        if (QTabBar* tabBar = mdiArea->findChild<QTabBar*>())
                        {
                            painter->fillRect(opt->rect.adjusted(-2, -2, 2, 2), opt->palette.window().color());
                            switch (mdiArea->tabPosition())
                            {
                                case QTabWidget::North:
                                    rect.adjust(0, tabBar->sizeHint().height() - 1, 0, 0);
                                    break;
                                case QTabWidget::South:
                                    rect.adjust(0, 0, 0, -tabBar->sizeHint().height() + 1);
                                    break;
                                case QTabWidget::West:
                                    rect.adjust(tabBar->sizeHint().width() - 1, 0, 1, 0);
                                    break;
                                case QTabWidget::East:
                                    rect.adjust(0, 0, -1, -1);
                                    break;
                            }
                            QPen thePen(opt->palette.shadow().color().darker(140));
                            painter->setPen(thePen);
                            painter->drawRect(rect.adjusted(0, 0, -1, -1));
                        }
                        ret = true;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    return ret;
}

/*! \internal */
bool Office2013StylePrivate::drawMenuItem(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle)
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(option))
    {
        int x, y, w, h;
        menuitem->rect.getRect(&x, &y, &w, &h);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        int tab = 0;
#else
        int tab = menuitem->tabWidth;
#endif
        bool disabled = !(menuitem->state & QStyle::State_Enabled);

        bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable ? menuitem->checked : false;
        bool activated = menuitem->state & QStyle::State_Selected;

        // windows always has a check column, regardless whether we have an icon or not
        int iconSize = p.proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, option, widget);
        int checkcol = iconSize;
#ifdef Q_OS_LINUX
        if (qobject_cast<const QComboBox*>(widget))
        {
            checkcol = 0;
            checked = false;
        }
#endif /* Q_OS_LINUX */
        if (activated)
        {
            QColor colorFill = buttonHighlightColor(option->palette, false, false, widget); // highlightDn
            QColor colorBorder = buttonHighlightColor(option->palette, false, true, widget); // highlightDn
            QtnBrush brush(painter, colorFill);
            QtnPen pen(painter, colorBorder, 1);
            qtn_drawRect(painter, menuitem->rect.adjusted(2, 1, -2, -1));
        }

//        if (menuitem->text.count(QString(_qtn_SplitActionPopup)) != 0)
//            drawSplitButtonPopup(p, menuitem->rect, activated, !disabled, menuitem->checkType == QStyleOptionMenuItem::Exclusive);

        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            drawMenuItemSeparator(option, painter, widget);
            return true;
        }

        QRect vCheckRect = p.visualRect(option->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));
        
        if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")))
        {
            iconSize = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, option, widget);
            if (iconSize > option->rect.height())
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, option, widget);
            checkcol = iconSize + 6;
        }

        // On Windows Style, if we have a checkable item and an icon we
        // draw the icon recessed to indicate an item is checked. If we
        // have no icon, we draw a checkmark instead.
        if (!menuitem->icon.isNull())
        {
            QIcon::Mode mode = disabled ? QIcon::Disabled : QIcon::Normal;
            if (activated && !disabled)
                mode = QIcon::Active;

            if (!(widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu"))))
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, option, widget);

            QPixmap pixmap = preparePixmap(menuitem->icon, QSize(iconSize, iconSize), mode, checked ? QIcon::On : QIcon::Off, widget);
            QRect pmr(QPoint(0, 0), QSize(iconSize, iconSize));
            pmr.moveCenter(vCheckRect.center());
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")))
                pmr.translate(6, 0);

            painter->setPen(menuitem->palette.text().color());
            if (checked)
            {
                QRect vIconRect = p.visualRect(option->direction, option->rect, pmr);
                QRect rcChecked = menuitem->rect;

                rcChecked.setLeft(vIconRect.left());
                if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")))
                    rcChecked.translate(6, 0);
                rcChecked.setWidth(vIconRect.width());

                QColor colorFill = buttonHighlightColor(option->palette, true, false, widget); // highlightDn
                QColor colorBorder = buttonHighlightColor(option->palette, true, true, widget); // highlightDn

                QtnBrush brush(painter, colorFill);
                QtnPen pen(painter, colorBorder, 1);
                qtn_drawRect(painter, rcChecked.adjusted(-2, 0, 2, 0));
            }
            painter->drawPixmap(pmr.topLeft(), pixmap);
        }
        else if (checked)
        {
            QStyleOptionMenuItem newMi = *menuitem;
            newMi.state = QStyle::State_None;
            if (!disabled)
                newMi.state |= QStyle::State_Enabled;
            if (activated)
                newMi.state |= QStyle::State_On;
            newMi.rect = p.visualRect(option->direction, menuitem->rect, QRect(menuitem->rect.x() + windowsItemFrame, menuitem->rect.y() + windowsItemFrame,
                checkcol - 2 * windowsItemFrame, menuitem->rect.height() - 2 * windowsItemFrame));
            p.proxy()->drawPrimitive(QStyle::PE_IndicatorMenuCheckMark, &newMi, painter, widget);
        }

        QColor discol;
        if (disabled)
        {
            discol = menuitem->palette.text().color();
            painter->setPen(discol);
        }

        int xm = windowsItemFrame + checkcol + 4;
#ifdef Q_OS_LINUX
        if (qobject_cast<const QComboBox*>(widget))
            xm = windowsItemFrame + checkcol + windowsItemHMargin;
#endif /* Q_OS_LINUX */
        int xpos = menuitem->rect.x() + xm;
        QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
        QRect vTextRect = p.visualRect(option->direction, menuitem->rect, textRect);
        QString s = menuitem->text;
        s = s.remove(QStringLiteral(_qtn_SplitActionPopup));
        // draw text    
        if (!s.isEmpty())
        {
            painter->save();
            int t = s.indexOf(QLatin1Char('\t'));
            int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, menuitem, widget))
                text_flags |= Qt::TextHideMnemonic;
            text_flags |= Qt::AlignLeft;
            // draw hotkeys
            if (t >= 0)
            {
                QRect vShortcutRect = p.visualRect(option->direction, menuitem->rect, QRect(textRect.topRight(),
                    QPoint(menuitem->rect.right(), textRect.bottom())));
                painter->setPen(option->state & QStyle::State_Enabled ? menuitem->palette.windowText().color() : menuitem->palette.windowText().color());
                painter->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                painter->setPen(discol);
                s = s.left(t);
            }

            QFont font = menuitem->font;
            if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                font.setBold(true);
            painter->setFont(font);
            painter->setPen(option->state & QStyle::State_Enabled ? menuitem->palette.windowText().color() : menuitem->palette.windowText().color());
            painter->drawText(vTextRect, text_flags, s.left(t));
            painter->setPen(discol);
            painter->restore();
        }
        // draw sub menu arrow
        if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            int dim = (h - 2 * windowsItemFrame) / 2;
            QStyle::PrimitiveElement arrow;
            arrow = (option->direction == Qt::RightToLeft) ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
            xpos = x + w - windowsArrowHMargin - windowsItemFrame - dim;
            QRect  vSubMenuRect = p.visualRect(option->direction, menuitem->rect, QRect(xpos, y + h / 2 - dim / 2, dim, dim));
            QStyleOptionMenuItem newMI = *menuitem;
            newMI.rect = vSubMenuRect;
            newMI.state = disabled ? QStyle::State_None : QStyle::State_Enabled;
            p.proxy()->drawPrimitive(arrow, &newMI, painter, widget);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    QTN_P(const OfficeStyle)
    QColor color = p.accentColor();
    int r = color.red();
    int g = color.green();
    int b = color.blue();
    color = (r + g + b + 1) / 3 < 128 ? Qt::white : Qt::gray;

    QPoint pt(opt->rect.right() - 3, opt->rect.bottom() - 3);
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3 - y; x++)
            painter->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), color);
    }
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawToolBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    Q_UNUSED(painter)
    Q_UNUSED(opt)
    return false;
}

/*! \internal */
QPoint adjustScrollPoint(const QPoint &point, Qt::Orientation orientation, bool add)
{
    int adder = add ? -1 : 1;
    QPoint retPoint;

    if (orientation == Qt::Horizontal) {
        retPoint = QPoint(point.y() * adder, point.x());
    }
    else {
        retPoint = QPoint(point.x(), point.y() * adder);
    }
    return retPoint;
}

/*! \internal */
bool Office2013StylePrivate::drawScrollBarAddSubLine(QStyle::ControlElement element, const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const OfficeStyle)
    if (p.isScrollBarsIgnored())
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool highlighted = ((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
            ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine));

        const bool pressed = opt->state & QStyle::State_Sunken;

        bool pressButton = false;
        if (highlighted && pressed)
        {
            if (((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)))
                pressButton = true;
        }

        QColor clrFill = pressButton || !enabled ? scrollbar->palette.shadow().color() : scrollbar->palette.window().color();
        painter->fillRect(scrollbar->rect, clrFill);

        painter->setPen(highlighted ? scrollbar->palette.shadow().color().darker() : scrollbar->palette.shadow().color().darker(140)); // QColor(170, 170. 170)
        painter->drawRect(scrollbar->rect);

        QStyle::PrimitiveElement arrow;
        if (opt->state & QStyle::State_Horizontal)
        {
            if (element == QStyle::CE_ScrollBarAddLine)
                arrow = opt->direction == Qt::LeftToRight ? QStyle::PE_IndicatorArrowRight : QStyle::PE_IndicatorArrowLeft;
            else
                arrow = opt->direction == Qt::LeftToRight ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
        }
        else
        {
            if (element == QStyle::CE_ScrollBarAddLine)
                arrow = QStyle::PE_IndicatorArrowDown;
            else
                arrow = QStyle::PE_IndicatorArrowUp;
        }
        QStyleOption arrowOpt = *opt;
        arrowOpt.rect = opt->rect.adjusted(1, 1, -1, -1);
        p.proxy()->drawPrimitive(arrow, &arrowOpt, painter, w);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawScrollBarAddSubPage(QStyle::ControlElement element, const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(element)
    Q_UNUSED(opt)
    Q_UNUSED(painter)
    Q_UNUSED(w)
    QTN_P(const OfficeStyle)
    if (p.isScrollBarsIgnored())
        return false;
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawScrollBarSlider(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const OfficeStyle)
    if (p.isScrollBarsIgnored())
        return false;
    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        if (!scrollbar->rect.isEmpty())
        {
            const bool enabled = scrollbar->state & QStyle::State_Enabled;
            const bool highlighted = scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider;
            const bool pressed = scrollbar->state & QStyle::State_Sunken;

            QRect rc(scrollbar->rect);
            bool show = scrollbar->state & QStyle::State_Horizontal ? rc.width() > 7 : rc.height() > 7;
            if (show)
            {
                QColor clrFill = (highlighted && pressed) || !enabled ? scrollbar->palette.shadow().color() : scrollbar->palette.window().color();
                painter->fillRect(rc, clrFill);
                QPen newPen(QBrush(highlighted ? scrollbar->palette.shadow().color().darker() : scrollbar->palette.shadow().color().darker(140)), 1);
                painter->setPen(newPen);
                painter->drawRect(rc);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawToolBoxTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w)
    qDrawShadePanel(p, opt->rect, opt->palette, opt->state & (QStyle::State_Sunken | QStyle::State_On), 1, &opt->palette.brush(QPalette::Button));
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawTabBarTabShape(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle)
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab *>(option))
    {
        painter->save();
        const bool rtlHorTabs = (tab->direction == Qt::RightToLeft && (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth));
        bool selected = tab->state & QStyle::State_Selected;
        bool isHot = tab->state & QStyle::State_MouseOver;
        const bool lastTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::End) || (rtlHorTabs && tab->position == QStyleOptionTab::Beginning));
        const bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
        const int tabOverlap = 1;//baseStyle()->pixelMetric(QStyle::PM_TabBarTabOverlap, option, widget);//pixelMetric(QStyle::PM_TabBarTabOverlap, option, widget);
        QRect rect = option->rect.adjusted(0, 0, (onlyOne || lastTab) ? 0 : tabOverlap, 0);

        QRect r2(rect);
        int x1 = r2.left();
        int x2 = r2.right();
        int y1 = r2.top();
        int y2 = r2.bottom();
        QTransform rotMatrix;
        bool flip = false;
        switch (tab->shape)
        {
            case QTabBar::RoundedNorth:
                break;
            case QTabBar::RoundedSouth:
                rotMatrix.rotate(180);
                rotMatrix.translate(0, -rect.height() + 1);
                rotMatrix.scale(-1, 1);
                painter->setTransform(rotMatrix, true);
                break;
            case QTabBar::RoundedWest:
                rotMatrix.rotate(180 + 90);
                rotMatrix.scale(-1, 1);
                flip = true;
                painter->setTransform(rotMatrix, true);
                break;
            case QTabBar::RoundedEast:
                rotMatrix.rotate(90);
                rotMatrix.translate(0, -rect.width() + 1);
                flip = true;
                painter->setTransform(rotMatrix, true);
                break;
            default:
                painter->restore();
                p.QCommonStyle::drawControl(QStyle::CE_TabBarTabShape, tab, painter, widget);
                return true;
        }

        if (flip)
        {
            QRect tmp = rect;
            rect = QRect(tmp.y(), tmp.x(), tmp.height(), tmp.width());
            int temp = x1;
            x1 = y1;
            y1 = temp;
            temp = x2;
            x2 = y2;
            y2 = temp;
        }

        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->translate(0.5, 0.5);
        QColor tabFrameColor = selected ? tab->palette.base().color() : buttonHighlightColor(tab->palette, false, false);
        QRect drawRect = rect.adjusted(0, selected ? 0 : 2, -1, 3);
        if (selected || isHot)
        {
            painter->setPen(tab->palette.dark().color());
            painter->setBrush(tabFrameColor);
            painter->drawRect(drawRect.adjusted(0, 0, -1, -1));
        }

        if (selected)
        {
            painter->fillRect(rect.left() + 1, rect.bottom() - 1, rect.width() - 2, rect.bottom() - 1, tabFrameColor);
            painter->fillRect(QRect(rect.bottomRight() + QPoint(-2, -1), QSize(1, 1)), tab->palette.window().color());
            painter->fillRect(QRect(rect.bottomLeft() + QPoint(0, -1), QSize(1, 1)), tab->palette.window().color());
            painter->fillRect(QRect(rect.bottomRight() + QPoint(-1, -1), QSize(1, 1)), tab->palette.window().color());
        }

        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawHeaderSection(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionHeader* header = qstyleoption_cast<const QStyleOptionHeader *>(opt))
    {
        QBrush fill;
        if (header->state & QStyle::State_Sunken)
            fill = buttonHighlightColor(header->palette, true, false/*border*/);
        else if (header->state & QStyle::State_MouseOver)
            fill = buttonHighlightColor(header->palette, false, false/*border*/);
        else
            fill = header->palette.brush(QPalette::Button);
        painter->fillRect(header->rect, fill);
        QtnPen pen(painter, header->palette.light().color());
        painter->drawRect(header->rect.adjusted(0, 0, -1, -1));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawDockWidgetTitle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle)
    if (qobject_cast<const QDockWidget*>(w))
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (const QStyleOptionDockWidget* dwOpt = qstyleoption_cast<const QStyleOptionDockWidget*>(opt)) 
#else
        if (const QStyleOptionDockWidgetV2* dwOpt = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(opt)) 
#endif
        {
            painter->fillRect(dwOpt->rect, dwOpt->palette.shadow());
            QRect rect = dwOpt->rect.adjusted(0, 0, -1, -1);
            if (dwOpt->movable) 
            {
                painter->setPen(dwOpt->palette.color(QPalette::Dark));
                painter->drawRect(rect);
            }

            if (!dwOpt->title.isEmpty()) 
            {
                const bool verticalTitleBar = dwOpt->verticalTitleBar;
                if (verticalTitleBar) 
                {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
                    rect = rect.transposed();
#else
                    rect = QRect(rect.topLeft(), QSize(rect.height(), rect.width()));
#endif
                    painter->save();
                    painter->translate(rect.left(), rect.top() + rect.width());
                    painter->rotate(-90);
                    painter->translate(-rect.left(), -rect.top());
                }

                QString text = dwOpt->title;
                if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), opt, w))
                    text = text.toUpper();

                const int indent = painter->fontMetrics().descent();
                p.proxy()->drawItemText(painter, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                    dwOpt->palette, dwOpt->state & QStyle::State_Enabled, text, QPalette::WindowText);

                if (verticalTitleBar)
                    painter->restore();
            }
        }
        return true;
    }
    return CommonStylePrivate::drawDockWidgetTitle(opt, painter, w);
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonBar(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    bool draw = false;
    if (const RibbonStyleOption* optRibbon = qstyleoption_cast<const RibbonStyleOption*>(opt))
    {
        RibbonStyleOption options = *optRibbon;
        if (optRibbon->isBackstageVisible)
        {
            QPalette_setBrush(options.palette, QPalette::Active, QPalette::Window, Qt::white);
            QPalette_setBrush(options.palette, QPalette::Inactive, QPalette::Window, Qt::white);
        }
        draw = CommonStylePrivate::drawRibbonBar(&options, painter, widget);
    }

#if 0 //TODO remove me
    if (draw)
    {
        if (const RibbonStyleOption* optRibbonBar = qstyleoption_cast<const RibbonStyleOption*>(opt))
        {
            if (!optRibbonBar->pixTitleBackground.isNull())
            {
                RibbonBackgroundStyleOption ob;
                ob.rect = optRibbonBar->rect;
                ob.titleBarHeight = optRibbonBar->titleBarHeight;
                ob.pixTitleBackground = optRibbonBar->pixTitleBackground;
                const int fwidth = p.proxy()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, Q_NULL, widget);
                ob.rect.translate(fwidth, fwidth);
                p.proxy()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(CommonStyle::PE_RibbonImageBackground), &ob, painter, widget);
            }
            return true;
        }                   
    }
#endif
    return draw;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonFrameGroup(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QRect rect = option->rect.adjusted(0, 0, 0, -6);
    painter->setPen(option->palette.midlight().color());
    painter->drawLine(QPoint(rect.right() - 1, rect.top()), QPoint(rect.right() - 1, rect.bottom()));
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonFrameGallery(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const QtStyleOptionFrame* frameGallery = qstyleoption_cast<const QtStyleOptionFrame *>(opt))
    {
        if (const QMenu* parentMenu = qobject_cast<const QMenu*>(widget ? widget->parentWidget() : Q_NULL))
        {
            QPalette pal = parentMenu->palette();
            painter->fillRect(opt->rect, pal.color(QPalette::Base));
            return true;
        }
        if (frameGallery->features & QtStyleOptionFrame::Flat)
        {
            const bool enabled = opt->state & QStyle::State_Enabled;
            const bool selected = opt->state & QStyle::State_MouseOver;
            painter->fillRect(opt->rect, selected && enabled ? opt->palette.window().color().lighter(110) : opt->palette.window().color());
        }
        if (frameGallery->lineWidth > 0)
        {
            const QPen savePen = painter->pen();
            painter->setPen(opt->palette.midlight().color());
            painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            painter->setPen(savePen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonBackstageFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_P(const Office2013Style);
    if (const RibbonBackstageStyleOption* optBackstage = qstyleoption_cast<const RibbonBackstageStyleOption*>(opt))
    {
        const bool hideTabs = p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonBackstageHideTabs));
        painter->fillRect(opt->rect, optBackstage->palette.window());
        if (!hideTabs)
            StyleHelper::drawGradientFill(*painter, opt->rect, fillBackstageFrameColor(optBackstage->palette), 
                optBackstage->palette.window().color(), false);

        const int top = hideTabs ? 0 : 2;
        QRect rc = QRect(QPoint(0, top), QPoint(optBackstage->menuWidth, opt->rect.bottom() + 1));
        painter->fillRect(rc, fillBackstageFrameColor(optBackstage->palette));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonBackstageCloseButton(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const QStyleOptionToolButton* optButton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        if (!optButton->icon.isNull())
        {
            const bool enabled = optButton->state & QStyle::State_Enabled;
            const bool highlighted = optButton->state & QStyle::State_MouseOver;
            QImage image(optButton->iconSize, QImage::Format_ARGB32_Premultiplied);
            image.fill(m_clrAccent);
            QPainter lockPainter(&image);
            QPixmap pixmap = optButton->icon.pixmap(optButton->iconSize, enabled ? QIcon::Active : QIcon::Disabled, QIcon::On);
            QImage img = pixmap.toImage();
            if (highlighted)
                img.setAlphaChannel(image);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            const int pixw = qRound(img.width() / pixmap.devicePixelRatio());
            const int pixh = qRound(img.height() / pixmap.devicePixelRatio());
#else
            const int pixw = img.width();
            const int pixh = img.height();
#endif
#if QTN_DEPRECATED_CODE
            if (pixw * pixh < optButton->iconSize.width() * optButton->iconSize.height())
                img = img.scaled(optButton->iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
#endif
            painter->drawImage(QPoint(), img);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2013StylePrivate::drawRibbonSliderButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    const bool left = opt->direction != Qt::RightToLeft;

    const bool highlighted = opt->state & QStyle::State_MouseOver;
    const bool pressed = opt->state & QStyle::State_Sunken;
    const bool enabled = opt->state & QStyle::State_Enabled;

    QPoint ptCenter = opt->rect.center();
    int radius = CommonStylePrivate::dpiScaled(7, w);
    QRect rectSign(QPoint(ptCenter.x() - radius / 2, ptCenter.y() - radius / 2), QSize(radius, radius));

    QColor clrLine;
    if (!enabled)
        clrLine = opt->palette.color(QPalette::ButtonText);
    else if (pressed && highlighted)
        clrLine = opt->palette.color(QPalette::ButtonText).darker(120);//d.m_clrMenubarBorder;
    else if (highlighted)
        clrLine = opt->palette.color(QPalette::ButtonText).darker(120);//d.m_clrMenubarBorder;
    else
        clrLine = opt->palette.color(QPalette::ButtonText);


    const QPen savePen = p->pen();
    p->setPen(clrLine);

    QPainterPath path;
    path.moveTo(rectSign.left(), ptCenter.y());
    path.lineTo(rectSign.right(), ptCenter.y());
    if (left)
    {
        path.moveTo(ptCenter.x(), rectSign.top());
        path.lineTo(ptCenter.x(), rectSign.bottom());
    }
    p->drawPath(path);
    p->setPen(savePen);
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawPanelTipLabel(const QStyleOption* opt, QPainter* painter, const QWidget* wd) const
{
    Q_UNUSED(wd)
    const QBrush brush(opt->palette.toolTipBase());
    const bool isColorLight = qtn_isColorLight(brush.color());
    const QColor border = isColorLight ? StyleHelper::colorMakeLighter(opt->palette.toolTipText().color().rgb(), 0.5) :
        StyleHelper::colorMakeDarker(opt->palette.toolTipText().color().rgb(), 0.55);
    qDrawPlainRect(painter, opt->rect, border, 1, &brush);
    return true;
}

/*! \internal */
bool Office2013StylePrivate::drawKeyTip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QColor clrLight = opt->palette.color(QPalette::Light);
    QColor clrDark = opt->palette.color(QPalette::Midlight);
    StyleHelper::drawGradientFill(*painter, opt->rect, clrLight, clrDark, true);
    return true;
}

/*! \internal */
int Office2013StylePrivate::ribbonStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    int ret = CommonStylePrivate::InvalidHint;
    switch (static_cast<CommonStyle::StyleHintEx>(stylehint))
    {
#ifdef Q_OS_WIN
        case CommonStyle::SH_RibbonBackstageHideTabs: ret = true; break;
#else // Q_OS_WIN
        case CommonStyle::SH_RibbonBackstageHideTabs: ret = false; break;
#endif // Q_OS_WIN
        case CommonStyle::SH_RibbonItemUpperCase: ret = true; break;
        default: ret = OfficeStylePrivate::ribbonStyleHint(stylehint, opt, widget, returnData); break;
    }
    return ret;
}

/*! \internal */
void Office2013StylePrivate::drawHighlightItemGallery(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    const bool highlighted = option->state & QStyle::State_MouseOver;
    const bool pressed = option->state & QStyle::State_Sunken;
    const bool enabled = option->state & QStyle::State_Enabled;
    const bool checked = option->state & QStyle::State_On;
    if (enabled && (highlighted || pressed || checked))
    {
        const QPen savePen = painter->pen();
        painter->setPen(buttonHighlightColor(option->palette, pressed, true));
        QBrush fill = buttonHighlightColor(option->palette, pressed, false);
        painter->fillRect(option->rect.adjusted(1, 0, -2, 0), fill);
        painter->drawRect(option->rect.adjusted(1, 0, -2, -1));
        painter->setPen(savePen);
    }
}

/*! \internal */
QColor Office2013StylePrivate::textGalleryItem(const QPalette& pal) const
{
    return pal.brush(QPalette::WindowText).color();
}

/*! \internal */
QColor Office2013StylePrivate::darkBackgroundLight(const QPalette& pal) const
{
    return pal.color(QPalette::Window);
}

/*! \internal */
QColor Office2013StylePrivate::buttonHighlightColor(const QPalette& /*palette*/, bool highlightDn, bool border, const QWidget* widget) const
{
    QTN_P(const Office2013Style)
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageView")))
    {
        if (border)
            return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.2) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.2);
        return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.11) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.19);
    }
    if (border)
        return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.0) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.2);
    return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.3) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.4);
}

/*! \internal */
QColor Office2013StylePrivate::fillTitleBarColor(const QPalette& /*pal*/, bool isBackstageVisible) const
{ 
    if (isBackstageVisible)
        return Qt::white;
    return QColor(); 
}

/*! \internal */
QColor Office2013StylePrivate::buttonRibbonFileColor(const SystemToolButtonStyleOption* opt) const 
{ 
    if (m_themeType == Office2013Style::Dark)
        return opt->palette.color(QPalette::Button);
    return m_clrAccent; 
}

/*! \internal */
QColor Office2013StylePrivate::fillBackstageFrameColor(const QPalette& pal) const 
{ 
    if (m_themeType == Office2013Style::Dark)
        return pal.color(QPalette::Dark);
    return m_clrAccent;
}

/*! \internal */
QColor Office2013StylePrivate::titleBarWindowColor(const QPalette& pal, bool isActive, const QWidget* widget) const
{
    QTN_P(const Office2013Style);
    if (widget != Q_NULL && (qtn_hasWindowTitleBar(widget) || widget->inherits("QMdiSubWindow")))
    {
        QPalette palette = pal;
        QPalette_setColor(palette, isActive ? QPalette::Active : QPalette::Inactive, QPalette::Shadow, p.accentColor());
        return palette.color(isActive ? QPalette::Active : QPalette::Inactive, QPalette::Shadow);
    }
    else
        return OfficeStylePrivate::titleBarWindowColor(pal, isActive, widget);
}

/*! \internal */
QColor Office2013StylePrivate::titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* widget) const
{
    QTN_P(const Office2013Style);
    if (widget != Q_NULL && widget->inherits("QMdiSubWindow"))
    {
        return QColor();
    }
    else if (qtn_hasWindowTitleBar(widget))
    {
        QPalette palette = pal;
        if (isActive)
            QPalette_setColor(palette, QPalette::Active, QPalette::Dark,
                m_themeType == Office2013Style::Dark ? QPalette::Shadow : p.accentColor());
        return palette.color(isActive ? QPalette::Active : QPalette::Inactive, QPalette::Dark);
    }
    else
        return OfficeStylePrivate::titleBarFrameColor(pal, isActive, widget);
}

/*! \internal */
QBrush Office2013StylePrivate::brushFromConfig(const QString& className, const QString& subControlId, QPalette::ColorGroup group, QPalette::ColorRole role, QStyle::State state, bool isBackstageVisible) const
{
    Q_UNUSED(state)
    Q_UNUSED(isBackstageVisible)
    return brushSubControl(className, subControlId, group, role);
}

/*! \internal */
void Office2013StylePrivate::updateTheme()
{
    QTN_P(OfficeStyle)
    QString themeName;
    switch (m_themeType)
    {
        case Office2013Style::White:
            themeName = QStringLiteral("White");
            break;
        case Office2013Style::Gray:
            themeName = QStringLiteral("Gray");
            break;
        case Office2013Style::Dark:
            themeName = QStringLiteral("Dark");
            break;
        default:
            Q_ASSERT(false);
            break;
    }
    p.setStyleConfig(QStringLiteral(":/res/Office2013/"), themeName);
}

/*! \internal */
QPalette Office2013StylePrivate::getWidgetPalette(QWidget* widget) const
{
    QTN_P(const Office2013Style);
    QPalette palette = OfficeStylePrivate::getWidgetPalette(widget);
    if (!widgetStyleSupport(widget))
        return palette;

    QColor clrAccent = p.accentColor();
    if (widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemRecentFileList")))
    {
        QColor colorFill = StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.5);
        QPalette_setBrush(palette, QPalette::Active, QPalette::Highlight, colorFill);
    } 
    else if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageViewMenu")))
    {
        if (p.theme() == Office2013Style::Dark)
            clrAccent = qApp->palette().color(QPalette::Dark);
        QPalette_setBrush(palette, QPalette::Active, QPalette::Midlight, clrAccent);
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Midlight, clrAccent);
        QPalette_setBrush(palette, QPalette::Active, QPalette::Light, clrAccent.lighter(130));
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Light, clrAccent.lighter(130));
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("StatusSeparator")))
    {
        if (p.theme() == Office2013Style::Dark)
            clrAccent = qApp->palette().color(QPalette::Light).darker(170);
        QPalette_setBrush(palette, QPalette::Active, QPalette::Dark, clrAccent.lighter(115));
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Dark, clrAccent.lighter(115));
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("StatusButton")) || (widget->parentWidget() && widget->parentWidget()->inherits("QStatusBar")))
    {
        if (p.theme() == Office2013Style::Dark)
        {
            QPalette_setBrush(palette, QPalette::Active, QPalette::ButtonText, Qt::white);
            QPalette_setBrush(palette, QPalette::Active, QPalette::Button, clrAccent);
            QPalette_setBrush(palette, QPalette::Inactive, QPalette::ButtonText, Qt::white);
            QPalette_setBrush(palette, QPalette::Inactive, QPalette::Button, clrAccent);
        }
        else
        {
            const bool isRibbonStatusBarDark = p.theme() != Office2013Style::Dark && isDarkColor(clrAccent);
            QPalette_setBrush(palette, QPalette::Active, QPalette::ButtonText, isRibbonStatusBarDark ? Qt::white : Qt::black);
            QPalette_setBrush(palette, QPalette::Active, QPalette::Button, clrAccent);
            QPalette_setBrush(palette, QPalette::Active, QPalette::Light, clrAccent); // line - left:top
            QPalette_setBrush(palette, QPalette::Active, QPalette::Dark, clrAccent);  // line - right:bottom 
            QPalette_setBrush(palette, QPalette::Inactive, QPalette::ButtonText, Qt::white);
            QPalette_setBrush(palette, QPalette::Inactive, QPalette::Button, clrAccent);
            QPalette_setBrush(palette, QPalette::Inactive, QPalette::Light, clrAccent); // line - left:top
            QPalette_setBrush(palette, QPalette::Inactive, QPalette::Dark, clrAccent);  // line - right:bottom 
        }
    }
    return palette;
}


/*!
\class Office2013Style
\inmodule QtitanStyle
\brief Office2013Style class implements Microsoft Office 2013 style.
*/
/*!
Constuctor of Office2013Style class.
*/
Office2013Style::Office2013Style()
    : OfficeStyle(*new Office2013StylePrivate)
{
    QTN_D(Office2013Style)
    d.updateTheme();
}

/*!
Destructor of Office2013Style class.
*/
Office2013Style::~Office2013Style()
{
}

Office2013Style::Theme Office2013Style::theme() const
{
    QTN_D(const Office2013Style)
    return static_cast<Office2013Style::Theme>(d.m_themeType);
}

/*!
\property OfficeStyle::theme
Sets the \a theme of the office style. You can choose from White, Gray or Dark theme's families.
\sa Theme
*/
void Office2013Style::setTheme(Office2013Style::Theme theme)
{
    QTN_D(Office2013Style)
    if (d.m_themeType != theme)
    {
        d.beginUpdate();
        d.m_themeType = theme;
        d.updateTheme();
        d.endUpdate();
    }
}

/*! \reimp */
QRect Office2013Style::subControlRect(QStyle::ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
{
    if (control == CC_TitleBar && widget && widget->inherits("QMdiSubWindow"))
        return QCommonStyle::subControlRect(QStyle::CC_TitleBar, option, subControl, widget);

    QRect rect = OfficeStyle::subControlRect(control, option, subControl, widget);
    switch (control)
    {
        case CC_SpinBox:
            if (const QStyleOptionSpinBox* spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) 
            {
                int center = spinbox->rect.height() / 2;
                int fw = spinbox->frame ? 3 : 0; // Is drawn with 3 pixels width in drawComplexControl, independently from PM_SpinBoxFrameWidth
                int y = fw;
                const int buttonWidth = CommonStylePrivate::dpiScaled(14, widget);
                int x, lx, rx;
                x = spinbox->rect.width() - y - buttonWidth + 2;
                lx = fw;
                rx = x - fw;
                switch (subControl) 
                {
                    case SC_SpinBoxUp:
                        if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                            return QRect();
                        rect = QRect(x, fw, buttonWidth, center - fw);
                        break;
                    case SC_SpinBoxDown:
                        if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                            return QRect();
                        rect = QRect(x, center, buttonWidth, spinbox->rect.bottom() - center - fw + 1);
                        break;
                    case SC_SpinBoxEditField:
                        if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                            rect = QRect(lx, fw, spinbox->rect.width() - 2 * fw, spinbox->rect.height() - 2 * fw);
                        else
                            rect = QRect(lx, fw, rx - qMax(fw - 1, 0), spinbox->rect.height() - 2 * fw);
                        break;
                    case SC_SpinBoxFrame:
                        rect = spinbox->rect;
                        break;
                    default:
                        break;
                }
                rect = visualRect(spinbox->direction, spinbox->rect, rect);
            }
            break;
        case CC_Slider:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSliderSlider")))
            {
                rect = CommonStyle::subControlRect(control, option, subControl, widget);

                if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider *>(option))
                {
                    int tickSize = proxy()->pixelMetric(PM_SliderTickmarkOffset, option, widget);

                    switch (subControl)
                    {
                    case SC_SliderHandle:
                    {
                        if (slider->orientation == Qt::Horizontal)
                        {
                            rect.setHeight(proxy()->pixelMetric(PM_SliderThickness, option, widget));
                            rect.setWidth(proxy()->pixelMetric(PM_SliderLength, option, widget));
                            int centerY = slider->rect.center().y() - rect.height() / 2;
                            if (slider->tickPosition & QSlider::TicksAbove)
                                centerY += tickSize;
                            if (slider->tickPosition & QSlider::TicksBelow)
                                centerY -= tickSize;
                            rect.moveTop(centerY);
                        }
                    break;
                    }
                    case SC_SliderGroove:
                    {
                        QPoint grooveCenter = slider->rect.center();
                        #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
                        const int grooveThickness = CommonStylePrivate::dpiScaled(7, option);
                        #else
                        const int grooveThickness = CommonStylePrivate::dpiScaled(7, widget);
                        #endif 
                        if (slider->orientation == Qt::Horizontal)
                        {
                            rect.setHeight(grooveThickness);
                            if (slider->tickPosition & QSlider::TicksAbove)
                                grooveCenter.ry() += tickSize;
                            if (slider->tickPosition & QSlider::TicksBelow)
                                grooveCenter.ry() -= tickSize;
                        }
                    }
                    break;
                    default:
                    break;
                    }
                    rect = visualRect(slider->direction, slider->rect, rect);
                }
            }
            else if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option))
            {
                int tickSize = proxy()->pixelMetric(PM_SliderTickmarkOffset, option, widget);
                switch (subControl)
                {
                case SC_SliderHandle:
                {
                    if (slider->orientation == Qt::Horizontal)
                    {
                        rect.setHeight(proxy()->pixelMetric(PM_SliderThickness));
                        rect.setWidth(proxy()->pixelMetric(PM_SliderLength));
                        int centerY = slider->rect.center().y() - rect.height() / 2;
                        if (slider->tickPosition & QSlider::TicksAbove)
                            centerY += tickSize;
                        if (slider->tickPosition & QSlider::TicksBelow)
                            centerY -= tickSize;
                        rect.moveTop(centerY);
                    }
                    else
                    {
                        rect.setWidth(proxy()->pixelMetric(PM_SliderThickness));
                        rect.setHeight(proxy()->pixelMetric(PM_SliderLength));
                        int centerX = slider->rect.center().x() - rect.width() / 2;
                        if (slider->tickPosition & QSlider::TicksAbove)
                            centerX += tickSize;
                        if (slider->tickPosition & QSlider::TicksBelow)
                            centerX -= tickSize;
                        rect.moveLeft(centerX);
                    }
                }
                break;
                case SC_SliderGroove:
                {
                    QPoint grooveCenter = slider->rect.center();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
                    const int grooveThickness = CommonStylePrivate::dpiScaled(7, option);
#else
                    const int grooveThickness = CommonStylePrivate::dpiScaled(7, widget);
#endif 
                    if (slider->orientation == Qt::Horizontal)
                    {
                        rect.setHeight(grooveThickness);
                        if (slider->tickPosition & QSlider::TicksAbove)
                            grooveCenter.ry() += tickSize;
                        if (slider->tickPosition & QSlider::TicksBelow)
                            grooveCenter.ry() -= tickSize;
                    }
                    else
                    {
                        rect.setWidth(grooveThickness);
                        if (slider->tickPosition & QSlider::TicksAbove)
                            grooveCenter.rx() += tickSize;
                        if (slider->tickPosition & QSlider::TicksBelow)
                            grooveCenter.rx() -= tickSize;
                    }
                    rect.moveCenter(grooveCenter);
                    break;
                }
                default:
                    break;
                }
            }
            break;

        default:
            break;
    };
    return rect;
}

/*! \reimp */
int Office2013Style::styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const
{
#if 0
    if (widget && widget->inherits("QMdiSubWindow") && hint == QStyle::SH_WindowFrame_Mask)
        return CommonStyle::styleHint(hint, opt, widget, shret);
#endif
    switch (hint)
    {
        case SH_ToolTip_Mask:
            return 0;
        default:
            break;
    }
    return CommonStyle::styleHint(hint, opt, widget, shret);
}
