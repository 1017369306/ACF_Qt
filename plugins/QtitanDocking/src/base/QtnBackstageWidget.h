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

#ifndef QTN_BACKSTAGEWIDGET_H
#define QTN_BACKSTAGEWIDGET_H

#include <QPointer>
#include <QMargins>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QWindow>
#endif

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

#ifdef QTN_PRIVATE_HEADERS
#define QTN_SHARED_BACKSTAGE_WINDOW
#endif

#ifdef QTN_SHARED_BACKSTAGE_WINDOW
class BackstageSharedWindow;
class QTITAN_BASE_EXPORT BackstageWidgetBase : public QObject
{
    Q_OBJECT
public:
    BackstageWidgetBase(QWidget* viewWidget, QObject* parent = Q_NULL);
    virtual ~BackstageWidgetBase();
    QWidget* viewWidget() const;
    virtual QWidget* widget() const = 0;
Q_SIGNALS:
    void changed(const QList<QRectF> &region);
protected:
    QWidget* m_viewWidget;
    BackstageSharedWindow* m_backstageWindow;
};
#else
class BackstageWidgetBase : public QGraphicsScene
{
public:
    BackstageWidgetBase(QWidget* viewWidget, QObject* parent)
        : QGraphicsScene(parent), m_viewWidget(viewWidget)
    {
    }
    QWidget* viewWidget() const
    {
        return m_viewWidget;
    }
private:
    QWidget* m_viewWidget;
};
#endif

class QTITAN_BASE_EXPORT BackstageWidget: public BackstageWidgetBase
{
    Q_OBJECT
public:
    BackstageWidget(QWidget* viewWidget, QObject* parent = Q_NULL);
    virtual ~BackstageWidget();
    virtual QWidget* widget() const;
    void setWidget(QWidget* widget, const QRect& geometry = QRect());
    void activate();
    void deactivate();
    void setMargins(int left, int top, int right, int bottom);
    void setMargins(const QMargins& margins);
    const QMargins& margins() const;
    bool hasFocusItem();
    bool hasMouseGrabber();
    bool isInputMethodSupported() const;
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    void paintContent(QPainter& painter);

    /* mouse events */
    void send_mouseMoveEvent(QMouseEvent* e);
    void send_mousePressEvent(QMouseEvent* e);
    void send_mouseReleaseEvent(QMouseEvent* e);
    void send_mouseDoubleClickEvent(QMouseEvent* e);
    void send_mouseOverEvent(const QPoint& pos);
    void send_mouseOutEvent(const QPoint& pos);
    void send_wheelEvent(QWheelEvent *);
    /* keyboard events */
    void send_keyPressEvent(QKeyEvent* e, const QPoint& globalPosition);
    void send_keyReleaseEvent(QKeyEvent* e);
    void send_shortcutEvent(QKeyEvent* e);
    void send_shortcutOverrideEvent(QKeyEvent* e);
    /* input method */
    void send_inputMethodEvent(QInputMethodEvent* e);
    /* focus events */
    void send_focusInEvent(Qt::FocusReason reason);
    void send_focusOutEvent(Qt::FocusReason reason);
    void send_contextMenuEvent(QContextMenuEvent* event);
public:
    QCursor cursor() const;
    bool hasCursor() const;
    void setGeometry(const QRect& geometry);
    QRect geometry() const;
    void setGlobalPos(const QPoint& globalPos) const;
private:
    void syncToView();
    QGraphicsScene* scene() const;
    void connectWidget(const QRect& geometry);
    void disconnectWidget();
    void checkCursor(const QPoint& p);
    QWidget* findWidgetByPos(const QPoint& pos);
    bool tryShortcut(QKeyEvent* e);
protected Q_SLOTS:
    void widgetDestroyed();
private:
    void updateMousePressWidget(QWidget* w, Qt::MouseButtons buttons);
    QWidget* m_widget;
    QWidget* m_cursorWidget;
    QWidget* m_mousePressWidget;
    QGraphicsProxyWidget* m_proxyWidget;
    Qt::MouseButtons m_mousePressButtons;
    Qt::MouseButton m_mousePressButton;
    QMargins m_margins;
    QRect m_geometry;
#ifdef QTN_SHARED_BACKSTAGE_WINDOW
    QPointer<QWidget> m_overWidget;
    QRegion m_needsRepaint;
    friend class BackstageSharedWindow;
#else
    QPointF m_mousePressPosition;
    QPoint m_mousePressScreenPosition;
    QPointF m_lastMouseMoveScenePosition;
    QPoint m_lastMouseMoveScreenPosition;
#endif
};

QTITAN_END_NAMESPACE

#endif //QTN_BACKSTAGEWIDGET_H

