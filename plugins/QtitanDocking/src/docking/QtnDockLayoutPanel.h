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

#ifndef QTN_DOCKLAYOUTPANEL_H
#define QTN_DOCKLAYOUTPANEL_H

#include "QtnDockPanelBase.h"

QTITAN_BEGIN_NAMESPACE
class DockWidgetPanel;
class DockDocumentLayoutPanel;
class DockLayoutPanelPrivate;
/* DockLayoutPanel */
class QTITAN_EXPORT DockLayoutPanel : public DockPanelBase
{
    Q_OBJECT
protected:
    DockLayoutPanel(DockLayoutPanelPrivate& d);
public:
    ~DockLayoutPanel() override;
    enum LayoutType
    {
        Central   = 0,
        Split     = 1,
        Tabbed    = 2,
        AutoHide  = 3,
        Document  = 4,
        DocumentTabbed = 5,
    };
public:
    const QList<DockPanelBase*>& panels() const;
public:
    virtual LayoutType layoutType() const = 0;
    virtual DockLayoutPanel* createCopyInstance() const;
    virtual bool isEmpty() const;
    virtual int childCount() const;
    virtual bool canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center) = 0;
    void childLayoutChanged() override;
    void parentLayoutChanged() override;
    virtual void updatePanelProperties(DockWidgetPanel* widgetPanel);
private:
    QTN_DECLARE_EX_PRIVATE(DockLayoutPanel)
    Q_DISABLE_COPY(DockLayoutPanel)
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKLAYOUTPANEL_H

