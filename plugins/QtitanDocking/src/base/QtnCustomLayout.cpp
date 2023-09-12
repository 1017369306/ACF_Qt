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

#include <QStyle>
#include <QEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>

#include "QtnCustomLayout.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

static inline void validateCustomBarArea(Qtitan::LayoutArea& area)
{
    switch (area) 
    {
        case Qtitan::LeftArea:
        case Qtitan::RightArea:
        case Qtitan::TopArea:
        case Qtitan::BottomArea:
            break;
        default:
            area = Qtitan::TopArea;
    }
}


CustomLayout::CustomLayout(QWidget* parent)
    : QLayout(parent)
    , m_customBarArea(Qtitan::DefaultArea)
    , m_customBar(Q_NULL)
    , m_centralWidget(Q_NULL)
    , m_statusBar(Q_NULL)
{
}

CustomLayout::~CustomLayout()
{
    delete m_customBar;
    delete m_statusBar;
    deleteCentralWidgetItem();
}

QWidget* CustomLayout::centralWidget() const
{
    return m_centralWidget ? m_centralWidget->widget() : Q_NULL;
}

void CustomLayout::setCentralWidget(QWidget* widget)
{
    QWidget* centralWidget = this->centralWidget();
    if (centralWidget && centralWidget != widget)
    {
        centralWidget->hide();
        removeWidget(centralWidget);
        centralWidget->deleteLater();
        // make sure we remove the widget
        deleteCentralWidgetItem();
    }

    if (widget != Q_NULL)
        addChildWidget(widget);

    m_centralWidget = widget ? new QWidgetItemV2(widget) : Q_NULL;
    invalidate();
}

QWidget* CustomLayout::customBar() const
{
    return m_customBar ? m_customBar->widget() : Q_NULL;
}

void CustomLayout::setCustomBar(QWidget* widget, Qtitan::LayoutArea area)
{
    validateCustomBarArea(area);
    removeItem(m_customBar);
    Q_DELETE_AND_NULL(m_customBar);
    if (widget != Q_NULL)
        addChildWidget(widget);
    m_customBar = widget ? new QWidgetItemV2(widget) : Q_NULL;
    m_customBarArea = area;
    invalidate();
}

Qtitan::LayoutArea CustomLayout::customBarArea() const
{
    return m_customBarArea;
}

void CustomLayout::setCustomBarArea(Qtitan::LayoutArea area)
{
    if (m_customBarArea == area)
        return;
    m_customBarArea = area;
    invalidate();
}

QStatusBar* CustomLayout::statusBar() const
{
    return m_statusBar ? qobject_cast<QStatusBar *>(m_statusBar->widget()) : Q_NULL;
}

void CustomLayout::setStatusBar(QStatusBar* statusBar)
{
    if (statusBar)
        addChildWidget(statusBar);
    delete m_statusBar;
    m_statusBar = statusBar ? new QWidgetItem(statusBar) : Q_NULL;
    invalidate();
}

int CustomLayout::calcItemHeight(const QWidget* wd)
{
    QFont fontRegular = wd->font();
    QFontMetrics fm(fontRegular);
    int rowItemHeight = qMax(fm.height() + ((fm.descent() + fm.ascent()) / 3), 17) + 3;

    QLineEdit ed;
    ed.setAttribute(Qt::WA_MacSmallSize);
    ed.setFont(fontRegular);
    QSize sizeHint = ed.sizeHint();
    rowItemHeight = qMax(sizeHint.height(), rowItemHeight);

    QComboBox box1;
    box1.setAttribute(Qt::WA_MacSmallSize);
    box1.setFont(fontRegular);
    sizeHint = box1.sizeHint();
    rowItemHeight = qMax(sizeHint.height(), rowItemHeight);

    QComboBox box2;
    box2.setAttribute(Qt::WA_MacSmallSize);
    box2.setEditable(true); //Affects on MacOSX widget
    box2.setFont(fontRegular);
    sizeHint = box2.sizeHint();
    rowItemHeight = qMax(sizeHint.height(), rowItemHeight);

    QSpinBox box3;
    box3.setAttribute(Qt::WA_MacSmallSize);
    box3.setFont(fontRegular);
    sizeHint = box3.sizeHint();
    rowItemHeight = qMax(sizeHint.height(), rowItemHeight);
    return rowItemHeight;
}

void CustomLayout::addItem(QLayoutItem* item)
{
    Q_UNUSED(item)
    qWarning("CustomLayout::addItem: Please do not use this function");
}

void CustomLayout::setGeometry(const QRect& r)
{
    QLayout::setGeometry(r);
    int left = 0, top = 0, right = 0, bottom = 0;
#if 0
    getContentsMargins(&left, &top, &right, &bottom);
#endif
    QRect rect = r.adjusted(left, top, -right, -bottom);
    
    if (m_statusBar)
    {
        QSize size = QSize(rect.width(), m_statusBar->heightForWidth(r.width())).expandedTo(m_statusBar->minimumSize());
        QRect sbr(QPoint(rect.left(), rect.top() + (rect.height() - size.height())), size);
        m_statusBar->setGeometry(sbr);
        rect.adjust(0, 0, 0, -size.height());
    }

    if (m_customBar)
    {
        QRect customBarRect;

        switch (m_customBarArea)
        {
            case Qtitan::LeftArea:
            {
                QSize size = QSize(m_customBar->sizeHint().width(), rect.height()).expandedTo(m_customBar->minimumSize());
                customBarRect = QRect(rect.topLeft(), size);
                rect.adjust(size.width(), 0, 0, 0);
            }
            break;
            case Qtitan::RightArea:
            {
                QSize size = QSize(m_customBar->sizeHint().width(), rect.height()).expandedTo(m_customBar->minimumSize());
                customBarRect = QRect(QPoint(rect.left() + (rect.width() - size.width()), rect.top()), size);
                rect.adjust(0, 0, -size.width(), 0);
            }
            break;
            case Qtitan::BottomArea:
            {
                QSize size = QSize(rect.width(), m_customBar->hasHeightForWidth() ? m_customBar->heightForWidth(r.width()) :
                    m_customBar->sizeHint().height()).expandedTo(m_customBar->minimumSize());
                customBarRect = QRect(QPoint(rect.left(), rect.top() + (rect.height() - size.height())), size);
                rect.adjust(0, 0, 0, -size.height());
            }
            break;
            default: //Qtitan::TopArea, Qtitan::DefaultArea
            {
                QSize size = QSize(rect.width(), m_customBar->hasHeightForWidth() ? m_customBar->heightForWidth(r.width()) :
                    m_customBar->sizeHint().height()).expandedTo(m_customBar->minimumSize());
                customBarRect = QRect(rect.topLeft(), size);
                rect.adjust(0, size.height(), 0, 0);
            }
            break;
        }
        m_customBar->setGeometry(customBarRect);
    }

    m_centralWidgetRect = rect;

    if (m_centralWidget && m_centralWidget->widget())
        m_centralWidget->setGeometry(m_centralWidgetRect);
}

QLayoutItem* CustomLayout::itemAt(int index) const
{
    if (index == 0)
        return m_centralWidget;
    return Q_NULL;
}

QLayoutItem* CustomLayout::takeAt(int index)
{
    if (index == 0)
    {
        QLayoutItem* ret = m_centralWidget;
        m_centralWidget = Q_NULL;
        return ret;
    }
    return Q_NULL;
}

int CustomLayout::count() const
{
    return 0;
}

QSize CustomLayout::sizeHint() const
{
    if (!m_szHint.isValid())
    {
        const QSize cbHint = m_customBar ? m_customBar->sizeHint() : QSize(0, 0);
        m_szHint = QSize(qMax(cbHint.width(), m_szHint.width()), cbHint.height() + m_szHint.height());

        const QSize cwHint = m_centralWidget && m_centralWidget->widget() ? m_centralWidget->sizeHint() : QSize(0, 0);
        m_szHint = QSize(qMax(cwHint.width(), m_szHint.width()), cwHint.height() + m_szHint.height());

        const QSize sbHint = m_statusBar ? m_statusBar->sizeHint() : QSize(0, 0);
        m_szHint = QSize(qMax(sbHint.width(), m_szHint.width()), sbHint.height() + m_szHint.height());
    }
    return m_szHint;
}

QSize CustomLayout::minimumSize() const
{
    if (!m_minSize.isValid()) 
    {
        m_minSize = QSize(0, 0);
        const QSize cbMin = m_customBar ? m_customBar->minimumSize() : QSize(0, 0);
        m_minSize = QSize(qMax(cbMin.width(), m_minSize.width()), cbMin.height() + m_minSize.height());

        const QSize cwMin = m_centralWidget && m_centralWidget->widget() ? m_centralWidget->minimumSize() : QSize(0, 0);
        m_minSize = QSize(qMax(cwMin.width(), m_minSize.width()), cwMin.height() + m_minSize.height());

        const QSize sbMin = m_statusBar ? m_statusBar->minimumSize() : QSize(0, 0);
        m_minSize = QSize(qMax(sbMin.width(), m_minSize.width()), sbMin.height() + m_minSize.height());
    }
    return m_minSize;
}

void CustomLayout::invalidate()
{
    QLayout::invalidate();
    m_minSize = QSize();
    m_szHint = QSize();
}

void CustomLayout::deleteCentralWidgetItem()
{
    Q_DELETE_AND_NULL(m_centralWidget)
}
