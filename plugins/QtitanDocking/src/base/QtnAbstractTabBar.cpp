/****************************************************************************
**
** Qtitan Library by Developer Machines (Navigation UI for Qt.C++)
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
#include <QToolTip>
#include <QWhatsThis>
#include <QStylePainter>
#include <QMouseEvent>
#include <QApplication>
#include <QWidgetAction>

#include "QtnCommonStyle.h"
#include "QtnCommonStylePrivate.h"
#include "QtnAbstractTabBar.h"

QTITAN_USE_NAMESPACE

#define QTN_ANIMATION_DURATION 250

static QString _qtn_computeElidedText(Qt::TextElideMode mode, const QString& text)
{
    if (text.length() <= 3)
        return text;

    static const QLatin1String ellipses("...");
    QString ret;
    switch (mode)
    {
        case Qt::ElideRight:
            ret = text.left(2) + ellipses;
            break;
        case Qt::ElideMiddle:
            ret = text.left(1) + ellipses + text.right(1);
            break;
        case Qt::ElideLeft:
            ret = ellipses + text.right(2);
            break;
        case Qt::ElideNone:
            ret = text;
            break;
    }
    return ret;
}

static void _qtn_initStyleBaseOption(QStyleOptionTabBarBase* optTabBase, AbstractTabBar* tabbar, const QSize& size)
{
    QStyleOptionTab tabOverlap;
    tabOverlap.shape = static_cast<QTabBar::Shape>(QTabBar::RoundedNorth);
    int overlap = tabbar->style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &tabOverlap, tabbar);
    QWidget* theParent = tabbar->parentWidget();
    optTabBase->initFrom(tabbar);
    optTabBase->shape = static_cast<QTabBar::Shape>(QTabBar::RoundedNorth);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    optTabBase->documentMode = false;
#endif
    if (theParent && overlap > 0)
    {
        QRect rect;
        rect.setRect(0, size.height() - overlap, size.width(), overlap);
        optTabBase->rect = rect;
    }
}

QTITAN_BEGIN_NAMESPACE
/* DragTabAnimation */
struct DragTabAnimation : public QVariantAnimation
{
    DragTabAnimation(AbstractTab* tab, AbstractTabBar* tabBar) : m_tab(tab), m_privatTabBar(tabBar)
    {
        setEasingCurve(QEasingCurve::InOutQuad);
    }
    ~DragTabAnimation() {}
    virtual void updateCurrentValue(const QVariant& current)
    { m_privatTabBar->moveTabOffset(m_privatTabBar->m_tabList.indexOf(m_tab), current.toInt()); }
    virtual void updateState(State newState, State)
    { if (newState == Stopped) m_privatTabBar->moveTabStopped(m_privatTabBar->m_tabList.indexOf(m_tab)); }
private:
    AbstractTab* m_tab;
    AbstractTabBar* m_privatTabBar;
};
QTITAN_END_NAMESPACE

QTITAN_BEGIN_NAMESPACE
/* MovableTabWidget */
class MovableTabWidget : public QWidget
{
public:
    explicit MovableTabWidget(QWidget* parent = Q_NULL);
    void initialize(AbstractTab* tab);
protected:
    virtual void paintEvent(QPaintEvent* event);
private:
    QPixmap m_pixmap;
private:
    Q_DISABLE_COPY(MovableTabWidget)
};
QTITAN_END_NAMESPACE

/* MovableTabWidget */
MovableTabWidget::MovableTabWidget(QWidget* parent)
    : QWidget(parent)
{
    setFont(parent->font());
}

void MovableTabWidget::initialize(AbstractTab* tab)
{
    if (tab == Q_NULL)
    {
        Q_ASSERT(false);
        return;
    }
    QRect rect = tab->tabBar()->tabRect(tab->index());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QPixmap pixmap(rect.size() * devicePixelRatioF());
    pixmap.setDevicePixelRatio(devicePixelRatioF());
#else
    QPixmap pixmap(grabRect.size());
#endif
    pixmap.fill(Qt::transparent);

    QStylePainter painter(&pixmap, tab);
    painter.setFont(tab->font());
    tab->tabBar()->paintTab(&painter, tab);
    m_pixmap = pixmap;
    setGeometry(rect);
    update();
}

void MovableTabWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.drawPixmap(0, 0, m_pixmap);
}

QTITAN_BEGIN_NAMESPACE
/* TabBarContainer */
class TabBarContainer : public AbstractScrollWidgetBar
{
public:
    explicit TabBarContainer(AbstractTabBar* parent);
public:
    QPoint viewOffset() const;
    int getColumnCount() const override;
    QSize getColumnSize(int column) const override;
    void updateItemsLayout() override;
protected:
    void wheelEvent(QWheelEvent* event) override;
protected:
#if 0
    virtual void paintEvent(QPaintEvent* event)
    {
        Q_UNUSED(event);
        QPainter p(this);
        p.setPen(Qt::blue);
        p.drawRect(rect().adjusted(0, 0, -1, -1));
    }
#endif
    AbstractTabBar* m_tabBar;
private:
    friend class AbstractTabBar;
    Q_DISABLE_COPY(TabBarContainer);
};
QTITAN_END_NAMESPACE

TabBarContainer::TabBarContainer(AbstractTabBar* parent)
    : AbstractScrollWidgetBar(parent)
    , m_tabBar(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setPageIndent(0);
    setItemIndent(0);
    setMargins(QMargins(0, 0, 0, 0));
    setMoveType(MoveByItem);
}

QPoint TabBarContainer::viewOffset() const
{ 
    return getViewOffset();
}

int TabBarContainer::getColumnCount() const
{ 
    return m_tabBar->count();
}

QSize TabBarContainer::getColumnSize(int column) const
{
    const AbstractTab* tab = m_tabBar->tabAt(column);
    QSize ret = tab->size();
    return ret;
}

void TabBarContainer::updateItemsLayout()
{
}

void TabBarContainer::wheelEvent(QWheelEvent* event)
{
    QRect rect = geometry();
    QRect rectTab = m_tabBar->tabRect(m_tabBar->currentIndex());
    QPoint pnt = mapToParent(rectTab.topLeft());
    rectTab.moveTo(pnt);
    rectTab.adjust(0, 1, 0, -1);
    if (!rect.contains(rectTab))
        AbstractScrollWidgetBar::wheelEvent(event);
    else
        event->ignore();
}

/* AbstractTab */
AbstractTab::AbstractTab(AbstractTabBar* bar, const QIcon& icon, const QString& txt)
    : QWidget(bar)
    , m_shortcutId(0)
    , m_text(txt)
    , m_icon(icon)
    , m_hoverClose(false)
    , m_pressClose(false)
    , m_lastTab(-1)
    , m_dragOffset(0)
    , m_animation(Q_NULL)
    , m_tabBar(bar)
    , m_object(Q_NULL)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
}

AbstractTab::~AbstractTab()
{
    Q_DELETE_AND_NULL(m_animation);
}

void AbstractTab::startAnimation(AbstractTabBar* tabBar, int duration)
{
    if (!tabBar->isAnimated())
    {
        tabBar->moveTabStopped(tabBar->m_tabList.indexOf(this));
        return;
    }
    if (m_animation == Q_NULL)
        m_animation = new DragTabAnimation(this, tabBar);
    m_animation->setStartValue(m_dragOffset);
    m_animation->setEndValue(0);
    m_animation->setDuration(duration);
    m_animation->start();
}

AbstractTabBar* AbstractTab::tabBar() const
{ 
    return m_tabBar;
}

int AbstractTab::index() const
{
    return tabBar()->tabIndex(this);
}

QObject* AbstractTab::object() const 
{ 
    return m_object;
}

void AbstractTab::setObject(QObject* obj) 
{
    m_object = obj;
}

void AbstractTab::clicked()
{
    AbstractTabBar* _tabBar = tabBar();
    if (_tabBar != Q_NULL)
        _tabBar->setCurrentIndex(_tabBar->tabIndex(this));
}

void AbstractTab::dblClicked() {}

void AbstractTab::initStyleOption(StyleOptionTab* option) const
{
    initBasicStyleOption(option);
    AbstractTabBar* _tabBar = tabBar();
    QRect textRect = style()->subElementRect(QStyle::SE_TabBarTabText, option, this);
    option->text = fontMetrics().elidedText(option->text, _tabBar->m_elideMode, textRect.width(), Qt::TextShowMnemonic);
}

void AbstractTab::initBasicStyleOption(StyleOptionTab* option) const
{
    AbstractTabBar* _tabBar = tabBar();
    int tabIndex = _tabBar->tabIndex(this);
    const int count = _tabBar->count();
    if (!option || (tabIndex < 0 || tabIndex >= count))
        return;
    option->initFrom(this);
    option->state &= ~(QStyle::State_HasFocus/* | QStyle::State_MouseOver*/);
    const bool isCurrent = tabIndex == _tabBar->m_currentIndex;
    option->row = 0;
    if (tabIndex == _tabBar->m_pressedIndex)
        option->state |= QStyle::State_Sunken;
    if (isCurrent)
        option->state |= QStyle::State_Selected;
    if (isCurrent && hasFocus())
        option->state |= QStyle::State_HasFocus;
    if (!isEnabled())
        option->state &= ~QStyle::State_Enabled;
    if (isActiveWindow())
        option->state |= QStyle::State_Active;
    option->shape = static_cast<QTabBar::Shape>(QTabBar::RoundedNorth);
    option->text = m_text;

    if (m_textColor.isValid())
        QPalette_setColor(option->palette, foregroundRole(), m_textColor);
    option->icon = m_icon;
    option->iconSize = _tabBar->iconSize();

    if (_tabBar->m_tabsClosable)
    {
        option->hoverClose = m_hoverClose;
        int width = style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, Q_NULL, this);
        int height = style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, Q_NULL, this);
        option->rightButtonSize = QSize(width, height);
    }
    else
        option->rightButtonSize = QSize();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    option->documentMode = false;
#endif
    if (tabIndex > 0 && tabIndex - 1 == _tabBar->m_currentIndex)
        option->selectedPosition = QStyleOptionTab::PreviousIsSelected;
    else if (tabIndex + 1 < count && tabIndex + 1 == _tabBar->m_currentIndex)
        option->selectedPosition = QStyleOptionTab::NextIsSelected;
    else
        option->selectedPosition = QStyleOptionTab::NotAdjacent;

    const bool paintBeginning = (tabIndex == _tabBar->m_beginVisible) || (_tabBar->m_startDrag && tabIndex == _tabBar->m_pressedIndex + 1);
    const bool paintEnd = (tabIndex == _tabBar->m_endVisible) || (_tabBar->m_startDrag && tabIndex == _tabBar->m_pressedIndex - 1);
    if (paintBeginning)
    {
        if (paintEnd)
            option->position = QStyleOptionTab::OnlyOneTab;
        else
            option->position = QStyleOptionTab::Beginning;
    }
    else if (paintEnd)
        option->position = QStyleOptionTab::End;
    else
        option->position = QStyleOptionTab::Middle;
    option->tabIndex = tabIndex;
}

/*! \reimp */
QSize AbstractTab::sizeHint() const
{
    AbstractTabBar* _tabBar = tabBar();
    StyleOptionTab opt;
    initBasicStyleOption(&opt);
    opt.text = m_text;
    QSize iconSize = m_icon.isNull() ? QSize(0, 0) : opt.iconSize;
    int hframe = style()->pixelMetric(QStyle::PM_TabBarTabHSpace, &opt, _tabBar);
    int vframe = style()->pixelMetric(QStyle::PM_TabBarTabVSpace, &opt, _tabBar);
    const QFontMetrics fm = _tabBar->fontMetrics();
    int maxWidgetHeight = qMax(opt.leftButtonSize.height(), opt.rightButtonSize.height());
    int widgetWidth = 0;
    int widgetHeight = 0;
    int padding = 0;
    if (!opt.rightButtonSize.isEmpty())
    {
        padding += 4;
        widgetWidth += opt.rightButtonSize.width();
        widgetHeight += opt.rightButtonSize.height();
    }
    if (!opt.icon.isNull())
        padding += 4;

    const int textWidth = fm.size(Qt::TextShowMnemonic, m_text).width();
    QSize size = QSize(textWidth + iconSize.width() + hframe + widgetWidth + padding,
        qMax(maxWidgetHeight, qMax(fm.height(), iconSize.height())) + vframe);
    return style()->sizeFromContents(QStyle::CT_TabBarTab, &opt, size, _tabBar);
}

QSize AbstractTab::minimumSizeHint() const
{
    QString saveText = m_text;
    AbstractTab* tab = const_cast<AbstractTab*>(this);
    tab->m_text = _qtn_computeElidedText(tabBar()->elideMode(), saveText);
    QSize size = tab->sizeHint();
    tab->m_text = saveText;
    return size;
}

bool AbstractTab::isCurrent() const
{
    return this == tabBar()->currentTab();
}


/*! \reimp */
void AbstractTab::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    AbstractTabBar* _tabBar = tabBar();
    QStylePainter p(this);
    StyleOptionTab tab;
    initStyleOption(&tab);

    const bool isCurrent = (_tabBar->m_currentIndex == _tabBar->tabIndex(this));
    if (!isCurrent)
    {
        if (_tabBar->m_startDrag)
        {
            tab.rect.moveLeft(tab.rect.x() + m_dragOffset);
            tab.previewRect.moveLeft(tab.previewRect.x() + m_dragOffset);
        }
        p.drawControl(QStyle::CE_TabBarTab, tab);
    }
    else
    {
        if (!_tabBar->m_startDrag)
            p.drawControl(QStyle::CE_TabBarTab, tab);
    }
}

/*! \reimp */
void AbstractTab::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        clicked();

    if (m_hoverClose)
    {
        m_pressClose = true;
        event->accept();
        return;
    }
    else
        QWidget::mousePressEvent(event);
}

void AbstractTab::mouseMoveEvent(QMouseEvent* event)
{
    AbstractTabBar* _tabBar = tabBar();
    if (_tabBar->m_tabsClosable)
    {
        StyleOptionTab opt;
        initStyleOption(&opt);
        bool saveHoverClose = m_hoverClose;
        m_hoverClose = false;
        QRect rect = style()->subElementRect(QStyle::SE_TabBarTabRightButton, &opt, this);
        if (rect.contains(event->pos()))
        {
            m_hoverClose = true;
            update(rect);
            event->ignore();
            return;
        }
        if (m_hoverClose != saveHoverClose)
            update(rect);
    }
    QWidget::mouseMoveEvent(event);
}

/*! \reimp */
void AbstractTab::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton)
        return;
    update();
    if (m_pressClose && m_hoverClose)
    {
        m_hoverClose = false;
        AbstractTabBar* _tabBar = tabBar();
        _tabBar->closeButtonPressed(_tabBar->tabIndex(this));
        event->accept();
    }
}

/*! \reimp */
void AbstractTab::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dblClicked();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

/*! \reimp */
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void AbstractTab::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);
    update();
}
#else
void AbstractTab::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
}
#endif

/*! \reimp */
void AbstractTab::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    m_hoverClose = false;
    update();
}

/*! \reimp */
bool AbstractTab::event(QEvent* event)
{
    if (event->type() == QEvent::HoverMove)
        update();
    return QWidget::event(event);
}

QTITAN_BEGIN_NAMESPACE
/* LayoutWidget */
class LayoutWidget : public QWidget
{
public:
    explicit LayoutWidget(QWidget* parent);
    virtual ~LayoutWidget();
public:
    void setAlignmentLayout(Qt::Alignment alignment);
    void addWidget(QWidget* widget);
    void removeWidget(QWidget* widget);
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent* event)
    {
        Q_UNUSED(event);
#if 0
        QPainter painter(this);
        painter.fillRect(rect(), QColor(255, 0, 0));
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
#endif
    }
private:
    QHBoxLayout* m_layout;
private:
    Q_DISABLE_COPY(LayoutWidget)
};
QTITAN_END_NAMESPACE

/* LayoutWidget */
LayoutWidget::LayoutWidget(QWidget* parent) 
    : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setDirection(QBoxLayout::RightToLeft);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addStretch(0);
}

LayoutWidget::~LayoutWidget()
{
}

void LayoutWidget::setAlignmentLayout(Qt::Alignment alignment)
{
    Q_ASSERT(alignment == Qt::AlignLeft || alignment == Qt::AlignRight);
    if (alignment == Qt::AlignLeft)
        m_layout->setDirection(QBoxLayout::RightToLeft);
    else if (alignment == Qt::AlignRight)
        m_layout->setDirection(QBoxLayout::LeftToRight);
}

void LayoutWidget::addWidget(QWidget* widget)
{
    m_layout->addWidget(widget);
}

void LayoutWidget::removeWidget(QWidget* widget)
{
    m_layout->removeWidget(widget);
}

QSize LayoutWidget::sizeHint() const { return  QWidget::sizeHint(); }

void AbstractTabBar::init()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_elideMode = Qt::TextElideMode(style()->styleHint(QStyle::SH_TabBar_ElideMode, Q_NULL, this));
}

void AbstractTabBar::calcFirstLastVisible(int index, bool visible, bool remove)
{
    if (visible)
    {
        m_beginVisible = qMin(index, m_beginVisible);
        m_endVisible = qMax(index, m_endVisible);
    }
    else
    {
        if (remove || (index == m_beginVisible))
        {
            m_beginVisible = -1;
            for (int i = 0; i < m_tabList.count(); ++i)
            {
                if (!m_tabList.at(i)->isHidden())
                {
                    m_beginVisible = i;
                    break;
                }
            }
            if (m_beginVisible < 0)
                m_beginVisible = 0;
        }
        if (remove || (index == m_endVisible))
        {
            m_endVisible = -1;
            for (int i = m_tabList.count() - 1; i >= 0; --i)
            {
                if (!m_tabList.at(i)->isHidden())
                {
                    m_endVisible = i;
                    break;
                }
            }
        }
    }
}

int AbstractTabBar::selectNewCurrentIndexFrom(int fromIndex)
{
    int newindex = -1;
    for (int i = fromIndex; i < m_tabList.count(); ++i)
    {
        if (!tabAt(i)->isHidden() && tabAt(i)->isEnabled())
        {
            newindex = i;
            break;
        }
    }
    if (newindex < 0)
    {
        for (int i = fromIndex - 1; i > -1; --i)
        {
            if (!tabAt(i)->isHidden() && tabAt(i)->isEnabled())
            {
                newindex = i;
                break;
            }
        }
    }
    return newindex;
}

int AbstractTabBar::calcPosition(int from, int to, int index) const
{
    if (index == from)
        return to;
    int start = qMin(from, to);
    int end = qMax(from, to);
    if (index >= start && index <= end)
        index += (from < to) ? -1 : 1;
    return index;
}

void AbstractTabBar::moveTabPosition(int from, int to)
{
    if (from == to || !isIndexValid(from) || !isIndexValid(to))
        return;
    int preLocation = tabRect(from).x();
    int oldPressedPosition = 0;
    if (m_pressedIndex != -1)
        oldPressedPosition = m_tabList[m_pressedIndex]->geometry().x();

    int start = qMin(from, to);
    int end = qMax(from, to);
    int width = m_tabList[from]->geometry().width();
    if (from < to)
        width *= -1;

    bool rtl = isRightToLeft();
    for (int i = start; i <= end; ++i)
    {
        if (i == from)
            continue;

        QRect rcMove = m_tabList[i]->geometry();
        rcMove.moveLeft(m_tabList[i]->geometry().x() + width);
        m_tabList[i]->setGeometry(rcMove);

        int direction = -1;
        if (rtl)
            direction *= -1;
        if (m_tabList[i]->m_dragOffset != 0)
            m_tabList[i]->m_dragOffset += (direction * width);
    }

    QRect rcMoveFrom = m_tabList[from]->geometry();
    if (from < to)
        rcMoveFrom.moveLeft(m_tabList[to]->geometry().right() + 1);
    else
        rcMoveFrom.moveLeft(m_tabList[to]->geometry().left() - width);

    m_tabList[from]->setGeometry(rcMoveFrom);

    m_tabList.move(from, to);

    for (int i = 0; i < m_tabList.count(); ++i)
        m_tabList[i]->m_lastTab = calcPosition(from, to, m_tabList[i]->m_lastTab);

    m_currentIndex = calcPosition(from, to, m_currentIndex);
    if (m_pressedIndex != -1)
    {
        m_pressedIndex = calcPosition(from, to, m_pressedIndex);
        int newPressedPosition = m_tabList[m_pressedIndex]->geometry().left();
        int diff = oldPressedPosition - newPressedPosition;
        if (isRightToLeft())
            diff *= -1;
        m_pntStartDrag.setX(m_pntStartDrag.x() - diff);
    }
    int postLocation = tabRect(to).x();
    int length = postLocation - preLocation;
    m_tabList[to]->m_dragOffset -= length;
    m_tabList[to]->startAnimation(this, QTN_ANIMATION_DURATION);
}

AbstractTab* AbstractTabBar::tabAt(int index)
{ return isIndexValid(index) ? m_tabList[index] : Q_NULL; }

const AbstractTab* AbstractTabBar::tabAt(int index) const
{ return isIndexValid(index) ? m_tabList[index] : Q_NULL; }

int AbstractTabBar::tabIndex(const AbstractTab* tab) const
{
    Q_ASSERT(tab != Q_NULL);
    for (int i = 0, count = m_tabList.count(); count > i; i++)
    {
        if (m_tabList[i] == tab)
            return i;
    }
    return -1;
}

QRect AbstractTabBar::tabRect(int index) const
{
    if (const AbstractTab* tab = tabAt(index))
    {
        Q_ASSERT(!m_layoutDirty);
        if (tab->isHidden())
            return QRect();
        QRect rc = tab->geometry();
        rc = QStyle::visualRect(layoutDirection(), rect(), rc);
        if (m_barContainer != Q_NULL)
            rc.translate(m_barContainer->viewOffset());
        return rc;
    }
    return QRect();
}

int AbstractTabBar::indexAtPos(const QPoint& pnt) const
{
    if (tabRect(m_currentIndex).contains(pnt))
        return m_currentIndex;
    for (int i = 0; i < m_tabList.count(); ++i)
        if (m_tabList.at(i)->isEnabled() && tabRect(i).contains(pnt))
            return i;
    return -1;
}

int AbstractTabBar::tabAt(const QPoint& position) const
{
    if (isIndexValid(m_currentIndex) && tabRect(m_currentIndex).contains(position))
        return m_currentIndex;
    const int max = m_tabList.size();
    for (int i = 0; i < max; ++i)
    {
        if (tabRect(i).contains(position))
            return i;
    }
    return -1;
}

AbstractTabBar::SelectionBehavior AbstractTabBar::selectionBehaviorOnRemove() const
{ 
    return m_selectionBehaviorOnRemove; 
}

void AbstractTabBar::setSelectionBehaviorOnRemove(AbstractTabBar::SelectionBehavior behavior)
{ 
    m_selectionBehaviorOnRemove = behavior;
}

void AbstractTabBar::updateLayout()
{
    m_layoutDirty = false;
    int sumWidth = 0, sumCount = 0;
    QSize sh = sizeHint();
    for (int i = 0; i < m_tabList.count(); ++i)
    {
        AbstractTab* tab = m_tabList[i];
        if (tab->isHidden())
            continue;
        QSize size = tab->sizeHint();
        tab->m_maxSize = size.width();
        sumWidth += size.width();
        size = tab->minimumSizeHint();
        tab->m_minSize = size.width();
        sumCount++;
    }

    QRect rc = this->rect();
    if (m_customWidgetLayout != Q_NULL)
    {
        if (m_layoutDirection == QBoxLayout::LeftToRight)
        {
            int widthItem = 0;
            for (int i = 0; i < m_items.size(); ++i)
            {
                TabBarItem* item = m_items.at(i);
                if (item == Q_NULL)
                    break;
                if (!item->widget->isHidden())
                    widthItem += item->widget->sizeHint().width();
            }
            if (rc.width() > sumWidth + widthItem)
                rc.setWidth(sumWidth + widthItem);
            m_customWidgetLayout->setGeometry(rc);
        }
        else if (m_layoutDirection == QBoxLayout::RightToLeft)
            m_customWidgetLayout->setGeometry(rc);
    }

    QRect rect = calcCenterRect();
    int newWidth = 0;
    int available = rect.width();
    if (!rect.isEmpty() && sumCount > 0 && available < sumWidth)
        newWidth = (sumWidth - available) / sumCount;

    int leftOffset = 0;
    for (int i = 0; i < m_tabList.count(); ++i)
    {
        AbstractTab* tab = m_tabList[i];
        if (tab->isHidden())
            continue;

        QRect tabRect = QRect(QPoint(leftOffset, 0), QSize(tab->m_maxSize, sh.height()));
        if (newWidth > 0)
        {
            tabRect.setWidth(tabRect.width() - newWidth);
            if (tab->m_minSize >= tabRect.width())
                tabRect.setWidth(tab->m_minSize);
        }
        tab->setGeometry(tabRect);
        leftOffset += tabRect.width();
    }

    if (m_barContainer != Q_NULL)
    {
        if (m_barContainer->geometry() != rect)
            m_barContainer->setGeometry(rect);
        else
            m_barContainer->updateLayout();
    }
}

QSize AbstractTabBar::calcMinSize(QWidget* widget) const
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

QRect AbstractTabBar::calcCenterRect() const
{
    int left = 0;
    for (int i = 0; i < m_items.size(); ++i)
    {
        TabBarItem* item = m_items.at(i);
        if (item == Q_NULL)
            break;
        if (!item->right && !item->widget->isHidden())
            left += item->widget->geometry().right();
    }

    int right = width();
    for (int i = 0; i < m_items.size(); ++i)
    {
        TabBarItem* item = m_items.at(i);
        if (item == Q_NULL)
            break;
        if (item->right && !item->widget->isHidden())
        {
            right = item->widget->geometry().left();
            break;
        }
    }
    return QRect(left, 0, right - left, height());
}

void AbstractTabBar::updateLayoutWidgets()
{
    Q_DELETE_AND_NULL(m_customWidgetLayout);
    m_customWidgetLayout = new QHBoxLayout(Q_NULL);
    m_customWidgetLayout->setContentsMargins(QMargins());

    int i = 0;
    TabBarItem* item = Q_NULL;
    for (i = 0, item = Q_NULL; i < m_items.size(); ++i)
    {
        item = m_items.at(i);
        if (!item || item->right)
            break;
        m_customWidgetLayout->addWidget(item->widget, item->stretch, item->alignment);
    }

    m_customWidgetLayout->addStretch(0);

    for (item = Q_NULL; i < m_items.size(); ++i)
    {
        item = m_items.at(i);
        if (!item)
            break;
        m_customWidgetLayout->addWidget(item->widget, item->stretch, item->alignment);
    }
}

int AbstractTabBar::indexToLastRightWidget() const
{
    int i = m_items.size() - 1;
    for (; i >= 0; --i)
    {
        TabBarItem* item = m_items.at(i);
        if (!(item && item->right))
            break;
    }
    return i;
}

void AbstractTabBar::closeButtonPressed(int index)
{
    if (index != -1)
        emit tabCloseRequested(index);
}

void AbstractTabBar::createMovableTab()
{
    if (m_movableTabWidget == Q_NULL)
        m_movableTabWidget = new MovableTabWidget(this);
    m_movableTabWidget->initialize(tabAt(m_pressedIndex));
    m_movableTabWidget->raise();
    m_movableTabWidget->show();
}

void AbstractTabBar::paintTab(QPainter* painter, AbstractTab* tab) const
{
    StyleOptionTab optTab;
    tab->initStyleOption(&optTab);
    optTab.position = QStyleOptionTab::OnlyOneTab;
    style()->drawControl(QStyle::CE_TabBarTab, &optTab, painter, tab);
}

void AbstractTabBar::refresh()
{
    if (m_pressedIndex != -1 && m_movable &&
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QGuiApplication::mouseButtons() == Qt::NoButton)
#else
        QApplication::mouseButtons() == Qt::NoButton)
#endif
    {
        moveTabStopped(m_pressedIndex);
        if (!isIndexValid(m_pressedIndex))
            m_pressedIndex = -1;
    }

    if (!isVisible()) 
    {
        m_layoutDirty = true;
    }
    else 
    {
        updateLayout();
        update();
        updateGeometry();
    }
}

AbstractTabBar::AbstractTabBar(QWidget* parent)
    : QWidget(parent)
    , m_currentIndex(-1)
    , m_pressedIndex(-1)
    , m_previousIndex(-1)
    , m_beginVisible(0)
    , m_endVisible(-1)
    , m_layoutDirty(false)
    , m_tabsClosable(false)
    , m_movable(false)
    , m_startDrag(false)
    , m_selectionBehaviorOnRemove(AbstractTabBar::SelectRightTab)
    , m_movableTabWidget(Q_NULL)
    , m_barContainer(Q_NULL)
    , m_layoutDirection(QBoxLayout::RightToLeft)
    , m_customWidgetLayout(Q_NULL)
{
    init();
}

AbstractTabBar::~AbstractTabBar()
{
    Q_DELETE_AND_NULL(m_customWidgetLayout);
    while (!m_items.isEmpty())
    {
        TabBarItem* item = m_items.takeFirst();
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(item->action))
        {
            if (item->customWidget)
                widgetAction->releaseWidget(item->widget);
        }
        Q_DELETE_AND_NULL(item);
    }
}

int AbstractTabBar::addTab(const QString& text, QObject* object)
{
    return insertTab(-1, text, object);
}

int AbstractTabBar::addTab(const QIcon& icon, const QString& text, QObject* object)
{
    return insertTab(-1, icon, text, object);
}

int AbstractTabBar::insertTab(int index, const QString &text, QObject* object)
{
    return insertTab(index, QIcon(), text, object);
}

int AbstractTabBar::insertTab(int index, const QIcon& icon, const QString& text, QObject* object)
{
    QIcon iconTab = icon;
    if (iconTab.isNull())
    {
        QPixmap px = SegoeMDL2Font::pixmap(SegoeMDL2Font::PreviewLink, iconSize());
        if (!px.isNull())
            iconTab.addPixmap(px);
    }

    AbstractTab* tab = createTab(iconTab, text, object);
    if (m_barContainer != Q_NULL)
    {
        tab->setParent(m_barContainer->viewWidget());
        tab->show();
    }

    if (!isIndexValid(index))
    {
        index = m_tabList.count();
        m_tabList.append(tab);
    }
    else 
    {
        m_tabList.insert(index, tab);
    }

    tab->m_shortcutId = grabShortcut(QKeySequence::mnemonic(text));
    m_beginVisible = qMax(qMin(index, m_beginVisible), 0);
    m_endVisible = qMax(index, m_endVisible);

    refresh();
    
    if (m_tabList.count() == 1)
        setCurrentIndex(index);
    else if (index <= m_currentIndex) 
    {
        ++m_currentIndex;
        ++m_endVisible;
    }

    for (int i = 0; i < m_tabList.count(); ++i) 
    {
        if (m_tabList[i]->m_lastTab >= index)
            ++m_tabList[i]->m_lastTab;
    }
    return index;
}

void AbstractTabBar::removeTab(int index)
{
    if (!isIndexValid(index))
        return;

    if (m_startDrag)
        moveTabStopped(m_pressedIndex);

    releaseShortcut(m_tabList.at(index)->m_shortcutId);

    int newIndex = m_tabList[index]->m_lastTab;
    AbstractTab* tab = m_tabList[index];
    m_tabList.removeAt(index);
    delete tab;

    for (int i = 0; i < m_tabList.count(); ++i) 
    {
        if (m_tabList[i]->m_lastTab == index)
            m_tabList[i]->m_lastTab = -1;
        if (m_tabList[i]->m_lastTab > index)
            --m_tabList[i]->m_lastTab;
    }

    calcFirstLastVisible(index, false, true);

    if (index == m_currentIndex) 
    {
        m_currentIndex = -1; //Reset current index, we will have a new.
        if (m_tabList.size() > 0) 
        {
            switch (m_selectionBehaviorOnRemove) 
            {
                case SelectPreviousTab:
                    if (newIndex > index)
                        newIndex--;
                    if (isIndexValid(newIndex) && !m_tabList.at(newIndex)->isHidden())
                        break;
                    QTN_FALLTHROUGH();
                case SelectRightTab:
                    newIndex = qBound(m_beginVisible, index, m_endVisible);
                    break;
                case SelectLeftTab:
                    newIndex = qBound(m_beginVisible, index - 1, m_endVisible);
                    if (newIndex < 0)
                        newIndex = 0;
                    break;
                default:
                    break;
            }

            if (isIndexValid(newIndex))
            {
                int bump = m_tabList[newIndex]->m_lastTab;
                setCurrentIndex(newIndex);
                m_tabList[newIndex]->m_lastTab = bump;
            }
        }
        else 
        {
            emit currentChanged(-1);
        }
    }
    else if (index < m_currentIndex) 
        setCurrentIndex(m_currentIndex - 1);

    refresh();
}

bool AbstractTabBar::isTabEnabled(int index) const
{
    if (const AbstractTab* tab = tabAt(index))
        return tab->isEnabled();
    return false;
}

void AbstractTabBar::setTabEnabled(int index, bool enabled)
{
    if (AbstractTab* tab = tabAt(index))
    {
        if (enabled == tab->isEnabled())
            return;
        tab->setEnabled(enabled);
        setShortcutEnabled(tab->m_shortcutId, enabled);
        if (!enabled && index == m_currentIndex)
            setCurrentIndex(selectNewCurrentIndexFrom(index + 1));
        else if (enabled && !isTabVisible(m_currentIndex))
            setCurrentIndex(selectNewCurrentIndexFrom(index));
    }
}

bool AbstractTabBar::isTabVisible(int index) const
{
    if (const AbstractTab* tab = tabAt(index))
        return !tab->isHidden();
    return false;
}

void AbstractTabBar::setTabVisible(int index, bool visible)
{
    if (AbstractTab* tab = tabAt(index))
    {
        if (visible == !tab->isHidden())
            return;
        tab->setVisible(visible);
        setShortcutEnabled(tab->m_shortcutId, visible);
        calcFirstLastVisible(index, visible, false);
        if (!visible && index == m_currentIndex) 
        {
            const int newindex = selectNewCurrentIndexFrom(index + 1);
            setCurrentIndex(newindex);
        }
        refresh();
    }
}

QString AbstractTabBar::tabText(int index) const
{
    if (const AbstractTab* tab = tabAt(index))
        return tab->m_text;
    return QString();
}

void AbstractTabBar::setTabText(int index, const QString &text)
{
    if (AbstractTab* tab = tabAt(index))
    {
        tab->m_text = text;
        releaseShortcut(tab->m_shortcutId);
        tab->m_shortcutId = grabShortcut(QKeySequence::mnemonic(text));
        setShortcutEnabled(tab->m_shortcutId, tab->isEnabled());
        refresh();
    }
}

QColor AbstractTabBar::tabTextColor(int index) const
{
    if (const AbstractTab* tab = tabAt(index))
        return tab->m_textColor;
    return QColor();
}

void AbstractTabBar::setTabTextColor(int index, const QColor &color)
{
    if (AbstractTab* tab = tabAt(index))
    {
        tab->m_textColor = color;
        update(tabRect(index));
    }
}

QIcon AbstractTabBar::tabIcon(int index) const
{
    if (const AbstractTab* tab = tabAt(index))
        return tab->m_icon;
    return QIcon();
}

void AbstractTabBar::setTabIcon(int index, const QIcon & icon)
{
    if (AbstractTab* tab = tabAt(index))
    {
        bool simpleIconChange = (!icon.isNull() && !tab->m_icon.isNull());
        tab->m_icon = icon;
        if (simpleIconChange)
            update(tabRect(index));
        else
            refresh();
    }
}

void AbstractTabBar::setTabToolTip(int index, const QString& tip)
{
    if (AbstractTab* tab = tabAt(index))
        tab->m_toolTip = tip;
}

QString AbstractTabBar::tabToolTip(int index) const
{
    if (const AbstractTab* tab = tabAt(index))
        return tab->m_toolTip;
    return QString();
}

int AbstractTabBar::currentIndex() const
{
    if (isIndexValid(m_currentIndex))
        return m_currentIndex;
    return -1;
}

const AbstractTab* AbstractTabBar::currentTab() const
{
    int index = currentIndex();
    if (index != -1)
        return tabAt(index);
    return Q_NULL;
}

void AbstractTabBar::setCurrentIndex(int index)
{
    if (m_currentIndex == index)
        return;

    if (m_startDrag && m_pressedIndex != -1)
        return;

    bool changed = true;
    emit currentAboutToBeChanged(index, changed);
    if (!changed)
        return;

    update();

    int oldIndex = m_currentIndex;
    if (isIndexValid(index))
    {
        m_currentIndex = index;
        m_tabList[index]->m_lastTab = oldIndex;
        emit currentChanged(index);
    }

    if (m_barContainer != Q_NULL)
        m_barContainer->ensureVisible(m_currentIndex);
}

QSize AbstractTabBar::iconSize() const
{
    if (m_iconSize.isValid())
        return m_iconSize;
    int iconExtent = style()->pixelMetric(QStyle::PM_TabBarIconSize, Q_NULL, this);
    return QSize(iconExtent, iconExtent);

}

void AbstractTabBar::setIconSize(const QSize &size)
{
    m_iconSize = size;
    m_layoutDirty = true;
    update();
    updateGeometry();
}

int AbstractTabBar::count() const
{
    return m_tabList.count();
}

/*! \reimp */
bool AbstractTabBar::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) 
    {
        if (const AbstractTab* tab = tabAt(tabAt(static_cast<QHelpEvent*>(event)->pos())))
        {
            if (!tab->m_toolTip.isEmpty()) 
            {
                QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tab->m_toolTip, this);
                return true;
            }
        }
    }
    else if (event->type() == QEvent::Shortcut) 
    {
        QShortcutEvent* se = static_cast<QShortcutEvent *>(event);
        for (int i = 0; i < m_tabList.count(); ++i) 
        {
            const AbstractTab* tab = m_tabList.at(i);
            if (tab->m_shortcutId == se->shortcutId())
            {
                setCurrentIndex(i);
                return true;
            }
        }
    }
    return QWidget::event(event);
}

/*! \reimp */
void AbstractTabBar::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    if (m_layoutDirty)
        updateGeometry();
    updateLayout();
    if (m_barContainer != Q_NULL)
        m_barContainer->moveTo(currentIndex());
}

/*! \reimp */
void AbstractTabBar::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
#if 0
    if (m_layoutDirty)
        refresh();
    if (!isIndexValid(m_currentIndex))
        setCurrentIndex(0);
#endif
}

/*! \reimp */
void AbstractTabBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStyleOptionTabBarBase option;
    _qtn_initStyleBaseOption(&option, this, size());

    QPainter p(this);
    int selected = m_currentIndex;
    if (m_startDrag)
        selected = m_pressedIndex;
    for (int i = 0; i < m_tabList.count(); ++i)
        option.tabBarRect |= tabRect(i);
    option.selectedTabRect = tabRect(selected);

    paintTabBarFrame(&p, &option);

    if (selected >= 0)
    {
        QRect tabRect = this->tabRect(selected);
        if (m_tabList[selected]->m_dragOffset != 0)
            tabRect.moveLeft(tabRect.x() + m_tabList[selected]->m_dragOffset);
        if (m_startDrag)
        {
            int taboverlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, Q_NULL, this);
            m_movableTabWidget->setGeometry(tabRect.adjusted(-taboverlap, 0, taboverlap, 0));
        }
    }
}

void AbstractTabBar::moveTab(int from, int to)
{
    if (from == to || !isIndexValid(from) || !isIndexValid(to))
        return;
    m_tabList.move(from, to);
    int previousIndex = m_currentIndex;
    m_currentIndex = calcPosition(from, to, m_currentIndex);
    update();
    emit tabMoved(from, to);
    if (previousIndex != m_currentIndex)
        emit currentChanged(m_currentIndex);
}

void AbstractTabBar::moveTabOffset(int index, int offset)
{
    if (!isIndexValid(index))
        return;
    m_tabList[index]->m_dragOffset = offset;
    update();
}

/*! \reimp */
void AbstractTabBar::mousePressEvent(QMouseEvent* event)
{
    event->ignore();
    if (event->button() != Qt::LeftButton) 
        return;

    const QPoint pos = event->pos();
    Q_UNUSED(pos);
    if (m_pressedIndex != -1 && m_movable)
        moveTabStopped(m_pressedIndex);

    const int pressedIndex = indexAtPos(event->pos());
    if (!isIndexValid(pressedIndex))
        return;

    event->accept();
    m_pressedIndex = pressedIndex;
    if (m_movable)
    {
        m_pntStartDrag = event->pos();
        m_previousIndex = m_pressedIndex;
    }
}

/*! \reimp */
void AbstractTabBar::mouseMoveEvent(QMouseEvent* event)
{
    if (m_movable) 
    {
        if (m_pressedIndex != -1 && event->buttons() == Qt::NoButton)
            moveTabStopped(m_pressedIndex);
        if (!m_startDrag && m_pressedIndex != -1)
        {
            if ((event->pos() - m_pntStartDrag).manhattanLength() > QApplication::startDragDistance()) 
            {
                m_startDrag = true;
                createMovableTab();
            }
        }

        if (event->buttons() == Qt::LeftButton && m_startDrag && isIndexValid(m_pressedIndex))
        {
            int dragOffset = (event->pos().x() - m_pntStartDrag.x());
            m_tabList[m_pressedIndex]->m_dragOffset = dragOffset;

            QRect startingRect = tabRect(m_pressedIndex);
            startingRect.moveLeft(startingRect.x() + dragOffset);

            int overIndex;
            if (dragOffset < 0)
                overIndex = tabAt(startingRect.topLeft());
            else
                overIndex = tabAt(startingRect.topRight());

            if (overIndex != m_pressedIndex && overIndex != -1) 
            {
                int offset = 1;
                if (isRightToLeft())
                    offset *= -1;
                if (dragOffset < 0)
                {
                    dragOffset *= -1;
                    offset *= -1;
                }
                for (int i = m_pressedIndex; offset > 0 ? i < overIndex : i > overIndex; i += offset) 
                {
                    QRect overIndexRect = tabRect(overIndex);
                    int needsToBeOver = overIndexRect.width() / 2;
                    if (dragOffset > needsToBeOver)
                        moveTabPosition(i + offset, m_pressedIndex);
                }
            }
            update();
        }
    }

    if (event->buttons() != Qt::LeftButton) 
    {
        event->ignore();
        return;
    }
}

/*! \reimp */
void AbstractTabBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) 
    {
        event->ignore();
        return;
    }

    event->accept();

    if (stopAnimation())
    {
        if (m_previousIndex != m_pressedIndex)
            emit tabMoved(m_previousIndex, m_pressedIndex);
    }
    m_pressedIndex = -1;
    m_previousIndex = -1;
}

/*! \reimp */
void AbstractTabBar::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && stopAnimation())
    {
        moveTabPosition(m_pressedIndex, m_previousIndex);
        m_pressedIndex = -1;
        m_previousIndex = -1;
        event->ignore();
        return;
    }
    if (event->key() != Qt::Key_Left && event->key() != Qt::Key_Right) 
    {
        event->ignore();
        return;
    }

    event->accept();

    int offset = event->key() == (isRightToLeft() ? Qt::Key_Right : Qt::Key_Left) ? -1 : 1;
    setCurrentNextEnabledIndex(offset);
}

/*! \reimp */
void AbstractTabBar::wheelEvent(QWheelEvent* event)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#if 0
    int delta = (qAbs(event->angleDelta().x()) > qAbs(event->angleDelta().y()) ?
        event->angleDelta().x() : event->angleDelta().y());
#else
    int delta = event->angleDelta().x();
#endif
#else
    int delta = event->delta();
#endif
    int offset = delta > 0 ? -1 : 1;
    setCurrentNextEnabledIndex(offset);
    event->accept();
}

bool AbstractTabBar::isAnimated() const 
{ 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return style()->styleHint(QStyle::SH_Widget_Animation_Duration, Q_NULL, this) > 0;
#else
    return false;
#endif
}

bool AbstractTabBar::stopAnimation()
{
    if (m_movable && m_startDrag && isIndexValid(m_pressedIndex))
    {
        int length = m_tabList[m_pressedIndex]->m_dragOffset;
        int width = tabRect(m_pressedIndex).width();
        int duration = qMin(QTN_ANIMATION_DURATION, (qAbs(length) * QTN_ANIMATION_DURATION) / width);
        m_tabList[m_pressedIndex]->startAnimation(this, duration);
        m_startDrag = false;
        m_movableTabWidget->hide();
        m_pntStartDrag = QPoint();
        return true;
    }
    return false;
}

void AbstractTabBar::setCurrentNextEnabledIndex(int offset)
{
    for (int index = m_currentIndex + offset; isIndexValid(index); index += offset)
    {
        AbstractTab* tab = m_tabList.at(index);
        if (!tab->isHidden() && tab->isEnabled())
        {
            setCurrentIndex(index);
            break;
        }
    }
}

void AbstractTabBar::moveTabStopped(int index)
{
    bool cleanup = (m_pressedIndex == index) || (m_pressedIndex == -1) || !isIndexValid(index);
    bool allAnimationsFinished = true;

    for (int i = 0; allAnimationsFinished && i < m_tabList.count(); ++i)
    {
        const AbstractTab& tab = *m_tabList.at(i);
        if (tab.m_animation && tab.m_animation->state() == QAbstractAnimation::Running)
            allAnimationsFinished = false;
    }

    if (allAnimationsFinished && cleanup)
    {
        if (m_movableTabWidget != Q_NULL)
            m_movableTabWidget->hide();

        for (int i = 0; i < m_tabList.count(); ++i)
            m_tabList[i]->m_dragOffset = 0;

        if (m_pressedIndex != -1 && m_movable)
        {
            m_pressedIndex = -1;
            m_startDrag = false;
            m_pntStartDrag = QPoint();
        }
    }
    else
    {
        if (!isIndexValid(index))
            return;
        m_tabList[index]->m_dragOffset = 0;
    }
    update();
}

/*! \reimp */
void AbstractTabBar::changeEvent(QEvent* event)
{
    switch (event->type()) 
    {
        case QEvent::StyleChange:
        case QEvent::FontChange:
            break;
        default:
            break;
    }
    QWidget::changeEvent(event);
}

Qt::TextElideMode AbstractTabBar::elideMode() const
{ return m_elideMode; }

void AbstractTabBar::setElideMode(Qt::TextElideMode mode)
{
    m_elideMode = mode;
    refresh();
}

bool AbstractTabBar::tabsClosable() const
{ return m_tabsClosable; }

void AbstractTabBar::setTabsClosable(bool closable)
{
    if (m_tabsClosable == closable)
        return;
    m_tabsClosable = closable;
    updateLayout();
    update();
}

bool AbstractTabBar::isMovable() const
{ 
    return m_movable;
}

void AbstractTabBar::setMovable(bool movable)
{ 
    m_movable = movable;
}

bool AbstractTabBar::isScrollable() const
{ 
    return m_barContainer != Q_NULL; 
}

void AbstractTabBar::setScrollable(bool scrollable)
{
    if (isScrollable() == scrollable)
        return;

    if (scrollable)
        m_barContainer = new TabBarContainer(this);
    else
        Q_DELETE_AND_NULL(m_barContainer);

    for (int i = 0; i < m_tabList.count(); ++i)
        m_tabList.at(i)->setParent(m_barContainer == Q_NULL ? this : m_barContainer->viewWidget());
}

bool AbstractTabBar::scrollButtonsBeside() const
{
    if (!isScrollable())
        return false;
    return m_barContainer->scrollButtonsBeside();
}

void AbstractTabBar::setScrollButtonsBeside(bool beside)
{
    if (!isScrollable())
        return;
    return m_barContainer->setScrollButtonsBeside(beside);
}

void AbstractTabBar::addWidgetToLeftSide(QWidget* widget, int stretch, Qt::Alignment alignment)
{
    if (widget == Q_NULL)
        return;
    insertWidgetToLeftSide(0, widget, stretch, alignment);
}

int AbstractTabBar::insertWidgetToLeftSide(int index, QWidget* widget, int stretch, Qt::Alignment alignment)
{
    if (widget == Q_NULL)
        return -1;
    TabBarItem* item = new TabBarItem(widget, stretch, alignment, false);
    int idx = indexToLastRightWidget();
    if (index < 0 || index > m_items.size() || (idx >= 0 && index > idx + 1))
    {
        qWarning("AbstractTabBar::insertWidgetToLeftSide: Index out of range (%d), appending widget", index);
        index = m_items.size();
    }
    m_items.insert(index, item);
    updateLayoutWidgets();
    return index;
}

void AbstractTabBar::setDirectionRightSide(QBoxLayout::Direction direction)
{
    if (m_layoutDirection == direction)
        return;
    m_layoutDirection = direction;
    updateLayout();
}

void AbstractTabBar::addWidgetToRightSide(QWidget *widget, int stretch, Qt::Alignment alignment)
{
    if (widget == Q_NULL)
        return;
    insertWidgetToRightSide(indexToLastRightWidget() + 1, widget, stretch, alignment);
}

int AbstractTabBar::insertWidgetToRightSide(int index, QWidget *widget, int stretch, Qt::Alignment alignment)
{
    if (widget == Q_NULL)
        return -1;
    TabBarItem* item = new TabBarItem(widget, stretch, alignment, true);
    int idx = indexToLastRightWidget();
    if (index < 0 || index > m_items.size() || (idx >= 0 && index <= idx))
    {
        qWarning("AbstractTabBar::insertWidgetToRightSide: Index out of range (%d), appending widget", index);
        index = idx + 1;
    }
    m_items.insert(index, item);
    updateLayoutWidgets();
    return index;
}

void AbstractTabBar::removeWidget(QWidget* widget)
{
    if (widget == Q_NULL)
        return;
    bool found = false;
    for (int i = 0; i < m_items.size(); ++i)
    {
        TabBarItem* item = m_items.at(i);
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

/*! \reimp */
QSize AbstractTabBar::sizeHint() const
{
    QSize size;
    if (m_customWidgetLayout != Q_NULL)
    {
        m_customWidgetLayout->invalidate();
        size = m_customWidgetLayout->sizeHint();
    }
    StyleOptionTab opt;
    int vframe = style()->pixelMetric(QStyle::PM_TabBarTabVSpace, &opt, this);
    const QFontMetrics fm = this->fontMetrics();

    int height = fm.height() + vframe;
    size.setHeight(qMax(height, size.height()));
    return size;
}
