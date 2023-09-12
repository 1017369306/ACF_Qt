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

#ifndef QTN_MAINWINDOW_H
#define QTN_MAINWINDOW_H

#include <QWidget>
#include <QStatusBar>
#include <QMenuBar>
#include "QtnDockDef.h"
#include "QtnDockBarManager.h"
#include "QtnDockPanelManager.h"
#include "QtnWindowTitleBar.h"

QTITAN_BEGIN_NAMESPACE

class DockMainWindowPrivate;
class QTITAN_EXPORT DockMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DockMainWindow(QWidget* parent = Q_NULL, Qt::WindowFlags flags = Qt::WindowFlags());
    virtual ~DockMainWindow();
public:
    WindowTitleBar* titleBar() const;

    QWidget* titleBarWidget() const;
    void setTitleBarWidget(QWidget* widget);

    QMenuBar* menuBar() const;
    void setMenuBar(QMenuBar* menubar);

    QStatusBar* statusBar() const;
    void setStatusBar(QStatusBar* statusbar);

    QWidget* centralWidget() const;
    void setCentralWidget(QWidget* widget);

    DockBarManager* dockBarManager() const;
    DockPanelManager* dockPanelManager() const;

    const QList<DockBarBase*>& dockBars() const;
    DockToolBar* addToolBar(const QString& title, Qtitan::DockBarArea barPosition);
    bool insertDockBar(DockToolBar* bar, Qtitan::DockBarArea position);
    void insertDockBar(DockToolBar* before, DockToolBar* bar);
    void removeDockBar(DockToolBar* bar);
    void moveDockBar(DockToolBar* before, DockToolBar* bar);
    void clearDockBars();

    DockWindow* addDockWindow(DockPanelBase* panel, const QRect& rect);
    DockDocumentPanel* addDocumentPanel(const QString& title = QString());
    void insertDocumentPanel(DockDocumentPanel* documentPanel);
    DockWidgetPanel* addDockPanel(const QString& title, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    DockWidgetPanel* addDockPanel(const QString& title, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    void insertDockPanel(DockWidgetPanel* panel, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    void insertDockPanel(DockWidgetPanel* panel, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    void removeDockPanel(DockWidgetPanel* panel);
    void clearDockPanels();
public:
    void saveStateToDevice(QIODevice* deviceBars, QIODevice* devicePanels);
    bool loadStateFromDevice(QIODevice* deviceBars, QIODevice* devicePanels);
protected:
    virtual bool event(QEvent* event);
    virtual void showEvent(QShowEvent* event);
private:
    QTN_DECLARE_PRIVATE(DockMainWindow)
    Q_DISABLE_COPY(DockMainWindow)
};

QTITAN_END_NAMESPACE

#endif // QTN_MAINWINDOW_H
