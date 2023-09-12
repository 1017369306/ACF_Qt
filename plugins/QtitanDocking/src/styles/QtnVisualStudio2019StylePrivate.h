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

#ifndef QTN_VISUALSTUDIO2019STYLEPRIVATE_H
#define QTN_VISUALSTUDIO2019STYLEPRIVATE_H

#include "QtnCommonStylePrivate.h"
#include "QtnVisualStudio2019Style.h"

QTITAN_BEGIN_NAMESPACE

class VisualStudio2019StylePrivate : public CommonStylePrivate
{
    Q_OBJECT
public:
    QTN_DECLARE_EX_PUBLIC(VisualStudio2019Style)
public:
    explicit VisualStudio2019StylePrivate();
    virtual ~VisualStudio2019StylePrivate();
    static VisualStudio2019StylePrivate* _get(VisualStudio2019Style* style);
    static const VisualStudio2019StylePrivate* _get(const VisualStudio2019Style* style);
public:
    bool isQtitanStyle() const override { return true; }
    QFont getWidgetFont(const QString& className) const override;
    // draw PrimitiveElement
    virtual bool drawHighDpiAreaDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget = Q_NULL) const;
    virtual bool drawHighDpiWindowClientDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget = Q_NULL) const;

    // draw PrimitiveElement
    virtual bool drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawIndicatorTabClose(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawIndicatorCheckBox(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawIndicatorRadioButton(const QStyleOption* opt, QPainter* p, const QWidget* w = Q_NULL) const;
    virtual bool drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    virtual bool drawIndicatorItemCheck(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* widget = Q_NULL) const;
    virtual bool drawFrameDockWidget(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawFrameTabBarBase(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawFrameGroupBox(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawFrameMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelMenu(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // PE - ButtonTool
    virtual bool drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelStatusBar(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawFrameLineEdit(const QStyleOption* opt, QPainter* p, const QWidget* w = Q_NULL) const;
    virtual bool drawPanelButtonCommand(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // draw ComplexControl
    virtual bool drawTitleBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawScrollBar(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawComboBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawToolButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawSpinBox(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // draw ControlElement
    virtual bool drawRibbonTabShapeLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawMenuBarItem(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawMenuItem(const QStyleOption* opt, QPainter* p, const QWidget* w = Q_NULL) const;
    virtual bool drawSplitter(const QStyleOption* opt, QPainter* p, const QWidget* widget = Q_NULL) const;
    virtual bool drawToolBar(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawMenuBarEmptyArea(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawShapedFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawTabBarTabLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawScrollBarAddSubLine(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawScrollBarAddSubPage(QStyle::ControlElement ce, const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawScrollBarSlider(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // CE - draw components ScrollBar
    virtual bool drawDockWidgetTitle(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawTabBarTabShape(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // for Dock
    // standardPixmap
    virtual QPixmap dockStandardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const;
    // subControlRect
    virtual QRect dockSubElementRect(QStyle::SubElement se, const QStyleOption* option, const QWidget* widget = Q_NULL) const;
public:
    void updateTheme();
protected:
    virtual QColor titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* wd = Q_NULL) const;
    static QWidget* qtn_findRibbonBar(const QWidget* parent);
    static bool qtn_isBackstageVisible(const QWidget* widget);
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
public:
    VisualStudio2019Style::Theme m_themeType;
private:
    Q_DISABLE_COPY(VisualStudio2019StylePrivate)
};

inline VisualStudio2019StylePrivate* VisualStudio2019StylePrivate::_get(VisualStudio2019Style* style) { return &style->qtn_d(); }
inline const VisualStudio2019StylePrivate* VisualStudio2019StylePrivate::_get(const VisualStudio2019Style* style) { return &style->qtn_d(); }

QTITAN_END_NAMESPACE

#endif // QTN_VISUALSTUDIO2019STYLEPRIVATE_H
