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

#ifndef QTN_DOCKTOOLBARLAYOUTPRIVATE_H
#define QTN_DOCKTOOLBARLAYOUTPRIVATE_H

#include <QLayout>
#include <QToolButton>
#include <QVector>
#include "QtitanDef.h"

class QAction;
class QMenu;

QTITAN_BEGIN_NAMESPACE

class DockToolBarExtension;
class DockToolBarItem : public QWidgetItem
{
public:
    DockToolBarItem(QWidget *widget);
    bool isEmpty() const;
public:
    QAction* action;
    bool customWidget;
};

class GeomLayout
{
public:
    int suitableSizeHint() { return (stretch > 0) ? minimumSize : sizeHint; }
    int bestSpacer(int uniformSpacer) const
    { Q_ASSERT(uniformSpacer >= 0 || spacing >= 0); return (uniformSpacer >= 0) ? uniformSpacer : spacing; }
public:
    int stretch;
    int sizeHint;
    int spacing;
    int maximumSize;
    int minimumSize;
    bool done;
    bool empty;
    bool expansive;
public: // result
    int pos;
    int size;
};

class DockTitleBar;
class DockToolBarLayout : public QLayout
{
    Q_OBJECT
public:
    DockToolBarLayout(QWidget* parent = Q_NULL);
    ~DockToolBarLayout() override;
public:
    void addItem(QLayoutItem* item) override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    int count() const override;

    bool isEmpty() const override;
    void invalidate() override;
    Qt::Orientations expandingDirections() const override;

    void setGeometry(const QRect &r) override;
    QSize minimumSize() const override;
    QSize sizeHint() const override;

    void insertAction(int index, QAction* action);
    int getActionIndex(QAction* action) const;
    int getWidgetIndex(QWidget* widget) const { return QLayout::indexOf(widget); }

    bool layoutActions(const QSize &size);
    QSize expandedSize(const QSize &size) const;
    bool expanded;
    bool animating;

    void setUsePopupMenu(bool set);
    void checkUsePopupMenu();

    bool movable() const;
    void updateMarginAndSpacing();
    bool hasExpandFlag() const;
public Q_SLOTS:
    void setExpanded(bool b);
private:
    QList<DockToolBarItem*> m_items;
    QSize m_szHint;
    QSize m_szMinSize;
    bool m_dirty;
    bool m_expanding;
    bool m_empty;
    bool m_expandFlag;
    QVector<GeomLayout> m_geomArray;
    DockToolBarExtension* m_extension;
    void updateGeomArray() const;
    DockToolBarItem* createItem(QAction *action);
    QMenu* m_popupMenu;
    int m_heightItem;
};

class ToolButton : public QToolButton
{
    Q_OBJECT
public:
    ToolButton(QWidget* parent) : QToolButton(parent) {}
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
};

QTITAN_END_NAMESPACE

#endif //QTN_DOCKTOOLBARLAYOUTPRIVATE_H

