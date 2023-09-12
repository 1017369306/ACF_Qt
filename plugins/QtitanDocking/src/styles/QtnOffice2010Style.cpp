/****************************************************************************
**
** Qtitan Library by Developer Machines (Office Style implementation for Qt.C++)
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
#include <QPixmapCache>
#include <QStringBuilder>
#include <QDockWidget>
#include <QBitmap>
#include <QMdiArea>
#include <QPainter>
#include <qdrawutil.h>
#include <QDialog>
#include <QMenu>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif /* Q_OS_WIN*/
#include "QtnStyleHelperPrivate.h"
#include "QtnOffice2010StylePrivate.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/* Office2010StylePrivate */
Office2010StylePrivate::Office2010StylePrivate()
    : Office2007StylePrivate(), m_themeType(Office2010Style::Blue)
{
    m_fileButtonImageCount = 7;
}

Office2010StylePrivate::~Office2010StylePrivate()
{
}

/*! \internal */
void Office2010StylePrivate::init()
{
    Office2007StylePrivate::init();
}

/*! \internal */
bool Office2010StylePrivate::drawRibbonBar(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Office2007StylePrivate::drawRibbonBar(opt, painter, w);
    if (const RibbonStyleOption* optRibbon = qstyleoption_cast<const RibbonStyleOption*>(opt))
    {
        QRect rcRibbonClient = opt->rect;
        QRect rcRibbonTabs = optRibbon->rectTabBar;
        rcRibbonTabs.setLeft(rcRibbonClient.left());
        rcRibbonTabs.setRight(rcRibbonClient.right());
        painter->fillRect(rcRibbonTabs, Qt::white);

        QString imageName = widgetPixmapName(QStringLiteral("RibbonBar"), QStringLiteral("Image_RibbonTabBackgroundDwm"));
        if (!imageName.isEmpty())
        {
            QPixmap soImage(imageName);
            if (!soImage.isNull())
            {
                QRect rcSrc = sourceRectImage(soImage.rect(), 0, 1);
                QRect rcRibbonClient = opt->rect;
                QRect rcRibbonTabs = optRibbon->rectTabBar;

                int height = qMin(rcRibbonTabs.height(), rcSrc.height() + 1);
                rcSrc.setTop(rcSrc.bottom() - height);
                QRect rcArea(QPoint(rcRibbonClient.left(), rcRibbonTabs.bottom() - height),
                    QPoint(rcRibbonClient.right(), rcRibbonTabs.bottom()));
                drawImage(soImage, *painter, rcArea, rcSrc, QRect(QPoint(10, 0), QPoint(10, 0)), true);
            }
        }
    }
    return true;
}

/*! \internal */
bool Office2010StylePrivate::drawRibbonFrameGroups(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    return Office2007StylePrivate::drawRibbonFrameGroups(opt, painter, w);
}

/*! \internal */
bool Office2010StylePrivate::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonToolBarControl")))
    {
        QRect rect = opt->rect;
        rect.setWidth(3);
        return drawWidgetPixmap(QStringLiteral("RibbonToolBarSeparator"), QStringLiteral("Image_Separator"), rect, painter, 0, 1);
    }
    else if (widget && widget->inherits("QToolBar"))
    {
        QPoint p1, p2;
        if (opt->state & QStyle::State_Horizontal) 
        {
            p1 = QPoint(opt->rect.width() / 2, 0);
            p2 = QPoint(p1.x(), opt->rect.height());
        }
        else 
        {
            p1 = QPoint(0, opt->rect.height() / 2);
            p2 = QPoint(opt->rect.width(), p1.y());
        }
        painter->setPen(QPen(opt->palette.color(QPalette::Dark), 2));
        painter->drawLine(p1, p2);
        painter->setPen(QPen(opt->palette.color(QPalette::Light), 1));
        painter->drawLine(p1, p2);
        return true;
    }
    return Office2007StylePrivate::drawIndicatorToolBarSeparator(opt, painter, widget);
}

/*! \internal */
bool Office2010StylePrivate::drawShapedFrame(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageSeparator")))
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt))
#else
        if (const QStyleOptionFrame* f = qstyleoption_cast<const QStyleOptionFrame*>(opt))
#endif
        {
            bool ret = false;
            painter->fillRect(opt->rect, QColor(255, 255, 255));
            if (f->frameShape == QFrame::HLine)
            {
                const uchar _cb[] = { 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc };
                QBitmap cb = QBitmap::fromData(QSize(8, 8), _cb);
                QBrush brush(QColor(199, 200, 201), cb);
                QRect rc(QPoint(opt->rect.left(), opt->rect.height() / 2), QSize(opt->rect.width(), 1));
                painter->fillRect(rc, brush);
                ret = true;
            }
            else if (f->frameShape == QFrame::VLine)
            {
                QPixmap soImage = loadPixmap(QStringLiteral("RibbonBackstageSeparator"), QStringLiteral("Image_BackstagePaneSeparator"));
                if (!soImage.isNull())
                {
                    QRect rc(QPoint(opt->rect.width() / 2 - 1, opt->rect.top()), QSize(2, opt->rect.height()));
                    drawImage(soImage, *painter, rc, sourceRectImage(soImage.rect(), 0, 1), QRect(QPoint(0, 30), QPoint(0, 30)));
                }
                ret = true;
            }
            return ret;
        }
    }
    return Office2007StylePrivate::drawShapedFrame(opt, painter, widget);
}

/*! \internal */
bool Office2010StylePrivate::drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const Office2007Style);
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("StatusButton")))
    {
        if (m_themeType == Office2010Style::Black)
        {
            if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
            {
                if (opt->state & QStyle::State_MouseOver)
                {
                    QStyleOptionToolButton tButton = *toolbutton;
                    QPalette_setColor(tButton.palette, QPalette::ButtonText, toolbutton->palette.color(QPalette::HighlightedText));
                    p.QCommonStyle::drawControl(QStyle::CE_ToolButtonLabel, &tButton, painter, widget);
                    return true;
                }
            }
        }
    }
    return OfficeStylePrivate::drawToolButtonLabel(opt, painter, widget);
}

/*! \internal */
bool Office2010StylePrivate::drawRibbonTitleBarWidget(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const
{
    return Office2007StylePrivate::drawRibbonTitleBarWidget(opt, painter, w);
}

/*! \internal */
void Office2010StylePrivate::updateTheme()
{
    QTN_P(Office2007Style);
    QString themeName;
    switch (m_themeType)
    {
        case Office2010Style::Blue:
            themeName = QStringLiteral("Blue");
#if 0
            m_popupOptionsStyle = PopupOffice2003DrawHelper::OS_SYSTEMBLUE;
#endif
            break;
        case Office2010Style::Black:
            themeName = QStringLiteral("Black");
#if 0
            m_popupOptionsStyle = PopupOffice2003DrawHelper::OS_SYSTEMROYALE;
#endif
            break;
        case Office2010Style::Silver:
            themeName = QStringLiteral("Silver");
#if 0
            m_popupOptionsStyle = PopupOffice2003DrawHelper::OS_SYSTEMSILVER;
#endif
            break;
        default:
            Q_ASSERT(false);
            break;
    }
    p.setStyleConfig(QStringLiteral(":/res/Office2010/"), themeName);
}


/*!
\class Office2010Style
\inmodule QtitanStyle
\brief Office2010Style class implements Microsoft Office 2010 style.
*/

/*!
Constuctor of Office2010Style class.
*/
Office2010Style::Office2010Style()
    : OfficeStyle(*new Office2010StylePrivate)
{
    QTN_D(Office2010Style)
    d.updateTheme();
}

/*!
Destructor of Office2010Style class.
*/
Office2010Style::~Office2010Style()
{
}

Office2010Style::Theme Office2010Style::theme() const
{
    QTN_D(const Office2010Style)
    return static_cast<Office2010Style::Theme>(d.m_themeType);
}

/*!
\property Office2010Style::theme
Sets the \a theme of the office style. You can choose from Blue, Black, Silver or Aqua theme's families.
\sa Theme
*/
void Office2010Style::setTheme(Office2010Style::Theme theme)
{
    QTN_D(Office2010Style)
    if (d.m_themeType != theme)
    {
        d.beginUpdate();
        d.m_themeType = theme;
        d.updateTheme();
        d.endUpdate();
    }
}

