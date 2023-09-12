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

#include "QtnDockToolBarExtensionPrivate.h"
#include "QtnDockBarBasePrivate.h"
#include <qpixmap.h>
#include <qstyle.h>
#include <qstylepainter.h>
#include <qstyleoption.h>

#include "QtnDockToolBar.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/* DockToolBarSeparator */
void DockToolBarSeparator::initStyleOption(QStyleOption* option) const
{
    option->initFrom(this);
    if (orientation() == Qt::Horizontal)
        option->state |= QStyle::State_Horizontal;
}

DockToolBarSeparator::DockToolBarSeparator(DockToolBar* parent)
    : QWidget(parent)
    , orient(DockBarBasePrivate::_get(parent)->orientation())
{ 
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); 
}

void DockToolBarSeparator::setOrientation(Qt::Orientation orientation)
{
    orient = orientation;
    update();
}

Qt::Orientation DockToolBarSeparator::orientation() const
{ 
    return orient; 
}

QSize DockToolBarSeparator::sizeHint() const
{
    QStyleOption opt;
    initStyleOption(&opt);
    const int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentWidget());
    return QSize(extent, extent);
}

void DockToolBarSeparator::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOption opt;
    initStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentWidget());
}

/* DockToolBarExtension */
DockToolBarExtension::DockToolBarExtension(QWidget *parent)
    : QToolButton(parent)
{
    setAutoRaise(true);
    setOrientation(Qt::Horizontal);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setCheckable(true);
}

void DockToolBarExtension::setOrientation(Qt::Orientation o)
{
    QStyleOption opt;
    opt.initFrom(this);
    if (o == Qt::Horizontal)
        setIcon(style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton, &opt));
    else
        setIcon(style()->standardIcon(QStyle::SP_ToolBarVerticalExtensionButton, &opt));
}

void DockToolBarExtension::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    // We do not need to draw both extension arrows
    opt.features &= ~QStyleOptionToolButton::HasMenu;
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

QSize DockToolBarExtension::sizeHint() const
{
    int ext = style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent);
    return QSize(ext, ext);
}


