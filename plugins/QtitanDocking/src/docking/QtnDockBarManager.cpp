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

#include "QtnDockBarManagerPrivate.h"
#include "QtnDockToolBarPrivate.h"
#include "QtnDockBarSitePrivate.h"
#include "QtnDockBarBasePrivate.h"
#include "QtnDockMainWindowLayout.h"
#include "QtnCommonStyle.h"

#include <qevent.h>
#include <QDebug>
#include <QLayout>
#include <QFile>
#include <QApplication>
#include <QMainWindow>
#include <QXmlStreamWriter>
#include <QTimer>

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

#define POINT_OFFSET        20
#define BEGIN_WIDGET_BAR_ID 2000
#define END_WIDGET_BAR_ID   3000

int generationSimpleBarID()
{
    static int s_id = BEGIN_WIDGET_BAR_ID;
    s_id++;
    Q_ASSERT(s_id >= BEGIN_WIDGET_BAR_ID && s_id <= END_WIDGET_BAR_ID);
    return s_id;
}

#define QTN_DOCK_TAGNAME_BARNUMBER           QStringLiteral("%1")
#define QTN_DOCK_TAGNAME_DOCKBARLAYOUTS      QStringLiteral("DockBarLayouts")
#define QTN_DOCK_TAGNAME_DOCKAREABARS        QStringLiteral("DockBarSite")
#define QTN_DOCK_TAGNAME_AREABAR             QStringLiteral("DockAreaBar")
#define QTN_DOCK_TAGNAME_EMPTYBAR            QStringLiteral("0")
#define QTN_DOCK_TAGNAME_TOOLBAR             QStringLiteral("DockToolBar")
#define QTN_DOCK_TAGNAME_TOOLBARS            QStringLiteral("ToolBars")
#define QTN_DOCK_TAGNAME_STARTBAR            QStringLiteral("-1")
#define QTN_DOCK_TAGNAME_RECTDOCKPOS         QStringLiteral("RectDockPos")
#define QTN_DOCK_TAGNAME_RECTFLOATPOS        QStringLiteral("RectFloatPos")

#define QTN_DOCK_ATTRNAME_COUNT              QStringLiteral("count")
#define QTN_DOCK_ATTRNAME_IDBAR              QStringLiteral("id")
#define QTN_DOCK_ATTRNAME_POSITIONAREABAR    QStringLiteral("position")
#define QTN_DOCK_ATTRNAME_VISIBLECONTROLBAR  QStringLiteral("visible")
#define QTN_DOCK_ATTRVALUE_TRUE              QStringLiteral("True")
#define QTN_DOCK_ATTRVALUE_FALSE             QStringLiteral("False") 
#define QTN_DOCK_ATTRNAME_POSITION_X         QStringLiteral("posX")
#define QTN_DOCK_ATTRNAME_POSITION_Y         QStringLiteral("posY")
#define QTN_DOCK_ATTRNAME_LEFT               QStringLiteral("left")
#define QTN_DOCK_ATTRNAME_RIGHT              QStringLiteral("right")
#define QTN_DOCK_ATTRNAME_TOP                QStringLiteral("top")
#define QTN_DOCK_ATTRNAME_BOTTOM             QStringLiteral("bottom")
#define QTN_DOCK_ATTRNAME_POSITIONBAR        QStringLiteral("positionBar")
#define QTN_DOCK_ATTRNAME_ORIENTATIONBAR     QStringLiteral("orientationBar")

QTITAN_BEGIN_NAMESPACE
bool isDockingPosition(Qtitan::DockBarArea barPosition)
{
    return barPosition == Qtitan::DockBarLeft || barPosition == Qtitan::DockBarTop ||
        barPosition == Qtitan::DockBarRight || barPosition == Qtitan::DockBarBottom;
}
QTITAN_END_NAMESPACE

/* DockBarManagerPrivate */
DockBarManagerPrivate::DockBarManagerPrivate()
{
}

DockBarManagerPrivate::~DockBarManagerPrivate()
{
    for (QList<DockBarBase *>::const_iterator it = m_bars.constBegin(); it != m_bars.constEnd(); ++it)
    {
        DockBarBase* bar = *it;
        DockBarBasePrivate::_get(bar)->setBarManager(Q_NULL);
    }
}

DockBarManagerPrivate* DockBarManagerPrivate::_get(DockBarManager* manager)
{ return &manager->qtn_d(); }

const DockBarManagerPrivate* DockBarManagerPrivate::_get(const DockBarManager* manager)
{ return &manager->qtn_d(); }

void DockBarManagerPrivate::initialization()
{
    QTN_P(DockBarManager);
    CommonStyle::ensureStyle();
    if (QWidget* parentWidget = p.managedWidget())
    {
        if (Q_UNLIKELY(qobject_cast<QMainWindow*>(parentWidget)))
            qFatal("DockBarManager: Cannot create a DockBarManager with QMainWindow, use DockMainWindow instead.");

        QLayout* otherLayout = parentWidget->layout();
        DockMainWindowLayout* mainWindowLayout = qobject_cast<DockMainWindowLayout*>(otherLayout);
        if (mainWindowLayout == Q_NULL)
        {
            delete otherLayout;
            DockMainWindowLayout* layout = new DockMainWindowLayout(parentWidget);
            layout->setDockBarManager(&p);
        }
        else if (!mainWindowLayout->dockBarManager())
            mainWindowLayout->setDockBarManager(&p);
    }
    makeDockBarSite();
}

void DockBarManagerPrivate::makeDockBarSite()
{
    QTN_P(DockBarManager);
    Q_ASSERT(p.managedWidget() != Q_NULL);
    for (int i = Qtitan::LeftArea; i <= Qtitan::BottomArea; ++i)
    {
        Qtitan::LayoutArea pos = static_cast<Qtitan::LayoutArea>(i);
        DockBarSite* dockBar = new DockBarSite(pos, p.managedWidget());
        DockBarSitePrivate::_get(dockBar)->setDockBarManager(&p);
        dockBar->setGeometry(QRect(QPoint(0, 0), QSize(0, 0)));
        dockBar->setVisible(true);
        m_dockSideMap[pos] = dockBar;
    }
}

static void qtnCheckUniqueName(const QList<DockBarBase*>& list)
{
    QStringList temp;
    for (QList<DockBarBase*>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        if (DockBarBase* bar = qobject_cast<DockBarBase*>((*it)))
        {
            if (!bar->id().isEmpty())
            {
                if (!temp.contains(bar->id()))
                    temp.append(bar->id());
                else
                    qWarning() << "qtnCheckUniqueName() - DockBarBase object name is not unique. Name repeat is found.";
            }
            else
                qWarning() << "qtnCheckUniqueName() - DockBarBase object name is empty.";
        }
    }
}

void DockBarManagerPrivate::assignUniqueName() const
{
    for (QList<DockBarBase*>::const_iterator it = m_bars.constBegin(); it != m_bars.constEnd(); ++it)
    {
        DockBarBase* bar = *it;
        if (bar->id().isEmpty())
            bar->setId(QString(QTN_DOCK_TAGNAME_BARNUMBER).arg(generationSimpleBarID()));
    }
    qtnCheckUniqueName(m_bars);
}

bool DockBarManagerPrivate::addDockBarToSite(DockBarBase* bar, QRect* rect, DockBarSite* dockSite)
{
    Q_ASSERT(dockSite);
    if (dockSite == Q_NULL)
        return false;
    bar->setOrientation(dockSite->orientation());
    DockBarBasePrivate::_get(bar)->m_dockBarArea = dockSite->dockBarArea();
    dockSite->addDockBar(bar, rect);
    return true;
}

bool DockBarManagerPrivate::addDockBarToDockArea(DockBarBase* bar, Qtitan::DockBarArea dockArea)
{
    DockBarSite* dockSite = areaBars(dockArea);
    Q_ASSERT(dockSite != Q_NULL);
    return addDockBarToSite(bar, Q_NULL, dockSite);
}

bool DockBarManagerPrivate::floatControlBar(DockBarBase* bar)
{
    DockBarBasePrivate* ptrPrivate = DockBarBasePrivate::_get(bar);
    if (ptrPrivate->m_dockSite != Q_NULL)
    {
        ptrPrivate->m_dockSite->removeDockBar(bar);
        ptrPrivate->m_dockSite = Q_NULL;
    }
    ptrPrivate->m_dockBarArea = Qtitan::DockBarFloat;
    return true;
}

void DockBarManagerPrivate::removeDockBar(DockBarBase* bar, bool destroy)
{
    QTN_P(DockBarManager);
    Q_ASSERT(bar != Q_NULL);
    if (bar == Q_NULL)
        return;

    int i = m_bars.indexOf(bar);
    if (i == -1)
        return;

    bar->setVisible(false);
    DockBarBasePrivate::_get(bar)->setBarManager(Q_NULL);
    if (DockBarBasePrivate::_get(bar)->m_dockSite != Q_NULL)
    {
        DockBarBasePrivate::_get(bar)->m_dockSite->removeDockBar(bar);
        DockBarBasePrivate::_get(bar)->m_dockSite = Q_NULL;
        if (p.managedWidget()->layout())
        {
            p.managedWidget()->layout()->invalidate();
            p.managedWidget()->layout()->activate();
        }
    }
    m_bars.removeAt(i);
    if (destroy)
        delete bar;
}

void DockBarManagerPrivate::clearDockBars()
{
    for (int i = 0, count = m_bars.size(); i < count; i++)
    {
        DockBarBase* bar = m_bars.at(i);
        bar->hide();

        if (DockBarBasePrivate::_get(bar)->m_dockSite != Q_NULL)
        {
            DockBarBasePrivate::_get(bar)->m_dockSite->removeDockBar(bar);
            DockBarBasePrivate::_get(bar)->m_dockSite = Q_NULL;
        }
    }
    m_bars.clear();
}

int DockBarManagerPrivate::calcMaxHeightBars() const
{
    int maxHeight = 0;
    for (int i = 0, count = m_bars.size(); i < count; i++)
    {
        DockBarBase* bar = m_bars.at(i);
        Qtitan::DockBarArea dockBarArea = bar->dockBarArea();
        if (dockBarArea == Qtitan::DockBarTop || dockBarArea == Qtitan::DockBarBottom /* TODO || dockBarArea == Qtitan::DockBarFloat*/)
            maxHeight = qMax(maxHeight, bar->sizeHint().height());
    }
    return maxHeight;
}

DockBarBase* DockBarManagerPrivate::toolBarById(const QString& strId) const
{
    if (strId.isEmpty())
        return Q_NULL;

    for (QList<DockBarBase*>::ConstIterator it = m_bars.begin(); it != m_bars.end(); ++it)
    {
        DockBarBase* bar = *it;
        if (bar->id() == strId)
            return bar;
    }
    return Q_NULL;
}

DockBarSite* DockBarManagerPrivate::areaBars(Qtitan::DockBarArea position) const
{
    Q_ASSERT(position >= Qtitan::DockBarLeft && position < Qtitan::DockBarFloat);
    return position >= Qtitan::DockBarLeft && position < Qtitan::DockBarFloat ? (DockBarSite*)m_dockSideMap.value(static_cast<uint>(position)) : Q_NULL;
}

DockBarSite* DockBarManagerPrivate::canDock(const QPoint& point, DockBarSite* area) const
{
    QTN_P(const DockBarManager);
    QRect rc;
    if (area)
    {
        rc = area->geometry();
        QPoint pt = p.managedWidget()->mapFromGlobal(point);

        if (area->orientation() == Qt::Horizontal &&
            rc.top() - POINT_OFFSET < pt.y() && rc.bottom() + POINT_OFFSET > pt.y() &&
            rc.left() - POINT_OFFSET < pt.x() && rc.right() + POINT_OFFSET > pt.x())
            return area;

        if (area->orientation() == Qt::Vertical &&
            rc.left() - POINT_OFFSET < pt.x() && rc.right() + POINT_OFFSET > pt.x() &&
            rc.top() - POINT_OFFSET < pt.y() && rc.bottom() + POINT_OFFSET > pt.y())
            return area;
    }

    for (int i = Qtitan::LeftArea; i <= Qtitan::BottomArea; i++)
    {
        DockBarSite* currentBars = (DockBarSite*)m_dockSideMap.value(static_cast<Qtitan::LayoutArea>(i));

        rc = currentBars->geometry();
        QPoint pt = p.managedWidget()->mapFromGlobal(point);

        if (currentBars->orientation() == Qt::Horizontal &&
            rc.top() - POINT_OFFSET < pt.y() && rc.bottom() + POINT_OFFSET > pt.y() &&
            rc.left() - POINT_OFFSET < pt.x() && rc.right() + POINT_OFFSET > pt.x())
            return currentBars;

        if (currentBars->orientation() == Qt::Vertical &&
            rc.left() - POINT_OFFSET < pt.x() && rc.right() + POINT_OFFSET > pt.x() &&
            rc.top() - POINT_OFFSET < pt.y() && rc.bottom() + POINT_OFFSET > pt.y())
            return currentBars;
    }
    return Q_NULL;
}



/*!
\class DockBarManager
\inmodule QtitanDocking
\brief Class DockBarManager is used to add Microsoft-like dock bars to the certain QWidget.

The example below demonstrates the simplest use of the manager.
\code
QWidget* widget = new QWidget;
DockBarManager* manager = new DockBarManager(widget);
DockToolBar* tooBar = manager->addToolBar("My Toolbar", Qtitan::ToolBarTop);
tooBar->addAction("Button");
widget->show();
\endcode
DockBarManager can be used with DockPanelManager on the same QWidget together.
Normally, you do not need to create it manually. Use DockMainWindow to get the main window, which contains the DockBarManager and a set of functions for bars creation.
\sa DockPanelManager
*/
/*!
Constructs the class with the given \a widget on which the docking mechanism will be deployed.
*/
DockBarManager::DockBarManager(QWidget* widget)
    : QObject(widget)
{
    Q_ASSERT(widget != Q_NULL);
    QTN_INIT_PRIVATE(DockBarManager);
    QTN_D(DockBarManager);
    d.initialization();
    managedWidget()->installEventFilter(this);
}

DockBarManager::~DockBarManager()
{
    managedWidget()->removeEventFilter(this);
    QTN_FINI_PRIVATE();
}

/*!
Returns the managed widget.
*/
QWidget* DockBarManager::managedWidget() const
{
    Q_ASSERT(qobject_cast<QWidget*>(parent()) != Q_NULL);
    return qobject_cast<QWidget*>(parent());
}

/*!
Creates and returns dock tool bar with the given \a name and \a dockBarArea.
*/
DockToolBar* DockBarManager::addToolBar(const QString& name, Qtitan::DockBarArea dockBarArea)
{
    DockToolBar* toolBar = new DockToolBar(name);
    if (!insertDockBar(toolBar, dockBarArea))
        Q_DELETE_AND_NULL(toolBar);
    return toolBar;
}

/*!
Inserts \a toolBar at the \a dockBarArea. Retruns true if success, otherwise return false.
*/
bool DockBarManager::insertDockBar(DockToolBar* toolBar, Qtitan::DockBarArea dockBarArea)
{
    QTN_D(DockBarManager);
    Q_ASSERT(isDockingPosition(dockBarArea) || dockBarArea == Qtitan::DockBarFloat);

    QWidget* parent = managedWidget();
    Q_ASSERT(parent != Q_NULL);

    if (toolBar == Q_NULL || d.m_bars.contains(toolBar))
        return false;

    DockBarBasePrivate::_get(toolBar)->setBarManager(this);
    if (!toolBar->setDockBarArea(dockBarArea))
        return false;

    d.m_bars.append(toolBar);

    if (parent->layout())
    {
        parent->layout()->invalidate();
        parent->layout()->activate();
    }
    return true;
}

/*!
Inserts \a toolBar before bar \a before. It appends the \a toolBar if \a before is 0 or \a before is not found in the bar's list.
*/
void DockBarManager::insertDockBar(DockToolBar* before, DockToolBar* toolBar)
{
    QTN_D(DockBarManager);
    Q_ASSERT(isDockingPosition(before->dockBarArea()) || before->dockBarArea() == Qtitan::DockBarFloat);

    if (before == Q_NULL || toolBar == Q_NULL ||
        !d.m_bars.contains(before) || d.m_bars.contains(toolBar))
        return;

    DockBarBasePrivate::_get(toolBar)->setBarManager(this);
    toolBar->setDockBarArea(before->dockBarArea());

    d.m_bars.append(toolBar);

    if (managedWidget()->layout())
    {
        managedWidget()->layout()->invalidate();
        managedWidget()->layout()->activate();
    }
    moveDockBar(before, toolBar);
}

/*!
Moves \a toolBar before the dock bar \a before. If \a toolBar has not been added to the manager yet then use 
insertDockBar(DockToolBar* before, DockToolBar* toolBar) instead to add it.
*/
void DockBarManager::moveDockBar(DockToolBar* before, DockToolBar* toolBar)
{
    QTN_D(DockBarManager);

    if (toolBar == Q_NULL || before == Q_NULL ||
        !d.m_bars.contains(toolBar) || !d.m_bars.contains(before))
        return;

    toolBar->adjustSize();
    before->adjustSize();

    QRect rc = before->geometry();
    rc.moveTopLeft(before->parentWidget()->mapToGlobal(rc.topLeft()));

    if (DockBarBasePrivate::_get(before)->orientation() == Qt::Vertical)
        rc.translate(0, rc.height());
    else
        rc.translate(rc.width(), 0);

    d.addDockBarToSite(toolBar, &rc, DockToolBarPrivate::_get(before)->m_dockSite);
}

/*!
Removes \a toolBar from the dock bar manager and clear all internal structures regarding this bar.
Note that the function does not destroy the instance of \a toolBar. After calling this function, you must delete the \a toolBar manually
using the operator 'delete'.
*/
void DockBarManager::removeDockBar(DockToolBar* toolBar)
{
    QTN_D(DockBarManager);
    d.removeDockBar(toolBar, false/*destroy*/);
}

/*!
Removes all dock bars from the dock bar manager and destroy its instanceses.
Note: All bar's references become invalid after this call.
*/
void DockBarManager::clearDockBars()
{
    QTN_D(DockBarManager);
    d.clearDockBars();
    if (managedWidget()->layout())
        managedWidget()->layout()->invalidate();
}

/*!
Returns the central widget for the dock layout.
*/
QWidget* DockBarManager::centralWidget() const
{
    Q_ASSERT(managedWidget() != Q_NULL);
    if (CustomLayout* layout = qobject_cast<CustomLayout*>(managedWidget()->layout()))
        return layout->centralWidget();
    return Q_NULL;
}

/*!
Sets the central \a widget for the dock layout. Actualy it is query the layout from the managedWidget() and sets the central widget to it.
If you have DockPanelManager and DockBarManager on the same QWidget then DockPanelManager::setCentralWidget() and DockBarManager::setCentralWidget() do the identical thing.
*/
void DockBarManager::setCentralWidget(QWidget* widget)
{
    Q_ASSERT(managedWidget() != Q_NULL);
    if (CustomLayout* layout = qobject_cast<CustomLayout*>(managedWidget()->layout()))
        layout->setCentralWidget(widget);
}

/*!
Creates QList of all dock bar that implements DockToolBar.
\sa dockBars()
*/
QList<DockToolBar*> DockBarManager::toolBars() const
{
    QTN_D(const DockBarManager);
    QList<DockToolBar*> list;
    for (QList<DockBarBase *>::const_iterator it = d.m_bars.constBegin(); it != d.m_bars.constEnd(); ++it)
    {
        if (DockToolBar* toolBar = qobject_cast<DockToolBar*>((*it)))
            list.append(toolBar);
    }
    return list;
}

/*!
Returns QList of all dock bar that manager owns.
*/
const QList<DockBarBase*>& DockBarManager::dockBars() const
{
    QTN_D(const DockBarManager);
    return d.m_bars;
}

/*!
Returns count of dock bars.
\sa dockBars()
*/
int DockBarManager::dockBarCount() const
{
    QTN_D(const DockBarManager);
    return d.m_bars.size();
}

/*!
Returns dock bar by the \a index.
\sa dockBars()
*/
DockBarBase* DockBarManager::dockBarAt(int index) const
{
    QTN_D(const DockBarManager);
    if (index >= 0 && index < dockBarCount())
        return d.m_bars.at(index);
    return Q_NULL;
}

QRect DockBarManager::updateLayoutGeometry(const QRect& rect)
{
    QTN_D(DockBarManager);
    QRect r = rect, rectCenter = rect;
    DockBarSite* areaBars = d.areaBars(Qtitan::DockBarTop);
    if (areaBars != Q_NULL)
    {
        DockBarSitePrivate::_get(areaBars)->updateLayout(r);
        rectCenter.setTop(rectCenter.top() + areaBars->sizeHint().height());
    }
    areaBars = d.areaBars(Qtitan::DockBarBottom);
    if (areaBars != Q_NULL)
    {
        DockBarSitePrivate::_get(areaBars)->updateLayout(r);
        rectCenter.setBottom(rectCenter.bottom() - areaBars->sizeHint().height());
    }
    areaBars = d.areaBars(Qtitan::DockBarLeft);
    if (areaBars != Q_NULL)
    {
        DockBarSitePrivate::_get(areaBars)->updateLayout(rectCenter);
        rectCenter.setLeft(rectCenter.left() + areaBars->sizeHint().width());
    }
    areaBars = d.areaBars(Qtitan::DockBarRight);
    if (areaBars != Q_NULL)
    {
        DockBarSitePrivate::_get(areaBars)->updateLayout(rectCenter);
        rectCenter.setRight(rectCenter.right() - areaBars->sizeHint().width());
    }
    return rectCenter;
}

static QString positionAreaBar(int pos)
{
    switch (pos)
    {
        case Qtitan::LeftArea:
            return QStringLiteral("Left");
        case Qtitan::RightArea:
            return QStringLiteral("Right");
        case Qtitan::TopArea:
            return QStringLiteral("Top");
        case Qtitan::BottomArea:
            return QStringLiteral("Bottom");
        default:
            return QStringLiteral("Left");
    }
}

struct ToolBarInfo
{
    bool visible;
    Qtitan::DockBarArea area;
    Qt::Orientation orientation;
    QString id;
    QPoint pos;
    QRect windowRect;
    QRect rect;

    void saveToStream(QXmlStreamWriter& xmlwriter)
    {
        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_TOOLBAR); // BEGIN
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_IDBAR, id);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_VISIBLECONTROLBAR, visible ? QTN_DOCK_ATTRVALUE_TRUE : QTN_DOCK_ATTRVALUE_FALSE);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_POSITION_X, QString::number(pos.x()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_POSITION_Y, QString::number(pos.y()));

        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_POSITIONBAR, QString::number(area));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ORIENTATIONBAR, QString::number(orientation));

        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_RECTDOCKPOS); // BEGIN
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_LEFT, QString::number(rect.left()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_RIGHT, QString::number(rect.right()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_TOP, QString::number(rect.top()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_BOTTOM, QString::number(rect.bottom()));
        xmlwriter.writeEndElement();

        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_RECTFLOATPOS); // BEGIN
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_LEFT, QString::number(windowRect.left()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_RIGHT, QString::number(windowRect.right()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_TOP, QString::number(windowRect.top()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_BOTTOM, QString::number(windowRect.bottom()));
        xmlwriter.writeEndElement();

        xmlwriter.writeEndElement();
    }

    bool loadFromStream(QXmlStreamReader& xmlreader)
    {
        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name() != QTN_DOCK_TAGNAME_TOOLBAR)
            return false;

        id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_IDBAR).toString();

        QString strVisible = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_VISIBLECONTROLBAR).toString();
        visible = strVisible == QTN_DOCK_ATTRVALUE_TRUE;

        pos = QPoint(xmlreader.attributes().value(QTN_DOCK_ATTRNAME_POSITION_X).toString().toInt(), 
            xmlreader.attributes().value(QTN_DOCK_ATTRNAME_POSITION_Y).toString().toInt());

        area = static_cast<Qtitan::DockBarArea>(xmlreader.attributes().value(QTN_DOCK_ATTRNAME_POSITIONBAR).toString().toInt());
        orientation = !xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ORIENTATIONBAR).isEmpty() ? 
            static_cast<Qt::Orientation>(xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ORIENTATIONBAR).toString().toInt()) : Qt::Horizontal;

        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name() != QTN_DOCK_TAGNAME_RECTDOCKPOS)
            return false;
        int left = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_LEFT).toString().toInt();
        int right = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_RIGHT).toString().toInt();
        int top = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_TOP).toString().toInt();
        int bottom = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_BOTTOM).toString().toInt();
        rect = QRect(QPoint(left, top), QPoint(right, bottom));
        xmlreader.skipCurrentElement();

        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name() != QTN_DOCK_TAGNAME_RECTFLOATPOS)
            return false;
        left = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_LEFT).toString().toInt();
        right = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_RIGHT).toString().toInt();
        top = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_TOP).toString().toInt();
        bottom = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_BOTTOM).toString().toInt();
        windowRect = QRect(QPoint(left, top), QPoint(right, bottom));
        xmlreader.skipCurrentElement();
        xmlreader.skipCurrentElement();

        return true;
    }
};

/*!
Save layout of the dock bars internals to the file given by \a fileName with given \a autoFormatting.
\sa saveStateToDevice()
*/
bool DockBarManager::saveStateToFile(const QString& fileName, bool autoFormatting)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    saveStateToDevice(&file, autoFormatting);
    return true;
}

/*!
Load layout of the internals dock bars from the given \a fileName.
\sa loadStateFromDevice()
*/
bool DockBarManager::loadStateFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    return loadStateFromDevice(&file);
}

/*!
Save layout of the dock bars internals to the \a device with given \a autoFormatting.
\sa saveStateToFile()
*/
void DockBarManager::saveStateToDevice(QIODevice* device, bool autoFormatting)
{
    QTN_D(const DockBarManager);
    QXmlStreamWriter xmlwriter(device);
    xmlwriter.setAutoFormatting(autoFormatting);
    xmlwriter.writeStartDocument(QStringLiteral("1.0"));
    xmlwriter.writeNamespace(QStringLiteral("https://www.devmachines.com/qt/qtitan"), QStringLiteral("Qtitan"));
    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_DOCKBARLAYOUTS);
    xmlwriter.writeAttribute(QStringLiteral("type"), QStringLiteral("layoutBar"));
    xmlwriter.writeAttribute(QStringLiteral("version"), QStringLiteral("1.2"));

    d.assignUniqueName();

    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_TOOLBARS); // BEGIN
    xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_COUNT, QString::number(d.m_bars.count()));

    for (QList<DockBarBase*>::ConstIterator it = d.m_bars.begin(); it != d.m_bars.end(); ++it)
    {
        DockBarBase* controlBase = *it;
        ToolBarInfo info;

        info.visible = !controlBase->isHidden();
        info.orientation = DockBarBasePrivate::_get(controlBase)->orientation();
        info.area = controlBase->dockBarArea();
        info.id = controlBase->id();
        info.pos = QPoint(controlBase->x(), controlBase->y());
        info.rect = DockBarBasePrivate::_get(controlBase)->m_rectDockPos;
        info.windowRect = DockBarBasePrivate::_get(controlBase)->m_windowRect;

        info.saveToStream(xmlwriter);
    }
    xmlwriter.writeEndElement(); // QTN_DOCK_TAGNAME_TOOLBARS

    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_DOCKAREABARS); // BEGIN
    for (int index = Qtitan::LeftArea; index <= Qtitan::BottomArea; ++index)
    {
        Qtitan::LayoutArea pos = static_cast<Qtitan::LayoutArea>(index);
        xmlwriter.writeStartElement(QString(QTN_DOCK_TAGNAME_AREABAR)); // BEGIN
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_POSITIONAREABAR, positionAreaBar(pos));

        DockBarSite* dockBar = d.m_dockSideMap.value(pos);
        int count = DockBarSitePrivate::_get(dockBar)->m_listBar.count();
        if (count > 1)
        {
            QString nameIdPrev = QTN_DOCK_TAGNAME_STARTBAR;
            int actualCount = 0;
            for (int i = 0; i < count; i++)
            {
                DockBarBase* controlBase = dockBar->dockedToolBar(i);
                QString nameId = controlBase ? controlBase->id() : QTN_DOCK_TAGNAME_EMPTYBAR;

                if (nameId == QTN_DOCK_TAGNAME_EMPTYBAR && nameIdPrev == QTN_DOCK_TAGNAME_EMPTYBAR)
                    continue;

                nameIdPrev = nameId;
                actualCount++;
            }
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_COUNT, QString::number(actualCount));
            if (actualCount > 1)
            {
                nameIdPrev = QTN_DOCK_TAGNAME_STARTBAR;
                for (int i = 0; i < count; i++)
                {
                    DockBarBase* controlBase = dockBar->dockedToolBar(i);
                    QString nameId = controlBase ? controlBase->id() : QTN_DOCK_TAGNAME_EMPTYBAR;

                    if (nameId == QTN_DOCK_TAGNAME_EMPTYBAR && nameIdPrev == QTN_DOCK_TAGNAME_EMPTYBAR)
                        continue;
                    nameIdPrev = nameId;

                    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_TOOLBAR); // BEGIN
                    xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_IDBAR, nameId);
                    xmlwriter.writeEndElement();
                }
            }
        }
        xmlwriter.writeEndElement(); // END - QTN_DOCK_TAGNAME_AREABAR
    }
    xmlwriter.writeEndElement(); // END - QTN_DOCK_TAGNAME_DOCKBARS

    xmlwriter.writeEndElement(); // END - QTN_DOCK_TAGNAME_DOCKBARLAYOUTS
    xmlwriter.writeEndDocument();
}

bool DockBarManager::loadStateFromDevice(QIODevice* device)
{
    QTN_D(DockBarManager);
    QXmlStreamReader xmlreader(device);

    if (xmlreader.readNext() != QXmlStreamReader::StartDocument)
        return false;

    if (!xmlreader.readNextStartElement())
        return false;

    if (xmlreader.name() != QTN_DOCK_TAGNAME_DOCKBARLAYOUTS)
        return false;

    QXmlStreamAttributes attrs = xmlreader.attributes();
    QString version = attrs.hasAttribute(QStringLiteral("version")) ? attrs.value(QStringLiteral("version")).toString() : QString();
    if (version != QStringLiteral("1.1") && version != QStringLiteral("1.2"))
        return false;

    d.assignUniqueName();

    if (!xmlreader.readNextStartElement())
        return false;
    if (xmlreader.name() != QTN_DOCK_TAGNAME_TOOLBARS)
        return false;
    int countBar = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_COUNT).toString().toInt();
    if (countBar != d.m_bars.count())
        return false;

    QVector<ToolBarInfo> toolBarInfoList;
    for (int i = 0; countBar > i; i++)
    {
        ToolBarInfo info;
        info.loadFromStream(xmlreader);
        toolBarInfoList.append(info);
    }

    xmlreader.skipCurrentElement();

    if (!xmlreader.readNextStartElement())
        return false;

    QString dockBarSide = version == QStringLiteral("1.1") ? QStringLiteral("DockBarArea") : QTN_DOCK_TAGNAME_DOCKAREABARS;
    if (xmlreader.name() != dockBarSide)
        return false;
    int index = version == QStringLiteral("1.1") ? Qtitan::DefaultArea : Qtitan::LeftArea;
    for (; index <= Qtitan::BottomArea; ++index)
    {
        Qtitan::LayoutArea area = static_cast<Qtitan::LayoutArea>(index);
        DockBarSite* dockSite = d.m_dockSideMap.value(area);

        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name() != QTN_DOCK_TAGNAME_AREABAR)
            return false;

        QString str = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_POSITIONAREABAR).toString();
        if (str != positionAreaBar(area))
            return false;

        int count = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_COUNT).toString().toInt();

        if (count > 1)
        {
            for (int i = 0; i < count; i++)
            {
                if (!xmlreader.readNextStartElement())
                    return false;
                if (xmlreader.name() != QTN_DOCK_TAGNAME_TOOLBAR)
                    return false;

                DockBarBase* controlBase = d.toolBarById(xmlreader.attributes().value(QTN_DOCK_ATTRNAME_IDBAR).toString());
                if (controlBase != Q_NULL)
                {
                    if (controlBase->parentWidget() != dockSite)
                        controlBase->setParent(dockSite);

                    if (DockBarBasePrivate::_get(controlBase)->m_dockSite != Q_NULL)
                        DockBarBasePrivate::_get(controlBase)->m_dockSite->removeDockBar(controlBase);
                    DockBarBasePrivate::_get(controlBase)->m_dockSite = dockSite;
                    DockBarBasePrivate::_get(controlBase)->m_dockBarArea = dockSite->dockBarArea();
                }
                DockBarSitePrivate::_get(dockSite)->m_listBar.insert(i, controlBase);

                xmlreader.skipCurrentElement();
            }
        }
        xmlreader.skipCurrentElement();
    }
    xmlreader.skipCurrentElement();
    xmlreader.skipCurrentElement();
    if (xmlreader.readNext() != QXmlStreamReader::EndDocument)
        return false;

    for (QVector<ToolBarInfo>::ConstIterator it = toolBarInfoList.constBegin(); it != toolBarInfoList.constEnd(); ++it)
    {
        const ToolBarInfo& info = *it;
        if (DockBarBase* controlBase = d.toolBarById(info.id))
        {
#if 0 //remove me
            controlBase->setVisible(info.visible);
#endif
            controlBase->move(info.pos);
            controlBase->setDockBarArea(info.area);
            DockBarBasePrivate::_get(controlBase)->setOrientation(info.orientation);
            DockBarBasePrivate::_get(controlBase)->m_rectDockPos = info.rect;
            DockBarBasePrivate::_get(controlBase)->m_windowRect = info.windowRect;
            if (info.area == Qtitan::DockBarFloat)
            {
                d.floatControlBar(controlBase);
                if (DockToolBar* toolBar = qobject_cast<DockToolBar*>(controlBase))
                {
                    DockBarWindow* dockWindow = DockToolBarPrivate::_get(toolBar)->getDockBarWindow();
                    dockWindow->setDockBar(toolBar);
                    dockWindow->setGeometry(info.windowRect);
                }
            }
            controlBase->setVisible(info.visible);
        }
    }

    return true;
}

bool DockBarManager::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != managedWidget())
        return QObject::eventFilter(watched, event);
    if (event->type() == QEvent::StyleChange)
        CommonStyle::ensureStyle();
    return QObject::eventFilter(watched, event);
}



