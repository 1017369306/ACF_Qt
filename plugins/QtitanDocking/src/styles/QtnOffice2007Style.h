/****************************************************************************
**
** Qtitan Library by Developer Machines Office Style implementation for Qt.C++)
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
#ifndef QTN_OFFICE2007STYLE_H
#define QTN_OFFICE2007STYLE_H

#include "QtnOfficeStyle.h"
#include "QtitanDef.h"  

QTITAN_BEGIN_NAMESPACE

class Office2007StylePrivate;

class QTITAN_EXPORT Office2007Style : public OfficeStyle
{
    Q_OBJECT
    Q_ENUMS(Theme)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme)
public:
    enum Theme
    {
        Blue,
        Black,
        Silver,
        Aqua,
    };
public:
    Office2007Style();
    virtual ~Office2007Style();
public:
    Office2007Style::Theme theme() const;
    void setTheme(Office2007Style::Theme theme);
public:
    QSize sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget = QWIDGET_NULL) const override;
    int styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const override;
private:
    QTN_DECLARE_EX_PRIVATE(Office2007Style)
    Q_DISABLE_COPY(Office2007Style)
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICE2007STYLE_H
