/****************************************************************************
**
** Qtitan Library by Developer Machines (Fluent Design Style implementation for Qt.C++)
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

#include <QMainWindow>
#include <QLabel>
#include <QStyleFactory>

#include "QtnWindowsColor.h"
#include "QtnFluentDesignStylePrivate.h"
#include "QtnOfficeStyle.h"

/*!
\enum ColorizedStyle::Theme
The enumerator describes the set of the predefined color themes.
\inmodule QtitanStyle
\value Light Simple light colored theme.
\value Dark Simple dark colored theme.
\value Blue Blue color theme.
\value LightBlue Light blue color theme.
\value Red Red color theme.
\value Custom Indicates that some of the colour was changed by the user.
*/

/*!
\class ColorizedStyle
\inmodule QtitanStyle
\brief ColorizedStyle class provides the base settings that can be used to customize application with the navigation ui.
*/
/*!
Constructor of the class ColorizedStyle. To create the style class, use:
\code
ColorizedStyle* style = new ColorizedStyle();
style->setTheme(ColorizedStyle::Red);
qApp->setStyle(style);
\endcode
*/
ColorizedStyle::ColorizedStyle()
    : CommonStyle(*new ColorizedStylePrivate)
{
    initStyleResource();
    QTN_D(ColorizedStyle);
    d.updateTheme();
}

ColorizedStyle::~ColorizedStyle()
{
}

ColorizedStyle::Theme ColorizedStyle::theme() const
{
    QTN_D(const ColorizedStyle);
    return d.m_theme;
}

/*!
\property ColorizedStyle::theme
Sets the \a colour theme of the fluent style.
\sa ColorizedStyle::Theme
*/
void ColorizedStyle::setTheme(ColorizedStyle::Theme theme)
{
    QTN_D(ColorizedStyle);
    if (d.m_theme != theme)
    {
        d.beginUpdate();
        d.m_theme = theme;
        d.updateTheme();
        d.endUpdate();
    }
}

const QColor& ColorizedStyle::windowColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_windowColor;
}

void ColorizedStyle::setWindowColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_windowColor == color)
        return;
    d.beginUpdate();
    d.m_windowColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::windowTextColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_windowTextColor;
}

void ColorizedStyle::setWindowTextColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_windowTextColor == color)
        return;
    d.beginUpdate();
    d.m_windowTextColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::materialColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_materialColor;
}

void ColorizedStyle::setMaterialColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_materialColor == color)
        return;
    d.beginUpdate();
    d.m_materialColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::tileMenuColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_tileMenuColor;
}

void ColorizedStyle::setTileMenuColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_tileMenuColor == color)
        return;
    d.beginUpdate();
    d.m_tileMenuColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::menuColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_menuColor;
}

void ColorizedStyle::setMenuColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_menuColor == color)
        return;
    d.beginUpdate();
    d.m_menuColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::linkColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_linkColor;
}

void ColorizedStyle::setLinkColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_linkColor == color)
        return;
    d.beginUpdate();
    d.m_linkColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::itemColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_itemColor;
}

void ColorizedStyle::setItemColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_itemColor == color)
        return;
    d.beginUpdate();
    d.m_itemColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::itemTextColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_itemTextColor;
}

void ColorizedStyle::setItemTextColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_itemTextColor == color)
        return;
    d.beginUpdate();
    d.m_itemTextColor = color;
    d.endUpdate();
}

const QColor& ColorizedStyle::captionColor() const
{
    QTN_D(const ColorizedStyle);
    return d.m_captionColor;
}

void ColorizedStyle::setCaptionColor(const QColor& color)
{
    QTN_D(ColorizedStyle);
    if (d.m_captionColor == color)
        return;
    d.beginUpdate();
    d.m_captionColor = color;
    d.endUpdate();
}

void ColorizedStyle::polish(QPalette& palette)
{
    CommonStyle::polish(palette);

    //QPalette& darkPalette = palette;
    //darkPalette.setColor(QPalette::Window, Qt::black);
    //darkPalette.setColor(QPalette::WindowText, Qt::white);
    //darkPalette.setColor(QPalette::Base, Qt::black);
    //darkPalette.setColor(QPalette::AlternateBase, QColor(Qt::black).lighter());
    //darkPalette.setColor(QPalette::ToolTipBase, Qt::black);
    //darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    //darkPalette.setColor(QPalette::Text, Qt::white);

    //darkPalette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    //darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    //darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);

}

void ColorizedStyle::polish(QWidget* widget)
{
    CommonStyle::polish(widget);
    if (widget->inherits(QTITAN_META_CLASSNAME("NavigationMainWindow")) ||
        widget->inherits("QMainWindow"))
    {
        QFont f = QApplication::font("QMenu");
#ifdef Q_OS_WIN
        f.setFamily(QStringLiteral("Segoe UI"));
        f.setPointSize(11);
#else
#endif
        widget->setFont(f);
    }

    if (widget->inherits(QTITAN_META_CLASSNAME("NavigationSidePane")) ||
        widget->inherits(QTITAN_META_CLASSNAME("NavigationMenu")))
    {
#if 1
        QFont f = QApplication::font("QMenu");
#ifdef Q_OS_WIN
        f.setFamily(QStringLiteral("Segoe UI"));
        f.setPointSize(11);
#else
#endif
        widget->setFont(f);
#endif
    }
}

/* ColorizedStylePrivate */
ColorizedStylePrivate::ColorizedStylePrivate()
    : CommonStylePrivate()
{
}

ColorizedStylePrivate::~ColorizedStylePrivate()
{
}

void ColorizedStylePrivate::init()
{
    QTN_P(ColorizedStyle);
    p.setBaseStyle(QStyleFactory::create(QLatin1String("fusion")));
    m_theme = ColorizedStyle::Blue;

    CommonStylePrivate::init();
}

void ColorizedStylePrivate::initStandardPalette(QPalette& palette) const
{
    QTN_P(const ColorizedStyle);
    CommonStylePrivate::initStandardPalette(palette);

    QPalette_setColor(palette, QPalette::Window, p.windowColor());
    QPalette_setColor(palette, QPalette::WindowText, p.windowTextColor());
    QPalette_setColor(palette, QPalette::Base, p.tileMenuColor());
    QPalette_setColor(palette, QPalette::AlternateBase, p.tileMenuColor().lighter());

    QPalette_setColor(palette, QPalette::Button, p.itemColor());
    QPalette_setColor(palette, QPalette::ButtonText, p.itemTextColor());
    QPalette_setColor(palette, QPalette::BrightText, p.captionColor());

    QPalette_setColor(palette, QPalette::Light, p.windowTextColor());
    QPalette_setColor(palette, QPalette::Link, p.linkColor());
    QPalette_setColor(palette, QPalette::LinkVisited, p.linkColor());
}

QPalette ColorizedStylePrivate::getWidgetPalette(QWidget* widget) const
{
    QTN_P(const ColorizedStyle);
    QPalette palette = CommonStylePrivate::getWidgetPalette(widget);
    if (!widgetStyleSupport(widget))
        return palette;

    if (widget->inherits(QTITAN_META_CLASSNAME("NavigationMainWindow")) ||
        widget->inherits("QMainWindow"))
    {
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("NavigationStatusBar")) ||
        widget->inherits(QTITAN_META_CLASSNAME("NavigationBar")) ||
        widget->inherits(QTITAN_META_CLASSNAME("NavigationViewMenuItems")))
    {
        widget->setBackgroundRole(QPalette::Base);
        QPalette_setColor(palette, QPalette::Base, p.materialColor());
        QPalette_setColor(palette, widget->foregroundRole(), p.windowTextColor());
        QPalette_setColor(palette, QPalette::ButtonText, p.windowTextColor());
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("NavigationViewHeader")))
    {
        QPalette_setColor(palette, QPalette::BrightText, p.captionColor());
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("NavigationFrame")))
    {
        QPalette_setColor(palette, widget->foregroundRole(), p.itemTextColor());
        QPalette_setColor(palette, widget->backgroundRole(), p.menuColor());
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("NavigationMainMenu")))
    {
        QPalette_setColor(palette, widget->backgroundRole(), p.tileMenuColor());
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("NavigationBackstageMenu")) ||
        widget->inherits(QTITAN_META_CLASSNAME("NavigationSidePane")))
    {
        QPalette_setColor(palette, widget->foregroundRole(), p.itemTextColor());
        QPalette_setColor(palette, widget->backgroundRole(), p.menuColor());
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("NavigationMenuItem")))
    {
#if 0
        if (p.itemColor().isValid())
            QPalette_setColor(palette, QPalette::Button, p.itemColor());
        if (p.itemTextColor().isValid())
            QPalette_setColor(palette, QPalette::ButtonText, p.itemTextColor());
#endif
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("NavigationView"))/* ||
        widget->inherits(QTITAN_META_CLASSNAME("NavigationToolButton"))*/)
    {
        QPalette_setColor(palette, QPalette::ButtonText, p.windowTextColor());
    }

#if 0
    if (QLabel* l = qobject_cast<QLabel*>(widget))
    {
        if (l->textFormat() != Qt::PlainText)
        {
            QString text = l->text();
            l->clear();
            l->setText(text); //Reset text parser and update HTML CSS
        }
    }
#endif
    return palette;
}

//static const QColor QtnNavigationItemColor = QColor(0x000071C5);

void ColorizedStylePrivate::updateTheme()
{
    switch (m_theme)
    {
        case ColorizedStyle::Light:
        {
            m_windowColor = QColor(0xFFF0F0F0);
            m_windowTextColor = Qt::black;
            m_materialColor = m_windowColor;
            m_itemTextColor = Qt::black;
            m_itemColor = Qt::lightGray;
            m_menuColor = Qt::white;
            m_tileMenuColor = Qt::lightGray;
            QWidget* w = qApp->topLevelWidgets().size() > 0 ? qApp->topLevelWidgets()[0] : Q_NULL;
            WindowTitleBar* titleBar = WindowTitleBar::find(w);
            m_captionColor = titleBar != Q_NULL ? titleBar->frameColor() : qtn_getColorizationColor();
            m_linkColor = m_captionColor;
        }
        break;
        case ColorizedStyle::Dark:
        {
            m_windowColor = QColor(0xFF2d2d2d);
            m_windowTextColor = Qt::white;
            m_materialColor = Qt::black;
            m_itemTextColor = Qt::white;
            m_itemColor = Qt::darkGray;
            m_menuColor = Qt::black;
            m_tileMenuColor = Qt::darkGray;
            QWidget* w = qApp->topLevelWidgets().size() > 0 ? qApp->topLevelWidgets()[0] : Q_NULL;
            WindowTitleBar* titleBar = WindowTitleBar::find(w);
            m_captionColor = titleBar != Q_NULL ? titleBar->frameColor() : qtn_getColorizationColor();
            m_linkColor = m_captionColor;
        }
        break;
        case ColorizedStyle::Blue:
        {
            m_windowColor = Qt::darkGray;
            m_windowTextColor = WindowsColor(WindowsColor::MetroUI_White);
            m_materialColor = WindowsColor(WindowsColor::MetroUI_Sapphire);
            m_menuColor = WindowsColor(WindowsColor::MetroUI_White);
            m_tileMenuColor = Qt::lightGray;
            m_linkColor = Qt::black;
            m_itemColor = WindowsColor(WindowsColor::MetroUI_Sapphire);
            m_itemTextColor = Qt::black;
            m_captionColor = QColor(0x001160B7);
        }
        break;
        case ColorizedStyle::LightBlue:
        {
            m_windowColor = WindowsColor(WindowsColor::MetroUI_White);
            m_windowTextColor = WindowsColor(WindowsColor::MetroUI_Darken);
            m_materialColor = QColor(0x0000aaff);
            m_menuColor = WindowsColor(WindowsColor::MetroUI_White);
            m_tileMenuColor = QColor(0x00c9dfff);
            m_linkColor = QColor(0x0055aaff);
            m_itemColor = QColor(0x000093dc);
            m_itemTextColor = QColor(0x0055aaff);
            m_captionColor = QColor(0x0001c4ff);
        }
        break;
        case ColorizedStyle::Red:
        {
            m_windowColor = Qt::darkGray;
            m_windowTextColor = WindowsColor(WindowsColor::MetroUI_White);
            m_materialColor = WindowsColor(WindowsColor::MetroUI_Cardinal);
            m_menuColor = WindowsColor(WindowsColor::MetroUI_White);
            m_tileMenuColor = QColor(0x00ffaa7f);
            m_itemColor = WindowsColor(WindowsColor::MetroUI_DarkRed);
            m_linkColor = QColor(0x00cc1a1a);
            m_itemTextColor = QColor(0x00cc1a1a);
            m_captionColor = QColor(0x00cc1a1a);
        }
        break;
        default:
            break;
    };
}


QTITAN_USE_NAMESPACE

