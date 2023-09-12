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

#ifndef QTN_DOCKWIDGETDRAGDROP_H
#define QTN_DOCKWIDGETDRAGDROP_H

#include <QWidget>
#include "QtnDockDef.h"
#include "QtnCommonStyle.h"
#include "QtnDockLayoutPanelPrivate.h"

QTITAN_BEGIN_NAMESPACE

class HighlightFrameMessages;
class DockWidgetDragDrop;
class DockWidgetPhantomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DockWidgetPhantomWidget(DockWidgetDragDrop* masterDrag, bool drawFrame = false);
    ~DockWidgetPhantomWidget() override;
public:
    void startDrag(const QPoint& pnt);
    void setStartRect(const QRect& rect);
    void setGeometryFrame(const QRect& rect);
public:
    inline void setPositionTab(Qtitan::LayoutArea posTab);
    inline void setDrawTab(bool tab);
    inline void setRectTab(const QRect& rectTab);
protected:
    void createMask(const QRect& rcDockWidget);
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
protected:
    DockWidgetDragDrop* m_masterDrag;
    HighlightFrameMessages* m_frameMessages;
    bool m_drawFrame;
    bool m_drawTab;
    Qtitan::LayoutArea m_posTab;
    QRect m_rectTab;
private:
    Q_DISABLE_COPY(DockWidgetPhantomWidget);
};

void DockWidgetPhantomWidget::setPositionTab(Qtitan::LayoutArea posTab)
{ m_posTab = posTab; }
void DockWidgetPhantomWidget::setDrawTab(bool tab)
{ m_drawTab = tab; }
void DockWidgetPhantomWidget::setRectTab(const QRect& rectTab)
{ m_rectTab = rectTab; }


class DockWidgetMarkerWidget : public QWidget
{
    Q_OBJECT
public:
    enum DockingWidgetMarkerType
    {
        MarkerNone    = 0,
        MarkerLeft    = 1,
        MarkerRight   = 2,
        MarkerTop     = 4,
        MarkerBottom  = 8,
        MarkerCenter  = 16,

        MarkerBasis   = 32,

        MarkerCenterLeft   = 64,
        MarkerCenterRight  = 128,
        MarkerCenterTop    = 256,
        MarkerCenterBottom = 512,
        MarkerCenterCenter = 1024,

        MarkerCenterBasis  = 2048,
    };
public:
    explicit DockWidgetMarkerWidget(DockWidgetDragDrop* masterDrag);
    ~DockWidgetMarkerWidget() override;
public:
    void setMarkerType(DockingWidgetMarkerType type) { m_typeMarker = type; }
    DockingWidgetMarkerType hitTest(const QPoint& pnt);
protected:
    void drawMarker(QPainter* p, QPointF* position, CommonStyle::StandardPixmapEx sp, bool active, bool basis = true) const;
    void drawMarkers(QPainter* p);
    void updateMask(const QSize& size);
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
protected:
    DockingWidgetMarkerType m_typeMarker;
    DockingWidgetMarkerType m_selectedMarker;
    DockWidgetDragDrop* m_masterDrag;
private:
    friend class DockWidgetDragDrop;
    Q_DISABLE_COPY(DockWidgetMarkerWidget)
};

class DockWidgetDragDrop
{
public:
    explicit DockWidgetDragDrop(DockPanelManager* dockManager);
    virtual ~DockWidgetDragDrop();
public:
    void execDrag(DockPanelBase* dockPanel, const QPoint& pos);
    void moveDrag(const QPoint& point, Qt::KeyboardModifiers modifiers);
    void startDrag(/*DockWidgetPanel* panel*/);
    void stopDrag();
    void acceptDrag();
    bool isDragInitialized() const;
    bool isStartDrag() const;
    inline DockWindow* dockWindow() const;
protected:
    void initDrag(DockPanelBase* dockPanel, const QPoint& point, const QRect& initializeRect);
    void findLayout(const QPoint& pt);
    bool checkTargetPanel(DockPanelBase* rootPanel, const QPoint& mousePos);
    bool canDock(const QRect& rc, const QPoint& pt, DockPanelBase* dockWidget, bool inside = true);
    bool isDockToAreaEnabled(DockPanelBase* dockWidget, Qtitan::DockPanelArea area, bool center = false);
    bool isAppendEnabled(DockPanelBase* dockWidget);
    bool isAfter(DockPanelBase* dockWidget, DockPanelBase* widgetCheck) const; 
protected:
    QSize markerSize(DockWidgetMarkerWidget::DockingWidgetMarkerType type) const;
    DockPanelBase* centralPanel() const;
    void updateMarkers();
    void destroyMarkers();
    void setInvisibleMarkers();
    DockWidgetMarkerWidget* createMarker(const QRect& rect, DockWidgetMarkerWidget::DockingWidgetMarkerType typeMarker);
    void updateRect(bool hide);
public:
    bool checkCurrentPanelDocked();
    DockLayoutPanel* childAtHelper(const QPoint& pt, bool& inadmissible);
    bool checkZOrderDockWindow(const DockPanelBase* panelBase, const QPoint& pt);
protected:
    DockPanelManager* m_dockManager;
    DockPanelBase* m_markerDockLayout;     // marker layout
    DockPanelBase* m_lastMarkerDockLayout; // last marker layout
    DockPanelBase* m_dockPanel;   // pointer to the active layout
    DockPanelBase* m_layoutPanel; // pointer to the associated layout

    DockWidgetPhantomWidget* m_panelPhantomWidget;
    DockWidgetPhantomWidget* m_framePhantomWidget;
    QMap<DockWidgetMarkerWidget::DockingWidgetMarkerType, DockWidgetMarkerWidget*> m_lstMarkers;
    bool  m_startDrag;
    bool  m_attachLast;
    bool  m_attach;
    bool  m_sideDock;
    bool  m_centerDock;
    bool  m_floatable;
    bool  m_dockable;

    bool m_useDockingMarkers; // if true to use docking markers
    bool m_usePanelTransparentDragDrop;
    QSize m_szTab;
    QRect m_rectDragFrame;
    QRect m_rectLayout;
    QRect m_rectMarkerDockWidget;
    QRect m_rectLast;
    QPoint m_pntLast;
    QPoint m_pntSnap;
    Qtitan::DockPanelArea m_containDirection;
private:
    friend class HighlightFrameMessages;
    friend class DockWidgetMarkerWidget;
    friend class DockWidgetPhantomWidget;
    Q_DISABLE_COPY(DockWidgetDragDrop)
};

inline DockWindow* DockWidgetDragDrop::dockWindow() const 
{ return qobject_cast<DockWindow*>(m_dockPanel->dockWindow()); }

QTITAN_END_NAMESPACE

#endif // QTN_DOCKWIDGETDRAGDROP_H
