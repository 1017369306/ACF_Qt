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

#ifndef QTN_DOCKBARMANAGERPRIVATE_H
#define QTN_DOCKBARMANAGERPRIVATE_H

#include <QMap>

#include "QtnDockBarManager.h"
#include "QtnDockBarSite.h"
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockBarBase;
class DockPanelManager;
/* DockBarManagerPrivate */
class DockBarManagerPrivate : private QObject
{
    Q_OBJECT
    QTN_DECLARE_PUBLIC(DockBarManager)
public:
    explicit DockBarManagerPrivate();
    virtual ~DockBarManagerPrivate();
public:
    static DockBarManagerPrivate* _get(DockBarManager* manager);
    static const DockBarManagerPrivate* _get(const DockBarManager* manager);
    void initialization();
public:
    void makeDockBarSite();
    void assignUniqueName() const;
    bool addDockBarToSite(DockBarBase* bar, QRect* rect, DockBarSite* dockSite);
    bool addDockBarToDockArea(DockBarBase* bar, Qtitan::DockBarArea dockArea);
    bool floatControlBar(DockBarBase* bar);
    void removeDockBar(DockBarBase* bar, bool destroy);
    void clearDockBars();
    int calcMaxHeightBars() const;
    DockBarBase* toolBarById(const QString& strId) const;
    DockBarSite* areaBars(Qtitan::DockBarArea position) const;
    DockBarSite* canDock(const QPoint& point, DockBarSite* areaBars = Q_NULL) const;
protected:
    QList<DockBarBase*> m_bars;
    QMap<uint, DockBarSite*> m_dockSideMap;
private:
    Q_DISABLE_COPY(DockBarManagerPrivate)
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKBARMANAGERPRIVATE_H
