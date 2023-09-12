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

#include "QtnDockTitleBarPrivate.h"
#include <QApplication>
#include <QWidgetAction>
#include <QLayout>
#include <QPainter>
#include <QLabel>

#include "QtnCommonStylePrivate.h"
#include "QtnDockToolBar.h"
#include "QtnDockWindow.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif


QTITAN_USE_NAMESPACE

/* DockWidgetTitleButton */
DockWidgetTitleButton::DockWidgetTitleButton(QWidget* dockWidget/*, DockTitleBarLayout::Role role*/)
    : QToolButton(dockWidget)
    , m_stateVisible(false)
{
    //add by zhangyang 20230912 自动扩展
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //内间距
//    setContentsMargins(0, 0, 0, 0);
    //指定样式，消除图标内部的padding
    setProperty("isTransparentAndNoPadding", true);

    setFocusPolicy(Qt::ClickFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
}

DockWidgetTitleButton::~DockWidgetTitleButton()
{
}

TypeButton DockWidgetTitleButton::typeButton() const
{
    QAction* action = defaultAction();
    return action ? (TypeButton)action->property(_qtn_Type_Button).toInt() : CloseButton;
}

/*! \reimp */
QSize DockWidgetTitleButton::sizeHint() const
{
    ensurePolished();
    QStyleOption opt;
    opt.initFrom(this);
#if (QT_VERSION > QT_VERSION_CHECK(5, 8, 0))
    int size = style()->pixelMetric(QStyle::PM_TitleBarButtonSize, &opt, this->parentWidget());
#else
    int size = CommonStylePrivate::dpiScaled(14.0, this->parentWidget());
#endif
    return QSize(size, size);
}

/*! \reimp */
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void DockWidgetTitleButton::enterEvent(QEnterEvent* event)
#else
void DockWidgetTitleButton::enterEvent(QEvent* event)
#endif
{
    if (isEnabled()) update();
    QToolButton::enterEvent(event);
}

/*! \reimp */
void DockWidgetTitleButton::leaveEvent(QEvent* event)
{
    if (isEnabled()) update();
    QToolButton::leaveEvent(event);
}

/*! \reimp */
void DockWidgetTitleButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
    if (!opt.icon.isNull())
        style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}

void DockWidgetTitleButton::initStyleOption(QStyleOptionToolButton* opt) const
{
    opt->initFrom(this);
    opt->state |= QStyle::State_AutoRaise;
    opt->state &= ~QStyle::State_HasFocus;
    if (style()->styleHint(QStyle::SH_DockWidget_ButtonsHaveFrame, Q_NULL, this))
    {
        if (isEnabled() && underMouse() && !isChecked() && !isDown())
            opt->state |= QStyle::State_Raised;
        if (isChecked())
            opt->state |= QStyle::State_On;
        if (isDown())
            opt->state |= QStyle::State_Sunken;
    }
    const DockTitleBar* titleBar = qobject_cast<const DockTitleBar*>(parentWidget());
    if (titleBar && titleBar->isTitleActive())
    {
        opt->state |= QStyle::State_Active;
        opt->palette.setCurrentColorGroup(QPalette::Active);
    }
    else
    {
        opt->state &= ~QStyle::State_Active;
        opt->palette.setCurrentColorGroup(QPalette::Inactive);
    }

    QIcon icon;
    TypeButton type = typeButton();
    if (type == CloseButton)
    {
        icon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, opt, this);
    }
    else if (type == MenuButton)
    {
        icon = style()->standardPixmap(QStyle::SP_ArrowDown, opt, this);
    }
    else if (type == AutoHideButton)
    {
        icon.addPixmap(style()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_TitleBarPinButton), opt, this), QIcon::Active, QIcon::Off);
        icon.addPixmap(style()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_TitleBarPinnedButton), opt, this), QIcon::Active, QIcon::On);
    }
    opt->icon = icon;

    QSize sz = opt->icon.actualSize(sizeHint()/*opt->rect.size()*/);
    if (type == AutoHideButton)
        opt->icon = m_stateVisible ? QIcon(opt->icon.pixmap(sz, QIcon::Normal, QIcon::Off)) :
            QIcon(opt->icon.pixmap(sz, QIcon::Normal, QIcon::On));
    opt->subControls = QStyle::SC_None;
    opt->activeSubControls = QStyle::SC_None;
    opt->features = QStyleOptionToolButton::None;
    opt->arrowType = Qt::NoArrow;
    //add by zhangyang 20230912 在内部没有padding的情况下，图标调整小一点
    opt->iconSize = QSize(sz.width() * 0.6, sz.height() * 0.6);
//    opt->iconSize = opt->rect.size() - QSize(4, 4);
//    opt->iconSize = this->iconSize();
}

QSize DockWidgetTitleButton::dockButtonIconSize() const
{
    int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this);
    return QSize(iconSize, iconSize);
}

/* DockTitleBarPrivate */
DockTitleBarPrivate::DockTitleBarPrivate()
    : m_boxLayout(Q_NULL)
    , m_icon(Q_NULL)
    , m_keepStrut(0)
    , m_active(false)
    , m_closeButton(false)
    , m_movable(false)
    , m_floatable(false)
    , m_autoHideButton(false)
    , m_menuButton(false)
    , m_noDrawGrip(true)
{
}

DockTitleBarPrivate::~DockTitleBarPrivate()
{
    while (!m_items.isEmpty())
    {
        TitleBarItem* item = m_items.takeFirst();
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(item->action))
        {
            if (item->customWidget)
                widgetAction->releaseWidget(item->widget);
        }
        Q_DELETE_AND_NULL(item);
    }
}

void DockTitleBarPrivate::init()
{
    QTN_P(DockTitleBar);
    p.setMouseTracking(true);
    p.setFocusPolicy(Qt::ClickFocus);
    p.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    p.setFont(QApplication::font("QDockWidgetTitle"));
    m_icon = new QLabel;
    addWidget(m_icon);
    m_icon->setVisible(false);
}

void DockTitleBarPrivate::addWidget(QWidget* widget, int stretch)
{
    if (widget == Q_NULL)
        return;
    insertWidget(m_items.size(), widget, stretch);
}

int DockTitleBarPrivate::insertWidget(int index, QWidget* widget, int stretch)
{
    if (widget == Q_NULL)
        return -1;
    TitleBarItem* item = new TitleBarItem(widget, stretch, false);

    int idx = indexToLastRightWidget();
    if (index < 0 || index > m_items.size() || (idx >= 0 && index > idx + 1))
    {
        qWarning("DockTitleBar::insertWidget: Index out of range (%d), appending widget", index);
        index = idx + 1;
    }
    m_items.insert(index, item);

    updateLayout();
    if (!widget->isHidden() || !widget->testAttribute(Qt::WA_WState_ExplicitShowHide))
        widget->show();
    return index;
}

void DockTitleBarPrivate::addWidgetToRightSide(QWidget *widget, int stretch)
{
    if (widget == Q_NULL)
        return;
    insertWidgetToRightSide(m_items.size(), widget, stretch);
}

int DockTitleBarPrivate::insertWidgetToRightSide(int index, QWidget* widget, int stretch)
{
    if (widget == Q_NULL)
        return -1;

    TitleBarItem* item = new TitleBarItem(widget, stretch, true);

    int idx = indexToLastRightWidget();
    if (index < 0 || index > m_items.size() || (idx >= 0 && index <= idx))
    {
        qWarning("DockTitleBar::insertWidgetToRightSide: Index out of range (%d), appending widget", index);
        index = m_items.size();
    }
    m_items.insert(index, item);

    updateLayout();
    if (!widget->isHidden() || !widget->testAttribute(Qt::WA_WState_ExplicitShowHide))
        widget->show();
    return index;
}

void DockTitleBarPrivate::removeWidget(QWidget* widget)
{
    if (widget == Q_NULL)
        return;
    bool found = false;
    for (int i = 0; i < m_items.size(); ++i)
    {
        TitleBarItem* item = m_items.at(i);
        if (item == Q_NULL)
            break;

        if (item->widget == widget)
        {
            m_items.removeAt(i);
            item->widget->hide();

            QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(item->action);
            if (widgetAction != Q_NULL && item->customWidget)
                widgetAction->releaseWidget(item->widget);
            Q_DELETE_AND_NULL(item);
            found = true;
            break;
        }
    }
    if (found)
        updateLayout();
}

DockWidgetTitleButton* DockTitleBarPrivate::titleButton(TypeButton type)
{
    for (int i = 0; i < m_items.size(); ++i)
    {
        TitleBarItem* item = m_items.at(i);
        if (item == Q_NULL)
            break;
        DockWidgetTitleButton* button = qobject_cast<DockWidgetTitleButton*>(item->widget);
        if (button != Q_NULL && button->typeButton() == type)
            return button;
    }
    return Q_NULL;
}

int DockTitleBarPrivate::indexToLastRightWidget() const
{
    int i = m_items.size() - 1;
    for (; i >= 0; --i) 
    {
        TitleBarItem* item = m_items.at(i);
        if (!(item && item->right))
            break;
    }
    return i;
}

QRect DockTitleBarPrivate::calcTextRect() const
{
    QTN_P(const DockTitleBar);
    int left = 0;
    for (int i = 0; i < m_items.size(); ++i)
    {
        TitleBarItem* item = m_items.at(i);
        if (item == Q_NULL)
            break;
        if (!item->right && item->widget->isVisible())
            left += item->widget->geometry().right();
     }

    int right = p.width();
    for (int i = 0; i < m_items.size(); ++i)
    {
        TitleBarItem* item = m_items.at(i);
        if (item == Q_NULL)
            break;
        if (item->right && item->widget->isVisible())
        {
            right = item->widget->geometry().left();
            break;
        }
    }
    return QRect(left, 0, right - left, p.height());
}

QRect DockTitleBarPrivate::calcGripRect(DockTitleBarStyleOption* option) const
{
    QTN_P(const DockTitleBar);
    int right = p.width();
    for (int i = 0; i < m_items.size(); ++i)
    {
        TitleBarItem* item = m_items.at(i);
        if (item == Q_NULL)
            break;
        if (item->right && item->widget->isVisible())
        {
            right = item->widget->geometry().left();
            break;
        }
    }
    int width = option->fontMetrics.horizontalAdvance(option->title) + option->fontMetrics.horizontalAdvance(QLatin1Char('X')) * 2;
    return QRect(QPoint(option->rcText.left() + width, option->rcText.top()), QPoint(right, option->rcText.bottom()));
}

QSize DockTitleBarPrivate::calcMinSize(QWidget* widget) const
{
    const QSize sizeHint = widget->sizeHint();
    const QSize minSizeHint = widget->minimumSizeHint();
    const QSize minSize = widget->minimumSize(); 
    const QSize maxSize = widget->maximumSize();
    const QSizePolicy sizePolicy = widget->sizePolicy();

    QSize size(0, 0);
    if (sizePolicy.horizontalPolicy() != QSizePolicy::Ignored) 
    {
        if (sizePolicy.horizontalPolicy() & QSizePolicy::ShrinkFlag)
            size.setWidth(minSizeHint.width());
        else
            size.setWidth(qMax(sizeHint.width(), minSizeHint.width()));
    }

    if (sizePolicy.verticalPolicy() != QSizePolicy::Ignored) 
    {
        if (sizePolicy.verticalPolicy() & QSizePolicy::ShrinkFlag) 
            size.setHeight(minSizeHint.height());
        else 
            size.setHeight(qMax(sizeHint.height(), minSizeHint.height()));
    }

    size = size.boundedTo(maxSize);
    if (minSize.width() > 0)
        size.setWidth(minSize.width());
    if (minSize.height() > 0)
        size.setHeight(minSize.height());
    return size.expandedTo(QSize(0, 0));
}

int DockTitleBarPrivate::titleHeight() const
{
    QTN_P(const DockTitleBar);
    QWidget* parent = p.parentWidget();
    m_boxLayout->activate();
    int height = m_boxLayout->sizeHint().height();
    QFontMetrics titleFontMetrics = parent->fontMetrics();
    int mw = p.style()->pixelMetric(QStyle::PM_DockWidgetTitleMargin, Q_NULL, &p);
    int heightFont = titleFontMetrics.boundingRect(QStringLiteral("X")).height();

    DockTitleBarPrivate::TitleBarItem* item = Q_NULL;
    for (int i = 0; i < m_items.size(); ++i)
    {
        item = m_items.at(i);
        if (!item)
            break;
        int itemHeight = qMin(calcMinSize(item->widget).height(), item->widget->maximumHeight());
        heightFont = qMax(heightFont, itemHeight);
    }
    int margin = 0;
    int left, top, right, bottom;
    m_boxLayout->getContentsMargins(&left, &top, &right, &bottom);
    if (left == top && top == right && right == bottom)
        margin = left;
    return qMax(height + 2 * mw, heightFont + mw) + margin / 2;
}

void DockTitleBarPrivate::updateLayout()
{
    QTN_P(DockTitleBar);
    if (m_boxLayout != Q_NULL)
        delete m_boxLayout;
    m_boxLayout = new QHBoxLayout(&p);
    m_boxLayout->setContentsMargins(QMargins());
    QBoxLayout* vbox = new QVBoxLayout;
    m_boxLayout->addLayout(vbox);

    vbox->addSpacing(3);
    QBoxLayout* left = new QHBoxLayout;
    vbox->addLayout(left);
    //add by zhangyang 20230912 最左侧新增一个弹簧，使得右侧的按钮宽高不变
    left->addStretch(1);

    left->addSpacing(2);
    left->setSpacing(1);
    //add by zhangyang 20230912 调整内部padding
//    left->setContentsMargins(6, 0, 6, 0);
    left->setContentsMargins(6, 6, 6, 6);

    int maxHeight = p.fontMetrics().height();

    int i = 0;
    TitleBarItem* item = Q_NULL;
    for (i = 0, item = nullptr; i < m_items.size(); ++i) 
    {
        item = m_items.at(i);
        if (!item || item->right)
            break;
        left->addWidget(item->widget, item->stretch);
        int itemHeight = qMin(calcMinSize(item->widget).height(), item->widget->maximumHeight());
        maxHeight = qMax(maxHeight, itemHeight);
    }

    left->addStretch(0);

    for (item = nullptr; i < m_items.size(); ++i) 
    {
        item = m_items.at(i);
        if (!item)
            break;
        left->addWidget(item->widget, item->stretch);
        int itemH = qMin(calcMinSize(item->widget).height(), item->widget->maximumHeight());
        maxHeight = qMax(maxHeight, itemH);
    }
    //add by zhangyang 20230912 不固定高度
//    left->addStrut(maxHeight);
    m_keepStrut = maxHeight;
    vbox->addSpacing(2);
    m_boxLayout->activate();
    p.update();
}


/* DockTitleBar */
DockTitleBar::DockTitleBar(QWidget* parent)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(DockTitleBar);
    QTN_D(DockTitleBar);
    d.init();
}

DockTitleBar::~DockTitleBar()
{
    QTN_FINI_PRIVATE();
}

bool DockTitleBar::isCloseButtonVisible() const
{
    QTN_D(const DockTitleBar);
    return d.m_closeButton;
}

void DockTitleBar::setCloseButtonVisible(bool visible)
{
    QTN_D(DockTitleBar);
    if (d.m_closeButton != visible)
    {
        d.m_closeButton = visible;
        updateButtons();
    }
}

bool DockTitleBar::isAutoHideButtonVisible() const
{
    QTN_D(const DockTitleBar);
    return d.m_autoHideButton;
}

void DockTitleBar::setAutoHideButtonVisible(bool visible)
{
    QTN_D(DockTitleBar);
    if (d.m_autoHideButton != visible)
    {
        d.m_autoHideButton = visible;
        updateButtons();
    }
}

void DockTitleBar::setAutoHideButtonState(bool state)
{
    QTN_D(DockTitleBar);
    if (DockWidgetTitleButton* button = d.titleButton(AutoHideButton))
    {
        if (button->isStateVisible() != state)
            button->setStateVisible(state);
    }
}

bool DockTitleBar::isMenuButtonVisible() const
{
    QTN_D(const DockTitleBar);
    return d.m_menuButton;
}

void DockTitleBar::setMenuButtonVisible(bool visible)
{
    QTN_D(DockTitleBar);
    if (d.m_menuButton != visible)
    {
        d.m_menuButton = visible;
        updateButtons();
    }
}

void DockTitleBar::setNoDrawGrip(bool draw)
{
    QTN_D(DockTitleBar);
    if (d.m_noDrawGrip != draw)
    {
        d.m_noDrawGrip = draw;
        update();
    }
}

void DockTitleBar::updateFeatures(DockWidgetPanel::DockPanelFeatures features)
{
    setCloseButtonVisible(features.testFlag(DockWidgetPanel::DockPanelClosable));
    setAutoHideButtonVisible(features.testFlag(DockWidgetPanel::DockPanelHideable));
    setMenuButtonVisible(features.testFlag(DockWidgetPanel::DockPanelMenuButton));
}

bool DockTitleBar::isTitleActive() const
{
    QTN_D(const DockTitleBar);
    return d.m_active;
}

QString DockTitleBar::text() const
{
    QTN_D(const DockTitleBar);
    return d.m_caption;
}

void DockTitleBar::setText(const QString& txt)
{
    QTN_D(DockTitleBar);
    if (text() != txt)
    {
        d.m_caption = txt;
        d.m_boxLayout->invalidate();
        update();
    }
}

QIcon DockTitleBar::icon() const
{
    QTN_D(const DockTitleBar);
    return QIcon(*d.m_icon->pixmap());
//    return d.m_icon->pixmap(Qt::ReturnByValue);
}

void DockTitleBar::setIcon(const QIcon& icon)
{
    QTN_D(const DockTitleBar);
    QStyleOption opt; opt.initFrom(this);
    int size = style()->pixelMetric(QStyle::PM_TitleBarButtonSize, &opt, this->parentWidget());
    QPixmap px = icon.pixmap(QSize(size, size));
    d.m_icon->setPixmap(px);
    d.m_icon->setVisible(true);
}

void DockTitleBar::setTitleActive(bool state)
{
    QTN_D(DockTitleBar);
    if (d.m_active != state && checkSetFocus())
    {
        d.m_active = state;
        update();
    }
}

QSize DockTitleBar::minimumSizeHint() const
{
    QTN_D(const DockTitleBar);
    QWidget* parentWidget = this->parentWidget();
    if (parentWidget == Q_NULL)
        return QSize(0, 0);
    QSize sz = d.m_boxLayout->sizeHint();
    sz.setHeight(d.titleHeight());
    return sz;
}

void DockTitleBar::initStyleOption(DockTitleBarStyleOption* option) const
{
    QTN_D(const DockTitleBar);
    if (!option)
        return;
    option->initFrom(this);
    
    if (d.m_active)
    {
        option->state |= QStyle::State_Active;
        option->palette.setCurrentColorGroup(QPalette::Active);
    }
    else
    {
        option->state &= ~QStyle::State_Active;
        option->palette.setCurrentColorGroup(QPalette::Inactive);
    }

    if (qobject_cast<DockToolBar*>(parentWidget()))
        option->state &= ~QStyle::State_Active;

    option->closable = d.m_closeButton;
    option->movable = d.m_movable;
    option->floatable = d.m_floatable;
    option->autohide = d.m_autoHideButton;
    option->menu = d.m_menuButton;
    option->title = text();
    option->rcText = d.calcTextRect();
    option->lineRect = d.m_noDrawGrip ? d.calcGripRect(option) : QRect();
}

bool DockTitleBar::checkSetFocus()
{
    QTN_D(DockTitleBar);
    bool focus = true;
    if (QWidget* wd = QApplication::focusWidget())
    {
        QWidget* widget = childAt(wd->mapFromGlobal(QCursor::pos()));

        if (QAbstractButton* button = qobject_cast<QAbstractButton*>(d.titleButton(CloseButton)))
            focus = widget != button && wd != button;
        if (focus)
        {
            if (QAbstractButton* button = qobject_cast<QAbstractButton*>(d.titleButton(AutoHideButton)))
                focus = widget != button && wd != button;
        }
    }
    return focus;
}

void DockTitleBar::updateButtons()
{
    QTN_D(DockTitleBar);
    if (QAbstractButton* button = qobject_cast<QAbstractButton*>(d.titleButton(CloseButton)))
        button->setVisible(d.m_closeButton);
    if (QAbstractButton* button = qobject_cast<QAbstractButton*>(d.titleButton(AutoHideButton)))
        button->setVisible(d.m_autoHideButton);
    if (QAbstractButton* button = qobject_cast<QAbstractButton*>(d.titleButton(MenuButton)))
        button->setVisible(d.m_menuButton);
}

/*! \reimp */
bool DockTitleBar::event(QEvent* event)
{
    return QWidget::event(event);
}

/*! \reimp */
void DockTitleBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    DockTitleBarStyleOption opt;
    initStyleOption(&opt);
    style()->drawControl(QStyle::CE_DockWidgetTitle, &opt, &painter, this);
}

/*! \reimp */
void DockTitleBar::actionEvent(QActionEvent* event)
{
    QTN_D(DockTitleBar);
    QAction* action = event->action();
    if (event->type() == QEvent::ActionAdded)
    { 
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action))
        {
            widgetAction->setParent(this);
            QWidget* widget = widgetAction->requestWidget(this);
            if (widget != Q_NULL) 
                widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            int index = d.insertWidgetToRightSide(1, widget, 0);
            DockTitleBarPrivate::TitleBarItem* item = d.m_items.at(index);
            item->action = action;
            item->customWidget = true;
            if (action->isVisible())
                widget->show();
        }
        else
        {
            bool ok = false;
            int typeButton = action->property(_qtn_Type_Button).toInt(&ok);
            if (ok)
            {
                DockWidgetTitleButton* button = new DockWidgetTitleButton(this);
                button->setDefaultAction(action);
                int index = d.insertWidgetToRightSide(d.m_items.size(), button, 0);
                DockTitleBarPrivate::TitleBarItem* item = d.m_items.at(index);
                item->action = action;
                if (typeButton == AutoHideButton)
                    QObject::connect(button, SIGNAL(clicked()), this, SIGNAL(autoHideClicked()));
                else if (typeButton == CloseButton)
                    QObject::connect(button, SIGNAL(clicked()), this, SIGNAL(closeClicked()));
                else if (typeButton == MenuButton)
                {
                    button->setPopupMode(QToolButton::InstantPopup);
                    QObject::connect(button, SIGNAL(clicked()), this, SIGNAL(menuClicked()));
                }
            }
            else
            {
                QToolButton* button = new QToolButton(this);
                button->setDefaultAction(action);
                int index = d.insertWidgetToRightSide(1, button);
                DockTitleBarPrivate::TitleBarItem* item = d.m_items.at(index);
                item->action = action;
            }
        }
    }
    else if (event->type() == QEvent::ActionRemoved)
    {
        bool found = false;
        for (int i = 0; i < d.m_items.size(); ++i) 
        {
            DockTitleBarPrivate::TitleBarItem* item = d.m_items.at(i);
            if (item == Q_NULL)
                break;
            if (item->action == action)
            {
                QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(item->action);
                if (widgetAction != Q_NULL && item->customWidget)
                    widgetAction->releaseWidget(item->widget);

                d.m_items.removeAt(i);
                Q_DELETE_AND_NULL(item->widget);
                Q_DELETE_AND_NULL(item);
                found = true;
                break;
            }
        }
        if (found)
            d.updateLayout();
    }
    QWidget::actionEvent(event);
}
