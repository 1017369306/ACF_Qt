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

#ifndef QTN_DOCKWIDGETPANELPRIVATE_H
#define QTN_DOCKWIDGETPANELPRIVATE_H

#include <QUuid>
#include <QLayout>
#include <QAbstractButton>
#include "QtnDockPanelManager.h"

QTITAN_BEGIN_NAMESPACE

struct qtnMinMaxSize
{
    qtnMinMaxSize() 
        : minTrackSize(0, 0)
        , maxTrackSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX) 
    {}
    QSize minTrackSize;
    QSize maxTrackSize;
};

class UpdatesEnabledBlocker
{
public:
    explicit UpdatesEnabledBlocker(QWidget* w) : m_widget(w)
    { m_widget->setUpdatesEnabled(false); }
    ~UpdatesEnabledBlocker()
    { m_widget->setUpdatesEnabled(true); }
private:
    QWidget* m_widget;
    Q_DISABLE_COPY(UpdatesEnabledBlocker);
};

class DockLayoutEngine;
/* DockPanelBasePrivate */
class DockPanelBasePrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(DockPanelBase)
public:
    explicit DockPanelBasePrivate(DockLayoutEngine* layoutEngine);
    virtual ~DockPanelBasePrivate();
    inline static DockPanelBasePrivate* _get(DockPanelBase* panel);
    inline static const DockPanelBasePrivate* _get(const DockPanelBase* panel);
public:
    virtual bool isEmpty() const;
    virtual bool isAutoHide() const { return false; }
    virtual int containsPanel(DockPanelBase* panelBase) const;
    virtual void getMinMaxData(qtnMinMaxSize& minmaxSize) const;
    virtual void updatePanel(bool activateChanged);
    virtual void setPanelGeometry(const QRect& rect);
    DockLayoutEngine* layoutEngine() const;
    DockPanelManager* dockManager() const;
protected:
    virtual void init();
    virtual void setPanelId(const QString& id);
protected Q_SLOTS:
    void panelObjectNameChanged(const QString& objectName);
protected:
    DockLayoutEngine* m_layoutEngine;
    DockPanelBase* m_parentPanel;
    DockWindowBase* m_dockWindow;
    QString m_id;
public:
    QSize m_sizeDocking;
    int m_length;
private:
    friend class DockSplitLayoutPanelPrivate;
    Q_DISABLE_COPY(DockPanelBasePrivate)
};

inline DockPanelBasePrivate* DockPanelBasePrivate::_get(DockPanelBase* panel)
{ return &panel->qtn_d(); }

inline const DockPanelBasePrivate* DockPanelBasePrivate::_get(const DockPanelBase* panel)
{ return &panel->qtn_d(); }

struct DockPanelInfo
{
    DockPanelInfo(bool _document) : document(_document) {
        clear();
    }
    
    void copyFrom(const DockPanelInfo& other) {
        document = other.document;
        lastOwner = other.lastOwner;
        dockingOwner = other.dockingOwner;
        floatingOwner = other.floatingOwner; 
    }
    
    void clear() { 
        lastOwner = Q_NULL;
        dockingOwner = Q_NULL;
        floatingOwner = Q_NULL;
    }
    bool document;
    DockPanelBase* floatingOwner;
    DockPanelBase* dockingOwner;
    DockPanelBase* lastOwner;
};

/* DockWidgetPanelPrivate */
class DockWidgetPanelPrivate : public DockPanelBasePrivate
{
    Q_OBJECT
public:
    QTN_DECLARE_EX_PUBLIC(DockWidgetPanel)
public:
    explicit DockWidgetPanelPrivate(DockPanelManager* manager);
    virtual ~DockWidgetPanelPrivate();
public:
    inline static DockWidgetPanelPrivate* _get(DockWidgetPanel* panel);
public:
    void addActionToTitleBar();
    QWidget* lastOwner(bool floating);
    void mousePress(const QPoint& pos, DockPanelBase* dragPanel);
    void mouseMove(const QPoint& pos);
    void mouseRelease();
    DockPanelInfo& panelInfo() const;
    const QRect& dragFrameRect() const {return m_dragFrameRect; }
    void setDragFrameRect(const QRect& rect) { m_dragFrameRect = rect; }
public:
    virtual bool isAutoHide() const;
    virtual void getMinMaxData(qtnMinMaxSize& minmaxSize) const;
    virtual Qtitan::DockPanelAreas allowedAreas() const;
protected:
    void init() override;
    void setPanelId(const QString& id) override;
    void idNeeded();
    void setLayoutEngine(DockLayoutEngine* layoutEngine);
public Q_SLOTS:
    void updateActions();
    void checkActions();
    void autoHideChanged(bool);
    void floatingChanged(bool);
    void dockableChanged(bool);
    void hideChanged(bool);
    void visibleChanged(bool);
private:
    QAction* m_actCloseButton;
    QAction* m_actMenuButton;
    QAction* m_actAutoButton;
    QAction* m_actAutoHide;
    QAction* m_actFloat;
    QAction* m_actDock;
    QAction* m_actHide;
    QAction* m_actVisible;

    QMenu* m_menuButton;
    QIcon m_icon;
    QString m_caption;
    int m_frameStyle;
    DockWidgetPanel::DockPanelFeatures m_features;
    Qtitan::DockPanelAreas m_allowedAreas;
    QRect m_dragFrameRect;
    QPoint m_pressedPos;
    DockPanelBase* m_dragPanel;
private:
    Q_DISABLE_COPY(DockWidgetPanelPrivate)
    friend class DockLayoutEngine;
    friend class DockPanelManagerPrivate;
};

inline DockWidgetPanelPrivate* DockWidgetPanelPrivate::_get(DockWidgetPanel* panel)
{ return &panel->qtn_d(); }


/* TitleBarWrapper */
class TitleBarWrapper : public QWidget
{
    Q_OBJECT
public:
    TitleBarWrapper(DockPanelManager* panelManager, QWidget* parent = Q_NULL);
    virtual ~TitleBarWrapper();
public:
    QWidget* titleBar() const { return m_titleBar; }
    void setTitleBar(QWidget* titleBar);
#ifdef Q_OS_LINUX //See QtnPlatform_linux : qtn_X11EmulateButtonRelease(w, pos);
    static bool isNativeMouseButtons_linux();
#endif
public:
    virtual QSize sizeHint() const { return minimumSizeHint(); }
    virtual QSize minimumSizeHint() const;
protected:
    bool handleMousePressEvent(QMouseEvent* event);
    bool handleMouseDoubleClickEvent(QMouseEvent* event);
protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
protected:
    QWidget* m_titleBar;
    DockPanelManager* m_panelManager;
private:
    Q_DISABLE_COPY(TitleBarWrapper)
};

/* DockWidgetPanelLayout */
class DockWidgetPanelLayout : public QLayout
{
    Q_OBJECT
public:
    enum Role { Content, TitleBar, RoleCount };
public:
    DockWidgetPanelLayout(QWidget* parent = Q_NULL);
    ~DockWidgetPanelLayout();
public:
    virtual void addItem(QLayoutItem* item);
    virtual QLayoutItem* itemAt(int index) const;
    virtual QLayoutItem* takeAt(int index);
    virtual int count() const;
public:
    virtual QSize maximumSize() const;
    virtual QSize minimumSize() const;
    virtual QSize sizeHint() const;
    virtual void setGeometry(const QRect& r);

    QSize sizeFromContent(const QSize& content) const;
    QWidget* widgetForRole(Role r) const;
    QLayoutItem* itemForRole(Role r) const;
    void setWidgetForRole(Role r, QWidget* w);

    int titleHeight() const;
    int minimumTitleWidth() const;
private:
    QVector<QLayoutItem*> m_itemList;
    bool m_verticalTitleBar;
private:
    Q_DISABLE_COPY(DockWidgetPanelLayout)
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKWIDGETPANELPRIVATE_H
