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
#ifndef QTN_WINDOWS7SCENICSTYLE_H
#define QTN_WINDOWS7SCENICSTYLE_H

#include "QtnOfficeStyle.h"
#include "QtitanDef.h"  

QTITAN_BEGIN_NAMESPACE

class Windows7ScenicStylePrivate;
/* CommonStyle */
class QTITAN_EXPORT Windows7ScenicStyle : public OfficeStyle
{
    Q_OBJECT
public:
    Windows7ScenicStyle();
    virtual ~Windows7ScenicStyle();
public:
    int styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const override;
private:
    QTN_DECLARE_EX_PRIVATE(Windows7ScenicStyle)
    Q_DISABLE_COPY(Windows7ScenicStyle)
};

QTITAN_END_NAMESPACE

#endif // QTN_WINDOWS7SCENICSTYLE_H
