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
#include <QPixmapCache>
#include <QStringBuilder>
#include <QDockWidget>
#include <QMdiArea>
#include <QComboBox>
#include <QPainter>
#include <qdrawutil.h>
#include <QDialog>
#include <QBitmap>
#include <QMenu>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif /* Q_OS_WIN*/
#include "QtnStyleHelperPrivate.h"
#include "QtnOffice2007StylePrivate.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

static const int iTextMargin = 3;
static const int splitActionPopupWidth = 26; // Split button drop down width in popups

/* Office2007StylePrivate */
Office2007StylePrivate::Office2007StylePrivate()
    : OfficeStylePrivate(), m_themeType(Office2007Style::Blue), m_fileButtonImageCount(3)
{
}

Office2007StylePrivate::~Office2007StylePrivate()
{
}

/*! \internal */
void Office2007StylePrivate::init()
{
    OfficeStylePrivate::init();
}

/*! \internal */
void Office2007StylePrivate::drawImagePart(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, bool alpha) const
{
    Q_UNUSED(alpha);
    if (rcDest.width() <= 0 || rcDest.height() <= 0 || rcSrc.width() <= 0 || rcSrc.height() <= 0)
        return;
    p.drawPixmap(rcDest, soSrc, rcSrc);
}

/*! \internal */
void Office2007StylePrivate::drawImage(const QPixmap& pixmap, QPainter& p, const QRect& rcDest, const QRect& rcSrc, QRect rcSizingMargins, QColor clrTransparent, bool alphaBlend) const
{
    if ((rcSizingMargins.top() + rcSizingMargins.bottom()) > rcSrc.height())
        return;

    bool alpha = pixmap.hasAlphaChannel();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = pixmap.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    QPixmap copy = QPixmap::fromImage(img);
#else
    QPixmap copy = pixmap;
    copy.setAlphaChannel(copy.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif

    drawPixmap(copy, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}

/*! \internal */
void Office2007StylePrivate::drawImage(const QPixmap& pixmap, QPainter& p, const QRect& rcDest, const QRect& rcSrc, QRect rcSizingMargins, bool alphaBlend) const
{
    bool alpha = pixmap.hasAlphaChannel();
    drawPixmap(pixmap, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}

static bool qtn_isRectNull(const QRect& rect) { return (rect.left() == 0 && rect.right() == 0 && rect.top() == 0 && rect.bottom() == 0); }

/*! \internal */
void Office2007StylePrivate::drawPixmap(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, bool alpha, QRect rcSizingMargins, bool alphaBlend) const
{
    Q_UNUSED(alphaBlend)
    QPixmap dcSrc = soSrc;
    if (rcSizingMargins.top() + rcSizingMargins.bottom() > rcSrc.height())
        return;

    if (dcSrc.isNull())
        return;

    if (rcDest.width() == rcSrc.width() - 1)
    {
        rcSizingMargins.setLeft(0);
        rcSizingMargins.setRight(0);
    }

    if (rcDest.height() == rcSrc.height() - 1)
    {
        rcSizingMargins.setTop(0);
        rcSizingMargins.setBottom(0);
    }

    QRect rcDestSizingMargins = rcSizingMargins;

    if (rcDest.left() >= rcDest.right() || rcDest.top() >= rcDest.bottom())
        return;


    if (::qtn_isRectNull(rcSizingMargins))
    {
        p.drawPixmap(rcDest, dcSrc, rcSrc);
    }
    else
    {
        class qtn_Rect : public QRect
        {
        public:
            qtn_Rect(int l, int t, int r, int b) { setLeft(l); setRight(r - 1); setTop(t); setBottom(b - 1); }
        };
        QRect rc = rcDest;
        rc.adjust(0, 0, 1, 1);
        QRect rcImage = rcSrc;
        rcImage.adjust(0, 0, 1, 1);

        // left-top
        drawImagePart(dcSrc, p, qtn_Rect(rc.left(), rc.top(), rc.left() + rcDestSizingMargins.left(), rc.top() + rcDestSizingMargins.top()),
            qtn_Rect(rcImage.left(), rcImage.top(), rcImage.left() + rcSizingMargins.left(), rcImage.top() + rcSizingMargins.top()), alpha);
        // top-center 
        drawImagePart(dcSrc, p, qtn_Rect(rc.left() + rcDestSizingMargins.left(), rc.top(), rc.right() - rcDestSizingMargins.right(), rc.top() + rcDestSizingMargins.top()),
            qtn_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.top(), rcImage.right() - rcSizingMargins.right(), rcImage.top() + rcSizingMargins.top()), alpha);
        // right-top 
        drawImagePart(dcSrc, p, qtn_Rect(rc.right() - rcDestSizingMargins.right(), rc.top(), rc.right(), rc.top() + rcDestSizingMargins.top()),
            qtn_Rect(rcImage.right() - rcSizingMargins.right(), rcImage.top(), rcImage.right(), rcImage.top() + rcSizingMargins.top()), alpha);
        // left-center
        drawImagePart(dcSrc, p, qtn_Rect(rc.left(), rc.top() + rcDestSizingMargins.top(), rc.left() + rcDestSizingMargins.left(), rc.bottom() - rcDestSizingMargins.bottom()),
            qtn_Rect(rcImage.left(), rcImage.top() + rcSizingMargins.top(), rcImage.left() + rcSizingMargins.left(), rcImage.bottom() - rcSizingMargins.bottom()), alpha);
        // center
        drawImagePart(dcSrc, p, qtn_Rect(rc.left() + rcDestSizingMargins.left(), rc.top() + rcDestSizingMargins.top(), rc.right() - rcDestSizingMargins.right(), rc.bottom() - rcDestSizingMargins.bottom()),
            qtn_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.top() + rcSizingMargins.top(), rcImage.right() - rcSizingMargins.right(), rcImage.bottom() - rcSizingMargins.bottom()), alpha);
        // right-center
        drawImagePart(dcSrc, p, qtn_Rect(rc.right() - rcDestSizingMargins.right(), rc.top() + rcDestSizingMargins.top(), rc.right(), rc.bottom() - rcDestSizingMargins.bottom()),
            qtn_Rect(rcImage.right() - rcSizingMargins.right(), rcImage.top() + rcSizingMargins.top(), rcImage.right(), rcImage.bottom() - rcSizingMargins.bottom()), alpha);
        // left-bottom
        drawImagePart(dcSrc, p, qtn_Rect(rc.left(), rc.bottom() - rcDestSizingMargins.bottom(), rc.left() + rcDestSizingMargins.left(), rc.bottom()),
            qtn_Rect(rcImage.left(), rcImage.bottom() - rcSizingMargins.bottom(), rcImage.left() + rcSizingMargins.left(), rcImage.bottom()), alpha);
        // bottom-center
        drawImagePart(dcSrc, p, qtn_Rect(rc.left() + rcDestSizingMargins.left(), rc.bottom() - rcDestSizingMargins.bottom(), rc.right() - rcDestSizingMargins.right(), rc.bottom()),
            qtn_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.bottom() - rcSizingMargins.bottom(), rcImage.right() - rcSizingMargins.right(), rcImage.bottom()), alpha);
        // right-bottom
        drawImagePart(dcSrc, p, qtn_Rect(rc.right() - rcDestSizingMargins.right(), rc.bottom() - rcDestSizingMargins.bottom(), rc.right(), rc.bottom()),
            qtn_Rect(rcImage.right() - rcSizingMargins.right(), rcImage.bottom() - rcSizingMargins.bottom(), rcImage.right(), rcImage.bottom()), alpha);
    }
}

/*! \internal */
void Office2007StylePrivate::drawDropDownGlyph(QPainter* p, QPoint pt, bool selected, bool popuped, bool enabled, bool vert, const QWidget* w) const
{
    Q_UNUSED(vert);
    Q_UNUSED(popuped);
    QPixmap soGlyphImage = getCachedPixmap(QStringLiteral("QComboBox"), QStringLiteral("Image_ToolbarButtonDropDownGlyph"), QSize(),
        !enabled ? 3 : selected ? 1 : 0, 4, QMargins(0, 0, 0, 0), QColor(0xFF, 0, 0xFF));

    if (soGlyphImage.isNull())
    {
        Q_ASSERT(false);
        return;
    }
    QRect rc(QPoint(pt.x() - 2, pt.y() - 2), QSize(soGlyphImage.size()));
    if (!CommonStylePrivate::isUse96Dpi())
    {
        rc.setWidth(dpiScaled(rc.width(), w));
        rc.setHeight(dpiScaled(rc.height(), w));
        bool wasSmoothPixmapTransform = p->renderHints() & QPainter::SmoothPixmapTransform;
        p->setRenderHint(QPainter::SmoothPixmapTransform, true);

        p->drawPixmap(rc, soGlyphImage);
        p->setRenderHint(QPainter::SmoothPixmapTransform, wasSmoothPixmapTransform);
    }
    else
        p->drawPixmap(rc, soGlyphImage);
}

/*! \internal */
QPixmap Office2007StylePrivate::loadPixmap(const QString& className, const QString& image) const
{
    return QPixmap(widgetPixmapName(className, image));
}

/*! \internal */
bool Office2007StylePrivate::drawPanelMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
    if (OfficeStylePrivate::drawPanelMenu(opt, painter, w))
        return true;

    drawWidgetPixmap(QStringLiteral("QMenu"), QStringLiteral("Image_PopupBarFrame"), opt->rect, painter, QMargins(4, 4, 4, 4), QColor(0xFF, 0, 0xFF));

    bool showGripper = true;
    // draw a Gripper or not
    QVariant var = w ? w->property("showGripper") : QVariant(false);
    showGripper = !var.isNull() ? var.toBool() : showGripper;

    if (showGripper)
    {
        const int menuPanelWidth = p.proxy()->pixelMetric(QStyle::PM_MenuPanelWidth, opt, w);
        const int iconSize = p.proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, w) + 2;
        QRect rcBorders(QPoint(menuPanelWidth, menuPanelWidth), QPoint(menuPanelWidth, menuPanelWidth));

        int x = rcBorders.left();
        int y = rcBorders.top() + 1;
        int cx = iconSize;
        int cy = opt->rect.bottom() - rcBorders.top() - rcBorders.bottom() - 1;
        // [1]
        StyleHelper::drawGradientFill(*painter, QRect(QPoint(x + 1, y), QSize(cx - 1, cy)), 
            opt->palette.window().color(), opt->palette.window().color(), true);

        painter->fillRect(x + cx - 1, y, 1, cy, opt->palette.light().color());
        painter->fillRect(x + cx, y, 1, cy, opt->palette.midlight().color());
    }
    return true;
}


/*! \internal */
bool Office2007StylePrivate::drawIndicatorCheckRadioButton(QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    bool isRadio = (element == QStyle::PE_IndicatorRadioButton);
    QString strClassName = isRadio ? QStringLiteral("QRadioBox") : QStringLiteral("QCheckBox");
    QString strImage = isRadio ? QStringLiteral("Image_ToolbarButtonRadioButton") : QStringLiteral("Image_ToolbarButtonCheckBox");
    int state = 0;
    const bool enabled = option->state & QStyle::State_Enabled;
    const bool checked = option->state & QStyle::State_On;
    const bool noChange = option->state & QStyle::State_NoChange;
    const bool selected = option->state & QStyle::State_MouseOver;
    const bool pressed = option->state & QStyle::State_Sunken;

    int stateChecked = checked ? 1 : 0;
    if (!isRadio && noChange)
        stateChecked = 2;

    if (!enabled)
        state = 3;
    else if (selected && pressed)
        state = 2;
    else if (selected)
        state = 1;

    if (stateChecked == 1)
        state += 4;

    if (stateChecked == 2)
        state += 8;
    drawWidgetPixmap(strClassName, strImage, option->rect, painter, state, isRadio ? 8 : 12);
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawIndicatorCheckBox(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    return drawIndicatorCheckRadioButton(QStyle::PE_IndicatorCheckBox, opt, painter, widget);
}

/*! \internal */
bool Office2007StylePrivate::drawIndicatorRadioButton(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    return drawIndicatorCheckRadioButton(QStyle::PE_IndicatorRadioButton, opt, painter, widget);
}

/*! \internal */
bool Office2007StylePrivate::drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_P(const Office2007Style);
    bool dis = !(opt->state & QStyle::State_Enabled);
    const int iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, w) + 4;
    QRect rc(QPoint(opt->rect.left() + opt->rect.width() / 2 - iconSize / 2,
        opt->rect.top() + opt->rect.height() / 2 - iconSize / 2), QSize(iconSize, iconSize));
    rc.adjust(2, 2, -2, -2);
    drawWidgetPixmap(QStringLiteral("QMenu"), QStringLiteral("Image_MenuCheckedItem"), rc, painter, !dis ? 0 : 1, 2, QMargins(2, 2, 2, 2));
    drawWidgetPixmap(QStringLiteral("QMenu"), QStringLiteral("Image_MenuCheckedItemMark"), rc.adjusted(2, 2, -2, -2), painter, !dis ? 0 : 1, 4, QMargins(0, 0, 0, 0));
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonToolBarControl")))
        return true;
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonStatusBar")))
    {
        QRect rect = opt->rect;
        rect.setRight(rect.left() + 1);
        rect.setTop(rect.top() - 2);
        return drawWidgetPixmap(QStringLiteral("RibbonStatusBar"), QStringLiteral("Image_StatusBarSeparator"), rect, painter, 0, 1, QMargins(2, 4, 2, 2));
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    painter->translate(opt->rect.x(), opt->rect.y());

    QRect rect(opt->rect);
    if (opt->state & QStyle::State_Horizontal)
    {
        for (int y = 4; y < rect.height() - 4; y += 4)
        {
            painter->fillRect(QRect(QPoint(3, y + 1), QPoint(4, y + 2)), QColor(234, 251, 251));
            painter->fillRect(QRect(QPoint(2, y), QPoint(3, y + 1)), opt->palette.dark().color());
        }
    }
    else
    {
        for (int x = 4; x < opt->rect.width() - 4; x += 4)
        {
            painter->fillRect(QRect(QPoint(x + 1, 3), QPoint(x + 2, 4)), QColor(234, 251, 251));
            painter->fillRect(QRect(QPoint(x, 2), QPoint(x + 1, 3)), opt->palette.dark().color());
        }
    }
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawFrameLineEdit(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool highlight = opt->state & QStyle::State_MouseOver;
        const bool hasFocus = opt->state & QStyle::State_HasFocus;

        bool usePalette = panel->palette.isBrushSet(panel->palette.currentColorGroup(), QPalette::Base);

        if (usePalette)
        {
            // we try to check if this lineedit is a delegate on a derived class.
            QPen oldPen = painter->pen();
            // Inner white border
            painter->setPen(QPen(opt->palette.base().color(), 1));
            painter->drawRect(opt->rect.adjusted(1, 1, -2, -2));
            // Outer dark border
            painter->setPen(QPen(opt->palette.shadow().color(), 1));
            painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            painter->setPen(oldPen);
        }
        else
        {
            QPen oldPen = painter->pen();
            QRegion clipRegion = opt->rect;
            clipRegion -= panel->rect.adjusted(2, 2, -2, -2);
            painter->setClipRegion(clipRegion);
            painter->setPen(panel->palette.color(enabled && (highlight || hasFocus) ? QPalette::Light : QPalette::Dark));
            painter->drawRect(panel->rect.adjusted(0, 0, -1, -1));
            painter->setPen(oldPen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawPanelLineEdit(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame *>(opt))
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool highlight = opt->state & QStyle::State_MouseOver;
        const bool hasFocus = opt->state & QStyle::State_HasFocus;

        painter->fillRect(panel->rect.adjusted(panel->lineWidth, panel->lineWidth, -panel->lineWidth, -panel->lineWidth),
            panel->palette.brush(enabled && (highlight || hasFocus) ? QPalette::HighlightedText : QPalette::Base));

        if (panel->lineWidth > 0)
            p.proxy()->drawPrimitive(QStyle::PE_FrameLineEdit, panel, painter, widget);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawPanelStatusBar(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    int widthPermanent = 0;
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonStatusBar")))
    {
        QRect var = widget->property("childrenPermanentRect").toRect();
        widthPermanent = !var.isNull() ? var.width() : widthPermanent;
    }

    QString fileName = widgetPixmapName(QStringLiteral("RibbonStatusBar"), QStringLiteral("Image_StatusBarDevider"));
    QPixmap soDevider(fileName);
    const int widthDevider = soDevider.isNull() ? 4 : soDevider.width();
    int nStatusHeight = opt->rect.height();
    QRect rc(opt->rect);
    int bRightOffset = widthPermanent == 0 ? widget->childrenRect().width() : widthPermanent;
    int nWidth = widthPermanent == 0 ? rc.width() : rc.width() - bRightOffset - widthDevider;

    QRect rcLight(QPoint(rc.left(), rc.top()), QPoint(nWidth, rc.bottom() + 1));
    drawWidgetPixmap(QStringLiteral("RibbonStatusBar"), QStringLiteral("Image_StatusBarLight"), rcLight, painter, 0, 1);

    if (widthPermanent > 0)
    {
        QRect rcDark(QPoint(nWidth + widthDevider, rc.bottom() - nStatusHeight + 1), QPoint(rc.right(), rc.bottom() + 1));
        drawWidgetPixmap(QStringLiteral("RibbonStatusBar"), QStringLiteral("Image_StatusBarDark"), rcDark, painter, 0, 1);

        QRect rcDevider(QPoint(nWidth, rc.bottom() - nStatusHeight + 1), QSize(/*rcSrc.width()*/4, rc.height() + 1));
        drawWidgetPixmap(QStringLiteral("RibbonStatusBar"), QStringLiteral("Image_StatusBarDevider"), rcDevider, painter, 0, 1);
    }
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
//    bool ret = drawGroupControlEntry(opt, painter, widget);
//    if (!ret)
    bool ret = drawRibbonPanelButton(opt, painter, widget);
    return ret;
}

/*! \internal */
bool Office2007StylePrivate::drawGroupControlEntry(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget == Q_NULL)
        return false;

    if (widget && widget->parentWidget() && widget->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonStatusBarSwitchGroup")))
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool checked = opt->state & QStyle::State_On;
        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;

        int index = widget->property(/*_qtn_Index_Group*/"IndexGroup").toInt();
        int count = widget->property(/*_qtn_Switch_Count*/"SwitchCount").toInt();

        int state = !enabled ? 5 : pressed ? 2 : checked ? (selected ? 4 : 3) : selected ? 1 : 0;

        QString imageName = widgetPixmapName(QStringLiteral("RibbonStatusBar"), QStringLiteral("Image_StatusBarSwitchGroup"));
        if (imageName.isEmpty())
            return false;

        QPixmap soImage(imageName);
        QRect rcSrc = CommonStylePrivate::sourceRectImage(soImage.rect(), state, 6);

        int srcWidth = rcSrc.width() / 4;
        int image = count == 1 ? 3 : index == 0 ? 0 : index == count - 1 ? 2 : 1;

        rcSrc.setLeft(srcWidth * image);
        rcSrc.setRight(rcSrc.left() + srcWidth - 1);
        drawImage(soImage, *painter, opt->rect, rcSrc, QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
        return true;
    }

    if (widget->property(/*_qtn_Group*/"ToolBarGroup").toBool())
    {
        if (const QStyleOptionToolButton*toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
        {
            if (toolbutton->subControls & QStyle::SC_ToolButtonMenu)
                return false;
        }

        bool beginGroup = widget->property(/*_qtn_Begin_Group*/"BeginToolBarGroup").toBool();
        bool endGroup = widget->property(/*_qtn_End_Group*/"EndToolBarGroup").toBool();
        bool wrap = widget->property(/*_qtn_Wrap*/"Wrap").toBool();
        bool left = beginGroup || wrap;
        bool right = endGroup;

        QString strImage = left && right ? QStringLiteral("Image_ToolbarButtonsSpecialSingle") :
            left  ? QStringLiteral("Image_ToolbarButtonsSpecialLeft") :
            right ? QStringLiteral("Image_ToolbarButtonsSpecialRight") :
            QStringLiteral("Image_ToolbarButtonsSpecialCenter");

        bool enabled = opt->state & QStyle::State_Enabled;
        bool checked = opt->state & QStyle::State_On;
        bool selected = opt->state & QStyle::State_MouseOver;
        bool pressed = opt->state & QStyle::State_Sunken;
        bool popuped = false;
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
            popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

        int state = 0;
        if (!enabled) state = 0;
        else if (popuped) state = 2 + 1;
        else if (checked && !selected && !pressed) state = 3;
        else if (checked && selected && !pressed) state = 4;
        else if ((selected && pressed)) state = 2;
        else if (selected || pressed) state = 1;
        drawWidgetPixmap(QStringLiteral("RibbonButton"), strImage, opt->rect, painter, state, 5, QMargins(2, 2, 2, 2), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawPanelButtonCommand(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(opt))
    {
        if (!btn->palette.isBrushSet(btn->palette.currentColorGroup(), QPalette::Base))
            return false;
        bool enabled = opt->state & QStyle::State_Enabled;
        bool checked = opt->state & QStyle::State_On;
        bool selected = opt->state & QStyle::State_MouseOver;
        bool pressed = opt->state & QStyle::State_Sunken;
        bool defaultBtn = btn->features & QStyleOptionButton::DefaultButton;

        int state = defaultBtn ? 4 : 0;

        if (!enabled)
            state = 3;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 1;
        else if ((selected && pressed)) state = 2;
        else if (selected || pressed) state = 1;
        drawWidgetPixmap(QStringLiteral("QPushButton"), QStringLiteral("Image_PushButton"), opt->rect,
            painter, state, 5, QMargins(4, 4, 4, 4), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawMenuBarItem(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionMenuItem* mbi = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        if (mbi->menuItemType == QStyleOptionMenuItem::DefaultItem)
            return true;

        QPixmap pix = mbi->icon.pixmap(p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, w), QIcon::Normal);

        uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, mbi, w))
            alignment |= Qt::TextHideMnemonic;

        bool enabled = opt->state & QStyle::State_Enabled;
        bool checked = opt->state & QStyle::State_On;
        bool selected = opt->state & QStyle::State_Selected;
        bool pressed = opt->state & QStyle::State_Sunken;
        bool popuped = (opt->state & QStyle::State_Selected) && (opt->state & QStyle::State_Sunken);

        drawRectangle(painter, opt->rect, selected, pressed, enabled, checked, popuped, TypeMenuBar, BarTop);

        QPalette::ColorRole textRole = !enabled ? QPalette::Text :
            selected ? QPalette::HighlightedText : QPalette::ButtonText;

        if (!pix.isNull())
            p.QStyle::drawItemPixmap(painter, mbi->rect, alignment, pix);
        else
            p.drawItemText(painter, mbi->rect, alignment, mbi->palette, mbi->state & QStyle::State_Enabled, mbi->text, textRole);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        int x, y, w, h;
        menuitem->rect.getRect(&x, &y, &w, &h);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        int tab = 0;
#else
        int tab = menuitem->tabWidth;
#endif
        const bool disabled = !(menuitem->state & QStyle::State_Enabled);
        bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable ? menuitem->checked : false;
        const bool activated = menuitem->state & QStyle::State_Selected;

        // windows always has a check column, regardless whether we have an icon or not
        int iconSize = p.proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
        int checkcol = iconSize;
#ifdef Q_OS_LINUX
        if (qobject_cast<const QComboBox*>(widget))
        {
            checkcol = 0;
            checked = false;
        }
#endif /* Q_OS_LINUX */
        if (activated)
            drawRectangle(painter, menuitem->rect, true/*selected*/, false/*pressed*/, !disabled/*enabled*/,
                false/*checked*/, false/*popuped*/, TypePopup, BarPopup);

//        if (menuitem->text.count(QString(_qtn_SplitActionPopup)) != 0)
//            drawSplitButtonPopup(painter, menuitem->rect, activated, !disabled, menuitem->checkType == QStyleOptionMenuItem::Exclusive);

        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            drawMenuItemSeparator(opt, painter, widget);
            return true;
        }

        const bool isSystemMenu = widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu"));
        QRect vCheckRect = p.visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));

        if (isSystemMenu)
        {
            iconSize = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > opt->rect.height())
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);
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

            if (!isSystemMenu)
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);

            QPixmap pixmap = preparePixmap(menuitem->icon, QSize(iconSize, iconSize), mode, checked ? QIcon::On : QIcon::Off, widget);

            QRect pmr(QPoint(0, 0), QSize(iconSize, iconSize));
            pmr.moveCenter(vCheckRect.center());
            if (isSystemMenu)
                pmr.translate(6, 0);

            painter->setPen(menuitem->palette.text().color());
            if (checked)
            {
                QRect vIconRect = p.visualRect(opt->direction, opt->rect, pmr);
                QRect rcChecked = menuitem->rect;

                rcChecked.setLeft(vIconRect.left());
                if (isSystemMenu)
                    rcChecked.translate(6, 0);
                rcChecked.setWidth(vIconRect.width());
                drawRectangle(painter, rcChecked.adjusted(-2, 0, 2, 0), false/*selected*/, true/*pressed*/, !disabled/*enabled*/,
                    true/*checked*/, false/*popuped*/, TypePopup, BarPopup);
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
            newMi.rect = p.visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x() + windowsItemFrame, menuitem->rect.y() + windowsItemFrame,
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
        QRect vTextRect = p.visualRect(opt->direction, menuitem->rect, textRect);
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
                QRect vShortcutRect = p.visualRect(opt->direction, menuitem->rect, QRect(textRect.topRight(),
                    QPoint(menuitem->rect.right(), textRect.bottom())));
                painter->setPen(opt->palette.text().color());
                painter->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                painter->setPen(discol);
                s = s.left(t);
            }

            QFont font = menuitem->font;
            if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                font.setBold(true);
            painter->setFont(font);
            painter->setPen(opt->palette.text().color());
            painter->drawText(vTextRect, text_flags, s.left(t));
            painter->setPen(discol);
            painter->restore();
        }

        // draw sub menu arrow
        if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            int dim = (h - 2 * windowsItemFrame) / 2;
            QStyle::PrimitiveElement arrow;
            arrow = (opt->direction == Qt::RightToLeft) ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
            xpos = x + w - windowsArrowHMargin - windowsItemFrame - dim;
            QRect  vSubMenuRect = p.visualRect(opt->direction, menuitem->rect, QRect(xpos, y + h / 2 - dim / 2, dim, dim));
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
void Office2007StylePrivate::drawMenuItemSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        if (menuitem->text.isEmpty())
        {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = p.proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
            const int yoff = (y - 1 + h / 2);
            int xIcon = iconSize;
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu")))
                xIcon = 0;

            QPen penSave = painter->pen();
            painter->setPen(menuitem->palette.dark().color());
            painter->drawLine(x + 4 + xIcon, yoff, x + w - 4, yoff);
            painter->setPen(menuitem->palette.light().color());
            painter->drawLine(x + 4 + xIcon, yoff + 1, x + w - 4, yoff + 1);
            painter->setPen(penSave);
        }
        else
        {
            painter->save();
            QRect rectSeparator = menuitem->rect.adjusted(1, 0, -1, 0);
            StyleHelper::drawGradientFill(*painter, rectSeparator, opt->palette.window().color(),
                opt->palette.window().color(), true);
            rectSeparator.setTop(rectSeparator.bottom() - 1);

            QRect rect1 = rectSeparator;
            QRect rect2;
            rect1.setTop(rect1.top() + rectSeparator.height() / 2 - 1);
            rect1.setBottom(rect1.top());
            rect2 = rect1;
            rect2.translate(0, 1);

            painter->fillRect(rect1, opt->palette.light().color());
            painter->fillRect(rect2, opt->palette.midlight().color());

            QRect rectText = menuitem->rect;
            rectText.adjust(iTextMargin, 0, -iTextMargin, -iTextMargin);

            int alignment = Qt::AlignVCenter | Qt::TextSingleLine;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                alignment |= Qt::TextHideMnemonic;

            QFont font = menuitem->font;
            font.setBold(true);
            painter->setFont(font);

            painter->setPen(opt->palette.text().color());
            painter->drawText(rectText, alignment, menuitem->text);
            painter->restore();
        }
    }
}

/*! \internal */
bool Office2007StylePrivate::drawToolBoxTabShape(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    QPixmap soImage = loadPixmap(QStringLiteral("QToolBox"), QStringLiteral("Image_ShortcutBarItems"));
    if (soImage.isNull())
    {
        Q_ASSERT(false);
        return false;
    }
    int state = 0;
    if (opt->state & QStyle::State_Sunken)
        state = 3;
    else if (opt->state & QStyle::State_MouseOver)
        state = 2;
    drawImage(soImage, *painter, opt->rect, sourceRectImage(soImage.rect(), state, 4), QRect(QPoint(2, 2), QPoint(2, 2)));
    painter->fillRect(opt->rect.left(), opt->rect.bottom() - 1, opt->rect.width(), 1, opt->palette.shadow());
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawDockWidgetTitle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
    if (w && !w->inherits("QDockWidget"))
        return OfficeStylePrivate::drawDockWidgetTitle(opt, painter, w);

    if (const QStyleOptionDockWidget* dwOpt = qstyleoption_cast<const QStyleOptionDockWidget*>(opt))
    {
        QRect rect = opt->rect;
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        const QStyleOptionDockWidgetV2* v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(dwOpt);
#else
        const QStyleOptionDockWidget* v2 = qstyleoption_cast<const QStyleOptionDockWidget*>(dwOpt);
#endif // QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
        bool verticalTitleBar = v2 == Q_NULL ? false : v2->verticalTitleBar;
        bool active = false;

        // [1]
        StyleHelper::drawGradientFill(*painter, opt->rect, active ? dwOpt->palette.color(QPalette::Midlight) : dwOpt->palette.color(QPalette::Midlight),
            active ? dwOpt->palette.color(QPalette::Dark) : dwOpt->palette.color(QPalette::Dark), !verticalTitleBar);

        QPen pen(dwOpt->palette.color(QPalette::Light));
        painter->setPen(pen);
        painter->drawRect(rect.adjusted(0, 0, -1, -1));

        if (verticalTitleBar)
        {
            QSize s = rect.size();
            s.transpose();
            rect.setSize(s);
            painter->translate(rect.left() - 1, rect.top() + rect.width());
            painter->rotate(-90);
            painter->translate(-rect.left() + 1, -rect.top());
        }

        bool floating = false;
        if (dwOpt->movable)
        {
            if (w && w->isWindow())
                floating = true;
        }

        if (!dwOpt->title.isEmpty())
        {
            QFont oldFont = painter->font();

            if (floating)
            {
                QFont font = oldFont;
                font.setBold(true);
                painter->setFont(font);
            }

            QPalette palette = dwOpt->palette;
//            QPalette_setColor(palette, QPalette::Window, d.m_clrNormalCaptionText);
//            QPalette_setColor(palette, QPalette::WindowText, d.m_clrActiveCaptionText);
//            QPalette_setColor(palette, QPalette::BrightText, d.m_clrActiveCaptionText);

            int buttonMargin = 4;
            int mw = p.proxy()->pixelMetric(QStyle::PM_DockWidgetTitleMargin, dwOpt, w);
            int fw = p.proxy()->pixelMetric(QStyle::PM_DockWidgetFrameWidth, dwOpt, w);
            const QDockWidget *dw = qobject_cast<const QDockWidget *>(w);
            bool isFloating = dw != Q_NULL && dw->isFloating();

            QRect r = opt->rect.adjusted(0, 2, -1, -3);
            QRect titleRect = r;

            if (dwOpt->closable) {
                QSize sz = p.standardIcon(QStyle::SP_TitleBarCloseButton, dwOpt, w).actualSize(QSize(10, 10));
                titleRect.adjust(0, 0, -sz.width() - mw - buttonMargin, 0);
            }

            if (dwOpt->floatable) {
                QSize sz = p.standardIcon(QStyle::SP_TitleBarMaxButton, dwOpt, w).actualSize(QSize(10, 10));
                titleRect.adjust(0, 0, -sz.width() - mw - buttonMargin, 0);
            }

            if (isFloating) {
                titleRect.adjust(0, -fw, 0, 0);
                if (w != Q_NULL && w->windowIcon().cacheKey() != QApplication::windowIcon().cacheKey())
                    titleRect.adjust(titleRect.height() + mw, 0, 0, 0);
            }
            else {
                titleRect.adjust(mw, 0, 0, 0);
                if (!dwOpt->floatable && !dwOpt->closable)
                    titleRect.adjust(0, 0, -mw, 0);
            }
            if (!verticalTitleBar)
                titleRect = p.visualRect(dwOpt->direction, r, titleRect);

            QString titleText = painter->fontMetrics().elidedText(dwOpt->title, Qt::ElideRight,
                verticalTitleBar ? titleRect.height() : titleRect.width());

            const int indent = 4;
            p.proxy()->drawItemText(painter, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
                dwOpt->state & QStyle::State_Enabled, titleText, QPalette::WindowText);

//            p.proxy()->drawItemText(painter, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
//                dwOpt->state & QStyle::State_Enabled, titleText,
//                floating ? (active ? QPalette::BrightText : QPalette::Window) : active ? QPalette::WindowText : QPalette::Window);
            painter->setFont(oldFont);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("StatusButton")))
    {
        if (m_themeType == Office2007Style::Black)
        {
            if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
            {
                if (opt->state & QStyle::State_MouseOver)
                {
                    QStyleOptionToolButton tButton = *toolbutton;
                    QPalette_setColor(tButton.palette, QPalette::ButtonText, toolbutton->palette.color(QPalette::HighlightedText));
                    p.QCommonStyle::drawControl(QStyle::CE_ToolButtonLabel, &tButton, painter, widget);
                    return true;
                }
            }
        }
    }
    return OfficeStylePrivate::drawToolButtonLabel(opt, painter, widget);
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonFrameGroups(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const RibbonStyleOption* styleOption = qstyleoption_cast<const RibbonStyleOption*>(opt))
    {
        const bool isRibbonBar = widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("RibbonBar"));
        QRect rect = opt->rect;

        if (isRibbonBar)
        {
            //It is needed to remove one pixel-line between tab bar and pages in Office 2007/2010 style.
            if (styleOption->tabBarPosition == 2)
                rect.adjust(0, -1, 0, 1);
            else
                rect.adjust(0, -1, 0, 1);
        }

        if (styleOption->pageColor != Qtitan::ContextColorNone)
        {
            QString resName(QStringLiteral("Image_RibbonFrameGroups"));
            resName += CommonStyle::contextColorToString(styleOption->pageColor);
            QPixmap image = loadPixmap(QStringLiteral("RibbonBar"), resName);
            if (!image.isNull())
                drawImage(image, *painter, rect, sourceRectImage(image.rect(), 0, 1), image.width() < 5 ? QRect(QPoint(1, 8), QPoint(1, 8)) : QRect(QPoint(8, 8), QPoint(8, 8)));
        }
        else
        {
            QPixmap pixmap = getCachedPixmap(QStringLiteral("RibbonBar"), QStringLiteral("Image_RibbonFrameGroups"), rect.size(), 0, 1, QMargins(4, 4, 4, 4));
            if (styleOption->tabBarPosition == 2)
                pixmap = pixmap.transformed(QTransform().rotate(180));
            painter->drawPixmap(rect, pixmap);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonFrameGroup(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const RibbonGroupStyleOption* groupStyleOption = qstyleoption_cast<const RibbonGroupStyleOption*>(opt))
    {
        QRect rcEntryCaption = opt->rect;
        rcEntryCaption.setTop(rcEntryCaption.bottom() - groupStyleOption->heightCaption);
        //rcEntryCaption.adjust(0, 0, 0, -2);

        QPixmap image, imageCaption;
        if (groupStyleOption->pageColor == Qtitan::ContextColorNone)
        {
            if (groupStyleOption->heightCaption > 0)
            {
                image = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupClient"));
                imageCaption = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupCaption"));
            }
            else
                image = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupClientButtom"));

            if (image.isNull())
                image = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupClient"));
        }
        else
        {
            QString resName(QStringLiteral("Image_ContextPageGroupClient"));
            resName += CommonStyle::contextColorToString(groupStyleOption->pageColor);
            image = loadPixmap(QStringLiteral("RibbonGroup"), resName);
            if (image.isNull())
            {
                image = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupClient"));
                imageCaption = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupCaption"));
            }
        }

        QRect rcEntry(groupStyleOption->rect);
        if (!imageCaption.isNull())
            rcEntry.setBottom(rcEntry.bottom() - groupStyleOption->heightCaption);

        const int state = opt->state & QStyle::State_MouseOver ? 1 : 0;

        if (!image.isNull())
            drawImage(image, *painter, rcEntry, sourceRectImage(image.rect(), state, 2), QRect(QPoint(5, 5), QPoint(5, 5)), QColor(0xFF, 0, 0xFF), (state == 1 && true));

        if (!imageCaption.isNull())
            drawImage(imageCaption, *painter, rcEntryCaption, sourceRectImage(imageCaption.rect(), state, 2), QRect(QPoint(5, 5), QPoint(5, 5)), QColor(0xFF, 0, 0xFF));

        QRect rcGroup(groupStyleOption->rect);
        QRect rcSeparator(QPoint(rcGroup.right() - 1, rcGroup.top() + 2), QPoint(rcGroup.right() + 1, rcGroup.bottom()));
        drawWidgetPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupSeparator"), rcSeparator, painter, 0, 1, QMargins(0, 0, 0, 0));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonPanelButton(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageButton")))
    {
        Q_ASSERT(false);
//        drawPanelBackstageButton(opt, p, widget);
        return true;
    }

    if (drawGroupControlEntry(opt, painter, widget))
        return true;

    if (widget != Q_NULL)
    {
        if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
        {
            bool smallSize = toolbutton->toolButtonStyle != Qt::ToolButtonTextUnderIcon;
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonButton")))
            {
                if (!opt->rect.isEmpty())
                {
                    const int minIcon = qMin(opt->rect.height(), opt->rect.width());
                    const int actualArea = minIcon*minIcon;

                    int index = -1;
                    int curArea = 0;
                    QList<QSize> lSz = toolbutton->icon.availableSizes();
                    if (lSz.count() > 1)
                    {
                        for (int i = 0, count = lSz.count(); count > i; i++)
                        {
                            QSize curSz = lSz[i];
                            int area = curSz.height()*curSz.width();
                            if (actualArea > area)
                            {
                                if (area > curArea)
                                    index = i;
                                curArea = area;
                            }
                        }
                        int iconExtent = index != -1 ? toolbutton->icon.actualSize(lSz[index]).height() : smallSize;
                        smallSize = toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon ? iconExtent < 32 : iconExtent;
                    }
                }
            }

            const bool enabled = opt->state & QStyle::State_Enabled;
            const bool checked = opt->state & QStyle::State_On;
            const bool selected = opt->state & QStyle::State_MouseOver;
            const bool mouseInSplit = opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButton;
            const bool mouseInSplitDropDown = opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu;
            const bool pressed = opt->state & QStyle::State_Sunken;
            const bool popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

            if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup))
            {
                bool autoRaise = toolbutton->state & QStyle::State_AutoRaise;
                if (!autoRaise && !widget->parentWidget()->inherits("QStatusBar"))
                {
                    QStyleOptionButton tmpBtn;// = *btn;
                    tmpBtn.state = toolbutton->state;
                    tmpBtn.rect = toolbutton->rect;
                    p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &tmpBtn, painter, widget);
                }
                else
                {
                    int state = -1;
                    if (!enabled) state = -1;

                    else if (popuped) state = 2;
                    else if (checked && !selected && !pressed) state = 2;
                    else if (checked && selected && !pressed) state = 3;
                    else if (selected && pressed) state = 1;
                    else if (selected || pressed) state = 0;
                    if (state != -1)
                        drawWidgetPixmap(QStringLiteral("RibbonButton"), smallSize ? QStringLiteral("Image_ToolbarButtons22") : QStringLiteral("Image_ToolbarButtons50"),
                            opt->rect, painter, state, 4, QMargins(8, 8, 8, 8));
                }
                return true;
            }
            bool toolBarControl = widget->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonToolBarControl"));
            if (toolBarControl)
                smallSize = true;

            int specialOffset = 0;
            QString strImageSplit, strImageSplitDropDown;
            if (toolBarControl && widget->property(/*_qtn_Group*/"ToolBarGroup").toBool())
            {
                bool beginGroup = widget->property(/*_qtn_Begin_Group*/"BeginToolBarGroup").toBool();
                bool endGroup = widget->property(/*_qtn_End_Group*/"EndToolBarGroup").toBool();
                bool wrap = widget->property(/*_qtn_Wrap*/"Wrap").toBool();
                bool left = beginGroup || wrap;

                bool right = endGroup;/// indexNext <= rg->qtn_d().getIndexWidget(widget) || beginGroupNext || wrapNext;
                strImageSplit = left ? QStringLiteral("Image_ToolbarButtonsSpecialSplitLeft") : QStringLiteral("Image_ToolbarButtonsSpecialSplitCenter");
                strImageSplitDropDown = right ? QStringLiteral("Image_ToolbarButtonsSpecialSplitDropDownRight") : QStringLiteral("Image_ToolbarButtonsSpecialSplitDropDownCenter");
                specialOffset = 1;
            }
            else
            {
                strImageSplit = smallSize ? QStringLiteral("Image_ToolbarButtonsSplit22") : QStringLiteral("Image_ToolbarButtonsSplit50");
                strImageSplitDropDown = smallSize ? QStringLiteral("Image_ToolbarButtonsSplitDropDown22") : QStringLiteral("Image_ToolbarButtonsSplitDropDown50");

            }

            QRect rcButton = opt->rect;
            QRect popupr = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, widget);

            QRect rcSplit = smallSize ? QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right() - popupr.width() - 2, rcButton.bottom()))
                : QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right(), rcButton.bottom() - popupr.height() - 2));

            int stateSplit = -1;

            if (/*enabledSplit && (selected || pressed || popuped)*/ false)
            {
                stateSplit = 4;
            }
            else if (!enabled)
            {
//              if (isKeyboardSelected(selected)) stateSplit = 5;
            }
            else if (checked)
            {
                if (popuped) stateSplit = 5;
                else if (!selected && !pressed) stateSplit = 2;
                else if (selected && !pressed) stateSplit = 3;
                else if ((selected && pressed)) stateSplit = 1;
                else if (pressed) stateSplit = 2;
                else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
            }
            else
            {
                if (popuped) stateSplit = 5;
                else if ((selected && pressed)) stateSplit = 1;
                else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
            }
            stateSplit += specialOffset;

            if (stateSplit != -1)
            {
                if (smallSize)
                    drawWidgetPixmap(QStringLiteral("RibbonButton"), strImageSplit, rcSplit, painter, stateSplit, 6 + specialOffset, QMargins(2, 2, 2, 2), QColor(0xFF, 0, 0xFF));
                else
                    drawWidgetPixmap(QStringLiteral("RibbonButton"), strImageSplit, rcSplit, painter, stateSplit, 6 + specialOffset, QMargins(2, 2, 2, 2));
            }

            QRect rcSplitDropDown = smallSize ? QRect(QPoint(rcButton.right() - popupr.width() - 1, rcButton.top()), QPoint(rcButton.right(), rcButton.bottom())) :
                QRect(QPoint(rcButton.left(), rcButton.bottom() - popupr.height() - 1), QPoint(rcButton.right(), rcButton.bottom()));

            int stateSplitDropDown = -1;

            if (/*enabledDropDown && (selected || pressed || popuped)*/ false)
            {
                stateSplitDropDown = 3;
            }
            else if (!enabled)
            {
//            if (isKeyboardSelected(selected)) stateSplitDropDown = 4;
            }
            else if (checked)
            {
                if (popuped) stateSplitDropDown = 2;
                else if (!selected && !pressed) stateSplitDropDown = 2;
                else if ((selected && pressed)) stateSplitDropDown = 0;
                else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
                else stateSplitDropDown = 4;
            }
            else
            {
                if (popuped) stateSplitDropDown = 2;
                else if ((selected && pressed)) stateSplitDropDown = 0;
                else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
            }

            stateSplitDropDown += specialOffset;
            if (stateSplitDropDown != -1)
            {
                if (smallSize)
                    drawWidgetPixmap(QStringLiteral("RibbonButton"), strImageSplitDropDown, rcSplitDropDown, painter,
                        stateSplitDropDown, 5 + specialOffset, QMargins(2, 2, 2, 2), QColor(0xFF, 0, 0xFF));
                else
                    drawWidgetPixmap(QStringLiteral("RibbonButton"), strImageSplitDropDown, rcSplitDropDown, painter,
                        stateSplitDropDown, 5 + specialOffset, QMargins(2, 2, 2, 2), QColor(0xFF, 0, 0xFF));
            }
            return true;
        }
    }
    else if (const CaptionButtonStyleOption* toolbutton = qstyleoption_cast<const CaptionButtonStyleOption*>(opt))
    {
        const bool down = opt->state & QStyle::State_Sunken;
        const bool select = opt->state & QStyle::State_MouseOver;
//        const bool active = opt->state & QStyle::State_Active;
//        const int state = !active ? 3 : down && select ? 2 : select || down ? 1 : 0;

        if (down || select)
        {
/*
            QPixmap soImageButton;
            if (toolbutton->activeSubControls == QStyle::SC_TitleBarCloseButton)
                soImageButton = cached("FrameTitleCloseButton.png");

            if (soImageButton.isNull())
                soImageButton = cached("FrameTitleButton.png");

            drawImage(soImageButton, *p, toolbutton->rect, sourceRectImage(soImageButton.rect(), down || select ? 1 : 0, 2),
                QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
*/
        }

        QString strGlyphSize(QStringLiteral("17.png"));
        if (toolbutton->rect.width() < 27) strGlyphSize = QStringLiteral("17.png");
        if (toolbutton->rect.width() < 15) strGlyphSize = QStringLiteral("13.png");

        QString strButton;
        if (toolbutton->activeSubControls == QStyle::SC_TitleBarCloseButton)
            strButton = QStringLiteral("FrameTitleClose");
        else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMaxButton)
            strButton = QStringLiteral("FrameTitleMaximize");
        else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMinButton)
            strButton = QStringLiteral("FrameTitleMinimize");
        else if (toolbutton->activeSubControls == QStyle::SC_TitleBarNormalButton)
            strButton = QStringLiteral("FrameTitleRestore");

        strButton += strGlyphSize;
/*
        QPixmap soImageIndicator = cached(strButton);
        if (!soImageIndicator.isNull())
        {
            QRect rcSrc = sourceRectImage(soImageIndicator.rect(), state, 5);
            QRect rcGlyph(QPoint((toolbutton->rect.right() + toolbutton->rect.left() - rcSrc.width() + 1) / 2, (toolbutton->rect.top() + toolbutton->rect.bottom() - rcSrc.height() + 2) / 2), rcSrc.size());
            drawImage(soImageIndicator, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        }
*/
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonOptionButton(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    if (const QStyleOptionToolButton* opt = qstyleoption_cast<const QStyleOptionToolButton*>(option))
    {
        QPixmap soRibbonOptionButton = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonOptionButton"));
        if (soRibbonOptionButton.isNull())
            return OfficeStylePrivate::drawRibbonOptionButton(option, painter, w);

        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;
        const bool enabled = opt->state & QStyle::State_Enabled;

        int state = 0;
        if (!enabled) state = 3;
        else if (pressed) state = 2;
        else if (selected) state = 1;

        QRect rcSrc = sourceRectImage(soRibbonOptionButton.rect(), state, 4);
        QRect rect = opt->rect;
        QImage img = soRibbonOptionButton.toImage();
        img.setAlphaChannel(img.createMaskFromColor(QColor(0xFF, 0, 0xFF).rgb(), Qt::MaskOutColor));
        QPixmap copyPix = QPixmap::fromImage(img);
        painter->drawPixmap(rect, copyPix, rcSrc);
// TODO       drawImage(soRibbonOptionButton, *painter, rect, rcSrc, QRect(QPoint(3, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return OfficeStylePrivate::drawRibbonOptionButton(option, painter, w);
}

/*! \internal */
bool Office2007StylePrivate::drawWorkspace(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (opt == Q_NULL)
        return false;
    QPixmap soWorkspace = loadPixmap(QStringLiteral("RibbonWorkspace"), QStringLiteral("Image_WorkspaceTopLeft"));
    if (!soWorkspace.isNull())
    {
        const QRect rc = opt->rect;
        const QRect rcSrc = soWorkspace.rect();
        QRect rcTopLeft = opt->rect;
        rcTopLeft.setBottom(rcTopLeft.top() + rcSrc.height());
        rcTopLeft.setRight(rcTopLeft.left() + qMax(rcTopLeft.width(), rcSrc.width()));
        drawImage(soWorkspace, *painter, rcTopLeft, rcSrc, QRect(QPoint(rcSrc.width() - 1, 0), QPoint(0, 0)));

        const QRect rcFill(QPoint(rc.left(), rc.top() + rcSrc.height()), QPoint(rc.right(), rc.bottom()));
        const QRect rcFillTop(QPoint(rcFill.left(), rcFill.top()), QPoint(rcFill.right() + 1, rcFill.top() + rcFill.height() * 2 / 3));
        const QRect rcFillBottom(QPoint(rcFill.left(), rcFillTop.bottom()), QPoint(rcFill.right() + 1, rcFill.bottom()));
        StyleHelper::drawGradientFill(*painter, rcFillTop, opt->palette.light().color(), opt->palette.mid().color(), true);
        StyleHelper::drawGradientFill(*painter, rcFillBottom, opt->palette.mid().color(), opt->palette.dark().color(), true);
        return true;
    }
    else
    {
        painter->fillRect(opt->rect, opt->palette.window().color());
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonSliderButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
//    QPixmap soButton = opt->direction == Qt::RightToLeft ? QPixmap("SliderUp.png") : QPixmap("SliderDown.png");
    QString strButton = opt->direction == Qt::RightToLeft ? QStringLiteral("Image_SliderUp") : QStringLiteral("Image_SliderDown");
    const bool selected = opt->state & QStyle::State_MouseOver;
    const bool pressed = opt->state & QStyle::State_Sunken;
    int state = 0;
    if (pressed)
        state = 2;
    else if (selected)
        state = 1;
//    drawImage(soButton, *p, opt->rect, sourceRectImage(soButton.rect(), state, 3));
    drawWidgetPixmap(QStringLiteral("QSlider"), strButton, opt->rect, painter, state, 3);
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonSysFrameMenu(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const RibbonFrameMenuStyleOption* menuOpt = qstyleoption_cast<const RibbonFrameMenuStyleOption*>(opt))
    {
        QRect rect = menuOpt->rect;
        QMargins margins = menuOpt->margins;

        QColor colorTop1 = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Midlight);
        QColor colorTop2 = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Dark);
        QLinearGradient gradTop(QPoint(rect.left(), rect.top()), QPoint(rect.left(), margins.top()));
        gradTop.setColorAt(0, colorTop1);
        gradTop.setColorAt(1, colorTop2);
        painter->fillRect(QRect(rect.left(), rect.top(), rect.right(), margins.top()), gradTop);

        QColor colorBottom1 = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Mid);
        QColor colorBottom2 = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Light);
        QLinearGradient gradBottom(QPoint(rect.left(), margins.top()),
            QPoint(rect.left(), rect.bottom() - margins.top() - margins.bottom() / 2));
        gradBottom.setColorAt(0, colorBottom1);
        gradBottom.setColorAt(1, colorBottom2);
        painter->fillRect(QRect(rect.left(), margins.top(),
            rect.right(), rect.bottom() - margins.top() - margins.bottom() / 2), gradBottom);

        QColor colorMostBottom1 = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Shadow);
        QColor colorMostBottom2 = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Window);
        QLinearGradient gradMostBottom(QPoint(rect.left(), rect.bottom() - margins.bottom() / 2),
            QPoint(rect.left(), rect.bottom()));
        gradMostBottom.setSpread(QGradient::ReflectSpread);
        gradMostBottom.setColorAt(0, colorMostBottom1);
        gradMostBottom.setColorAt(1, colorMostBottom2);
        painter->fillRect(QRect(rect.left(), rect.bottom() - margins.bottom() / 2,
            rect.right(), rect.bottom()), gradMostBottom);

        QColor colorCenter = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Base);
        painter->fillRect(QRect(rect.left() + margins.left(), margins.top(),
            rect.right() - margins.right() * 2, rect.bottom() - margins.top() - margins.bottom()), colorCenter);

        QColor colorOuterCenterBorder = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::ButtonText);
        QtnPen penOuterCenterBorder(painter, colorOuterCenterBorder);
        painter->drawRect(QRect(rect.left() + margins.left(), margins.top(), rect.right() - margins.right() * 2, rect.bottom() - margins.top() - margins.bottom()).adjusted(-1, -1, 1, 1));

        QColor colorInnerCenterBorder = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::WindowText);
        QtnPen penInnerCenterBorder(painter, colorInnerCenterBorder);
        painter->drawRect(QRect(rect.left() + margins.left(), margins.top(), rect.right() - margins.right() * 2, rect.bottom() - margins.top() - margins.bottom()).adjusted(-2, -2, 2, 2));

        QColor colorOuterBorder = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::AlternateBase);
        QtnPen penOuterBorder(painter, colorOuterBorder);
        painter->drawRect(rect.adjusted(0, 0, -1, -1));

        QColor colorInnerBorder = colorSubControl(QStringLiteral("RibbonSystemMenu"), QStringLiteral("RibbonSystemMenu_Menu"), QPalette::Active, QPalette::Button);
        QtnPen penInnerBorder(painter, colorInnerBorder);
        painter->drawRect(rect.adjusted(1, 1, -2, -2));

        if (!menuOpt->isAdditionalPages)
        {
            int hMargin = 0;//p.proxy()->pixelMetric(QStyle::PM_MenuHMargin, Q_NULL, w) * 2;
            painter->fillRect(QRect(QPoint(menuOpt->actionsWidth + menuOpt->margins.left() - hMargin, menuOpt->margins.top()),
                QSize(1, (menuOpt->actionsHeight + menuOpt->margins.top()) - menuOpt->margins.bottom() * 0.5 - 2)), opt->palette.mid());
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonSysMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const StyleOptionMenuScrollItem* menuitem = qstyleoption_cast<const StyleOptionMenuScrollItem *>(opt))
    {
        QString textItem = menuitem->text;
        if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup")))
            textItem = textItem + QLatin1Char('\n') + menuitem->toolTip;

        int x, y, w, h;
        menuitem->rect.getRect(&x, &y, &w, &h);
        int tab = menuitem->tabWidth;
        bool dis = !(menuitem->state & QStyle::State_Enabled);
        bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable ? menuitem->checked : false;
        bool act = menuitem->state & QStyle::State_Selected;

        const bool isSystemMenu = widget && (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")) || widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup")));
        // windows always has a check column, regardless whether we have an icon or not
        int iconSize = p.proxy()->pixelMetric(isSystemMenu ? QStyle::PM_ToolBarIconSize : QStyle::PM_SmallIconSize, opt, widget);
        int checkcol = isSystemMenu ? iconSize : qMax<int>(menuitem->maxIconWidth, windowsCheckMarkWidth);
            
        QBrush fill;
        if (act)
        {
            drawRectangle(painter, menuitem->rect, true/*selected*/, false/*pressed*/, !dis/*enabled*/,
                false/*checked*/, false/*popuped*/, TypePopup, BarPopup);
        }
        else
        {
            fill = Qt::transparent;
            painter->fillRect(menuitem->rect.adjusted(0, 0, -1, 0), fill);
        }

        if (textItem.count(QStringLiteral(_qtn_SplitActionPopup)) != 0)
            drawSplitButtonPopup(painter, menuitem->rect, act, !dis, menuitem->checkType == QStyleOptionMenuItem::Exclusive);
            
        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            int yoff = y - 1 + h / 2;
            int xIcon = iconSize;
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu")))
                xIcon = 0;
            painter->setPen(opt->palette.brush(QPalette::Midlight).color().darker(105));
            painter->drawLine(x + 2 + xIcon, yoff, x + w - 4, yoff);
            painter->setPen(opt->palette.brush(QPalette::Light).color());
            painter->drawLine(x + 2 + xIcon, yoff + 1, x + w - 4, yoff + 1);
            return true;
        }

        if (isSystemMenu)
        {
            iconSize = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > opt->rect.height())
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);
            checkcol = iconSize + 6;
        }

        // On Windows Style, if we have a checkable item and an icon we
        // draw the icon recessed to indicate an item is checked. If we
        // have no icon, we draw a checkmark instead.
        if (!menuitem->icon.isNull())
        {
            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            if (act && !dis)
                mode = QIcon::Active;

            int smollIconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);
            QPixmap pixmap = preparePixmap(menuitem->icon, QSize(smollIconSize, smollIconSize), mode, checked ? QIcon::On : QIcon::Off, widget);

            if (isSystemMenu)
            {
                QRect vCheckRect = p.visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));
                QRect pmr(QPoint(0, 0), QSize(iconSize, iconSize));
                pmr.moveCenter(vCheckRect.center());
                painter->drawPixmap(pmr.topLeft(), pixmap);
            }
            else
            {
                QRect pmr(0, 0, pixmap.width(), pixmap.height());
                QRect vCheckRect = p.visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));
                pmr.moveCenter(vCheckRect.center());
                painter->setPen(menuitem->palette.text().color());
                painter->drawPixmap(pmr.topLeft(), pixmap);
            }
        }

        QPalette pal = opt->palette;
        painter->setPen(pal.color(QPalette::WindowText));

        QColor discol;
        if (dis)
        {
            discol = pal.color(QPalette::Disabled, QPalette::WindowText);//pal.color(QPalette::Dark).dark(130);
            painter->setPen(discol);
        }

        int xm = int(windowsItemFrame) + checkcol + int(windowsItemHMargin);
        int xpos = menuitem->rect.x() + xm;

        QStringList split = textItem.split(QStringLiteral("\n"));
        QString s = split.count() > 0 ? split.at(0) : textItem;
        if (split.count() > 1)
        {
            int height = menuitem->fontMetrics.height();
            QRect textRect(xpos, y + windowsItemVMargin + 2, w - xm - windowsRightBorder - tab + 1, height);
            QRect vTextRect = p.visualRect(opt->direction, menuitem->rect, textRect);

            QFont font = menuitem->font;
            QFont saveFont = font;
            font.setBold(true);
            painter->setFont(font);

            int t = s.indexOf(QLatin1Char('\t'));
            int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, menuitem, widget))
                text_flags |= Qt::TextHideMnemonic;
            text_flags |= Qt::AlignLeft;

            // draw hotkeys
            if (t >= 0)
            {
                QRect vShortcutRect = p.visualRect(opt->direction, menuitem->rect, QRect(textRect.topRight(),
                    QPoint(menuitem->rect.right(), textRect.bottom())));

                painter->setPen(opt->state & QStyle::State_Enabled ? pal.color(QPalette::Normal, QPalette::WindowText) : discol);
                painter->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                s = s.left(t);
            }

            painter->setPen(opt->state & QStyle::State_Enabled ? pal.color(QPalette::Normal, QPalette::WindowText) : discol);
            painter->drawText(vTextRect, text_flags, s.left(t));
            painter->setFont(saveFont);

            if (split.count() > 1)
            {
                text_flags |= Qt::TextWordWrap;
                QRect textTipRect(xpos, y + vTextRect.height(), w - xm - 1, opt->rect.height());
                QRect rc = menuitem->fontMetrics.boundingRect(textTipRect, text_flags, split.at(1));
                QRect vTextTipRect = p.visualRect(opt->direction, menuitem->rect, rc);
                vTextTipRect.setTop(y + vTextRect.height());
                vTextTipRect.setWidth(textTipRect.width());
                painter->setPen(opt->state & QStyle::State_Enabled ? pal.color(QPalette::Normal, QPalette::WindowText) : discol);
                painter->drawText(vTextTipRect, text_flags, split.at(1));
            }
        }
        else
        {
            QRect textRect(xpos, y + windowsItemVMargin,
                w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
            QRect vTextRect = p.visualRect(opt->direction, menuitem->rect, textRect);

            QString s(menuitem->text);
            s = s.remove(QStringLiteral(_qtn_SplitActionPopup));
            // draw text
            if (!s.isEmpty())
            {
                int t = s.indexOf(QLatin1Char('\t'));
                int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
                if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, menuitem, widget))
                    text_flags |= Qt::TextHideMnemonic;
                text_flags |= Qt::AlignLeft;

                if (t >= 0)
                {
                    QRect vShortcutRect = p.visualRect(opt->direction, menuitem->rect, QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
                    const QString textToDraw = s.mid(t + 1);
                    if (dis && !act && p.proxy()->styleHint(QStyle::SH_EtchDisabledText, opt, widget))
                    {
                        painter->setPen(menuitem->palette.light().color());
                        painter->drawText(vShortcutRect.adjusted(1, 1, 1, 1), text_flags, textToDraw);
                        painter->setPen(discol);
                    }
                    painter->drawText(vShortcutRect, text_flags, textToDraw);
                    s = s.left(t);
                }

                QFont saveFont = menuitem->font;
                QFont font = saveFont;
                if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                    font.setBold(true);
                painter->setFont(font);

                const QString textToDraw = s.left(t);
                if (dis && !act && p.proxy()->styleHint(QStyle::SH_EtchDisabledText, opt, widget))
                {
                    painter->setPen(menuitem->palette.light().color());
                    painter->drawText(vTextRect.adjusted(1, 1, 1, 1), text_flags, textToDraw);
                    painter->setPen(discol);
                }
                painter->drawText(vTextRect, text_flags, textToDraw);
                painter->setFont(saveFont);
            }
        }
        // draw sub menu arrow
        if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            int dim = (h - 2 * windowsItemFrame) / 2;
            QStyle::PrimitiveElement arrow = (opt->direction == Qt::RightToLeft) ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
            xpos = x + w - windowsArrowHMargin - windowsItemFrame - dim;
            xpos += splitActionPopupWidth / 2 - 3;
            QRect  vSubMenuRect = p.visualRect(opt->direction, menuitem->rect, QRect(xpos, y + h / 2 - dim / 2, dim, dim));
            QStyleOptionMenuItem newMI = *menuitem;
            newMI.rect = vSubMenuRect;
            newMI.state = dis ? QStyle::State_None : QStyle::State_Enabled;
            if (act)
                QPalette_setColor(newMI.palette, QPalette::ButtonText, newMI.palette.highlightedText().color());
            p.proxy()->drawPrimitive(arrow, &newMI, painter, widget);
        }
    }
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonSysFramePagePopupList(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    painter->fillRect(opt->rect, opt->palette.shadow());

    QFont saveFont = painter->font();
    QFont font = saveFont;
    font.setBold(true);
    painter->setFont(font);

    QFontMetrics fm(font);
    QRect rcCaption = p.proxy()->subElementRect(static_cast<QStyle::SubElement>(CommonStyle::SE_RibbonSysHeaderLabelPopupList), opt, widget);

    const int hMarginCaption = p.proxy()->pixelMetric(static_cast<QStyle::PixelMetric>(CommonStyle::PM_RibbonHMarginCaptionSysMenu), opt, widget);
    QRect rcText = rcCaption.adjusted(hMarginCaption, 0, 0, 0);

    uint alignment = Qt::AlignVCenter | Qt::TextSingleLine;
    p.proxy()->drawItemText(painter, rcText, alignment, opt->palette, true, widget->windowTitle(), QPalette::WindowText);
    painter->setFont(saveFont);

    painter->setPen(opt->palette.mid().color());
    painter->drawLine(rcCaption.left(), rcCaption.bottom(), rcCaption.right(), rcCaption.bottom());
    painter->setPen(opt->palette.light().color());
    painter->drawLine(rcCaption.left(), rcCaption.bottom() + 1, rcCaption.right(), rcCaption.bottom() + 1);
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonFrameGallery(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const QtStyleOptionFrame* frameGallery = qstyleoption_cast<const QtStyleOptionFrame*>(opt))
    {
        if (qobject_cast<const QMenu*>(widget ? widget->parentWidget() : Q_NULL))
        {
            painter->fillRect(opt->rect, opt->palette.light());
            return true;
        }
        if (frameGallery->features & QtStyleOptionFrame::Flat)
        {
            bool enabled = opt->state & QStyle::State_Enabled;
            bool selected = opt->state & QStyle::State_MouseOver;
            painter->fillRect(opt->rect, selected && enabled ? opt->palette.highlight() : opt->palette.window());
        }
        if (frameGallery->lineWidth > 0)
        {
            const QPen savePen = painter->pen();
            painter->setPen(opt->palette.color(QPalette::Midlight));
            painter->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            painter->setPen(savePen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonBackstageFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const RibbonBackstageStyleOption* optBackstage = qstyleoption_cast<const RibbonBackstageStyleOption*>(opt))
    {
        painter->fillRect(opt->rect, opt->palette.window());

        QPixmap pixmap = loadPixmap(QStringLiteral("RibbonBackstageView"), QStringLiteral("Image_BackstageTopBorder"));
        if (!pixmap.isNull())
            painter->drawPixmap(QRect(QPoint(0, 0), QSize(opt->rect.width(), 2)), pixmap, pixmap.rect());

        pixmap = loadPixmap(QStringLiteral("RibbonBackstageView"), QStringLiteral("Image_BackstageMenuBackground"));
        if (!pixmap.isNull())
            painter->drawPixmap(QRect(QPoint(0, 2), QPoint(optBackstage->menuWidth, opt->rect.bottom())), pixmap, pixmap.rect());
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonBackstagePanelButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const BackstageButtonStyleOption* btnStyle = qstyleoption_cast<const BackstageButtonStyleOption*>(opt))
    {
        QRect rc = btnStyle->rect;
        const bool isDefault = btnStyle->features & QStyleOptionButton::DefaultButton;
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool checked = opt->state & QStyle::State_On;
        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;

        if (btnStyle->tabStyle && !selected && !pressed && !checked)
            return true;

        QPixmap soImage = loadPixmap(QStringLiteral("RibbonBackstageButton"), QStringLiteral("Image_BackstageButton"));
        int state = isDefault ? 4 : 0;

        if (!enabled) state = 3;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 1;
        else if ((selected && pressed)) state = 2;
        else if (selected || pressed) state = 1;

        if (!soImage.isNull() && state != -1)
            drawImage(soImage, *painter, rc, sourceRectImage(soImage.rect(), state, 5), QRect(QPoint(4, 4), QPoint(4, 4)), QColor(0xFF, 0, 0xFF));

        if (checked && btnStyle->tabStyle)
        {
            QPixmap soImage = loadPixmap(QStringLiteral("RibbonBackstageButton"), QStringLiteral("Image_BackstageButtonGlyph"));
            if (!soImage.isNull())
            {
                QRect rcSrc = soImage.rect();
                int y = rc.center().y();
                QRect rectImage(QPoint(rc.right() - rcSrc.width(), y - rcSrc.height() / 2), QPoint(rc.right(), y - rcSrc.height() / 2 + rcSrc.height()));
                drawImage(soImage, *painter, rectImage, rcSrc);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonGroupScrollButton(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(option))
    {
        QTN_P(const CommonStyle);
        QRect rect = toolbutton->rect;
        QPoint topleft(4, 6), bottomright(3, 6);
        QPixmap soRibbonScrollButton;
        QStyle::PrimitiveElement prim = QStyle::PE_IndicatorArrowLeft;
        if (toolbutton->arrowType == Qt::LeftArrow)
            soRibbonScrollButton = loadPixmap(QStringLiteral("RibbonPage"), QStringLiteral("Image_RibbonGroupScrollButtonLeft"));
        else if (toolbutton->arrowType == Qt::RightArrow)
        {
            prim = QStyle::PE_IndicatorArrowRight;
            soRibbonScrollButton = loadPixmap(QStringLiteral("RibbonPage"), QStringLiteral("Image_RibbonGroupScrollButtonRight"));
        }
        else if (toolbutton->arrowType == Qt::UpArrow)
        {
            topleft = QPoint(2, 1);
            bottomright = QPoint(2, 1);
            rect.adjust(0, 0, -2, 0);
            prim = QStyle::PE_IndicatorArrowUp;
            soRibbonScrollButton = loadPixmap(QStringLiteral("RibbonPage"), QStringLiteral("Image_RibbonGroupScrollButtonUp"));
        }
        else if (toolbutton->arrowType == Qt::DownArrow)
        {
            topleft = QPoint(2, 1);
            bottomright = QPoint(2, 1);
            rect.adjust(0, 0, -2, 0);
            prim = QStyle::PE_IndicatorArrowDown;
            soRibbonScrollButton = loadPixmap(QStringLiteral("RibbonPage"), QStringLiteral("Image_RibbonGroupScrollButtonDown"));
        }

        if (soRibbonScrollButton.isNull())
            return CommonStylePrivate::drawRibbonGroupScrollButton(option, painter, w);

        const bool selected = option->state & QStyle::State_MouseOver;
        const bool pressed = option->state & QStyle::State_Sunken;
        const int state = pressed ? 2 : selected ? 1 : 0;
        QRect rcSrc = sourceRectImage(soRibbonScrollButton.rect(), state, 3);
        drawImage(soRibbonScrollButton, *painter, rect, rcSrc, QRect(topleft, bottomright), QColor(0xFF, 0, 0xFF));
        p.proxy()->drawPrimitive(prim, toolbutton, painter, w);
        return true;
    }
    return CommonStylePrivate::drawRibbonGroupScrollButton(option, painter, w);
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonPopupBarButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
    {
        drawWidgetPixmap(QStringLiteral("RibbonSystemMenu"), QStringLiteral("Image_RibbonSystemMenuButton"), toolbutton->rect,
            painter, toolbutton->state & QStyle::State_MouseOver ? 1 : 0, 2, QMargins(2, 2, 2, 2), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawPanelItemViewItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    p.QProxyStyle::drawPrimitive(QStyle::PE_PanelItemViewItem, opt, painter, widget);
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawToolBar(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const QStyleOptionToolBar* toolbar = qstyleoption_cast<const QStyleOptionToolBar*>(opt))
    {
        if (toolbar->state & QStyle::State_Horizontal)
            drawWidgetPixmap(QStringLiteral("QToolBar"), QStringLiteral("Image_ToolbarFaceHorizontal"), opt->rect, painter, QMargins(5, 5, 5, 5));
        else
            drawWidgetPixmap(QStringLiteral("QToolBar"), QStringLiteral("Image_ToolbarFaceVertical"), opt->rect, painter, QMargins(5, 5, 5, 5));
        return true;
    }
    else if (const RibbonQuickAccessBarStyleOption* quickAccessBar = qstyleoption_cast<const RibbonQuickAccessBarStyleOption*>(opt))
    {
        if (quickAccessBar->roundButton && widget && widget->parent() && widget->parent()->inherits(QTITAN_META_CLASSNAME("RibbonTitleBarWidget")))
        {
            QRect rcQuickAccess = quickAccessBar->rect;
            QRect rcQuickAccessArea = rcQuickAccess.adjusted(-16, -2, 6, 2);
            drawWidgetPixmap(QStringLiteral("RibbonQuickAccessBar"), QStringLiteral("Image_RibbonQuickAccessAreaDwm"), rcQuickAccessArea, painter, QMargins(16, 3, 16, 3));
        }
        else if (quickAccessBar->quickAccessBarPosition != 1)
            drawWidgetPixmap(QStringLiteral("RibbonQuickAccessBar"), QStringLiteral("Image_PanelQuickAccessBar"), opt->rect, painter, QMargins(5, 5, 5, 5));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonTabShapeLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (const RibbonOptionHeaderStyleOption* optTab = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        QTN_P(const CommonStyle);
        int flags = optTab->textAlignment;
        flags |= Qt::TextShowMnemonic;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;

        QString text = optTab->text;
        if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), opt, w))
            text = text.toUpper();
        const bool selected = optTab->state & QStyle::State_Selected;
        // draw text
        p.proxy()->drawItemText(painter, optTab->rect, flags, optTab->palette,
            opt->state & QStyle::State_Enabled, text, selected ? QPalette::WindowText : QPalette::ButtonText);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    const RibbonOptionHeaderStyleOption* ribbonOption = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt);
    Q_ASSERT(ribbonOption != Q_NULL);
    const Qtitan::ContextColor pageColor = ribbonOption->pageColor;
    const bool popuped = (opt->state & QStyle::State_Selected) && (opt->state & QStyle::State_Sunken);

    bool selected = opt->state & QStyle::State_Selected;
    bool highlighted = opt->state & QStyle::State_MouseOver;
    bool focused = false;
    if (QtnPrivate::qtn_isAncestor(w, QTITAN_META_CLASSNAME("RibbonBar")))
    {
        if (selected && ribbonOption->minimized)
        {
            if (popuped)
            {
                focused = false;
                highlighted = false;
            }
            else
            {
                focused = false;
                selected = false;
            }
        }
    }

    if (pageColor != Qtitan::ContextColorNone)
    {
        int state = 0;
        if (selected || highlighted)
        {
            QString strImage(QStringLiteral("Image_ContextTab"));
            strImage += CommonStyle::contextColorToString(pageColor);

            QString fileName;
            fileName += widgetPixmapName(QStringLiteral("RibbonTab"), strImage);
            QPixmap pix(fileName);
            if (!pix.isNull() && pix.height() < 70)
            {
                if (selected)
                    state = focused ? 2 : 1;
                else if (highlighted)
                    state = 0;
                drawWidgetPixmap(QStringLiteral("RibbonTab"), strImage, opt->rect, painter, state, 3, QMargins(5, 5, 5, 5));
            }
            else
            {
                if (selected)
                    state = focused || highlighted ? 4 : 3;
                else if (highlighted)
                    state = 1;

                if (state == 4)
                {
                    strImage = QStringLiteral("Image_RibbonTab");
                    QPixmap pixmap = getCachedPixmap(QStringLiteral("RibbonTab"), strImage, opt->rect.size(), state, 6, QMargins(5, 5, 5, 5));
                    if (ribbonOption->tabBarPosition == 2)
                        pixmap = pixmap.transformed(QTransform().rotate(180));
                    painter->drawPixmap(opt->rect, pixmap);
                }
                else
                {
                    QPixmap pixmap = getCachedPixmap(QStringLiteral("RibbonTab"), strImage, opt->rect.size(), state, 5, QMargins(5, 5, 5, 5));
                    if (ribbonOption->tabBarPosition == 2)
                        pixmap = pixmap.transformed(QTransform().rotate(180));
                    painter->drawPixmap(opt->rect, pixmap);
                }
            }
        }
    }
    else
    {
        if (selected || highlighted)
        {
            int state = 0;
            if (selected && focused)
                state = 5;
            else if (selected && highlighted)
                state = 4;
            else if (selected)
                state = 3;
            else if (opt->state & QStyle::State_Sunken)
                state = 2;
            else if (highlighted)
                state = 1;
            QString image = QStringLiteral("Image_RibbonTab");
  
            QPixmap pixmap = getCachedPixmap(QStringLiteral("RibbonTab"), image, opt->rect.size(), state, 6, QMargins(5, 5, 5, 5));
            if (ribbonOption->tabBarPosition == 2)
                pixmap = pixmap.transformed(QTransform().rotate(180));
            painter->drawPixmap(opt->rect, pixmap);
        }
    }

    if (w && w->inherits(QTITAN_META_CLASSNAME("RibbonTab")))
    {
        if (pageColor != Qtitan::ContextColorNone && ribbonOption->firstTab)
        {
            QRect rcSeparator(QPoint(opt->rect.left(), opt->rect.top()), QPoint(opt->rect.left(), opt->rect.bottom() - 2));
            drawWidgetPixmap(QStringLiteral("RibbonTab"), QStringLiteral("Image_ContextTabSeparator"), rcSeparator, painter, 0, 1);
        }

        if (pageColor != Qtitan::ContextColorNone && ribbonOption->lastTab)
        {
            QRect rcSeparator(QPoint(opt->rect.right(), opt->rect.top()), QPoint(opt->rect.right(), opt->rect.bottom() - 2));
            drawWidgetPixmap(QStringLiteral("RibbonTab"), QStringLiteral("Image_ContextTabSeparator"), rcSeparator, painter, 0, 1);
        }
    }
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(w);
    QRect rcGripper(opt->rect);
    QPixmap soGrip = getCachedPixmap(QStringLiteral("RibbonStatusBar"), QStringLiteral("Image_StatusBarGripper"), QSize(0, 0), QMargins(0, 0, 0, 0), QColor(0xFF, 0, 0xFF));
    if (!soGrip.isNull())
    {
        rcGripper.setWidth(soGrip.width()); rcGripper.setHeight(soGrip.height());
        painter->drawPixmap(rcGripper, soGrip);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawSlider(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (widget && widget->parentWidget() && !widget->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonSliderPane")))
    {
        p.baseStyle()->drawComplexControl(QStyle::CC_Slider, opt, painter, widget);
        return true;
    }

    if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        if (slider->orientation == Qt::Horizontal)
        {
            const QRect groove = p.proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderGroove, widget);
            const QRect handle = p.proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderHandle, widget);

//            QPixmap soTrack = cached("SliderTrack.png");
            QPixmap soTrack = getCachedPixmap(QStringLiteral("QSlider"), QStringLiteral("Image_SliderTrack"), QSize(), 0, 1, QMargins(0, 0, 0, 0));
            QRect rcTrackDest(QPoint(groove.left(), (groove.top() + groove.bottom() - soTrack.height()) / 2),
                QSize(groove.width(), soTrack.height()));
            painter->drawPixmap(rcTrackDest, soTrack, soTrack.rect());

            if (slider->maximum >= slider->minimum)
            {
//                QPixmap soTick = cached("SliderTick.png");
                QPixmap soTick = getCachedPixmap(QStringLiteral("QSlider"), QStringLiteral("Image_SliderTick"), QSize(), 0, 1, QMargins(0, 0, 0, 0), QColor(0xFF, 0, 0xFF));
                QRect rcTrackTickDest(QPoint((groove.left() + groove.right() - soTick.width()) / 2,
                    (groove.top() + groove.bottom() - soTick.height()) / 2), soTick.size());

                painter->drawPixmap(rcTrackTickDest, soTick, soTick.rect());

//                drawImage(soTick, *p, rcTrackTickDest, soTick.rect(), QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            }
            if ((opt->subControls & QStyle::SC_SliderHandle))
            {
//                drawWidgetPixmap("QSlider", "Image_SliderThumb", rc, painter, 0, 1, QMargins(2, 2, 2, 2));

//                QPixmap hndl = cached("SliderThumb.png");

                bool selected = slider->activeSubControls & QStyle::SC_SliderHandle && opt->state & QStyle::State_MouseOver;
                bool nPressetHt = slider->state & QStyle::State_Sunken;

                QPixmap hndl = getCachedPixmap(QStringLiteral("QSlider"), QStringLiteral("Image_SliderThumb"), QSize(), nPressetHt ? 2 : selected ? 1 : 0, 3);

//                QRect rcSrc = sourceRectImage(hndl.rect(), nPressetHt ? 2 : selected ? 1 : 0, 3);
                QRect rcSrc = hndl.rect();
                QPoint point((handle.left() + handle.right() - rcSrc.width()) / 2, (handle.top() + handle.bottom() - rcSrc.height()) / 2);
                painter->drawPixmap(point, hndl, rcSrc);
            }
            return true;
        }
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonGroupLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
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
            painter->setPen(optGroupBox->palette.color((m_themeType == Office2007Style::Black || m_themeType == Office2007Style::Aqua) ? 
                QPalette::BrightText : QPalette::WindowText));
            painter->drawText(rcEntryCaption, flags, text);
            painter->setPen(savePen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonReducedGroup(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const RibbonGroupStyleOption* optGroup = qstyleoption_cast<const RibbonGroupStyleOption*>(opt))
    {
        Qtitan::ContextColor pageColor = optGroup->pageColor;
        QPixmap soImage;
        if (pageColor != Qtitan::ContextColorNone)
        {
            QString resName(QStringLiteral("Image_ContextPage"));
            resName += CommonStyle::contextColorToString(optGroup->pageColor);
            resName += QStringLiteral("GroupButton");
            soImage = loadPixmap(QStringLiteral("RibbonGroup"), resName);
        }
        else
            soImage = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupButton"));

        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return false;

        QRect rcEntry(opt->rect);
        const bool popuped = optGroup->features & QStyleOptionToolButton::HasMenu;
        int state = popuped ? 2 : (opt->state & QStyle::State_MouseOver ? 1 : 0);
        QRect rcSrc = sourceRectImage(soImage.rect(), state, 3);
        // draw background
        drawImage(soImage, *painter, rcEntry, rcSrc, QRect(QPoint(5, 5), QPoint(5, 5)), (state == 1 && true));

        // ----
        QFontMetrics fm(optGroup->fontMetrics);
        int flags = Qt::TextSingleLine;

        flags |= Qt::TextShowMnemonic;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, optGroup, widget))
            flags |= Qt::TextHideMnemonic;

        if (!optGroup->icon.isNull())
        {
            QPixmap pm;
            QIcon::State stateIcon = optGroup->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(optGroup->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                mode = QIcon::Active;
            else
                mode = QIcon::Normal;
            pm = preparePixmap(optGroup->icon, optGroup->rect.size().boundedTo(optGroup->iconSize), mode, stateIcon, widget);

            soImage = loadPixmap(QStringLiteral("RibbonGroup"), QStringLiteral("Image_RibbonGroupButtonIcon"));

            QSize sz = soImage.size();
            sz = QSize(CommonStylePrivate::dpiScaled(sz.height(), widget),
                       CommonStylePrivate::dpiScaled(sz.width(), widget));
            QRect rc(QPoint(rcEntry.center().x() - sz.width() / 2, rcEntry.top() + 2), sz);

            if (!soImage.isNull())
            {
                painter->save();
                painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
                drawImage(soImage, *painter, rc, sourceRectImage(soImage.rect(), 0, 1), QRect(QPoint(0, 0), QPoint(0, 0)), /*isStyle2010()*/true);
                painter->restore();
            }

            QRect rectImage = rc;
            rectImage.setLeft(rectImage.center().x() - pm.width() / 2);
            rectImage.setRight(rectImage.left() + pm.width());
            int indent = CommonStylePrivate::dpiScaled(6, widget);
            rectImage.adjust(-indent, -indent, indent, indent);
            p.drawItemPixmap(painter, rectImage, Qt::AlignCenter, pm);
        }

        //===========================================================================================
        QSize sz = optGroup->fontMetrics.boundingRect(optGroup->rect, flags | Qt::AlignVCenter | Qt::TextWordWrap, optGroup->text).size();
        bool twoLines = sz.height() / optGroup->fontMetrics.height() == 2;

        QRect rcText = rcEntry;
        const int textHeightTotal = optGroup->rect.height() * 0.5;
        rcText.setHeight(sz.height());
        rcText.moveTo(0, textHeightTotal);

        QString strText = optGroup->text;
        const int menuArrowWidth = twoLines ? p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, optGroup, widget) : 0;
        if (menuArrowWidth > 0)
            strText += QStringLiteral("   ");

        p.proxy()->drawItemText(painter, rcText, flags | Qt::TextWordWrap | Qt::AlignHCenter, optGroup->palette,
            optGroup->state & QStyle::State_Enabled, strText, QPalette::WindowText);
        //===========================================================================================
        soImage = loadPixmap(QStringLiteral("QComboBox"), QStringLiteral("Image_ToolbarButtonDropDownGlyph"));
        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return false;

        state = !(optGroup->state & QStyle::State_Enabled) ? 3 : opt->state & QStyle::State_MouseOver ? 1 : 0;
        rcSrc = sourceRectImage(soImage.rect(), state, 4);

        QStyleOption tool(0);
        tool.palette = opt->palette;
        tool.state |= QStyle::State_Enabled;

        const int mbi = rcSrc.height();
        QPoint pt;
        if (twoLines)
        {
            QRect r = rcText;
            r.setLeft(rcText.left() + (rcText.width() - sz.width()) / 2);
            r.setWidth(sz.width());
            pt = QPoint(r.right(), (r.bottom() - (optGroup->fontMetrics.height() / 2 + mbi / 2)) + optGroup->fontMetrics.descent() - 1);
        }
        else
            pt = QPoint(rcText.center().x() - mbi / 2, (rcText.top() + optGroup->fontMetrics.height() + optGroup->fontMetrics.descent() + 1));

        QRect rc(pt, rcSrc.size());
        if (!CommonStylePrivate::isUse96Dpi())
        {
            rc.setWidth(CommonStylePrivate::dpiScaled(rc.width(), widget));
            rc.setHeight(CommonStylePrivate::dpiScaled(rc.height(), widget));
            painter->save();
            painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
            drawImage(soImage, *painter, rc, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            painter->restore();
        }
        else
            drawImage(soImage, *painter, rc, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    }
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonGalleryItem(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_P(const Office2007Style);
    if (const RibbonGalleryItemStyleOption* optItem = qstyleoption_cast<const RibbonGalleryItemStyleOption*>(opt))
    {
        if (optItem->separator)
        {
            QRect rectItem = optItem->rectItem;
            painter->fillRect(rectItem, optItem->palette.alternateBase());
            painter->fillRect(rectItem.left(), rectItem.bottom() - 1, rectItem.width(), 1, optItem->palette.mid());

            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter;

            rectItem.adjust(10, 0, 0, 0);
            const QFont& saveFont = painter->font();
            QFont font(saveFont);
            font.setBold(true);
            painter->setFont(font);
            p.proxy()->drawItemText(painter, rectItem, alignment, optItem->palette, true, optItem->caption, QPalette::WindowText);
            painter->setFont(saveFont);
            return true;
        }
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;
        const bool checked = opt->state & QStyle::State_On;

        if (!optItem->icon.isNull())
        {
            QRect rect = optItem->rectItem;
            drawRectangle(painter, rect, selected, pressed, enabled, checked, false, TypeNormal, BarPopup);

            QList<QSize> lstSz = optItem->icon.availableSizes(enabled ? QIcon::Normal : QIcon::Disabled);
            QSize iconSize = lstSz.size() > 0 ? lstSz[0] : optItem->rectItem.size();
            iconSize = QSize(CommonStylePrivate::dpiScaled(iconSize.width(), w), CommonStylePrivate::dpiScaled(iconSize.height(), w));
            QPixmap pm = preparePixmap(optItem->icon, iconSize, enabled ? QIcon::Normal : QIcon::Disabled, QIcon::Off, w);
            if (!pm.isNull())
                p.proxy()->drawItemPixmap(painter, rect, Qt::AlignCenter, pm);
        }
        else
        {
            drawRectangle(painter, optItem->rectItem, selected, false, enabled, checked, false, TypePopup, BarPopup);
            QRect rcText(optItem->rectItem);
            rcText.setLeft(rcText.left() + 3);
            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter | Qt::TextHideMnemonic;
            p.proxy()->drawItemText(painter, rcText, alignment, optItem->palette, enabled, optItem->caption, QPalette::WindowText);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonBackstageMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        const bool act = menuitem->state & QStyle::State_Selected;
        const bool focus = menuitem->state & QStyle::State_HasFocus;
        const bool disable = !(menuitem->state & QStyle::State_Enabled);

        QPalette palette = menuitem->palette;
        QColor clrText = focus ? Qt::white : menuitem->palette.windowText().color();
        if (disable)
            clrText = menuitem->palette.color(QPalette::Disabled, QPalette::WindowText).lighter();

        QPalette_setColor(palette, QPalette::WindowText, clrText);

        QRect rcText(opt->rect);
        rcText.adjust(15, 0, 0, 0);

        if (menuitem->menuItemType == QStyleOptionMenuItem::Normal)
        {
            if (!disable && (act || focus))
            {
                QPixmap soImage = loadPixmap(QStringLiteral("RibbonBackstageView"), QStringLiteral("Image_BackstageMenuCommand"));
                if (!soImage.isNull())
                    drawImage(soImage, *painter, opt->rect, sourceRectImage(soImage.rect(), 0, 3), QRect(QPoint(2, 2), QPoint(2, 2)));
            }
        }
        else if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            if (!disable && (act || focus))
            {
                QPixmap soImage = loadPixmap(QStringLiteral("RibbonBackstageView"), QStringLiteral("Image_BackstageMenuPage"));
                if (!soImage.isNull())
                    drawImage(soImage, *painter, opt->rect, sourceRectImage(soImage.rect(), focus ? 1 : 0, 4), QRect(QPoint(2, 2), QPoint(2, 2)));

                if (focus)
                {
                    soImage = loadPixmap(QStringLiteral("RibbonBackstageView"), QStringLiteral("Image_BackstageMenuPageGlyph"));
                    if (!soImage.isNull())
                    {
                        QRect rcSrc = soImage.rect();
                        QRect rcGlyph(QPoint(opt->rect.right() - rcSrc.width() + 1, (opt->rect.top() + opt->rect.bottom() - rcSrc.height()) / 2), rcSrc.size());
                        painter->drawPixmap(rcGlyph, soImage, rcSrc);
                    }
                }
            }
        }
        else if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            drawMenuItemSeparator(opt, painter, widget);
            return true;
        }

        int iconWidth = 0;
        int indent = menuitem->menuItemType == QStyleOptionMenuItem::SubMenu ? 6 : 0;
        if (!menuitem->icon.isNull())
        {
            int iconSize = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);

            QIcon::Mode mode = disable ? QIcon::Disabled : QIcon::Normal;
            QPixmap pixmap = menuitem->icon.pixmap(QSize(iconSize, iconSize), mode);
            QPoint pmr(rcText.left() + indent, (rcText.top() + rcText.bottom() - iconSize) / 2);
            painter->save();
            painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
            painter->drawPixmap(QRect(pmr, QSize(iconSize, iconSize)), pixmap);
            painter->restore();
            iconWidth = iconSize;
        }

        indent += !menuitem->icon.isNull() ? 7 : 0;
        rcText.setLeft(rcText.left() + iconWidth + indent);
        rcText.setHeight(qMax(rcText.height(), iconWidth));

        if (!menuitem->text.isEmpty())
        {
            int flags = 0;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;

            int index = menuitem->text.indexOf(QStringLiteral("&"));
            if (index != -1)
            {
                index = menuitem->text.indexOf(QStringLiteral(" "), index);
                if (index != -1)
                    flags &= ~Qt::TextHideMnemonic;
            }
            // draw text
            p.proxy()->drawItemText(painter, rcText, flags, palette, 
                opt->state & QStyle::State_Enabled, menuitem->text, QPalette::WindowText);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawOfficePopupSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionSizeGrip* sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip*>(opt))
    {
        QPixmap soImage = loadPixmap(QStringLiteral("OfficePopupMenu"), QStringLiteral("Image_PopupBarResizeGripperFace"));
        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return false;
        drawImage(soImage, *painter, sizeGrip->rect, soImage.rect(), QRect(QPoint(1, 1), QPoint(1, 1)), QColor(0xFF, 0, 0xFF));

        soImage = loadPixmap(QStringLiteral("OfficePopupMenu"), QStringLiteral("Image_PopupBarResizeGripperWidth"));
        QRect rc = soImage.rect();
        QRect rcDest(QPoint(sizeGrip->rect.right() - rc.width(), sizeGrip->rect.bottom() - rc.height()), rc.size());
        drawImage(soImage, *painter, rcDest, rc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

/*! \internal */
static QRect offsetSourceRect(QRect rc, int state)
{
    rc.translate(0, (state - 1) * rc.height());
    return rc;
}

/*! \internal */
bool Office2007StylePrivate::drawScrollBarAddSubLine(QStyle::ControlElement element, const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
    if (p.isScrollBarsIgnored())
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        const bool light = w && ((w->parentWidget() != Q_NULL && w->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonGallery"))) ||
                qobject_cast<const QDialog*>(w->topLevelWidget()) || QtnPrivate::qtn_isAncestor(w, QTITAN_META_CLASSNAME("RibbonBar")));

        QString strImage;
        if (opt->state & QStyle::State_Horizontal)
            strImage = light ? QStringLiteral("Image_ControlGalleryScrollArrowsHorizontalLight") : QStringLiteral("Image_ControlGalleryScrollArrowsHorizontalDark");
        else
            strImage = light ? QStringLiteral("Image_ControlGalleryScrollArrowsVerticalLight") : QStringLiteral("Image_ControlGalleryScrollArrowsVerticalDark");

        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;

        int state = -1;
        if (!enabled)
            state = 0;
        else if (selected && pressed)
        {
            if (((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)))
                state = 3;
            else
                state = 1;
        }
        else if (selected)
        {
            if (((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)))
                state = 2;
            else
                state = 1;
        }

        if (state != -1)
            drawWidgetPixmap(QStringLiteral("QScrollBar"), strImage, opt->rect, painter, state, 4, QMargins(3, 3, 3, 3), QColor(0xFF, 0, 0xFF));

        QPixmap soImageArrowGlyphs;
        if (!light)
            soImageArrowGlyphs = loadPixmap(QStringLiteral("RibbonGallery"), QStringLiteral("Image_ControlGalleryScrollArrowGlyphsDark"));

        if (soImageArrowGlyphs.isNull())
            soImageArrowGlyphs = loadPixmap(QStringLiteral("RibbonGallery"), QStringLiteral("Image_ControlGalleryScrollArrowGlyphs"));

        int number = -1;
        if (opt->state & QStyle::State_Horizontal)
        {
//            QRect rcSrc;
            if (element == QStyle::CE_ScrollBarAddLine)
                number = !enabled ? 16 : state != 0 ? 14 : 13;
            else
                number = !enabled ? 12 : state != 0 ? 10 : 9;
        }
        else
        {
            if (element == QStyle::CE_ScrollBarAddLine)
                number = !enabled ? 8 : state != 0 ? 6 : 5;
            else
                number = !enabled ? 4 : state != 0 ? 2 : 1;
        }
        QRect rcArrowGripperSrc(0, 0, 9, 9);
        QRect rcSrc = ::offsetSourceRect(rcArrowGripperSrc, number);
        QRect rcArrowGripper(QPoint((opt->rect.left() + opt->rect.right() - 8) / 2, (opt->rect.top() + opt->rect.bottom() - 8) / 2), QSize(9, 9));

        QSize sz = p.proxy()->sizeFromContents(QStyle::CT_ScrollBar, opt, scrollbar->rect.size(), w);
        if (opt->state & QStyle::State_Horizontal)
        {
            if (sz.height() % 2 == 0) //Horizontal
                rcArrowGripper.setHeight(rcArrowGripper.height() + 1);
        }
        else
        {
            if (sz.width() % 2 == 0) //for vertical
                rcArrowGripper.setWidth(rcArrowGripper.width() + 1);
        }
        drawImage(soImageArrowGlyphs, *painter, rcArrowGripper, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(255, 0, 255));
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawScrollBarAddSubPage(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(ce);
    QTN_P(const Office2007Style);
    if (p.isScrollBarsIgnored())
        return false;

    if (!(opt->state & QStyle::State_Sunken))
        return true;

    const bool light = w && ((w->parentWidget() != Q_NULL && w->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonGallery"))) ||
            qobject_cast<const QDialog*>(w->topLevelWidget()) || QtnPrivate::qtn_isAncestor(w, QTITAN_META_CLASSNAME("RibbonBar")));

    if (opt->state & QStyle::State_Horizontal)
    {
        QString strImage = light ? QStringLiteral("Image_ControlGalleryScrollHorizontalLight") : QStringLiteral("Image_ControlGalleryScrollHorizontalDark");
        drawWidgetPixmap(QStringLiteral("QScrollBar"), strImage, opt->rect, painter, 1, 2, QMargins(0, 1, 0, 1));
    }
    else
    {
        QString strImage = light ? QStringLiteral("Image_ControlGalleryScrollVerticalLight") : QStringLiteral("Inage_ControlGalleryScrollVerticalDark");
        drawWidgetPixmap(QStringLiteral("QScrollBar"), strImage, opt->rect, painter, 1, 2, QMargins(1, 0, 1, 0));
    }
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawScrollBarSlider(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
    if (p.isScrollBarsIgnored())
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        if (!opt->rect.isEmpty())
        {
            const bool light = w && ((w->parentWidget() != Q_NULL && w->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonGallery"))) ||
                    qobject_cast<const QDialog*>(w->topLevelWidget()) || QtnPrivate::qtn_isAncestor(w, QTITAN_META_CLASSNAME("RibbonBar")));

            const bool enabled = opt->state & QStyle::State_Enabled;
            const bool selected = opt->state & QStyle::State_MouseOver;
            const bool pressed = opt->state & QStyle::State_Sunken;

            int state = -1;
            if (!enabled)
                state = 0;
            else if (pressed && scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider)
                state = 2;
            else if (selected && scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider)
                state = 1;
            else if ((selected || pressed) && scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider)
                state = 2;
            else
                state = 0;


            QRect rc(opt->rect);
            QPixmap scrollThumb;
            if (opt->state & QStyle::State_Horizontal)
            {
                if (!light)
                    scrollThumb = loadPixmap(QStringLiteral("QScrollBar"), QStringLiteral("Image_ControlGalleryScrollThumbHorizontalDark"));
                if (scrollThumb.isNull())
                    scrollThumb = loadPixmap(QStringLiteral("QScrollBar"), QStringLiteral("Image_ControlGalleryScrollThumbHorizontal"));
            }
            else
            {
                if (!light)
                    scrollThumb = loadPixmap(QStringLiteral("QScrollBar"), QStringLiteral("Image_ControlGalleryScrollThumbVerticalDark"));
                if (scrollThumb.isNull())
                    scrollThumb = loadPixmap(QStringLiteral("QScrollBar"), QStringLiteral("Image_ControlGalleryScrollThumbVertical"));
            }

            Q_ASSERT(!scrollThumb.isNull());

            const bool show = opt->state & QStyle::State_Horizontal ? rc.width() > 7 : rc.height() > 7;
            if (!rc.isEmpty() && show)
                drawImage(scrollThumb, *painter, rc, sourceRectImage(scrollThumb.rect(), state, 3), QRect(QPoint(5, 5), QPoint(5, 5)));

            QRect rcGripper(QPoint(opt->rect.center().x() - 3, opt->rect.center().y() - 3), QSize(8, 8));
            if (opt->state & QStyle::State_Horizontal)
            {
                if (opt->rect.width() > 10)
                {
                    drawWidgetPixmap(QStringLiteral("QScrollBar"), QStringLiteral("Image_ControlGalleryScrollThumbGripperHorizontal"), rcGripper, painter, state, 3);

//                    QPixmap soImageScrollThumbGripperHorizontal = cached("ControlGalleryScrollThumbGripperHorizontal.png");
//                    Q_ASSERT(!soImageScrollThumbGripperHorizontal.isNull());
//                    drawImage(soImageScrollThumbGripperHorizontal, *p, rcGripper, sourceRectImage(soImageScrollThumbGripperHorizontal.rect(), state, 3));
                }
            }
            else
            {
                if (opt->rect.height() > 10)
                {
                    drawWidgetPixmap(QStringLiteral("QScrollBar"), QStringLiteral("Image_ControlGalleryScrollThumbGripperVertical"), rcGripper, painter, state, 3);
//                    QPixmap soImageScrollThumbGripperVertical = cached("ControlGalleryScrollThumbGripperVertical.png");
//                    Q_ASSERT(!soImageScrollThumbGripperVertical.isNull());
//                    drawImage(soImageScrollThumbGripperVertical, *p, rcGripper, sourceRectImage(soImageScrollThumbGripperVertical.rect(), state, 3));
                }
            }

        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawTabBarTabShape(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(opt))
    {
        QPixmap soImage = tab->position == QStyleOptionTab::Beginning || tab->position == QStyleOptionTab::OnlyOneTab ?
            loadPixmap(QStringLiteral("QTabBar"), QStringLiteral("Image_TabItemTopLeft")) : loadPixmap(QStringLiteral("QTabBar"), QStringLiteral("Image_TabItemTop"));

        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        const bool enabled = tab->state & QStyle::State_Enabled;
        const bool selected = tab->state & QStyle::State_Selected;
        const bool highlight = tab->state & QStyle::State_MouseOver;
        const bool focused = tab->state & QStyle::State_HasFocus;
        const bool pressed = tab->state & QStyle::State_Sunken;

        QRect rect = opt->rect;
        int state = 0;
        if (selected && focused)
            state = 4;
        if (selected && (highlight && enabled))
            state = 4;
        else if (selected)
            state = 2;
        else if (pressed)
            state = 4;
        else if (highlight && enabled)
            state = 1;

        if (state == 0)
            return true;

        const bool correct = w != Q_NULL ? qobject_cast<QMdiArea*>(w->parentWidget()) != Q_NULL : true;
        const int borderThickness = p.proxy()->pixelMetric(QStyle::PM_DefaultFrameWidth, opt, w) / 2;

        QSize sz;
        QTransform matrix;
        switch (tab->shape)
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                if (!selected)
                    rect.adjust(0, 0, 0, correct ? 0 : -borderThickness);

                sz = QSize(rect.width(), rect.height());
                matrix.rotate(0.);
                break;
            }
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {
                if (!selected)
                    rect.adjust(0, correct ? -borderThickness : borderThickness, 0, 0);

                sz = QSize(rect.width(), rect.height());
                matrix.rotate(-180., Qt::XAxis);
                break;
            }
            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {
                if (!selected)
                    rect.adjust(0, 0, correct ? 0 : -borderThickness, 0);

                sz = QSize(rect.height(), rect.width());
                matrix.rotate(-90.);
                matrix.rotate(180., Qt::YAxis);
                break;
            }
            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {
                if (!selected)
                    rect.adjust(correct ? 0 : borderThickness, 0, 0, 0);

                sz = QSize(rect.height(), rect.width());
                matrix.rotate(90.);
                break;
            }
            default:
                break;
        }

        QPixmap soImageRotate(sz);
        soImageRotate.fill(Qt::transparent);

        QPainter painterImage(&soImageRotate);
        drawPixmap(soImage, painterImage, QRect(QPoint(0, 0), sz), sourceRectImage(soImage.rect(), state, 5), false, QRect(QPoint(6, 6), QPoint(6, 6)));

        QPixmap resultImage = soImageRotate.transformed(matrix);
        painter->drawPixmap(rect, resultImage);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawHeaderSection(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    bool columnPressed = opt->state & QStyle::State_Sunken;
    StyleHelper::drawGradientFill(*painter, opt->rect, columnPressed ? opt->palette.color(QPalette::Light) : opt->palette.color(QPalette::Light),
        columnPressed ? opt->palette.color(QPalette::Midlight) : opt->palette.color(QPalette::Midlight), true);

    painter->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.bottom()), QSize(opt->rect.width(), 1)), opt->palette.color(QPalette::Dark));
    painter->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), opt->palette.color(QPalette::Dark));
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonFileButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const SystemToolButtonStyleOption* optButton = qstyleoption_cast<const SystemToolButtonStyleOption*>(opt))
    {
        if (optButton->toolButtonStyle != Qt::ToolButtonIconOnly)
        {
            QPixmap soRibbonFileButton = loadPixmap(QStringLiteral("RibbonSystemButton"), QStringLiteral("Image_FileButton"));
            const bool popuped = (optButton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);
            const bool isHighlighted = opt->state & QStyle::State_MouseOver;
            const bool enabled = opt->state & QStyle::State_Enabled;

            bool usePalette = optButton->palette.isBrushSet(optButton->palette.currentColorGroup(), QPalette::Base);

            if (usePalette)
            {
                QBrush brButton = optButton->palette.brush(QPalette::Active, QPalette::Button);
                QBrush brButtonDisabled = optButton->palette.brush(QPalette::Disabled, QPalette::Button);

                QBrush brHighlightButton = optButton->palette.isBrushSet(optButton->palette.currentColorGroup(), QPalette::Highlight) ? 
                    optButton->palette.brush(QPalette::Active, QPalette::Highlight) : brButton;
                QBrush brPopupButton = optButton->palette.isBrushSet(optButton->palette.currentColorGroup(), QPalette::Dark) ?
                    optButton->palette.brush(QPalette::Active, QPalette::Dark) : brButton;

                QBrush brush = !enabled ? brButtonDisabled : popuped ? (isHighlighted ? brHighlightButton : brPopupButton) :
                    false ? brButton : isHighlighted ? brHighlightButton : brButton;

                painter->fillRect(optButton->rect, brush);
            }
            else if (m_fileButtonImageCount == 7)
            {
                int state = !enabled ? 5 : popuped ? (isHighlighted ? 3 : 2) : false ? 4 : isHighlighted ? 1 : 0;
                QRect rcSrc = sourceRectImage(soRibbonFileButton.rect(), state, 7);
                QRect rcDest = optButton->rect.adjusted(0, 1, 0, 0);
                drawImage(soRibbonFileButton, *painter, rcDest, rcSrc, QRect(QPoint(3, 3), QPoint(3, 3)));
            }
            else
            {
                int state = popuped ? 2 : isHighlighted ? 1 : 0;
                QRect rcSrc = sourceRectImage(soRibbonFileButton.rect(), state, 3);
                drawImage(soRibbonFileButton, *painter, optButton->rect.adjusted(0, 1, 0, 0), rcSrc, QRect(QPoint(3, 3), QPoint(3, 3)));
            }

            if (optButton->toolButtonStyle == Qt::ToolButtonIconOnly && !optButton->icon.isNull())
            {
                bool enabled = optButton->state & QStyle::State_Enabled;
                QRect rc = optButton->rect;
                QPixmap pxIcon = optButton->icon.pixmap(optButton->iconSize,
                    enabled ? QIcon::Normal : QIcon::Disabled, isHighlighted ? QIcon::On : QIcon::Off);
                QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
                painter->drawPixmap(ptIcon, pxIcon);
            }

            int alignment = Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, optButton, widget))
                alignment |= Qt::TextHideMnemonic;
            alignment |= Qt::AlignCenter;
            alignment |= Qt::TextSingleLine;

            QPalette palette = optButton->palette;
            QPalette_setBrush(palette, QPalette::All, QPalette::ButtonText, Qt::white);
            p.proxy()->drawItemText(painter, optButton->rect, alignment, palette,
                optButton->state & QStyle::State_Enabled, optButton->text, QPalette::ButtonText);
            return true;
        }
        else
        {
            QPixmap soImage;
            if (optButton->rect.width() > 54 && optButton->rect.height() > 54)
                soImage = loadPixmap(QStringLiteral("RibbonSystemButton"), QStringLiteral("Image_FrameSystemButton52"));
            if (soImage.isNull())
                soImage = loadPixmap(QStringLiteral("RibbonSystemButton"), QStringLiteral("Image_FrameSystemButton"));

            Q_ASSERT(!soImage.isNull());
            if (soImage.isNull())
                return false;

            const bool enabled = optButton->state & QStyle::State_Enabled;
            const bool selected = optButton->state & QStyle::State_MouseOver;
            const bool popuped = (optButton->activeSubControls & QStyle::SC_ToolButtonMenu) && (optButton->state & QStyle::State_Sunken);

            int state = 0;
            if (popuped)
                state = 4;
            else if (selected)
                state = 2;
            else if (!enabled)
                state = 1;

            QRect rcSrc(sourceRectImage(soImage.rect(), state == 0 ? 0 : state == 2 ? 1 : 2, 3));
            painter->save();
            painter->translate(optButton->rect.topLeft());
            QPoint ptDest;
            painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
            drawImage(soImage, *painter, QRect(ptDest, QSize(optButton->rect.size())), rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)));
            if (!optButton->icon.isNull())
            {
                QRect rc = optButton->rect;
                QPixmap pxIcon = optButton->icon.pixmap(optButton->iconSize,
                    enabled ? QIcon::Normal : QIcon::Disabled, selected ? QIcon::On : QIcon::Off);
                QPoint ptIcon((rc.width() - pxIcon.width()) / 2, (rc.height() - pxIcon.height()) / 2);
                painter->drawPixmap(ptIcon.x(), ptIcon.y(), pxIcon);
            }
            painter->restore();
            return true;
        }
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonTitleBarWidget(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const TitleBarStyleOption* styleOption = qstyleoption_cast<const TitleBarStyleOption*>(option))
    {
        // const bool isActive = optTitleBar->state & QStyle::State_Active;
        const bool isRibbonBar = QtnPrivate::qtn_isAncestor(widget, QTITAN_META_CLASSNAME("RibbonBar"));
        if (isRibbonBar)
            drawWidgetPixmap(QStringLiteral("RibbonTitleBarWidget"), QStringLiteral("Image_FrameTopCenter"), styleOption->rect.adjusted(0, -1, 0, 0), painter, 0, 2, QMargins(0, 5, 0, 3));

        if (styleOption->optButton.rect.isValid())
            p.proxy()->drawComplexControl(static_cast<QStyle::ComplexControl>(CommonStyle::CC_RibbonFileButton), &styleOption->optButton, painter, Q_NULL);

        for (QVector<TitleBarStyleOption::ContextualTabInfo>::const_iterator it = styleOption->contextualTabs.constBegin();
            it != styleOption->contextualTabs.constEnd(); ++it) 
        {
            QRect rc = it->rect;
            QString resName(QStringLiteral("Image_ContextTabHeader"));
            resName += CommonStyle::contextColorToString(it->color);
            drawWidgetPixmap(QStringLiteral("RibbonTitleBarWidget"), resName, rc, painter, 0, 1, QMargins(2, 2, 2, 2));

            QPalette pal = option->palette;
            painter->setPen(isRibbonBar ? pal.color(QPalette::Dark).darker(240) : pal.color(QPalette::WindowText));
            painter->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine, painter->fontMetrics().elidedText(it->strTitle, Qt::ElideRight, rc.width()));
        }

        QPalette pal = styleOption->palette;
        painter->setFont(styleOption->fontTextTitle);
        QString text = QFontMetrics(styleOption->fontTextTitle).elidedText(styleOption->text, Qt::ElideRight, styleOption->rcTitleText.width());
        p.proxy()->drawItemText(painter, styleOption->rcTitleText, styleOption->alignment, pal, true, text, QPalette::WindowText);
    }
    return true;
}

/*! \internal */
bool Office2007StylePrivate::drawRibbonGalleryButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        if (widget)
        {
            QPixmap soImage;
            if (widget->property(_qtn_PopupButtonGallery).toBool())
                soImage = loadPixmap(QStringLiteral("RibbonGalleryButton"), QStringLiteral("Image_ControlGalleryPopup"));
            else if (widget->property(_qtn_ScrollUpButtonGallery).toBool())
                soImage = loadPixmap(QStringLiteral("RibbonGalleryButton"), QStringLiteral("Image_ControlGalleryUp"));
            else if (widget->property(_qtn_ScrollDownButtonGallery).toBool())
                soImage = loadPixmap(QStringLiteral("RibbonGalleryButton"), QStringLiteral("Image_ControlGalleryDown"));

            if (!soImage.isNull())
            {
                const bool enabled = opt->state & QStyle::State_Enabled;
                const bool selected = opt->state & QStyle::State_MouseOver;
                const bool pressed = opt->state & QStyle::State_Sunken;
#define CALCSTATE \
                (!enabled ? 4 : pressed ? 3 : selected ? 2 : selected || pressed ? 1 : 0)
                drawImage(soImage, *painter, toolbutton->rect, sourceRectImage(soImage.rect(), CALCSTATE, 5), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
                return true;
            }
        }
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawToolButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
//        if (d.officePaintManager()->drawGalleryToolButton(opt, p, widget))
//            return true;

        QRect menuarea;
        menuarea = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, widget);

        QStyle::State flags = toolbutton->state & ~QStyle::State_Sunken;
        if (flags & QStyle::State_AutoRaise)
        {
            if (!(flags & QStyle::State_MouseOver) || !(flags & QStyle::State_Enabled))
                flags &= ~QStyle::State_Raised;
        }
        QStyle::State mflags = flags;
        if (toolbutton->state & QStyle::State_Sunken)
        {
            if (toolbutton->activeSubControls & QStyle::SC_ToolButton)
                flags |= QStyle::State_Sunken;
            mflags |= QStyle::State_Sunken;
        }

        p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, toolbutton, painter, widget);

        QStyleOptionToolButton label = *toolbutton;
        label.state = flags;
        p.proxy()->drawControl(QStyle::CE_ToolButtonLabel, &label, painter, widget);

        if (toolbutton->subControls & QStyle::SC_ToolButtonMenu)
        {
            QStyleOption tool(0);
            tool.palette = toolbutton->palette;

            QRect ir = menuarea, rcArrow;
            if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
            {
                int heightLine = 0;
                int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget) / 2 + 2;
                bool twoLines = canWordWrap(toolbutton->rect, toolbutton->fontMetrics, toolbutton->text, &heightLine);
                rcArrow = QRect(QPoint(!twoLines ? opt->rect.width() / 2 - 2 : opt->rect.right() - mbi, opt->rect.bottom() - heightLine / 2 - 1), QSize(5, 4));
            }
            else
            {
                int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget);
                rcArrow = QRect(ir.right() - mbi - 1, opt->rect.top() + (ir.height() - mbi) / 2 + ((ir.height() - mbi) % 2), mbi, mbi);
            }

            tool.rect = rcArrow;
            tool.state = mflags;

            p.proxy()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, painter, widget);
        }
        else if (toolbutton->features & QStyleOptionToolButton::HasMenu)
        {
            QStyleOptionToolButton newBtn = *toolbutton;
            if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
            {
                int heightLine = 0;
                int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget) / 2 + 2;
                bool twoLines = canWordWrap(toolbutton->rect, toolbutton->fontMetrics, toolbutton->text, &heightLine);
                newBtn.rect = QRect(QPoint(!twoLines ? opt->rect.width() / 2 - 2 : opt->rect.right() - mbi, opt->rect.bottom() - heightLine / 2 - 1), QSize(5, 5));
            }
            else
            {
                QRect ir = menuarea;
                int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget);
                newBtn.rect = QRect(ir.right() - mbi - 1, opt->rect.top() + (ir.height() - mbi) / 2 + ((ir.height() - mbi) % 2), mbi, mbi);
            }
            p.proxy()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &newBtn, painter, widget);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawComboBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
    if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt))
    {
        QStyle::State flags = opt->state;
        QStyle::SubControls sub = opt->subControls;
        QRect r = opt->rect;

        bool enabled = flags & QStyle::State_Enabled;
        bool highlight = flags & QStyle::State_MouseOver;
        bool dropped = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && ((cmb->state & QStyle::State_On) || (cmb->state & QStyle::State_Sunken));
        bool dropButtonHot = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && (cmb->state & QStyle::State_MouseOver);
        bool hasFocus = opt->state & QStyle::State_HasFocus;

        if (cmb->frame)
        {
            r.adjust(0, 0, -1, -1);
            painter->fillRect(cmb->rect, cmb->palette.brush(enabled && (highlight || hasFocus) ? QPalette::HighlightedText : QPalette::Base));
            const QPen savePen = painter->pen();
            painter->setPen(cmb->palette.color(enabled && (highlight || hasFocus) ? QPalette::Light : QPalette::Dark));
            painter->drawRect(r);
            painter->setPen(savePen);
        }

        if (sub & QStyle::SC_ComboBoxArrow)
        {
            QRect rcBtn = p.proxy()->subControlRect(QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxArrow, w);
            if (!enabled)
                drawWidgetPixmap(QStringLiteral("QComboBox"), QStringLiteral("Image_ToolbarButtonsComboDropDown"), rcBtn, painter, 4, 5, QMargins(2, 2, 2, 2));
            else if (dropped)
                drawWidgetPixmap(QStringLiteral("QComboBox"), QStringLiteral("Image_ToolbarButtonsComboDropDown"), rcBtn, painter, 3, 5, QMargins(2, 2, 2, 2));
            else if (highlight)
            {
                int state = !cmb->editable || dropButtonHot || hasFocus ? 2 : highlight ? 1 : 2;
                drawWidgetPixmap(QStringLiteral("QComboBox"), QStringLiteral("Image_ToolbarButtonsComboDropDown"), rcBtn, painter, state, 5, QMargins(2, 2, 2, 2));
            }
            else // NORMAL
                drawWidgetPixmap(QStringLiteral("QComboBox"), QStringLiteral("Image_ToolbarButtonsComboDropDown"), rcBtn, painter, hasFocus ? 2 : 0, 5, QMargins(2, 2, 2, 2));

            QPoint pt = rcBtn.center();
            QRect rc(QPoint(pt.x() - 2, pt.y() - 2), QPoint(pt.x() + 3, pt.y() + 2));
            drawDropDownGlyph(painter, pt, highlight, dropped, enabled, false, w);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawSpinBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_P(const Office2007Style);
    if (const QStyleOptionSpinBox* sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt))
    {
        const bool highlight = opt->state & QStyle::State_MouseOver;
        const bool selected = opt->state & QStyle::State_MouseOver;
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool hasFocus = opt->state & QStyle::State_HasFocus;

        if (sb->frame && (sb->subControls & QStyle::SC_SpinBoxFrame))
        {
            QRect r = p.proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxFrame, w);

//            painter->fillRect(r, enabled && (selected || hasFocus) ? sb->palette.brush(QPalette::Normal, QPalette::Base) :
//                enabled ? sb->palette.brush(QPalette::Normal, QPalette::AlternateBase) : sb->palette.brush(QPalette::Disabled, QPalette::Base));

            painter->fillRect(sb->rect, sb->palette.brush(enabled && (highlight || hasFocus) ? QPalette::Highlight : QPalette::Base));

            const QPen savePen = painter->pen();
            painter->setPen(sb->palette.color(enabled && (highlight || hasFocus) ? QPalette::Light : QPalette::Dark));
//            painter->setPen(sb->palette.brush(QPalette::Shadow).color());
            r.adjust(0, 0, -1, -1);
            painter->drawRect(r);
            painter->setPen(savePen);
        }

        QStyleOptionSpinBox copy = *sb;
        copy.state = QStyle::State_None;
        copy.subControls |= QStyle::SC_SpinBoxUp;
        QRect rcTop = p.proxy()->subControlRect(QStyle::CC_SpinBox, &copy, QStyle::SC_SpinBoxUp, w);

        QString strBackground = QStringLiteral("Image_ToolbarButtonSpinArrowsVertical");
        
        struct ClassImage { QString strClassName; QString strImage; } classImage;
        if (rcTop.width() > 12 && rcTop.height() > 12)
        {
            classImage.strClassName = QStringLiteral("RibbonGallery");
            classImage.strImage = QStringLiteral("Image_ControlGalleryScrollArrowGlyphs");
        }
        else
        {
            classImage.strClassName = QStringLiteral("RibbonButton");
            classImage.strImage = QStringLiteral("Image_ToolbarButtonSpinArrowGlyphs");
        }

        copy = *sb;

        if (sb->subControls & QStyle::SC_SpinBoxUp)
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            rcTop = p.proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxUp, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            int state = !enabled ? 4 : (hasFocus || selected) && pressedButton ? 3 : hotButton ? 2 : (hasFocus || selected) ? 1 : 0;
            if (sb->state & QStyle::State_MouseOver)
                drawWidgetPixmap(QStringLiteral("RibbonButton"), strBackground, rcTop, painter, state, 10, QMargins(2, 2, 2, 2), QColor(0xFF, 0, 0xFF));

            state = !enabledIndicator || !enabled ? 3 : selected && pressedButton ? 2 : selected ? 2 : selected ? 1 : 0;
            QPixmap pxImage = getCachedPixmap(classImage.strClassName, classImage.strImage, QSize(), state, 16, QMargins(0, 0, 0, 0), QColor(0xFF, 0, 0xFF));
            QRect rcSrc = pxImage.rect();
            QSize sz(rcSrc.size());
            QRect rcDraw(QPoint(rcTop.left() + (rcTop.width() / 2 - sz.width() / 2), rcTop.top() + (rcTop.height() / 2 - sz.height() / 2)), rcSrc.size());
            painter->drawPixmap(rcDraw, pxImage, rcSrc);
        }

        if (sb->subControls & QStyle::SC_SpinBoxDown)
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            QRect rcBottom = p.proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxDown, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            int state = !enabled ? 9 : (hasFocus || selected) && pressedButton ? 8 : hotButton ? 7 : (hasFocus || selected) ? 6 : 5;
            if (sb->state & QStyle::State_MouseOver)
                drawWidgetPixmap(QStringLiteral("RibbonButton"), strBackground, rcBottom, painter, state, 10, QMargins(2, 2, 2, 2), QColor(0xFF, 0, 0xFF));

            state = 4 + (!enabledIndicator || !enabled ? 3 : selected && pressedButton ? 2 : selected ? 2 : selected ? 1 : 0);
            QPixmap pxImage = getCachedPixmap(classImage.strClassName, classImage.strImage, QSize(), state, 16, QMargins(0, 0, 0, 0), QColor(0xFF, 0, 0xFF));
            QRect rcSrc = pxImage.rect();
            QSize sz(rcSrc.size());
            QRect rcDraw(QPoint(rcBottom.left() + (rcBottom.width() / 2 - sz.width() / 2), rcBottom.top() + (rcBottom.height() / 2 - sz.height() / 2)), rcSrc.size());
            painter->drawPixmap(rcDraw, pxImage, rcSrc);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool Office2007StylePrivate::drawScrollBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2007Style);
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

        bool light = w && ((w->parentWidget() != Q_NULL && w->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonGallery"))) ||
            qobject_cast<const QDialog*>(w->topLevelWidget()) || QtnPrivate::qtn_isAncestor(w, QTITAN_META_CLASSNAME("RibbonBar")));

        if (opt->state & QStyle::State_Horizontal)
        {
            QString strImage = light ? QStringLiteral("Image_ControlGalleryScrollHorizontalLight") : QStringLiteral("Image_ControlGalleryScrollHorizontalDark");
            drawWidgetPixmap(QStringLiteral("QScrollBar"), strImage, newScrollbar.rect, painter, 0, 2, QMargins(0, 1, 0, 1));
        }
        else
        {
            QString strImage = light ? QStringLiteral("Image_ControlGalleryScrollVerticalLight") : QStringLiteral("Image_ControlGalleryScrollVerticalDark");
            drawWidgetPixmap(QStringLiteral("QScrollBar"), strImage, newScrollbar.rect, painter, 0, 2, QMargins(1, 0, 1, 0));
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubLine, w);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarSubLine, &newScrollbar, painter, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddLine, w);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarAddLine, &newScrollbar, painter, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSubPage)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubPage, w);

            if (newScrollbar.rect.isValid())
            {
                if (!(scrollbar->activeSubControls & QStyle::SC_ScrollBarSubPage))
                    newScrollbar.state &= ~(QStyle::State_Sunken | QStyle::State_MouseOver);
                p.proxy()->drawControl(QStyle::CE_ScrollBarSubPage, &newScrollbar, painter, w);
            }
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddPage)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddPage, w);

            if (newScrollbar.rect.isValid())
            {
                if (!(scrollbar->activeSubControls & QStyle::SC_ScrollBarAddPage))
                    newScrollbar.state &= ~(QStyle::State_Sunken | QStyle::State_MouseOver);
                p.proxy()->drawControl(QStyle::CE_ScrollBarAddPage, &newScrollbar, painter, w);
            }
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSlider)
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = p.proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSlider, w);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_ScrollBarSlider, &newScrollbar, painter, w);
        }
        return true;
    }
    return false;
}

/*! \internal */
int Office2007StylePrivate::ribbonStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    int ret = OfficeStylePrivate::ribbonStyleHint(stylehint, opt, widget, returnData);
    return ret;
}

/*! \internal */
int Office2007StylePrivate::ribbonPixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    int ret = OfficeStylePrivate::ribbonPixelMetric(metric, option, widget);
    return ret;
}

/*! \internal */
void Office2007StylePrivate::drawRectangle(QPainter* painter, const QRect& rect, bool selected, bool pressed, bool enabled,
    bool checked, bool popuped, BarType barType, BarPosition barPos) const
{
    if (barType != TypePopup)
    {
        bool smallSize = rect.height() < 33;
        QPixmap pixmap = smallSize ? loadPixmap(QStringLiteral("RibbonButton"), QStringLiteral("Image_ToolbarButtons22")) :
            loadPixmap(QStringLiteral("RibbonButton"), QStringLiteral("Image_ToolbarButtons50"));
        Q_ASSERT(!pixmap.isNull());
        if (pixmap.isNull())
            return;

        int state = -1;

        if (!enabled)
        {
        }
        else if (popuped) state = 2;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 3;
        else if ((selected && pressed)) state = 1;
        else if (selected || pressed) state = 0;

        if (state != -1)
            drawImage(pixmap, *painter, rect, sourceRectImage(pixmap.rect(), state, 4), QRect(QPoint(8, 8), QPoint(8, 8)));
    }
    else if (barPos == BarPopup && (selected || checked) && barType == TypePopup)
    {
        bool smallSize = rect.height() < 33;
        QString strImage;
        if (enabled)
            strImage = smallSize ? QStringLiteral("Image_MenuSelectedItem22") : QStringLiteral("Image_MenuSelectedItem54");
        else
            strImage = smallSize ? QStringLiteral("Image_MenuSelectedDisabledItem22") : QStringLiteral("Image_MenuSelectedDisabledItem54");
        drawWidgetPixmap(QStringLiteral("QMenu"), strImage, rect.adjusted(1, 1, -1, -1), painter, QMargins(4, 4, 4, 4));
    }
    else
    {
//        Q_ASSERT(false);
//        OfficePaintManager::drawRectangle(p, rect, selected, pressed, enabled, checked, popuped, barType, barPos);
    }
}

/*! \internal */
void Office2007StylePrivate::drawSplitButtonPopup(QPainter* p, const QRect& rect, bool selected, bool enabled, bool popuped) const
{
    if (rect.height() > 33 && enabled)
    {
        QPixmap pixmap = loadPixmap(QStringLiteral("RibbonSystemMenu"), QStringLiteral("Image_MenuSplitItem"));
        if (!pixmap.isNull())
        {
            int state = !selected ? 0 : popuped ? 2 : 1;
            QRect rcButton(rect.left(), rect.top(), rect.width() - splitActionPopupWidth, rect.height());
            drawImage(pixmap, *p, rcButton, sourceRectImage(pixmap.rect(), state, 3), QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
        }
        pixmap = loadPixmap(QStringLiteral("RibbonSystemMenu"), QStringLiteral("Image_MenuSplitDropDown"));
        int state = !selected || !enabled ? 0 : 2;
        QRect rcButton(rect.right() - splitActionPopupWidth, rect.top(), splitActionPopupWidth, rect.height());
        drawImage(pixmap, *p, rcButton, sourceRectImage(pixmap.rect(), state, 3), QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
    }
}

/*! \internal */
void Office2007StylePrivate::updateTheme()
{
    QTN_P(Office2007Style);
    QString themeName;
    switch (m_themeType)
    {
        case Office2007Style::Blue:
            themeName = QStringLiteral("Blue");
#if 0
            m_popupOptionsStyle = PopupOffice2003DrawHelper::OS_SYSTEMBLUE;
#endif
            break;
        case Office2007Style::Black:
            themeName = QStringLiteral("Black");
#if 0
            m_popupOptionsStyle = PopupOffice2003DrawHelper::OS_SYSTEMROYALE;
#endif
            break;
        case Office2007Style::Silver:
            themeName = QStringLiteral("Silver");
#if 0
            m_popupOptionsStyle = PopupOffice2003DrawHelper::OS_SYSTEMSILVER;
#endif
            break;
        case Office2007Style::Aqua:
            themeName = QStringLiteral("Aqua");
#if 0
            m_popupOptionsStyle = PopupOffice2003DrawHelper::OS_SYSTEMOLIVE;
#endif
            break;
        default:
            Q_ASSERT(false);
            break;
    }
    p.setStyleConfig(QStringLiteral(":/res/Office2007/"), themeName);
}


/*!
\class Office2007Style
\inmodule QtitanStyle
\brief Office2007Style class implements Microsoft Office 2007 style.
*/
/*!
Constuctor of Office2007Style class.
*/
Office2007Style::Office2007Style()
    : OfficeStyle(*new Office2007StylePrivate)
{
    QTN_D(Office2007Style)
    d.updateTheme();
}

/*!
Destructor of Office2007Style class.
*/
Office2007Style::~Office2007Style()
{
}

Office2007Style::Theme Office2007Style::theme() const
{
    QTN_D(const Office2007Style)
    return static_cast<Office2007Style::Theme>(d.m_themeType);
}

/*!
\property Office2007Style::theme
Sets the \a theme of the office style. You can choose from Blue, Black, Silver or Aqua theme's families.
\sa Theme
*/
void Office2007Style::setTheme(Office2007Style::Theme theme)
{
    QTN_D(Office2007Style)
    if (d.m_themeType != theme)
    {
        d.beginUpdate();
        d.m_themeType = theme;
        d.updateTheme();
        d.endUpdate();
    }
}

/*! \reimp */
QSize Office2007Style::sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget) const
{
    QTN_D(const Office2007Style)
    QSize sz = OfficeStyle::sizeFromContents(ct, opt, contentsSize, widget);
    CommonStyle::ContentsTypeEx cte = static_cast<CommonStyle::ContentsTypeEx>(ct);
    switch (cte)
    {
        case CT_RibbonSliderButton:
        {
            QString strButton = opt->direction == Qt::RightToLeft ? QStringLiteral("Image_SliderUp") : QStringLiteral("Image_SliderDown");
            QPixmap soButton = d.getCachedPixmap(QStringLiteral("QSlider"), strButton, QSize(0, 0), opt->state == QStyle::State_None ? 0 : opt->state&QStyle::State_Sunken ? 2 : 1, 3);
            sz = soButton.size();
            if (sz.isNull())
                sz = QSize(7, 7);
        }
        break;
        default:
        break;
    }
    return sz;
}

/*! \reimp */
int Office2007Style::styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const
{
    switch (hint)
    {
        case SH_ToolButtonStyle:
        {
            if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemButton")))
                return Qt::ToolButtonIconOnly;
        }
        break;
    default:
        break;
    }

    return OfficeStyle::styleHint(hint, opt, widget, shret);
}
