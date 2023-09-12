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
#include "QtnCommonStyle.h"
#include "QtnCommonStylePrivate.h"
#include "QtnPlatform_win.h"

#include <qt_windows.h>
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif


QTITAN_USE_NAMESPACE

int CommonStylePrivate::pixelMetricFromSystemDp(QStyle::PixelMetric pm, const QStyleOption* option, const QWidget* widget)
{
    Q_UNUSED(option);
    switch (pm) 
    {
        case QStyle::PM_TitleBarHeight:
        {
            QMargins frame = qtn_fromNativeDPI(qtn_getWindowFrameMargings(widget), widget);
            return widget && (widget->windowType() == Qt::Tool)
                ? GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CXSIZEFRAME)
                : frame.top();
        }
        default:
            break;
    }
    return CommonStylePrivate::InvalidMetric;
}


