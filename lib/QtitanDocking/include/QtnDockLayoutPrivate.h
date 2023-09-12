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

#ifndef QTN_DOCKLAYOUTPRIVATE_H
#define QTN_DOCKLAYOUTPRIVATE_H

#include <QStyleOption>
#include <QBasicTimer>

#include "QtnDockPanelSplitter.h"
#include "QtnDockLayoutPanel.h"

QTITAN_BEGIN_NAMESPACE

static const QPoint cursorPositionInvalid = QPoint(-100, -100);

/* DockLayoutPanelPrivate */
class DockLayoutPanelPrivate : public DockPanelBasePrivate
{
    QTN_DECLARE_EX_PUBLIC(DockLayoutPanel);
public:
    explicit DockLayoutPanelPrivate(DockLayoutEngine* layoutEngine) : DockPanelBasePrivate(layoutEngine) {}
    virtual ~DockLayoutPanelPrivate();
    inline static DockLayoutPanelPrivate* _get(DockLayoutPanel* layout)
    { return &layout->qtn_d(); }
public:
    virtual bool isEmpty() const;
public:
    static Qtitan::DockPanelAreas isAllowedAreas(DockPanelBase* panelBase, const char* className);
public:
    QList<DockPanelBase*> m_panelList;
private:
    Q_DISABLE_COPY(DockLayoutPanelPrivate)
};

/*DockSplitLayoutPanelPrivate*/
class DockSplitLayoutPanelPrivate : public DockLayoutPanelPrivate
{
    QTN_DECLARE_EX_PUBLIC(DockSplitLayoutPanel);
public:
    explicit DockSplitLayoutPanelPrivate(DockLayoutEngine* layoutEngine);
    virtual ~DockSplitLayoutPanelPrivate();
    inline static DockSplitLayoutPanelPrivate* _get(DockSplitLayoutPanel* layout)
    { return &layout->qtn_d(); }
    inline static const DockSplitLayoutPanelPrivate* _get(const DockSplitLayoutPanel* layout)
    { return &layout->qtn_d(); }
public:
    virtual int containsPanel(DockPanelBase* panelBase) const;
    virtual void getMinMaxData(qtnMinMaxSize& minmaxSize) const;
    virtual void setPanelGeometry(const QRect& rect);
public:
    void extraInsertParam(DockPanelBase* next = Q_NULL, bool after = true);
    int& getMinSize(qtnMinMaxSize& minMaxInfo, bool horiz) const;
    int& getMaxSize(qtnMinMaxSize& minMaxInfo, bool horiz) const;
    void updateSplittersAndPanelsLength(const QRect& rect, const QList<DockPanelBase*>& lstPanel);
    void replaceChildToParent();
    void updateSplitters();
protected:
    virtual void insertDockPanel(DockPanelBase* panelBase);
public:
    QList<DockPanelSplitter*> m_lstSpliters;
    DockPanelBase* m_next;
    bool m_after;
    bool m_replaceParent;
    bool m_horizontal;
    bool m_recalcLayout;
private:
    Q_DISABLE_COPY(DockSplitLayoutPanelPrivate)
};

/* DockAreaTabBar */
class DockAreaTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit DockAreaTabBar(QWidget* parent);
    virtual ~DockAreaTabBar();
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent* event);
private:
    QPoint m_pressedPos;
};

/* DockTabbedLayoutPanelPrivate */
class DockTabbedLayoutPanelPrivate : public DockLayoutPanelPrivate
{
    Q_OBJECT
    QTN_DECLARE_EX_PUBLIC(DockTabbedLayoutPanel);
public:
    explicit DockTabbedLayoutPanelPrivate(DockLayoutEngine* layoutEngine, DockAreaTabBar* tabBar = Q_NULL);
    virtual ~DockTabbedLayoutPanelPrivate();
    inline  static DockTabbedLayoutPanelPrivate* _get(DockTabbedLayoutPanel* layout)
    { return &layout->qtn_d(); }
    inline  static const DockTabbedLayoutPanelPrivate* _get(const DockTabbedLayoutPanel* layout)
    { return &layout->qtn_d(); }
public:
    void updateButtons();
    virtual void repositionTabBar();
    void restoreFocus();
    void makeCurrentTabVisible();
    bool isPanelRestored() const;
    bool isAllowMaximize() const;
    bool isPanelMaximized() const;
    bool isPanelMinimized() const;
    void restore();
    void adjustDockingSize();
    void closeDockPanel(DockWidgetPanel* panel);
    void connectToTitleBar(DockWidgetPanel* widgetPanel, bool connect);
protected:
    void init() override;
    virtual void insertDockPanel(DockPanelBase* panel, bool setFocus = true);
Q_SIGNALS:
    void sigVisibilityAutohide(bool visible);
    void sigStateAutohide(bool);
    void sigActivate(bool);
protected Q_SLOTS:
    void slotCloseDockPanel();
    void slotAutoHideDockPanel();
    void slotMenuButtonDockPanel();
    void slotSetCurrentTab(int index);
public:
    virtual bool isAutoHide() const;
    virtual int containsPanel(DockPanelBase* panelBase) const;
    virtual void getMinMaxData(qtnMinMaxSize& minmaxSize) const;
    virtual void updatePanel(bool activateChanged);
    virtual void setPanelGeometry(const QRect& rect);
    virtual void setChildrenGeometry(const QRect& rect);
public:
    DockWidgetPanel* m_activePanel;
    DockAreaTabBar* m_tabBar;
    QPoint m_pressedPos;
    bool m_titleVisible;
    bool m_active;
    bool m_maximized;
    bool m_ensureSelectedTab;
    bool m_preserved;
private:
    Q_DISABLE_COPY(DockTabbedLayoutPanelPrivate);
};

/* TabWidgetAutoHideBar */
class TabWidgetAutoHideBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabWidgetAutoHideBar(DockAutoHideLayoutPanel* autoHideLayout) 
        : QTabBar(autoHideLayout), m_autoHideLayout(autoHideLayout) { setMouseTracking(true); };
    virtual ~TabWidgetAutoHideBar() {}
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_SIGNALS:
    void qtn_tabBarClicked(int index);
#endif // (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
protected:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void leaveEvent(QEvent* event);
private:
    DockAutoHideLayoutPanel* m_autoHideLayout;
private:
    Q_DISABLE_COPY(TabWidgetAutoHideBar);
};

class DockAutoHideWindow;
/* DockAutoHideLayoutPanelPrivate */
class DockAutoHideLayoutPanelPrivate : public DockLayoutPanelPrivate
{
    Q_OBJECT
    QTN_DECLARE_EX_PUBLIC(DockAutoHideLayoutPanel);
public:
    explicit DockAutoHideLayoutPanelPrivate(DockLayoutEngine* layoutEngine);
    virtual ~DockAutoHideLayoutPanelPrivate();
    inline static DockAutoHideLayoutPanelPrivate* _get(DockAutoHideLayoutPanel* layout)
    { return &layout->qtn_d(); }
    inline static const DockAutoHideLayoutPanelPrivate* _get(const DockAutoHideLayoutPanel* layout)
    { return &layout->qtn_d(); }
public:
    void initWindowTimer(DockWidgetPanel* widgetPanel);
public Q_SLOTS:
    void activeWindow(int index);
public:
    virtual void updatePanel(bool activateChanged);
protected:
    void init() override;
    virtual void timerEvent(QTimerEvent* timerEvent);
public:
    static bool m_closeActiveWindow;
    static int m_hoverTimeout;
    Qtitan::LayoutArea m_layoutArea;
    DockAutoHideWindow* m_activeWindow;
    TabWidgetAutoHideBar* m_tabBar;
    DockWidgetPanel* m_widgetPanel;
    QBasicTimer m_timerMouseHoverDelay;
    QRect m_rcFrameRect;
    int m_itemsLength;
    int m_currentIndexTab;
    bool m_collapseWindow;
private:
    Q_DISABLE_COPY(DockAutoHideLayoutPanelPrivate)
};

inline bool qtn_checkLayoutType(const DockPanelBase* panelBase, DockLayoutPanel::LayoutType layoutType)
{
    const DockLayoutPanel* layoutPanel = qobject_cast<const DockLayoutPanel*>(panelBase);
    return (layoutPanel != Q_NULL && layoutPanel->layoutType() == layoutType);
}

QTITAN_END_NAMESPACE

#endif // QTN_DOCKLAYOUTPRIVATE_H

