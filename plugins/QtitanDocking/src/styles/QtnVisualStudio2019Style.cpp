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

#include <QMenu>
#include <QScrollBar>
#include <QMdiArea>
#include <QComboBox>
#include <QListView>
#include <QTreeView>
#include <QPainterPath>
#include <qmath.h>
#include <qdrawutil.h>
#include "QtnStyleOption.h"
#include "QtnStyleHelperPrivate.h"
#include "QtnVisualStudio2019StylePrivate.h"
#include "QtnOfficeStyle.h"

QTITAN_USE_NAMESPACE

static const qreal g_roundedRadius = 4.0;

/* VisualStudio2019StylePrivate */
VisualStudio2019StylePrivate::VisualStudio2019StylePrivate()
    : m_themeType(VisualStudio2019Style::Blue)
{
}

VisualStudio2019StylePrivate::~VisualStudio2019StylePrivate()
{
}

QFont VisualStudio2019StylePrivate::getWidgetFont(const QString& className) const
{
    if (className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBar")) ||
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonStatusBar")) ||
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBackstageView")))
    {

        QFont font = QApplication::font("QMenuBar");
        font.setFamily(QStringLiteral("Segoe UI"));
        font.setPointSize(8);
        return font;
    }
    return CommonStylePrivate::getWidgetFont(className);
}

bool VisualStudio2019StylePrivate::drawHighDpiAreaDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const DockMarkerStyleOption* marker = qstyleoption_cast<const DockMarkerStyleOption*>(opt))
    {
        if (m_themeType == VisualStudio2019Style::Light || m_themeType == VisualStudio2019Style::Blue || m_themeType == VisualStudio2019Style::Dark)
        {
            painter->save();
            painter->translate(0.5, 0.5);
            painter->setRenderHint(QPainter::Antialiasing, true);

            QPainterPath path;
            const qreal radius = CommonStylePrivate::dpiScaled(1.0, widget);
            const QRectF markRect = opt->rect;
            const QPointF pntOffset = qtn_DPIScaledF(markRect.topLeft(), widget);
            const qreal value4 = CommonStylePrivate::dpiScaled(4.0, widget);
            const qreal value35 = CommonStylePrivate::dpiScaled(35.0, widget);
            QRectF rectArea(QPointF(value4, value4), QPointF(value35, value35));
            path.addRoundedRect(rectArea, radius, radius, Qt::AbsoluteSize);
            path.translate(pntOffset);
            if (m_themeType == VisualStudio2019Style::Blue)
            {
                rectArea.translate(pntOffset);
                const QColor clr = marker->palette.button().color();
                QLinearGradient linearGrad(rectArea.topLeft(), rectArea.bottomLeft());
                linearGrad.setColorAt(0, StyleHelper::colorMakeLighter(clr.rgb(), 0.5));
                linearGrad.setColorAt(1, clr);
                painter->fillPath(path, QBrush(linearGrad));
            }
            else
            {
                QColor shadowColor = opt->palette.light().color();
                shadowColor.setAlphaF(0.75);
                painter->fillPath(path, shadowColor);
            }

            painter->setPen(marker->palette.midlight().color());
            painter->drawPath(path);
            painter->restore();
            return true;
        }
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawHighDpiWindowClientDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const DockMarkerStyleOption* marker = qstyleoption_cast<const DockMarkerStyleOption*>(opt))
    {
        if (m_themeType == VisualStudio2019Style::Blue)
        {
            const CommonStyle::StandardPixmapEx sp = static_cast<CommonStyle::StandardPixmapEx>(marker->sp);
            QRectF markRect(opt->rect);
            QPointF pntOffset = qtn_DPIScaledF(markRect.topLeft(), widget);

            qreal value8 = CommonStylePrivate::dpiScaledF(8.0, widget);
            qreal value19 = CommonStylePrivate::dpiScaledF(19.0, widget);
            qreal value19_5 = CommonStylePrivate::dpiScaledF(19.5, widget);
            qreal value20 = CommonStylePrivate::dpiScaledF(20.0, widget);
            qreal value31 = CommonStylePrivate::dpiScaledF(31.0, widget);

            QRectF rect;
            if (sp == CommonStyle::SP_DockMarkerLeft || sp == CommonStyle::SP_DockMarkerBasisLeft || sp == CommonStyle::SP_DockMarkerCenterLeft)
                rect = QRectF(QPointF(value8, value8), QPointF(value19, value31));
            else if (sp == CommonStyle::SP_DockMarkerRight || sp == CommonStyle::SP_DockMarkerBasisRight || sp == CommonStyle::SP_DockMarkerCenterRight)
                rect = QRectF(QPointF(value20, value8), QPointF(value31, value31));
            else if (sp == CommonStyle::SP_DockMarkerTop || sp == CommonStyle::SP_DockMarkerBasisTop || sp == CommonStyle::SP_DockMarkerCenterTop)
                rect = QRectF(QPointF(value8, value8), QPointF(value31, value19));
            else if (sp == CommonStyle::SP_DockMarkerBottom || sp == CommonStyle::SP_DockMarkerBasisBottom || sp == CommonStyle::SP_DockMarkerCenterBottom)
                rect = QRectF(QPointF(value8, value20), QPointF(value31, value31));
            if (sp == CommonStyle::SP_DockMarkerCenter)
                rect = QRectF(QPointF(value8, value8), QPointF(value31, value31));
            else if (sp == CommonStyle::SP_DockMarkerCenterSplitLeft)
                rect = QRectF(QPointF(value8, value8), QPointF(value19_5, value31));
            else if (sp == CommonStyle::SP_DockMarkerCenterSplitRight)
                rect = QRectF(QPointF(value19_5, value8), QPointF(value31, value31));
            else if (sp == CommonStyle::SP_DockMarkerCenterSplitTop)
                rect = QRectF(QPointF(value8, value8), QPointF(value31, value19_5));
            else if (sp == CommonStyle::SP_DockMarkerCenterSplitBottom)
                rect = QRectF(QPointF(value8, value19_5), QPointF(value31, value31));

            QPainterPath pathWnd;
            pathWnd.addRect(rect);
            pathWnd.translate(pntOffset);
            rect.translate(pntOffset);

            QLinearGradient linearGrad(rect.topLeft(), rect.bottomLeft());
            linearGrad.setColorAt(0, StyleHelper::colorMakeLighter(marker->palette.shadow().color().rgb(), 0.5));
            linearGrad.setColorAt(1, marker->palette.shadow().color());

            painter->fillPath(pathWnd, QBrush(linearGrad));
            return true;
        }
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    painter->save();
    painter->translate(opt->rect.x(), opt->rect.y());
    if (opt->state & QStyle::State_Horizontal) 
    {
        int x = opt->rect.width() / 3;
        if (opt->direction == Qt::RightToLeft)
            x -= 2;
        if (opt->rect.height() > 4) 
        {
            QPen pen(QBrush(opt->palette.dark().color(), Qt::Dense6Pattern), 6);
            painter->setPen(pen);
            painter->drawLine(QPoint(x, 4), QPoint(x, opt->rect.height() - 4));
        }
    }
    else if (opt->rect.width() > 4)
    {
            int y = opt->rect.height() / 3;
            QPen pen(QBrush(opt->palette.dark().color(), Qt::Dense6Pattern), 6);
            painter->setPen(pen);
            painter->drawLine(QPoint(4, y), QPoint(opt->rect.width() - 4, 3));
    }
    painter->restore();

    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const VisualStudio2019Style);
    if (qobject_cast<const QMenu*>(w) /*|| qobject_cast<const QScrollBar*>(w)*/)
        return false;
    if (option->rect.width() <= 1 || option->rect.height() <= 1)
        return true;

    QPixmap arrow;
    switch (pe)
    {
        case QStyle::PE_IndicatorArrowUp:
            arrow = p.proxy()->standardPixmap(QStyle::SP_ArrowUp, option, w);
            break;
        case QStyle::PE_IndicatorArrowDown:
            arrow = p.proxy()->standardPixmap(QStyle::SP_ArrowDown, option, w);
            break;
        case QStyle::PE_IndicatorArrowRight:
            arrow = p.proxy()->standardPixmap(QStyle::SP_ArrowRight, option, w);
            break;
        case QStyle::PE_IndicatorArrowLeft:
            arrow = p.proxy()->standardPixmap(QStyle::SP_ArrowLeft, option, w);
            break;
        default:
            break;
    }
    if (arrow.isNull())
        return false;

    int size = qMin(option->rect.width(), option->rect.height());
    QSize iconSize = QSize(size, size);
    QSize pmSize = arrow.size();
    if (pmSize.width() * pmSize.height() < iconSize.width() * iconSize.height())
        arrow = arrow.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    p.proxy()->drawItemPixmap(painter, option->rect, Qt::AlignCenter, arrow);
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawIndicatorTabClose(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
    if (((opt->state & QStyle::State_Selected) == 0) && widget)
    {
        if (const QTabBar* tb = qobject_cast<const QTabBar*>(widget->parent()))
        {
            int index = tb->tabAt(tb->mapFromGlobal(QCursor::pos()));
            QTabBar::ButtonPosition position = static_cast<QTabBar::ButtonPosition>(p.proxy()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, Q_NULL, tb));
            if (tb->tabButton(index, position) != widget)
                return true;
        }
    }

    int size = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize);
    QIcon::Mode mode = opt->state & QStyle::State_Enabled ? (opt->state & QStyle::State_Raised ? QIcon::Active : QIcon::Normal) : QIcon::Disabled;
    if (!(opt->state & QStyle::State_Raised) && !(opt->state & QStyle::State_Sunken) && !(opt->state & QStyle::State_Selected))
        mode = QIcon::Disabled;

    QIcon::State state = opt->state & QStyle::State_Sunken ? QIcon::On : QIcon::Off;
    QIcon icon = p.proxy()->standardPixmap(QStyle::SP_TitleBarCloseButton, opt, widget);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QPixmap pixmap = icon.pixmap(QtnPrivate::qtn_getWindow(widget), QSize(size, size), mode, state);
#else
    QPixmap pixmap = icon.pixmap(QSize(size, size), mode, state);
#endif
    p.proxy()->drawItemPixmap(painter, opt->rect, Qt::AlignCenter, pixmap);
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits("QStatusBar"))
    {
        QPoint p1, p2;
        if (opt->state & QStyle::State_Horizontal)
        {
            p1 = QPoint(opt->rect.width() / 2, 0);
            p2 = QPoint(p1.x(), opt->rect.height());
        }
        else
        {
            p1 = QPoint(0, opt->rect.height() / 2);
            p2 = QPoint(opt->rect.width(), p1.y());
        }
        painter->setPen(QPen(opt->palette.dark().color()));
        painter->drawLine(p1, p2);
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawIndicatorCheckBox(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    if (const QStyleOptionButton* checkbox = qstyleoption_cast<const QStyleOptionButton*>(option))
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->translate(0.5, 0.5);

        QRect rect = option->rect.adjusted(0, 0, -1, -1);
        QColor clrAccentText = option->palette.color(QPalette::Text);
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
bool VisualStudio2019StylePrivate::drawIndicatorRadioButton(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
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

    QColor clrAccentText = option->palette.color(QPalette::Text);
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
bool VisualStudio2019StylePrivate::drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
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
bool VisualStudio2019StylePrivate::drawIndicatorItemCheck(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (const QStyleOptionViewItem* itemViewOpt = qstyleoption_cast<const QStyleOptionViewItem *>(opt))
    {
        painter->save();
        painter->translate(0.5, 0.5);

        painter->setPen(itemViewOpt && itemViewOpt->showDecorationSelected && opt->state & QStyle::State_Selected ?
            opt->palette.highlightedText().color() : opt->palette.text().color());
        if (opt->state & QStyle::State_NoChange)
            painter->setBrush(opt->palette.brush(QPalette::Button));
        qreal size = CommonStylePrivate::dpiScaled(11.0, w);
        QRect rc = QRect(QPoint(opt->rect.x() + qRound(qreal(opt->rect.height()) * 0.5 - size * 0.5),
            opt->rect.y() + qRound(qreal(opt->rect.width()) * 0.5 - size * 0.5)), QSize(qRound(size), qRound(size)));
        painter->drawRect(rc);

        if (!(opt->state & QStyle::State_Off))
        {
            painter->setRenderHint(QPainter::Antialiasing, true);
            const qreal checkMarkPadding = 1 + rc.width() * 0.13; // at least one pixel padding
            QPainterPath path;
            const qreal rectHeight = rc.height(); // assuming height equals width
            path.moveTo(checkMarkPadding + rectHeight * 0.11, rectHeight * 0.47);
            path.lineTo(rectHeight * 0.5, rectHeight - checkMarkPadding);
            path.lineTo(rectHeight - checkMarkPadding, checkMarkPadding);
            painter->setPen(QPen(opt->palette.text().color(), 1));
            painter->drawPath(path.translated(rc.topLeft() - QPoint(1, 0)));
        }
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawPanelItemViewItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QStyleOptionViewItem* vopt = qstyleoption_cast<const QStyleOptionViewItem*>(opt))
#else
    if (const QStyleOptionViewItemV4* vopt = qstyleoption_cast<const QStyleOptionViewItemV4*>(opt))
#endif
    {
        QPalette::ColorGroup cg = (widget ? widget->isEnabled() : (vopt->state & QStyle::State_Enabled)) ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
            cg = QPalette::Inactive;

        if (vopt->showDecorationSelected && (vopt->state & QStyle::State_Selected))
        {
            painter->fillRect(vopt->rect, vopt->palette.brush(cg, QPalette::Highlight));
        }
        else
        {
            if (vopt->backgroundBrush.style() != Qt::NoBrush)
            {
                QPointF oldBO = painter->brushOrigin();
                painter->setBrushOrigin(vopt->rect.topLeft());
                painter->fillRect(vopt->rect, vopt->backgroundBrush);
                painter->setBrushOrigin(oldBO);
            }

            if (vopt->state & QStyle::State_Selected || vopt->state & QStyle::State_MouseOver)
            {
                QRect textRect = p.proxy()->subElementRect(QStyle::SE_ItemViewItemText, opt, widget);
                QColor clr = vopt->palette.brush(cg, QPalette::Highlight).color();
                if (vopt->state & QStyle::State_Selected)
                    clr = StyleHelper::colorMakeDarker(clr.rgba(), 0.09);
                painter->fillRect(textRect, clr);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawFrameDockWidget(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (widget && (widget->inherits(QTITAN_META_CLASSNAME("DockTabbedLayoutPanel")) || widget->inherits(QTITAN_META_CLASSNAME("DockBarSite"))))
    {
        if (const QStyleOptionFrame* frame = qstyleoption_cast<const QStyleOptionFrame *>(option))
        {
            if (widget->inherits(QTITAN_META_CLASSNAME("DockBarSite")))
            {
                if (frame->rect.width() == 1)
                    return true;
                if (frame->rect.height() == 1)
                    return true;
            }
            painter->fillRect(frame->rect, option->palette.window());
            return true;
        }
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawFrameTabBarBase(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (w && w->inherits(QTITAN_META_CLASSNAME("DockDocumentAreaTabBar")))
    {
        if (const QStyleOptionTabBarBase* tbb = qstyleoption_cast<const QStyleOptionTabBarBase *>(opt)) 
        {
            painter->save();
            switch (tbb->shape) 
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                    painter->setPen(QPen(tbb->palette.light(), 4));
                    painter->drawLine(tbb->rect.topLeft(), tbb->rect.topRight());
                    break;
                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                    painter->setPen(QPen(tbb->palette.light(), 4));
                    painter->drawLine(tbb->rect.topLeft(), tbb->rect.bottomLeft());
                    break;
                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                    painter->setPen(QPen(tbb->palette.shadow(), 4));
                    painter->drawLine(tbb->rect.left(), tbb->rect.bottom(), tbb->rect.right(), tbb->rect.bottom());
                    painter->setPen(QPen(tbb->palette.dark(), 4));
                    painter->drawLine(tbb->rect.left(), tbb->rect.bottom() - 1, tbb->rect.right() - 1, tbb->rect.bottom() - 1);
                    break;
                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                    painter->setPen(QPen(tbb->palette.dark(), 4));
                    painter->drawLine(tbb->rect.topRight(), tbb->rect.bottomRight());
                    break;
            }
            painter->restore();
            return true;
        }
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawFrameGroupBox(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QStyleOptionFrame* frame = qstyleoption_cast<const QStyleOptionFrame*>(opt)) 
#else
    if (const QStyleOptionFrameV3* frame = qstyleoption_cast<const QStyleOptionFrameV3*>(opt))
#endif
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (frame->features & QStyleOptionFrame::Flat)
#else
        if (frame->features & QStyleOptionFrameV3::Flat)
#endif
        {
            QRect fr = frame->rect;
            QPoint p1(fr.x(), fr.y() + 1);
            QPoint p2(fr.x() + fr.width(), p1.y());
            qDrawShadeLine(painter, p1, p2, frame->palette, true, 0, frame->lineWidth);
        }
        else 
            qDrawShadeRect(painter, frame->rect.x(), frame->rect.y(), frame->rect.width(), frame->rect.height(), frame->palette, true, 0, frame->lineWidth);
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawFrameMenu(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits("QMenu"))
    {
        painter->setPen(QPen(opt->palette.light().color()));
        painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawPanelMenu(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits("QMenu"))
    {
        painter->save();
        const QBrush menuBackground = opt->palette.window().color();
        QColor borderColor = opt->palette.light().color();
        qDrawPlainRect(painter, opt->rect, borderColor, 1, &menuBackground);
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(opt)
    Q_UNUSED(painter)
    if (w && w->inherits(QTITAN_META_CLASSNAME("DockWidgetTitleButton")))
    {
        if (const QStyleOptionToolButton* option = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
        {
            const bool highlighted = option->state & QStyle::State_MouseOver;
            const bool pressed = opt->state & QStyle::State_Sunken;
            if (highlighted)
                painter->fillRect(opt->rect, opt->palette.highlight());
            else if (highlighted && pressed)
                painter->fillRect(opt->rect, opt->palette.shadow());
            return true;
        }
    }
    else if (w && w->inherits("QDockWidgetTitleButton"))
        return true;

    if (w && w->inherits("QMdiSubWindow"))
    {
        const bool highlighted = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;
        if (highlighted && !pressed)
        {
            QColor frameColor = colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiTitleBar"), QPalette::Active, QPalette::Highlight);
            QColor frameBorder = colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiTitleBar"), QPalette::Active, QPalette::Dark);
            painter->fillRect(opt->rect, frameColor);
            QtnPen pen(painter, frameBorder);
            painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        }
        else if (highlighted && pressed)
        {
            QColor frameColor = colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiTitleBar"), QPalette::Active, QPalette::Light);
            QColor frameBorder = colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiTitleBar"), QPalette::Active, QPalette::Dark);
            painter->fillRect(opt->rect, frameColor);
            QtnPen pen(painter, frameBorder);
            painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        }
        return true;
    }
    else if (w && w->inherits("QToolButton"))
    {
        if (w->parentWidget() && (QtnPrivate::qtn_isAncestor(w, "QStatusBar") || w->parentWidget()->inherits("QToolBar") || w->parentWidget()->inherits(QTITAN_META_CLASSNAME("DockToolBar"))))
        {
            if (opt->state & QStyle::State_Enabled)
            {
                if (opt->state & QStyle::State_Sunken)
                    painter->fillRect(opt->rect, opt->palette.light());
                else
                    painter->fillRect(opt->rect, opt->palette.highlight());
                QtnPen pen(painter, opt->palette.dark().color());
                painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
                return true;
            }
        }
    }
    return CommonStylePrivate::drawPanelButtonTool(opt, painter, w);
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawPanelStatusBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    painter->fillRect(opt->rect, opt->palette.window());
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawFrameLineEdit(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
    {
        const bool enabled = panel->state & QStyle::State_Enabled;
        const bool highlighted = panel->state & QStyle::State_MouseOver;

        painter->save();
        QRegion clipRegion = opt->rect;
        clipRegion -= panel->rect.adjusted(2, 2, -2, -2);
        painter->setClipRegion(clipRegion);

        QColor colorBorder = panel->palette.midlight().color();
        if (enabled && highlighted)
            colorBorder = panel->palette.mid().color();

        QtnPen pen(painter, colorBorder);
        qtn_drawRect(painter, panel->rect);
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawPanelButtonCommand(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(opt))
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool checked = opt->state & QStyle::State_On;
        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;

        if (enabled)
        {
            if (pressed)
            {
                if (!checked)
                    painter->fillRect(opt->rect, opt->palette.color(QPalette::Highlight).lighter(80));
            }
            else if (selected || checked || (opt->state & QStyle::State_HasFocus))
                painter->fillRect(opt->rect, opt->palette.color(QPalette::Highlight));
            else
                painter->fillRect(opt->rect, opt->palette.color(QPalette::Button));
        }
        else
            painter->fillRect(opt->rect, opt->palette.color(QPalette::Button));

        QtnPen pen(painter, opt->palette.color(QPalette::Dark));
        painter->drawRect(btn->rect.adjusted(0, 0, -1, -1));
        return true;
    }
    return false;
}

QWidget* VisualStudio2019StylePrivate::qtn_findRibbonBar(const QWidget* parent)
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
bool VisualStudio2019StylePrivate::qtn_isBackstageVisible(const QWidget* widget)
{
    const QWidget* wd = Q_NULL;
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
        wd = widget;
    if (wd == Q_NULL)
        wd = qtn_findRibbonBar(widget);
    return wd && wd->property("backstageVisible").toBool();
}

// draw ComplexControl
/*! \internal */
bool VisualStudio2019StylePrivate::drawTitleBar(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockWindow")))
        return CommonStylePrivate::drawTitleBar(option, painter, widget);

    if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
    {
        QRect ir;
        const bool isActive = tb->state & QStyle::State_Active;

        const bool mdi = widget && widget->inherits("QMdiSubWindow");

        if (option->subControls & QStyle::SC_TitleBarLabel)
        {
            QColor frameColor = titleBarFrameColor(option->palette, isActive);
            QColor windowColor = option->palette.color(QPalette::Button);
            if (!windowColor.isValid())
                windowColor = widget ? widget->palette().color(QPalette::Window) : option->palette.color(QPalette::Window);

            if (mdi)
            {
                frameColor = colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiTitleBar"), QPalette::Active, QPalette::Button);
                windowColor = frameColor;
            }

            drawfillTitleBar(option, painter, widget, frameColor, windowColor);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarLabel, widget);
            if (mdi)
            {
                QColor clrText = isActive ? colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiTitleBar"), QPalette::Active, QPalette::Text) : 
                    colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiTitleBar"), QPalette::Disabled, QPalette::Text);
                painter->setPen(clrText);
            }
            else
                painter->setPen(isActive ? option->palette.text().color() : option->palette.color(QPalette::Disabled, QPalette::Text));

            painter->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, tb->text);
        }

        static qreal scaleFactor = g_roundedRadius / 1.2;
        bool down = false;
        bool highlight = false;
        QPixmap pm;
        QPalette savePalette = tb->palette;
        QStyleOptionTitleBar tool = *tb;
        if (tb->subControls & QStyle::SC_TitleBarCloseButton && tb->titleBarFlags & Qt::WindowSystemMenuHint)
        {
            QColor clrText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"), QPalette::Active, QPalette::WindowText).color();
            QColor clrHighlightedText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"), QPalette::Active, QPalette::HighlightedText).color();
            QColor clrButton = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarCloseButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarCloseButton) && (option->state & QStyle::State_Sunken);
            highlight = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / scaleFactor);
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
            QColor clrText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMaxButton"), QPalette::Active, QPalette::WindowText).color();
            QColor clrHighlightedText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMaxButton"), QPalette::Active, QPalette::HighlightedText).color();
            QColor clrButton = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMaxButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMaxButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarMaxButton) && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarMaxButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / scaleFactor);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarMaxButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        if (tb->subControls & QStyle::SC_TitleBarMinButton && tb->titleBarFlags & Qt::WindowMinimizeButtonHint && !(tb->titleBarState & Qt::WindowMinimized))
        {
            QColor clrText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMinButton"), QPalette::Active, QPalette::WindowText).color();
            QColor clrHighlightedText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMinButton"), QPalette::Active, QPalette::HighlightedText).color();
            QColor clrButton = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarMinButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMinButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarMinButton) && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarMinButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / scaleFactor);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarMinButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        bool drawNormalButton = (tb->subControls & QStyle::SC_TitleBarNormalButton) && (((tb->titleBarFlags & Qt::WindowMinimizeButtonHint) &&
            (tb->titleBarState & Qt::WindowMinimized)) || ((tb->titleBarFlags & Qt::WindowMaximizeButtonHint) && (tb->titleBarState & Qt::WindowMaximized)));
        if (drawNormalButton)
        {
            QColor clrText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarNormalButton"), QPalette::Active, QPalette::WindowText).color();
            QColor clrHighlightedText = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarNormalButton"), QPalette::Active, QPalette::HighlightedText).color();
            QColor clrButton = brushSubControl(mdi ? QStringLiteral("QMdiSubWindow") : QStringLiteral("QMainWindow"), QStringLiteral("TitleBarNormalButton"), QPalette::Active, QPalette::Highlight).color();
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, clrButton);

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarNormalButton, widget);
            down = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarNormalButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = qRound(tool.rect.height() / scaleFactor);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::WindowText, clrText);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPalette_setColor(tool.palette, QPalette::Inactive, QPalette::HighlightedText, clrHighlightedText.isValid() ? clrHighlightedText : clrText);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarNormalButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }
        if (tb->subControls & QStyle::SC_TitleBarShadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint && !(tb->titleBarState & Qt::WindowMinimized))
        {
            QColor clrText = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarShadeButton"), QPalette::Active, QPalette::WindowText).color();
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

            int margin = qRound(tool.rect.height() / scaleFactor);
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

            int margin = qRound(tool.rect.height() / scaleFactor);
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
bool VisualStudio2019StylePrivate::drawScrollBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
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
            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarAddLine, &newScrollbar, painter, widget);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSlider)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSlider, widget);
            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarSlider, &newScrollbar, painter, widget);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawComboBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const VisualStudio2019Style);
    if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt))
    {
        QStyle::State flags = cmb->state;
        QStyle::SubControls sub = cmb->subControls;

        const bool enabled = flags & QStyle::State_Enabled;
        const bool highlighted = flags & QStyle::State_MouseOver;
//        const bool selected = flags & QStyle::State_Selected;
        const bool dropped = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && ((flags & QStyle::State_On) || (flags & QStyle::State_Sunken));
//        const bool dropButtonHot = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && (flags & QStyle::State_MouseOver);
//        const bool hasFocus = flags & QStyle::State_HasFocus;

        if (cmb->frame)
        {
            QRect r = cmb->rect.adjusted(0, 0, -1, -1);
            painter->fillRect(r, cmb->palette.brush(QPalette::Base));
            QColor colorBorder = cmb->palette.midlight().color();
            if (enabled && highlighted)
                colorBorder = cmb->palette.mid().color();

            QtnPen pen(painter, colorBorder);
            qtn_drawRect(painter, cmb->rect);
        }

        if (sub & QStyle::SC_ComboBoxArrow)
        {
            QRect rcBtn = p.proxy()->subControlRect(QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxArrow, w);
            if (enabled && highlighted)
            {
                QColor clrFill = cmb->palette.light().color();
                if (dropped)
                    clrFill = cmb->palette.dark().color();
//                else if (dropButtonHot || hasFocus)
//                    clrFill = buttonHighlightColor(cmb->palette, false/*highlightDn*/, false/*border*/);
                painter->fillRect(rcBtn.adjusted(-1, -1, 1, 1), clrFill);

                QColor colorBorder = cmb->palette.midlight().color();
                if (enabled && highlighted)
                    colorBorder = cmb->palette.mid().color();

                QtnPen pen(painter, colorBorder);
                qtn_drawRect(painter, cmb->rect);
            }

            QStyleOption optArray = *opt;
/*
            int margin = dpiScaled(2, w);
            rcBtn.adjust(margin, margin, -margin, -margin);
            int size = qMax(rcBtn.width(), rcBtn.height());
            rcBtn.setSize(QSize(size, size));
*/
            optArray.rect = rcBtn;
            drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, &optArray, painter, w);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawToolButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    Q_UNUSED(painter)
    Q_UNUSED(opt)
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawSpinBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
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
                    colorBorder = sb->palette.light().color();
                else if (highlighted || hasFocus)
                    colorBorder = sb->palette.light().color();

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
            if (press && hasFocus)
                clrFill = StyleHelper::colorMakeDarker(sb->palette.highlight().color().rgb(), 0.14);
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_MouseOver))
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
            if (press && hasFocus)
                clrFill = StyleHelper::colorMakeDarker(sb->palette.highlight().color().rgb(), 0.14);
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_MouseOver))
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
bool VisualStudio2019StylePrivate::drawRibbonTabShapeLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const VisualStudio2019Style);
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
            textColor = opt->palette.highlight().color();
//            textColor = textRibbonTabBarColor(opt);

//        if (m_isActiveTabAccented && selected)
//            textColor = opt->palette.color(QPalette::Window);

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
bool VisualStudio2019StylePrivate::drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const RibbonOptionHeaderStyleOption* optTab = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        QRect rectTab = optTab->rect;
        if ((optTab->state & QStyle::State_Selected))
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

            QRect rcFirst(optTab->rcFirst);
            QRect rcLast(optTab->rcLast);

            QRect rc(rcFirst.left(), opt->rect.top(), rcLast.right(), rcLast.bottom());
            painter->fillRect(rc, clrContextTab);
        }
    }
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawMenuBarItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
    if (const QStyleOptionMenuItem* mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
    {
        const bool dis = !(mbi->state & QStyle::State_Enabled);
        const bool highlight = !dis && (opt->state & QStyle::State_Selected);
        if (highlight)
        {
            painter->fillRect(opt->rect, opt->palette.highlight().color());
            QtnPen pen(painter, opt->palette.dark().color());
            painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        }

        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, mbi, widget))
            alignment |= Qt::TextHideMnemonic;

        int iconExtent = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QPixmap pix = mbi->icon.pixmap(QtnPrivate::qtn_getWindow(widget), QSize(iconExtent, iconExtent), (mbi->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled);
#else
        QPixmap pix = mbi->icon.pixmap(QSize(iconExtent, iconExtent), (mbi->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled);
#endif
        if (!pix.isNull())
            p.proxy()->drawItemPixmap(painter, mbi->rect, alignment, pix);
        else
        {
            QString text = mbi->text;
            if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), opt, widget))
                text = text.toUpper();
            p.proxy()->drawItemText(painter, mbi->rect, alignment, mbi->palette, mbi->state & QStyle::State_Enabled, text, QPalette::ButtonText);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawMenuItem(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
    painter->save();
    QColor highlightedOutline = option->palette.highlight().color();
    QColor shadow = option->palette.dark().color();

    // Draws one item in a popup menu.
    if (const QStyleOptionMenuItem* menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option))
    {
        QColor highlightOutline = highlightedOutline;
        QColor highlight = option->palette.highlight().color();
        if (menuItem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            int w = 0;
/*
            if (!menuItem->text.isEmpty())
            {
                painter->setFont(menuItem->font);
                p.proxy()->drawItemText(painter, menuItem->rect.adjusted(5, 0, -5, 0), Qt::AlignLeft | Qt::AlignVCenter,
                    menuItem->palette, menuItem->state & QStyle::State_Enabled, menuItem->text,
                    QPalette::Text);
                w = qtn_horizontalAdvanceFont(menuItem->fontMetrics, menuItem->text) + 5;
            }
*/
            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = p.proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, option, widget);
            painter->setPen(shadow);
            bool reverse = menuItem->direction == Qt::RightToLeft;
            painter->drawLine(menuItem->rect.left() + 5 + (reverse ? 0 : w) + iconSize, menuItem->rect.center().y(),
                menuItem->rect.right() - 5 - (reverse ? w : 0), menuItem->rect.center().y());
            painter->restore();
            return true;
        }
        bool selected = menuItem->state & QStyle::State_Selected && menuItem->state & QStyle::State_Enabled;
        if (selected)
        {
            QRect r = option->rect;
            painter->fillRect(r, highlight);
            painter->setPen(QPen(highlightOutline));
            painter->drawRect(QRectF(r).adjusted(0.5, 0.5, -0.5, -0.5));
        }
        const bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
        const bool checked = menuItem->checked;
        const bool sunken = menuItem->state & QStyle::State_Sunken;
        const bool enabled = menuItem->state & QStyle::State_Enabled;

        bool ignoreCheckMark = false;
        int checkcol = qMax(menuItem->maxIconWidth, 20);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (qobject_cast<const QComboBox*>(widget) || (option->styleObject && option->styleObject->property("_q_isComboBoxPopupItem").toBool()))
            ignoreCheckMark = true; //ignore the checkmarks provided by the QComboMenuDelegate
#endif
        if (!ignoreCheckMark)
        {
            // Check
            QRect checkRect(option->rect.left() + 7, option->rect.center().y() - 6, 14, 14);
            checkRect = p.visualRect(menuItem->direction, menuItem->rect, checkRect);
            if (checkable)
            {
                if (menuItem->checkType & QStyleOptionMenuItem::Exclusive)
                {
                    // Radio button
                    if (checked || sunken)
                    {
                        painter->setRenderHint(QPainter::Antialiasing);
                        painter->setPen(Qt::NoPen);

                        painter->fillRect(checkRect.adjusted(-1, -1, 1, 1), option->palette.shadow());
                        QPalette::ColorRole textRole = !enabled ? QPalette::Text :
                            selected ? QPalette::HighlightedText : QPalette::ButtonText;
                        painter->setBrush(option->palette.brush(option->palette.currentColorGroup(), textRole));
                        painter->drawEllipse(checkRect.adjusted(4, 4, -4, -4));
                    }
                }
                else if (menuItem->icon.isNull() && checked) // Check box
                {
                    QStyleOptionButton box;
                    box.QStyleOption::operator=(*option);
                    box.rect = checkRect;
                    box.state |= QStyle::State_On;
                    painter->fillRect(box.rect.adjusted(-1, -1, 1, 1), option->palette.shadow());
                    p.proxy()->drawPrimitive(QStyle::PE_IndicatorMenuCheckMark, &box, painter, widget);
                }
            }
        }
        else
        { //ignore checkmark
            if (menuItem->icon.isNull())
                checkcol = 0;
            else
                checkcol = menuItem->maxIconWidth;
        }

        // Text and icon, ripped from windows style
        bool dis = !(menuItem->state & QStyle::State_Enabled);
        bool act = menuItem->state & QStyle::State_Selected;
        const QStyleOption* opt = option;
        const QStyleOptionMenuItem* menuitem = menuItem;

        QPainter* painter2 = painter;
        QRect vCheckRect = p.visualRect(opt->direction, menuitem->rect,
            QRect(menuitem->rect.x() + 4, menuitem->rect.y(), checkcol, menuitem->rect.height()));

        if (!menuItem->icon.isNull())
        {
            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            if (act && !dis)
                mode = QIcon::Active;
            int smallIconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, option, widget);
            QSize iconSize(smallIconSize, smallIconSize);
            if (const QComboBox* combo = qobject_cast<const QComboBox*>(widget))
                iconSize = combo->iconSize();

            QPixmap pixmap = preparePixmap(menuitem->icon, iconSize, mode, checked ? QIcon::On : QIcon::Off, widget);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            double devicePixelRatio = pixmap.devicePixelRatio();
#else
            double devicePixelRatio = 1.0;
#endif
            const int pixw = static_cast<int>(pixmap.width() / devicePixelRatio);
            const int pixh = static_cast<int>(pixmap.height() / devicePixelRatio);

            QRect pmr(0, 0, pixw, pixh);
            pmr.moveCenter(vCheckRect.center());
            painter->setPen(menuItem->palette.text().color());
            if (!ignoreCheckMark && checkable && checked)
            {
                QStyleOption opt = *option;
                if (act)
                {
//                    QColor activeColor = mergedColors(option->palette.window().color(), option->palette.highlight().color());
//                    QPalette_setBrush(opt.palette, QPalette::Button, activeColor);
                }
                opt.state |= QStyle::State_Sunken;
                opt.rect = vCheckRect;
                p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, painter, widget);
            }
            painter->drawPixmap(pmr.topLeft(), pixmap);
        }
        if (selected)
            painter->setPen(menuItem->palette.highlightedText().color());
        else
            painter->setPen(menuItem->palette.text().color());

        int x, y, w, h;
        menuitem->rect.getRect(&x, &y, &w, &h);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        int tab = 0;
#else
        int tab = menuitem->tabWidth;
#endif
        QColor discol;
        if (dis)
        {
            discol = menuitem->palette.text().color();
            painter2->setPen(discol);
        }
        int xm = windowsItemFrame + checkcol + windowsItemHMargin + 2;
        int xpos = menuitem->rect.x() + xm;

        QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
        QRect vTextRect = p.visualRect(opt->direction, menuitem->rect, textRect);
        QString s(menuitem->text);
        if (!s.isEmpty())  // draw text
        {
            painter2->save();
            int t = s.indexOf(QLatin1Char('\t'));
            int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
            if (!p.styleHint(QStyle::SH_UnderlineShortcut, menuitem, widget))
                text_flags |= Qt::TextHideMnemonic;
            text_flags |= Qt::AlignLeft;
            if (t >= 0)
            {
                QRect vShortcutRect = p.visualRect(opt->direction, menuitem->rect,
                    QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
                const QString textToDraw = s.mid(t + 1);
                if (dis && !act && p.proxy()->styleHint(QStyle::SH_EtchDisabledText, option, widget))
                {
                    painter2->setPen(menuitem->palette.light().color());
                    painter2->drawText(vShortcutRect.adjusted(1, 1, 1, 1), text_flags, textToDraw);
                    painter2->setPen(discol);
                }
                painter2->drawText(vShortcutRect, text_flags, textToDraw);
                s = s.left(t);
            }
            QFont font = menuitem->font;
            // font may not have any "hard" flags set. We override
            // the point size so that when it is resolved against the device, this font will win.
            // This is mainly to handle cases where someone sets the font on the window
            // and then the combo inherits it and passes it onward. At that point the resolve mask
            // is very, very weak. This makes it stonger.
            font.setPointSizeF(QFontInfo(menuItem->font).pointSizeF());

            if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                font.setBold(true);

            painter2->setFont(font);
            const QString textToDraw = s.left(t);
            if (dis && !act && p.proxy()->styleHint(QStyle::SH_EtchDisabledText, option, widget))
            {
                painter2->setPen(menuitem->palette.light().color());
                painter2->drawText(vTextRect.adjusted(1, 1, 1, 1), text_flags, textToDraw);
                painter2->setPen(discol);
            }
            painter2->drawText(vTextRect, text_flags, textToDraw);
            painter2->restore();
        }

        // Arrow
        // draw sub menu arrow
        if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            int dim = (menuItem->rect.height() - 4) / 2;
            QStyle::PrimitiveElement arrow;
            arrow = option->direction == Qt::RightToLeft ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
            int xpos = menuItem->rect.left() + menuItem->rect.width() - 3 - dim;
            QRect  vSubMenuRect = p.visualRect(option->direction, menuItem->rect,
                QRect(xpos, menuItem->rect.top() + menuItem->rect.height() / 2 - dim / 2, dim, dim));
            QStyleOptionMenuItem newMI = *menuItem;
            newMI.rect = vSubMenuRect;
            newMI.state = !enabled ? QStyle::State_None : QStyle::State_Enabled;
            if (selected)
                QPalette_setColor(newMI.palette, QPalette::WindowText, newMI.palette.highlightedText().color());
            p.proxy()->drawPrimitive(arrow, &newMI, painter, widget);
        }
    }
    painter->restore();
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawSplitter(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockPanelSplitter")))
    {
        p->fillRect(opt->rect, opt->palette.window());
        return true;
    }
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawToolBar(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const QStyleOptionToolBar* toolBar = qstyleoption_cast<const QStyleOptionToolBar *>(opt))
    {
        if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockToolBar")))
        {
            Q_UNUSED(toolBar)
            Q_UNUSED(painter)
            Q_UNUSED(widget)
            return true;
        }
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QPoint pt(opt->rect.right() - 4, opt->rect.bottom() - 4);
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3 - y; x++)
            painter->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), opt->palette.dark().color());
    }
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawMenuBarEmptyArea(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits("QMenuBar") && !widget->testAttribute(Qt::WA_NoSystemBackground))
    {
        painter->eraseRect(opt->rect);
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawShapedFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(opt)
    Q_UNUSED(painter)
    if (w && w->inherits(QTITAN_META_CLASSNAME("DockWidgetPanel")))
        return true;
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawTabBarTabLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab *>(opt))
#else
    if (const QStyleOptionTabV3* tab = qstyleoption_cast<const QStyleOptionTabV3 *>(opt))
#endif
    {
        QRect tr = tab->rect;
        bool verticalTabs = tab->shape == QTabBar::RoundedEast      || 
                            tab->shape == QTabBar::RoundedWest      || 
                            tab->shape == QTabBar::TriangularEast   || 
                            tab->shape == QTabBar::TriangularWest;
        const bool selected = tab->state & QStyle::State_Selected;
        const bool highlighted = tab->state & QStyle::State_MouseOver;
        const int stripTab = CommonStylePrivate::dpiScaled(g_roundedRadius, widget);
        const bool autoHideBar = widget && widget->inherits(QTITAN_META_CLASSNAME("TabWidgetAutoHideBar"));

        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
            alignment |= Qt::TextHideMnemonic;

        if (verticalTabs) 
        {
            painter->save();
            int newX, newY, newRot;
            if (tab->shape == QTabBar::RoundedEast || tab->shape == QTabBar::TriangularEast) 
            {
                if (autoHideBar)
                    newX = tr.width() + tr.x() + stripTab / 2;
                else
                    newX = tr.width() + tr.x();
                newY = tr.y();
                newRot = 90;
            }
            else
            {
                if (autoHideBar)
                {
                    newX = tr.width() + tr.x() - stripTab / 2;
                    newY = tr.y();
                    newRot = 90;
                }
                else
                {
                    newX = tr.x();
                    newY = tr.y() + tr.height();
                    newRot = -90;
                }
            }
            QTransform m = QTransform::fromTranslate(newX, newY);
            m.rotate(newRot);
            painter->setTransform(m, true);
        }

        QRect iconRect;
        tabLayout(tab, widget, &tr, &iconRect);

        tr = p.proxy()->subElementRect(QStyle::SE_TabBarTabText, opt, widget);

        if (!tab->icon.isNull()) 
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QPixmap tabIcon = tab->icon.pixmap(QtnPrivate::qtn_getWindow(widget), tab->iconSize, (tab->state & QStyle::State_Enabled) ? 
                QIcon::Normal : QIcon::Disabled, (tab->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
#else
            QPixmap tabIcon = tab->icon.pixmap(tab->iconSize, (tab->state & QStyle::State_Enabled) ?
                QIcon::Normal : QIcon::Disabled, (tab->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
#endif
            painter->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
        }

        QPalette::ColorRole textRole = QPalette::ButtonText;
        if (selected)
            textRole = QPalette::WindowText;
        else if (highlighted)
            textRole = QPalette::HighlightedText;

        if (autoHideBar && !verticalTabs && !(tab->shape == QTabBar::RoundedSouth || tab->shape == QTabBar::TriangularSouth))
            tr.setHeight(tr.height() - stripTab);
        p.proxy()->drawItemText(painter, tr, alignment, tab->palette, tab->state & QStyle::State_Enabled, tab->text, textRole);

        if (verticalTabs)
            painter->restore();
        return true;
    }
    return false;
}

static QStyle::PrimitiveElement arrowTypeToPE(Qt::ArrowType type)
{
    QStyle::PrimitiveElement pe = QStyle::PE_IndicatorArrowLeft;
    switch (type) 
    {
        case Qt::LeftArrow:
            pe = QStyle::PE_IndicatorArrowLeft;
            break;
        case Qt::RightArrow:
            pe = QStyle::PE_IndicatorArrowRight;
            break;
        case Qt::UpArrow:
            pe = QStyle::PE_IndicatorArrowUp;
            break;
        case Qt::DownArrow:
            pe = QStyle::PE_IndicatorArrowDown;
            break;
        default:
        break;
    }
    return pe;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockWidgetTitleButton")))
    {
        if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
        {
            QIcon::State state = toolbutton->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(toolbutton->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                mode = QIcon::Active;
            else
                mode = QIcon::Normal;

            QPixmap pm = preparePixmap(toolbutton->icon, toolbutton->rect.size().boundedTo(toolbutton->iconSize), mode, state, widget);
            if (!pm.isNull())
                p.proxy()->drawItemPixmap(painter, opt->rect, Qt::AlignCenter, pm);
            return true;
        }
    }
    else if (widget && widget->inherits("QDockWidgetTitleButton"))
    {
        if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
        {
            QIcon::State state = toolbutton->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(toolbutton->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                mode = QIcon::Active;
            else
                mode = QIcon::Normal;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
            QPixmap pm = toolbutton->icon.pixmap(QtnPrivate::qtn_getWindow(widget), toolbutton->iconSize, mode, state);
#else
            QPixmap pm = toolbutton->icon.pixmap(toolbutton->iconSize, mode, state);
#endif
            pm = QtnPrivate::qtn_ColorizedImage(pm, Qt::black, mode == QIcon::Active ? Qt::white : QColor(192, 192, 192));

            int space = dpiScaled(2, widget);
            QSize iconSize = toolbutton->rect.size().boundedTo(toolbutton->iconSize) + QSize(space, space);
            if (pm.width() * pm.height() < iconSize.width() * iconSize.height())
                pm = pm.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.proxy()->drawItemPixmap(painter, opt->rect, Qt::AlignCenter, pm);
            return true;
        }
    }

    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) 
    {
        QRect rect = toolbutton->rect;
        // Arrow type always overrules and is always shown
        bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) 
        {
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                alignment |= Qt::TextHideMnemonic;
            painter->setFont(toolbutton->font);
            p.proxy()->drawItemText(painter, rect, alignment, toolbutton->palette, opt->state & QStyle::State_Enabled, toolbutton->text, QPalette::ButtonText);
        }
        else 
        {
            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (!toolbutton->icon.isNull()) 
            {
                QIcon::State state = toolbutton->state & QStyle::State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(toolbutton->state & QStyle::State_Enabled))
                    mode = QIcon::Disabled;
                else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;

                //pm = toolbutton->icon.pixmap(QtnPrivate::qtn_getWindow(widget), toolbutton->rect.size().boundedTo(toolbutton->iconSize), mode, state);
                //pmSize = pm.size() / pm.devicePixelRatio();
                int size = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);
                QSize iconSize(size, size);
                pm = preparePixmap(toolbutton->icon, toolbutton->rect.size().boundedTo(/*toolbutton->iconSize*/iconSize), mode, state, widget);
//                pm = preparePixmap(toolbutton->icon, toolbutton->rect.size().boundedTo(toolbutton->iconSize), mode, state, widget);
            }

            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) 
            {
                painter->setFont(toolbutton->font);
                QRect pr = rect, tr = rect;
                int alignment = Qt::TextShowMnemonic;
                if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                    alignment |= Qt::TextHideMnemonic;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) 
                {
                    pr.setHeight(pmSize.height() + 6);
                    tr.adjust(0, pr.height() - 1, 0, -1);
                    if (!hasArrow) 
                    {
//                        p.proxy()->drawItemPixmap(painter, rectItem, Qt::AlignCenter, pm);

                        p.proxy()->drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                    }
                    else 
                    {
                        drawIndicatorArrow(arrowTypeToPE(toolbutton->arrowType), opt, painter, widget);
//                        drawArrow(p.proxy(), toolbutton, pr, painter, widget);
                    }
                    alignment |= Qt::AlignCenter;
                }
                else 
                {
                    pr.setWidth(pmSize.width() + 8);
                    tr.adjust(pr.width(), 0, 0, 0);
                    if (!hasArrow)
                        p.proxy()->drawItemPixmap(painter, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                    else
                        drawIndicatorArrow(arrowTypeToPE(toolbutton->arrowType), opt, painter, widget);
//                        drawArrow(p.proxy(), toolbutton, pr, painter, widget);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                p.proxy()->drawItemText(painter, QStyle::visualRect(opt->direction, rect, tr), alignment, toolbutton->palette, 
                    toolbutton->state & QStyle::State_Enabled, toolbutton->text, QPalette::ButtonText);
            }
            else 
            {
                if (hasArrow) 
                    drawIndicatorArrow(arrowTypeToPE(toolbutton->arrowType), opt, painter, widget);
//                drawArrow(p.proxy(), toolbutton, rect, painter, widget);
                else 
                    p.proxy()->drawItemPixmap(painter, rect, Qt::AlignCenter, pm);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawScrollBarAddSubLine(QStyle::ControlElement element, const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const VisualStudio2019Style);
    if (p.isScrollBarsIgnored())
        return false;
    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
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
        p.proxy()->drawPrimitive(arrow, scrollbar, painter, w);
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawScrollBarAddSubPage(QStyle::ControlElement element, const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(element)
    Q_UNUSED(opt)
    Q_UNUSED(painter)
    Q_UNUSED(w)

    QTN_P(const VisualStudio2019Style);
    if (p.isScrollBarsIgnored())
        return false;
    return true;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawScrollBarSlider(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const VisualStudio2019Style);
    if (p.isScrollBarsIgnored())
        return false;
    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        if (!scrollbar->rect.isEmpty())
        {
            const bool horizontal = scrollbar->state & QStyle::State_Horizontal;
            const bool enabled = scrollbar->state & QStyle::State_Enabled;
            const bool highlighted = scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider;
            const bool pressed = scrollbar->state & QStyle::State_Sunken;

            QRect rc = scrollbar->rect;
            if (horizontal)
            {
                int space = ((50 * rc.height()) / 100) / 2;
                rc.adjust(0, space, 0, -space);
            }
            else
            {
                int space = ((50 * rc.width()) / 100) / 2;
                rc.adjust(space, 0, -space, 0);
            }

            bool show = horizontal ? rc.width() > 7 : rc.height() > 7;
            if (show)
            {
                QColor clrFill = scrollbar->palette.light().color();
                if (highlighted && pressed && enabled)
                    clrFill = scrollbar->palette.dark().color();
                else if (highlighted && enabled)
                    clrFill = scrollbar->palette.mid().color();
                painter->fillRect(rc, clrFill);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawDockWidgetTitle(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const VisualStudio2019Style);
    if (w && w->inherits("QDockWidget")) //判断控件是否存在 且 是否继承自QDockWidget
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (const QStyleOptionDockWidget* dwOpt = qstyleoption_cast<const QStyleOptionDockWidget*>(option)) //QStyleOptionDockWidget  QT原生
#else
        if (const QStyleOptionDockWidgetV2* dwOpt = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(option))
#endif
        {
            bool active = false;
            QColor clrWindow = colorSubControl(QStringLiteral("QDockWidget"), QStringLiteral("DockWidgetTitle"), active ? QPalette::Active : QPalette::Inactive, QPalette::Window);
            QColor clrText   = colorSubControl(QStringLiteral("QDockWidget"), QStringLiteral("DockWidgetTitle"), QPalette::Active, QPalette::WindowText);

            painter->fillRect(dwOpt->rect, clrWindow); //用clrWindow填充给定的矩形。
            QRect rect = dwOpt->rect.adjusted(0, 0, -1, -1); // adjusted 调整矩形位置 左上角不变 右下角同时-1 相当于宽高减了一个像素
            if (dwOpt->movable)
            {
                painter->setPen(dwOpt->palette.color(QPalette::Dark));
                painter->drawRect(rect);
            }

            if (!dwOpt->title.isEmpty())
            {
                const bool verticalTitleBar = dwOpt->verticalTitleBar;
                if (verticalTitleBar) //如果是垂直标题栏
                {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
                    rect = rect.transposed(); //返回宽度和高度互换的矩形的副本
#else
                    rect = QRect(rect.topLeft(), QSize(rect.height(), rect.width()));
#endif
                    painter->save(); //保存当前绘制器状态(将状态推到堆栈上)。save()后面必须跟着相应的restore();
                    painter->translate(rect.left(), rect.top() + rect.width()); //按给定偏移量平移坐标系;也就是说，将给定的偏移量加到点上。
                    painter->rotate(-90); //顺时针旋转坐标系。给定的角度参数的单位是度数。
                    painter->translate(-rect.left(), -rect.top());
                }

                QString text = dwOpt->title;
                if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), option, w))
                    text = text.toUpper();

                // draw text
                QPalette palette = dwOpt->palette;
                QPalette_setColor(palette, QPalette::WindowText, clrText);
                const int indent = painter->fontMetrics().descent(); // fontMetrics 如果绘制器处于活动状态，则返回绘制器的字体指标。否则，返回值未定义。 descent 返回字体的下降。
                p.proxy()->drawItemText(painter, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                    palette, dwOpt->state & QStyle::State_Enabled, text, QPalette::WindowText);

                if (verticalTitleBar)
                    painter->restore(); //展开堆栈。
            }
        }
        return true;
    } else if (w && w->inherits(QTITAN_META_CLASSNAME("DockTitleBar"))) {
        if (const DockTitleBarStyleOption* dwOpt = qstyleoption_cast<const DockTitleBarStyleOption*>(option))
        {
            const QRect rc = dwOpt->rect;
            QColor clr = dwOpt->palette.window().color();
            painter->fillRect(rc, clr);

            if ((m_themeType == VisualStudio2019Style::Light || m_themeType == VisualStudio2019Style::Dark) && !dwOpt->lineRect.isEmpty())
            {
                QtnPen pen(painter, dwOpt->palette.dark().color(), dpiScaled(1, w));
                QPoint pt(dwOpt->lineRect.left(), dwOpt->lineRect.center().y());
                int offsetX = 0;
                int spaceX = dpiScaled(4, w);
                int spaceY = dpiScaled(2, w);
                for (int y = -1; y < 2; y++)
                {
                    int xx = 0;
                    int countX = dwOpt->lineRect.right() - spaceX;
                    if (y == 0)
                    {
                        offsetX += spaceX / 2;
                        countX -= spaceX;
                    }
                    for (int x = 0; xx < countX; x++)
                    {
                        xx = pt.x() + x * spaceX;
                        painter->drawPoint(QPoint(xx + offsetX, pt.y() + 1 - y * spaceY));
                    }
                    offsetX = 0;
                }
            }

            if (!dwOpt->icon.isNull())
            {
                QRect rc = dwOpt->rcIcon;
                QPixmap pixmap = dwOpt->icon.pixmap(p.pixelMetric(QStyle::PM_SmallIconSize, option, w));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                const int pixw = qRound(static_cast<qreal>(pixmap.width()) / pixmap.devicePixelRatio());
                const int pixh = qRound(static_cast<qreal>(pixmap.height()) / pixmap.devicePixelRatio());
#else
                const int pixw = pixmap.width();
                const int pixh = pixmap.height();
#endif
                rc = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(pixw, pixh), rc);
                if (!pixmap.isNull())
                    painter->drawPixmap(rc, pixmap);
            }

            QRect titleRect = dwOpt->rcText;
            const QString text = dwOpt->fontMetrics.elidedText(dwOpt->title, Qt::ElideMiddle, titleRect.width() - 6);
            p.proxy()->drawItemText(painter, titleRect.adjusted(6, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                dwOpt->palette, true, text, QPalette::WindowText);
            return true;
        }
    } else
        return CommonStylePrivate::drawDockWidgetTitle(option, painter, w);
    return false;
}

/*! \internal */
bool VisualStudio2019StylePrivate::drawTabBarTabShape(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab *>(opt))
#else
    if (const QStyleOptionTabV3* tab = qstyleoption_cast<const QStyleOptionTabV3 *>(opt))
#endif
    {
        painter->save();

        const bool rtlHorTabs = (tab->direction == Qt::RightToLeft && (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth));
        const bool selected = tab->state & QStyle::State_Selected;
        const bool lastTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::End) || (rtlHorTabs && tab->position == QStyleOptionTab::Beginning));
        const bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
        const bool highlighted = tab->state & QStyle::State_MouseOver;
        int tabOverlap = 0;//p.proxy()->pixelMetric(QStyle::PM_TabBarTabOverlap, opt, widget);
        QRect rect = opt->rect.adjusted(0, 0, (onlyOne || lastTab) ? 0 : tabOverlap, 0);

        QRect r2(rect);
        int x1 = r2.left();
        int x2 = r2.right();
        int y1 = r2.top();
        int y2 = r2.bottom();

        bool flip = false;

        QTransform rotMatrix;
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

        const int stripTab = CommonStylePrivate::dpiScaled(g_roundedRadius, widget);
        if (selected)
        {
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockAreaTabBar")))
            {
                QRect drawRect = rect.adjusted(0, 0, 0, 0);
                painter->fillRect(drawRect, opt->palette.base().color());
            }
            else if (widget && widget->inherits(QTITAN_META_CLASSNAME("TabWidgetAutoHideBar")))
            {
                QRect drawRect = rect.adjusted(0, 0, 0, 0);
                drawRect.setTop(drawRect.bottom() - stripTab);
                painter->fillRect(drawRect, highlighted ? opt->palette.highlight().color() : opt->palette.base().color());
            }
            else if (widget && widget->inherits("QTabBar"))
                painter->fillRect(rect, opt->palette.base().color());
        }
        else
        {
            if (widget && (widget->inherits(QTITAN_META_CLASSNAME("DockAreaTabBar")) || widget->inherits(QTITAN_META_CLASSNAME("DockDocumentAreaTabBar")) /*|| widget->inherits("QTabBar")*/))
            {
                QRect drawRect = rect.adjusted(tab->position == QStyleOptionTab::Beginning ? 0 : 1, 1, -1, -2);
                if (widget->inherits(QTITAN_META_CLASSNAME("DockDocumentAreaTabBar")))
                    drawRect = rect.adjusted(tab->position == QStyleOptionTab::Beginning ? 0 : 1, 2, -1, -4);
                painter->fillRect(drawRect, highlighted ? opt->palette.highlight().color() : opt->palette.button().color());
            }
            else if (widget && widget->inherits(QTITAN_META_CLASSNAME("TabWidgetAutoHideBar")))
            {
                QRect drawRect = rect.adjusted(stripTab, 0, -stripTab, 0);
                drawRect.setTop(drawRect.bottom() - stripTab);
                painter->fillRect(drawRect, highlighted ? opt->palette.highlight().color() : opt->palette.base().color());
            }
            else if (widget && widget->inherits("QTabBar"))
            {
                QRect drawRect = rect.adjusted(tab->position == QStyleOptionTab::Beginning ? 0 : 1, 1, -1, -2);
                painter->fillRect(drawRect, highlighted ? opt->palette.highlight().color() : opt->palette.button().color());
            }
        }
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
QPixmap VisualStudio2019StylePrivate::dockStandardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    QPixmap px;
    if (px.isNull() && widget && (widget->inherits(QTITAN_META_CLASSNAME("DockWidgetTitleButton")) || 
        widget->inherits(QTITAN_META_CLASSNAME("DockTitleBar")) || widget->inherits("CloseButton")))
    {
        ImageState state = CommonStylePrivate::LtGray;
        if (opt != Q_NULL)
        {
            if (((opt->state & QStyle::State_Active) == 0) && widget->inherits("CloseButton"))
            {
                state = m_themeType == VisualStudio2019Style::Light ? CommonStylePrivate::DkGray : CommonStylePrivate::Black;
                if (opt->state & QStyle::State_MouseOver || opt->state & QStyle::State_Sunken)
                    state = CommonStylePrivate::DkGray;
            }
            else if (opt->state & QStyle::State_Active)
            {
                state = m_themeType == VisualStudio2019Style::Light ? CommonStylePrivate::DkGray : CommonStylePrivate::Black;
                if (opt->state & QStyle::State_MouseOver || opt->state & QStyle::State_Sunken)
                    state = CommonStylePrivate::DkGray;
            }
            else if (opt->state & QStyle::State_MouseOver)
                state = CommonStylePrivate::White;
        }

        switch (sp)
        {
            case CommonStyle::SP_TitleBarCloseButton:
                px = standardPixmap(QStringLiteral("Image_Close"), state, widget);
                break;
            case CommonStyle::SP_ArrowDown:
                px = standardPixmap(QStringLiteral("Image_ShadedArrowDown"), state, widget);
                break;
            default:
                break;
        }

        switch (static_cast<CommonStyle::StandardPixmapEx>(sp))
        {
            case CommonStyle::SP_TitleBarPinButton:
                px = standardPixmap(QStringLiteral("Image_Pin"), state, widget);
                break;
            case CommonStyle::SP_TitleBarPinnedButton:
                px = standardPixmap(QStringLiteral("Image_Pinned"), state, widget);
                break;
            default:
                break;
        }
    }
    return px.isNull() ? CommonStylePrivate::dockStandardPixmap(sp, opt, widget) : px;
}

/*! \internal */
QRect VisualStudio2019StylePrivate::dockSubElementRect(QStyle::SubElement sr, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_P(const VisualStudio2019Style);
    QRect r;
    switch (sr)
    {
        case QStyle::SE_TabBarTabLeftButton:
        case QStyle::SE_TabBarTabRightButton:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab *>(opt))
#else
            if (const QStyleOptionTabV3* tab = qstyleoption_cast<const QStyleOptionTabV3 *>(opt))
#endif
            {
                bool selected = tab->state & QStyle::State_Selected;
                int verticalShift = p.proxy()->pixelMetric(QStyle::PM_TabBarTabShiftVertical, tab, widget);
                int horizontalShift = p.proxy()->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, tab, widget);
                int hpadding = p.proxy()->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
                hpadding = qMax(hpadding, 4); //workaround KStyle returning 0 because they workaround an old bug in Qt

                bool verticalTabs = tab->shape == QTabBar::RoundedEast || tab->shape == QTabBar::RoundedWest || 
                    tab->shape == QTabBar::TriangularEast || tab->shape == QTabBar::TriangularWest;

                QRect tr = tab->rect;
                if (tab->shape == QTabBar::RoundedSouth || tab->shape == QTabBar::TriangularSouth)
                    verticalShift = -verticalShift;
                if (verticalTabs) 
                {
                    qSwap(horizontalShift, verticalShift);
                    horizontalShift *= -1;
                    verticalShift *= -1;
                }
                if (tab->shape == QTabBar::RoundedWest || tab->shape == QTabBar::TriangularWest)
                    horizontalShift = -horizontalShift;

                tr.adjust(0, 0, horizontalShift, verticalShift);
                if (selected)
                {
                    tr.setBottom(tr.bottom() - verticalShift);
                    tr.setRight(tr.right() - horizontalShift);
                }

                QSize size = (sr == QStyle::SE_TabBarTabLeftButton) ? tab->leftButtonSize : tab->rightButtonSize;
                int w = size.width();
                int h = size.height();
                int midHeight = static_cast<int>(qCeil(qreal(tr.height() - h) / 2));
                int midWidth = ((tr.width() - w) / 2);

                bool atTheTop = true;
                switch (tab->shape) 
                {
                    case QTabBar::RoundedWest:
                    case QTabBar::TriangularWest:
                        atTheTop = (sr == QStyle::SE_TabBarTabLeftButton);
                        break;
                    case QTabBar::RoundedEast:
                    case QTabBar::TriangularEast:
                        atTheTop = (sr == QStyle::SE_TabBarTabRightButton);
                        break;
                    default:
                        if (sr == QStyle::SE_TabBarTabLeftButton)
                            r = QRect(tab->rect.x() + hpadding, midHeight, w, h);
                        else
                            r = QRect(tab->rect.right() - w /*- hpadding*/, midHeight, w, h);
                        r = p.visualRect(tab->direction, tab->rect, r);
                }
                if (verticalTabs) 
                {
                    if (atTheTop)
                        r = QRect(midWidth, tr.y() + tab->rect.height() - hpadding - h, w, h);
                    else
                        r = QRect(midWidth, tr.y() + hpadding, w, h);
                }
            }
            break;
        default:
            break;
    }
    return r;
}

/*! \internal */
void VisualStudio2019StylePrivate::updateTheme()
{
    QTN_P(VisualStudio2019Style)
    QString themeName = QStringLiteral("Blue");
    switch (m_themeType)
    {
        case VisualStudio2019Style::Dark:
            themeName = QStringLiteral("Dark");
            break;
        case VisualStudio2019Style::Light:
            themeName = QStringLiteral("Light");
            break;
        case VisualStudio2019Style::Blue:
            themeName = QStringLiteral("Blue");
            break;
    }
    p.setStyleConfig(QStringLiteral(":/res/VisualStudio2019/"), themeName);
}

/*! \internal */
QColor VisualStudio2019StylePrivate::titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* wd) const
{
    QPalette palette = pal;
    QColor frameColor = colorSubControl(QStringLiteral("QMainWindow"), QStringLiteral("MainFrameWindow"), 
        isActive ? QPalette::Active : QPalette::Inactive, QPalette::Dark);
    QPalette_setColor(palette, isActive ? QPalette::Active : QPalette::Inactive, QPalette::Shadow, frameColor);
    return CommonStylePrivate::titleBarFrameColor(palette, isActive, wd);
}


/*!
\class VisualStudio2019Style
\inmodule QtitanStyle
\brief Class implements Microsoft Visual Studio 2019 style for Qt application with three themes: Light, Dark and Blue.
*/
/*!
Constuctor of VisualStudio2019Style class. 
*/
VisualStudio2019Style::VisualStudio2019Style()
    : CommonStyle(*new VisualStudio2019StylePrivate)
{
    initStyleResource();
    QTN_D(VisualStudio2019Style);
    d.updateTheme();
}

VisualStudio2019Style::~VisualStudio2019Style()
{
}

VisualStudio2019Style::Theme VisualStudio2019Style::theme() const
{
    QTN_D(const VisualStudio2019Style);
    return d.m_themeType;
}

/*!
\property VisualStudio2019Style::theme
Sets the \a theme of the office style. You can choose from Blue, Dark or Light theme's families.
\sa Theme
*/
void VisualStudio2019Style::setTheme(Theme theme)
{
    QTN_D(VisualStudio2019Style);
    if (d.m_themeType != theme)
    {
        d.beginUpdate();
        d.m_themeType = theme;
        d.updateTheme();
        d.endUpdate();
    }
}

/*! \reimp */
void VisualStudio2019Style::polish(QWidget* widget)
{
    CommonStyle::polish(widget);
    if (QListView* list = qobject_cast<QListView*> (widget))
        list->viewport()->setAttribute(Qt::WA_Hover, true);
    else if (QTreeView* tree = qobject_cast<QTreeView*> (widget))
        tree->viewport()->setAttribute(Qt::WA_Hover, true);
    else if (QTabBar* tabBar = qobject_cast<QTabBar*>(widget))
        tabBar->setExpanding(false);

    if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
        mdiArea->setBackground(widget->palette().brush(QPalette::Dark));
}

/*! \reimp */
void VisualStudio2019Style::polish(QApplication* app)
{
    CommonStyle::polish(app);

    QFont font = app->font("QMenu");
    font.setFamily(QStringLiteral("Segoe UI"));
    font.setPointSize(8);
    app->setFont(font, QTITAN_META_CLASSNAME("RibbonBar"));
}

/*! \reimp */
void VisualStudio2019Style::unpolish(QWidget* widget)
{
    QTN_D(VisualStudio2019Style);
    CommonStyle::unpolish(widget);
    // d.m_basePalette
    if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
        mdiArea->setBackground(d.m_systemPalette.brush(QPalette::Dark));
}

/*! \reimp */
QSize VisualStudio2019Style::sizeFromContents(QStyle::ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget) const
{
    QSize sz;
    switch (ct)
    {
        case CT_MenuBarItem:
            if (const QStyleOptionMenuItem* menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
            {
                QFontMetrics metrics(menuItem->font);
                QRect boundingRect = metrics.boundingRect(menuItem->text);
                int width = boundingRect.width() + metrics.height() + 1;
                int height = boundingRect.height() + metrics.height() / 2;
                sz =  QSize(width, height);
            }
            break;
        default:
            sz = QSize();
            break;
    }
    if (sz.isEmpty())
        return CommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
    return sz;
}

/*! \reimp */
QPixmap VisualStudio2019Style::standardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const VisualStudio2019Style);
    QPixmap px = d.dockStandardPixmap(sp, opt, widget);
    if (!px.isNull())
        return px;

    switch (sp)
    {
        case SP_ArrowUp   :
        case SP_ArrowDown :
        case SP_ArrowLeft :
        case SP_ArrowRight:
        {
            if (widget && qobject_cast<const QScrollBar*>(widget))
            {
                if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
                {
                    const bool enabled = opt && opt->state & QStyle::State_Enabled;
                    const bool highlighted = (scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine && (sp == QStyle::SP_ArrowDown || sp == QStyle::SP_ArrowRight)) ||
                        (scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine && (sp == QStyle::SP_ArrowUp || sp == QStyle::SP_ArrowLeft));
                    const bool pressed = opt->state & QStyle::State_Sunken;

                    CommonStylePrivate::ImageState state = CommonStylePrivate::Color0;
                    if (enabled && pressed && highlighted)
                        state = CommonStylePrivate::Color2;
                    else if (enabled && highlighted)
                        state = CommonStylePrivate::Color1;
                    else
                        state = CommonStylePrivate::Color3;
                    if (sp == QStyle::SP_ArrowRight || sp == QStyle::SP_ArrowLeft)
                        px = d.standardPixmap(sp == QStyle::SP_ArrowRight ? QStringLiteral("Image_ShadedArrowRight") : QStringLiteral("Image_ShadedArrowLeft"), state, widget);
                    else
                        px = d.standardPixmap(sp == QStyle::SP_ArrowUp ? QStringLiteral("Image_ShadedArrowUp") : QStringLiteral("Image_ShadedArrowDown"), state, widget);
                }
            }
            else if (widget && qobject_cast<const QComboBox*>(widget) && (d.m_themeType == VisualStudio2019Style::Light || d.m_themeType == VisualStudio2019Style::Dark))
            {
                const bool enabled = opt && opt->state & QStyle::State_Enabled;
                bool dropped = ((opt->state & QStyle::State_On) || (opt->state & QStyle::State_Sunken));

                CommonStylePrivate::ImageState state = CommonStylePrivate::Black;
                if (enabled && dropped)
                    state = CommonStylePrivate::Black2;
                else if (!enabled)
                    state = CommonStylePrivate::Gray;

                if (sp == SP_ArrowDown)
                    px = d.standardPixmap(QStringLiteral("Image_ShadedArrowDown"), state, widget);
                else
                    Q_ASSERT(false);
            }
            else
            {
                if (sp == SP_ArrowUp)
                    px = d.standardPixmap(QStringLiteral("Image_ShadedArrowUp"), opt && opt->state & QStyle::State_Enabled ? CommonStylePrivate::Black : CommonStylePrivate::Gray, widget);
                else if (sp == SP_ArrowDown)
                    px = d.standardPixmap(QStringLiteral("Image_ShadedArrowDown"), opt && opt->state & QStyle::State_Enabled ? CommonStylePrivate::Black : CommonStylePrivate::Gray, widget);
                else if (sp == SP_ArrowLeft)
                    px = d.standardPixmap(QStringLiteral("Image_ShadedArrowLeft"), opt && opt->state & QStyle::State_Enabled ? CommonStylePrivate::Black : CommonStylePrivate::Gray, widget);
                else if (sp == SP_ArrowRight)
                    px = d.standardPixmap(QStringLiteral("Image_ShadedArrowRight"), opt && opt->state & QStyle::State_Enabled ? CommonStylePrivate::Black : CommonStylePrivate::Gray, widget);
            }
        }
        break;
        default:
        break;
    }
    if (px.isNull())
        return CommonStyle::standardPixmap(sp, opt, widget);
    return px;
}

