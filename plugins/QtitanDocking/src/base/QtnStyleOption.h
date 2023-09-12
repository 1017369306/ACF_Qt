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
#ifndef QTN_STYLEOPTION_H
#define QTN_STYLEOPTION_H

#include <QStyleOption>
#include <QStyleOptionFrame>
#include <QtnCommonStyle.h>
#include "QtnToggleSwitch.h"
#include "QtitanDef.h"

class QTextDocument;

QTITAN_BEGIN_NAMESPACE

// to support Qt4
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
typedef QStyleOptionFrame QtStyleOptionFrame;
#else
typedef QStyleOptionFrameV3 QtStyleOptionFrame;
#endif

#define QTN_OPT_VERSION 100

class QTITAN_BASE_EXPORT FrameStyleOption : public QStyleOptionFrame
{
public:
    enum StyleOptionVersion { Version = QStyleOptionFrame::Version + QTN_OPT_VERSION };
    FrameStyleOption();
    FrameStyleOption(const FrameStyleOption& other);
    FrameStyleOption& operator=(const FrameStyleOption&) = default;
public:
    Qt::WindowFrameSection section;
};

class QTITAN_BASE_EXPORT BackstageButtonStyleOption : public QStyleOptionToolButton
{
public:
    enum StyleOptionVersion { Version = QStyleOptionToolButton::Version + QTN_OPT_VERSION };
    BackstageButtonStyleOption();
    BackstageButtonStyleOption(const BackstageButtonStyleOption& other);
    BackstageButtonStyleOption& operator=(const BackstageButtonStyleOption&) = default;
public:
    bool tabStyle;
    QTextDocument* document;
};

class QTITAN_BASE_EXPORT RibbonBackstageStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    RibbonBackstageStyleOption();
    RibbonBackstageStyleOption(const RibbonBackstageStyleOption& other);
    RibbonBackstageStyleOption& operator=(const RibbonBackstageStyleOption&) = default;
public:
    int menuWidth;
};

class QTITAN_BASE_EXPORT RibbonStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    RibbonStyleOption();
    RibbonStyleOption(const RibbonStyleOption& other);
    RibbonStyleOption& operator=(const RibbonStyleOption&) = default;
public:
    QRect rectTabBar;
    QRect rectCurrentTab;
    int titleBarHeight;
    bool minimized;
    bool isBackstageVisible;
    uint tabBarPosition;
    uint quickAccessBarPosition;
    Qtitan::ContextColor pageColor;
};

class QTITAN_BASE_EXPORT SystemToolButtonStyleOption : public QStyleOptionToolButton
{
public:
    enum StyleOptionVersion { Version = QStyleOptionToolButton::Version + QTN_OPT_VERSION };
    SystemToolButtonStyleOption();
    SystemToolButtonStyleOption(const SystemToolButtonStyleOption& other);
    SystemToolButtonStyleOption& operator=(const SystemToolButtonStyleOption&) = default;
};

class QTITAN_BASE_EXPORT TitleBarStyleOption : public QStyleOptionTitleBar
{
public:
    enum StyleOptionVersion { Version = QStyleOptionTitleBar::Version + QTN_OPT_VERSION };
    struct ContextualTabInfo
    {
        ContextualTabInfo()
            : rect(QRect())
            , strTitle()
            , color(Qtitan::ContextColorNone) {}
        ContextualTabInfo(const QRect& _rect, const QString& _strTitle, Qtitan::ContextColor _color)
            : rect(_rect)
            , strTitle(_strTitle)
            , color(_color) {}
        QRect rect;
        QString strTitle;
        Qtitan::ContextColor color;
    };
    TitleBarStyleOption();
    TitleBarStyleOption(const TitleBarStyleOption& other);
    TitleBarStyleOption& operator=(const TitleBarStyleOption&) = default;
public:
    QRect rcTitleText;
    QRect rectTabBar;
    QRect rcRibbonClient;
    QRect rcQuickAccess;
    bool drawIcon;
    bool isBackstageVisible;
    int menuWidth;
    QPalette paletteBackstageFrameMenu;
    int alignment;
    QVector<ContextualTabInfo> contextualTabs;
    QColor clrText;
    QFont fontTextTitle;
    SystemToolButtonStyleOption optButton;
protected:
};

class QTITAN_BASE_EXPORT CaptionButtonStyleOption : public QStyleOptionToolButton
{
public:
    enum StyleOptionVersion { Version = QStyleOptionToolButton::Version + QTN_OPT_VERSION };
    CaptionButtonStyleOption();
};

class QTITAN_BASE_EXPORT RibbonQuickAccessBarStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    RibbonQuickAccessBarStyleOption();
    RibbonQuickAccessBarStyleOption(const RibbonQuickAccessBarStyleOption& other);
    RibbonQuickAccessBarStyleOption& operator=(const RibbonQuickAccessBarStyleOption&) = default;
public:
    uint quickAccessBarPosition;
    bool roundButton;
};

class QTITAN_BASE_EXPORT RibbonGroupStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    RibbonGroupStyleOption();
    RibbonGroupStyleOption(const RibbonGroupStyleOption& other);
    RibbonGroupStyleOption& operator=(const RibbonGroupStyleOption&) = default;
public:
    int widthOptButton;
    int heightCaption;
    QString text;
    QIcon icon;
    QSize iconSize;
    QStyle::SubControls subControls;
    QStyleOptionToolButton::ToolButtonFeatures features;
    Qt::Alignment textAlignment;
    Qt::TextElideMode textElideMode;
    Qtitan::ContextColor pageColor;
};

class QTITAN_BASE_EXPORT RibbonFrameMenuStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    RibbonFrameMenuStyleOption();
    RibbonFrameMenuStyleOption(const RibbonFrameMenuStyleOption& other);
    RibbonFrameMenuStyleOption& operator=(const RibbonFrameMenuStyleOption&) = default;
public:
    bool isAdditionalPages;
    int actionsWidth;
    int actionsHeight;
    QMargins margins;
};

class QTITAN_BASE_EXPORT RibbonGalleryItemStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    RibbonGalleryItemStyleOption();
    RibbonGalleryItemStyleOption(const RibbonGalleryItemStyleOption& other);
    RibbonGalleryItemStyleOption& operator=(const RibbonGalleryItemStyleOption&) = default;
public:
    bool separator;
    QRect rectItem;
    QString caption;
    QIcon icon;
};

class QTITAN_BASE_EXPORT StyleOptionMenuScrollItem : public QStyleOptionMenuItem
{
public:
    enum StyleOptionVersion { Version = QStyleOptionMenuItem::Version + QTN_OPT_VERSION };
    StyleOptionMenuScrollItem();
    StyleOptionMenuScrollItem(const StyleOptionMenuScrollItem& other);
    StyleOptionMenuScrollItem& operator=(const StyleOptionMenuScrollItem&) = default;
public:
    QString toolTip;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    int tabWidth;
#endif
};

class QTITAN_BASE_EXPORT HintReturnSysFrameMenuStyleOption : public QStyleHintReturn
{
public:
    HintReturnSysFrameMenuStyleOption(const QMargins& mrgs);
public:
    QMargins margins;
};

class QTITAN_BASE_EXPORT AutoHideWindowStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    AutoHideWindowStyleOption();
    AutoHideWindowStyleOption(const AutoHideWindowStyleOption& other);
    AutoHideWindowStyleOption& operator=(const AutoHideWindowStyleOption&) = default;
public:
    Qtitan::LayoutArea position;
};

class QTITAN_BASE_EXPORT DockTitleBarStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    DockTitleBarStyleOption();
    DockTitleBarStyleOption(const DockTitleBarStyleOption& other);
    DockTitleBarStyleOption& operator=(const DockTitleBarStyleOption&) = default;
public:
    QIcon icon;
    QString title;
    QRect rcText;
    QRect rcIcon;
    QRect lineRect;
    bool closable;
    bool movable;
    bool floatable;
    bool autohide;
    bool menu;
    bool verticalTitleBar;
};

class QTITAN_BASE_EXPORT DockMarkerStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    DockMarkerStyleOption();
    DockMarkerStyleOption(const DockMarkerStyleOption& other);
    DockMarkerStyleOption& operator=(const DockMarkerStyleOption&) = default;
public:
    QStyle::StandardPixmap sp;
    QPointF position;
    bool basis;
};

class QTITAN_BASE_EXPORT DockPhantomStyleOption : public QStyleOption
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    DockPhantomStyleOption();
    DockPhantomStyleOption(const DockPhantomStyleOption& other);
    DockPhantomStyleOption& operator=(const DockPhantomStyleOption&) = default;
public:
    bool drawFrame;
    Qtitan::LayoutArea posTab;
    int borderWidth;
    QRect rectTab;
};

class QTITAN_BASE_EXPORT ToggleSwitchStyleOption : public QStyleOptionComplex
{
public:
    enum StyleOptionVersion { Version = QStyleOption::Version + QTN_OPT_VERSION };
    ToggleSwitchStyleOption();
    ToggleSwitchStyleOption(const ToggleSwitchStyleOption& other);
    ToggleSwitchStyleOption& operator=(const ToggleSwitchStyleOption&) = default;
public:
    bool isAnimated;
    bool dontDrawThumbOn;
    bool dontDrawThumbOff;
    QSize sizeIndicator;
    Qt::Orientation orientation;
    Qt::Alignment alignment;
    CommonStyle::SubControlsEx subControls;
    ToggleSwitch::Shape shape;
    qreal animationOffset;
    qreal disabledOpacity;
    QString statusText;
    QString text;
};

class QTITAN_BASE_EXPORT StyleOptionTab 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    : public QStyleOptionTab
#else
    : public QStyleOptionTabV3
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
{
public:
    enum StyleOptionVersion { Version = QStyleOptionTab::Version + QTN_OPT_VERSION };
    StyleOptionTab();
public:
    QPixmap previewPixmap;
    QRect previewRect;
    int tabIndex;
    bool hoverClose;
protected:
    StyleOptionTab(int version);
};

class QTITAN_BASE_EXPORT RibbonOptionHeaderStyleOption : public StyleOptionTab
{
public:
    enum StyleOptionVersion { Version = StyleOptionTab::Version + QTN_OPT_VERSION };
    RibbonOptionHeaderStyleOption();
    RibbonOptionHeaderStyleOption(const RibbonOptionHeaderStyleOption& other);
    RibbonOptionHeaderStyleOption& operator=(const RibbonOptionHeaderStyleOption&) = default;
public:
    bool firstTab;
    bool lastTab;
    bool minimized;
    QRect rcFirst;
    QRect rcLast;
    QString contextText;
    Qtitan::ContextColor pageColor;
    uint tabBarPosition;
    Qt::Alignment textAlignment;
};

#define QTN_HEADER_OPTION_PROPERTY "qtn_header_style_option"
class QTITAN_BASE_EXPORT GridHeaderStyleOption : public QStyleOptionHeader
{
public:
    enum StyleOptionVersion { Version = QStyleOptionHeader::Version + QTN_OPT_VERSION };
    GridHeaderStyleOption();
    GridHeaderStyleOption(const QStyleOptionHeader& option);
    GridHeaderStyleOption(const GridHeaderStyleOption& other);
    GridHeaderStyleOption& operator=(const GridHeaderStyleOption&) = default;
public:
    QMargins margins;
    Qt::TextElideMode textElideMode;
    double scaleFactor;
    QColor decorationColor;
    QTextDocument* textDocument;
};

QTITAN_END_NAMESPACE

#endif // QTN_STYLEOPTION_H
