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

#ifndef QTN_DOCKWINDOW_H
#define QTN_DOCKWINDOW_H

#include "QtnDockPanelBase.h"

class QStyleOptionDockWidget;

QTITAN_BEGIN_NAMESPACE
/* DockWindowBase */
class QTITAN_EXPORT DockWindowBase : public QWidget
{
    Q_OBJECT
protected:
    explicit DockWindowBase(DockPanelManager* manager);
    virtual ~DockWindowBase();
private:
    Q_DISABLE_COPY(DockWindowBase)
};

class DockLayoutEngine;
class DockLayoutPanel;
class DockWindowPrivate;
/* DockWindow */
class QTITAN_EXPORT DockWindow : public DockWindowBase
{
    Q_OBJECT
public:
    explicit DockWindow(DockLayoutEngine* layoutEngine);
protected:
    virtual ~DockWindow();
public:
    void initializationWindow(DockPanelBase* panelBase);
    void finalizationWindow();
    DockPanelManager* dockManager() const;
    DockLayoutPanel* layoutPanel() const;
protected:
    virtual bool event(QEvent* event);
    virtual void moveEvent(QMoveEvent* event);
private:
    QTN_DECLARE_PRIVATE(DockWindow)
    Q_DISABLE_COPY(DockWindow)
    friend class DockLayoutEngine;
};
QTITAN_END_NAMESPACE

#endif //QTN_DOCKWINDOW_H

