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

#ifndef QTN_DOCKLAYOUTPANELPRIVATE_H
#define QTN_DOCKLAYOUTPANELPRIVATE_H

#include <QStyleOption>

#include "QtnDockWidgetPanelPrivate.h"
#include "QtnDockLayoutPanel.h"

QTITAN_BEGIN_NAMESPACE

class DockSplitLayoutPanelPrivate;
class DockSplitLayoutPanel : public DockLayoutPanel
{
    Q_OBJECT
public:
    explicit DockSplitLayoutPanel(DockLayoutEngine* layoutEngine);
protected:
    virtual ~DockSplitLayoutPanel();
public:
    bool isHorizontal() const;
    virtual DockLayoutPanel* createCopyInstance() const;
    virtual LayoutType layoutType() const;
    virtual bool canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center);
    void replacePanel(DockPanelBase* panelBefore, DockPanelBase* panelBase);
    void initialization(DockPanelBase* panelBase, bool horiz);
    void adjustDockingSize();
protected:
    bool beforeByPosition(const DockPanelBase* panelBase, int pos) const;
    QRect calculatePanelsRectSelf(DockPanelBase* panelBase, Qtitan::DockPanelArea panelArea, DockPanelBase* adjacent);
    virtual void insertChildPanel(DockPanelBase* panelBase);
    virtual void removeChildPanel(DockPanelBase* panelBase);
    virtual void childLayoutChanged();
    virtual void parentLayoutChanged();
    void setHorizontal(bool horiz);
protected:
    virtual void paintEvent(QPaintEvent* event);
protected:
    static QRect calculatePanelsRectChild(DockPanelBase* splitter, DockPanelBase* panel, Qtitan::DockPanelArea area);
    static QRect calculatePanelsRect(bool horiz, const QList<DockPanelBase*>& lst, QRect rect, DockPanelBase* panelBase);
    static void adjustPanesLength(DockPanelManager* manager, const QList<DockPanelBase*>& lst, 
        const QRect& rect, bool horiz, bool apply, int& totalLength, int& lengthAvail);
    static int minSize(qtnMinMaxSize& minmaxSize, bool horiz);
    static int maxSize(qtnMinMaxSize& minmaxSize, bool horiz);
protected:
    DockSplitLayoutPanel(DockSplitLayoutPanelPrivate& d);
private:
    QTN_DECLARE_EX_PRIVATE(DockSplitLayoutPanel)
    Q_DISABLE_COPY(DockSplitLayoutPanel)
    friend class DockPanelManagerPrivate;
    friend class DockWindowPrivate;
    friend class DockLayoutEngine;
};

class DockTabbedLayoutPanelPrivate;
class DockTabbedLayoutPanel : public DockLayoutPanel
{
    Q_OBJECT
public:
    explicit DockTabbedLayoutPanel(DockLayoutEngine* layoutEngine);
protected:
    virtual ~DockTabbedLayoutPanel();
public:
    Qtitan::LayoutArea tabPosition() const;
    void setTabPosition(Qtitan::LayoutArea pos);
    bool isPreserved() const;
    void setPreserved(bool preserved);
    void setActivePanel(DockWidgetPanel* panel, bool focus = true, bool update = true);
    DockWidgetPanel* activePanel() const;
    QString title() const;

    bool isActive() const;
    int titleHeight() const;
    virtual bool isTitleVisible() const;
public:
    //add by zhangyang 20230911
    virtual void initialization(DockWidgetPanel* panel);
    void adjustMinMaxClientSize(qtnMinMaxSize& minmaxSize, bool caption = false) const;
public:
    virtual DockLayoutPanel* createCopyInstance() const;
    virtual LayoutType layoutType() const;
    virtual bool canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center);
    virtual bool isTabsVisible() const;
    virtual bool canDock(QRect& rcClient, const QPoint& pt) const;
protected:
    void rebuildTabs();
    int currentIndexTab() const;
protected:
    virtual void focusChanged();
    virtual void insertChildPanel(DockPanelBase* panelBase);
    virtual void removeChildPanel(DockPanelBase* panelBase);
    virtual void childLayoutChanged();
    virtual void parentLayoutChanged();
    virtual void updatePanelProperties(DockWidgetPanel* widgetPanel);

    void setVisibleTitle(bool visible);
    void showDockPanel(DockWidgetPanel* panel, bool focus);
protected:
    DockTabbedLayoutPanel(DockTabbedLayoutPanelPrivate& dd);
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);
private:
    QTN_DECLARE_EX_PRIVATE(DockTabbedLayoutPanel)
    Q_DISABLE_COPY(DockTabbedLayoutPanel)
    friend class DockWindowPrivate;
    friend class DockPanelManager;
    friend class DockLayoutEngine;
    friend class DockPanelManagerPrivate;
};

class DockAutoHideWindow;
class DockAutoHideLayoutPanelPrivate;
class DockAutoHideLayoutPanel : public DockLayoutPanel
{
    Q_OBJECT
public:
    explicit DockAutoHideLayoutPanel(DockLayoutEngine* layoutEngine);
protected:
    virtual ~DockAutoHideLayoutPanel();
public:
    DockAutoHideWindow* activeWindow() const;
    void setActiveWindow(DockAutoHideWindow* window);
    Qtitan::LayoutArea layoutArea() const;
    void setLayoutArea(Qtitan::LayoutArea area);
public:
    virtual LayoutType layoutType() const;
    virtual bool canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center);
public:
    void setCollapseWindow(DockTabbedLayoutPanel* tabbedPanel);
    void closeActiveWindow(bool delayed = false);
    void closeActiveWindows();
    void showDockPanel(DockWidgetPanel* panel, bool focus = true);
    int dockHeight() const;
public:
    static bool isCloseActiveWindow();
protected:
    QRect setPanelGeometryEx(const QRect& rect);
    virtual void updatePanelProperties(DockWidgetPanel* widgetPanel);
    virtual void insertChildPanel(DockPanelBase* panelBase);
    virtual void removeChildPanel(DockPanelBase* panelBase);
    virtual void childLayoutChanged();
protected:
    void updateGeometryTabBar(const QRect& rcClient);
    void rebuildTabs();
protected:
    virtual void paintEvent(QPaintEvent* event);
private:
    QTN_DECLARE_EX_PRIVATE(DockAutoHideLayoutPanel)
    Q_DISABLE_COPY(DockAutoHideLayoutPanel)
    friend class DockLayoutEngine;
};


QTITAN_END_NAMESPACE

#endif // QTN_DOCKLAYOUTPANELPRIVATE_H

