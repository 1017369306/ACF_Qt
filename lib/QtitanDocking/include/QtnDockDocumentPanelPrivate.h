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

#ifndef QTN_DOCKDOCUMENTPANELPRIVATE_H
#define QTN_DOCKDOCUMENTPANELPRIVATE_H

#include "QtnDockLayoutPrivate.h"
#include "QtnDockLayoutPanel.h"
#include "QtnDockDocumentPanel.h"

QTITAN_BEGIN_NAMESPACE

/* DockDocumentAreaTabBar */
class DockDocumentAreaTabBar : public DockAreaTabBar
{
    Q_OBJECT
public:
    explicit DockDocumentAreaTabBar(QWidget* parent = Q_NULL);
    virtual ~DockDocumentAreaTabBar();
protected:
    Q_DISABLE_COPY(DockDocumentAreaTabBar);
};

class DockDocumentTabbedLayoutPanelPrivate;
/* DockDocumentTabbedLayoutPanel */
class DockDocumentTabbedLayoutPanel : public DockTabbedLayoutPanel
{
    Q_OBJECT
public:
    explicit DockDocumentTabbedLayoutPanel(DockLayoutEngine* layoutEngine);
public:
    virtual bool isTitleVisible() const;
    virtual bool isTabsVisible() const;
    virtual bool canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center);
protected:
    virtual ~DockDocumentTabbedLayoutPanel();
protected:
    virtual DockLayoutPanel* createCopyInstance() const;
    virtual void insertChildPanel(DockPanelBase* panelBase);
    virtual void removeChildPanel(DockPanelBase* panelBase);
protected:
    virtual void paintEvent(QPaintEvent* event);
private:
    QTN_DECLARE_EX_PRIVATE(DockDocumentTabbedLayoutPanel);
    Q_DISABLE_COPY(DockDocumentTabbedLayoutPanel);
};

/* DockDocumentTabbedLayoutPanelPrivate */
class DockDocumentTabbedLayoutPanelPrivate : public DockTabbedLayoutPanelPrivate
{
    Q_OBJECT
    QTN_DECLARE_EX_PUBLIC(DockDocumentTabbedLayoutPanel);
public:
    explicit DockDocumentTabbedLayoutPanelPrivate(DockLayoutEngine* layoutEngine);
    virtual ~DockDocumentTabbedLayoutPanelPrivate();
    inline static DockDocumentTabbedLayoutPanelPrivate* _get(DockDocumentTabbedLayoutPanel* layout)
    { return &layout->qtn_d(); }
    inline static const DockDocumentTabbedLayoutPanelPrivate* _get(const DockDocumentTabbedLayoutPanel* layout)
    { return &layout->qtn_d(); }
public:
    virtual void repositionTabBar();
public Q_SLOTS:
    void closePanel(int index);
public:
    virtual void setChildrenGeometry(const QRect& rect);
private:
    Q_DISABLE_COPY(DockDocumentTabbedLayoutPanelPrivate);
};

class DockDocumentLayoutPanelPrivate;
class DockDocumentLayoutPanel : public DockSplitLayoutPanel
{
    Q_OBJECT
public:
    explicit DockDocumentLayoutPanel(DockLayoutEngine* layoutEngine);
protected:
    virtual ~DockDocumentLayoutPanel();
public:
    Qtitan::LayoutArea tabPosition() const;
    DockDocumentTabbedLayoutPanel* activTabbedLayout() const;
public:
    virtual DockLayoutPanel* createCopyInstance() const;
    virtual LayoutType layoutType() const { return DockLayoutPanel::Document;}
    virtual bool canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center);
protected:
    virtual void insertChildPanel(DockPanelBase* panelBase);
    virtual void removeChildPanel(DockPanelBase* panelBase);
protected:
    virtual void paintEvent(QPaintEvent* event);
private:
    QTN_DECLARE_EX_PRIVATE(DockDocumentLayoutPanel);
    Q_DISABLE_COPY(DockDocumentLayoutPanel);
    friend class DockLayoutEngine;
};

/* DockTabbedLayoutPanelPrivate */
class DockDocumentLayoutPanelPrivate : public DockSplitLayoutPanelPrivate
{
    QTN_DECLARE_EX_PUBLIC(DockDocumentLayoutPanel);
public:
    explicit DockDocumentLayoutPanelPrivate(DockLayoutEngine* layoutEngine);
    virtual ~DockDocumentLayoutPanelPrivate();
    static DockDocumentLayoutPanelPrivate* _get(DockDocumentLayoutPanel* layout)
    { return &layout->qtn_d(); }
    static const DockDocumentLayoutPanelPrivate* _get(const DockDocumentLayoutPanel* layout)
    { return &layout->qtn_d(); }
public:
    virtual bool isEmpty() const;
    bool isEmptyFull() const;
protected:
    virtual void insertDockPanel(DockPanelBase* panelBase);
public:
    Qtitan::LayoutArea m_tabPosition;
private:
    Q_DISABLE_COPY(DockDocumentLayoutPanelPrivate);
};

/* DockDocumentPanelPrivate */
class DockDocumentPanelPrivate : public DockWidgetPanelPrivate
{
    QTN_DECLARE_EX_PUBLIC(DockDocumentPanel);
public:
    explicit DockDocumentPanelPrivate(DockPanelManager* manager);
    virtual ~DockDocumentPanelPrivate();
public:
    void initialization();
public:
    virtual Qtitan::DockPanelAreas allowedAreas() const;
    virtual void setPanelGeometry(const QRect& rect);
private:
    Q_DISABLE_COPY(DockDocumentPanelPrivate);
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKDOCUMENTPANELPRIVATE_H
