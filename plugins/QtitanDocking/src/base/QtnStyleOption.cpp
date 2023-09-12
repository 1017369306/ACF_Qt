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
#include "QtnStyleOption.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

/* FrameStyleOption */
FrameStyleOption::FrameStyleOption()
    : QStyleOptionFrame(FrameStyleOption::Version)
    , section(Qt::NoSection)
{
}

FrameStyleOption::FrameStyleOption(const FrameStyleOption& other) : QStyleOptionFrame(FrameStyleOption::Version)
{
    *this = other;
}


/* BackstageButtonStyleOption */
BackstageButtonStyleOption::BackstageButtonStyleOption()
    : QStyleOptionToolButton(BackstageButtonStyleOption::Version)
    , tabStyle(false)
    , document(Q_NULL)
{
}

BackstageButtonStyleOption::BackstageButtonStyleOption(const BackstageButtonStyleOption& other) : QStyleOptionToolButton(BackstageButtonStyleOption::Version)
{
    *this = other;
}

/* RibbonBackstageStyleOption */
RibbonBackstageStyleOption::RibbonBackstageStyleOption()
    : QStyleOption(RibbonBackstageStyleOption::Version)
    , menuWidth(-1)
{
}

RibbonBackstageStyleOption::RibbonBackstageStyleOption(const RibbonBackstageStyleOption& other) : QStyleOption(RibbonBackstageStyleOption::Version)
{
    *this = other;
}

/* RibbonStyleOption */
RibbonStyleOption::RibbonStyleOption()
    : QStyleOption(RibbonStyleOption::Version)
    , titleBarHeight(0)
    , minimized(false)
    , isBackstageVisible(false)
    , tabBarPosition(1)
    , quickAccessBarPosition(1)
    , pageColor(Qtitan::ContextColorNone)
{
}

RibbonStyleOption::RibbonStyleOption(const RibbonStyleOption& other) : QStyleOption(RibbonStyleOption::Version)
{
    *this = other;
}

/* SystemToolButtonStyleOption */
SystemToolButtonStyleOption::SystemToolButtonStyleOption()
    : QStyleOptionToolButton(SystemToolButtonStyleOption::Version)
{
}

SystemToolButtonStyleOption::SystemToolButtonStyleOption(const SystemToolButtonStyleOption& other)
    : QStyleOptionToolButton(SystemToolButtonStyleOption::Version)
{
    *this = other;
}

/* TitleBarStyleOption */
TitleBarStyleOption::TitleBarStyleOption()
    : QStyleOptionTitleBar(TitleBarStyleOption::Version)
    , drawIcon(true)
    , isBackstageVisible(false)
    , menuWidth(0)
    , alignment(0)
    , clrText()
    , fontTextTitle(QApplication::font("QMdiSubWindowTitleBar"))
{
}

TitleBarStyleOption::TitleBarStyleOption(const TitleBarStyleOption& other)
    : QStyleOptionTitleBar(TitleBarStyleOption::Version)
{
    *this = other;
}

/* CaptionButtonStyleOption */
CaptionButtonStyleOption::CaptionButtonStyleOption()
    : QStyleOptionToolButton(CaptionButtonStyleOption::Version)
{
}

/*! RibbonQuickAccessBarStyleOption */
RibbonQuickAccessBarStyleOption::RibbonQuickAccessBarStyleOption()
    : QStyleOption(RibbonQuickAccessBarStyleOption::Version)
    , quickAccessBarPosition(1)
    , roundButton(false)
{
}

RibbonQuickAccessBarStyleOption::RibbonQuickAccessBarStyleOption(const RibbonQuickAccessBarStyleOption& other)
    : QStyleOption(RibbonQuickAccessBarStyleOption::Version)
{
    *this = other;
}

/* RibbonGroupStyleOption */
RibbonGroupStyleOption::RibbonGroupStyleOption()
    : QStyleOption(RibbonGroupStyleOption::Version)
    , widthOptButton(0)
    , heightCaption(0)
    , text(QString())
    , icon(QIcon())
    , iconSize(QSize(32, 32))
    , subControls(QStyle::SC_ToolButton)
    , features(QStyleOptionToolButton::HasMenu)
    , textAlignment(Qt::AlignCenter)
    , textElideMode(Qt::ElideNone)
    , pageColor(Qtitan::ContextColorNone)
{
}

RibbonGroupStyleOption::RibbonGroupStyleOption(const RibbonGroupStyleOption& other) : QStyleOption(RibbonGroupStyleOption::Version)
{
    *this = other;
}

/* RibbonFrameMenuStyleOption */
RibbonFrameMenuStyleOption::RibbonFrameMenuStyleOption()
    : QStyleOption(RibbonFrameMenuStyleOption::Version)
    , isAdditionalPages(false)
    , actionsWidth(0)
    , actionsHeight(0)
    , margins()
{
}

RibbonFrameMenuStyleOption::RibbonFrameMenuStyleOption(const RibbonFrameMenuStyleOption& other)
    : QStyleOption(RibbonFrameMenuStyleOption::Version)
{
    *this = other;
}

/* RibbonGalleryItemStyleOption */
RibbonGalleryItemStyleOption::RibbonGalleryItemStyleOption()
    : QStyleOption(RibbonGalleryItemStyleOption::Version)
    , separator(false)
    , rectItem(QRect())
    , caption()
    , icon()
{
}

RibbonGalleryItemStyleOption::RibbonGalleryItemStyleOption(const RibbonGalleryItemStyleOption& other)
    : QStyleOption(RibbonGalleryItemStyleOption::Version)
{
    *this = other;
}

/* StyleOptionMenuScrollItem */
StyleOptionMenuScrollItem::StyleOptionMenuScrollItem()
    : QStyleOptionMenuItem(StyleOptionMenuScrollItem::Version)
{
}

StyleOptionMenuScrollItem::StyleOptionMenuScrollItem(const StyleOptionMenuScrollItem& other)
    : QStyleOptionMenuItem(StyleOptionMenuScrollItem::Version)
{
    *this = other;
}

/* HintReturnSysFrameMenuStyleOption */
HintReturnSysFrameMenuStyleOption::HintReturnSysFrameMenuStyleOption(const QMargins& mrgs)
    : QStyleHintReturn()
    , margins(mrgs)
{
}

/* AutoHideWindowStyleOption */
AutoHideWindowStyleOption::AutoHideWindowStyleOption()
    : QStyleOption(AutoHideWindowStyleOption::Version)
    , position(Qtitan::LeftArea)
{
}

AutoHideWindowStyleOption::AutoHideWindowStyleOption(const AutoHideWindowStyleOption& other)
    : QStyleOption(AutoHideWindowStyleOption::Version)
{
    *this = other;
}

/* DockTitleBarStyleOption */
DockTitleBarStyleOption::DockTitleBarStyleOption()
    : QStyleOption(DockTitleBarStyleOption::Version)
    , lineRect()
    , closable(false)
    , movable(false)
    , floatable(false)
    , autohide(false)
    , menu(false)
    , verticalTitleBar(false)
{
}

DockTitleBarStyleOption::DockTitleBarStyleOption(const DockTitleBarStyleOption& other)
    : QStyleOption(DockTitleBarStyleOption::Version)
{
    *this = other;
}

/* DockMarkerStyleOption */
DockMarkerStyleOption::DockMarkerStyleOption()
    : QStyleOption(DockMarkerStyleOption::Version)
    , sp(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_DockMarkerBasis))
    , position()
    , basis(false)
{
}

DockMarkerStyleOption::DockMarkerStyleOption(const DockMarkerStyleOption& other) : QStyleOption(DockMarkerStyleOption::Version)
{
    *this = other;
}

/* DockPhantomStyleOption */
DockPhantomStyleOption::DockPhantomStyleOption()
    : QStyleOption(DockPhantomStyleOption::Version)
    , drawFrame(false)
    , posTab(Qtitan::DefaultArea)
    , borderWidth(7)
{
}

DockPhantomStyleOption::DockPhantomStyleOption(const DockPhantomStyleOption& other) 
    : QStyleOption(DockPhantomStyleOption::Version)
{
    *this = other;
}

/* ToggleSwitchStyleOption */
ToggleSwitchStyleOption::ToggleSwitchStyleOption()
    : QStyleOptionComplex(ToggleSwitchStyleOption::Version)
    , isAnimated(false)
    , dontDrawThumbOn(false)
    , dontDrawThumbOff(false)
    , sizeIndicator()
    , orientation(Qt::Horizontal)
    , alignment(Qt::AlignLeft)
    , subControls(static_cast<CommonStyle::SubControlEx>(QStyle::SC_None))
    , shape(ToggleSwitch::Circle)
    , animationOffset(0.0)
    , disabledOpacity(0.4)
    , statusText()
    , text()
{
}

ToggleSwitchStyleOption::ToggleSwitchStyleOption(const ToggleSwitchStyleOption& other)
    : QStyleOptionComplex(ToggleSwitchStyleOption::Version)
{
    *this = other;
}

/* StyleOptionTab */
StyleOptionTab::StyleOptionTab() 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    : QStyleOptionTab(StyleOptionTab::Version)
#else
    : QStyleOptionTabV3(StyleOptionTab::Version)
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    , tabIndex(-1)
    , hoverClose(false)
{ 
}

StyleOptionTab::StyleOptionTab(int version)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    : QStyleOptionTab(version)
#else
    : QStyleOptionTabV3(version)
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    , tabIndex(-1)
    , hoverClose(false)
{
}

/* RibbonOptionHeaderStyleOption */
RibbonOptionHeaderStyleOption::RibbonOptionHeaderStyleOption()
    : StyleOptionTab(RibbonOptionHeaderStyleOption::Version)
    , firstTab(false)
    , lastTab(false)
    , minimized(false)
    , pageColor(Qtitan::ContextColorNone)
    , tabBarPosition(1)
    , textAlignment(Qt::AlignLeft)
{
}

RibbonOptionHeaderStyleOption::RibbonOptionHeaderStyleOption(const RibbonOptionHeaderStyleOption& other)
    : StyleOptionTab(RibbonOptionHeaderStyleOption::Version)
{
    *this = other;
}

/* GridHeaderStyleOption */
GridHeaderStyleOption::GridHeaderStyleOption()
    : QStyleOptionHeader(GridHeaderStyleOption::Version),
    margins(QMargins()), textElideMode(Qt::ElideRight),
    scaleFactor(1.0), decorationColor(Qt::transparent), textDocument(Q_NULL)
{
}

GridHeaderStyleOption::GridHeaderStyleOption(const QStyleOptionHeader& option)
    : QStyleOptionHeader(option),
    margins(QMargins()), textElideMode(Qt::ElideRight),
    scaleFactor(1.0), decorationColor(Qt::transparent), textDocument(Q_NULL)
{
    version = GridHeaderStyleOption::Version;
}

GridHeaderStyleOption::GridHeaderStyleOption(const GridHeaderStyleOption& other)
    : QStyleOptionHeader(GridHeaderStyleOption::Version)
{
    *this = other;
}

