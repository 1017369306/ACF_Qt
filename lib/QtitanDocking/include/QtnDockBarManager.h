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

#ifndef QTN_DOCKBARMANAGER_H
#define QTN_DOCKBARMANAGER_H

#include <QObject>
#include <QRect>
#include <QPoint>
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockToolBar;
class DockBarBase;
class DockBarManagerPrivate;
class QTITAN_EXPORT DockBarManager : public QObject
{
    Q_OBJECT
public:
    explicit DockBarManager(QWidget* widget);
    virtual ~DockBarManager();
public:
    QWidget* managedWidget() const;
    DockToolBar* addToolBar(const QString& name, Qtitan::DockBarArea dockBarArea);
    void insertDockBar(DockToolBar* before, DockToolBar* toolBar);
    bool insertDockBar(DockToolBar* toolBar, Qtitan::DockBarArea dockBarArea);
    void moveDockBar(DockToolBar* before, DockToolBar* toolBar);
    void removeDockBar(DockToolBar* toolBar);
    void clearDockBars();

    QWidget* centralWidget() const;
    void setCentralWidget(QWidget* widget);

    QList<DockToolBar*> toolBars() const;
    const QList<DockBarBase*>& dockBars() const;
    int dockBarCount() const;
    DockBarBase* dockBarAt(int index) const;

    QRect updateLayoutGeometry(const QRect& rect);
public:
    bool saveStateToFile(const QString& fileName, bool autoFormatting = true);
    void saveStateToDevice(QIODevice* device, bool autoFormatting = true);
    bool loadStateFromFile(const QString& fileName);
    bool loadStateFromDevice(QIODevice* device);
protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);
private:
    QTN_DECLARE_PRIVATE(DockBarManager);
    Q_DISABLE_COPY(DockBarManager);
};

extern bool isDockingPosition(Qtitan::DockBarArea barPosition);

QTITAN_END_NAMESPACE

#endif // QTN_DOCKBARMANAGER_H
