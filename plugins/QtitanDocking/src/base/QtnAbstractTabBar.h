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
#ifndef QTN_ABSTRACTTABBAR_H
#define QTN_ABSTRACTTABBAR_H

#include <QPainter>
#include <QToolButton>
#include <QBoxLayout>
#include <QStyleOption>

#include "QtnStyleOption.h"
#include "QtnSegoeMDL2Assets.h"
#include "QtnScrollWidgetBar.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class AbstractTab;
class TabBarContainer;
class MovableTabWidget;
class QTITAN_BASE_EXPORT AbstractTabBar : public QWidget
{
    Q_OBJECT
#if 0
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(SelectionBehavior selectionBehaviorOnRemove READ selectionBehaviorOnRemove WRITE setSelectionBehaviorOnRemove)
#endif
public:
    explicit AbstractTabBar(QWidget* parent = Q_NULL);
    virtual ~AbstractTabBar();
    enum SelectionBehavior
    {
        SelectLeftTab,
        SelectRightTab,
        SelectPreviousTab,
    };
public:
    int addTab(const QString& text, QObject* object = Q_NULL);
    int addTab(const QIcon& icon, const QString& text, QObject* object = Q_NULL);

    int insertTab(int index, const QString& text, QObject* object = Q_NULL);
    int insertTab(int index, const QIcon& icon, const QString& text, QObject* object = Q_NULL);

    void removeTab(int index);
    virtual void moveTab(int from, int to);

    bool isTabEnabled(int index) const;
    void setTabEnabled(int index, bool enabled);

    bool isTabVisible(int index) const;
    void setTabVisible(int index, bool visible);

    QString tabText(int index) const;
    void setTabText(int index, const QString &text);

    QColor tabTextColor(int index) const;
    void setTabTextColor(int index, const QColor &color);

    QIcon tabIcon(int index) const;
    void setTabIcon(int index, const QIcon &icon);

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode mode);

    void setTabToolTip(int index, const QString &tip);
    QString tabToolTip(int index) const;

    int currentIndex() const;
    const AbstractTab* currentTab() const;
    int count() const;

    QSize iconSize() const;
    void setIconSize(const QSize& size);

    bool tabsClosable() const;
    void setTabsClosable(bool closable);

    bool isMovable() const;
    void setMovable(bool movable);

    bool isScrollable() const;
    void setScrollable(bool scrollable);

    bool scrollButtonsBeside() const;
    void setScrollButtonsBeside(bool beside);

    void addWidgetToLeftSide(QWidget* widget, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());
    int insertWidgetToLeftSide(int index, QWidget* widget, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());
    void setDirectionRightSide(QBoxLayout::Direction direction);
    void addWidgetToRightSide(QWidget *widget, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());
    int insertWidgetToRightSide(int index, QWidget *widget, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());
    void removeWidget(QWidget* widget);
public:
    virtual AbstractTab* createTab(const QIcon& icon, const QString& text, QObject* object) = 0;
    virtual QSize sizeHint() const override;
    inline bool isIndexValid(int index) const { return index >= 0 && index < m_tabList.count(); }
    AbstractTab* tabAt(int index);
    const AbstractTab* tabAt(int index) const;
    int tabIndex(const AbstractTab* tab) const;
    QRect tabRect(int index) const;
    void updateLayout();
protected:
    virtual void paintTab(QPainter* painter, AbstractTab* tab) const;
    virtual void paintTabBarFrame(QPainter* painter, QStyleOptionTabBarBase* optTabBase) = 0;
    void moveTabOffset(int index, int offset);
    QSize calcMinSize(QWidget* widget) const;
    QRect calcCenterRect() const;
    int indexToLastRightWidget() const;
    void updateLayoutWidgets();
private:
    void init();
    void calcFirstLastVisible(int index, bool visible, bool remove);
    int selectNewCurrentIndexFrom(int m_currentIndex);
    int calcPosition(int from, int to, int index) const;
    void moveTabPosition(int from, int to);

    int indexAtPos(const QPoint& p) const;
    int tabAt(const QPoint& pos) const;

    SelectionBehavior selectionBehaviorOnRemove() const;
    void setSelectionBehaviorOnRemove(SelectionBehavior behavior);

    bool isAnimated() const;
    bool stopAnimation();
    void setCurrentNextEnabledIndex(int offset);
    void moveTabStopped(int index);

    void refresh();
    void createMovableTab();
public Q_SLOTS:
    void setCurrentIndex(int index);
Q_SIGNALS:
    void currentAboutToBeChanged(int index, bool& changed);
    void currentChanged(int index);
    void tabCloseRequested(int index);
    void tabMoved(int from, int to);
protected:
    virtual bool event(QEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void showEvent(QShowEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void changeEvent(QEvent* event) override;
protected:
    void closeButtonPressed(int index);
    virtual void setLayoutDirty() { m_layoutDirty = true; }
protected:
    int m_currentIndex;
    int m_pressedIndex;
    int m_previousIndex;
    int m_beginVisible;
    int m_endVisible;
    bool m_layoutDirty;
    QList<AbstractTab*> m_tabList;

    QSize m_iconSize;
    Qt::TextElideMode m_elideMode;
    bool m_tabsClosable;

    QPoint m_pntStartDrag;
    bool m_movable;
    bool m_startDrag;
    SelectionBehavior m_selectionBehaviorOnRemove;
    MovableTabWidget* m_movableTabWidget;
    TabBarContainer* m_barContainer;
    QBoxLayout::Direction m_layoutDirection;

    struct TabBarItem
    {
        TabBarItem(QWidget* w, int s, Qt::Alignment a, bool r)
            : stretch(s), widget(w), right(r), customWidget(false), 
              alignment(a), action(Q_NULL) {}
        int stretch;
        QWidget* widget;
        bool right;
        bool customWidget;
        Qt::Alignment alignment;
        QAction* action;
    };
    QBoxLayout* m_customWidgetLayout;
    QList<TabBarItem*> m_items;
private:
    friend class AbstractTab;
    friend struct DragTabAnimation;
    friend class MovableTabWidget;
    Q_DISABLE_COPY(AbstractTabBar)
};

class QTITAN_BASE_EXPORT AbstractTab : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractTab(AbstractTabBar* bar, const QIcon& icon, const QString& text);
    ~AbstractTab() override;
public:
    void startAnimation(AbstractTabBar* tabBar, int duration);
    AbstractTabBar* tabBar() const;
    int index() const;
    QObject* object() const;
    void setObject(QObject* obj);

    void initStyleOption(StyleOptionTab* option) const;
    virtual void initBasicStyleOption(StyleOptionTab* option) const;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    bool isCurrent() const;
public Q_SLOTS:
    virtual void clicked();
    virtual void dblClicked();
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    void enterEvent(QEnterEvent* event) override;
#else
    void enterEvent(QEvent* event) override;
#endif
    void leaveEvent(QEvent* event) override;
    bool event(QEvent* event) override;
public:
    int m_shortcutId;
    QString m_text;
    QString m_toolTip;
    QIcon m_icon;
    int m_minSize;
    int m_maxSize;
    bool m_hoverClose;
    bool m_pressClose;
    QColor m_textColor;
    int m_lastTab;
    int m_dragOffset;
    QVariantAnimation* m_animation;
    AbstractTabBar* m_tabBar;
private:
    QObject* m_object;
private:
    Q_DISABLE_COPY(AbstractTab);
};

QTITAN_END_NAMESPACE

#endif // QTN_ABSTRACTTABBAR_H
