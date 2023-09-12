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

#ifndef QTN_DOCKPANELBASE_H
#define QTN_DOCKPANELBASE_H

#include <QWidget>
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockWindowBase;
class DockPanelManager;
class DockPanelBasePrivate;
class QTITAN_EXPORT DockPanelBase : public QWidget
{
    Q_OBJECT
public:
    ~DockPanelBase() override;
protected:
    DockPanelBase(DockPanelBasePrivate& dd);
public:
    DockPanelManager* dockManager() const;
    QString id() const;
    void setId(const QString& id);
    DockPanelBase* parentPanel() const;
    virtual void setParentPanel(DockPanelBase* panelBase);
    DockWindowBase* topDockWindow() const;
    DockWindowBase* dockWindow() const;
    virtual void setDockWindow(DockWindowBase* dockWindow);
    QList<DockPanelBase*> findPanels(const char* className, bool ignoreEmpty = true) const;
protected:
    virtual void focusChanged();
    virtual void childLayoutChanged();
    virtual void parentLayoutChanged();
private:
    virtual void insertChildPanel(DockPanelBase* panelBase);
    virtual void removeChildPanel(DockPanelBase* panelBase);
protected:
    QTN_DECLARE_PRIVATE(DockPanelBase)
private:
    Q_DISABLE_COPY(DockPanelBase)
    friend class DockCentralPanel;
    friend class DockAutoHideLayoutPanel;
    friend class DockLayoutPanel;
    friend class DockTabbedLayoutPanel;
    friend class DockSplitLayoutPanel;
    friend class DockSplitLayoutPanelPrivate;
    friend class DockPanelManagerPrivate;
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKPANELBASE_H
