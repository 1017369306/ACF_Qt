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
#include <QApplication>

#include "QtnDockBarBasePrivate.h"
#include "QtnDockBarManagerPrivate.h"
#include "QtnDockBarManager.h"
#include <qevent.h>
#include <QLayout>

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/* DockBarBase */
DockBarBasePrivate::DockBarBasePrivate()
    : m_orientation(Qt::Horizontal)
    , m_dockBarArea(Qtitan::DockBarTop)
    , m_dockSite(Q_NULL)
    , m_explicitCaption(false)
    , m_barManager(Q_NULL)
{
}

DockBarBasePrivate::~DockBarBasePrivate()
{
}

DockBarBasePrivate* DockBarBasePrivate::_get(DockBarBase* controlBar) { return &controlBar->qtn_d(); }
const DockBarBasePrivate* DockBarBasePrivate::_get(const DockBarBase* controlBar) { return &controlBar->qtn_d(); }

Qt::Orientation DockBarBasePrivate::orientation() const
{
    return m_orientation;
}

void DockBarBasePrivate::setOrientation(Qt::Orientation orientation)
{
    QTN_P(DockBarBase);
    if (m_orientation == orientation)
        return;

    m_orientation = orientation;

    if (m_orientation == Qt::Vertical)
        p.setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    else
        p.setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    if (QLayout* layout = p.layout())
    {
        layout->invalidate();
        layout->activate();
    }

    if (QWidget* widget = p.parentWidget())
    {
        if (widget->isWindow())
        {
            QRect oldRect = widget->geometry();
            widget->adjustSize();

            QRect newRect = widget->geometry();
            QPoint pnt = oldRect.topLeft();

            if (m_orientation == Qt::Horizontal)
            {
                pnt.setX(pnt.x() - newRect.width() / 2);
                pnt.setY(pnt.y() + oldRect.height() / 2);
                newRect.moveTopLeft(pnt);
            }
            else if (m_orientation == Qt::Vertical)
            {
                pnt.setX(pnt.x() + oldRect.width() / 2);
                pnt.setY(pnt.y() - newRect.height() / 2);
                newRect.moveTopLeft(pnt);
            }
            widget->setGeometry(newRect);
        }
    }

    emit orientationChanged(m_orientation);
}

DockBarManager* DockBarBasePrivate::barManager() const
{ return m_barManager; }

void DockBarBasePrivate::setBarManager(DockBarManager* manager)
{ m_barManager = manager; }


/* DockBarBase */
DockBarBase::DockBarBase(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    QTN_INIT_PRIVATE(DockBarBase)
}

DockBarBase::DockBarBase(DockBarBasePrivate& dd, QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    qtn_d_ptr = &dd;
    qtn_d_ptr->setPublic(this);
}

DockBarBase::~DockBarBase()
{
    QTN_D(DockBarBase);
    if (d.m_barManager != Q_NULL)
        DockBarManagerPrivate::_get(d.m_barManager)->removeDockBar(this, false/*destroy*/);
    QTN_FINI_PRIVATE();
}

QString DockBarBase::id() const
{
    return objectName();
}

void DockBarBase::setId(const QString& id)
{
    setObjectName(id);
}

Qt::Orientation DockBarBase::orientation() const
{
    QTN_D(const DockBarBase);
    return d.orientation();
}

void DockBarBase::setOrientation(Qt::Orientation orientation)
{
    QTN_D(DockBarBase);
    d.setOrientation(orientation);
}

/*!
Returns current position for the dock bar.
*/
Qtitan::DockBarArea DockBarBase::dockBarArea() const
{
    QTN_D(const DockBarBase);
    return d.m_dockBarArea;
}

bool DockBarBase::setDockBarArea(Qtitan::DockBarArea dockBarArea)
{
    QTN_D(DockBarBase);
    if (d.m_barManager == Q_NULL)
        return false;
    if (isDockingPosition(dockBarArea))
    {
        if (!DockBarManagerPrivate::_get(d.m_barManager)->addDockBarToDockArea(this, dockBarArea))
            return false;
    }
    else if (dockBarArea == Qtitan::DockBarFloat)
    {
        if (!DockBarManagerPrivate::_get(d.m_barManager)->floatControlBar(this))
            return false;
    }
    else
        return false;
    Q_ASSERT(d.m_dockBarArea == dockBarArea);
    return true;
}

/*!
\property DockToolBar::caption
The property contains the caption for the toolbar. When floating, the caption is displayed on the title bar of the window.
By default it is a title of the main window. The caption can be initialized empty string.
*/
QString DockBarBase::caption() const
{
    QTN_D(const DockBarBase);
    if (d.m_explicitCaption)
        return d.m_caption;
    QWidget* managedWidget = d.barManager()->managedWidget();
    if (managedWidget == Q_NULL)
        return QString();
    QString text = managedWidget->window()->windowTitle();
    if (text.isEmpty())
        text = qApp->applicationName();
    return text;
}

void DockBarBase::setCaption(const QString& caption)
{
    QTN_D(DockBarBase);
    d.m_explicitCaption = true;
    d.m_caption = caption;
}



