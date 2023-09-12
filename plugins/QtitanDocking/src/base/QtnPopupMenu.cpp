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

#include <QApplication>
#include <QStyleOption>
#include <QWidgetAction>
#include <QPainter>
#include <QMouseEvent>
#include <QMenuBar>

#include "QtnCommonStylePrivate.h"
#include "QtnPopupMenuPrivate.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

static const int splitActionPopupWidth = 20; // Split button drop down width in popups

/* PopupMenuView */
PopupMenuView::PopupMenuView(QWidget* parent, PopupMenu* menu)
    : QWidget(parent)
    , m_menu(menu)
    , m_scrollOffset(0)
{
    setMouseTracking(true);
}

PopupMenuBar* PopupMenuView::scrollMenuBar() const
{
    return qobject_cast<PopupMenuBar*>(parentWidget());
}

/*! \reimp */
void PopupMenuView::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    QRegion emptyArea = QRegion(rect());

    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    menuOpt.tabWidth = 0;
#endif
    style()->drawPrimitive(QStyle::PE_PanelMenu, &menuOpt, &p, this);

    QRect rc = this->geometry();
    QPoint offset = QPoint(0, rc.top() - m_scrollOffset);

    //draw the items that need updating..
    QList<QAction*> actionList = m_menu->actions();
    for (QList<QAction*>::iterator it = actionList.begin(); it != actionList.end(); ++it)
    {
        QAction* action = (*it);
        QRect adjustedActionRect = m_menu->actionGeometry(action);
        adjustedActionRect.moveTo(offset + this->mapFrom(m_menu, adjustedActionRect.topLeft()));

        if (!event->rect().intersects(adjustedActionRect))
            continue;
        // set the clip region to be extra safe (and adjust for the scrollers)
        emptyArea -= QRegion(adjustedActionRect);

        StyleOptionMenuScrollItem opt;
        m_menu->initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;
        opt.toolTip = action->toolTip();

        if (action->property(_qtn_SplitActionPopup).toBool())
        {
            opt.text = QStringLiteral(_qtn_SplitActionPopup) + opt.text;
            QPoint pnt = mapFromGlobal(QCursor::pos());
            if (opt.rect.adjusted(0, 0, -splitActionPopupWidth, 0).contains(pnt))
                opt.checkType = QStyleOptionMenuItem::Exclusive;
        }

        QRegion adjustedActionReg(adjustedActionRect);
        p.setClipRegion(adjustedActionReg);

        m_menu->paintMenuItem(&p, &opt);
    }

    const int fw = style()->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, this);
    // draw border
    if (fw) 
    {
        QRegion borderReg;
        borderReg += QRect(0, 0, fw, height()); //left
        borderReg += QRect(width() - fw, 0, fw, height()); //right
        borderReg += QRect(0, 0, width(), fw); //top
        borderReg += QRect(0, height() - fw, width(), fw); //bottom
        p.setClipRegion(borderReg);
        emptyArea -= borderReg;
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth, &frame);
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
    }

    // finally the rest of the spaces
    p.setClipRegion(emptyArea);
    menuOpt.state = QStyle::State_None;
    menuOpt.menuItemType = QStyleOptionMenuItem::EmptyArea;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.rect = rect();
    menuOpt.menuRect = rect();
    style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
}

/*! \reimp */
void PopupMenuView::mousePressEvent(QMouseEvent* event)
{
    QPoint pnt = event->pos();
    pnt = this->mapTo(m_menu, pnt);
    QMouseEvent ev(event->type(), pnt, event->button(), event->buttons(), event->modifiers());
    m_menu->mousePressEvent(&ev);
}

/*! \reimp */
void PopupMenuView::mouseReleaseEvent(QMouseEvent* event)
{
    QPoint pnt = event->pos();
    pnt = this->mapTo(m_menu, pnt);
    QMouseEvent ev(event->type(), pnt, event->button(), event->buttons(), event->modifiers());
    m_menu->mouseReleaseEvent(&ev);
}

/*! \reimp */
void PopupMenuView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pnt = event->pos();
    pnt = this->mapTo(m_menu, pnt);
    QMouseEvent ev(event->type(), pnt, event->button(), event->buttons(), event->modifiers());
    m_menu->mouseMoveEvent(&ev);
}

/*! \reimp */
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void PopupMenuView::enterEvent(QEnterEvent* event)
#else
void PopupMenuView::enterEvent(QEvent* event)
#endif
{
    m_menu->enterEvent(event);
}

/*! \reimp */
void PopupMenuView::leaveEvent(QEvent* event)
{
    m_menu->leaveEvent(event);
}

/* PopupMenuBar */
PopupMenuBar::PopupMenuBar(PopupMenu* parent)
    : AbstractScrollWidgetBar(parent, Qt::Vertical)
    , m_menu(parent)
{
    setPageIndent(0);
    setItemIndent(0);
    setMargins(QMargins(0, 0, 0, 0));
    setAmimationSettings(300, QEasingCurve::OutCubic /* QEasingCurve::OutCubic */);
    setFocusProxy(m_menu);
}

PopupMenuView* PopupMenuBar::menuViewWidget() const
{
    return qobject_cast<PopupMenuView*>(this->viewWidget());
}

QWidget* PopupMenuBar::createViewWidget()
{
    return new PopupMenuView(this, m_menu);
}

void PopupMenuBar::updateLayout()
{
    AbstractScrollWidgetBar::updateLayout();
    PopupMenuView* view = menuViewWidget();
    QSize viewSize = view->geometry().size();
    QPoint offset = viewOffset();
    view->m_scrollOffset = offset.y();
    PopupMenuPrivate* menuPrivate =  PopupMenuPrivate::_get(m_menu);
    m_menu->setContentsMargins(menuPrivate->m_margins.left(), view->m_scrollOffset + menuPrivate->m_margins.top(), 
        menuPrivate->m_margins.right(), -viewSize.height());
}

void PopupMenuBar::updateItemsLayout()
{
}

int PopupMenuBar::getColumnCount() const
{
    return m_menu->actions().count();
}

QSize PopupMenuBar::getColumnSize(int column) const
{
    return m_menu->actionGeometry(m_menu->actions()[column]).size();
}

void PopupMenuBar::keyPressEvent(QKeyEvent* event)
{
    Q_UNUSED(event);
    if (QAction* action = m_menu->activeAction())
    {
        int index = m_menu->actions().indexOf(action);
        if (index != -1)
            ensureVisible(index);
    }
}

/* PopupMenuPrivate */
PopupMenuPrivate::PopupMenuPrivate()
    : m_preview(Q_NULL)
    , m_scrollMenuBar(Q_NULL)
{
}

void PopupMenuPrivate::styleChanged()
{
    QTN_P(PopupMenu);
    QStyleOption opt; opt.initFrom(&p);
    const int fw = p.style()->pixelMetric(QStyle::PM_MenuPanelWidth, &opt, &p);
    const int hmargin = p.style()->pixelMetric(QStyle::PM_MenuHMargin, &opt, &p);
    const int vmargin = p.style()->pixelMetric(QStyle::PM_MenuVMargin, &opt, &p);
    m_scrollMenuBar->setMargins(QMargins(hmargin + fw, vmargin + fw, hmargin + fw, vmargin + fw));
}

void PopupMenuPrivate::updateLayout()
{
    QTN_P(PopupMenu);
    QSize s = p.size();
    QMargins margins = m_margins;

    int width = s.width();
    if (m_preview != Q_NULL)
        width = m_scrollMenuBar->sizeHint().width();

    s.rwidth() = width;
    s.rheight() = s.height() - margins.top() - margins.bottom();

    m_scrollMenuBar->setGeometry(QRect(QPoint(margins.left(), margins.top()), s));

    if (m_preview != Q_NULL)
    {
        QSize sh = m_preview->sizeHint();
        sh.setHeight(qMin(sh.height(), s.height()));
        QRect rc(QPoint(s.width() + m_margins.left(), m_margins.top()), sh);
        m_preview->setGeometry(rc);
    }
}

/*!
\class PopupMenu
\inmodule QtitanBase
\brief The Qtitan::PopupMenu class provides a menu widget for use in menu bars, context menus, and other popup menus.

The Qtitan::PopupMenu is derived from QMenu and should be used in the same cases where regular QMenu is used.
Additionally to the QMenu features, this class implements the ability to scroll items vertically if they do not fit completely on the screen 
or the number of items on the screen is limited by the property - PopupMenu::setMaxItemCount().
Moreover, it allows to display a custom QWidget on the right part from the menu items view 
which can be used as a preview for the selected item or jsut for additional info for the end-users.
*/

/*!
Constructor of the class Qtitan::PopupMenu. The parameter \c parent is a widget which will be parent and owner for the menu.
*/
PopupMenu::PopupMenu(QWidget* parent)
    : QMenu(parent)
{
    QTN_INIT_PRIVATE(PopupMenu);
    QTN_D(PopupMenu);
    d.m_scrollMenuBar = new PopupMenuBar(this);
    d.m_scrollMenuBar->viewWidget();
    d.styleChanged();
}

PopupMenu::PopupMenu(PopupMenuPrivate& d, QWidget* parent)
    : QMenu(parent)
{
    qtn_d_ptr = &d;
    d.setPublic(this);
    d.m_scrollMenuBar = new PopupMenuBar(this);
    d.m_scrollMenuBar->viewWidget();
    d.styleChanged();
}

PopupMenu::~PopupMenu()
{
    QTN_FINI_PRIVATE();
}

/*!
Returns the preview widget for the menu.
*/
QWidget* PopupMenu::previewWidget() const
{
    QTN_D(const PopupMenu);
    return d.m_preview;
}

/*!
Sets the \a preview widget for the menu. It will be shown at the right side from the menu items.
*/
void PopupMenu::setPreviewWidget(QWidget* preview)
{
    QTN_D(PopupMenu);
    if (d.m_preview != preview)
    {
        d.m_preview = preview;
        d.m_preview->setParent(this);
        d.m_preview->setFont(font());
    }
}

/*!
Sets the maximum count of items at the menu view to \a maxCount. By default the count of items is not limited, but the menu height will be limited by the screen size.
If the menu items do not fit completely at the view, then scrolling buttons will appear.
*/
void PopupMenu::setMaxItemCount(int maxCount)
{
    QTN_D(PopupMenu);
    d.m_scrollMenuBar->setMaxViewItemCount(maxCount);
}

/*!
Returns the maximum count of items at the menu view.
*/
int PopupMenu::maxItemCount() const
{
    QTN_D(const PopupMenu);
    return d.m_scrollMenuBar->maxViewItemCount();
}

/*! \reimp */
QSize PopupMenu::sizeHint() const
{
    QTN_D(const PopupMenu);
    QSize s = d.m_scrollMenuBar->sizeHint();
    if (d.m_preview != Q_NULL)
        s.setWidth(s.width() + d.m_preview->sizeHint().width());

    QStyleOption opt(0);
    opt.initFrom(this);
    QSize sz(s.width() + d.m_margins.left() + d.m_margins.right(), s.height() + d.m_margins.top() + d.m_margins.bottom());
    return style()->sizeFromContents(QStyle::CT_Menu, &opt, sz, this);
}

/*! \reimp */
void PopupMenu::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
}

void PopupMenu::paintMenuItem(QPainter* painter, QStyleOptionMenuItem* option)
{
    style()->drawControl(QStyle::CE_MenuItem, option, painter, this);
}

/*! \reimp */
void PopupMenu::changeEvent(QEvent* event)
{
    QTN_D(PopupMenu);
    QMenu::changeEvent(event);
    if (event->type() == QEvent::StyleChange)
        d.styleChanged();
}

/*! \reimp */
void PopupMenu::keyPressEvent(QKeyEvent* event)
{
    QTN_D(PopupMenu);
    QMenu::keyPressEvent(event);
    d.m_scrollMenuBar->keyPressEvent(event);
}

/*! \reimp */
void PopupMenu::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    QTN_D(PopupMenu);
    d.updateLayout();
}

