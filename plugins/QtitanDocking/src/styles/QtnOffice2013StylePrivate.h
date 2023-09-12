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
#ifndef QTN_OFFICE2013STYLEPRIVATE_H
#define QTN_OFFICE2013STYLEPRIVATE_H

#include "QtnCommonStylePrivate.h"
#include "QtnOfficeStylePrivate.h"
#include "QtnOffice2013Style.h"

QTITAN_BEGIN_NAMESPACE

class Office2013StylePrivate : public OfficeStylePrivate
{
    Q_OBJECT
public:
    QTN_DECLARE_EX_PUBLIC(Office2013Style)
public:
    explicit Office2013StylePrivate();
    ~Office2013StylePrivate() override;
public:
    void init() override;
    QColor accentIndexToColor(OfficeStyle::AccentColor accentcolor) const;
    void initStandardPalette(QPalette& palette) const override;
public:
    // for standard controls
    // draw PrimitiveElement
    bool drawFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawFrameWindow(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawFrameMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawPanelMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawPanelStatusBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawFrameGroupBox(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawIndicatorCheckBox(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawIndicatorRadioButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawIndicatorButtonDropDown(const QStyleOption*, QPainter*, const QWidget*) const override;
    bool drawPanelButtonBevel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawFrameTabBarBase(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawFrameFocusRect(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    // PE - ButtonTool
    bool drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawFrameLineEdit(const QStyleOption* opt, QPainter* p, const QWidget* w) const override;
    bool drawPanelButtonCommand(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    // draw ComplexControl
    bool drawRibbonFileButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget = Q_NULL) const override;
    bool drawRibbonTitleBarWidget(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawTitleBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const override;
    bool drawScrollBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawComboBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawSlider(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const override;
    bool drawSpinBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const override;
    // draw ControlElement
    bool drawRibbonTabShapeLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawRibbonGalleryItem(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    void drawMenuItemSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawShapedFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawToolBar(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawScrollBarAddSubLine(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawScrollBarAddSubPage(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawScrollBarSlider(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawToolBoxTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const override;
    bool drawTabBarTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const override;
    bool drawHeaderSection(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    bool drawDockWidgetTitle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const override;
    // for RibbonBar
    // draw PrimitiveElement
    bool drawRibbonBar(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawRibbonFrameGroup(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawRibbonFrameGallery(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawRibbonBackstageFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawRibbonBackstageCloseButton(const QStyleOption* opt, QPainter* p, const QWidget* widget) const override;
    bool drawRibbonSliderButton(const QStyleOption* opt, QPainter* p, const QWidget* w = Q_NULL) const override;
    bool drawPanelTipLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawKeyTip(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    int ribbonStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const override;
protected:
    void drawHighlightItemGallery(const QStyleOption* option, QPainter* painter, const QWidget* widget) const override;
    QColor textGalleryItem(const QPalette& pal) const override;
    QColor darkBackgroundLight(const QPalette& pal) const override;
    QColor buttonHighlightColor(const QPalette& pal, bool highlightDn, bool border = false, const QWidget* wd = Q_NULL) const override;
    QColor titleBarWindowColor(const QPalette& pal, bool isActive, const QWidget* wd) const override;
    QColor titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* wd) const override;
protected:
    virtual void updateTheme();
    virtual QColor fillTitleBarColor(const QPalette& /*pal*/, bool isBackstageVisible) const ;
    virtual QColor textRibbonTabBarColor(const QStyleOption* /*opt*/) const { return m_clrAccent; }
    virtual QColor buttonRibbonFileColor(const SystemToolButtonStyleOption* /*opt*/) const;
    virtual QColor fillBackstageFrameColor(const QPalette& /*pal*/) const;
    virtual QBrush brushFromConfig(const QString& className, const QString& subControlId, QPalette::ColorGroup group,
        QPalette::ColorRole role, QStyle::State state, bool isBackstageVisible) const;

    static QWidget* qtn_findRibbonBar(const QWidget* parent);
    static bool qtn_isBackstageVisible(const QWidget* widget);
public:
    QPalette getWidgetPalette(QWidget* widget) const override;
public:
    uint m_themeType;
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICE2013STYLEPRIVATE_H
