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

#ifndef QTN_DOCKBARBASEPRIVATE_H
#define QTN_DOCKBARBASEPRIVATE_H

#include "QtnDockBarBase.h"
#include "QtnDockMainWindow.h"
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockBarSite;
class DockBarManager;
class DockBarBasePrivate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation 
        DESIGNABLE(qobject_cast<DockMainWindow *>(parent()->parent()) == 0) NOTIFY orientationChanged)
public:
    QTN_DECLARE_PUBLIC(DockBarBase)
public:
    explicit DockBarBasePrivate();
    virtual ~DockBarBasePrivate();
    static DockBarBasePrivate* _get(DockBarBase* controlBar);
    static const DockBarBasePrivate* _get(const DockBarBase* controlBar);
public:
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    DockBarManager* barManager() const;
    void setBarManager(DockBarManager* manager);
Q_SIGNALS:
    void orientationChanged(Qt::Orientation orientation);
public:
    Qt::Orientation m_orientation;
    Qtitan::DockBarArea m_dockBarArea;
    DockBarSite* m_dockSite;
    QRect m_rectDockPos;
    QRect m_windowRect;
    QString m_caption;
    bool m_explicitCaption;
private:
    DockBarManager* m_barManager;
private:
    Q_DISABLE_COPY(DockBarBasePrivate)
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKBARBASEPRIVATE_H
