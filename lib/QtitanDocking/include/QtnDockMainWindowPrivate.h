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

#ifndef QTN_MAINWINDOWPRIVATE_H
#define QTN_MAINWINDOWPRIVATE_H

#include <QMdiArea>
#include <QMainWindow>
#include <QStyleOption>
#include <QMdiSubWindow>

#include "QtnDockMainWindowLayout.h"
#include "QtnDockMainWindow.h"

class QStatusBar;
class QMenuBar;

QTITAN_BEGIN_NAMESPACE

enum WindowStateAction 
{
    RestoreAction,
    MoveAction,
    ResizeAction,
    MinimizeAction,
    MaximizeAction,
    StayOnTopAction,
    CloseAction,
    /* Add new states _above_ this line! */
    NumWindowStateActions
};

class ControllerWidget : public QWidget
{
    Q_OBJECT
public:
    ControllerWidget(QMdiArea* mdiArea, QWidget *parent = 0);
    QSize sizeHint() const;
    void setControlVisible(WindowStateAction action, bool visible);
    bool hasVisibleControls() const;
signals:
    void _qtn_minimize();
    void _qtn_restore();
    void _qtn_close();
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    bool event(QEvent *event);
private:
    QStyle::SubControl activeControl;
    QStyle::SubControl hoverControl;
    QStyle::SubControls visibleControls;
    void initStyleOption(QStyleOptionComplex *option) const;
    QMdiArea *mdiArea;
    QStyle::SubControl getSubControl(const QPoint &pos) const;
};

class WindowTitleBar;
class DockMainWindowLayout;
/* DockMainWindowPrivate */
class DockMainWindowPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(DockMainWindow)
public:
    explicit DockMainWindowPrivate();
    virtual ~DockMainWindowPrivate();
public:
    void initialization();
    WindowTitleBar* findTitleBar();
protected:
    ControllerWidget* controlWidget() const;
    void setVisibleAction(WindowStateAction action, bool visible);
    void updateActions(QMdiSubWindow* window);
protected Q_SLOTS:
    void subWindowActivated(QMdiSubWindow*);
protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);
public:
    DockMainWindowLayout* m_layout;
private:
    Q_DISABLE_COPY(DockMainWindowPrivate);
};


QTITAN_END_NAMESPACE

#endif // QTN_MAINWINDOWPRIVATE_H
