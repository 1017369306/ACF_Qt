/****************************************************************************
**
** Qtitan Library by Developer Machines (AdobePhotoshop Style implementation for Qt.C++)
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
#include "QtnAdobePhotoshopStyle.h"
#include "QtnCommonStylePrivate.h"
#include "QtnStyleHelperPrivate.h"
#include "QtnOfficeStyle.h"

#include "QtnStyleOption.h"
#include <qdrawutil.h>
#include <QPainterPath>
#include <QAbstractItemView>
#include <QTableView>
#include <QMenuBar>
#include <QPixmapCache>
#include <QMenu>
#include <QDockWidget>
#include <QComboBox>
#include <QToolTip>
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif


QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE

static const char * const qtn_titlebar_context_help[] = {
    "10 10 3 1",
    "  c None",
    "# c #000000",
    "+ c #444444",
    "  +####+  ",
    " ###  ### ",
    " ##    ## ",
    "     +##+ ",
    "    +##   ",
    "    ##    ",
    "    ##    ",
    "          ",
    "    ##    ",
    "    ##    " };

static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

/* AdobePhotoshopStylePrivate */
class AdobePhotoshopStylePrivate : public CommonStylePrivate
{
public:
    QTN_DECLARE_EX_PUBLIC(AdobePhotoshopStyle)
public:
    AdobePhotoshopStylePrivate();
    virtual ~AdobePhotoshopStylePrivate();
    void init() override;
public:
    bool isQtitanStyle() const override { return true; }
    QFont getWidgetFont(const QString& className) const override;
    // PrimitiveElement
    virtual bool drawFrameWindow(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawPanelMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorItemCheck(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawPanelItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
    virtual bool drawFrameGroupBox(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorCheckBox(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorRadioButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorHeaderArrow(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawIndicatorBranch(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawFrameFocusRect(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawRibbonFrameGallery(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelTipLabel(const QStyleOption*, QPainter*, const QWidget*) const;
    // PE - ButtonTool
    virtual bool drawPanelButtonTool(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawPanelButtonCommand(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawFrameLineEdit(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    // ComplexControl
    virtual bool drawTitleBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawComboBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawSlider(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawSpinBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const;
    // ControlElement
    virtual bool drawMenuBarItem(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawMenuItem(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawToolBoxTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawRibbonGroupLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonGalleryItem(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonBackstageFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // CE - draw components ScrollBar
    virtual bool drawScrollBarAddSubLine(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawScrollBarAddSubPage(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawScrollBarSlider(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawTabBarTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawHeaderSection(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
    virtual bool drawDockWidgetTitle(const QStyleOption* opt, QPainter* p, const QWidget* w) const;
protected:
    virtual QColor buttonHighlightColor(const QPalette& pal, bool highlightDn, bool border = false, const QWidget* wd = Q_NULL) const;
    virtual QColor titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* wd) const;
protected:
    virtual void drawHighlightItemGallery(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
public:
    void updateTheme();
public:
    virtual QColor textGalleryItem(const QPalette& pal) const;
    QColor qtnButtonColor(const QPalette &pal) const
    {
        QColor buttonColor = pal.button().color();
        int val = qGray(buttonColor.rgb());
        buttonColor = buttonColor.lighter(100 + qMax(1, (180 - val) / 6));
        buttonColor.setHsv(buttonColor.hue(), static_cast<int>(buttonColor.saturation() * 0.75), buttonColor.value());
        return buttonColor;
    }
public:
    enum
    {
        windowsItemFrame = 2, // menu item frame width
        windowsSepHeight = 9, // separator item height
        windowsItemHMargin = 3, // menu item hor text margin
        windowsItemVMargin = 2, // menu item ver text margin
        windowsArrowHMargin = 6, // arrow horizontal margin
        windowsRightBorder = 15, // right border on windows
        windowsCheckMarkWidth = 12  // checkmarks width on windows
    };
    int m_heightMenuBar;
    AdobePhotoshopStyle::Theme m_theme;
private:
    Q_DISABLE_COPY(AdobePhotoshopStylePrivate)
};
QTITAN_END_NAMESPACE

/* AdobePhotoshopStylePrivate */
AdobePhotoshopStylePrivate::AdobePhotoshopStylePrivate()
    : CommonStylePrivate(), m_heightMenuBar(19), m_theme(AdobePhotoshopStyle::LightGray)
{
}

AdobePhotoshopStylePrivate::~AdobePhotoshopStylePrivate()
{
}

void AdobePhotoshopStylePrivate::init()
{
    CommonStylePrivate::init();
}

QFont AdobePhotoshopStylePrivate::getWidgetFont(const QString& className) const
{
    if (className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBar")) ||
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonStatusBar")) ||
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBackstageView")))
    {
        QFont font = QApplication::font("QMenuBar");
#ifdef Q_OS_WIN
        font.setFamily(QStringLiteral("Bahnschrift"));
        //        font.setFamily(QStringLiteral("Freestyle Script"));
#else
        font.setFamily(QStringLiteral("Lucida Grande"));
#endif
        int psize = font.pointSize();
        font.setPointSize(psize - 2);
        font.setBold(false);
        font.setItalic(false);
        return font;
    }
    return CommonStylePrivate::getWidgetFont(className);
}

static void qtn_adobe_draw_mdibutton(QPainter* painter, const QStyleOptionTitleBar* option, const QRect& tmp, bool hover, bool sunken)
{
    if (sunken)
        painter->fillRect(tmp.adjusted(1, 0, -1, -1), option->palette.light().color());
    else if (hover)
        painter->fillRect(tmp.adjusted(1, 0, -1, -1), option->palette.highlight().color());

    QColor mdiButtonBorderColor(option->palette.light().color());
    painter->setPen(QPen(mdiButtonBorderColor));
    const QLine lines[4] = 
    {
        QLine(tmp.left() + 2, tmp.bottom(), tmp.right() - 2, tmp.bottom()),
        QLine(tmp.left(),  tmp.top(), tmp.left(), tmp.bottom() - 2),
        QLine(tmp.right(), tmp.top(), tmp.right(), tmp.bottom() - 2)
    };
    painter->drawLines(lines, 3);
    const QPoint points[4] = 
    {
        QPoint(tmp.left() + 1, tmp.bottom() - 1),
        QPoint(tmp.right() - 1, tmp.bottom() - 1)
    };
    painter->drawPoints(points, 2);
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawFrameWindow(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (const QStyleOptionFrame* frm = qstyleoption_cast<const QStyleOptionFrame *>(option))
    {
        if (qtn_hasWindowTitleBar(widget))
        {
            return CommonStylePrivate::drawFrameWindow(option, painter, widget);
        }
        else
        {
            painter->save();
            QRect rect = frm->rect;
            painter->setPen(QPen(frm->palette.light().color(), 1));
            rect = rect.adjusted(0, 0, -1, -1);
            painter->drawRect(rect);

            painter->setPen(QPen(frm->palette.midlight(), 3));
            rect = rect.adjusted(2, 2, -2, -2);
            painter->drawRect(rect);

            painter->setPen(QPen(frm->palette.light().color(), 1));
            rect = rect.adjusted(2, 2, -2, -2);
            painter->drawRect(rect);
            painter->restore();
            return true;
        }
    }
    return false;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawPanelMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (CommonStylePrivate::drawPanelMenu(opt, p, w))
        return true;
    p->save();
    QColor menuBackground = opt->palette.base().color();
    QColor borderColor = opt->palette.window().color();
    p->setPen(borderColor);
    p->setBrush(menuBackground);
    p->drawRect(qtn_adjustRect(p->pen(), opt->rect));
    p->restore();
    return true;
}


/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    if (qobject_cast<const QMenu*>(w))
        return false;
    return drawIndicatorArrowDp(pe, option, painter, w);
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorItemCheck(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const AdobePhotoshopStyle);
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
            painter->drawPath(path.translated(rc.topLeft() - QPoint(1, 0)));
        }
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawPanelItemViewItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionViewItem* vopt = qstyleoption_cast<const QStyleOptionViewItem*>(opt))
#else
    if (const QStyleOptionViewItemV4* vopt = qstyleoption_cast<const QStyleOptionViewItemV4*>(opt))
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
                            clr = StyleHelper::colorMakeLighter(clr.rgba(), 0.6);
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
                        painter->drawPixmap(QRect(pixmapRect.topRight() - QPoint(frame - 1, 0), QSize(frame, pixmapRect.height())), pixmap,QRect(QPoint(pixmapRect.width() - frame, 0), QSize(frame, pixmapRect.height())));
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
void qtnDrawShadeLine(QPainter *p, int x1, int y1, int x2, int y2,
    const QPalette &pal, bool sunken, int lineWidth, int midLineWidth)
{
    if (Q_UNLIKELY(!p || lineWidth < 0 || midLineWidth < 0)) 
    {
        qWarning("qtnDrawShadeLine: Invalid parameters");
        return;
    }
    int tlw = lineWidth * 2 + midLineWidth;        // total line width
    QPen oldPen = p->pen();                        // save pen
    if (sunken)
        p->setPen(pal.light().color());
    else
        p->setPen(pal.dark().color());
    QPolygon a;
    int i;

    if (y1 == y2) // horizontal line
    {  
        int y = y1 - tlw / 2;
        for (i = 0; i<lineWidth; i++) 
        { 
            a.setPoints(3, x1 + i, y + tlw - i - 1,
                x2 - i, y + tlw - i - 1,
                x2 - i, y + i + 1);
            p->drawPolyline(a);
        }

    }
    else if (x1 == x2)  // vertical line
    {
        int x = x1 - tlw / 2;
        for (i = 0; i<lineWidth; i++) 
        { 
            a.setPoints(3, x + lineWidth, y2 - i,
                x + tlw - i - 1, y2 - i,
                x + tlw - i - 1, y1 + lineWidth);
            p->drawPolyline(a);
        }
    }
    p->setPen(oldPen);
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawFrameGroupBox(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QtStyleOptionFrame* frame = qstyleoption_cast<const QtStyleOptionFrame *>(opt))
    {
        QPen oldPen = p->pen();
        p->setPen(frame->palette.light().color());

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
                p->drawPolyline(a);
            }
        }
        else 
        {
            int x = frame->rect.x(), y = frame->rect.y(), w = frame->rect.width(), h = frame->rect.height();
            int x1 = x, y1 = y;
            p->drawRect(x1, y1, w - 2, h - 2);
        }
        p->setPen(oldPen);
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawTitleBar(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    painter->save();
    if (const QStyleOptionTitleBar* titleBar = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
    {
        int buttonMargin = 5;
        bool isActive = (titleBar->titleBarState & QStyle::State_Active);
        QPalette palette = option->palette;
        QColor textColor(titleBar->palette.text().color());
        {
            // Fill title bar background
            QColor windowColor = QColor(palette.midlight().color());
            QColor frameColor = titleBarFrameColor(option->palette, isActive, widget);
            drawfillTitleBar(option, painter, widget, frameColor, windowColor);

            // Frame and rounded corners
            //painter->setPen(titleBarFrameBorder);
            //drawLineDPI(*painter, fullRect, true, true, true, false);
#if 0
            const int lineWidth = p.proxy()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, option, widget);
            QRect fullRect = titleBar->rect;
            QColor titleBarHighlight(palette.midlight().color());
            // top outline
            painter->drawLine(fullRect.left() + 5, fullRect.top(), fullRect.right() - 5, fullRect.top());
            painter->drawLine(fullRect.left(), fullRect.top() + 4, fullRect.left(), fullRect.bottom());
            const QPoint points[5] = {
                QPoint(fullRect.left() + 4, fullRect.top() + 1),
                QPoint(fullRect.left() + 3, fullRect.top() + 1),
                QPoint(fullRect.left() + 2, fullRect.top() + 2),
                QPoint(fullRect.left() + 1, fullRect.top() + 3),
                QPoint(fullRect.left() + 1, fullRect.top() + 4)
            };
            painter->drawPoints(points, 5);

            painter->drawLine(fullRect.right(), fullRect.top() + 4, fullRect.right(), fullRect.bottom());
            const QPoint points2[5] = {
                QPoint(fullRect.right() - 3, fullRect.top() + 1),
                QPoint(fullRect.right() - 4, fullRect.top() + 1),
                QPoint(fullRect.right() - 2, fullRect.top() + 2),
                QPoint(fullRect.right() - 1, fullRect.top() + 3),
                QPoint(fullRect.right() - 1, fullRect.top() + 4)
            };
            painter->drawPoints(points2, 5);

            // draw bottomline
            painter->drawLine(fullRect.right() - lineWidth + 1, fullRect.bottom(), fullRect.left() + lineWidth - 1, fullRect.bottom());
#endif
        }
        // draw title
        QRect textRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarLabel, widget);
        // Note workspace also does elliding but it does not use the correct font
        QString title = painter->fontMetrics().elidedText(titleBar->text, Qt::ElideRight, textRect.width() - 14);
        painter->setPen(textColor);
        painter->drawText(textRect, title, QTextOption(Qt::Alignment(Qt::AlignHCenter | Qt::AlignVCenter)));

        QBrush brushLightButton = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"), QPalette::Active, QPalette::Light);
        QBrush brushHighlightButton = brushSubControl(QStringLiteral("QMainWindow"), QStringLiteral("TitleBarCloseButton"), QPalette::Active, QPalette::Highlight);
        // min button
        if ((titleBar->subControls & QStyle::SC_TitleBarMinButton) && (titleBar->titleBarFlags & Qt::WindowMinimizeButtonHint) &&
            !(titleBar->titleBarState& Qt::WindowMinimized))
        {
            QRect minButtonRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarMinButton, widget);
            if (minButtonRect.isValid())
            {
                bool hover = (titleBar->activeSubControls & QStyle::SC_TitleBarMinButton) && (titleBar->state & QStyle::State_MouseOver);
                bool sunken = (titleBar->activeSubControls & QStyle::SC_TitleBarMinButton) && (titleBar->state & QStyle::State_Sunken);
                QStyleOptionTitleBar optButton = *titleBar;
                QPalette_setBrush(optButton.palette, QPalette::Light, brushLightButton);
                QPalette_setBrush(optButton.palette, QPalette::Highlight, brushHighlightButton);
                qtn_adobe_draw_mdibutton(painter, &optButton, minButtonRect, hover, sunken);

                QRect minButtonIconRect = minButtonRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);
                painter->setPen(textColor);
                painter->drawLine(minButtonIconRect.center().x() - 2, minButtonIconRect.center().y() + 3,
                    minButtonIconRect.center().x() + 3, minButtonIconRect.center().y() + 3);
                painter->drawLine(minButtonIconRect.center().x() - 2, minButtonIconRect.center().y() + 4,
                    minButtonIconRect.center().x() + 3, minButtonIconRect.center().y() + 4);
                painter->setPen(textColor);
                painter->drawLine(minButtonIconRect.center().x() - 3, minButtonIconRect.center().y() + 3,
                    minButtonIconRect.center().x() - 3, minButtonIconRect.center().y() + 4);
                painter->drawLine(minButtonIconRect.center().x() + 4, minButtonIconRect.center().y() + 3,
                    minButtonIconRect.center().x() + 4, minButtonIconRect.center().y() + 4);
            }
        }

        // max button
        if ((titleBar->subControls & QStyle::SC_TitleBarMaxButton) && (titleBar->titleBarFlags & Qt::WindowMaximizeButtonHint) &&
            !(titleBar->titleBarState & Qt::WindowMaximized))
        {
            QRect maxButtonRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarMaxButton, widget);
            if (maxButtonRect.isValid())
            {
                bool hover = (titleBar->activeSubControls & QStyle::SC_TitleBarMaxButton) && (titleBar->state & QStyle::State_MouseOver);
                bool sunken = (titleBar->activeSubControls & QStyle::SC_TitleBarMaxButton) && (titleBar->state & QStyle::State_Sunken);
                QStyleOptionTitleBar optButton = *titleBar;
                QPalette_setBrush(optButton.palette, QPalette::Light, brushLightButton);
                QPalette_setBrush(optButton.palette, QPalette::Highlight, brushHighlightButton);
                qtn_adobe_draw_mdibutton(painter, &optButton, maxButtonRect, hover, sunken);

                QRect maxRect = maxButtonRect;
                maxRect.setWidth(maxRect.height());

                QRect maxButtonIconRect = maxRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);
                maxButtonIconRect.translate(maxButtonRect.width() / 2 - maxRect.width() / 2, 0);

                painter->setPen(textColor);
                painter->drawRect(maxButtonIconRect.adjusted(0, 0, -1, -1));
                painter->drawLine(maxButtonIconRect.left() + 1, maxButtonIconRect.top() + 1, maxButtonIconRect.right() - 1, maxButtonIconRect.top() + 1);
                painter->setPen(textColor);
                const QPoint points[4] = {
                    maxButtonIconRect.topLeft(),
                    maxButtonIconRect.topRight(),
                    maxButtonIconRect.bottomLeft(),
                    maxButtonIconRect.bottomRight()
                };
                painter->drawPoints(points, 4);
            }
        }

        // close button
        if ((titleBar->subControls & QStyle::SC_TitleBarCloseButton) && (titleBar->titleBarFlags & Qt::WindowSystemMenuHint))
        {
            const bool closeHint = titleBar->titleBarFlags & Qt::WindowCloseButtonHint;
            QRect closeButtonRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarCloseButton, widget);
            if (closeButtonRect.isValid())
            {
                bool hover = (titleBar->activeSubControls & QStyle::SC_TitleBarCloseButton) && (titleBar->state & QStyle::State_MouseOver);
                bool sunken = (titleBar->activeSubControls & QStyle::SC_TitleBarCloseButton) && (titleBar->state & QStyle::State_Sunken);
                QStyleOptionTitleBar optButton = *titleBar;
                QPalette_setBrush(optButton.palette, QPalette::Light, brushLightButton);
                if (!closeHint)
                {
                    textColor = QColor(titleBar->palette.color(QPalette::Disabled, QPalette::Text));
                    hover = false;
                }
                else
                    QPalette_setBrush(optButton.palette, QPalette::Highlight, brushHighlightButton);
                qtn_adobe_draw_mdibutton(painter, &optButton, closeButtonRect, hover, sunken);

                QRect closeRect = closeButtonRect;
                closeRect.setWidth(closeRect.height());

                QRect closeIconRect = closeRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);
                closeIconRect.translate(closeButtonRect.width() / 2 - closeRect.width() / 2, 0);

                painter->setPen(textColor);
                const QLine lines[4] = {
                    QLine(closeIconRect.left() + 1, closeIconRect.top(),
                    closeIconRect.right(), closeIconRect.bottom() - 1),
                    QLine(closeIconRect.left(), closeIconRect.top() + 1,
                    closeIconRect.right() - 1, closeIconRect.bottom()),
                    QLine(closeIconRect.right() - 1, closeIconRect.top(),
                    closeIconRect.left(), closeIconRect.bottom() - 1),
                    QLine(closeIconRect.right(), closeIconRect.top() + 1,
                    closeIconRect.left() + 1, closeIconRect.bottom())
                };
                painter->drawLines(lines, 4);

                const QPoint points[4] = {
                    closeIconRect.topLeft(),
                    closeIconRect.topRight(),
                    closeIconRect.bottomLeft(),
                    closeIconRect.bottomRight()
                };
                painter->drawPoints(points, 4);

                painter->setPen(textColor);
                painter->drawLine(closeIconRect.left() + 1, closeIconRect.top() + 1, closeIconRect.right() - 1, closeIconRect.bottom() - 1);
                painter->drawLine(closeIconRect.left() + 1, closeIconRect.bottom() - 1, closeIconRect.right() - 1, closeIconRect.top() + 1);
            }
        }

        // normalize button
        if ((titleBar->subControls & QStyle::SC_TitleBarNormalButton) && (((titleBar->titleBarFlags & Qt::WindowMinimizeButtonHint) && (titleBar->titleBarState & Qt::WindowMinimized)) ||
            ((titleBar->titleBarFlags & Qt::WindowMaximizeButtonHint) && (titleBar->titleBarState & Qt::WindowMaximized))))
        {
            QRect normalButtonRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarNormalButton, widget);
            if (normalButtonRect.isValid())
            {
                buttonMargin = 4;
                bool hover = (titleBar->activeSubControls & QStyle::SC_TitleBarNormalButton) && (titleBar->state & QStyle::State_MouseOver);
                bool sunken = (titleBar->activeSubControls & QStyle::SC_TitleBarNormalButton) && (titleBar->state & QStyle::State_Sunken);
                QStyleOptionTitleBar optButton = *titleBar;
                QPalette_setBrush(optButton.palette, QPalette::Light, brushLightButton);
                QPalette_setBrush(optButton.palette, QPalette::Highlight, brushHighlightButton);
                qtn_adobe_draw_mdibutton(painter, &optButton, normalButtonRect, hover, sunken);

                QRect normalRect = normalButtonRect;
                normalRect.setWidth(normalRect.height());
                QRect normalButtonIconRect = normalRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);
                normalButtonIconRect.translate((normalRect.width() / 2 - normalButtonIconRect.width() / 2) + 3, 0);

                QRect frontWindowRect = normalButtonIconRect.adjusted(0, 3, -3, 0);
                painter->setPen(textColor);
                painter->drawRect(frontWindowRect.adjusted(0, 0, -1, -1));
                painter->drawLine(frontWindowRect.left() + 1, frontWindowRect.top() + 1,
                    frontWindowRect.right() - 1, frontWindowRect.top() + 1);
                painter->setPen(textColor);
                const QPoint points[4] = {
                    frontWindowRect.topLeft(),
                    frontWindowRect.topRight(),
                    frontWindowRect.bottomLeft(),
                    frontWindowRect.bottomRight()
                };
                painter->drawPoints(points, 4);

                QRect backWindowRect = normalButtonIconRect.adjusted(3, 0, 0, -3);
                QRegion clipRegion = backWindowRect;
                clipRegion -= frontWindowRect;
                painter->save();
                painter->setClipRegion(clipRegion);
                painter->setPen(textColor);
                painter->drawRect(backWindowRect.adjusted(0, 0, -1, -1));
                painter->drawLine(backWindowRect.left() + 1, backWindowRect.top() + 1,
                    backWindowRect.right() - 1, backWindowRect.top() + 1);
                painter->setPen(textColor);
                const QPoint points2[4] = {
                    backWindowRect.topLeft(),
                    backWindowRect.topRight(),
                    backWindowRect.bottomLeft(),
                    backWindowRect.bottomRight()
                };
                painter->drawPoints(points2, 4);
                painter->restore();
            }
        }

        // context help button
        if (titleBar->subControls & QStyle::SC_TitleBarContextHelpButton && (titleBar->titleBarFlags & Qt::WindowContextHelpButtonHint)) 
        {
            QRect contextHelpButtonRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarContextHelpButton, widget);
            if (contextHelpButtonRect.isValid()) 
            {
                bool hover = (titleBar->activeSubControls & QStyle::SC_TitleBarContextHelpButton) && (titleBar->state & QStyle::State_MouseOver);
                bool sunken = (titleBar->activeSubControls & QStyle::SC_TitleBarContextHelpButton) && (titleBar->state & QStyle::State_Sunken);
                QStyleOptionTitleBar optButton = *titleBar;
                QPalette_setBrush(optButton.palette, QPalette::Light, brushLightButton);
                QPalette_setBrush(optButton.palette, QPalette::Highlight, brushHighlightButton);
                qtn_adobe_draw_mdibutton(painter, &optButton, contextHelpButtonRect, hover, sunken);

                QRect contextHelpRect = contextHelpButtonRect;
                contextHelpRect.setWidth(contextHelpRect.height());
                QRect contextHelpButtonIconRect = contextHelpRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);
                contextHelpButtonIconRect.translate((contextHelpRect.width() / 2 - contextHelpButtonIconRect.width() / 2) + 1, 0);

                QImage image(qtn_titlebar_context_help);
                QColor alpha = textColor;
                alpha.setAlpha(128);
                image.setColor(1, textColor.rgba());
                image.setColor(2, alpha.rgba());
                painter->setRenderHint(QPainter::SmoothPixmapTransform);
                painter->drawImage(contextHelpButtonIconRect, image);
            }
        }
/*
        // shade button
        if (titleBar->subControls & QStyle::SC_TitleBarShadeButton && (titleBar->titleBarFlags & Qt::WindowShadeButtonHint)) 
        {
            QRect shadeButtonRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarShadeButton, widget);
            if (shadeButtonRect.isValid()) 
            {
                bool hover = (titleBar->activeSubControls & QStyle::SC_TitleBarShadeButton) && (titleBar->state & QStyle::State_MouseOver);
                bool sunken = (titleBar->activeSubControls & QStyle::SC_TitleBarShadeButton) && (titleBar->state & QStyle::State_Sunken);
                qt_fusion_draw_mdibutton(painter, titleBar, shadeButtonRect, hover, sunken);
                QPixmap arrow = colorizedImage(QStringLiteral(":/qt-project.org/styles/commonstyle/images/fusion_arrow.png"), textColor);
                painter->drawPixmap(shadeButtonRect.adjusted(5, 7, -5, -7), arrow);
            }
        }
       
        // unshade button
        if (titleBar->subControls & QStyle::SC_TitleBarUnshadeButton && (titleBar->titleBarFlags & Qt::WindowShadeButtonHint))
        {
            QRect unshadeButtonRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, SC_TitleBarUnshadeButton, widget);
            if (unshadeButtonRect.isValid()) 
            {
                bool hover = (titleBar->activeSubControls & SC_TitleBarUnshadeButton) && (titleBar->state & QStyle::State_MouseOver);
                bool sunken = (titleBar->activeSubControls & SC_TitleBarUnshadeButton) && (titleBar->state & QStyle::State_Sunken);
                qt_fusion_draw_mdibutton(painter, titleBar, unshadeButtonRect, hover, sunken);
                QPixmap arrow = colorizedImage(QStringLiteral(":/qt-project.org/styles/commonstyle/images/fusion_arrow.png"), textColor, 180);
                painter->drawPixmap(unshadeButtonRect.adjusted(5, 7, -5, -7), arrow);
            }
        }
*/
        if ((titleBar->subControls & QStyle::SC_TitleBarSysMenu) && (titleBar->titleBarFlags & Qt::WindowSystemMenuHint))
        {
            QRect iconRect = p.proxy()->subControlRect(QStyle::CC_TitleBar, titleBar, QStyle::SC_TitleBarSysMenu, widget);
            if (iconRect.isValid())
            {
                if (!titleBar->icon.isNull())
                {
                    titleBar->icon.paint(painter, iconRect);
                }
                else
                {
                    QStyleOption tool = *titleBar;
                    QPixmap pm = p.proxy()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, widget).pixmap(16, 16);
                    tool.rect = iconRect;
                    painter->save();
                    p.proxy()->drawItemPixmap(painter, iconRect, Qt::AlignCenter, pm);
                    painter->restore();
                }
            }
        }
    }
    painter->restore();
    return true;
}

/*! \internal */
inline QPixmap qtnStyleCachePixmap(const QSize &size)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    const qreal pixelRatio = qApp->devicePixelRatio();
    QPixmap cachePixmap = QPixmap(size * pixelRatio);
    cachePixmap.setDevicePixelRatio(pixelRatio);
#else
    QPixmap cachePixmap = QPixmap(size);
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return cachePixmap;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawComboBox(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    painter->save();
    if (const QStyleOptionComboBox* comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) 
    {
        QColor outline = option->palette.mid().color();
        const bool hasFocus = option->state & QStyle::State_HasFocus /*&& option->state & QStyle::State_KeyboardFocusChange*/;
        const bool sunken = comboBox->state & QStyle::State_On; // play dead, if combobox has no items
        QPixmap cache;
        {
            cache = qtnStyleCachePixmap(comboBox->rect.size());
            cache.fill(Qt::transparent);
            QPainter cachePainter(&cache);
            QRect pixmapRect(0, 0, comboBox->rect.width(), comboBox->rect.height());
            QStyleOptionComboBox comboBoxCopy = *comboBox;
            comboBoxCopy.rect = pixmapRect;

            QRect rect = pixmapRect;
            QRect downArrowRect = p.proxy()->subControlRect(QStyle::CC_ComboBox, &comboBoxCopy, QStyle::SC_ComboBoxArrow, widget);
            // Draw a line edit
            if (comboBox->editable) 
            {
                QStyleOptionFrame  buttonOption;
                buttonOption.QStyleOption::operator=(*comboBox);
                buttonOption.rect = rect;
                buttonOption.state = (comboBox->state & (QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_HasFocus))
                    | QStyle::State_KeyboardFocusChange; // Always show hig

                if (sunken) 
                {
                    buttonOption.state |= QStyle::State_Sunken;
                    buttonOption.state &= ~QStyle::State_MouseOver;
                }

                p.proxy()->drawPrimitive(QStyle::PE_FrameLineEdit, &buttonOption, &cachePainter, widget);

                // Draw button clipped
                cachePainter.save();
                cachePainter.setClipRect(downArrowRect.adjusted(0, 0, 1, 0));
                buttonOption.rect.setLeft(comboBox->direction == Qt::LeftToRight ? downArrowRect.left() - 6 : downArrowRect.right() + 6);
                p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &buttonOption, &cachePainter, widget);
                cachePainter.restore();
                cachePainter.setPen(QPen(hasFocus ? option->palette.highlight() : outline.lighter(110), 1));
                
                if (!sunken) 
                {
                    int borderSize = 1;
                    if (comboBox->direction == Qt::RightToLeft) 
                        cachePainter.drawLine(QPoint(downArrowRect.right() - 1, downArrowRect.top() + borderSize), QPoint(downArrowRect.right() - 1, downArrowRect.bottom() - borderSize));
                    else
                        cachePainter.drawLine(QPoint(downArrowRect.left(), downArrowRect.top() + borderSize), QPoint(downArrowRect.left(), downArrowRect.bottom() - borderSize));
                }
                else 
                {
                    if (comboBox->direction == Qt::RightToLeft) 
                        cachePainter.drawLine(QPoint(downArrowRect.right(), downArrowRect.top() + 2), QPoint(downArrowRect.right(), downArrowRect.bottom() - 2));
                    else
                        cachePainter.drawLine(QPoint(downArrowRect.left(), downArrowRect.top() + 2), QPoint(downArrowRect.left(), downArrowRect.bottom() - 2));
                }
            }
            else 
            {
                QStyleOptionButton buttonOption;
                buttonOption.QStyleOption::operator=(*comboBox);
                buttonOption.rect = rect;
                buttonOption.state = comboBox->state & (QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_HasFocus | QStyle::State_KeyboardFocusChange);
                if (sunken) {
                    buttonOption.state |= QStyle::State_Sunken;
                    buttonOption.state &= ~QStyle::State_MouseOver;
                }
                p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &buttonOption, &cachePainter, widget);
            }
            if (comboBox->subControls & QStyle::SC_ComboBoxArrow) 
            {
                QStyleOption opt;
                opt.QStyleOption::operator=(*comboBox);
                QPoint pt = downArrowRect.center();
                int size = CommonStylePrivate::dpiScaled(10, widget);
                QRect rc(QPoint(pt.x() - size / 2, pt.y() - size / 2), QSize(size, size));
                opt.rect = rc;

                drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, &opt, &cachePainter, widget);
            }
            cachePainter.end();
        }
        painter->drawPixmap(comboBox->rect.topLeft(), cache);
    }
    painter->restore();
    return true;
}

/*! \internal */
enum QtnSliderDirection { SlUp, SlDown, SlLeft, SlRight };
bool AdobePhotoshopStylePrivate::drawSlider(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const AdobePhotoshopStyle);
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
                    QColor clrFill = opt->palette.color(QPalette::Button);

                    if (enabled && (pressetHt || selected))
                    {
                        QColor clrAccentLight = StyleHelper::colorMakePale(clrFill.rgb(), .9);
                        clrFill = clrAccentLight;
                        QColor clrHighlightDn = StyleHelper::colorMakeDarker(clrLine.rgb(), .14);
                        clrLine = clrHighlightDn;
                    }

                    handle = qtn_adjustRect(painter->pen(), handle);
                    painter->fillRect(handle, clrFill);
                    painter->setPen(clrLine);
                    painter->drawRect(handle);
                }
            }
        }
        return true;
    }
    else if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider *>(opt))
    {
        QRect groove = p.proxy()->subControlRect(QStyle::CC_Slider, slider, QStyle::SC_SliderGroove, widget);
        QRect handle = p.proxy()->subControlRect(QStyle::CC_Slider, slider, QStyle::SC_SliderHandle, widget);

        const bool horizontal = slider->orientation == Qt::Horizontal;
        const bool ticksAbove = slider->tickPosition & QSlider::TicksAbove;
        const bool ticksBelow = slider->tickPosition & QSlider::TicksBelow;

        int thickness = p.proxy()->pixelMetric(QStyle::PM_SliderControlThickness, slider, widget);
        int len = p.proxy()->pixelMetric(QStyle::PM_SliderLength, slider, widget);

        if ((slider->subControls & QStyle::SC_SliderGroove) && groove.isValid())
        {
            int mid = thickness / 2;
            if (ticksAbove)
                mid += len / 8;
            if (ticksBelow)
                mid -= len / 8;
            painter->save();
            painter->setPen(slider->palette.shadow().color());
            painter->setBrush(slider->palette.shadow().color());
            if (horizontal)
            {
                if ((ticksAbove && ticksBelow) || (!ticksAbove && !ticksBelow))
                    painter->drawRect(groove.x(), groove.y() - mid + 2, groove.width() - 1, 2);
                else if (ticksAbove)
                    painter->drawRect(groove.x(), groove.y() - handle.height() / 2 + 2, groove.width() - 1, 2);
                else if (ticksBelow)
                    painter->drawRect(groove.x(), groove.y() + handle.height() / 2 + 2, groove.width() - 1, 2);
            }
            else 
            {
                if ((ticksAbove && ticksBelow) || (!ticksAbove && !ticksBelow))
                    painter->drawRect(groove.x() + 2, groove.y(), 2, groove.height() - 1);
                else if (ticksAbove)
                    painter->drawRect(groove.x() - handle.height() / 2 + 2, groove.y(), 2, groove.height() - 1);
                else if (ticksBelow)
                    painter->drawRect(groove.x() + handle.height() / 2 + 2, groove.y(), 2, groove.height() - 1);
            }
            painter->restore();
        }

        if (slider->subControls & QStyle::SC_SliderTickmarks)
        {
            painter->setPen(slider->palette.shadow().color());
            int tickSize = p.proxy()->pixelMetric(QStyle::PM_SliderTickmarkOffset, opt, widget);
            int available = p.proxy()->pixelMetric(QStyle::PM_SliderSpaceAvailable, slider, widget);
            int interval = slider->tickInterval;
            if (interval <= 0) 
            {
                interval = slider->singleStep;
                if (QStyle::sliderPositionFromValue(slider->minimum, slider->maximum, interval, available) - QStyle::sliderPositionFromValue(slider->minimum, slider->maximum, 0, available) < 3)
                    interval = slider->pageStep;
            }
            if (interval <= 0)
                interval = 1;

            int v = slider->minimum;
            int len = p.proxy()->pixelMetric(QStyle::PM_SliderLength, slider, widget);
            while (v <= slider->maximum + 1) 
            {
                if (v == slider->maximum + 1 && interval == 1)
                    break;
                const int v_ = qMin(v, slider->maximum);
                int pos = p.sliderPositionFromValue(slider->minimum, slider->maximum, v_, (horizontal ? slider->rect.width() : slider->rect.height()) - len, slider->upsideDown) + len / 2;
                int extra = 2 - ((v_ == slider->minimum || v_ == slider->maximum) ? 1 : 0);

                if (horizontal) 
                {
                    if (ticksAbove) 
                        painter->drawLine(pos, slider->rect.top() + extra, pos, slider->rect.top() + tickSize);
                    if (ticksBelow) 
                        painter->drawLine(pos, slider->rect.bottom() - extra, pos, slider->rect.bottom() - tickSize);
                }
                else 
                {
                    if (ticksAbove)
                        painter->drawLine(slider->rect.left() + extra, pos, slider->rect.left() + tickSize, pos);
                    if (ticksBelow)
                        painter->drawLine(slider->rect.right() - extra, pos, slider->rect.right() - tickSize, pos);
                }
                // in the case where maximum is max int
                int nextInterval = v + interval;
                if (nextInterval < v)
                    break;
                v = nextInterval;
            }
        }

        if (slider->subControls & QStyle::SC_SliderHandle)
        {
            const QColor c0 = slider->palette.dark().color();
            const QColor c1 = slider->palette.dark().color();
            const QColor c3 = slider->palette.dark().color();
            const QColor c4 = slider->palette.dark().color();
            const bool selected = slider->activeSubControls & QStyle::SC_SliderHandle && opt->state & QStyle::State_MouseOver;
            const bool pressetHt = opt->state & QStyle::State_Sunken;

            QBrush handleBrush;
            if (slider->state & QStyle::State_Enabled)
                handleBrush = selected || pressetHt ? StyleHelper::colorMakePale(slider->palette.color(QPalette::Button).rgb(), .9) :
                    slider->palette.color(QPalette::Button);
            else
                handleBrush = QBrush(slider->palette.color(QPalette::Button), Qt::Dense4Pattern);

            int x = handle.x(), y = handle.y(), wi = handle.width(), he = handle.height();
            int x1 = x;
            int x2 = x + wi/* - 1*/;
            int y1 = y;
            int y2 = y + he/* - 1*/;

            if (slider->state & QStyle::State_HasFocus)
            {
/*
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*slider);
                painter->setPen(slider->palette.shadow().color());
                fropt.rect = p.subElementRect(QStyle::SE_SliderFocusRect, slider, widget);
                p.proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, painter, widget);
*/
            }

            if ((ticksAbove && ticksBelow) || (!ticksAbove && !ticksBelow)) 
            {
                painter->setPen(slider->palette.dark().color());
                painter->setBrush(handleBrush);
                painter->drawRect(QRect(x, y, wi, he).adjusted(1, 1, -1, -1));
                return true;
            }

            QtnSliderDirection dir;
            if (horizontal)
                dir = ticksAbove ? SlUp : SlDown;
            else
                dir = ticksAbove ? SlLeft : SlRight;

            QPolygon a;
            int d = 0;
            switch (dir) 
            {
                case SlUp:
                    y1 = y1 + wi / 2;
                    d = (wi + 1) / 2 - 1;
                    a.setPoints(5, x1, y1, x1, y2, x2, y2, x2, y1, x1 + d, y1 - d);
                    break;
                case SlDown:
                    y2 = y2 - wi / 2;
                    d = (wi + 1) / 2 - 1;
                    a.setPoints(5, x1, y1, x1, y2, x1 + d, y2 + d, x2, y2, x2, y1);
                    break;
                case SlLeft:
                    d = (he + 1) / 2 - 1;
                    x1 = x1 + he / 2;
                    a.setPoints(5, x1, y1, x1 - d, y1 + d, x1, y2, x2, y2, x2, y1);
                    break;
                case SlRight:
                    d = (he + 1) / 2 - 1;
                    x2 = x2 - he / 2;
                    a.setPoints(5, x1, y1, x1, y2, x2, y2, x2 + d, y1 + d, x2, y1);
                    break;
            }

            QBrush oldBrush = painter->brush();
            painter->setPen(Qt::NoPen);
            painter->setBrush(handleBrush);
            Qt::BGMode oldMode = painter->backgroundMode();
            painter->setBackgroundMode(Qt::OpaqueMode);
            painter->drawRect(x1, y1, x2 - x1, y2 - y1);
            painter->drawPolygon(a);
            painter->setBrush(oldBrush);
            painter->setBackgroundMode(oldMode);

            if (dir != SlUp) 
            {
                painter->setPen(c4);
                painter->drawLine(x1, y1, x2, y1);
                painter->setPen(c3);
                painter->drawLine(x1, y1 + 1, x2, y1 + 1);
            }
            if (dir != SlLeft) 
            {
                painter->setPen(c3);
                painter->drawLine(x1 + 1, y1 + 1, x1 + 1, y2);
                painter->setPen(c4);
                painter->drawLine(x1, y1, x1, y2);
            }
            if (dir != SlRight) 
            {
                painter->setPen(c0);
                painter->drawLine(x2, y1, x2, y2);
                painter->setPen(c1);
                painter->drawLine(x2 - 1, y1 + 1, x2 - 1, y2 - 1);
            }
            if (dir != SlDown) 
            {
                painter->setPen(c0);
                painter->drawLine(x1, y2, x2, y2);
                painter->setPen(c1);
                painter->drawLine(x1 + 1, y2 - 1, x2 - 1, y2 - 1);
            }

            switch (dir) 
            {
                case SlUp:
                    painter->setPen(c4);
                    painter->drawLine(x1, y1, x1 + d, y1 - d);
                    painter->setPen(c0);
                    d = wi - d - 1;
                    painter->drawLine(x2, y1, x2 - d, y1 - d);
                    d--;
                    painter->setPen(c3);
                    painter->drawLine(x1 + 1, y1, x1 + 1 + d, y1 - d);
                    painter->setPen(c1);
                    painter->drawLine(x2 - 1, y1, x2 - 1 - d, y1 - d);
                    break;
                case SlDown:
                    painter->setPen(c4);
                    painter->drawLine(x1, y2, x1 + d, y2 + d);
                    painter->setPen(c0);
                    d = wi - d - 1;
                    painter->drawLine(x2, y2, x2 - d, y2 + d);
                    d--;
                    painter->setPen(c3);
                    painter->drawLine(x1 + 1, y2, x1 + 1 + d, y2 + d);
                    painter->setPen(c1);
                    painter->drawLine(x2 - 1, y2, x2 - 1 - d, y2 + d);
                    break;
                case SlLeft:
                    painter->setPen(c4);
                    painter->drawLine(x1, y1, x1 - d, y1 + d);
                    painter->setPen(c0);
                    d = he - d - 1;
                    painter->drawLine(x1, y2, x1 - d, y2 - d);
                    d--;
                    painter->setPen(c3);
                    painter->drawLine(x1, y1 + 1, x1 - d, y1 + 1 + d);
                    painter->setPen(c1);
                    painter->drawLine(x1, y2 - 1, x1 - d, y2 - 1 - d);
                    break;
                case SlRight:
                    painter->setPen(c4);
                    painter->drawLine(x2, y1, x2 + d, y1 + d);
                    painter->setPen(c0);
                    d = he - d - 1;
                    painter->drawLine(x2, y2, x2 + d, y2 - d);
                    d--;
                    painter->setPen(c3);
                    painter->drawLine(x2, y1 + 1, x2 + d, y1 + 1 + d);
                    painter->setPen(c1);
                    painter->drawLine(x2, y2 - 1, x2 + d, y2 - 1 - d);
                    break;
            }
        }
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawSpinBox(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const AdobePhotoshopStyle);
    if (const QStyleOptionSpinBox* spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option))
    {
        QColor outline = option->palette.mid().color();
        QPixmap cache = qtnStyleCachePixmap(spinBox->rect.size());
        cache.fill(Qt::transparent);

        QRect pixmapRect(0, 0, spinBox->rect.width(), spinBox->rect.height());
        QRect rect = pixmapRect;
        QRect r = rect.adjusted(0, 1, 0, -1);
        QPainter cachePainter(&cache);
        QColor arrowColor = spinBox->palette.windowText().color();
        arrowColor.setAlpha(220);

        bool isEnabled = (spinBox->state & QStyle::State_Enabled);
        bool hover = isEnabled && (spinBox->state & QStyle::State_MouseOver);
        bool sunken = (spinBox->state & QStyle::State_Sunken);
        bool upIsActive = (spinBox->activeSubControls == QStyle::SC_SpinBoxUp);
        bool downIsActive = (spinBox->activeSubControls == QStyle::SC_SpinBoxDown);
        bool hasFocus = (option->state & QStyle::State_HasFocus);

        QStyleOptionSpinBox spinBoxCopy = *spinBox;
        spinBoxCopy.rect = pixmapRect;
        QRect upRect = p.proxy()->subControlRect(QStyle::CC_SpinBox, &spinBoxCopy, QStyle::SC_SpinBoxUp, widget);
        QRect downRect = p.proxy()->subControlRect(QStyle::CC_SpinBox, &spinBoxCopy, QStyle::SC_SpinBoxDown, widget);

        if (spinBox->frame) 
        {
            cachePainter.save();
            cachePainter.setRenderHint(QPainter::Antialiasing, true);
            cachePainter.translate(0.5, 0.5);
            QColor buttonColor = option->palette.button().color();
            if ((spinBox->stepEnabled & QAbstractSpinBox::StepUpEnabled) && upIsActive) 
            {
                if (sunken)
                    cachePainter.fillRect(upRect.adjusted(0, -1, 0, 0), buttonColor.darker(150));
                else if (hover)
                    cachePainter.fillRect(upRect.adjusted(0, -1, 0, 0), buttonColor.darker(120));
            }

            if ((spinBox->stepEnabled & QAbstractSpinBox::StepDownEnabled) && downIsActive) 
            {
                if (sunken)
                    cachePainter.fillRect(downRect.adjusted(0, 0, 0, 1), buttonColor.darker(150));
                else if (hover)
                    cachePainter.fillRect(downRect.adjusted(0, 0, 0, 1), buttonColor.darker(120));
            }

            cachePainter.setPen(hasFocus ? option->palette.highlight().color() : outline);
            cachePainter.setBrush(Qt::NoBrush);
            cachePainter.drawRoundedRect(r.adjusted(0, 0, -1, -1), 2, 2);
            if (hasFocus) 
            {
                QColor softHighlight = option->palette.highlight().color();
                softHighlight.setAlpha(40);
                cachePainter.setPen(softHighlight);
                cachePainter.drawRoundedRect(r.adjusted(1, 1, -2, -2), 1.7, 1.7);
            }
            cachePainter.restore();
        }

        // outline the up/down buttons
        cachePainter.setPen(hasFocus ? option->palette.highlight().color() : outline);
        if (spinBox->direction == Qt::RightToLeft) 
            cachePainter.drawLine(upRect.right(), upRect.top() - 1, upRect.right(), downRect.bottom() + 1);
        else 
            cachePainter.drawLine(upRect.left(), upRect.top() - 1, upRect.left(), downRect.bottom() + 1);
        // mid-outline
        cachePainter.drawLine(upRect.left(), spinBox->rect.height() / 2, upRect.right(), spinBox->rect.height() / 2);

        QColor disabledColor = mergedColors(arrowColor, option->palette.button().color());
        if (spinBox->buttonSymbols == QAbstractSpinBox::PlusMinus) 
        {
            int centerX = upRect.center().x();
            int centerY = upRect.center().y();

            // plus/minus
            cachePainter.setPen((spinBox->stepEnabled & QAbstractSpinBox::StepUpEnabled) ? arrowColor : disabledColor);
            cachePainter.drawLine(centerX - 1, centerY, centerX + 3, centerY);
            cachePainter.drawLine(centerX + 1, centerY - 2, centerX + 1, centerY + 2);

            centerX = downRect.center().x();
            centerY = downRect.center().y();
            cachePainter.setPen((spinBox->stepEnabled & QAbstractSpinBox::StepDownEnabled) ? arrowColor : disabledColor);
            cachePainter.drawLine(centerX - 1, centerY, centerX + 3, centerY);
        }
        else if (spinBox->buttonSymbols == QAbstractSpinBox::UpDownArrows) 
        {
            // arrows
            painter->setRenderHint(QPainter::SmoothPixmapTransform);
            QStyleOption opt;
            opt.QStyleOption::operator=(*spinBox);
            const bool enabled = option->state & QStyle::State_Enabled;

            QSize sz = QtnPrivate::qtn_bestImageSize(widget);
            QRectF upArrowRect = QRectF(upRect.center().x() - sz.width() / 2.0 + 2.0, upRect.center().y() - sz.height() / 2.0, sz.width(), sz.height());
            opt.rect = upArrowRect.toRect().adjusted(0, 0, -2, -2);
            if (enabled && !(spinBox->stepEnabled & QAbstractSpinBox::StepUpEnabled))
                opt.state &= ~QStyle::State_Enabled;
            drawIndicatorArrow(QStyle::PE_IndicatorArrowUp, &opt, &cachePainter, widget);

            QRectF downArrowRect = QRectF(downRect.center().x() - sz.width() / 2.0 + 2.0, downRect.center().y() - sz.height() / 2.0 + 2.0, sz.width(), sz.height());
            opt.rect = downArrowRect.toRect().adjusted(0, 0, -2, -2);
            if (enabled && !(spinBox->stepEnabled & QAbstractSpinBox::StepDownEnabled))
                opt.state &= ~QStyle::State_Enabled;
            drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, &opt, &cachePainter, widget);
        }
        cachePainter.end();
        painter->drawPixmap(spinBox->rect.topLeft(), cache);
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorCheckBox(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    painter->save();
    QRect rect = option->rect;
    const QStyle::State state = option->state;
    const bool dis = !(option->state & QStyle::State_Enabled);
    QColor outline = option->palette.window().color();
    QColor highlightedOutline = option->palette.highlight().color();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(0.5, 0.5);
    rect = rect.adjusted(1, 1, -1, -1);

    QColor pressedColor = option->palette.light().color();
    painter->setBrush(Qt::NoBrush);

    if (dis)
        painter->setBrush(QBrush(outline));
    else
        painter->setBrush((state & QStyle::State_Sunken) ? QBrush(pressedColor) : option->state & (QStyle::State_On) ?
            QBrush(option->palette.buttonText().color()) : (QBrush(option->palette.light().color().lighter(126))) );
    painter->setPen((state & QStyle::State_Sunken) ? QPen(highlightedOutline) : QPen(outline.lighter(110)));

    if (option->state & QStyle::State_HasFocus && option->state & QStyle::State_KeyboardFocusChange)
        painter->setPen(QPen(highlightedOutline));
    painter->drawRoundedRect(rect, 2.0, 2.0);

    QColor checkMarkColor = option->palette.base().color().darker(120);
    const qreal checkMarkPadding = 1 + rect.width() * 0.13; // at least one pixel padding

    if (option->state & QStyle::State_NoChange)
    {
        QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
        gradient.setColorAt(0, (state & QStyle::State_Sunken) ? pressedColor : option->palette.base().color().darker(115));
        gradient.setColorAt(0.15, (state & QStyle::State_Sunken) ? pressedColor : option->palette.base().color());
        gradient.setColorAt(1, (state & QStyle::State_Sunken) ? pressedColor : option->palette.base().color());

        gradient = QLinearGradient(rect.topLeft(), rect.bottomLeft());
        checkMarkColor.setAlpha(80);
        gradient.setColorAt(0, checkMarkColor);
        checkMarkColor.setAlpha(140);
        gradient.setColorAt(1, checkMarkColor);
        checkMarkColor.setAlpha(180);
        painter->setPen(QPen(checkMarkColor, 1));
        painter->setBrush(gradient);
        painter->drawRect(rect.adjusted(checkMarkPadding, checkMarkPadding, -checkMarkPadding, -checkMarkPadding));
    }
    else if (option->state & (QStyle::State_On))
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
/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorRadioButton(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    painter->save();
    const QStyle::State state = option->state;
    const bool dis = !(option->state & QStyle::State_Enabled);
    QRect rect = option->rect;

    QColor outline = option->palette.window().color();
    QColor highlightedOutline = option->palette.highlight().color();

    QColor pressedColor = option->palette.light().color();
    if (dis)
        painter->setBrush(QBrush(outline));
    else
        painter->setBrush((state & QStyle::State_Sunken) ? QBrush(pressedColor) : option->state & (QStyle::State_On) ?
            QBrush(option->palette.buttonText().color()) : (QBrush(option->palette.light().color().lighter(126))));

    painter->setRenderHint(QPainter::Antialiasing, true);
    QPainterPath circle;
    const QPointF circleCenter = rect.center() + QPoint(1, 1);
    const qreal outlineRadius = (rect.width() + (rect.width() + 1) % 2) / 2.0 - 1;
    circle.addEllipse(circleCenter, outlineRadius, outlineRadius);

    painter->setPen((state & QStyle::State_Sunken) ? QPen(highlightedOutline) : QPen(outline.lighter(110)));
    if (option->state & QStyle::State_HasFocus && option->state & QStyle::State_KeyboardFocusChange)
        painter->setPen(QPen(highlightedOutline));
    painter->drawPath(circle);

    if (state & (QStyle::State_On)) 
    {
        circle = QPainterPath();
        const qreal checkmarkRadius = outlineRadius / 2.32;
        circle.addEllipse(circleCenter, checkmarkRadius, checkmarkRadius);
        QColor checkMarkColor = option->palette.base().color().darker(120);
        painter->setPen(checkMarkColor);
        painter->setBrush(checkMarkColor);
        painter->drawPath(circle);
    }
    painter->restore();
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
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
bool AdobePhotoshopStylePrivate::drawIndicatorToolBarHandle(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QRect rect = option->rect;
    QColor outline = option->palette.light().color().lighter(126);
    //draw grips
    if (option->state & QStyle::State_Horizontal) 
    {
        for (int i = -2; i < 1; i += 2) 
        {
            for (int j = -8; j < 10; j += 2) 
                painter->fillRect(rect.center().x() + i, rect.center().y() + j, 2, 1, outline);
        }
    }
    else //vertical toolbar
    {
        for (int i = -8; i < 12; i += 2) 
        {
            for (int j = -2; j < 1; j += 2) 
                painter->fillRect(rect.center().x() + i, rect.center().y() + j, 1, 2, outline);
        }
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorToolBarSeparator(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QRect rect = option->rect;
    const int margin = 0;
    if (option->state & QStyle::State_Horizontal) 
    {
        const int offset = rect.width() / 2;
        painter->setPen(QPen(option->palette.window().color().darker(138)));
        painter->drawLine(rect.bottomLeft().x() + offset, rect.bottomLeft().y() - margin,
            rect.topLeft().x() + offset, rect.topLeft().y() + margin);
    }
    else 
    {   // Draw vertical separator
        const int offset = rect.height() / 2;
        painter->setPen(QPen(option->palette.window().color().darker(110)));
        painter->drawLine(rect.topLeft().x() + margin, rect.topLeft().y() + offset,
            rect.topRight().x() - margin, rect.topRight().y() + offset);
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorHeaderArrow(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QStyleOptionHeader* header = qstyleoption_cast<const QStyleOptionHeader *>(option))
    {
        QStyleOption opt;
        opt.QStyleOption::operator=(*header);

        QSize sz = QtnPrivate::qtn_bestImageSize(w);
        opt.rect.setSize(QSize(sz.width(), sz.height()));
        opt.rect.moveCenter(header->rect.center());
        opt.rect.translate(QPoint(0, -1));

        if (header->sortIndicator & QStyleOptionHeader::SortUp)
            drawIndicatorArrow(QStyle::PE_IndicatorArrowUp, &opt, painter, w);
        else if (header->sortIndicator & QStyleOptionHeader::SortDown)
            drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, &opt, painter, w);
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawIndicatorBranch(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (!(option->state & QStyle::State_Children))
        return true;
    if (option->state & QStyle::State_Open)
        drawIndicatorArrow(QStyle::PE_IndicatorArrowDown, option, painter, widget);
    else
        drawIndicatorArrow(QStyle::PE_IndicatorArrowRight, option, painter, widget);
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawFrameFocusRect(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (const QStyleOptionFocusRect* focusRectOption = qstyleoption_cast<const QStyleOptionFocusRect*>(option))
    {
        const QColor highlightedOutline = focusRectOption->palette.highlight().color();
        const bool isColorLight = qtn_isColorLight(highlightedOutline);
        const bool isPushButton = widget != Q_NULL && widget->inherits("QPushButton");
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        QColor focuscolor = isColorLight ? highlightedOutline.darker() : highlightedOutline.lighter();
        focuscolor.setAlpha(50);
        painter->setPen(QPen(focuscolor, 1, Qt::DotLine));
        if (!isPushButton)
        {
            QRect rect = focusRectOption->rect;
            QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
            gradient.setColorAt(0, isColorLight ? focuscolor.darker() : focuscolor.lighter());
            gradient.setColorAt(1, focuscolor);
            painter->setBrush(gradient);
        }

        const qreal radius = isPushButton ? static_cast<qreal>(option->rect.height()) / 2.0 : 2.0;
        QRectF rect = qtn_adjustRectF(painter->pen(), option->rect);
        painter->drawRoundedRect(rect, radius, radius);
        painter->restore();
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawRibbonFrameGallery(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
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
            if (m_theme == AdobePhotoshopStyle::LightGray)
                painter->fillRect(opt->rect, selected && enabled ? StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.03) : 
                    StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.018));
            else if (m_theme == AdobePhotoshopStyle::Gray)
                painter->fillRect(opt->rect, selected && enabled ? StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.18) :
                    StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.14));
            else if (m_theme == AdobePhotoshopStyle::DarkGray)
                painter->fillRect(opt->rect, selected && enabled ? StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.80) :
                    StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.6));
            else if (m_theme == AdobePhotoshopStyle::Black)
                painter->fillRect(opt->rect, selected && enabled ? StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.80) :
                    StyleHelper::colorMakeLighter(opt->palette.color(QPalette::Base).rgb(), 0.6));
        }
        if (frameGallery->lineWidth > 0)
        {
            const QPen savePen = painter->pen();
            painter->setPen(opt->palette.midlight().color());
            painter->drawRect(qtn_adjustRect(painter->pen(), opt->rect));
            painter->setPen(savePen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawPanelTipLabel(const QStyleOption* opt, QPainter* painter, const QWidget* wd) const
{
    Q_UNUSED(wd)
    const QBrush brush(opt->palette.toolTipBase());
    const bool isColorLight = qtn_isColorLight(brush.color());
    const QColor border = isColorLight ?  StyleHelper::colorMakeLighter(opt->palette.toolTipText().color().rgb(), 2.5) :
        StyleHelper::colorMakeDarker(opt->palette.toolTipText().color().rgb(), 0.55);
    qDrawPlainRect(painter, opt->rect, border, 1, &brush);
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawPanelButtonTool(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if ((option->state & QStyle::State_Enabled || option->state & QStyle::State_On) || !(option->state & QStyle::State_AutoRaise))
    {
        if (widget && widget->inherits("QDockWidgetTitleButton"))
        {
            if (option->state & QStyle::State_MouseOver)
                p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, option, painter, widget);
        }
        else 
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, option, painter, widget);
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawPanelButtonCommand(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits("QMdiArea") && (!(option->state & QStyle::State_MouseOver) && !(option->state & QStyle::State_Sunken)))
        return true;

    painter->save();
    QColor outline = option->palette.mid().color();
    QColor highlightedOutline = option->palette.highlight().color();

    bool isDefault = false;
    bool isFlat = false;
    bool isDown = (option->state & QStyle::State_Sunken) || (option->state & QStyle::State_On);
    if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton*>(option))
    {
        isDefault = (button->features & QStyleOptionButton::DefaultButton) && (button->state & QStyle::State_Enabled);
        isFlat = (button->features & QStyleOptionButton::Flat);
    }

    if (isFlat && !isDown)
    {
        if (isDefault)
        {
            QRect r = option->rect.adjusted(0, 1, 0, -1);
            painter->setPen(QPen(Qt::black));
            const QLine lines[4] =
            {
                QLine(QPoint(r.left() + 2, r.top()),
                QPoint(r.right() - 2, r.top())),
                QLine(QPoint(r.left(), r.top() + 2),
                QPoint(r.left(), r.bottom() - 2)),
                QLine(QPoint(r.right(), r.top() + 2),
                QPoint(r.right(), r.bottom() - 2)),
                QLine(QPoint(r.left() + 2, r.bottom()),
                QPoint(r.right() - 2, r.bottom()))
            };
            painter->drawLines(lines, 4);
            const QPoint points[4] =
            {
                QPoint(r.right() - 1, r.bottom() - 1),
                QPoint(r.right() - 1, r.top() + 1),
                QPoint(r.left() + 1, r.bottom() - 1),
                QPoint(r.left() + 1, r.top() + 1)
            };
            painter->drawPoints(points, 4);
        }
        return true;
    }

    bool isEnabled = option->state & QStyle::State_Enabled;
    bool hasFocus = (option->state & QStyle::State_HasFocus && option->state & QStyle::State_KeyboardFocusChange);
    QColor buttonColor = option->palette.button().color();

    QColor darkOutline = outline;
    if (hasFocus | isDefault)
        darkOutline = highlightedOutline;

    if (isDefault)
        buttonColor = mergedColors(buttonColor, highlightedOutline.lighter(130), 90);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::transparent);
    if (isDown && isEnabled)
        painter->setBrush(buttonColor.darker(150));
    else if (isEnabled)
    {
        if (widget != Q_NULL && widget->inherits("QDockWidgetTitleButton") && 
            (m_theme == AdobePhotoshopStyle::DarkGray || m_theme == AdobePhotoshopStyle::Black))
            painter->setBrush(option->state & QStyle::State_MouseOver ? highlightedOutline : buttonColor);
        else
            painter->setBrush(option->state & QStyle::State_MouseOver ? buttonColor.darker(120) : buttonColor);
    }
    else if (!isEnabled)
        painter->setBrush(buttonColor);
    painter->setPen(!isEnabled ? QPen(darkOutline.lighter(80)) : QPen(darkOutline));

    const qreal radius = widget && widget->inherits("QPushButton") ? static_cast<qreal>(option->rect.height()) / 2.0 : 2.0;
    QRectF rect = qtn_adjustRectF(painter->pen(), option->rect);
    painter->drawRoundedRect(rect, radius, radius);
    painter->restore();
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawFrameLineEdit(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(painter)
    Q_UNUSED(w)
    QRect rect = option->rect;
    QRect r = rect;

    const bool isEnabled = option->state & QStyle::State_Enabled;
    const bool hasFocus = option->state & QStyle::State_HasFocus;

    QColor outline = isEnabled ?  option->palette.mid().color() : option->palette.light().color();
    QColor highlightedOutline = option->palette.highlight().color();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    //  ### highdpi painter bug.
    painter->translate(0.5, 0.5);

    // Draw Outline
    painter->setPen(QPen(hasFocus ? highlightedOutline : outline));
    painter->setBrush(option->palette.base());
    painter->drawRoundedRect(r.adjusted(0, 0, -1, -1), 2, 2);

    if (hasFocus) 
    {
        QColor softHighlight = highlightedOutline;
        softHighlight.setAlpha(40);
        painter->setPen(softHighlight);
        painter->drawRoundedRect(r.adjusted(1, 1, -2, -2), 1.7, 1.7);
    }
    painter->restore();
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawMenuBarItem(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option))
    {
        painter->save();
        QWidget* that = const_cast<QWidget*>(widget);
        that->setAttribute(Qt::WA_Hover, true);
        QStyleOptionMenuItem item = *mbi;
        item.rect = mbi->rect.adjusted(0, 1, 0, -3);
        const bool dis = !(mbi->state & QStyle::State_Enabled);
        const bool hover = !dis && (option->state & QStyle::State_Selected);
        const bool act = !dis && (mbi->state & QStyle::State_Selected && mbi->state & QStyle::State_Sunken);

        if (hover)
        {
            painter->save();
            QRect r = option->rect;
            if (act)
                painter->setBrush(option->palette.light().color());
            else
                painter->setBrush(option->palette.highlight().color());

            QColor highlightOutline = option->palette.highlight().color().lighter(144);
            painter->setPen(QPen(highlightOutline));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->translate(0.5, -0.5);
            painter->drawRoundedRect(r.adjusted(2, 2, -2, -2), 2, 2);
            painter->restore();
        }

        //draw text
        QPalette::ColorRole textRole = QPalette::ButtonText;
        if (dis)
            textRole = QPalette::Text;
        else if (hover || act)
            textRole = QPalette::HighlightedText;

        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
        if (!p.styleHint(QStyle::SH_UnderlineShortcut, mbi, widget))
            alignment |= Qt::TextHideMnemonic;
        p.proxy()->drawItemText(painter, item.rect, alignment, mbi->palette, mbi->state & QStyle::State_Enabled, mbi->text, textRole);
        painter->restore();
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawMenuItem(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
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
            if (!menuItem->text.isEmpty()) 
            {
                painter->setFont(menuItem->font);
                p.proxy()->drawItemText(painter, menuItem->rect.adjusted(5, 0, -5, 0), Qt::AlignLeft | Qt::AlignVCenter,
                    menuItem->palette, menuItem->state & QStyle::State_Enabled, menuItem->text,
                    QPalette::Text);
                w = qtn_horizontalAdvanceFont(menuItem->fontMetrics, menuItem->text) + 5;
            }
            painter->setPen(shadow.lighter(106));
            bool reverse = menuItem->direction == Qt::RightToLeft;
            painter->drawLine(menuItem->rect.left() + 5 + (reverse ? 0 : w), menuItem->rect.center().y(),
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
            painter->drawRect(qtn_adjustRect(painter->pen(), r));
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
        const QStyleOption *opt = option;
        const QStyleOptionMenuItem *menuitem = menuItem;

        QPainter* painter2 = painter;
        QRect vCheckRect = p.visualRect(opt->direction, menuitem->rect,
            QRect(menuitem->rect.x() + 4, menuitem->rect.y(), checkcol, menuitem->rect.height()));
        if (!menuItem->icon.isNull()) 
        {
            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            if (act && !dis)
                mode = QIcon::Active;
            QPixmap pixmap;

            int smallIconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, option, widget);
            QSize iconSize(smallIconSize, smallIconSize);
            if (const QComboBox *combo = qobject_cast<const QComboBox*>(widget))
                iconSize = combo->iconSize();

            if (checked)
                pixmap = menuItem->icon.pixmap(iconSize, mode, QIcon::On);
            else
                pixmap = menuItem->icon.pixmap(iconSize, mode);

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
                    QColor activeColor = mergedColors(option->palette.window().color(),
                        option->palette.highlight().color());
                    QPalette_setBrush(opt.palette, QPalette::Button, activeColor);
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
bool AdobePhotoshopStylePrivate::drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const RibbonOptionHeaderStyleOption* styleOption = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        const QColor color = CommonStyle::contextColorToColor(styleOption->pageColor);
        if ((styleOption->state & QStyle::State_Selected) && !color.isValid())
        {
            if (CommonStylePrivate::drawRibbonTab(opt, painter, widget))
            {
                const int heightButton = CommonStylePrivate::dpiScaled(3, widget);
                QRect rect = styleOption->rect.adjusted(1, 1, -1, -1);
                if (styleOption->tabBarPosition == 1)
                    rect.setBottom(rect.top() + heightButton);
                else if (styleOption->tabBarPosition == 2)
                    rect.setTop(rect.bottom() - heightButton);

                // fill the background of the tab
                painter->fillRect(rect, styleOption->palette.color(QPalette::Highlight));
                return true;
            }
        }
    }
    return CommonStylePrivate::drawRibbonTab(opt, painter, widget);
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawToolBoxTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w)
    qDrawShadePanel(p, opt->rect, opt->palette, opt->state & (QStyle::State_Sunken | QStyle::State_On), 1, &opt->palette.brush(QPalette::Button));
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawRibbonGroupLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const RibbonGroupStyleOption* optGroupBox = qstyleoption_cast<const RibbonGroupStyleOption*>(opt))
    {
        int flags = static_cast<int>(optGroupBox->textAlignment);
        flags |= Qt::TextHideMnemonic;

        QRect rcEntryCaption = optGroupBox->rect;
        rcEntryCaption.setTop(rcEntryCaption.bottom() - optGroupBox->heightCaption);
        rcEntryCaption.adjust(-2, 0, -optGroupBox->widthOptButton, 0);
        // draw text
        QString text = optGroupBox->fontMetrics.elidedText(optGroupBox->text, optGroupBox->textElideMode, rcEntryCaption.width());
        if (!text.isEmpty())
        {
            const QPen savePen = painter->pen();
            painter->setPen(optGroupBox->palette.color(QPalette::WindowText).darker(130));
            painter->drawText(rcEntryCaption, flags, text);
            painter->setPen(savePen);
        }
    }
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawRibbonGalleryItem(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const AdobePhotoshopStyle);
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
bool AdobePhotoshopStylePrivate::drawRibbonBackstageFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const RibbonBackstageStyleOption* optBackstage = qstyleoption_cast<const RibbonBackstageStyleOption*>(opt))
    {
        painter->fillRect(opt->rect, optBackstage->palette.window());
        if (optBackstage->menuWidth != -1)
        {
            QRect rc = QRect(QPoint(optBackstage->menuWidth, 0), QPoint(optBackstage->menuWidth + 1, opt->rect.bottom()));
            painter->fillRect(rc, optBackstage->palette.dark());
        }
        return true;
    }
    return false;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawScrollBarAddSubLine(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if ((opt->state & QStyle::State_Sunken))
        painter->fillRect(opt->rect, opt->palette.dark().color()/*.dark(240)*/);
    else 
        painter->fillRect(opt->rect, opt->palette.light().color().lighter(132));

    QStyle::PrimitiveElement arrow;
    if (opt->state & QStyle::State_Horizontal)
    {
        if (ce == QStyle::CE_ScrollBarAddLine)
            arrow = opt->direction == Qt::LeftToRight ? QStyle::PE_IndicatorArrowRight : QStyle::PE_IndicatorArrowLeft;
        else
            arrow = opt->direction == Qt::LeftToRight ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
    }
    else 
    {
        if (ce == QStyle::CE_ScrollBarAddLine)
            arrow = QStyle::PE_IndicatorArrowDown;
        else
            arrow = QStyle::PE_IndicatorArrowUp;
    }
    QStyleOption arrowOpt = *opt;
    arrowOpt.rect = opt->rect.adjusted(1, 1, -1, -1);
    p.proxy()->drawPrimitive(arrow, &arrowOpt, painter, widget);
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawScrollBarAddSubPage(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(ce)
    Q_UNUSED(w)
    painter->fillRect(opt->rect, opt->palette.light().color().lighter(132));
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawScrollBarSlider(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    painter->save();
    painter->fillRect(opt->rect, (opt->state & QStyle::State_Enabled) ? opt->palette.button().color().lighter(125) : opt->palette.light());
    painter->setPen(opt->palette.light().color().lighter(132));
    painter->drawRect(qtn_adjustRect(painter->pen(), opt->rect));
    painter->restore();
    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawTabBarTabShape(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    painter->save();
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab *>(option)) 
    {
        QRect rect = option->rect;
        QColor outline = option->palette.light().color();
        bool enabled = tab->state & QStyle::State_Enabled;
        bool rtlHorTabs = (tab->direction == Qt::RightToLeft && (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth));
        bool selected = tab->state & QStyle::State_Selected;
        bool lastTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::End) || (rtlHorTabs && tab->position == QStyleOptionTab::Beginning));
        bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
        int tabOverlap = p.pixelMetric(QStyle::PM_TabBarTabOverlap, option, widget);
        rect = option->rect.adjusted(0, 0, (onlyOne || lastTab) ? 0 : tabOverlap, 0);

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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QColor tabFrameColor = tab->features & QStyleOptionTab::HasFrame ? option->palette.button().color() : option->palette.window().color();
#else
        QColor tabFrameColor = option->palette.button().color();
#endif
        QRect drawRect = rect;
        painter->setPen(outline.lighter(110));
        painter->save();
        painter->setClipRect(rect.adjusted(-1, -1, 1, selected ? -2 : -3));
        painter->setBrush(selected ? tabFrameColor.lighter(104) : ((enabled && (option->state & QStyle::State_MouseOver)) ? 
            tabFrameColor.darker(105) : tabFrameColor.darker(110)));
        painter->drawRect(qtn_adjustRect(painter->pen(), drawRect));
        painter->restore();
    }
    painter->restore();
    return true;
}

// Draws the header in tables.
/*! \internal */
bool AdobePhotoshopStylePrivate::drawHeaderSection(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    painter->save();
    if (const QStyleOptionHeader* header = qstyleoption_cast<const QStyleOptionHeader *>(option))
    {
        QRect rect = header->rect;

        if (w && w->inherits(QTITAN_META_CLASSNAME("GridBase")))
        {
            if (!(header->state & QStyle::State_Sunken) && !(header->state & QStyle::State_MouseOver))
                painter->fillRect(header->rect, header->palette.button());
        }

        if (header->orientation == Qt::Horizontal)
        {
            if (header->state & QStyle::State_Sunken)
                painter->fillRect(rect.adjusted(0, 0, -2, -2), header->palette.button().color().darker(135));
            else if (header->state & QStyle::State_MouseOver)
                painter->fillRect(rect.adjusted(0, 0, -2, -2), header->palette.button().color().darker(120));
        }
        else
        {
            if (header->state & QStyle::State_Sunken)
                painter->fillRect(rect.adjusted(0, 0, -1, -1), header->palette.button().color().darker(135));
            else if (header->state & QStyle::State_MouseOver)
                painter->fillRect(rect.adjusted(0, 0, -1, -1), header->palette.button().color().darker(120));
        }

        QBrush fill = header->palette.window().color().darker(138);
        QPixmap cache = qtnStyleCachePixmap(rect.size());
        cache.fill(Qt::transparent);
        QRect pixmapRect(0, 0, rect.width(), rect.height());
        QPainter cachePainter(&cache);
        cachePainter.setBrush(Qt::NoBrush);
        cachePainter.setPen(fill.color());
        cachePainter.drawLine(pixmapRect.bottomLeft(), pixmapRect.bottomRight());

        if (header->orientation == Qt::Horizontal &&
            header->position != QStyleOptionHeader::End &&
            header->position != QStyleOptionHeader::OnlyOneSection) 
        {
            const int indent = (pixmapRect.height() * 15) / 100;
            pixmapRect.adjust(0, indent, 0, -indent);

            cachePainter.setPen(fill.color().lighter(130));
            cachePainter.drawLine(pixmapRect.topRight(), pixmapRect.bottomRight() + QPoint(0, -1));
            cachePainter.setPen(fill.color());
            cachePainter.drawLine(pixmapRect.topRight() + QPoint(-1, 0), pixmapRect.bottomRight() + QPoint(-1, -1));
        }
        else if (header->orientation == Qt::Vertical) 
        {
            cachePainter.setPen(fill.color());
            cachePainter.drawLine(pixmapRect.topRight(), pixmapRect.bottomRight());
        }
        cachePainter.end();
        painter->drawPixmap(rect.topLeft(), cache);
    }
    painter->restore();

    return true;
}

/*! \internal */
bool AdobePhotoshopStylePrivate::drawDockWidgetTitle(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const AdobePhotoshopStyle);
    if (widget != Q_NULL && widget->inherits("QDockWidget"))
    {

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (const QStyleOptionDockWidget* dwOpt = qstyleoption_cast<const QStyleOptionDockWidget *>(opt))
#else
        if (const QStyleOptionDockWidgetV2* dwOpt = qstyleoption_cast<const QStyleOptionDockWidgetV2 *>(opt))
#endif
        {
            QRect rect = opt->rect;
            const bool verticalTitleBar = dwOpt->verticalTitleBar;
            if (verticalTitleBar)
            {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
                rect = rect.transposed();
#else
                rect = QRect(rect.topLeft(), QSize(rect.height(), rect.width()));
#endif
                painter->translate(rect.left() - 1, rect.top() + rect.width());
                painter->rotate(-90);
                painter->translate(-rect.left() + 1, -rect.top());
            }

            painter->setBrush(opt->palette.window().color().darker(110));
            painter->setPen(opt->palette.window().color().darker(130));
            painter->drawRect(rect.adjusted(0, 1, -1, -3));

            int buttonMargin = 4;
            int mw = p.proxy()->pixelMetric(QStyle::PM_DockWidgetTitleMargin, dwOpt, widget);
            int fw = p.proxy()->pixelMetric(QStyle::PM_DockWidgetFrameWidth, dwOpt, widget);
            const QDockWidget* dw = qobject_cast<const QDockWidget*>(widget);
            bool isFloating = dw != Q_NULL && dw->isFloating();

            QRect r = opt->rect.adjusted(0, 2, -1, -3);
            QRect titleRect = r;

            if (dwOpt->closable)
            {
                QSize sz = p.proxy()->standardIcon(QStyle::SP_TitleBarCloseButton, dwOpt, widget).actualSize(QSize(10, 10));
                titleRect.adjust(0, 0, -sz.width() - mw - buttonMargin, 0);
            }

            if (dwOpt->floatable)
            {
                QSize sz = p.proxy()->standardIcon(QStyle::SP_TitleBarMaxButton, dwOpt, widget).actualSize(QSize(10, 10));
                titleRect.adjust(0, 0, -sz.width() - mw - buttonMargin, 0);
            }

            if (isFloating)
            {
                titleRect.adjust(0, -fw, 0, 0);
                if (widget != Q_NULL && widget->windowIcon().cacheKey() != QApplication::windowIcon().cacheKey())
                    titleRect.adjust(titleRect.height() + mw, 0, 0, 0);
            }
            else
            {
                titleRect.adjust(mw, 0, 0, 0);
                if (!dwOpt->floatable && !dwOpt->closable)
                    titleRect.adjust(0, 0, -mw, 0);
            }
            if (!verticalTitleBar)
                titleRect = p.visualRect(dwOpt->direction, r, titleRect);

            if (!dwOpt->title.isEmpty())
            {
                QString titleText = painter->fontMetrics().elidedText(dwOpt->title, Qt::ElideRight,
                    verticalTitleBar ? titleRect.height() : titleRect.width());
                const int indent = 4;
                p.drawItemText(painter, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter,
                    dwOpt->palette, dwOpt->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
            }
        }
        return true;
    }
    else if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("DockTitleBar")))
    {
        if (const DockTitleBarStyleOption* dwOpt = qstyleoption_cast<const DockTitleBarStyleOption*>(opt))
        {
            const QRect rc = dwOpt->rect;
            QColor clr = dwOpt->palette.window().color();
            painter->fillRect(rc/*.adjusted(0, 1, -1, 0)*/, clr);

            if ((m_theme == AdobePhotoshopStyle::LightGray || m_theme == AdobePhotoshopStyle::DarkGray) && !dwOpt->lineRect.isEmpty())
            {
                QtnPen pen(painter, dwOpt->palette.dark().color(), dpiScaled(1, widget));
                QPoint pt(dwOpt->lineRect.left(), dwOpt->lineRect.center().y());
                int offsetX = 0;
                int spaceX = dpiScaled(4, widget);
                int spaceY = dpiScaled(2, widget);
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
                QPixmap pixmap = dwOpt->icon.pixmap(p.pixelMetric(QStyle::PM_SmallIconSize, opt, widget));
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
    }
    else
    {
        return CommonStylePrivate::drawDockWidgetTitle(opt, painter, widget);
    }
    return false;
}

/*! \internal */
QColor AdobePhotoshopStylePrivate::buttonHighlightColor(const QPalette& pal, bool highlightDn, bool border, const QWidget* wd) const
{
    Q_UNUSED(wd)
    Q_UNUSED(pal)
    if (border)
        return highlightDn ? pal.brush(QPalette::Highlight).color().lighter(135) : pal.brush(QPalette::Highlight).color().lighter(155);
    return highlightDn ? pal.brush(QPalette::Highlight).color().lighter(155) : pal.brush(QPalette::Highlight).color().lighter(170);
}
/*! \internal */
QColor AdobePhotoshopStylePrivate::titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* wd) const
{ Q_UNUSED(wd); return pal.color(isActive ? QPalette::Active : QPalette::Inactive, QPalette::Midlight); }

/*! \internal */
void AdobePhotoshopStylePrivate::drawHighlightItemGallery(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    const bool highlighted = option->state & QStyle::State_MouseOver;
    const bool pressed = option->state & QStyle::State_Sunken;
    const bool enabled = option->state & QStyle::State_Enabled;
    const bool checked = option->state & QStyle::State_On;
    if (enabled && (highlighted || pressed || checked))
    {
        const QPen savePen = painter->pen();
        painter->setPen(buttonHighlightColor(option->palette, pressed/*highlightDn*/, true));
        QBrush fill = buttonHighlightColor(option->palette, pressed/*highlightDn*/, false);
        painter->fillRect(option->rect.adjusted(1, 0, -2, 0), fill);
        painter->drawRect(option->rect.adjusted(1, 0, -2, -1));
        painter->setPen(savePen);
    }
    else
    {
//        QPalette pal = option->palette;
//        if (const QMenu* parentMenu = qobject_cast<const QMenu*>(widget ? widget->parentWidget() : Q_NULL))
//            pal = parentMenu->palette();
//        painter->fillRect(option->rect, pal.brush(QPalette::Base).color());
    }
}

QColor AdobePhotoshopStylePrivate::textGalleryItem(const QPalette& pal) const
{
    return pal.brush(QPalette::WindowText).color();
}

void AdobePhotoshopStylePrivate::updateTheme()
{
    QTN_P(AdobePhotoshopStyle);
    QString themeName = QStringLiteral("LightGray");
    switch (m_theme)
    {
        case AdobePhotoshopStyle::LightGray:
            themeName = QStringLiteral("LightGray");
            break;
        case AdobePhotoshopStyle::Gray:
            themeName = QStringLiteral("Gray");
            break;
        case AdobePhotoshopStyle::DarkGray:
            themeName = QStringLiteral("DarkGray");
            break;
        case AdobePhotoshopStyle::Black:
            themeName = QStringLiteral("Black");
            break;
    }
    p.setStyleConfig(QStringLiteral(":/res/AdobePhotoshop/"), themeName);
}



/*!
\class AdobePhotoshopStyle
\inmodule QtitanStyle
\brief AdobePhotoshopStyle class implements Adobe Pthotoshop style for application with four themes: light gray, gray, dark dray and black.
*/
/*!
Constuctor of AdobePhotoshopStyle class. 
*/
AdobePhotoshopStyle::AdobePhotoshopStyle()
    : CommonStyle(*new AdobePhotoshopStylePrivate)
{
    initStyleResource();
    QTN_D(AdobePhotoshopStyle);
    d.updateTheme();
}

AdobePhotoshopStyle::~AdobePhotoshopStyle()
{
}

AdobePhotoshopStyle::Theme AdobePhotoshopStyle::theme() const
{
    QTN_D(const AdobePhotoshopStyle);
    return d.m_theme;
}

/*!
\property AdobePhotoshopStyle::theme
Sets the \a theme of the office style. You can choose from Light Gray, Gray, Dark Gray or Black theme's families.
\sa Theme
*/
void AdobePhotoshopStyle::setTheme(AdobePhotoshopStyle::Theme theme)
{
    QTN_D(AdobePhotoshopStyle);
    if (d.m_theme != theme)
    {
        d.beginUpdate();
        d.m_theme = theme;
        d.updateTheme();
        d.endUpdate();
    }
}

/*! \reimp */
void AdobePhotoshopStyle::polish(QApplication* app)
{
    CommonStyle::polish(app);
}

/*! \reimp */
void AdobePhotoshopStyle::polish(QWidget* widget)
{
    QTN_D(AdobePhotoshopStyle);
    CommonStyle::polish(widget);
    if (qobject_cast<QMenuBar*>(widget))
        d.m_heightMenuBar = widget->height();
    else if (QTabBar* tabBar = qobject_cast<QTabBar*>(widget))
        tabBar->setExpanding(false);
    else if (widget->inherits("QTipLabel"))
        widget->setContentsMargins(3, 0, 4, 0);
}

/*! \reimp */
QIcon AdobePhotoshopStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const AdobePhotoshopStyle);
    if (widget != Q_NULL && widget->inherits("QDockWidget"))
    {
        bool black = (d.m_theme == AdobePhotoshopStyle::DarkGray || d.m_theme == AdobePhotoshopStyle::Black);
        if (standardIcon == QStyle::SP_TitleBarCloseButton)
            return QIcon(d.standardPixmap(QStringLiteral("Image_Close"), black ? CommonStylePrivate::White : CommonStylePrivate::Black, widget));
        else if (standardIcon == QStyle::SP_TitleBarNormalButton)
            return QIcon(d.standardPixmap(QStringLiteral("Image_Normal"), black ? CommonStylePrivate::White : CommonStylePrivate::Black, widget));
    }
    return CommonStyle::standardIcon(standardIcon, opt, widget);
}

/*! \reimp */
QRect AdobePhotoshopStyle::subControlRect(QStyle::ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
{
    QRect rect = CommonStyle::subControlRect(control, option, subControl, widget);
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
                    default:
                    break;
                }
                rect = visualRect(spinbox->direction, spinbox->rect, rect);
            }
            break;

        case CC_ComboBox:
            switch (subControl)
            {
                case SC_ComboBoxArrow:
                    rect = visualRect(option->direction, option->rect, rect);
                    rect.setRect(rect.right() - CommonStylePrivate::dpiScaled(18, widget), rect.top() - 2, CommonStylePrivate::dpiScaled(19, widget), rect.height() + 4);
                    rect = visualRect(option->direction, option->rect, rect);
                    break;
                case SC_ComboBoxEditField:
                {
                    int frameWidth = 2;
                    rect = visualRect(option->direction, option->rect, rect);
                    rect.setRect(option->rect.left() + frameWidth, option->rect.top() + frameWidth,
                    option->rect.width() - CommonStylePrivate::dpiScaled(19, widget) - 2 * frameWidth,
                    option->rect.height() - 2 * frameWidth);
                    if (const QStyleOptionComboBox *box = qstyleoption_cast<const QStyleOptionComboBox *>(option))
                    {
                        if (!box->editable)
                        {
                            rect.adjust(2, 0, 0, 0);
                            if (box->state & (State_Sunken | State_On))
                                rect.translate(1, 1);
                        }
                    }
                    rect = visualRect(option->direction, option->rect, rect);
                    break;
                }
                default:
                break;
            }
            break;

        case CC_Slider:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSliderSlider")))
            {
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

        case CC_TitleBar:
            if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
            {
                SubControl sc = subControl;
                QRect &ret = rect;
                const int indent = 6;
                const int controlTopMargin = 1;
                const int controlBottomMargin = (rect.height() * 46) / 100;
                const int controlWidthMargin = 2;
                const int controlHeight = tb->rect.height() - controlTopMargin - controlBottomMargin;
                int controlWidth = controlHeight + controlWidthMargin * 5;
                const int controlCloseWidth = controlHeight + controlHeight + controlWidthMargin * 5;
                const int delta = controlWidth;
                int offset = 0;

                bool isMinimized = tb->titleBarState & Qt::WindowMinimized;
                bool isMaximized = tb->titleBarState & Qt::WindowMaximized;

                switch (sc)
                {
                case SC_TitleBarLabel:
                    if (tb->titleBarFlags & (Qt::WindowTitleHint | Qt::WindowSystemMenuHint))
                    {
                        ret = tb->rect;

                        if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                            ret.adjust(delta, 0, -controlCloseWidth - controlHeight, 0);
                        if (tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
                            ret.adjust(0, 0, -delta, 0);
                        if (tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
                            ret.adjust(0, 0, -delta, 0);
                        if (tb->titleBarFlags & Qt::WindowShadeButtonHint)
                            ret.adjust(0, 0, -delta, 0);
                        if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                            ret.adjust(0, 0, -delta, 0);
                    }
                    break;
                case SC_TitleBarContextHelpButton:
                    if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                        offset += delta;
                    //fall through
                    QTN_FALLTHROUGH();
                case SC_TitleBarMinButton:
                    if (!isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                        offset += delta;
                    else if (sc == SC_TitleBarMinButton)
                        break;
                    //fall through
                    QTN_FALLTHROUGH();
                case SC_TitleBarNormalButton:
                    if (isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                        offset += delta;
                    else if (isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                        offset += delta;
                    else if (sc == SC_TitleBarNormalButton)
                        break;
                    //fall through
                    QTN_FALLTHROUGH();
                case SC_TitleBarMaxButton:
                    if (!isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                        offset += delta;
                    else if (sc == SC_TitleBarMaxButton)
                        break;
                    //fall through
                    QTN_FALLTHROUGH();
                case SC_TitleBarShadeButton:
                    if (!isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                        offset += delta;
                    else if (sc == SC_TitleBarShadeButton)
                        break;
                    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                    //fall through
                    QTN_FALLTHROUGH();
                    #endif
                case SC_TitleBarUnshadeButton:
                    if (isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                        offset += delta;
                    else if (sc == SC_TitleBarUnshadeButton)
                        break;
                    //fall through
                    QTN_FALLTHROUGH();
                case SC_TitleBarCloseButton:
                    if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                    {
                        offset += controlCloseWidth;
                        if (sc == SC_TitleBarCloseButton)
                            controlWidth = controlCloseWidth;
                    }
                    else if (sc == SC_TitleBarCloseButton)
                        break;
                    ret.setRect(tb->rect.right() - indent - offset, tb->rect.top() + controlTopMargin, controlWidth, controlHeight);
                    ret.adjust(0, 0, 1, 0);
                    ret.translate(-1, 0);
                    break;
                case SC_TitleBarSysMenu:
                    if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                        ret.setRect(tb->rect.left() + controlWidthMargin + indent, tb->rect.top() + (controlHeight / 2)/*controlTopMargin*/, controlHeight, controlHeight);
                    break;
                default:
                    break;
                }
                ret = visualRect(tb->direction, tb->rect, ret);
            }
            break;
        case CC_MdiControls:
        {
            int numSubControls = 0;
            if (option->subControls & SC_MdiCloseButton)
                ++numSubControls;
            if (option->subControls & SC_MdiMinButton)
                ++numSubControls;
            if (option->subControls & SC_MdiNormalButton)
                ++numSubControls;
            if (numSubControls == 0)
                break;

            QRect rc = option->rect;
            int oneWidth = rc.width() / numSubControls;
            if (oneWidth != rc.height())
            {
                rc.setTop(((rc.height() - oneWidth) / 2) + 1);
                rc.setHeight(oneWidth);
            }

            int buttonWidth = rc.width() / numSubControls - 1;
            int offset = 0;
            switch (subControl)
            {
                case SC_MdiCloseButton:
                    // Only one sub control, no offset needed.
                    if (numSubControls == 1)
                        break;
                    offset += buttonWidth;
                    QTN_FALLTHROUGH();
                case SC_MdiNormalButton:
                    // No offset needed if
                    // 1) There's only one sub control
                    // 2) We have a close button and a normal button (offset already added in SC_MdiClose)
                    if (numSubControls == 1 || (numSubControls == 2 && !(option->subControls & SC_MdiMinButton)))
                        break;
                    if (option->subControls & SC_MdiNormalButton)
                        offset += buttonWidth;
                    break;
                default:
                    break;
            }
            // Subtract one pixel if we only have one sub control. At this point
            // buttonWidth is the actual width + 1 pixel margin, but we don't want the
            // margin when there are no other controllers.
            if (numSubControls == 1)
                --buttonWidth;
            rect = QRect(offset, 0, buttonWidth, rc.height());
            rect.moveTop(rc.top());
            break;
        }
        default:
            break;
    };
    return rect;
}

/*! \reimp */
QSize AdobePhotoshopStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption* opt, const QSize& size, const QWidget* widget) const
{
    QTN_D(const AdobePhotoshopStyle);
    QSize sz = d.ribbonSizeFromContents(type, opt, size, widget);
    if (sz.isEmpty())
    {
        sz = size;
        switch (type)
        {
            case CT_PushButton:
                if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton *>(opt))
                {
                    sz = QCommonStyle::sizeFromContents(type, opt, size, widget);
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
                    sz += QSize(AdobePhotoshopStylePrivate::windowsItemHMargin * 5 + 1, 5);
                break;
            case CT_MenuItem:
            {
                if (const QStyleOptionMenuItem* mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                {
                    int w = sz.width();
                    sz = QCommonStyle::sizeFromContents(type, opt, size, widget);

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
                        sz = QSize(10, AdobePhotoshopStylePrivate::windowsSepHeight);
                    }
                    else if (mi->icon.isNull())
                    {
                        sz.setHeight(sz.height() - 2);
                        w -= 6;
                    }

                    if (mi->menuItemType != QStyleOptionMenuItem::Separator && !mi->icon.isNull())
                    {
                        int iconExtent = proxy()->pixelMetric(PM_SmallIconSize, opt, widget);
                        sz.setHeight(qMax(sz.height(), mi->icon.actualSize(QSize(iconExtent, iconExtent)).height() + 2 * AdobePhotoshopStylePrivate::windowsItemFrame));
                    }
                    int maxpmw = mi->maxIconWidth;
                    int tabSpacing = 20;
                    if (mi->text.contains(QLatin1Char('\t')))
                        w += tabSpacing;
                    else if (mi->menuItemType == QStyleOptionMenuItem::SubMenu)
                        w += 2 * AdobePhotoshopStylePrivate::windowsArrowHMargin;
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

                    int checkcol = qMax<int>(maxpmw, AdobePhotoshopStylePrivate::windowsCheckMarkWidth); // Windows always shows a check column
                    w += checkcol;
                    w += int(AdobePhotoshopStylePrivate::windowsRightBorder) + sz.height();
                    sz.setWidth(w);
                }
            }
            break;
            case CT_MdiControls:
                if (const QStyleOptionComplex *styleOpt = qstyleoption_cast<const QStyleOptionComplex *>(opt))
                {
                    int height = d.m_heightMenuBar - 1;
                    int width = 0;
                    if (styleOpt->subControls & SC_MdiMinButton)
                        width += 19;
                    if (styleOpt->subControls & SC_MdiNormalButton)
                        width += 19;
                    if (styleOpt->subControls & SC_MdiCloseButton)
                        width += 19;
                    sz = QSize(width, height);
                }
                else
                    sz = QSize(60, 19);
                break;
            default:
                sz = QSize();
                break;
        }
    }
    if (sz.isEmpty())
    {
        if (d.isQtitanStyle())
            return QCommonStyle::sizeFromContents(type, opt, size, widget);
        else
            return QProxyStyle::sizeFromContents(type, opt, size, widget);
    }
    return sz;
}

/*! \reimp */
int AdobePhotoshopStyle::styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const
{
    //    if (widget && widget->inherits("QMdiSubWindow"))
    //        return QCommonStyle::styleHint(hint, opt, widget, shret);
    return CommonStyle::styleHint(hint, opt, widget, shret);
}
