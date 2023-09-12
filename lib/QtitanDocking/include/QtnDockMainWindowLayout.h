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

#ifndef QTN_DOCKWIDGETLAYOUT_H
#define QTN_DOCKWIDGETLAYOUT_H

#include <QWidget>
#include <QMap>
#include <QAction>
#include <QLayoutItem>
#include "QtnDockDef.h"
#include "QtnCustomLayout.h"
#include "QtnDockBarManager.h"
#include "QtnDockPanelManager.h"

class QMenu;
class QStatusBar;

QTITAN_BEGIN_NAMESPACE


/* DockMainWindowLayout */
class QTITAN_EXPORT DockMainWindowLayout : public CustomLayout
{
    Q_OBJECT
public:
    explicit DockMainWindowLayout(QWidget* parent);
    virtual ~DockMainWindowLayout();
public:
    virtual void setGeometry(const QRect& rect);
    virtual void invalidate();
public:
    void updateGeometry();

    void setDockBarManager(DockBarManager* mng);
    DockBarManager* dockBarManager() const;

    void setDockPanelManager(DockPanelManager* mng);
    DockPanelManager* dockPanelManager() const;

    QMenu* createToolBarsContextMenu();
protected:
    DockBarManager* m_dockBarManager;
    DockPanelManager* m_dockPanelManager;
private:
    friend class DockToolBar;
    friend class DockToolBarPrivate;
    Q_DISABLE_COPY(DockMainWindowLayout)
};


QTITAN_END_NAMESPACE

#endif // QTN_DOCKWIDGETLAYOUT_H
