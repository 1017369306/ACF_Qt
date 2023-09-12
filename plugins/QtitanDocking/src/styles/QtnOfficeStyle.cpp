/****************************************************************************
**
** Qtitan Library by Developer Machines (Office Style implementation for Qt.C++)
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
#include <QPixmapCache>
#include <QScrollBar>
#include <QListView>
#include <QTreeView>
#include <QTableView>
#include <QToolButton>
#include <QMdiArea>
#include <QMenuBar>
#include <QDockWidget>
#include <QToolTip>
#include <QToolBar>
#include <QPainterPath>
#include <qdrawutil.h>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif /* Q_OS_WIN*/
#include "QtnSegoeMDL2Assets.h"
#include "QtnOfficeStylePrivate.h"
#include "QtnStyleOption.h"
#include "QtnStyleHelperPrivate.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

/*
    \class OfficeStyle
    \inmodule QtitanStyle
    \brief  OfficeStyle the base class for Office2007Style, Office2010Style and Office2016Style and Windows7ScenicStyle. Contains the general settings
            that are typical for all styles of Microsof Office. The style available for Windows, Linux and MacOSX without any restrictions.
*/


/*!
    \enum OfficeStyle::OfficePopupDecoration
    \inmodule QtitanStyle
    \brief  This enum determines the decoration for specialized popup windows - OfficePopupWindow, which are used for informing users about the events which are occurred.
    \value  PopupSystemDecoration Decoration of the popup depends from value of the style installed in the QApplication or in the poup window inside.
            If the installed style is descendant of OfficeStyle then the value of the getTheme() will be taken into account when decoration will be painting.
    \value  PopupCompatibleDecoration Old decoration which has used in XP and other old Microsoft products.
    \value  PopupOfficeFlatDecoration Flat decoration of the popup
    \value  PopupMSNDecoration Decoration which has used in Microsoft Messanger product.
*/

QTITAN_USE_NAMESPACE

#if 0
#ifdef Q_OS_WIN
static QFont::Weight weightFromInteger(int weight)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (weight < 150)
        return QFont::Thin;
    if (weight < 250)
        return QFont::ExtraLight;
    if (weight < 350)
        return QFont::Light;
    if (weight < 450)
        return QFont::Normal;
    if (weight < 550)
        return QFont::Medium;
    if (weight < 650)
        return QFont::DemiBold;
    if (weight < 750)
        return QFont::Bold;
    if (weight < 850)
        return QFont::ExtraBold;
    return QFont::Black;
#else
    if (weight < 400)
        return QFont::Light;
    else if (weight < 600)
        return QFont::Normal;
    else if (weight < 700)
        return QFont::DemiBold;
    else if (weight < 800)
        return QFont::Bold;
    else
        return QFont::Black;
#endif
}

const double g_logPixel = 96.0;
static QFont qtn_LOGFONTtoQFont(LOGFONT& lf, bool dpiaware, const QWidget* widget)
{
    const HDC displayDC = GetDC(Q_NULL);

    qreal factor = dpiaware ? 1.0 : static_cast<qreal>(qtn_getWindowDPI(widget)) / g_logPixel;
    qreal verticalDPI_In = static_cast<qreal>(GetDeviceCaps(displayDC, LOGPIXELSY));

    QFont qFont(QString::fromUtf16(reinterpret_cast<const ushort *>(lf.lfFaceName)));
    qFont.setItalic(lf.lfItalic);
    if (lf.lfWeight != FW_DONTCARE)
        qFont.setWeight(::weightFromInteger(lf.lfWeight));

    const qreal logFontHeight = qAbs(lf.lfHeight);
    qFont.setPointSizeF(logFontHeight * (72.0 / factor) / qreal(verticalDPI_In));
    qFont.setUnderline(lf.lfUnderline);
    qFont.setOverline(false);
    qFont.setStrikeOut(lf.lfStrikeOut);
    ReleaseDC(Q_NULL, displayDC);
    return qFont;
}
#endif
#endif

/* OfficeStylePrivate */
OfficeStylePrivate::OfficeStylePrivate()
    : CommonStylePrivate()
    , m_clrAccent(QColor(43, 87, 154))
    , m_curAccentColor(OfficeStyle::AccentColorBlue)
    , m_isActiveTabAccented(false)
    , m_ignoreMDIWindowTitle(false)
    , m_typePopupStyle(OfficeStyle::PopupSystemDecoration)
{
}

OfficeStylePrivate::~OfficeStylePrivate()
{
}

void OfficeStylePrivate::init()
{
    CommonStylePrivate::init();
}

QFont OfficeStylePrivate::getWidgetFont(const QString& className) const
{
    if (className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBar")) ||
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonStatusBar")) ||
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBackstageView")))
    {

        QFont font = QApplication::font("QMenuBar");
#ifdef Q_OS_WIN
        font.setFamily(QStringLiteral("Segoe UI"));
#else
        font.setFamily(QStringLiteral("Arial"));
#endif
        int psize = font.pointSize();
        font.setPointSize(psize - 2);
        font.setBold(false);
        font.setItalic(false);
        return font;
    }
    return CommonStylePrivate::getWidgetFont(className);
}

QColor OfficeStylePrivate::accentIndexToColor(OfficeStyle::AccentColor accentcolor) const
{
    switch (accentcolor)
    {
        case OfficeStyle::AccentColorBlue:
        default:
            return QColor(43, 87, 154);
        case OfficeStyle::AccentColorBrown:
            return QColor(161, 53, 55);
        case OfficeStyle::AccentColorGreen:
            return QColor(33, 115, 70);
        case OfficeStyle::AccentColorLime:
            return QColor(137, 164, 48);
        case OfficeStyle::AccentColorMagenta:
            return QColor(216, 0, 115);
        case OfficeStyle::AccentColorOrange:
            return QColor(208, 69, 37);
        case OfficeStyle::AccentColorPink:
            return QColor(230, 113, 184);
        case OfficeStyle::AccentColorPurple:
            return QColor(126, 56, 120);
        case OfficeStyle::AccentColorRed:
            return QColor(229, 20, 0);
        case OfficeStyle::AccentColorTeal:
            return QColor(7, 114, 101);
    }
}

/*! \internal */
bool OfficeStylePrivate::drawTabBarTabLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const OfficeStyle);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) 
#else
    if (const QStyleOptionTabV3* tab = qstyleoption_cast<const QStyleOptionTabV3 *>(opt)) 
#endif
    {
        QRect tr = tab->rect;
        bool verticalTabs = tab->shape == QTabBar::RoundedEast || 
            tab->shape == QTabBar::RoundedWest    || 
            tab->shape == QTabBar::TriangularEast || 
            tab->shape == QTabBar::TriangularWest;

        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
            alignment |= Qt::TextHideMnemonic;

        if (verticalTabs) 
        {
            painter->save();
            int newX, newY, newRot;
            if (tab->shape == QTabBar::RoundedEast || tab->shape == QTabBar::TriangularEast) 
            {
                newX = tr.width() + tr.x();
                newY = tr.y();
                newRot = 90;
            }
            else 
            {
                newX = tr.x();
                newY = tr.y() + tr.height();
                newRot = -90;
            }
            QTransform m = QTransform::fromTranslate(newX, newY);
            m.rotate(newRot);
            painter->setTransform(m, true);
        }
        QRect iconRect;
        tabLayout(tab, widget, &tr, &iconRect);
        tr = p.proxy()->subElementRect(QStyle::SE_TabBarTabText, opt, widget); //we compute tr twice because the style may override subElementRect

        if (!tab->icon.isNull()) 
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QPixmap tabIcon = tab->icon.pixmap(QtnPrivate::qtn_getWindow(widget), tab->iconSize,
                (tab->state & QStyle::State_Enabled) ? QIcon::Normal
                : QIcon::Disabled,
                (tab->state & QStyle::State_Selected) ? QIcon::On
                : QIcon::Off);
#else
            QPixmap tabIcon = tab->icon.pixmap(tab->iconSize,
                (tab->state & QStyle::State_Enabled) ? QIcon::Normal
                : QIcon::Disabled,
                (tab->state & QStyle::State_Selected) ? QIcon::On
                : QIcon::Off);
#endif
            painter->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
        }

        QPalette palette = tab->palette;
        if (tab->state & QStyle::State_Selected || QtnPrivate::qtn_isAncestor(widget, "QDialog"))
            QPalette_setColor(palette, QPalette::WindowText, palette.color(QPalette::Text));

        p.proxy()->drawItemText(painter, tr, alignment, palette, tab->state & QStyle::State_Enabled, tab->text, QPalette::WindowText);
        if (verticalTabs)
            painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStylePrivate::drawPanelTipLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    const QBrush brush(opt->palette.toolTipBase());
    const QColor clrLight = StyleHelper::colorMakeLighter(brush.color().rgb(), 2.5);
    const QColor clrDark = brush.color();
    StyleHelper::drawGradientFill(*painter, opt->rect, clrLight, clrDark, true);
    return drawWidgetPixmap(QStringLiteral("ToolTip"), QStringLiteral("Image_TooltipFrame"), opt->rect, painter, 0, 1, QMargins(3, 3, 3, 3), QColor(0xFF, 0, 0xFF));
}

/*! \internal */
bool OfficeStylePrivate::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    if (qobject_cast<const QMenu*>(w))
        return false;
    return drawIndicatorArrowDp(pe, option, painter, w);
}

/*! \internal */
bool OfficeStylePrivate::drawIndicatorItemCheck(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const OfficeStyle);
    if (const QStyleOptionViewItem* itemViewOpt = qstyleoption_cast<const QStyleOptionViewItem *>(opt))
    {
        painter->save();
        painter->translate(0.5, 0.5);

        painter->setPen(itemViewOpt && itemViewOpt->showDecorationSelected && opt->state & QStyle::State_Selected ?
            opt->palette.highlightedText().color() : opt->palette.text().color());
        if (opt->state & QStyle::State_NoChange)
            painter->setBrush(opt->palette.brush(QPalette::Button));
        qreal size = p.proxy()->pixelMetric(QStyle::PM_IndicatorWidth, opt, w);
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
bool OfficeStylePrivate::drawIndicatorBranch(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (!(opt->state & QStyle::State_Children))
        return true;
    if (opt->state & QStyle::State_Open)
        drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, opt, p, widget);
    else
        drawIndicatorArrow(QStyle::PE_IndicatorArrowRight, opt, p, widget);
    return true;

/*
    int decoration_size = CommonStylePrivate::dpiScaled(11.0, widget);//qMax(opt->rect.width(), opt->rect.height());

//    const int decoration_size = 19;
    int mid_h = opt->rect.x() + opt->rect.width() / 2;
    int mid_v = opt->rect.y() + opt->rect.height() / 2;
    int bef_h = mid_h;
    int bef_v = mid_v;
    int aft_h = mid_h;
    int aft_v = mid_v;
    if (opt->state & QStyle::State_Children) 
    {
        int delta = CommonStylePrivate::dpiScaled(decoration_size / 2, widget);
//        bef_h -= delta;
        bef_v -= delta;
        aft_h += delta;
        aft_v += delta;
        p->drawLine(bef_h + CommonStylePrivate::dpiScaled(2.0, widget), bef_v + CommonStylePrivate::dpiScaled(4.0, widget),
            bef_h + CommonStylePrivate::dpiScaled(6.0, widget), bef_v + CommonStylePrivate::dpiScaled(4.0, widget));

        if (!(opt->state & QStyle::State_Open))
            p->drawLine(bef_h + CommonStylePrivate::dpiScaled(4.0, widget), bef_v + CommonStylePrivate::dpiScaled(2.0, widget), 
                bef_h + CommonStylePrivate::dpiScaled(4.0, widget), bef_v + CommonStylePrivate::dpiScaled(6.0, widget));
        QPen oldPen = p->pen();
        p->setPen(opt->palette.dark().color());
        p->drawRect(bef_h, bef_v, decoration_size - 1, decoration_size - 1);
        p->setPen(oldPen);
    }
*/
/*
    QBrush brush(opt->palette.dark().color(), Qt::Dense4Pattern);
    if (opt->state & QStyle::State_Item) 
    {
        if (opt->direction == Qt::RightToLeft)
            p->fillRect(opt->rect.left(), mid_v, bef_h - opt->rect.left(), 1, brush);
        else
            p->fillRect(aft_h, mid_v, opt->rect.right() - aft_h + 1, 1, brush);
    }
    if (opt->state & QStyle::State_Sibling)
        p->fillRect(mid_h, aft_v, 1, opt->rect.bottom() - aft_v + 1, brush);
    if (opt->state & (QStyle::State_Open | QStyle::State_Children | QStyle::State_Item | QStyle::State_Sibling))
        p->fillRect(mid_h, opt->rect.y(), 1, bef_v - opt->rect.y(), brush);
*/
    return true;
}

/*! \internal */
bool OfficeStylePrivate::drawPanelItemViewItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionViewItem* vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt))
#else
    if (const QStyleOptionViewItemV4* vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(opt))
#endif
    {
        bool newStyle = true;
        QAbstractItemView::SelectionBehavior selectionBehavior = QAbstractItemView::SelectRows;
        QAbstractItemView::SelectionMode selectionMode = QAbstractItemView::NoSelection;
        if (const QAbstractItemView *view = qobject_cast<const QAbstractItemView *>(widget))
        {
            newStyle = !qobject_cast<const QTableView*>(view);
            selectionBehavior = view->selectionBehavior();
            selectionMode = view->selectionMode();
        }
        if (newStyle)
        {
            bool selected = vopt->state & QStyle::State_Selected;
            const bool hover = selectionMode != QAbstractItemView::NoSelection && (vopt->state & QStyle::State_MouseOver);
            bool active = vopt->state & QStyle::State_Active;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
            if (vopt->features & QStyleOptionViewItem::Alternate)
#else
            if (vopt->features & QStyleOptionViewItemV4::Alternate)
#endif
                painter->fillRect(vopt->rect, vopt->palette.alternateBase());

            QPalette::ColorGroup cg = vopt->state & QStyle::State_Enabled
                ? QPalette::Normal : QPalette::Disabled;
            if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
                cg = QPalette::Inactive;

            QRect itemRect = p.proxy()->subElementRect(QStyle::SE_ItemViewItemFocusRect, opt, widget).adjusted(-1, 0, 1, 0);
            itemRect.setTop(vopt->rect.top());
            itemRect.setBottom(vopt->rect.bottom());

            QSize sectionSize = itemRect.size();
            if (vopt->showDecorationSelected)
                sectionSize = vopt->rect.size();

            if (selectionBehavior == QAbstractItemView::SelectRows)
                sectionSize.setWidth(vopt->rect.width());
            QPixmap pixmap;

            if (vopt->backgroundBrush.style() != Qt::NoBrush)
            {
                const QPointF oldBrushOrigin = painter->brushOrigin();
                painter->setBrushOrigin(vopt->rect.topLeft());
                painter->fillRect(vopt->rect, vopt->backgroundBrush);
                painter->setBrushOrigin(oldBrushOrigin);
            }

            if (hover || selected)
            {
                if (sectionSize.width() > 0 && sectionSize.height() > 0)
                {
                    QString key = QString::fromLatin1("qtnvdelegate-%1-%2-%3-%4-%5").arg(sectionSize.width()).arg(sectionSize.height()).arg(selected).arg(active).arg(hover);
                    if (!QPixmapCache::find(key, &pixmap))
                    {
                        pixmap = QPixmap(sectionSize);
                        pixmap.fill(Qt::transparent);

                        QPainter pixmapPainter(&pixmap);
                        if (selected && hover) // LISS_HOTSELECTED
                        {
                            pixmapPainter.fillRect(QRect(0, 0, sectionSize.width(), sectionSize.height()), vopt->palette.brush(cg, QPalette::Highlight));
                        }
                        else if (selected && !active) // LISS_SELECTEDNOTFOCUS
                        {
                            pixmapPainter.fillRect(QRect(0, 0, sectionSize.width(), sectionSize.height()), vopt->palette.brush(cg, QPalette::Highlight));
                        }
                        else if (selected) // LISS_SELECTED
                        {
                            pixmapPainter.fillRect(QRect(0, 0, sectionSize.width(), sectionSize.height()), vopt->palette.brush(cg, QPalette::Highlight));
                        }
                        else // LISS_HOT
                        {
                            QColor clr = vopt->palette.brush(cg, QPalette::Highlight).color();
                            clr = StyleHelper::colorMakeLighter(clr.rgba(), 0.07);
                            pixmapPainter.fillRect(QRect(0, 0, sectionSize.width(), sectionSize.height()), clr);
                        }
                        QPixmapCache::insert(key, pixmap);
                    }
                }

                if (vopt->showDecorationSelected)
                {
                    const int frame = 2; //Assumes a 2 pixel pixmap border
                    QRect srcRect = QRect(0, 0, sectionSize.width(), sectionSize.height());
                    QRect pixmapRect = vopt->rect;
                    bool reverse = vopt->direction == Qt::RightToLeft;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
                    bool leftSection = vopt->viewItemPosition == QStyleOptionViewItem::Beginning;
                    bool rightSection = vopt->viewItemPosition == QStyleOptionViewItem::End;
                    if (vopt->viewItemPosition == QStyleOptionViewItem::OnlyOne || vopt->viewItemPosition == QStyleOptionViewItem::Invalid)
#else
                    bool leftSection = vopt->viewItemPosition == QStyleOptionViewItemV4::Beginning;
                    bool rightSection = vopt->viewItemPosition == QStyleOptionViewItemV4::End;
                    if (vopt->viewItemPosition == QStyleOptionViewItemV4::OnlyOne || vopt->viewItemPosition == QStyleOptionViewItemV4::Invalid)
#endif
                        painter->drawPixmap(pixmapRect.topLeft(), pixmap);
                    else if (reverse ? rightSection : leftSection)
                    {
                        painter->drawPixmap(QRect(pixmapRect.topLeft(), QSize(frame, pixmapRect.height())), pixmap, QRect(QPoint(0, 0), QSize(frame, pixmapRect.height())));
                        painter->drawPixmap(pixmapRect.adjusted(frame, 0, 0, 0), pixmap, srcRect.adjusted(frame, 0, -frame, 0));
                    }
                    else if (reverse ? leftSection : rightSection) {
                        painter->drawPixmap(QRect(pixmapRect.topRight() - QPoint(frame - 1, 0), QSize(frame, pixmapRect.height())), pixmap, QRect(QPoint(pixmapRect.width() - frame, 0), QSize(frame, pixmapRect.height())));
                        painter->drawPixmap(pixmapRect.adjusted(0, 0, -frame, 0), pixmap, srcRect.adjusted(frame, 0, -frame, 0));
                    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
                    else if (vopt->viewItemPosition == QStyleOptionViewItem::Middle)
#else
                    else if (vopt->viewItemPosition == QStyleOptionViewItemV4::Middle)
#endif
                        painter->drawPixmap(pixmapRect, pixmap, srcRect.adjusted(frame, 0, -frame, 0));
                }
                else
                {
                    if (vopt->text.isEmpty() && vopt->icon.isNull())
                        return false;
                    painter->drawPixmap(itemRect.topLeft(), pixmap);
                }
            }
            return true;
        }
        else
            return false;
    }
    return false;
}

/*! \internal */
bool OfficeStylePrivate::drawKeyTip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QColor clrLight = opt->palette.color(QPalette::Light);
    QColor clrDark = opt->palette.color(QPalette::Midlight);
    StyleHelper::drawGradientFill(*painter, opt->rect, clrLight, clrDark, true);
    if (!drawWidgetPixmap(QStringLiteral("ToolTip"), QStringLiteral("Image_TooltipFrame"), opt->rect, painter, 0, 1, QMargins(3, 3, 3, 3), QColor(0xFF, 0, 0xFF)))
        return CommonStylePrivate::drawKeyTip(opt, painter, w);
    return true;
}

/*! \internal */
bool OfficeStylePrivate::drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget != Q_NULL && widget->inherits("QDockWidgetTitleButton"))
        return false;
    return drawToolButtonLabelDp(opt, painter, widget);
}

/*! 
\class OfficeStyle
\inmodule QtitanStyle
\brief OfficeStyle base class for Office2007Style, Office2010Style and Office2016Style. Contains the general settings 
that are typical for all styles of Microsof office. The style available for Windows, Linux and MacOSX without any restrictions. 
*/
OfficeStyle::OfficeStyle(OfficeStylePrivate& d)
    : CommonStyle(d)
{
    initStyleResource();
}


/*!
Destructor of OfficeStyle class.
*/
OfficeStyle::~OfficeStyle()
{
}

/*!
Returns an accent color for Office style.
*/
QColor OfficeStyle::accentColor() const
{
    QTN_D(const OfficeStyle);
    return d.m_clrAccent;
}

/*!
Sets an accent color for Office style by color index.
*/
void OfficeStyle::setAccentColor(AccentColor index)
{
    QTN_D(OfficeStyle);
    if (d.m_curAccentColor != index)
    {
        d.beginUpdate();
        d.m_curAccentColor = index;
        d.m_clrAccent = d.accentIndexToColor(index);
        d.endUpdate();
    }
}

/*!
Returns an accent index color for Office style.
*/
OfficeStyle::AccentColor OfficeStyle::accentIndexColor() const
{
    QTN_D(const OfficeStyle);
    return d.m_curAccentColor;
}

/*!
Sets an accent color for Office style.
*/
void OfficeStyle::setAccentColor(const QColor& accentcolor)
{
    QTN_D(OfficeStyle);
    if (d.m_clrAccent != accentcolor)
    {
        d.beginUpdate();
        d.m_curAccentColor = static_cast<AccentColor>(-1);
        d.m_clrAccent = accentcolor;
        d.endUpdate();
    }
}

/*!
\property OfficeStyle::activeTabAccented
This property holds whether the painting of active tab with accent color is enabled. Accent color depends on the style or theme and can't be changed. Implemented for Office 2013 theme only.
*/
bool OfficeStyle::isActiveTabAccented() const
{
    QTN_D(const OfficeStyle);
    return d.m_isActiveTabAccented;
}

void OfficeStyle::setActiveTabAccented(bool accented)
{
    QTN_D(OfficeStyle);
    if (d.m_isActiveTabAccented == accented)
        return;
    d.beginUpdate();
    d.m_isActiveTabAccented = accented;
    d.endUpdate();
}

/*!
\property OfficeStyle::ignoreMDIWindowTitle
Sets the flag for drawing MDI window title with native style or with office/ribbon style.
*/ 
void OfficeStyle::setMDIWindowTitleIgnored(bool ignore)
{
    QTN_D(OfficeStyle);
    d.m_ignoreMDIWindowTitle = ignore;
}

/*!
Returs the flag for drawing MDI window title with native style or with office/ribbon style.
*/ 
bool OfficeStyle::isMDIWindowTitleIgnored() const
{
    QTN_D(const OfficeStyle);
    return d.m_ignoreMDIWindowTitle;
}

/*!
Returns the value of the popupDecoration flag.
*/
OfficeStyle::OfficePopupDecoration OfficeStyle::popupDecoration() const
{
    QTN_D(const OfficeStyle);
    return d.m_typePopupStyle;
}

/*!
Sets the style of the \a decoration for OfficePopupWindow. 
*/
void OfficeStyle::setPopupDecoration(OfficeStyle::OfficePopupDecoration decoration)
{
    QTN_D(OfficeStyle);
    d.m_typePopupStyle = decoration;
}

/*! \reimp */
void OfficeStyle::polish(QWidget* widget)
{
    QTN_D(OfficeStyle);
    CommonStyle::polish(widget);
    if (widget->inherits("QCheckBox")    ||
        widget->inherits("QRadioButton") ||
        widget->inherits("QToolButton")  ||
        widget->inherits("QComboBox")    ||
        widget->inherits("QLineEdit")    ||
        widget->inherits("QScrollBar")   ||
        widget->inherits("QGroupBox")    ||
        widget->inherits("QTabBar")      ||
        widget->inherits("QAbstractSpinBox") ||
        widget->inherits("QSlider")      ||
        widget->inherits("QTabWidget")   ||
        widget->inherits("QAbstractButton"))
    {
        widget->setAttribute(Qt::WA_Hover, true);
    }

    if (QListView* list = qobject_cast<QListView*>(widget))
        list->viewport()->setAttribute(Qt::WA_Hover, true);
    else if (QTreeView* tree = qobject_cast<QTreeView*>(widget))
        tree->viewport()->setAttribute(Qt::WA_Hover, true);
    else if (QTableView* table = qobject_cast<QTableView*>(widget))
        table->viewport()->setAttribute(Qt::WA_Hover, true);
    else if (QTabBar* tabBar = qobject_cast<QTabBar*>(widget))
        tabBar->setExpanding(false);

    if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
    {
        d.m_saveBrushBackground = mdiArea->background();
        mdiArea->setBackground(widget->palette().brush(QPalette::Dark));
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
        widget->setAttribute(Qt::WA_MacSmallSize, true);
    else if (widget->inherits("QTipLabel"))
        widget->setContentsMargins(3, 4, 4, 4);
}

/*! \reimp */
void OfficeStyle::polish(QPalette& palette)
{
    CommonStyle::polish(palette);
}

/*! \reimp */
void OfficeStyle::polish(QApplication* app)
{
    CommonStyle::polish(app);
}

/*! \reimp */
void OfficeStyle::unpolish(QWidget* widget)
{
    QTN_D(OfficeStyle);
    CommonStyle::unpolish(widget);

    if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
        widget->setAttribute(Qt::WA_MacSmallSize, false);

    if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
        mdiArea->setBackground(d.m_saveBrushBackground);
}

/*! \reimp */
QPixmap OfficeStyle::standardPixmap(StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const OfficeStyle);
    QPixmap px;
    switch (sp)
    {
        case SP_ArrowUp:
            px = d.standardPixmap(QStringLiteral("Image_ShadedArrowUp"), (opt && !(opt->state & QStyle::State_Enabled)) ? CommonStylePrivate::Gray : CommonStylePrivate::Black, widget);
            break;
        default:
            break;
    };
    if (px.isNull())
        px = CommonStyle::standardPixmap(sp, opt, widget);
    return px;
}

/*! \reimp */
QRect OfficeStyle::subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
{
    if (control == CC_TitleBar && widget && widget->inherits("QMdiSubWindow"))
        return QProxyStyle::subControlRect(QStyle::CC_TitleBar, option, subControl, widget);

    QRect rect = CommonStyle::subControlRect(control, option, subControl, widget);
    switch (control)
    {
        case CC_Slider:
            if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option))
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
                        const int grooveThickness = CommonStylePrivate::dpiScaled(7, widget);
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
        case CC_ComboBox:
            if (const QStyleOptionComboBox* cb = qstyleoption_cast<const QStyleOptionComboBox*>(option))
            {
                if (subControl == SC_ComboBoxArrow)
                {
                    QRect rect;
                    int x = cb->rect.x(), y = cb->rect.y(), wi = cb->rect.width(), he = cb->rect.height();
                    int bmarg = cb->frame ? CommonStylePrivate::dpiScaled(2, widget) : 0;
                    const int xpos = x + wi - bmarg - CommonStylePrivate::dpiScaled(16, widget);
                    int arrowButtonWidth = bmarg + CommonStylePrivate::dpiScaled(16, widget);
                    rect.setRect(xpos, y, arrowButtonWidth, he);
                    return visualRect(cb->direction, cb->rect, rect);
                }
            }
        default:
            break;
    }
    return rect;
}

/*! \reimp */
QSize OfficeStyle::sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    QSize sz = d.ribbonSizeFromContents(ct, opt, contentsSize, widget);
    if (sz.isEmpty())
    {
        sz = contentsSize;
        switch (ct)
        {
            case CT_PushButton:
                if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton *>(opt))
                {
                    sz = QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
                    int w = sz.width(),
                        h = sz.height();
                    int defwidth = 0;
                    if (btn->features & QStyleOptionButton::AutoDefaultButton)
                        defwidth = 2 * proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn, widget);
                    int minwidth = CommonStylePrivate::dpiScaled(75, widget);
                    int minheight = CommonStylePrivate::dpiScaled(23, widget);

                    if (w < minwidth + defwidth && !btn->text.isEmpty())
                        w = minwidth + defwidth;
                    if (h < minheight + defwidth)
                        h = minheight + defwidth;
                    sz = QSize(w, h);
                }
                break;
            case CT_MenuBarItem:
                if (!sz.isEmpty())
                    sz += QSize(OfficeStylePrivate::windowsItemHMargin * 5 + 1, 5);
                break;
            case CT_MenuItem:
            {
                if (const QStyleOptionMenuItem* mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) 
                {
                    int w = sz.width();
                    sz = QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);

                    if (mi->menuItemType == QStyleOptionMenuItem::Separator && !mi->text.isEmpty())
                    {
                        int w = 0;
                        QFontMetrics fm(mi->font);
                        QFont fontBold = mi->font;
                        fontBold.setBold(true);
                        QFontMetrics fmBold(fontBold);
                        w += qtn_horizontalAdvanceFont(fmBold, mi->text) + 6 + 6;
                        int h = mi->fontMetrics.height() + 6;
                        sz.setHeight(qMax(h, 22));
                        sz.setWidth(qMax(w, sz.width()));
                    }
                    else if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                    {
                        sz = QSize(10, OfficeStylePrivate::windowsSepHeight);
                    }
                    else if (mi->icon.isNull())
                    {
                        sz.setHeight(sz.height() - 2);
                        w -= 6;
                    }

                    if (mi->menuItemType != QStyleOptionMenuItem::Separator && !mi->icon.isNull()) 
                    {
                        int iconExtent = proxy()->pixelMetric(PM_SmallIconSize, opt, widget);
                        sz.setHeight(qMax(sz.height(), mi->icon.actualSize(QSize(iconExtent, iconExtent)).height() + 2 * OfficeStylePrivate::windowsItemFrame));
                    }
                    int maxpmw = mi->maxIconWidth;
                    int tabSpacing = 20;
                    if (mi->text.contains(QLatin1Char('\t')))
                        w += tabSpacing;
                    else if (mi->menuItemType == QStyleOptionMenuItem::SubMenu)
                        w += 2 * OfficeStylePrivate::windowsArrowHMargin;
                    else if (mi->menuItemType == QStyleOptionMenuItem::DefaultItem) 
                    {
                        // adjust the font and add the difference in size.
                        // it would be better if the font could be adjusted in the initStyleOption qmenu func!!
                        QFontMetrics fm(mi->font);
                        QFont fontBold = mi->font;
                        fontBold.setBold(true);
                        QFontMetrics fmBold(fontBold);
                        w += qtn_horizontalAdvanceFont(fmBold, mi->text) - qtn_horizontalAdvanceFont(fm, mi->text);
                    }
                    // Windows always shows a check column
                    int checkcol = qMax<int>(maxpmw, OfficeStylePrivate::windowsCheckMarkWidth); 
                    w += checkcol;
                    w += int(OfficeStylePrivate::windowsRightBorder) + sz.height();
                    sz.setWidth(w);
                }
                return sz;
            }
            default:
                sz = QSize();
                break;
        }
    }
    if (sz.isEmpty())
        return QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
    return sz;
}

/*! \reimp */
int OfficeStyle::styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const
{
    if (widget && widget->inherits("QMdiSubWindow"))
        return QProxyStyle::styleHint(hint, opt, widget, shret);
    switch (hint)
    {
        case SH_Menu_MouseTracking:
        case SH_ComboBox_ListMouseTracking:
            return 1;
        case SH_ToolTip_Mask:
            if (QStyleHintReturnMask* mask = qstyleoption_cast<QStyleHintReturnMask*>(shret))
            {
                mask->region = opt->rect;
                //left rounded corner
                mask->region -= QRect(opt->rect.left(), opt->rect.top(), 1, 1);
                //right rounded corner
                mask->region -= QRect(opt->rect.right(), opt->rect.top(), 1, 1);
                //right-bottom rounded corner
                mask->region -= QRect(opt->rect.right(), opt->rect.bottom(), 1, 1);
                //left-bottom rounded corner
                mask->region -= QRect(opt->rect.left(), opt->rect.bottom(), 1, 1);
                return 1;
            }
            break;
        default:
            break;
    }
    return CommonStyle::styleHint(hint, opt, widget, shret);
}
