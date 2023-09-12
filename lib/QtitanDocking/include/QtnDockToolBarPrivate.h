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

#ifndef QTN_DOCKTOOLBARPRIVATE_H
#define QTN_DOCKTOOLBARPRIVATE_H

#include <QtCore/qbasictimer.h>
#include <QVBoxLayout>

#include "QtnDockToolBar.h"
#include "QtnDockBarBasePrivate.h"
#include "QtnDockTitleBar.h"

class QTimer;
class QLayoutItem;

QTITAN_BEGIN_NAMESPACE

class DockTitleBar;
class DockBarWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DockBarWindow(DockBarManager* manager);
    virtual ~DockBarWindow();
public:
    DockTitleBar* titleBar() const;
    void setDockBar(DockToolBar* dockBar);
    DockToolBar* toolBar() const;
protected:
    virtual bool event(QEvent* event);
    virtual void changeEvent(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
protected:
    void moveToMouse(const QPoint& mousePos);
    DockBarManager* m_barManager;
    DockToolBar* m_toolBar;
    DockTitleBar* m_titleBar;
    QVBoxLayout* m_boxLayout;
    QAction* m_actCloseButton;
private:
    friend class DockToolBarPrivate;
    Q_DISABLE_COPY(DockBarWindow);
};

class DockMainWindowLayout;
class DockToolBarLayout;
class DockToolBarPrivate : public DockBarBasePrivate
{
    Q_OBJECT
    QTN_DECLARE_EX_PUBLIC(DockToolBar);
public:
    DockToolBarPrivate();
    virtual ~DockToolBarPrivate();
public:
    static DockToolBarPrivate* _get(DockToolBar* toolBar)
    { return &toolBar->qtn_d(); }
    static const DockToolBarPrivate* _get(const DockToolBar* toolBar)
    { return &toolBar->qtn_d(); }
    void initialization();
    QRect toolBarHandleRect() const;
    void activateMainWindow();
public:
    bool mousePressEvent(QMouseEvent *e);
    bool mouseReleaseEvent(QMouseEvent *e);
    bool mouseMoveEvent(QMouseEvent *e);
public:
    bool isFloating() const;
    bool isDragging() const;
    DockBarWindow* getDockBarWindow();
    void initStyleOption(QStyleOptionToolBar* option) const;
    void initDragMode(const QPoint& pos, bool immediateStart);
    void endDragMode();
public:
    bool m_explicitIconSize;
    bool m_explicitToolButtonStyle;
    bool m_explicitCaption;
    bool m_movable;
    Qtitan::DockBarArea m_allowedAreas;
    Qt::ToolButtonStyle m_toolButtonStyle;
    QSize m_iconSize;
    DockToolBarLayout* m_layout;
    DockBarWindow* m_dockBarWindow;
    bool m_dragging;
    QPoint m_pressedPos;
    QBasicTimer waitForPopupTimer;
    int m_height;
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKTOOLBARPRIVATE_H
