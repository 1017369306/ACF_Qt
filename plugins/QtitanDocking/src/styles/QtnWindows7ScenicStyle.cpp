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
#include <QPixmapCache>
#include <QStringBuilder>
#include <QDockWidget>
#include <QMdiArea>
#include <QPainter>
#include <qdrawutil.h>
#include <QDialog>
#include <QMenu>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif /* Q_OS_WIN*/
#include "QtnStyleHelperPrivate.h"
#include "QtnWindows7ScenicStylePrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/* Windows7ScenicStylePrivate */
Windows7ScenicStylePrivate::Windows7ScenicStylePrivate()
    : Office2007StylePrivate()
{
}

Windows7ScenicStylePrivate::~Windows7ScenicStylePrivate()
{
}

/*! \internal */
void Windows7ScenicStylePrivate::init()
{
    Office2007StylePrivate::init();
}

/*! \internal */
void Windows7ScenicStylePrivate::updateTheme()
{
    QTN_P(Windows7ScenicStyle)
    p.setStyleConfig(QStringLiteral(":/res/Windows7Scenic/"), QStringLiteral(_qtn_SingleTheme));
}


/*!
\class Windows7ScenicStyle
\inmodule QtitanStyle
\brief Windows7ScenicStyle class implements Windows 7 Scenic style.
*/
/*!
Constuctor of Windows7ScenicStyle class.
*/
Windows7ScenicStyle::Windows7ScenicStyle()
    : OfficeStyle(*new Windows7ScenicStylePrivate)
{
    QTN_D(Windows7ScenicStyle)
    d.updateTheme();
}

/*!
Destructor of Windows7ScenicStyle class.
*/
Windows7ScenicStyle::~Windows7ScenicStyle()
{
}

/*! \reimp */
int Windows7ScenicStyle::styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* shret) const
{
    switch (hint)
    {
    case SH_ToolButtonStyle:
    {
        if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemButton")))
            return Qt::ToolButtonIconOnly;
    }
    break;
    default:
        break;
    }

    return OfficeStyle::styleHint(hint, opt, widget, shret);
}
