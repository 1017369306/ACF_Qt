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

#ifndef QTN_DOCKWINDOWPRIVATE_H
#define QTN_DOCKWINDOWPRIVATE_H

#include <QToolButton>
#include <QAction>
#include <QTimer>

#include "QtnDockWindow.h"
#include "QtnDockTitleBar.h"
#include "QtnDockLayoutPrivate.h"
#include "QtnDockLayoutPanelPrivate.h"
#include "QtnDockPanelManagerPrivate.h"

QTITAN_BEGIN_NAMESPACE

/* DockWindowLayout */
class DockWindowLayout : public QLayout
{
    Q_OBJECT
public:
    enum Role { Content, TitleBar, RoleCount };
    explicit DockWindowLayout(QWidget* parent);
    virtual ~DockWindowLayout();
public:
    virtual void addItem(QLayoutItem* item);
    virtual QLayoutItem* itemAt(int index) const;
    virtual QLayoutItem* takeAt(int index);
    virtual int count() const;

    virtual QSize maximumSize() const;
    virtual QSize minimumSize() const;
    virtual QSize sizeHint() const;
    virtual void setGeometry(const QRect&);
public:
    void updateGeometry();
    QWidget* widgetForRole(Role r) const;
    void setWidgetForRole(Role r, QWidget* w);
    QRect titleArea() const { return m_titleArea; }
public:
    bool m_verticalTitleBar;
private:
    QVector<QLayoutItem*> m_itemList;
    QRect m_titleArea;
private:
    Q_DISABLE_COPY(DockWindowLayout)
};

class DockDocumentLayoutPanel;
/* DockWindowPrivate */
class DockWindowPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(DockWindow)
public:
    explicit DockWindowPrivate(DockLayoutEngine* layoutEngine);
    virtual ~DockWindowPrivate();
    void checkToFinalize();
public:
    inline static DockWindowPrivate* _get(DockWindow* panel)
    { return &panel->qtn_d(); }
    inline static const DockWindowPrivate* _get(const DockWindow* panel)
    { return &panel->qtn_d(); }
public:
    void initialization();
    DockLayoutEngine* layoutEngine() const { return m_layoutEngine; }
    DockPanelManager* dockManager() const { return m_layoutEngine->dockManager(); }

    void setRootPanel(DockSplitLayoutPanel* panel);
    void showOnScreen(const QRect& rect);
    void updatePosition();
    void updateButtons();
    bool isThemed() const;
#ifdef Q_OS_WIN
    bool checkSizeConstraint();
#endif // Q_OS_WIN
    void minMaxInfo(qtnMinMaxSize* minmaxSize) const;
    DockWidgetPanel* selectedPanel() const;
    QWidget* titleBarWrapper() const;
public Q_SLOTS:
    void closeDockPanel();
public:
    virtual void focusChanged();
    virtual void childLayoutChanged();
    virtual void updatePanel(bool activateChanged);
public:
    DockLayoutEngine* m_layoutEngine;
    DockLayoutPanel* m_rootPanel;
    DockWindowLayout* m_internalLayout;
    QAction* m_actCloseButton;
    QRect m_assignRect;
    QSize m_sizeDocking;
    bool m_active;
    bool m_hasGeometrySet;
private:
    Q_DISABLE_COPY(DockWindowPrivate)
};

inline void DockWindowPrivate::setRootPanel(DockSplitLayoutPanel* layoutPanel)
{
    QTN_P(DockWindow);
    Q_ASSERT(layoutPanel != Q_NULL && (layoutPanel->layoutType() == DockLayoutPanel::Split || layoutPanel->layoutType() == DockLayoutPanel::Document));
    m_internalLayout->setWidgetForRole(DockWindowLayout::Content, layoutPanel);
    layoutPanel->setParentPanel(Q_NULL);
    layoutPanel->setDockWindow(&p);
    m_rootPanel = layoutPanel;
}

/* DockAutoHideWindowPrivate */
class DockAutoHideWindowPrivate
{
    QTN_DECLARE_PUBLIC(DockAutoHideWindow)
public:
    explicit DockAutoHideWindowPrivate();
    virtual ~DockAutoHideWindowPrivate();
public:
    void init(DockAutoHideLayoutPanel* panel, DockTabbedLayoutPanel* tabbedWidget);
public:
    static qreal m_animationDelay;
    static int m_animationInterval;
    static int m_animationDuration;
    static uint m_inactiveCollapseDelay;

    DockTabbedLayoutPanel* m_tabbedPanel;
    DockAutoHideLayoutPanel* m_autoHidePanel;
    Qtitan::LayoutArea m_layoutArea;

    QTimer m_timerSlidein;
    QTimer m_timerCheckActive;
    QTimer m_timerSlideout;
    QTimer m_closeTimer;
    QCursor m_defaultCursor;

    QRect m_rc;
    QRect m_rcCatch; 
    QRect m_rcClient;

    int m_slideStep;
    int m_stepsCount;
    int m_deactivationCount;

    bool m_slideOut : 1;
    bool m_horiz : 1;
    bool m_tracking : 1;
private:
    Q_DISABLE_COPY(DockAutoHideWindowPrivate)
};

class DockAutoHideWindowPrivate;
/* DockAutoHideWindow */
class DockAutoHideWindow : public DockWindowBase
{
    Q_OBJECT
private:
    explicit DockAutoHideWindow(DockAutoHideLayoutPanel* autoHidePanel, DockTabbedLayoutPanel* tabbedPanel);
    virtual ~DockAutoHideWindow();
public:
    void updateLayout();
private:
    void closeWindow();
    DockPanelManager* dockManager() const;
    void calcMinMaxInfo(qtnMinMaxSize& minmaxSize, bool includeSplitter = true) const;
    void recalcLayout();
    void slideStep();
    void reposition(QRect rc, QRect rcClient);
    bool allowedRect(QRect& rcAvail, QRect& rc) const;
    bool isSlideOut() const;
protected Q_SLOTS:
    void closeTimerTimeout();
    void timerSlidein();
    void timerSlideout();
    void timerCheckActive();
protected:
    void startCloseTimer();
    void clearWidgetTabbed();
    DockTabbedLayoutPanel* widgetTabbed() const;
    DockAutoHideLayoutPanel* widgetAutoHide() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    QRect m_rcWindow;
private:
    QTN_DECLARE_PRIVATE(DockAutoHideWindow);
    Q_DISABLE_COPY(DockAutoHideWindow);
    friend class DockAutoHideLayoutPanel;
    friend class DockAutoHideLayoutPanelPrivate;
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKWINDOWPRIVATE_H

