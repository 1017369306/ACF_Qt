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

#ifndef QTN_DOCKPANEMANAGER_PRIVATE_H
#define QTN_DOCKPANEMANAGER_PRIVATE_H

#include <QLayout>
#include <QIODevice>
#include <QApplication>

#include "QtnDockPanelManager.h"
#include "QtnDockLayoutPanelPrivate.h"
#include "QtnDockWidgetPanelPrivate.h"


QTITAN_BEGIN_NAMESPACE

#define __DEBUG_PAINTER 0

class DockCentralPanelPrivate;
class DockCentralPanel : public DockLayoutPanel
{
    Q_OBJECT
public:
    DockCentralPanel(DockLayoutEngine* layoutEngine);
    ~DockCentralPanel() override;
    LayoutType layoutType() const override;
    bool canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center) override;
public:
    QWidget* widget() const;
    void setWidget(QWidget* widget);
    void removeWidget(QWidget* widget);
protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
private:
    QTN_DECLARE_EX_PRIVATE(DockCentralPanel)
    Q_DISABLE_COPY(DockCentralPanel)
};

class DockDocumentLayoutPanel;
class DockDocumentTabbedLayoutPanel;
/* DockLayoutEngine */
class DockLayoutEngine : public QObject
{
    Q_OBJECT
public:
    explicit DockLayoutEngine(DockPanelManager* manager);
    virtual ~DockLayoutEngine();
    void initNew(bool documentLayout);
    void finalization();
    DockLayoutPanel* createLayoutPanel(const QString& type);
public:
    void setGeometry(const QRect&);
    void updateGeometry();
public:
    DockPanelManager* dockManager() const { return m_manager; }
    DockLayoutPanel* centralPanel() const { return m_centralPanel; }

    inline const QList<DockWidgetPanel*>& widgetPanelList() const;
    inline const QList<DockLayoutPanel*>& layoutPanelList() const;
    inline const QList<DockWindow*>& windowList() const;
    DockWidgetPanel* findPanelById(const QString& id) const;
    DockSplitLayoutPanel* rootSplitPanel() const { return m_rootSplitPanel; }
    void setRootSplitPanel(DockSplitLayoutPanel* rootPanel);
    Qtitan::LayoutArea calcDockPanelArea(const DockPanelBase* panelBase) const;
    void saveStateToDevice(QIODevice* device, bool autoFormatting);
    bool loadStateFromDevice(QIODevice* device);
    void addLayoutPanel(DockLayoutPanel* panel);
    void removeLayoutPanel(DockLayoutPanel* panel);
    void addWidgetPanel(DockWidgetPanel* panel);
    void removeWidgetPanel(DockWidgetPanel* panel);
    void attachDockWindow(DockWindow* window);
    void resetDockWindow(DockWindow* window);
    bool deleteDockWindow(DockWindow* dockWindow);
    void stackUnderDockWindow(DockWindow* dockWindow);
    void deleteEmptyDockWidgets();
    void detachDockPanel(DockPanelBase* panelBase);
    void setLayoutDirty();
    bool isDestroying() const;
    QString generatePanelId(const QString& name) const;
    bool setPanelId(const DockWidgetPanel* panel, const QString& oldId, const QString& newId);
    DockPanelInfo& getPanelInfo(const DockWidgetPanel* panel, const QString& id);
    bool hasPanelInfo(const QString& id) const;
    void removePanelInfo(const QString& id);
private:
    void autoHideDockPanel(DockPanelBase* panelBase);
    void destroyPanels(bool all = false);
    void copyLayoutEngine(DockLayoutEngine* oldLayoutEngine);

    bool deleteTabbedLayout(const QMap<DockPanelBase*, bool>& map, DockTabbedLayoutPanel* panel);
    bool deleteSplitLayout(DockSplitLayoutPanel* panel);
    bool deleteDocumentTabbedLayout(const QMap<DockPanelBase*, bool>& map, DockDocumentTabbedLayoutPanel* panel);
    bool deleteDocumentLayout(DockDocumentLayoutPanel* panel);

    bool findTabbedPaneToHide(DockAutoHideLayoutPanel* autoHidePanel, DockPanelBase* panelBase);
    void addDockWidgetsTo(DockTabbedLayoutPanel* tabbed, QList<DockPanelBase*>& lst, int features);
    void updateChildrenGeometry(const QRect& rect);

    void repositionMargins(QRect& rect, const QRect& lpszClientMargins);
private:
    DockPanelManager* m_manager;
    DockSplitLayoutPanel* m_rootSplitPanel;
    DockLayoutPanel* m_centralPanel;
    QList<DockWidgetPanel*> m_widgetPanelList;
    QList<DockLayoutPanel*> m_layoutPanelList;
    QList<DockWindow*> m_windowList;
    DockAutoHideLayoutPanel* m_wndAutoHide[4];
    QHash<QString, DockPanelInfo> m_panelInfoMap;
    QHash<DockLayoutPanel*, QStringList> m_extraLoadData;
    QRect m_rect;
    bool m_dirty;
    bool m_destroying;
private:
    friend class DockAutoHideLayoutPanel;
    friend class DockPanelManagerPrivate;
};

inline void DockLayoutEngine::setLayoutDirty()
{ m_dirty = true; }

inline bool DockLayoutEngine::isDestroying() const
{ return m_destroying; }

inline const QList<DockWidgetPanel*>& DockLayoutEngine::widgetPanelList() const
{ return m_widgetPanelList; }

inline const QList<DockLayoutPanel*>& DockLayoutEngine::layoutPanelList() const
{ return m_layoutPanelList; }

inline const QList<DockWindow*>& DockLayoutEngine::windowList() const
{ return m_windowList; }

class DockWidgetDragDrop;
class DockDocumentPanel;
class DockPanelManagerPrivate : public QObject
{
    Q_OBJECT
    QTN_DECLARE_PUBLIC(DockPanelManager)
public:
    explicit DockPanelManagerPrivate();
    virtual ~DockPanelManagerPrivate();
    inline static DockPanelManagerPrivate* _get(DockPanelManager* manager);
    inline static const DockPanelManagerPrivate* _get(const DockPanelManager* manager);
    void initialization();
public:
    inline bool isCentralPanelVisible() const { return m_centralPanelVisible; }
    inline bool isSplitterTrackerUsed() const { return m_useSplitterTracker; }
    inline bool isSplittersLocked() const { return m_lockSplitters; }

    inline bool isUseThemedFloatingFrames() const { return m_useThemedFloatingFrames; }
    void setUseThemedFloatingFrames(bool themedFloatingFrames);

    inline Qtitan::LayoutArea dockPanelArea(const DockPanelBase* panelBase) const;
    inline Qtitan::LayoutArea rtlPosition(Qtitan::LayoutArea pos) const;

    inline DockWidgetDragDrop* dockingMasterDrag() const { return m_dockMasterDrag; }
    inline bool attachingDockWidget() const { return m_attachingPane; }

    inline bool isSaveState() const { return m_stateManager == Save; }
    inline bool isReadState() const { return m_stateManager == Read; }
    inline bool isNormalState() const { return m_stateManager == Normal; }
    inline bool isDockPanelHidden(DockWidgetPanel* widgetPanel) const { return widgetPanel != Q_NULL && widgetPanel->isAutoHide(); }

    DockLayoutPanel* centralPanel() const;
    DockLayoutEngine* dockLayoutEngine() const;

    void setLayoutEngine(DockLayoutEngine* layoutEngine);

    QRect calculateResultDockingRect(DockPanelBase* panelBase, Qtitan::DockPanelArea area, DockPanelBase* adjacent);

    void internal_insertDockPanel(DockWidgetPanel* widgetPanel, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel);
    void internal_insertDockPanel(DockPanelBase* panelBase, Qtitan::DockPanelArea area, DockPanelBase* targetPanel);
    void internal_attachDockWidget(DockPanelBase* target, DockPanelBase* panel);
    void internal_removeDockPanel(DockPanelBase* panelBase);
    void internal_detachDockPanel(DockWidgetPanel* widgetPanel);

    void insertCenterDockPanel(DockPanelBase* panelBase, Qtitan::DockPanelArea area, DockPanelBase* before = Q_NULL);
    void insertDockPanelBefore(DockPanelBase* panelBase, Qtitan::DockPanelArea area, DockPanelBase* before = Q_NULL);
    void tabifyDockPanel(DockPanelBase* target, DockPanelBase* panelBase);

    void switchDocking(DockPanelBase* panelBase);
    bool switchDocking(DockWidgetPanel* widgetPanel, DockPanelBase* ownerPanel);

    void autoHideDockPanel(DockPanelBase* panelBase);
    void preservePanelState();
    void ensureVisible(DockPanelBase* panelBase);
    QRect updateLayoutGeometry(const QRect& rect);
    void changedActiveDockPanel(DockWidgetPanel* widgetPanel, bool active);
    void recalcLayout(DockPanelBase* panelBase = Q_NULL);
    void updatePanelProperties(DockWidgetPanel* widgetPanel);
    void updateFocusPanels();
    void updatePanels();
public Q_SLOTS:
    void focusChanged(QWidget* old, QWidget* now);
    void setCentralPanelVisible(bool visible);
public:
    DockLayoutEngine* m_layoutEngine;
    DockWidgetDragDrop* m_dockMasterDrag;
    DockWidgetPanel* m_activePanel;
    DockDocumentPanel* m_lastActiveDocPanel;
    QWidget* m_widgetLastFocus;
    QString m_dockWindowTitle;
    int m_margin;
    DockWidgetPanel::DockPanelFeatures m_defaultPanelFeatures;

    enum StateManager
    {
        Normal,
        Read,
        Save,
    };

    StateManager m_stateManager;
    bool m_attachingPane;
    bool m_showDockWindowFullContentsWhileDragging;    // if true, then DockWidgetPanel content is visible when dragging.
    bool m_showMarkers;                 // if true, then docking markers will be visible when dragging.
    bool m_panelTransparent;       // if true, then DockWidgetPanel position will be semitransparent when dragging. 

    bool m_centralPanelVisible;
    bool m_useSplitterTracker;
    bool m_lockSplitters;
    bool m_useThemedFloatingFrames;
    bool m_layoutCreated;

    int m_splitterGap;
    bool m_closeGroupOnButtonClick;
    bool m_hideGroupOnButtonClick;
    QSize m_ptMinClientSize;
    uint m_updateLayoutCount;
};

inline DockPanelManagerPrivate* DockPanelManagerPrivate::_get(DockPanelManager* manager)
{ return &manager->qtn_d();}

inline const DockPanelManagerPrivate* DockPanelManagerPrivate::_get(const DockPanelManager* manager)
{ return &manager->qtn_d(); }

inline Qtitan::LayoutArea DockPanelManagerPrivate::dockPanelArea(const DockPanelBase* panelBase) const
{
    if (!m_layoutEngine)
        return Qtitan::TopArea;
    return m_layoutEngine->calcDockPanelArea(panelBase);
}

inline Qtitan::LayoutArea DockPanelManagerPrivate::rtlPosition(Qtitan::LayoutArea pos) const
{
    if (QApplication::isRightToLeft())
    {
        if (pos == Qtitan::LeftArea)
            pos = Qtitan::RightArea;
        else if (pos == Qtitan::RightArea)
            pos = Qtitan::LeftArea;
    }
    return pos;
}

QTITAN_END_NAMESPACE

#endif //QTN_DOCKPANEMANAGER_PRIVATE_H
