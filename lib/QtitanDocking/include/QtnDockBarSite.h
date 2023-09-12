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

#ifndef QTN_DOCKBARSITE_H
#define QTN_DOCKBARSITE_H

#include <QWidget>
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockBarBase;
class DockBarSitePrivate;
/* DockBarSite */
class DockBarSite : public QWidget
{
    Q_OBJECT
public:
    DockBarSite(Qtitan::LayoutArea pos, QWidget* parent);
    virtual ~DockBarSite();
public:
    Qtitan::DockBarArea dockBarArea() const;
    Qt::Orientation orientation() const;
protected:
    void addDockBar(DockBarBase* bar, QRect* ptRect);
    bool removeDockBar(DockBarBase* bar, int posExclude = -1);

    DockBarBase* dockedToolBar(int index) const;
    int dockedCount() const;
    int dockedVisibleCount() const;
    void visibleToolbars(int pos, QList<DockBarBase*>& arrBars);
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent* event);
private:
    QTN_DECLARE_PRIVATE(DockBarSite)
    Q_DISABLE_COPY(DockBarSite)
    friend class DockBarManager;
    friend class DockBarManagerPrivate;
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKBARSITE_H
