/****************************************************************************
**
** Qtitan Library by Developer Machines
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

#ifndef QTN_POPUPMENUPRIVATE_H
#define QTN_POPUPMENUPRIVATE_H

#include <QPointer>

#include "QtnPopupMenu.h"
#include "QtnStyleOption.h"
#include "QtnScrollWidgetBar.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class PopupMenuBar;

class PopupMenuView : public QWidget
{
    Q_OBJECT
public:
    explicit PopupMenuView(QWidget* parent, PopupMenu* menu);
public:
    PopupMenuBar* scrollMenuBar() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    virtual void enterEvent(QEnterEvent* event);
#else
    virtual void enterEvent(QEvent* event);
#endif
    virtual void leaveEvent(QEvent* event);
protected:
    PopupMenu* m_menu;
    int m_scrollOffset;
private:
    friend class PopupMenuBar;
    Q_DISABLE_COPY(PopupMenuView)
};

class PopupMenuBar : public AbstractScrollWidgetBar
{
    Q_OBJECT
public:
    explicit PopupMenuBar(PopupMenu* parent);
public:
    PopupMenuView* menuViewWidget() const;
    ScrollWidgetBarButton* downButton() const { return m_nextButton; }
    QPoint viewOffset() const { return getViewOffset(); }
protected:
    QWidget* createViewWidget() override;
    void updateLayout() override;
    void updateItemsLayout() override;
    int getColumnCount() const override;
    QSize getColumnSize(int column) const override;
protected:
    void keyPressEvent(QKeyEvent* event) override;
protected:
    PopupMenu* m_menu;
private:
    friend class PopupMenu;
    Q_DISABLE_COPY(PopupMenuBar);
};

/* PopupMenuPrivate */
class QTITAN_BASE_EXPORT PopupMenuPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(PopupMenu)
public:
    explicit PopupMenuPrivate();
public:
    static inline PopupMenuPrivate* _get(PopupMenu* menu)
    { return &menu->qtn_d(); }
    static inline const PopupMenuPrivate* _get(const PopupMenu* menu)
    { return &menu->qtn_d(); }
public:
    void styleChanged();
    void updateLayout();
public:
    QMargins m_margins;
    QWidget* m_preview;
    PopupMenuBar* m_scrollMenuBar;
};

QTITAN_END_NAMESPACE

#endif // QTN_POPUPMENUPRIVATE_H


