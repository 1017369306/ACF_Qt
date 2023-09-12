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
#include <QPainterPath>
#include "QtnOffice2016Style.h"
#include "QtnOffice2013StylePrivate.h"
#include "QtnStyleOption.h"
#include "QtnStyleHelperPrivate.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE
/* Office2016StylePrivate */
class Office2016StylePrivate : public Office2013StylePrivate
{
public:
    QTN_DECLARE_EX_PUBLIC(Office2016Style)
public:
    Office2016StylePrivate();
    virtual ~Office2016StylePrivate();
public:
    virtual void initStandardPalette(QPalette& palette) const;
public:
    /*! \internal */
    bool isQtitanStyle() const override { return true; }
    QPalette getWidgetPalette(QWidget* widget) const override;
    
    virtual bool drawRibbonBar(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonTitleBarWidget(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelStatusBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    // PE - ButtonTool
    virtual bool drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    // draw ControlElement
    virtual bool drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;

    virtual int ribbonStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const;
    // standardPixmap
    virtual QPixmap ribbonStandardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget = Q_NULL) const;
public:
    void updateTheme();
protected:
    virtual QBrush brushFromConfig(const QString& className, const QString& subControlId, QPalette::ColorGroup group, QPalette::ColorRole role, 
        QStyle::State state, bool isBackstageVisible) const;
    virtual QColor colorSubControl(const QString& className, const QString& subControlId, QPalette::ColorGroup group, QPalette::ColorRole role) const;
    virtual QColor fillTitleBarColor(const QPalette& pal, bool isBackstageVisible) const;
    virtual QColor fillBackstageFrameColor(const QPalette& pal) const;
    virtual QColor textRibbonTabBarColor(const QStyleOption* opt) const;
    virtual QColor buttonRibbonFileColor(const SystemToolButtonStyleOption* /*opt*/) const;
    virtual QColor buttonHighlightColor(const QPalette& palette, bool highlightDn, bool border, const QWidget* widget) const;
    virtual QColor titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* wd) const;
private:
    Q_DISABLE_COPY(Office2016StylePrivate)
};
QTITAN_END_NAMESPACE

Office2016StylePrivate::Office2016StylePrivate()
{
    m_themeType = Office2016Style::Colorful;
}

Office2016StylePrivate::~Office2016StylePrivate()
{
}

/*! \internal */
void Office2016StylePrivate::initStandardPalette(QPalette& palette) const
{
    if (m_themeType == Office2016Style::DarkGray || m_themeType == Office2016Style::Black)
        CommonStylePrivate::initStandardPalette(palette);
    else
        Office2013StylePrivate::initStandardPalette(palette);
}

/*! \internal */
QPalette Office2016StylePrivate::getWidgetPalette(QWidget* widget) const
{
    QTN_P(const Office2016Style);
    QPalette palette = OfficeStylePrivate::getWidgetPalette(widget);
    if (!widgetStyleSupport(widget))
        return palette;

    QColor clrAccent = p.accentColor();
    if (widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemRecentFileList")))
    {
        if ((m_themeType == Office2016Style::DarkGray || m_themeType == Office2016Style::Black))
        {
            QColor colorFill = StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.5);
            QPalette_setBrush(palette, QPalette::Active, QPalette::Highlight, colorFill);
        }
        else
        {
            QPalette_setBrush(palette, QPalette::Active, QPalette::Highlight, QColor(202, 202, 202));
        }
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageViewMenu")) && 
            (p.theme() == Office2016Style::Colorful || p.theme() == Office2016Style::White))
    {
        QPalette_setBrush(palette, QPalette::Active, QPalette::Midlight, clrAccent);
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Midlight, clrAccent);
        QPalette_setBrush(palette, QPalette::Active, QPalette::Light, clrAccent.lighter(130));
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Light, clrAccent.lighter(130));
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("StatusSeparator")) && p.theme() == Office2016Style::White)
    {
        QPalette_setBrush(palette, QPalette::Active, QPalette::Dark, clrAccent.lighter(115));
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Dark, clrAccent.lighter(115));
    }

    if (widget->inherits(QTITAN_META_CLASSNAME("StatusButton")) || (widget->parentWidget() && widget->parentWidget()->inherits("QStatusBar")))
    {
        if (p.theme() == Office2016Style::DarkGray)
        {
            QPalette_setBrush(palette, QPalette::Active, QPalette::ButtonText, Qt::white);
            QPalette_setBrush(palette, QPalette::Inactive, QPalette::ButtonText, Qt::white);
        }
        else if (p.theme() == Office2016Style::White)
        {
            QPalette_setBrush(palette, QPalette::Active, QPalette::ButtonText, Qt::white);
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


/*! \internal */
bool Office2016StylePrivate::drawRibbonBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const Office2016Style)
    if (const RibbonStyleOption* optRibbon = qstyleoption_cast<const RibbonStyleOption*>(opt))
    {
        if (m_themeType == Office2016Style::Colorful)
        {
            QRect rc = opt->rect;
            rc.setHeight(rc.height() - 1);
            painter->fillRect(rc, optRibbon->isBackstageVisible ? opt->palette.button() : p.accentColor());
            if (optRibbon->minimized)
            {
                QPainterPath path;
                path.moveTo(opt->rect.left(), opt->rect.bottom());
                path.lineTo(opt->rect.right(), opt->rect.bottom());
                const QPen savePen = painter->pen();
                painter->setPen(optRibbon->palette.midlight().color());
                painter->drawPath(path);
                painter->setPen(savePen);
            }
            if (optRibbon->quickAccessBarPosition == 2)
            {
                QRect rect = opt->rect;
                rect.setTop(rect.bottom() - optRibbon->titleBarHeight);
                painter->fillRect(rect, optRibbon->palette.window().color());
            }
        }
        else if (m_themeType == Office2016Style::DarkGray)
        {
            QRect rc = opt->rect;
            rc.setHeight(rc.height() - 1);
            painter->fillRect(rc, optRibbon->isBackstageVisible ? opt->palette.midlight() : opt->palette.dark());
            if (optRibbon->quickAccessBarPosition == 2)
            {
                QRect rect = opt->rect;
                rect.setTop(rect.bottom() - optRibbon->titleBarHeight);
                painter->fillRect(rect, optRibbon->palette.midlight().color());
            }
        }
        else if (m_themeType == Office2016Style::Black)
        {
            QRect rc = opt->rect;
            rc.setHeight(rc.height() - 1);
            painter->fillRect(rc, optRibbon->isBackstageVisible ? opt->palette.dark() : opt->palette.window());

            if (optRibbon->quickAccessBarPosition == 2)
            {
                QRect rect = opt->rect;
                rect.setTop(rect.bottom() - optRibbon->titleBarHeight);
                painter->fillRect(rect, optRibbon->palette.button().color());
            }
        }
        else
            CommonStylePrivate::drawRibbonBar(opt, painter, w);

        return true;
    }
    return false;

}

/*! \internal */
bool Office2016StylePrivate::drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (m_themeType == Office2016Style::White)
        return Office2013StylePrivate::drawRibbonTab(opt, painter, widget);

    if (const RibbonOptionHeaderStyleOption* styleOption = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        if (styleOption->state & QStyle::State_Selected)
        {
            QRect rect = styleOption->rect;
            painter->fillRect(rect, styleOption->palette.button().color());

            QPainterPath path;
            if (styleOption->tabBarPosition == 1)
            {
                path.moveTo(rect.left(), rect.bottom());
                path.lineTo(rect.left(), rect.top());
                path.lineTo(rect.right(), rect.top());
                path.lineTo(rect.right(), rect.bottom());
            }
            else if (styleOption->tabBarPosition == 2)
            {
                path.moveTo(rect.left(), rect.top());
                path.lineTo(rect.left(), rect.bottom());
                path.lineTo(rect.right(), rect.bottom());
                path.lineTo(rect.right(), rect.top());
            }
            else
                return true;

            const QPen savePen = painter->pen();
            const QColor color = CommonStyle::contextColorToColor(styleOption->pageColor);
            painter->setPen(color.isValid() ? color : styleOption->palette.dark().color().lighter(135));
            painter->drawPath(path);
            painter->setPen(savePen);
        }
        else if (opt->state & QStyle::State_MouseOver)
        {
            QRect rect = opt->rect;
            if (styleOption->tabBarPosition == 1)
                rect.adjust(1, 1, -1, 0);
            else
                rect.adjust(1, 0, -1, -1);

            if (m_themeType == Office2016Style::Black)
                painter->fillRect(rect, StyleHelper::colorMakeLighter(styleOption->palette.button().color().rgb()));
            else
                painter->fillRect(rect, StyleHelper::colorMakeLighter(m_clrAccent.rgb()));
        }

        if (styleOption->state & QStyle::State_Small)
        {
            QRect rect = styleOption->rect;
            QPainterPath path;
            path.moveTo(rect.right(), rect.top());
            path.lineTo(rect.right(), rect.bottom());
            const QPen savePen = painter->pen();
            painter->setPen(styleOption->palette.dark().color().lighter(135));
            painter->drawPath(path);
            painter->setPen(savePen);
        }

        if (styleOption->pageColor != Qtitan::ContextColorNone && 
            !(styleOption->state & QStyle::State_Selected) && !(opt->state & QStyle::State_MouseOver))
        {
            QColor clrContextTab = colorSubControl(QStringLiteral("RibbonTitleBarWidget"), QStringLiteral("ContextTab"), QPalette::Active, QPalette::Window);
            if (!clrContextTab.isValid())
            {
                if (m_themeType == Office2016Style::Colorful)
                    clrContextTab = StyleHelper::colorMakeDarker(m_clrAccent.rgb());
                else
                    clrContextTab = StyleHelper::colorMakePale(CommonStyle::contextColorToColor(styleOption->pageColor).rgb());
            }
            QRect rc = styleOption->rcFirst | styleOption->rcLast;
            painter->fillRect(rc.adjusted(0, 0, 0, -1), clrContextTab);
        }
    }
    return true;
}

/*! \internal */
bool Office2016StylePrivate::drawRibbonTitleBarWidget(const QStyleOptionComplex* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const OfficeStyle);
    if (m_themeType == Office2016Style::White)
        return Office2013StylePrivate::drawRibbonTitleBarWidget(option, painter, w);

    if (const TitleBarStyleOption* styleOption = qstyleoption_cast<const TitleBarStyleOption*>(option))
    {
        QPalette palette = styleOption->palette;
        if (styleOption->isBackstageVisible && p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonBackstageHideTabs)))
        {
            QRect rect = styleOption->rect;
            rect.setWidth(styleOption->menuWidth);
            painter->fillRect(rect, fillBackstageFrameColor(styleOption->paletteBackstageFrameMenu));
            QPalette_setColor(palette, QPalette::Active, QPalette::WindowText, palette.color(QPalette::BrightText));
            QPalette_setColor(palette, QPalette::Inactive, QPalette::WindowText, palette.color(QPalette::BrightText));
        }

        for (QVector<TitleBarStyleOption::ContextualTabInfo>::const_iterator it = styleOption->contextualTabs.constBegin();
            it != styleOption->contextualTabs.constEnd(); ++it)
        {
            QRect rc = it->rect;
            QColor color = CommonStyle::contextColorToColor(it->color);
            QColor clrLight = StyleHelper::colorMakePale(color.rgb());

            if (m_themeType == Office2016Style::Colorful)
                clrLight = StyleHelper::colorMakeDarker(m_clrAccent.rgb());
            else
            {
                QColor clrContextTab = colorSubControl(QStringLiteral("RibbonTitleBarWidget"), QStringLiteral("ContextTab"), QPalette::Active, QPalette::Window);
                if (clrContextTab.isValid())
                    clrLight = clrContextTab;
            }
            painter->fillRect(rc, clrLight);

            QRect rectTop = rc;
            rectTop.setBottom(rectTop.top() + 4);
            painter->fillRect(rectTop, color);

            QString text = it->strTitle;
            if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), option, w))
                text = text.toUpper();

            QPalette pal = option->palette;
            painter->setPen(color);
            painter->setFont(styleOption->fontTextTitle);
            painter->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine, painter->fontMetrics().elidedText(text, Qt::ElideRight, rc.width()));
        }
        painter->setFont(styleOption->fontTextTitle);
        QString text = QFontMetrics(styleOption->fontTextTitle).elidedText(styleOption->text, Qt::ElideRight, styleOption->rcTitleText.width());
        p.proxy()->drawItemText(painter, styleOption->rcTitleText, styleOption->alignment, palette, true/*enabled*/, text, QPalette::WindowText);
        return true;
    }
    return false;
}

/*! \internal */
bool Office2016StylePrivate::drawPanelStatusBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w);
    if (m_themeType == Office2016Style::Black || m_themeType == Office2016Style::DarkGray)
        painter->fillRect(opt->rect, opt->palette.window().color());
    else if (m_themeType == Office2016Style::Colorful)
        return CommonStylePrivate::drawPanelStatusBar(opt, painter, w);
    else
        return Office2013StylePrivate::drawPanelStatusBar(opt, painter, w);
    return true;
}

/*! \internal */
bool Office2016StylePrivate::drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (m_themeType == Office2016Style::Colorful)
    {
        if (widget && (widget->inherits(QTITAN_META_CLASSNAME("StatusButton")) || (widget->parentWidget() && widget->parentWidget()->inherits("QStatusBar"))))
        {
            const bool isOn = opt->state & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised);
            const bool isPress = opt->state & QStyle::State_Sunken;
            const bool isHighlight = opt->state & QStyle::State_MouseOver;
            QColor fill = opt->palette.color(QPalette::Active, QPalette::Button);
            if (isPress)
                fill = StyleHelper::colorMakeDarker(opt->palette.color(QPalette::Shadow).rgb(), 0.1);
            else if (isHighlight)
                fill = StyleHelper::colorMakeDarker(opt->palette.color(QPalette::Shadow).rgb(), 0.01);
            else if (isOn)
                fill = StyleHelper::colorMakeDarker(opt->palette.color(QPalette::Shadow).rgb(), 0.1);
            painter->fillRect(opt->rect, fill);
            return true;
        }
    }
    return Office2013StylePrivate::drawPanelButtonTool(opt, painter, widget);
}

/*! \internal */
bool Office2016StylePrivate::drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (m_themeType == Office2016Style::Colorful)
    {
        QPoint pt(opt->rect.right() - 3, opt->rect.bottom() - 3);
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 3 - y; x++)
                painter->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), opt->palette.dark().color().darker(110));
        }
        return true;
    }
    return Office2013StylePrivate::drawSizeGrip(opt, painter, w);
}

/*! \internal */
int Office2016StylePrivate::ribbonStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    QTN_P(const Office2016Style);
    int ret = CommonStylePrivate::InvalidHint;
    switch (static_cast<CommonStyle::StyleHintEx>(stylehint))
    {
        case CommonStyle::SH_WindowTitleBarPalette:
            if (QStyleHintReturnVariant* vret = qstyleoption_cast<QStyleHintReturnVariant*>(returnData))
            {
                QPalette pal = p.standardPalette();
                if (m_themeType == Office2016Style::Colorful)
                    QPalette_setColor(pal, QPalette::WindowText, qtn_isColorLight(m_clrAccent) ? Qt::black : Qt::white);
                else if (m_themeType == Office2016Style::DarkGray)
                    QPalette_setColor(pal, QPalette::WindowText, Qt::white);
                vret->variant = pal;
                ret = true;
            }
            break;
        case CommonStyle::SH_RibbonItemUpperCase: ret = false; break;
        default: ret = Office2013StylePrivate::ribbonStyleHint(stylehint, opt, widget, returnData); break;
    }
    return ret;
}

// standardPixmap
QPixmap Office2016StylePrivate::ribbonStandardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    QPixmap px;
    switch (static_cast<CommonStyle::StandardPixmapEx>(sp))
    {
        case CommonStyle::SP_RibbonButtonPopupMenu:
            px = CommonStylePrivate::standardPixmap(QStringLiteral("Image_RibbonButtonPopupMenu"), (opt && opt->state & QStyle::State_Enabled) ? CommonStylePrivate::Black : CommonStylePrivate::Gray, widget);
            break;
        case CommonStyle::SP_RibbonButtonGroupOption:
            px = CommonStylePrivate::standardPixmap(QStringLiteral("Image_GroupOption"), (opt && opt->state & QStyle::State_Enabled) ? CommonStylePrivate::Black : CommonStylePrivate::Gray, widget);
            break;
        default:
            break;
    };
    return px.isNull() ? Office2013StylePrivate::ribbonStandardPixmap(sp, opt, widget) : px;
}

/*! \internal */
void Office2016StylePrivate::updateTheme()
{
    QTN_P(OfficeStyle);
    QString themeName;
    switch (m_themeType)
    {
        case Office2016Style::Colorful:
            themeName = QStringLiteral("Colorful");
            break;
        case Office2016Style::White:
            themeName = QStringLiteral("White");
            break;
        case Office2016Style::DarkGray:
            themeName = QStringLiteral("DarkGray");
            break;
        case Office2016Style::Black:
            themeName = QStringLiteral("Black");
            break;
        default:
            break;
    }
    p.setStyleConfig(QStringLiteral(":/res/Office2016/"), themeName);
}

/*! \internal */
QBrush Office2016StylePrivate::brushFromConfig(const QString& className, const QString& subControlId, QPalette::ColorGroup group, QPalette::ColorRole role, 
    QStyle::State state, bool isBackstageVisible) const
{
    if (m_themeType == Office2016Style::Colorful)
    {
        if (isBackstageVisible)
        {
            if ((role == QPalette::HighlightedText) && (state & QStyle::State_MouseOver) && QStringLiteral("TitleBarCloseButton") == subControlId)
                return Office2013StylePrivate::brushSubControl(className, subControlId, group, role);
            else if (role == QPalette::WindowText)
                return Office2013StylePrivate::brushSubControl(className, subControlId, group, QPalette::BrightText);
        }
    }
    else if (m_themeType == Office2016Style::DarkGray)
    {
        if (isBackstageVisible)
        {
            if ((role == QPalette::HighlightedText) && (state & QStyle::State_MouseOver) && QStringLiteral("TitleBarCloseButton") == subControlId)
                return Office2013StylePrivate::brushSubControl(className, subControlId, group, role);
            else if (role == QPalette::WindowText)
                return Office2013StylePrivate::brushSubControl(className, subControlId, group, QPalette::BrightText);
        }
    }
    return Office2013StylePrivate::brushSubControl(className, subControlId, group, role);
}

/*! \internal */
QColor Office2016StylePrivate::colorSubControl(const QString& className, const QString& subControlId, QPalette::ColorGroup group, QPalette::ColorRole role) const
{
    return Office2013StylePrivate::colorSubControl(className, subControlId, group, role);
}

/*! \internal */
QColor Office2016StylePrivate::fillTitleBarColor(const QPalette& pal, bool isBackstageVisible) const
{
    QTN_P(const Office2016Style);
    if (m_themeType == Office2016Style::Colorful)
        return isBackstageVisible ? pal.window().color() : p.accentColor();
    else if (m_themeType == Office2016Style::DarkGray)
        return isBackstageVisible ? pal.midlight().color() : pal.dark().color();
    else if (m_themeType == Office2016Style::Black && isBackstageVisible)
        return isBackstageVisible ? brushFromConfig(QStringLiteral("QMainWindow"), QStringLiteral("BackstageTitleBar"),
            QPalette::Active, QPalette::Window, QStyle::State_None, isBackstageVisible).color() : pal.window().color();
    else
        return Office2013StylePrivate::fillTitleBarColor(pal, isBackstageVisible);
}

/*! \internal */
QColor Office2016StylePrivate::fillBackstageFrameColor(const QPalette& pal) const
{
    if (m_themeType == Office2016Style::Black || m_themeType == Office2016Style::DarkGray)
        return pal.color(QPalette::Window);
    else
        return Office2013StylePrivate::fillBackstageFrameColor(pal);
}

/*! \internal */
QColor Office2016StylePrivate::textRibbonTabBarColor(const QStyleOption* opt) const
{
    QTN_P(const Office2016Style);
    if ((m_themeType == Office2016Style::Colorful) && 
        (opt->state & QStyle::State_MouseOver) && !(opt->state & QStyle::State_Selected))
        return p.accentColor().lighter(240);
    else if (m_themeType == Office2016Style::DarkGray)
    {
        if ((opt->state & QStyle::State_Selected))
            return opt->palette.color(QPalette::Text);
        return opt->palette.color(QPalette::WindowText);
    }
    else if ((m_themeType == Office2016Style::Black) && ((opt->state & QStyle::State_MouseOver) || (opt->state & QStyle::State_Selected)))
        return opt->palette.color(QPalette::WindowText);
    return Office2013StylePrivate::textRibbonTabBarColor(opt);
}

/*! \internal */
QColor Office2016StylePrivate::buttonRibbonFileColor(const SystemToolButtonStyleOption* toolbutton) const
{
    QTN_P(const Office2016Style);
    const bool popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (toolbutton->state & QStyle::State_Sunken);
    const bool isHighlighted = toolbutton->state & QStyle::State_MouseOver;
    QColor fillColor = p.accentColor();
    if (isHighlighted && !popuped)
        fillColor = fillColor.lighter(125);
    else if (isHighlighted || popuped)
        fillColor = fillColor.lighter(70);
    else if (m_themeType == Office2016Style::DarkGray || m_themeType == Office2016Style::Black)
        fillColor = toolbutton->palette.color(QPalette::Button);
    return fillColor;
}

/*! \internal */
QColor Office2016StylePrivate::buttonHighlightColor(const QPalette& palette, bool highlightDn, bool border, const QWidget* widget) const
{
    QTN_P(const Office2016Style)
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageView")))
    {
        if (border)
            return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.2) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.2);
        return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.11) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.19);
    }

    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonButton")))
    {
        if (widget->parentWidget() && widget->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonTabBar")))
        {
            if (m_themeType == Office2016Style::Colorful)
                return highlightDn ? StyleHelper::colorMakeDarker(p.accentColor().rgb(), 0.10) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.14);
        }
    }
    else if (widget && widget->inherits("QToolButton"))
    {
        if (QWidget* parent = widget->parentWidget())
        {
            if (parent && parent->inherits(QTITAN_META_CLASSNAME("RibbonQuickAccessBar")) &&
                parent->parentWidget() && parent->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonTitleBarWidget")))
            {
                if (m_themeType == Office2016Style::Colorful)
                    return highlightDn ? StyleHelper::colorMakeDarker(p.accentColor().rgb(), 0.10) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 0.14);
            }
        }
    }
    else if (widget && widget->isWindow())
    {
        const bool isBackstageVisible = qtn_isBackstageVisible(widget);
        QColor color = isBackstageVisible && m_themeType == Office2016Style::Colorful ? palette.color(QPalette::Shadow) : p.accentColor().rgb();
        if (m_themeType == Office2016Style::DarkGray || m_themeType == Office2016Style::Black)
            color = palette.color(QPalette::Shadow);
        else if (widget->inherits("QMenu") && m_themeType == Office2016Style::Colorful)
            color = palette.color(QPalette::Shadow);

        if (palette.color(QPalette::Active, QPalette::Highlight) == Qt::red)
        {
            color = Qt::red;
            if (m_themeType == Office2016Style::White)
                color = color.darker();
        }

        if (m_themeType == Office2016Style::Colorful)
        {
            if (isBackstageVisible)
                return highlightDn ? StyleHelper::colorMakeDarker(color.rgb(), 0.10) : StyleHelper::colorMakeLighter(color.rgb(), 0.05);
            else
                return highlightDn ? StyleHelper::colorMakeDarker(color.rgb(), 0.10) : StyleHelper::colorMakeLighter(color.rgb(), 0.14);
        }
        else if (m_themeType == Office2016Style::White)
        {
            if (isBackstageVisible)
                return highlightDn ? StyleHelper::colorMakeLighter(color.rgb(), 1.35) : StyleHelper::colorMakeLighter(color.rgb(), 1.45);
            else
                return highlightDn ? StyleHelper::colorMakeLighter(color.rgb(), 1.35) : StyleHelper::colorMakeLighter(color.rgb(), 1.45);
        }
        else if (m_themeType == Office2016Style::DarkGray)
        {
            if (isBackstageVisible)
                return highlightDn ? StyleHelper::colorMakeLighter(color.rgb(), 0.5) : StyleHelper::colorMakeLighter(color.rgb(), 0.0);
            else
                return highlightDn ? StyleHelper::colorMakeLighter(color.rgb(), 0.5) : StyleHelper::colorMakeLighter(color.rgb(), 0.0);
        }
        else if (m_themeType == Office2016Style::Black)
        {
            if (isBackstageVisible)
                return highlightDn ? StyleHelper::colorMakeLighter(color.rgb(), 0.5) : StyleHelper::colorMakeLighter(color.rgb(), 0.2);
            else
                return highlightDn ? StyleHelper::colorMakeLighter(color.rgb(), 0.5) : StyleHelper::colorMakeLighter(color.rgb(), 0.2);
        }
    }
    if (m_themeType == Office2016Style::White)
    {
        if (border)
            return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.3) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.2);
        return highlightDn ? StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.3) : StyleHelper::colorMakeLighter(p.accentColor().rgb(), 1.4);
    }

    QColor color = palette.color(QPalette::Shadow);
    if (m_themeType == Office2016Style::Black)
        return highlightDn ? StyleHelper::colorMakeLighter(color.rgb(), 0.4) : StyleHelper::colorMakeLighter(color.rgb(), 0.8);
    else
    {
        if (border)
            return highlightDn ? StyleHelper::colorMakeDarker(color.rgb(), 0.3) : StyleHelper::colorMakeDarker(color.rgb(), 0.2);
        return highlightDn ? StyleHelper::colorMakeDarker(color.rgb(), 0.10) : StyleHelper::colorMakeDarker(color.rgb(), 0.01);
    }
}

/*! \internal */
QColor Office2016StylePrivate::titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* widget) const
{
    QColor color = Office2013StylePrivate::titleBarFrameColor(pal, isActive, widget);
    if (qtn_hasWindowTitleBar(widget))
    {
        if (m_themeType == Office2016Style::DarkGray)
        {
            QPalette palette = pal;
            if (!isActive)
                QPalette_setColor(palette, QPalette::Inactive, QPalette::Dark, palette.color(QPalette::Shadow));
            color = palette.color(isActive ? QPalette::Active : QPalette::Inactive, QPalette::Dark);
        }
        else if (m_themeType == Office2016Style::Black)
        {
            QPalette palette = pal;
            if (isActive)
                QPalette_setColor(palette, QPalette::Active, QPalette::Dark, palette.color(QPalette::Light));
            else
                QPalette_setColor(palette, QPalette::Inactive, QPalette::Dark, Qt::black);
            color = palette.color(isActive ? QPalette::Active : QPalette::Inactive, QPalette::Dark);
        }
    }
    return color;
}

/*!
\class Office2016Style
\inmodule QtitanStyle
\brief Office2016Style class implements Microsoft Office 2016 style.
*/

/*!
Constuctor of Office2016Style class. 
*/
Office2016Style::Office2016Style()
    : OfficeStyle(*new Office2016StylePrivate)
{
    QTN_D(Office2016Style)
    d.updateTheme();
}

/*!
Destructor of Office2016Style class.
*/
Office2016Style::~Office2016Style()
{
}

Office2016Style::Theme Office2016Style::theme() const
{
    QTN_D(const Office2016Style)
    return static_cast<Office2016Style::Theme>(d.m_themeType);
}

/*!
\property OfficeStyle::theme
Sets the \a theme of the office style. You can choose from Colorful, White, DarkGray or Black theme's families.
\sa Theme
*/
void Office2016Style::setTheme(Office2016Style::Theme theme)
{
    QTN_D(Office2016Style)
    if (d.m_themeType != theme)
    {
        d.beginUpdate();
        d.m_themeType = theme;
        d.updateTheme();
        d.endUpdate();
    }
}

/*! \reimp */
QRect Office2016Style::subControlRect(QStyle::ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
{
    if (control == CC_TitleBar && widget && widget->inherits("QMdiSubWindow"))
        return QCommonStyle::subControlRect(QStyle::CC_TitleBar, option, subControl, widget);

    QRect rect = OfficeStyle::subControlRect(control, option, subControl, widget);
    switch (control)
    {
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
QPixmap Office2016Style::standardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const Office2016Style);
    QPixmap px;
    CommonStyle::StandardPixmapEx spEx = static_cast<CommonStyle::StandardPixmapEx>(sp);
    switch (spEx)
    {
        case SP_RibbonButtonPopupMenu :
            if (d.m_themeType == Office2016Style::DarkGray)
            {
                if (QtnPrivate::qtn_isAncestor(widget, QTITAN_META_CLASSNAME("RibbonQuickAccessBar")))
                    px = d.standardPixmap(QStringLiteral("Image_RibbonButtonPopupMenu"), (opt && !(opt->state & QStyle::State_Enabled)) ? CommonStylePrivate::Gray : CommonStylePrivate::White, widget);
            }
            break;
        default:
            break;
    }

    switch (sp)
    {
        case SP_ArrowUp:
            if (d.m_themeType == Office2016Style::Colorful || d.m_themeType == Office2016Style::DarkGray)
            {
                if (QtnPrivate::qtn_isAncestor(widget, QTITAN_META_CLASSNAME("RibbonTabBar")))
                    px = d.standardPixmap(QStringLiteral("Image_ShadedArrowUp"), (opt && !(opt->state & QStyle::State_Enabled)) ? CommonStylePrivate::Gray : CommonStylePrivate::White, widget);
            }
            break;
        default:
            break;
    };
    return px.isNull() ? OfficeStyle::standardPixmap(sp, opt, widget) : px;
}

/*! \reimp */
QIcon Office2016Style::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const Office2016Style);
    if (widget != Q_NULL && widget->inherits("QDockWidget"))
    {
        if (standardIcon == QStyle::SP_TitleBarCloseButton)
            return QIcon(d.standardPixmap(QStringLiteral("Image_Close"), CommonStylePrivate::Black, widget));
        else if (standardIcon == QStyle::SP_TitleBarNormalButton)
            return QIcon(d.standardPixmap(QStringLiteral("Image_Normal"), CommonStylePrivate::Black, widget));
    }
    return OfficeStyle::standardIcon(standardIcon, opt, widget);
}

/*! \reimp */
int Office2016Style::styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const
{
#if 0
    if (widget && widget->inherits("QMdiSubWindow") && hint == QStyle::SH_WindowFrame_Mask)
        return CommonStyle::styleHint(hint, opt, widget, shret);
#endif
    return CommonStyle::styleHint(hint, opt, widget, shret);
}

