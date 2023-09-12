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
#include <QStyle>
#include <QEvent>
#include <QStyleOption>
#include <QPainter>

#include "QtnDockWindow.h"
#include "QtnDockPanelBase.h"
#include "QtnDockWindowPrivate.h"
#include "QtnDockWidgetPanelPrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

/* DockPanelBasePrivate */
DockPanelBasePrivate::DockPanelBasePrivate(DockLayoutEngine* layoutEngine)
    : m_layoutEngine(layoutEngine)
    , m_parentPanel(Q_NULL)
    , m_dockWindow(Q_NULL)
    , m_sizeDocking(QSize())
    , m_length(0)
{
    Q_ASSERT(m_layoutEngine != Q_NULL);
}

DockPanelBasePrivate::~DockPanelBasePrivate()
{
}

bool DockPanelBasePrivate::isEmpty() const
{ return false; }

int DockPanelBasePrivate::containsPanel(DockPanelBase* panelBase) const
{
    QTN_P(const DockPanelBase);
    return panelBase == &p ? 1 : -1;
}

/*! \internal */
void DockPanelBasePrivate::getMinMaxData(qtnMinMaxSize& minmaxSize) const
{
    QTN_P(const DockPanelBase);
    if (qobject_cast<const DockCentralPanel*>(&p) && DockPanelManagerPrivate::_get(p.dockManager())->isCentralPanelVisible())
        minmaxSize.minTrackSize = DockPanelManagerPrivate::_get(p.dockManager())->m_ptMinClientSize;
    minmaxSize.maxTrackSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

/*! \internal */
void DockPanelBasePrivate::updatePanel(bool activateChanged)
{
    QTN_P(DockPanelBase);
    if (DockPanelBase* panelBase = p.parentPanel())
        DockPanelBasePrivate::_get(panelBase)->updatePanel(activateChanged);
    else if (DockWindow* window = qobject_cast<DockWindow*>(p.dockWindow()))
        DockWindowPrivate::_get(window)->updatePanel(activateChanged);
}

void DockPanelBasePrivate::setPanelGeometry(const QRect& rect)
{
    QTN_P(DockPanelBase);
    p.setGeometry(rect);
}

DockLayoutEngine* DockPanelBasePrivate::layoutEngine() const 
{ 
    return m_layoutEngine;
}

DockPanelManager* DockPanelBasePrivate::dockManager() const 
{ 
    return m_layoutEngine->dockManager();
}

void DockPanelBasePrivate::init()
{
    QTN_P(DockPanelBase);
    QObject::connect(&p, SIGNAL(objectNameChanged(const QString&)), this, SLOT(panelObjectNameChanged(const QString&)));
}

void DockPanelBasePrivate::setPanelId(const QString& id)
{
    m_id = id;
}

void DockPanelBasePrivate::panelObjectNameChanged(const QString& objectName)
{
    setPanelId(objectName);
}

/*!
\class DockPanelBase
\inmodule QtitanDocking
\brief Base class for dock panels.
*/
DockPanelBase::DockPanelBase(DockPanelBasePrivate& dd)
    : QWidget()
{
    qtn_d_ptr = &dd;
    qtn_d_ptr->setPublic(this);
    qtn_d_ptr->init();
}

DockPanelBase::~DockPanelBase()
{
    //Q_ASSERT(qtn_d_ptr->m_dockWindow == Q_NULL);
    Q_ASSERT(qtn_d_ptr->m_parentPanel == Q_NULL);
    Q_ASSERT(qtn_d_ptr->m_layoutEngine != Q_NULL);
    QTN_FINI_PRIVATE();
}

DockPanelManager* DockPanelBase::dockManager() const
{
    QTN_D(const DockPanelBase);
    return d.dockManager();
}

QString DockPanelBase::id() const
{
    QTN_D(const DockPanelBase);
    return d.m_id;
}

void DockPanelBase::setId(const QString& id)
{
    QTN_D(DockPanelBase);
    if (d.m_id == id)
        return;
    d.setPanelId(id);
}

DockPanelBase* DockPanelBase::parentPanel() const
{
    QTN_D(const DockPanelBase);
    return d.m_parentPanel;
}

void DockPanelBase::setParentPanel(DockPanelBase* panelBase)
{
    QTN_D(DockPanelBase);
    if (d.m_parentPanel == panelBase)
        return;

    if (d.m_parentPanel != Q_NULL)
        d.m_parentPanel->removeChildPanel(this);

    d.m_parentPanel = panelBase;
    setParent(d.m_parentPanel);

    if (d.m_parentPanel != Q_NULL)
    {
        d.m_parentPanel->insertChildPanel(this);
        d.m_parentPanel->childLayoutChanged();
    }
    parentLayoutChanged();

    if (d.layoutEngine() != Q_NULL)
        d.layoutEngine()->setLayoutDirty();
}

DockWindowBase* DockPanelBase::topDockWindow() const
{
    if (DockWindowBase* parentWindow = this->dockWindow())
        return parentWindow;
    else if (DockPanelBase* panelBase = parentPanel())
        return panelBase->topDockWindow();
    return Q_NULL;
}

DockWindowBase* DockPanelBase::dockWindow() const
{ 
    QTN_D(const DockPanelBase);
    return d.m_dockWindow;
}

void DockPanelBase::setDockWindow(DockWindowBase* dockWindow)
{
    QTN_D(DockPanelBase);
    if (d.m_dockWindow == dockWindow)
        return;
    d.m_dockWindow = dockWindow;
    setParent(d.m_dockWindow);

    if (d.layoutEngine() != Q_NULL)
        d.layoutEngine()->setLayoutDirty();
}

/*! \internal */
static void qtn_findPanels_helper(const DockPanelBase* parent, const char* className, QList<DockPanelBase*>& list, bool ignoreEmpty)
{
    if (parent == Q_NULL || className == Q_NULL)
    {
        Q_ASSERT(parent != Q_NULL && className != Q_NULL);
        return;
    }
    const DockLayoutPanel* layoutPanel = qobject_cast<const DockLayoutPanel*>(parent);
    if (layoutPanel == Q_NULL)
        return;

    const QList<DockPanelBase*>& lstPanelBase = layoutPanel->panels();
    for (QList<DockPanelBase*>::const_iterator it = lstPanelBase.constBegin(); it != lstPanelBase.constEnd(); ++it)
    {
        DockPanelBase* panelBase = (*it);
        if (panelBase != Q_NULL && panelBase->inherits(className))
        {
            bool append = true;
            if (ignoreEmpty)
            {
                if (DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(panelBase))
                {
                    if (layoutPanel->isEmpty())
                        append = false;
                }
            }
            if (append)
                list << panelBase;
        }
        qtn_findPanels_helper(panelBase, className, list, ignoreEmpty);
    }
}

QList<DockPanelBase*> DockPanelBase::findPanels(const char* className, bool ignoreEmpty) const
{
    QList<DockPanelBase*> lstPanel;
    qtn_findPanels_helper(this, className, lstPanel, ignoreEmpty);
    return lstPanel;
}

void DockPanelBase::focusChanged()
{
}

void DockPanelBase::childLayoutChanged()
{
}

void DockPanelBase::parentLayoutChanged()
{
}

void DockPanelBase::insertChildPanel(DockPanelBase* panelBase)
{
    Q_UNUSED(panelBase)
    Q_ASSERT(false);
}

void DockPanelBase::removeChildPanel(DockPanelBase* panelBase)
{
    Q_UNUSED(panelBase)
    Q_ASSERT(false);
}
