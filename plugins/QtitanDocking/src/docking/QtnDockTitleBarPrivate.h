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

#ifndef QTN_DOCKTITLEBARPRIVATE_H
#define QTN_DOCKTITLEBARPRIVATE_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include "QtnDockTitleBar.h"

QTITAN_BEGIN_NAMESPACE

#define _qtn_Type_Button       "Type_Button"

enum TypeButton
{
    CloseButton,
    AutoHideButton,
    MenuButton
};

/* DockWidgetTitleButton */
class DockWidgetTitleButton : public QToolButton
{
    Q_OBJECT
public:
    explicit DockWidgetTitleButton(QWidget* dockWidget);
    virtual ~DockWidgetTitleButton();
public:
    void setStateVisible(bool visible) { m_stateVisible = visible; update(); }
    bool isStateVisible() const { return m_stateVisible; }
    TypeButton typeButton() const;
    QSize sizeHint() const;
    inline QSize minimumSizeHint() const { return sizeHint(); }
protected:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    virtual void enterEvent(QEnterEvent* event);
#else
    virtual void enterEvent(QEvent* event);
#endif
    virtual void leaveEvent(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
private:
    void initStyleOption(QStyleOptionToolButton *option) const;
    QSize dockButtonIconSize() const;
private:
    bool m_stateVisible;
private:
    Q_DISABLE_COPY(DockWidgetTitleButton)
};

/* DockTitleBarPrivate */
class DockTitleBarPrivate : public QObject
{
    QTN_DECLARE_PUBLIC(DockTitleBar)
public:
    explicit DockTitleBarPrivate();
    virtual ~DockTitleBarPrivate();
public:
    struct TitleBarItem 
    {
        TitleBarItem(QWidget* w, int s, bool r)
            : stretch(s), widget(w), right(r), customWidget(false), action(Q_NULL) {}
        int stretch;
        QWidget* widget;
        bool right;
        bool customWidget;
        QAction* action;
    };
    void init();
    void addWidget(QWidget* widget, int stretch = 0);
    int insertWidget(int index, QWidget* widget, int stretch);
    void addWidgetToRightSide(QWidget *widget, int stretch = 0);
    int insertWidgetToRightSide(int index, QWidget *widget, int stretch = 0);
    void removeWidget(QWidget *widget);
    DockWidgetTitleButton* titleButton(TypeButton type);

    int indexToLastRightWidget() const;
    QRect calcTextRect() const;
    QRect calcGripRect(DockTitleBarStyleOption* option) const;
    QSize calcMinSize(QWidget* widget) const;
    int titleHeight() const;
    void updateLayout();
public:
    QList<TitleBarItem*> m_items;
    QBoxLayout* m_boxLayout;
    QLabel* m_icon;
    QString m_caption;
    int m_keepStrut;
    bool m_active;
    bool m_closeButton;
    bool m_movable;
    bool m_floatable;
    bool m_autoHideButton;
    bool m_menuButton;
    bool m_noDrawGrip;
private:
    Q_DISABLE_COPY(DockTitleBarPrivate)
};


QTITAN_END_NAMESPACE

#endif // QTN_DOCKTITLEBARPRIVATE_H
