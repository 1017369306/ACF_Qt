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
#ifndef QTN_OFFICE2010STYLEPRIVATE_H
#define QTN_OFFICE2010STYLEPRIVATE_H

#include "QtnCommonStylePrivate.h"
#include "QtnOfficeStylePrivate.h"
#include "QtnOffice2007StylePrivate.h"
#include "QtnOffice2010Style.h"

QTITAN_BEGIN_NAMESPACE

/* Office2010StylePrivate */
class Office2010StylePrivate : public Office2007StylePrivate
{
public:
    QTN_DECLARE_EX_PUBLIC(Office2007Style)
public:
    explicit Office2010StylePrivate();
    virtual ~Office2010StylePrivate();
public:
    void init() override;
public:
    ///////////////////////////////////////////////////////////////////////////////
    // for RibbonBar
    // draw PrimitiveElement
    virtual bool drawRibbonBar(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawRibbonFrameGroups(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* w) const;
    // draw ControlElement
    virtual bool drawShapedFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    virtual bool drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
    // draw ComplexControl
    virtual bool drawRibbonTitleBarWidget(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w = Q_NULL) const;
public:
    virtual void updateTheme();
public:
    uint m_themeType;
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICE2010STYLEPRIVATE_H
