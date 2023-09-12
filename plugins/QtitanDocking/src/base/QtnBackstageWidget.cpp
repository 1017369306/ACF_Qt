/****************************************************************************
**
** Qtitan Library by Developer Machines (Components for Qt.C++)
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

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QAction>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#ifdef QTN_PRIVATE_HEADERS
#include <private/qwidget_p.h>
#include <private/qgraphicsitem_p.h>
#include <private/qapplication_p.h>
#endif
#include "QtnPlatform.h"
#include "QtnBackstageWidget.h"
#ifdef QTN_SHARED_BACKSTAGE_WINDOW
#include <QAccessibleObject>
#endif
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

#ifndef QT_STATIC
#ifdef _MSC_VER
static
#endif
bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event)
{
    return QCoreApplication::sendSpontaneousEvent(receiver, event);
}
#else
extern bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event);
#endif

QTITAN_BEGIN_NAMESPACE

static void qtn_activateScene(QGraphicsScene* scene)
{
    if (scene == Q_NULL || scene->isActive())
        return;
    QEvent event(QEvent::WindowActivate);
    qt_sendSpontaneousEvent(scene, &event);
}

static void qtn_deactivateScene(QGraphicsScene* scene)
{
    if (scene == Q_NULL || !scene->isActive())
        return;
    QEvent event(QEvent::WindowDeactivate);
    qt_sendSpontaneousEvent(scene, &event);
}

#ifdef QTN_SHARED_BACKSTAGE_WINDOW
class BackstageSharedWindow;

class QGraphicsSceneEx : public QGraphicsScene
{
public:
    QGraphicsSceneEx(BackstageSharedWindow* sharedWindow);
protected:
    virtual bool event(QEvent* e);
private:
    BackstageSharedWindow* m_sharedWindow;
    int m_processDirtyItemsIndex;
};

#ifdef Q_OS_LINUX
#define QTN_SHAREDWINDOW_PROPERTY "qtn_sharedWindow"
class QGraphicsSceneExAccessible : public QAccessibleObject
{
public:
    QGraphicsSceneExAccessible(QObject* obj) : QAccessibleObject(obj) {}
    virtual bool isValid() const { return true; }
    virtual QAccessibleInterface *parent() const { return QAccessible::queryAccessibleInterface(object()->parent()); }
    virtual QAccessibleInterface *child(int) const { return Q_NULL; }
    virtual int childCount() const { return -1; }
    virtual int indexOfChild(const QAccessibleInterface *) const { return -1; }
    virtual QString text(QAccessible::Text) const { return object()->objectName(); }
    virtual QAccessible::Role role() const { return QAccessible::Grouping; }
    virtual QAccessible::State state() const { return QAccessible::State(); }
    static QAccessibleInterface* factory(const QString& key, QObject* object)
    {
        Q_UNUSED(key)
        if (qobject_cast<QGraphicsScene*>(object) == Q_NULL || !object->property(QTN_SHAREDWINDOW_PROPERTY).toBool())
            return Q_NULL;
        return new QGraphicsSceneExAccessible(object);
    }
    static bool installFactory()
    {
        static bool _installed = (QAccessible::installFactory(factory), true);
        return _installed;
    }
};
#endif

class BackstageSharedWindow : public QWidget
{
public:
    BackstageSharedWindow(QWidget* window);
    virtual ~BackstageSharedWindow();
    void addWidget(BackstageWidget* backstageWidget);
    void removeWidget(BackstageWidget* backstageWidget);
    bool containsWidget(BackstageWidget* backstageWidget) const;
    bool isEmpty() const;
    QGraphicsScene* scene() const;
    void ensureSize(const QSize& size);
public:
    typedef QHash<QWidget *, BackstageSharedWindow *> SharedWindowHash;
    static SharedWindowHash* allBackstageWindows;
    static bool setWindow(BackstageWidget* backstageWidget);
    static void resetWindow(BackstageWidget* backstageWidget);
    static void deleteNotUsed();
    void updateRequest(bool qwidgetupdate);
protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject* object, QEvent* event) override;
private:
    void finalize();
    QGraphicsSceneEx m_scene;
    QSet<BackstageWidget*> m_widgets;
    QGraphicsProxyWidget* m_windowProxyWidget;
};
#endif
QTITAN_END_NAMESPACE

#ifdef QTN_SHARED_BACKSTAGE_WINDOW
/* BackstageSharedWindow */
BackstageSharedWindow::SharedWindowHash* BackstageSharedWindow::allBackstageWindows = Q_NULL;
bool BackstageSharedWindow::setWindow(BackstageWidget* backstageWidget)
{
    QWidget* window = backstageWidget->viewWidget()->window();
    Q_ASSERT(window != Q_NULL);
    BackstageSharedWindow* backstageWindow = Q_NULL;
    if (allBackstageWindows != Q_NULL)
    {
        if (allBackstageWindows->contains(window))
            backstageWindow = allBackstageWindows->value(window);
    }

    if (backstageWidget->m_backstageWindow != Q_NULL)
    {
        if (backstageWidget->m_backstageWindow == backstageWindow)
            return false;
    }

    if (backstageWindow == Q_NULL)
        backstageWindow = new BackstageSharedWindow(window);

    if (backstageWidget->m_backstageWindow != Q_NULL)
        resetWindow(backstageWidget);
    Q_ASSERT(backstageWidget->m_backstageWindow == Q_NULL);
  
    backstageWindow->addWidget(backstageWidget);
    backstageWidget->m_backstageWindow = backstageWindow;
    return true;
}

void BackstageSharedWindow::resetWindow(BackstageWidget* backstageWidget)
{
    if (backstageWidget->m_backstageWindow != Q_NULL)
    {
        backstageWidget->m_backstageWindow->removeWidget(backstageWidget);
        if (backstageWidget->m_backstageWindow->isEmpty())
        {
            backstageWidget->m_backstageWindow->finalize();
            backstageWidget->m_backstageWindow->deleteLater();
        }
    }
    backstageWidget->m_backstageWindow = Q_NULL;
}

void BackstageSharedWindow::deleteNotUsed()
{
    while (allBackstageWindows != Q_NULL)
    {
        bool hasEmpty = false;

        for (SharedWindowHash::iterator it = allBackstageWindows->begin();
            it != allBackstageWindows->end(); ++it)
        {
            BackstageSharedWindow* w = it.value();
            if (w->isEmpty())
            {
                delete w;
                hasEmpty = true;
                break;
            }
        }
        if (!hasEmpty)
            break;
    }
}

BackstageSharedWindow::BackstageSharedWindow(QWidget* window)
    : QWidget(), m_scene(this), m_windowProxyWidget(Q_NULL)
{
    //Means all toplevel/popup child windows will be created natively.
    setWindowFlags(windowFlags() | Qt::BypassGraphicsProxyWidget);
    setGeometry(0, 0, 400, 200);
    m_scene.setSceneRect(0, 0, 400, 200);
    m_windowProxyWidget = m_scene.addWidget(this);
    qtn_activateScene(&m_scene);
    if (allBackstageWindows == Q_NULL)
        allBackstageWindows = new SharedWindowHash();
    allBackstageWindows->insert(window, this);
    window->installEventFilter(this);
    setVisible(window->isVisible());
    if (window->isActiveWindow())
        qtn_activateScene(&m_scene);
    else
        qtn_deactivateScene(&m_scene);
}

void BackstageSharedWindow::finalize()
{
    if (m_windowProxyWidget != Q_NULL)
        m_windowProxyWidget->setWidget(Q_NULL);
    Q_DELETE_AND_NULL(m_windowProxyWidget);

    Q_ASSERT(isEmpty());

    if (allBackstageWindows != Q_NULL)
    {
        for (SharedWindowHash::iterator it = allBackstageWindows->begin();
            it != allBackstageWindows->end(); ++it)
        {
            if (it.value() == this)
            {
                it.key()->removeEventFilter(this);
                allBackstageWindows->erase(it);
                break;
            }
        }
        if (allBackstageWindows->size() == 0)
            Q_DELETE_AND_NULL(allBackstageWindows);
    }
}

BackstageSharedWindow::~BackstageSharedWindow()
{
    finalize();
}

void BackstageSharedWindow::addWidget(BackstageWidget* backstageWidget)
{
    m_widgets.insert(backstageWidget);

    QWidget* w = backstageWidget->widget();
    Q_ASSERT(w != Q_NULL);
    if (w == Q_NULL)
        return;
    if (backstageWidget->m_proxyWidget != Q_NULL)
    {
        Q_ASSERT(false);
        backstageWidget->m_proxyWidget->setWidget(Q_NULL);
        Q_DELETE_AND_NULL(backstageWidget->m_proxyWidget);
    }
    w->setParent(this);
    backstageWidget->m_proxyWidget = m_windowProxyWidget->createProxyForChildWidget(w);
}

void BackstageSharedWindow::removeWidget(BackstageWidget* backstageWidget)
{
    if (!containsWidget(backstageWidget))
    {
        Q_ASSERT(false);
        return;
    }
    m_widgets.remove(backstageWidget);
    
    if (backstageWidget->m_proxyWidget != Q_NULL)
    {
        backstageWidget->m_proxyWidget->setWidget(Q_NULL);
        Q_DELETE_AND_NULL(backstageWidget->m_proxyWidget);
    }

    if (backstageWidget->widget() != Q_NULL)
        backstageWidget->widget()->setParent(Q_NULL);
}

bool BackstageSharedWindow::containsWidget(BackstageWidget* backstageWidget) const
{
    return m_widgets.contains(backstageWidget);
}

bool BackstageSharedWindow::isEmpty() const
{
    return m_widgets.isEmpty();
}

QGraphicsScene* BackstageSharedWindow::scene() const
{
    BackstageSharedWindow* w = const_cast<BackstageSharedWindow *>(this);
    return &w->m_scene;
}

void BackstageSharedWindow::ensureSize(const QSize& size)
{
    QSize oldSize = this->size();
    QSize s(qMax(oldSize.width(), size.width()), qMax(oldSize.height(), size.height()));
    if (oldSize != s)
        resize(s);
}

static void qtn_add_dirty_region(QWidget* w, QRegion& region, const QPoint& offset)
{
    if (!w->isVisible())
        return;
    QWidgetPrivate* p = qt_widget_private(w);
    if (p->inDirtyList)
    {
        Q_ASSERT(!p->dirty.isEmpty());
        region += p->dirty.translated(offset);
    }
    else if (p->dirtyOpaqueChildren || p->isScrolled || p->isMoved)
    {
        region += w->rect().translated(offset);
        return;
    }

    for (QObjectList::const_iterator it = w->children().constBegin(); 
        it != w->children().constEnd(); ++it)
    {
        if (!(*it)->isWidgetType())
            continue;
        QWidget* w = static_cast<QWidget *>(*it);
        qtn_add_dirty_region(w, region, offset + w->geometry().topLeft());
    }
}

void BackstageSharedWindow::updateRequest(bool qwidgetupdate)
{
    for (QSet<BackstageWidget*>::const_iterator it = m_widgets.constBegin();
        it != m_widgets.constEnd(); ++it)
    {
        BackstageWidget* backstageWidget = *it;
        QWidget* w = backstageWidget->widget();
        if (w == Q_NULL)
            continue;
        QRegion region = backstageWidget->m_needsRepaint;

        if (qwidgetupdate)
        {
            qtn_add_dirty_region(w, region, w->geometry().topLeft());
            backstageWidget->m_needsRepaint = region;
        }
        else
        {
            QGraphicsItemPrivate* item =
                QGraphicsItemPrivate::get(backstageWidget->m_proxyWidget);
            Q_ASSERT(item != Q_NULL);
            if (item != Q_NULL && item->dirty)
                region += item->needsRepaint.toRect();
            region -= backstageWidget->m_needsRepaint; //Already was painted
            backstageWidget->m_needsRepaint = QRegion();

        }

        if (region.isEmpty())
            continue;
        QList<QRectF> rects;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
        for (QRegion::const_iterator it = region.begin(); it != region.end(); ++it)
            rects.append(*it);
#else
        QVector<QRect> rcs = region.rects();
        for (QVector<QRect>::const_iterator it = rcs.constBegin(); it != rcs.constEnd(); ++it)
            rects.append(*it);
#endif
        backstageWidget->changed(rects);
    }
}

bool BackstageSharedWindow::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::UpdateRequest:
        {
            updateRequest(true);
        }
        break;
        default:
            break;
    }
    return QWidget::event(event);
}

bool BackstageSharedWindow::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Show || event->type() == QEvent::Hide)
        setVisible(event->type() == QEvent::Show);
    else if (event->type() == QEvent::Destroy)
    {
        while (m_widgets.size() > 0)
            BackstageSharedWindow::resetWindow(*m_widgets.begin());
        return false;
    }

    return QWidget::eventFilter(object, event);
}

/* BackstageWidgetBase */
BackstageWidgetBase::BackstageWidgetBase(QWidget* viewWidget, QObject* parent)
    : QObject(parent),
      m_viewWidget(viewWidget), m_backstageWindow(Q_NULL)
{
}

BackstageWidgetBase::~BackstageWidgetBase()
{
}

QWidget* BackstageWidgetBase::viewWidget() const
{
    return m_viewWidget;
}

/* QGraphicsSceneEx */
QGraphicsSceneEx::QGraphicsSceneEx(BackstageSharedWindow* sharedWindow)
    : QGraphicsScene(), m_sharedWindow(sharedWindow)
{
    m_processDirtyItemsIndex = metaObject()->indexOfSlot("_q_processDirtyItems()");
#ifdef Q_OS_LINUX
    setProperty(QTN_SHAREDWINDOW_PROPERTY, true);
    QGraphicsSceneExAccessible::installFactory();
#endif
}

bool QGraphicsSceneEx::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::MetaCall:
    {
        QMetaCallEvent *mce = static_cast<QMetaCallEvent*>(e);
        if (mce->id() == m_processDirtyItemsIndex)
            m_sharedWindow->updateRequest(false);
    }
    break;
    default:
        break;
    }
    return QGraphicsScene::event(e);
}

#endif

/* BackstageWidget */
BackstageWidget::BackstageWidget(QWidget* viewWidget, QObject* parent)
    : BackstageWidgetBase(viewWidget, parent),
    m_widget(Q_NULL),
    m_cursorWidget(Q_NULL),
    m_mousePressWidget(Q_NULL),
    m_proxyWidget(Q_NULL),
    m_mousePressButtons(Qt::NoButton),
    m_mousePressButton(Qt::NoButton)
#ifdef QTN_SHARED_BACKSTAGE_WINDOW    
    , m_overWidget(Q_NULL) 
#else
#endif
{
}

BackstageWidget::~BackstageWidget()
{
    disconnectWidget();
    Q_DELETE_AND_NULL(m_widget);
#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    BackstageSharedWindow::deleteNotUsed();
#endif
}

QWidget* BackstageWidget::widget() const
{
    return m_widget;
}

void BackstageWidget::setWidget(QWidget* widget, const QRect& geometry)
{
    if (m_widget == widget)
        return;
    if (m_widget != Q_NULL)
        disconnectWidget();
    m_widget = widget;
    connectWidget(geometry);
}

void BackstageWidget::syncToView()
{
    QRect r = QRect(m_margins.left(), m_margins.top(),
        m_geometry.size().width() - (m_margins.left() + m_margins.right()),
        m_geometry.size().height() - (m_margins.top() + m_margins.bottom()));
    r.setWidth(qMax(r.width(), 0));
    r.setHeight(qMax(r.height(), 0));

#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    scene()->setSceneRect(m_geometry);
    const bool _changed = false;
#else
    const bool _changed = BackstageSharedWindow::setWindow(this);
    m_backstageWindow->ensureSize(r.size());
#endif

    if (m_widget->geometry() != r || _changed)
    {
        m_widget->setGeometry(r);
        if (r.width() == 0 || r.height() == 0)
        {
            //#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            m_widget->setVisible(false);
            m_widget->setAttribute(Qt::WA_Mapped, false);
            //#endif
        }
        else
        {
            //#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            m_widget->setVisible(true);
            m_widget->setAttribute(Qt::WA_Mapped, true);
            //#endif
        }
    }
}

QGraphicsScene* BackstageWidget::scene() const
{
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    return const_cast<BackstageWidget *>(this);
#else
    return m_backstageWindow != Q_NULL ? m_backstageWindow->scene() : Q_NULL;
#endif
}

void BackstageWidget::widgetDestroyed()
{
    m_widget = Q_NULL;
    disconnectWidget();
}

void BackstageWidget::connectWidget(const QRect& geometry)
{
    if (m_widget == Q_NULL)
        return;
    connect(m_widget, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    m_widget->setWindowFlags(m_widget->windowFlags() | Qt::BypassGraphicsProxyWidget);  //Means all toplevel/popup child windows will be created natively.
    Q_ASSERT(m_proxyWidget == Q_NULL);
    m_proxyWidget = addWidget(m_widget);
    m_proxyWidget->setMinimumSize(0, 0);
#endif
    if (!geometry.isEmpty())
        setGeometry(geometry);
    else
        setGeometry(m_widget->rect());
}

void BackstageWidget::disconnectWidget()
{
    if (m_widget != Q_NULL)
        disconnect(m_widget, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
    m_cursorWidget = Q_NULL;
    m_mousePressWidget = Q_NULL;
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    if (m_proxyWidget == Q_NULL)
        return;
    m_proxyWidget->setWidget(Q_NULL);
    Q_DELETE_AND_NULL(m_proxyWidget);
#else
    BackstageSharedWindow::resetWindow(this);
    Q_ASSERT(m_proxyWidget == Q_NULL && m_backstageWindow == Q_NULL);
#endif
}

void BackstageWidget::checkCursor(const QPoint& pos)
{
    m_cursorWidget = Q_NULL;
    QWidget* w = findWidgetByPos(pos);
    if (w == Q_NULL)
        return;
#if 0
    QWidget* w = m_widget != Q_NULL ? m_widget->childAt(pos - QPoint(m_margins.left(), m_margins.top())) : Q_NULL;
    if (w == Q_NULL && m_widget != Q_NULL && m_widget->underMouse())
        w = m_widget;
#endif

    while (w)
    {
        if (w->testAttribute(Qt::WA_SetCursor))
        {
            m_cursorWidget = w;
            break;
        }
        w = w->parentWidget();
    }
}

void BackstageWidget::activate()
{
    qtn_activateScene(scene());
}

void BackstageWidget::deactivate()
{
    qtn_deactivateScene(scene());
}

void BackstageWidget::setMargins(int left, int top, int right, int bottom)
{
    setMargins(QMargins(left, top, right, bottom));
}

const QMargins& BackstageWidget::margins() const
{
    return m_margins;
}

bool BackstageWidget::hasFocusItem()
{
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    return scene()->focusItem() != Q_NULL;
#else
    return m_widget != Q_NULL && m_widget->hasFocus();
#endif
}

bool BackstageWidget::hasMouseGrabber()
{
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    return scene()->mouseGrabberItem() != Q_NULL;
#else
    return false;
#endif
}

bool BackstageWidget::isInputMethodSupported() const
{
    if (widget() == Q_NULL)
        return false;
    return widget()->testAttribute(Qt::WA_InputMethodEnabled);
}

static QWidget* qtn_get_focus_widget(QWidget* w)
{
    if (w == Q_NULL)
        return Q_NULL;
    QWidget* retval = w->focusWidget();
    if (retval == Q_NULL)
        retval = w;
    return retval;
}

QVariant BackstageWidget::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (widget() == Q_NULL)
        return QVariant();
    QWidget* focusWidget = qtn_get_focus_widget(widget());
    return focusWidget->inputMethodQuery(query);
}

void BackstageWidget::setMargins(const QMargins& margins)
{
    if (m_margins == margins)
        return;
    m_margins = margins;
    setGeometry(QRect(QPoint(0, 0), geometry().size()));
}

void BackstageWidget::paintContent(QPainter& painter)
{
    if (m_widget == Q_NULL)
        return;
    syncToView();
    
    if (painter.opacity() == 0)
        return;

    QPoint offset = m_widget->geometry().topLeft();
    QRect clipRect = painter.hasClipping() ? painter.clipBoundingRect().toRect() :
        QRect(0, 0, painter.device()->width(), painter.device()->height());
    QRegion dirty = clipRect.translated(-offset).intersected(m_widget->rect());
    if (dirty.isEmpty())
        return;
    painter.save();
    m_widget->render(&painter, dirty.boundingRect().topLeft() + offset,
        dirty, QWidget::RenderFlags(QWidget::DrawChildren));
    painter.restore();

#if 0
    QRect rect(0, 0, scene()->width(), scene()->height());
    rect.adjust(m_margins.left(), m_margins.top(), -m_margins.right(), -m_margins.bottom());
    rect.translate(scene()->sceneRect().left(), scene()->sceneRect().top());

    QRect clipRect = painter.hasClipping() ? painter.clipBoundingRect().toRect() : 
        QRect(0, 0, painter.device()->width(), painter.device()->height());
    clipRect = clipRect.intersected(rect);

    if (clipRect.isEmpty())
        return;

    painter.save();

    painter.setClipRect(clipRect);
    QPoint offset(rect.left(), rect.top());
    painter.translate(offset);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    if (painter.opacity() == 1.0)
        painter.setOpacity(0.9999); //Hack to fix the flickering under Qt4.
#endif

#if 1
    QStyleOptionGraphicsItem option;
    option.exposedRect = QRect(0, 0, rect.width(), rect.height());
    m_proxyWidget->paint(&painter, &option, Q_NULL);
#else
    Q_ASSERT(false);
    m_scene.render(&painter, paintRect, paintRect);
#endif
    painter.restore();
#endif
}

QWidget* BackstageWidget::findWidgetByPos(const QPoint& pos)
{
    if (m_widget == Q_NULL)
        return Q_NULL;
    QRect geom = m_widget->geometry();
    if (!geom.contains(pos))
        return Q_NULL;
    QWidget* w = m_widget->childAt(pos - geom.topLeft());
    if (w == Q_NULL)
        w = m_widget;
    return w;
}

bool BackstageWidget::tryShortcut(QKeyEvent* e)
{
    if (e->key() == Qt::Key_unknown)
        return false;

    if (e->key() >= Qt::Key_Shift &&
        e->key() <= Qt::Key_Alt)
        return false;

    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return false;
    }

    QKeySequence keySequence(e->key() | e->modifiers());
    QList<QWidget*> lstWidget = m_widget->findChildren<QWidget*>();
    for (QList<QWidget*>::const_iterator it = lstWidget.constBegin(); it != lstWidget.constEnd(); ++it)
    {
        QList<QAction*> lstAction = (*it)->actions();
        for (QList<QAction*>::const_iterator itAct = lstAction.constBegin(); itAct != lstAction.constEnd(); ++itAct)
        {
            QAction* action = (*itAct);
            if (action->shortcutContext() == Qt::WindowShortcut &&
                action->isEnabled() && keySequence == action->shortcut())
            {
                action->trigger();
                e->setAccepted(true);
                return true;
            }
        }
    }
    return false;
}

void BackstageWidget::send_mouseOverEvent(const QPoint& pos)
{
    updateMousePressWidget(Q_NULL, Qt::NoButton);

    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;
    QPoint screenPos = QCursor::pos();
#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    QPointer<QWidget> w = findWidgetByPos(pos);
    QApplicationPrivate::dispatchEnterLeave(w, m_overWidget, screenPos);
    m_overWidget = w;
    checkCursor(pos);
#else
    QGraphicsSceneHoverEvent hoverEvent(QEvent::GraphicsSceneHoverEnter);
    hoverEvent.setScenePos(pos - scene()->sceneRect().topLeft());
    hoverEvent.setScreenPos(screenPos);
    hoverEvent.setLastScenePos(hoverEvent.scenePos());
    hoverEvent.setLastScreenPos(hoverEvent.screenPos());
    hoverEvent.setWidget(viewWidget());

    qt_sendSpontaneousEvent(scene(), &hoverEvent);

    checkCursor(hoverEvent.scenePos().toPoint());
#endif
}

void BackstageWidget::send_mouseOutEvent(const QPoint& pos)
{
    updateMousePressWidget(Q_NULL, Qt::NoButton);

    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;

#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    Q_UNUSED(pos);
    QApplicationPrivate::dispatchEnterLeave(Q_NULL, m_overWidget, QPoint(-1, -1));
    m_overWidget = Q_NULL;
    checkCursor(pos);
#else
    QPoint screenPos = QCursor::pos();

    if (scene()->mouseGrabberItem() != Q_NULL)
        scene()->mouseGrabberItem()->ungrabMouse();

    QGraphicsSceneHoverEvent hoverEvent(QEvent::GraphicsSceneHoverLeave);
    hoverEvent.setScenePos(pos - scene()->sceneRect().topLeft());
    hoverEvent.setScreenPos(screenPos);
    hoverEvent.setLastScenePos(hoverEvent.scenePos());
    hoverEvent.setLastScreenPos(hoverEvent.screenPos());

    hoverEvent.setWidget(viewWidget());

    qt_sendSpontaneousEvent(scene(), &hoverEvent);
#endif
}

void BackstageWidget::send_mouseMoveEvent(QMouseEvent* e)
{
    updateMousePressWidget(m_mousePressWidget, e->buttons());

    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    QPointer<QWidget> w = m_mousePressWidget != Q_NULL ?
        m_mousePressWidget : findWidgetByPos(e->pos());
    QApplicationPrivate::dispatchEnterLeave(w, m_overWidget, e->screenPos());
    m_overWidget = w;
    if (w != Q_NULL)
    {
        QPoint offset = w->mapTo(m_widget, m_widget->geometry().topLeft());
        MouseEventQt mouseEvent(*e, e->pos() - offset);
        qt_sendSpontaneousEvent(w, &mouseEvent);
        bool isAccepted = mouseEvent.isAccepted();
        e->setAccepted(isAccepted);
    }
    checkCursor(e->pos());
#else
    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
    mouseEvent.setWidget(viewWidget());
    mouseEvent.setButtonDownScenePos(m_mousePressButton, m_mousePressPosition);
    mouseEvent.setButtonDownScreenPos(m_mousePressButton, m_mousePressScreenPosition);
    mouseEvent.setScenePos(e->pos() - scene()->sceneRect().topLeft());
    mouseEvent.setScreenPos(e->globalPos());
    mouseEvent.setLastScenePos(m_lastMouseMoveScenePosition);
    mouseEvent.setLastScreenPos(m_lastMouseMoveScreenPosition);
    mouseEvent.setButtons(e->buttons());
    mouseEvent.setButton(e->button());
    mouseEvent.setModifiers(e->modifiers());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    mouseEvent.setSource(e->source());
    mouseEvent.setFlags(e->flags());
#endif
    m_lastMouseMoveScenePosition = mouseEvent.scenePos();
    m_lastMouseMoveScreenPosition = mouseEvent.screenPos();
    mouseEvent.setAccepted(false);

    qt_sendSpontaneousEvent(scene(), &mouseEvent);

    bool isAccepted = mouseEvent.isAccepted();
    e->setAccepted(isAccepted);
    checkCursor(mouseEvent.scenePos().toPoint());
#endif
}

void BackstageWidget::updateMousePressWidget(QWidget* w, Qt::MouseButtons buttons)
{
    if (m_mousePressWidget == Q_NULL)
    {
        m_mousePressWidget = w;
        m_mousePressButtons = w != Q_NULL ? buttons : Qt::NoButton;
        return;
    }

    if (m_mousePressWidget != w || m_mousePressButtons != buttons)
    {
        m_mousePressWidget = Q_NULL;
        m_mousePressButtons = Qt::NoButton;
        return;
    }
}

void BackstageWidget::send_mousePressEvent(QMouseEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
    setGlobalPos(e->globalPos() + geometry().topLeft() - e->pos());


#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    QWidget* w = findWidgetByPos(e->pos());
    if (w == Q_NULL)
        return;
    
    updateMousePressWidget(w, e->buttons());

    QPoint offset = w->mapTo(m_widget, m_widget->geometry().topLeft());

    MouseEventQt mouseEvent(*e, e->pos() - offset);
    mouseEvent.accept();
    qt_sendSpontaneousEvent(w, &mouseEvent);
    bool isAccepted = mouseEvent.isAccepted();
    e->setAccepted(isAccepted);
#else
    m_mousePressButton = e->button();
    m_mousePressPosition = e->pos() - scene()->sceneRect().topLeft();
    m_mousePressScreenPosition = e->globalPos();
    m_lastMouseMoveScenePosition = m_mousePressPosition;
    m_lastMouseMoveScreenPosition = m_mousePressScreenPosition;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
    mouseEvent.setWidget(viewWidget());
    mouseEvent.setButtonDownScenePos(m_mousePressButton, m_mousePressPosition);
    mouseEvent.setButtonDownScreenPos(m_mousePressButton, m_mousePressScreenPosition);
    mouseEvent.setScenePos(m_mousePressPosition);
    mouseEvent.setScreenPos(m_mousePressScreenPosition);
    mouseEvent.setLastScenePos(m_lastMouseMoveScenePosition);
    mouseEvent.setLastScreenPos(m_lastMouseMoveScreenPosition);
    mouseEvent.setButtons(e->buttons());
    mouseEvent.setButton(e->button());
    mouseEvent.setModifiers(e->modifiers());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    mouseEvent.setSource(e->source());
    mouseEvent.setFlags(e->flags());
#endif
    mouseEvent.setAccepted(false);
    qt_sendSpontaneousEvent(scene(), &mouseEvent);
    bool isAccepted = mouseEvent.isAccepted();
    e->setAccepted(isAccepted);
#endif
}

void BackstageWidget::send_mouseReleaseEvent(QMouseEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        updateMousePressWidget(Q_NULL, Qt::NoButton);
        e->ignore();
        return;
    }

    setGlobalPos(e->globalPos() + geometry().topLeft() - e->pos());

#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    QPointer<QWidget> widgetUnderMouse = findWidgetByPos(e->pos());
    QPointer<QWidget> w = m_mousePressWidget != Q_NULL ?
        m_mousePressWidget : widgetUnderMouse.data();
    if (w != Q_NULL)
    {
        QPoint offset = w->mapTo(m_widget, m_widget->geometry().topLeft());
        MouseEventQt mouseEvent(*e, e->pos() - offset);
        mouseEvent.accept();
        qt_sendSpontaneousEvent(w, &mouseEvent);
        bool isAccepted = mouseEvent.isAccepted();
        e->setAccepted(isAccepted);
    }

    w = widgetUnderMouse;
    QApplicationPrivate::dispatchEnterLeave(w, m_overWidget, e->screenPos());
    m_overWidget = w;
    checkCursor(e->pos());
#else
    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
    mouseEvent.setWidget(viewWidget());
    mouseEvent.setButtonDownScenePos(m_mousePressButton, m_mousePressPosition);
    mouseEvent.setButtonDownScreenPos(m_mousePressButton, m_mousePressScreenPosition);
    mouseEvent.setScenePos(e->pos() - scene()->sceneRect().topLeft());
    mouseEvent.setScreenPos(e->globalPos());
    mouseEvent.setLastScenePos(m_lastMouseMoveScenePosition);
    mouseEvent.setLastScreenPos(m_lastMouseMoveScreenPosition);
    mouseEvent.setButtons(e->buttons());
    mouseEvent.setButton(e->button());
    mouseEvent.setModifiers(e->modifiers());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    mouseEvent.setSource(e->source());
    mouseEvent.setFlags(e->flags());
#endif
    mouseEvent.setAccepted(false);

    qt_sendSpontaneousEvent(scene(), &mouseEvent);

    bool isAccepted = mouseEvent.isAccepted();
    e->setAccepted(isAccepted);
#endif
}

void BackstageWidget::send_mouseDoubleClickEvent(QMouseEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }

    setGlobalPos(e->globalPos() + geometry().topLeft() - e->pos());

#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    QWidget* w = findWidgetByPos(e->pos());
    if (w == Q_NULL)
        return;
    QPoint offset = w->mapTo(m_widget, m_widget->geometry().topLeft());

    MouseEventQt mouseEvent(*e, e->pos() - offset);
    mouseEvent.accept();
    qt_sendSpontaneousEvent(w, &mouseEvent);
    bool isAccepted = mouseEvent.isAccepted();
    e->setAccepted(isAccepted);
#else
    m_mousePressButton = e->button();
    m_mousePressPosition = e->pos() - scene()->sceneRect().topLeft();
    m_mousePressScreenPosition = e->globalPos();
    m_lastMouseMoveScenePosition = m_mousePressPosition;
    m_lastMouseMoveScreenPosition = m_mousePressScreenPosition;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
    mouseEvent.setWidget(viewWidget());

    mouseEvent.setButtonDownScenePos(m_mousePressButton, m_mousePressPosition);
    mouseEvent.setButtonDownScreenPos(m_mousePressButton, m_mousePressScreenPosition);
    mouseEvent.setScenePos(m_mousePressPosition);
    mouseEvent.setScreenPos(m_mousePressScreenPosition);
    mouseEvent.setLastScenePos(m_lastMouseMoveScenePosition);
    mouseEvent.setLastScreenPos(m_lastMouseMoveScreenPosition);
    mouseEvent.setButtons(e->buttons());
    mouseEvent.setButton(e->button());
    mouseEvent.setModifiers(e->modifiers());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    mouseEvent.setSource(e->source());
    mouseEvent.setFlags(e->flags());
#endif
    mouseEvent.setAccepted(false);

    qt_sendSpontaneousEvent(scene(), &mouseEvent);

    const bool isAccepted = mouseEvent.isAccepted();
    e->setAccepted(isAccepted);
#endif
}

void BackstageWidget::send_wheelEvent(QWheelEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
#ifdef QTN_SHARED_BACKSTAGE_WINDOW
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QPoint pos = e->position().toPoint();
#else
    QPoint pos = e->pos();
#endif

    QWidget* w = findWidgetByPos(pos);
    if (w == Q_NULL)
        return;
    QPoint offset = w->mapTo(m_widget, m_widget->geometry().topLeft());

    WheelEventQt wheelEvent(*e, pos - offset);
    wheelEvent.accept(); 
#if 0 //MACOSX bug
    qt_sendSpontaneousEvent(w, &wheelEvent);
#endif
    QCoreApplication::sendEvent(w, &wheelEvent);
    bool isAccepted = wheelEvent.isAccepted();
    e->setAccepted(isAccepted);
#else
    QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
    wheelEvent.setWidget(viewWidget());
    wheelEvent.setScenePos(e->pos() - geometry().topLeft());
    wheelEvent.setScreenPos(e->globalPos());
    wheelEvent.setButtons(e->buttons());
    wheelEvent.setModifiers(e->modifiers());
    wheelEvent.setDelta(e->delta());
    wheelEvent.setOrientation(e->orientation());
    wheelEvent.setAccepted(false);
    qt_sendSpontaneousEvent(scene(), &wheelEvent);
    e->setAccepted(wheelEvent.isAccepted());
#endif
}

void BackstageWidget::send_keyPressEvent(QKeyEvent* e, const QPoint& globalPosition)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }

    setGlobalPos(globalPosition);

    const bool keyTab = !(e->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) &&
         (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab
            /*|| (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier))*/);

    QWidget* oldfocus = m_widget->focusWidget();
    QWidget* w = keyTab ? widget() : qtn_get_focus_widget(widget());
    qt_sendSpontaneousEvent(w, e);
    if (keyTab && oldfocus == m_widget->focusWidget())
            e->ignore(); //focusNextPrevChild(bool next) actually returns false, 
                         //but event is accepted by default. We have to fix it. 
                         //See for details - QGraphicsScene::keyPressEvent(QKeyEvent *keyEvent)
}

void BackstageWidget::send_keyReleaseEvent(QKeyEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
    QWidget* w = qtn_get_focus_widget(widget());
    qt_sendSpontaneousEvent(w, e);
}

void BackstageWidget::send_shortcutEvent(QKeyEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }

    e->ignore();
    QWidget* w = qtn_get_focus_widget(widget());
    qt_sendSpontaneousEvent(w, e);
    if (!e->isAccepted())
        tryShortcut(e);
}

void BackstageWidget::send_shortcutOverrideEvent(QKeyEvent* e)
{
    send_shortcutEvent(e);
}

void BackstageWidget::send_inputMethodEvent(QInputMethodEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
    QWidget* w = qtn_get_focus_widget(widget());
    qt_sendSpontaneousEvent(w, e);
}

void BackstageWidget::send_focusInEvent(Qt::FocusReason reason)
{
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    activate();
#endif
    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;

    syncToView();
    QWidget* w = qtn_get_focus_widget(widget());
    if (w != Q_NULL)
        w->setFocus(reason);
}

void BackstageWidget::send_focusOutEvent(Qt::FocusReason reason)
{
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    deactivate();
#endif
    if (m_widget == Q_NULL || !m_widget->isEnabled())
        return;

    if (scene() != Q_NULL)
        scene()->setFocusItem(Q_NULL, reason);
}

void BackstageWidget::send_contextMenuEvent(QContextMenuEvent* e)
{
    if (m_widget == Q_NULL || !m_widget->isEnabled())
    {
        e->ignore();
        return;
    }
#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    QWidget* w = findWidgetByPos(e->pos());
    if (w == Q_NULL)
        return;
    QPoint offset = w->mapTo(m_widget, m_widget->geometry().topLeft());

    ContextMenuEventQt contextEvent(*e);
    contextEvent.accept();
    contextEvent.setPos(e->pos() - offset);
    qt_sendSpontaneousEvent(w, &contextEvent);
    e->setAccepted(contextEvent.isAccepted());
#else
    QGraphicsSceneContextMenuEvent contextEvent(QEvent::GraphicsSceneContextMenu);
    contextEvent.setWidget(viewWidget());
    contextEvent.setScenePos(e->pos() - geometry().topLeft());
    contextEvent.setScreenPos(e->globalPos());
    contextEvent.setModifiers(e->modifiers());
    contextEvent.setReason((QGraphicsSceneContextMenuEvent::Reason)(e->reason()));
    contextEvent.setAccepted(e->isAccepted());
    qt_sendSpontaneousEvent(scene(), &contextEvent);
    e->setAccepted(contextEvent.isAccepted());
#endif
}

QCursor BackstageWidget::cursor() const
{
    if (hasCursor())
        return m_cursorWidget->cursor();
    else
        return QCursor(Qt::ForbiddenCursor);
}

bool BackstageWidget::hasCursor() const
{
    return m_cursorWidget != Q_NULL;
}

void BackstageWidget::setGeometry(const QRect& geometry)
{
    m_geometry = geometry;
}

QRect BackstageWidget::geometry() const
{
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    return scene()->sceneRect().toRect();
#else
    return m_geometry;
#endif
}

void BackstageWidget::setGlobalPos(const QPoint& globalPos) const
{
    if (m_widget == Q_NULL)
        return;
    QPoint gp = globalPos;
#ifndef QTN_SHARED_BACKSTAGE_WINDOW
    gp += QPoint(margins().left(), margins().top());
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWindow* w = m_widget->window()->windowHandle();
    if (w != Q_NULL)
    {
        QRect r(gp, w->geometry().size());
        w->setGeometry(r);
        m_widget->window()->setGeometry(r);
    }
#else
    qtn_setWidgetPostion(m_widget, gp);
#endif
}
