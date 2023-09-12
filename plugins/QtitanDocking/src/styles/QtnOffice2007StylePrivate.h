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
#ifndef QTN_OFFICE2007STYLEPRIVATE_H
#define QTN_OFFICE2007STYLEPRIVATE_H

#include "QtnCommonStylePrivate.h"
#include "QtnOfficeStylePrivate.h"
#include "QtnOffice2007Style.h"

QTITAN_BEGIN_NAMESPACE

/* Office2007StylePrivate */
class Office2007StylePrivate : public OfficeStylePrivate
{
public:
    QTN_DECLARE_EX_PUBLIC(Office2007Style)
public:
    enum BarType
    {
        TypeMenuBar,
        TypeNormal,
        TypePopup,
        TypeRibbon,
        TypeSplitButtonPopup,
    };

    enum BarPosition
    {
        BarTop,
        BarBottom,
        BarLeft,
        BarRight,
        BarFloating,
        BarPopup,
        BarListBox,
        BarNone
    };
    explicit Office2007StylePrivate();
    virtual ~Office2007StylePrivate();
public:
    void init() override;
public:
    // for standard controls
    // draw PrimitiveElement
    virtual bool drawPanelMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawIndicatorCheckBox(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawIndicatorRadioButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawIndicatorCheckRadioButton(QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    virtual bool drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawFrameLineEdit(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawPanelLineEdit(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawPanelStatusBar(const QStyleOption*, QPainter*, const QWidget*) const;
    // PE - ButtonTool
    virtual bool drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawGroupControlEntry(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelButtonCommand(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // draw ControlElement
    virtual bool drawMenuBarItem(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual void drawMenuItemSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawToolBoxTabShape(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawDockWidgetTitle(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    ///////////////////////////////////////////////////////////////////////////////
    // for RibbonBar
    // draw PrimitiveElement
    virtual bool drawRibbonFrameGroups(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonFrameGroup(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonPanelButton(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonOptionButton(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawWorkspace(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonSliderButton(const QStyleOption* opt, QPainter* p, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonSysFrameMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonSysMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonSysFramePagePopupList(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonFrameGallery(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonBackstageFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonBackstagePanelButton(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonGroupScrollButton(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonPopupBarButton(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* widget) const;
    // draw ControlElement
    virtual bool drawToolBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawRibbonTabShapeLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawSlider(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawRibbonGroupLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonReducedGroup(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonGalleryItem(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonBackstageMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawOfficePopupSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // CE - draw components ScrollBar
    virtual bool drawScrollBarAddSubLine(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawScrollBarAddSubPage(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawScrollBarSlider(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawTabBarTabShape(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawHeaderSection(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    // draw ComplexControl
    virtual bool drawRibbonFileButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget = Q_NULL) const;
    virtual bool drawRibbonTitleBarWidget(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonGalleryButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawToolButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawComboBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawSpinBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawScrollBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // pixelMetric
    virtual int ribbonStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const;
    // pixelMetric
    virtual int ribbonPixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget = Q_NULL) const;
protected:
    void drawImagePart(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, bool alpha) const;
    void drawImage(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, QRect rcSizingMargins, QColor clrTransparent, bool alphaBlend = false) const;
    void drawImage(const QPixmap& dcSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;
    void drawPixmap(const QPixmap& soSrc, QPainter& p, const QRect& rcDest, const QRect& rcSrc, bool alpha, QRect rcSizingMargins, bool alphaBlend = false) const;
    void drawDropDownGlyph(QPainter* p, QPoint pt, bool selected, bool popuped, bool enabled, bool vert, const QWidget* w) const;
    QPixmap loadPixmap(const QString& className, const QString& image) const;
    virtual void drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, 
        bool checked, bool popuped, BarType barType, BarPosition barPos) const;
    virtual void drawSplitButtonPopup(QPainter* p, const QRect& rect, bool selected, bool enabled, bool popuped) const;
public:
    virtual void updateTheme();
public:
    uint m_themeType;
    uint m_fileButtonImageCount;
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICE2007STYLEPRIVATE_H
