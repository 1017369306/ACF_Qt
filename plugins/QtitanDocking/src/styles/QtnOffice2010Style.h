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
#ifndef QTN_OFFICE2010STYLE_H
#define QTN_OFFICE2010STYLE_H

#include "QtnOfficeStyle.h"
#include "QtitanDef.h"  

QTITAN_BEGIN_NAMESPACE

class Office2010StylePrivate;
/* CommonStyle */
class QTITAN_EXPORT Office2010Style : public OfficeStyle
{
    Q_OBJECT
    Q_ENUMS(Theme)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme)
public:
    enum Theme
    {
        Black,
        Blue,
        Silver,
    };
public:
    Office2010Style();
    virtual ~Office2010Style();
public:
    Office2010Style::Theme theme() const;
    void setTheme(Office2010Style::Theme theme);
private:
    QTN_DECLARE_EX_PRIVATE(Office2010Style)
    Q_DISABLE_COPY(Office2010Style)
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICE2007STYLE_H
