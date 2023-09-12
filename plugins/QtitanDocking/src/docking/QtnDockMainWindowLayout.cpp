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

#include <qtoolbar.h>
#include <QtGui>
#include <QStatusBar>
#include <QApplication>
#include <QMenu>
#include <QStyle>

#include "QtnDockMainWindowLayout.h"
#include "QtnDockPanelManagerPrivate.h"
#include "QtnDockBarManagerPrivate.h"
#include "QtnDockBarSitePrivate.h"
#include "QtnDockToolBarPrivate.h"
#include "QtnDockToolBar.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/* DockMainWindowLayout */
DockMainWindowLayout::DockMainWindowLayout(QWidget* parent)
    : CustomLayout(parent)
    , m_dockBarManager(Q_NULL)
    , m_dockPanelManager(Q_NULL)
{
    setSpacing(0);
    setContentsMargins(QMargins());
}

DockMainWindowLayout::~DockMainWindowLayout()
{
    m_dockBarManager = Q_NULL;
    m_dockPanelManager = Q_NULL;
}

void DockMainWindowLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);
    updateGeometry();
}

void DockMainWindowLayout::invalidate()
{
    CustomLayout::invalidate();
}

void DockMainWindowLayout::updateGeometry()
{
    UpdatesEnabledBlocker blocker(parentWidget());
    QRect r = geometry();
#ifndef Q_OS_WIN
    if (QWidget* menuBarItem = menuBar())
    {
        if (r.width() != menuBarItem->width())
            menuBarItem->setFixedWidth(r.width());
    }
#endif
    if (m_statusBar)
    {
        QRect sbr(r.topLeft(), QSize(r.width(), m_statusBar->heightForWidth(r.width())).expandedTo(m_statusBar->minimumSize()));
        sbr.moveBottom(r.bottom());
        QRect vr = QStyle::visualRect(parentWidget()->layoutDirection(), r, sbr);
        m_statusBar->setGeometry(vr);
        r.setBottom(sbr.top() - 1);
    }

    QRect rectCenter = r;
    if (m_dockBarManager)
        rectCenter = m_dockBarManager->updateLayoutGeometry(r);

    if (m_dockPanelManager)
        rectCenter = DockPanelManagerPrivate::_get(m_dockPanelManager)->updateLayoutGeometry(rectCenter);

    if (m_centralWidget && m_centralWidget->widget())
        m_centralWidget->setGeometry(rectCenter);
}

QMenu* DockMainWindowLayout::createToolBarsContextMenu()
{
    if (m_dockBarManager == Q_NULL)
        return Q_NULL;

    QMenu* menu = new QMenu();
    QList<DockToolBar*> lstToolBars = m_dockBarManager->toolBars();
    for (QList<DockToolBar *>::const_iterator it = lstToolBars.constBegin(); it != lstToolBars.constEnd(); ++it)
    {
        DockToolBar* toolBar = (*it);
        QAction* action = menu->addAction(toolBar->windowTitle());
        action->setCheckable(true);
        action->setChecked(!toolBar->isHidden());
        QObject::connect(action, SIGNAL(triggered(bool)), toolBar, SLOT(setVisible(bool)));
    }
    return menu;
}

void DockMainWindowLayout::setDockBarManager(DockBarManager* mng)
{
    m_dockBarManager = mng;
}

DockBarManager* DockMainWindowLayout::dockBarManager() const
{
    return m_dockBarManager;
}

void DockMainWindowLayout::setDockPanelManager(DockPanelManager* mng)
{
    m_dockPanelManager = mng;
}

DockPanelManager* DockMainWindowLayout::dockPanelManager() const
{
    return m_dockPanelManager;
}

