/****************************************************************************
**
** Qtitan Library by Developer Machines (Fluent Design Style implementation for Qt.C++)
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

#ifndef QTN_FLUENTDESIGNSTYLEPRIVATE_H
#define QTN_FLUENTDESIGNSTYLEPRIVATE_H

#include "QtnFluentDesignStyle.h"
#include "QtnCommonStylePrivate.h"

QTITAN_BEGIN_NAMESPACE

class ColorizedStylePrivate : public CommonStylePrivate
{
public:
    QTN_DECLARE_EX_PUBLIC(ColorizedStyle)
public:
    ColorizedStylePrivate();
    virtual ~ColorizedStylePrivate();
    void init() override;
    void updateTheme();
public:
    bool isQtitanStyle()const override { return false; }
    void initStandardPalette(QPalette& palette) const override;
    QPalette getWidgetPalette(QWidget* widget) const override;
private:
    ColorizedStyle::Theme m_theme;
    QColor m_windowColor;
    QColor m_windowTextColor;
    QColor m_materialColor;
    QColor m_tileMenuColor;
    QColor m_menuColor;
    QColor m_linkColor;
    QColor m_itemColor;
    QColor m_itemTextColor;
    QColor m_captionColor;
};

QTITAN_END_NAMESPACE

#endif // QTN_FLUENTDESIGNSTYLEPRIVATE_H

