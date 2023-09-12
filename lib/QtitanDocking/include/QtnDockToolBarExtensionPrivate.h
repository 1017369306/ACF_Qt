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

#ifndef QTN_DOCKTOOLBAREXTENSIONPRIVATE_H
#define QTN_DOCKTOOLBAREXTENSIONPRIVATE_H

#include <QToolButton>
#include <QStyleOption>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class DockToolBar;
/* DockToolBarSeparator */
class DockToolBarSeparator : public QWidget
{
    Q_OBJECT
    Qt::Orientation orient;
public:
    explicit DockToolBarSeparator(DockToolBar* parent);
    Qt::Orientation orientation() const;
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
    void initStyleOption(QStyleOption* option) const;
public Q_SLOTS:
    void setOrientation(Qt::Orientation orientation);
};

/* DockToolBarExtension */
class DockToolBarExtension : public QToolButton
{
    Q_OBJECT
    Qt::Orientation orientation;
public:
    explicit DockToolBarExtension(QWidget *parent);
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;

public Q_SLOTS:
    void setOrientation(Qt::Orientation o);
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKTOOLBAREXTENSIONPRIVATE_H
