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

#ifndef QTN_DOCKBARBASE_H
#define QTN_DOCKBARBASE_H

#include <QWidget>
#include "QtnDockMainWindow.h"
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockBarBasePrivate;
class QTITAN_EXPORT DockBarBase : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Qtitan::DockBarArea dockBarArea READ dockBarArea WRITE setDockBarArea)
public:
    explicit DockBarBase(QWidget* parent, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~DockBarBase();
public:
    QString id() const;
    void setId(const QString& id);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    Qtitan::DockBarArea dockBarArea() const;
    bool setDockBarArea(Qtitan::DockBarArea barPosition);

    QString caption() const;
    void setCaption(const QString& caption);
protected:
    DockBarBase(DockBarBasePrivate& dd, QWidget* parent, Qt::WindowFlags f);
protected:
    QTN_DECLARE_PRIVATE(DockBarBase)
private:
    Q_DISABLE_COPY(DockBarBase)
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKBARBASE_H
