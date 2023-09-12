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
#ifndef QTN_OFFICESTYLEPRIVATE_H
#define QTN_OFFICESTYLEPRIVATE_H

#include "QtnCommonStylePrivate.h"
#include "QtnStyleOption.h"
#include "QtnOfficeStyle.h"

QTITAN_BEGIN_NAMESPACE

class OfficeStylePrivate : public CommonStylePrivate
{
public:
    QTN_DECLARE_EX_PUBLIC(OfficeStyle)
public:
    explicit OfficeStylePrivate();
    ~OfficeStylePrivate() override;
public:
    void init() override;
    QColor accentIndexToColor(OfficeStyle::AccentColor accentcolor) const;
public:
    QFont getWidgetFont(const QString& className) const override;
    bool isQtitanStyle() const override { return true; }
    bool drawTabBarTabLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawPanelTipLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    // for RibbonBar
    // draw PrimitiveElement
    bool drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawIndicatorItemCheck(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    bool drawIndicatorBranch(const QStyleOption* opt, QPainter* p, const QWidget* w = Q_NULL) const override;
    bool drawPanelItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* widget = Q_NULL) const override;
    bool drawKeyTip(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
    // draw ControlElement
    bool drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const override;
public:
    QColor m_clrAccent;
    QBrush m_saveBrushBackground;
    OfficeStyle::AccentColor m_curAccentColor;
    bool m_isActiveTabAccented;
    bool m_ignoreMDIWindowTitle;
#if 0
    QStyle* m_popupProxy;
    uint    m_refCountProxy;
    PopupOffice2003DrawHelper::OptionsStyle m_popupOptionsStyle;
#endif
    OfficeStyle::OfficePopupDecoration m_typePopupStyle;

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
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICESTYLEPRIVATE_H
