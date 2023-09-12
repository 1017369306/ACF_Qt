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
#include "QtnCommonStylePrivate.h"
#include "QtnStyleHelperPrivate.h"
#include "QtnStyleOption.h"
#include "QtnPlatform.h"
#include "QtnToolTip.h"
#ifdef Q_OS_WASM
#include "QtnScrollWidgetBar.h"
#endif
#include <math.h>
#include <QPixmapCache>
#include <QStringBuilder>
#include <QBitmap>
#include <QMenu>
#include <QFileInfo>
#include <QApplication>
#include <QStatusBar>
#include <QDialog>
#include <QMdiArea>
#include <QPainterPath>
#include <qdrawutil.h>

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QTextFrame>
#ifdef QTN_PRIVATE_HEADERS
#include <private/qstylesheetstyle_p.h>
#endif
#ifdef QTITAN_DEMO
#include "QtnDemo.h"
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QWindow>
#endif

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

static const int windowsSepHeight       = 9;    // height of the separator
static const int windowsItemFrame       = 2;    // menu item frame width
static const int windowsCheckMarkWidth  = 16;   // checkmarks width on windows
static const int windowsArrowHMargin    = 6;    // arrow horizontal margin
static const int windowsRightBorder     = 15;   // right border on windows
static const int windowsItemVMargin     = 8;    // menu item ver text margin
static const int windowsItemHMargin     = 3;    // menu item hor text margin

static const int systemPopupBorderLeft   = 1;
static const int systemPopupBorderRight  = 1;
static const int systemPopupBorderTop    = 18;
static const int systemPopupBorderBottom = 29;
static const qreal marginRatio = 2.81;

static const QString Image_NameGroupOption           = QStringLiteral("group_option");
static const QString Image_NameRibbonButtonPopupMenu = QStringLiteral("qaccess_button");
static const QString Image_NameShadedArrowDown       = QStringLiteral("indicator_shadedarrowdown");
static const QString Image_NameShadedArrowUp         = QStringLiteral("indicator_shadedarrowup");
static const QColor clrFileButton = QColor(0x19, 0x79, 0xCA);


QTITAN_USE_NAMESPACE

/*!
    \enum Qtitan::ContextColor
    This enum describes the colors for the ribbon context pages.
    \inmodule QtitanBase
    \value ContextColorNone Color is not used to highlight contextual tab
    \value ContextColorGreen Contextual tab is highlighted in green color
    \value ContextColorBlue Contextual tab is highlighted in blue color
    \value ContextColorRed Contextual tab is highlighted in red color
    \value ContextColorYellow Contextual tab is highlighted in yellow color
    \value ContextColorCyan Contextual tab is highlighted in cyan color
    \value ContextColorPurple Contextual tab is highlighted in purple color
    \value ContextColorOrange Contextual tab is highlighted in orange color
*/

/* QtitanAnimation */
QtitanAnimation::~QtitanAnimation()
{
    setWidget(Q_NULL);
}

void QtitanAnimation::setWidget(QWidget* widget)
{
    if (widget)
        widget->installEventFilter(this);
    else if (m_widget)
        m_widget->removeEventFilter(this);

    m_widget = widget;
}

void QtitanAnimation::paint(QPainter *painter, const QStyleOption *option)
{
    Q_UNUSED(option)
    Q_UNUSED(painter)
}

void QtitanAnimation::drawBlendedImage(QPainter* painter, QRect rect, qreal alpha)
{
    if (m_secondaryImage.isNull() || m_primaryImage.isNull())
        return;

    if (m_tempImage.isNull())
        m_tempImage = m_secondaryImage;

    const int a = qRound(alpha * 256.0);
    const int ia = 256 - a;
    const int sw = m_primaryImage.width();
    const int sh = m_primaryImage.height();
    const int bpl = m_primaryImage.bytesPerLine();
    switch (m_primaryImage.depth())
    {
        case 32:
        {
            uchar* mixed_data = m_tempImage.bits();
            const uchar* back_data = m_primaryImage.bits();
            const uchar* front_data = m_secondaryImage.bits();
            for (int sy = 0; sy < sh; sy++)
            {
                quint32* mixed = reinterpret_cast<quint32*>(mixed_data);
                const quint32* back = reinterpret_cast<const quint32*>(back_data);
                const quint32* front = reinterpret_cast<const quint32*>(front_data);
                for (int sx = 0; sx < sw; sx++)
                {
                    quint32 bp = back[sx];
                    quint32 fp = front[sx];
                    mixed[sx] = qRgba((qRed(bp)*ia + qRed(fp)*a) >> 8,
                        (qGreen(bp)*ia + qGreen(fp)*a) >> 8,
                        (qBlue(bp)*ia + qBlue(fp)*a) >> 8,
                        (qAlpha(bp)*ia + qAlpha(fp)*a) >> 8);
                }
                mixed_data += bpl;
                back_data += bpl;
                front_data += bpl;
            }
        }
    }
    painter->drawPixmap(rect, QPixmap::fromImage(m_tempImage, Qt::AutoColor));
}

bool QtitanAnimation::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_widget && event->type() == QEvent::Hide)
        setWidget(Q_NULL);

    return QObject::eventFilter(obj, event);
}

QtitanTransition::QtitanTransition()
    : QtitanAnimation()
{
}

QtitanTransition::~QtitanTransition()
{
}

void QtitanTransition::paint(QPainter* painter, const QStyleOption* option)
{
    qreal alpha = 1.0;
    if (m_duration > 0)
    {
        QTime current = QTime::currentTime();

        if (m_startTime > current)
            m_startTime = current;

        int timeDiff = m_startTime.msecsTo(current);
        alpha = static_cast<qreal>(timeDiff) / static_cast<qreal>(m_duration);
        if (timeDiff > m_duration)
        {
            m_running = false;
            alpha = 1.0;
        }
    }
    else
    {
        m_running = false;
    }
    drawBlendedImage(painter, option->rect, alpha);
}

/* ClickLightAnimation */
ClickLightAnimation::ClickLightAnimation(QWidget* w)
    : QVariantAnimation(w), m_w(w)
{
    setDuration(1000);
    setEasingCurve(QEasingCurve::InCirc);
    connect(this, SIGNAL(valueChanged(const QVariant&)), this, SLOT(animationChanged(const QVariant&)));
    connect(this, SIGNAL(finished()), this, SLOT(animationFinished()));
    reset();
}

ClickLightAnimation::~ClickLightAnimation()
{
}

void ClickLightAnimation::reset()
{
    stop();
    setStartValue(qreal(0.0));
    setEndValue(qreal(1.0));
    start();
}

void ClickLightAnimation::startClickAnimation(QWidget* w)
{
    QList<ClickLightAnimation*> list = w->findChildren<ClickLightAnimation*>();
    if (list.size() > 0)
        list[0]->reset();
    else
        new ClickLightAnimation(w);
}

void ClickLightAnimation::animationChanged(const QVariant &value)
{
    Q_UNUSED(value);
    m_w->update();
}

void ClickLightAnimation::animationFinished()
{
    deleteLater();
}

static inline qreal alpha_bounded(qreal alpha)
{
    if (alpha < 0)
        return 0;
    else if (alpha > 1)
        return 1;
    return alpha;
}

void QTITAN_PREPEND_NAMESPACE(qtn_clickLight)(const QWidget* widget)
{
    ClickLightAnimation::startClickAnimation(const_cast<QWidget *>(widget));
}

void QTITAN_PREPEND_NAMESPACE(qtn_paintLight)(const QWidget* widget, QPainter *p, const QRect& r)
{
    QColor light = widget->palette().light().color();
    light.setAlpha(0x80);
    QPoint pos = widget->mapFromGlobal(QCursor::pos()); /* rect().center() */
    Qt::MouseButtons buttons = qtn_getMouseButtons();
    bool pressed = widget->rect().contains(pos) && (buttons & Qt::LeftButton);

    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);

    if (qtn_isColorLight(light))
        p->setOpacity(pressed ? 0.7 : 0.5);
    else
        p->setOpacity(pressed ? 0.5 : 0.3);


    const qreal radius = qMax(r.width() / 2, r.height() / 2) * 2;
    const int border = qtn_DPIScaled(1, widget);

    QRadialGradient g(pos, radius, pos);

    qreal clickWave = 0.0;
    qreal wavesize = 0.2;

    QList<ClickLightAnimation*> list = widget->findChildren<ClickLightAnimation*>();
    if (list.size() > 0)
    {
        QVariant v = list[0]->currentValue();
        clickWave = v.toReal();
    }

    g.setColorAt(0.00, light);

    if (clickWave != 0)
    {
        QColor wave = light.lighter(100);
        wave.setAlphaF(alpha_bounded(light.alphaF() * (1 - (clickWave - wavesize))));
        g.setColorAt(alpha_bounded(clickWave - wavesize), wave);

        wave.setAlphaF(alpha_bounded((light.alphaF() + 0.10) * (1 - clickWave)));
        g.setColorAt(alpha_bounded(clickWave), wave.lighter(100));

        wave.setAlphaF(alpha_bounded(light.alphaF() * (1 - (clickWave + wavesize))));
        g.setColorAt(alpha_bounded(clickWave + wavesize), wave);
    }
    g.setColorAt(1.00, Qt::transparent);

    p->fillRect(r.adjusted(border, border, -border, -border), g);
    p->restore();
}

void QTITAN_PREPEND_NAMESPACE(qtn_paintLightFrame)(const QWidget* widget, QPainter *p, const QRect& r)
{
    QColor light = widget->palette().light().color();
#if 0
    light.setAlpha(0xF0);
#endif
    QPoint pos = widget->mapFromGlobal(QCursor::pos()); /* rect().center() */
    Qt::MouseButtons buttons = qtn_getMouseButtons();
    bool pressed = widget->rect().contains(pos) && (buttons & Qt::LeftButton);

    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setOpacity(pressed ? 0.7 : 0.4);

    const qreal radius = qMin(r.width(), r.height());
    const int border = qtn_DPIScaled(2, widget);

    QRadialGradient g(pos, radius, pos);
    g.setSpread(QGradient::PadSpread);

    g.setColorAt(0.00, light);
    g.setColorAt(1.00, Qt::transparent);

    QRect rect = r;

    QRegion frameRegion = rect;
    rect = rect.adjusted(border, border, -border, -border);
    frameRegion -= rect;
    p->setClipRegion(frameRegion);
    p->fillRect(r, g);
#if 0
    frameRegion = rect;
    rect = rect.adjusted(border, border, -border, -border);
    frameRegion -= rect;
    p->setClipRegion(frameRegion);
    p->setOpacity(p->opacity() * 0.90);
    p->fillRect(r, g);
#endif
    p->restore();
}

#ifdef _DEBUG_PALETTE
void QTITAN_PREPEND_NAMESPACE(dump_palette)(const QPalette& p, bool _out)
{
    if (!_out)
        return;
        
    qDebug() << QLatin1String("--- Dump palette ---");
    for (int role = 0; role < (int)QPalette::NColorRoles; ++role)
    {
        QString s;
        bool modified = false;
        
        if (role == QPalette::WindowText)
            s += QLatin1String("[WindowText]");

        else if (role == QPalette::Button)
            s += QLatin1String("[Button]");

        else if (role == QPalette::Light)
            s += QLatin1String("[Light]");

        else if (role == QPalette::Midlight)
            s += QLatin1String("[Midlight]");

        else if (role == QPalette::Dark)
            s += QLatin1String("[Dark]");

        else if (role == QPalette::Mid)
            s += QLatin1String("[Mid]");

        else if (role == QPalette::Text)
            s += QLatin1String("[Text]");

        else if (role == QPalette::BrightText)
            s += QLatin1String("[BrightText]");

        else if (role == QPalette::ButtonText)
            s += QLatin1String("[ButtonText]");

        else if (role == QPalette::Base)
            s += QLatin1String("[Base]");

        else if (role == QPalette::Window)
            s += QLatin1String("[Window]");

        else if (role == QPalette::Shadow)
            s += QLatin1String("[Shadow]");

        else if (role == QPalette::Highlight)
            s += QLatin1String("[Highlight]");

        else if (role == QPalette::HighlightedText)
            s += QLatin1String("[HighlightedText]");

        else if (role == QPalette::Link)
            s += QLatin1String("[Link]");

        else if (role == QPalette::LinkVisited)
            s += QLatin1String("[LinkVisited]");

        else if (role == QPalette::AlternateBase)
            s += QLatin1String("[AlternateBase]");

        else if (role == QPalette::NoRole)
            s += QLatin1String("[NoRole]");

        else if (role == QPalette::ToolTipBase)
            s += QLatin1String("[ToolTipBase]");

        else if (role == QPalette::ToolTipText)
            s += QLatin1String("[ToolTipText]");

        else if (role == QPalette::PlaceholderText)
            s += QLatin1String("[PlaceholderText]");

        for (int group = 0; group < (int)QPalette::NColorGroups; ++group)
        {
            if (group == QPalette::Active)
                s += QLatin1String("[Active]");
            if (group == QPalette::Inactive)
                s += QLatin1String("[Inactive]");
            if (group == QPalette::Disabled)
                s += QLatin1String("[Disabled]");

            if (p.isBrushSet((QPalette::ColorGroup)group, (QPalette::ColorRole)role))
            {
                QColor color = p.color((QPalette::ColorGroup)group, (QPalette::ColorRole)role);
                s += QString(QLatin1String("[%1]")).arg(color.name());
                modified = true;
            }
            else
            {
                s += QLatin1String("Default");

            }
        }

        if (modified)
            qDebug() << s;
    }
}

static void check_icon(const QImage& image)
{
    for (int x = 0; x < image.width(); ++x)
    {
        for (int y = 0; y < image.height(); ++y)
        {
            QRgb rgb = image.pixel(x, y);
            int alpha = qAlpha(rgb);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            if (r != 0 || g != 0 || b != 0)
            {
                //Q_ASSERT(false);
            }
            alpha = alpha;
        }
    }
}

class PaletteDialog : public QDialog
{
public:
    PaletteDialog(QWidget* parent, const QPalette& palette)
        : QDialog(parent), m_palette(palette)
    {
    }
protected:
    const QSize rectSize = { 280, 50 };
    void paintRect(QPainter* painter, const QString& text, const QColor& color)
    {
        QRect r = QRect(0, 0, rectSize.width(), rectSize.height());
        painter->fillRect(r, color);
        r.adjust(30, 15, -30, -15);
        painter->fillRect(r, Qt::white);
        painter->setPen(Qt::black);
        qtn_drawRect(painter, r);
        painter->drawText(r, text, QTextOption(Qt::AlignCenter));
    }
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);

        for (int role = 0; role < (int)QPalette::NColorRoles; ++role)
        {
            QString s;

            if (role == QPalette::WindowText)
                s = QLatin1String("[WindowText]");

            else if (role == QPalette::Button)
                s = QLatin1String("[Button]");

            else if (role == QPalette::Light)
                s = QLatin1String("[Light]");

            else if (role == QPalette::Midlight)
                s = QLatin1String("[Midlight]");

            else if (role == QPalette::Dark)
                s = QLatin1String("[Dark]");

            else if (role == QPalette::Mid)
                s = QLatin1String("[Mid]");

            else if (role == QPalette::Text)
                s = QLatin1String("[Text]");

            else if (role == QPalette::BrightText)
                s = QLatin1String("[BrightText]");

            else if (role == QPalette::ButtonText)
                s = QLatin1String("[ButtonText]");

            else if (role == QPalette::Base)
                s = QLatin1String("[Base]");

            else if (role == QPalette::Window)
                s = QLatin1String("[Window]");

            else if (role == QPalette::Shadow)
                s = QLatin1String("[Shadow]");

            else if (role == QPalette::Highlight)
                s = QLatin1String("[Highlight]");

            else if (role == QPalette::HighlightedText)
                s = QLatin1String("[HighlightedText]");

            else if (role == QPalette::Link)
                s = QLatin1String("[Link]");

            else if (role == QPalette::LinkVisited)
                s = QLatin1String("[LinkVisited]");

            else if (role == QPalette::AlternateBase)
                s = QLatin1String("[AlternateBase]");

            else if (role == QPalette::NoRole)
                s = QLatin1String("[NoRole]");

            else if (role == QPalette::ToolTipBase)
                s = QLatin1String("[ToolTipBase]");

            else if (role == QPalette::ToolTipText)
                s = QLatin1String("[ToolTipText]");

            else if (role == QPalette::PlaceholderText)
                s = QLatin1String("[PlaceholderText]");

            painter.save();
            for (int group = 0; group < (int)QPalette::NColorGroups; ++group)
            {
                QString text = s;
                if (group == QPalette::Active)
                    text += QLatin1String("[Active]");
                if (group == QPalette::Inactive)
                    text += QLatin1String("[Inactive]");
                if (group == QPalette::Disabled)
                    text += QLatin1String("[Disabled]");

                QColor color = m_palette.color((QPalette::ColorGroup)group, (QPalette::ColorRole)role);
                text += QString(QLatin1String("[%1]")).arg(color.name());

                paintRect(&painter, text, color);
                painter.translate(rectSize.width(), 0);
            }
            painter.restore();
            painter.translate(0, rectSize.height());

        }
    }
private:
    QPalette m_palette;
};

void QTITAN_PREPEND_NAMESPACE(show_palette_dialog)(const QPalette& p)
{
    PaletteDialog dialog(0, p);
    dialog.exec();
}

#endif

void QTITAN_PREPEND_NAMESPACE(qtn_set_shadecolor)(QImage& image, const QColor& color, const QColor& origColor)
{
#ifdef _DEBUG_PALETTE
    check_icon(image);
#endif
    const int r = origColor.red();
    const int g = origColor.green();
    const int b = origColor.blue();

    for (int x = 0; x < image.width(); ++x)
    {
        for (int y = 0; y < image.height(); ++y)
        {
            QRgb rgb = image.pixel(x, y);
            int alpha = qAlpha(rgb);
            if (alpha > 0 && qRed(rgb) == r && qGreen(rgb) == g && qBlue(rgb) == b)
            {
                rgb = qRgba(color.red(), color.green(), color.blue(), alpha);
                image.setPixel(x, y, rgb);
            }
        }
    }
}

bool QTITAN_PREPEND_NAMESPACE(qtn_hasWindowTitleBar)(const QWidget* widget)
{
    WindowTitleBar* titleBar = (widget != Q_NULL && widget->isWindow()) ? WindowTitleBar::find(widget) : Q_NULL;
    return titleBar != Q_NULL && titleBar->isVisible();
}

static bool toBool(const QString& str)
{
    return str == QStringLiteral("true") ? true : false;
}

static QPalette::ColorGroup qtn_stringToGroup(const QString& str)
{
    if (str == QStringLiteral("Normal"))             return QPalette::Normal;
    else if (str == QStringLiteral("Active"))        return QPalette::Active;
    else if (str == QStringLiteral("Disabled"))      return QPalette::Disabled;
    else if (str == QStringLiteral("Inactive"))      return QPalette::Inactive;
    else if (str == QStringLiteral("NColorGroups"))  return QPalette::NColorGroups;
    else if (str == QStringLiteral("Current"))       return QPalette::Current;
    else if (str == QStringLiteral("All"))           return QPalette::All;
    return QPalette::Normal;
}

static QPalette::ColorRole qtn_stringToRole(const QString& str)
{
    if (str == QStringLiteral("WindowText"))           return QPalette::WindowText;
    else if (str == QStringLiteral("Button"))          return QPalette::Button;
    else if (str == QStringLiteral("Light"))           return QPalette::Light;
    else if (str == QStringLiteral("Midlight"))        return QPalette::Midlight;
    else if (str == QStringLiteral("Dark"))            return QPalette::Dark;
    else if (str == QStringLiteral("Mid"))             return QPalette::Mid;
    else if (str == QStringLiteral("Text"))            return QPalette::Text;
    else if (str == QStringLiteral("BrightText"))      return QPalette::BrightText;
    else if (str == QStringLiteral("ButtonText"))      return QPalette::ButtonText;
    else if (str == QStringLiteral("Base"))            return QPalette::Base;
    else if (str == QStringLiteral("Window"))          return QPalette::Window;
    else if (str == QStringLiteral("Shadow"))          return QPalette::Shadow;
    else if (str == QStringLiteral("Highlight"))       return QPalette::Highlight;
    else if (str == QStringLiteral("HighlightedText")) return QPalette::HighlightedText;
    else if (str == QStringLiteral("Link"))            return QPalette::Link;
    else if (str == QStringLiteral("LinkVisited"))     return QPalette::LinkVisited;
    else if (str == QStringLiteral("AlternateBase"))   return QPalette::AlternateBase;
    else if (str == QStringLiteral("ToolTipBase"))     return QPalette::ToolTipBase;
    else if (str == QStringLiteral("ToolTipText"))     return QPalette::ToolTipText;
    else if (str == QStringLiteral("NColorRoles"))     return QPalette::NColorRoles;
    else if (str == QStringLiteral("WindowText"))      return QPalette::WindowText;
    else if (str == QStringLiteral("Window"))          return QPalette::Window;
    return QPalette::NoRole;
}

static CommonStylePrivate::ImageState qtn_stringToState(const QString& str)
{
    if (str == QStringLiteral("Black"))         return CommonStylePrivate::Black;
    else if (str == QStringLiteral("Gray"))     return CommonStylePrivate::Gray;
    else if (str == QStringLiteral("LtGray"))   return CommonStylePrivate::LtGray;
    else if (str == QStringLiteral("White"))    return CommonStylePrivate::White;
    else if (str == QStringLiteral("DkGray"))   return CommonStylePrivate::DkGray;
    else if (str == QStringLiteral("Black2"))   return CommonStylePrivate::Black2;
    else if (str == QStringLiteral("Color0"))   return CommonStylePrivate::Color0;
    else if (str == QStringLiteral("Color1"))   return CommonStylePrivate::Color1;
    else if (str == QStringLiteral("Color2"))   return CommonStylePrivate::Color2;
    else if (str == QStringLiteral("Color3"))   return CommonStylePrivate::Color3;
    return CommonStylePrivate::InvalidState;
}

PixelMetricData::PixelMetricData() : value(static_cast<uint>(CommonStylePrivate::InvalidMetric)), scalableDpi(true) {}

/* StyleConfigReader */
StyleConfigReader::StyleConfigReader(StyleConfig* config, const QString& pathRes, const QString& themeName)
    : m_pathRes(pathRes)
    , m_themeName(themeName)
    , m_config(config)
{
}

/*! \internal */
bool StyleConfigReader::read(QIODevice* device)
{
    Q_ASSERT(!m_pathRes.isEmpty());
    xml.setDevice(device);
    if (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("STYLE") && xml.attributes().value(QStringLiteral("version")) == QStringLiteral("1.0"))
            readStyle();
        else
            xml.raiseError(QObject::tr("The file is not an STYLE version 1.0 file."));
    }
    return !xml.error();
}

/*! \internal */
QString StyleConfigReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3").arg(xml.errorString()).
        arg(xml.lineNumber()).arg(xml.columnNumber());
}

/*! \internal */
void StyleConfigReader::readStyle()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("STYLE"));

    while (xml.readNextStartElement()) 
    {
        if (xml.name() == QStringLiteral("widget"))
            readWidget();
        else if (xml.name() == QStringLiteral("global"))
            readGlobal();
        else
            xml.skipCurrentElement();
    }
}

/*! \internal */
void StyleConfigReader::readWidget()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("widget"));

    QString className;
    className += xml.attributes().value(QStringLiteral("className")).toString();

    Q_ASSERT(className.size() > 0);
    WidgetData* widgetItem = new WidgetData;
    Q_ASSERT(!m_config->m_hashWidgetsData.contains(className));

    QString strHover = xml.attributes().value(QStringLiteral("hover")).toString();
    if (!strHover.isEmpty())
        widgetItem->hover = toBool(strHover);

    QString strAutoFillBackground = xml.attributes().value(QStringLiteral("autoFillBackground")).toString();
    if (!strAutoFillBackground.isEmpty())
        widgetItem->autoFillBackground = toBool(strAutoFillBackground);

    while (xml.readNextStartElement()) 
    {
        if (xml.name() == QStringLiteral("palette"))
            readPalette(widgetItem->m_paletteHash);
        else if (xml.name() == QStringLiteral("subControl"))
            readSubControl(widgetItem->m_subControlHash);
        else if (xml.name() == QStringLiteral("pixelMetric"))
            readPixelMetric(m_config->m_hashWidgetMetricsData[className]);
        else if (xml.name() == QStringLiteral("styleHint"))
            readStyleHint(m_config->m_hashWidgetStyleHint[className]);
        else if (xml.name() == QStringLiteral("pixmaps"))
            readWidgetPixmap(m_config->m_hashWidgetPixmap[className]);
        else
            xml.skipCurrentElement();
    }
    m_config->m_hashWidgetsData.insert(className, WidgetData(*widgetItem));
    delete widgetItem;
}

/*! \internal */
void StyleConfigReader::readGlobal()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("global"));
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("standardPixmap"))
            readStandardPixmap();
        else if (xml.name() == QStringLiteral("styleHint"))
            readStyleHint(m_config->m_hashGlobalStyleHint);
        else if (xml.name() == QStringLiteral("pixelMetric"))
            readPixelMetric(m_config->m_hashGlobalMetricsData);
        else if (xml.name() == QStringLiteral("palette"))
            readPalette(m_config->m_hashGlobalPalette);
        else
            xml.skipCurrentElement();
    }
}

/*! \internal */
void StyleConfigReader::readStandardPixmap()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("standardPixmap"));
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("colorState"))
        {
            QString strState = xml.attributes().value(QStringLiteral("theme")).toString();
            if (strState == m_themeName)
            {
                QString strState = xml.attributes().value(QStringLiteral("state")).toString();
                CommonStylePrivate::ImageState state = qtn_stringToState(strState);
                QString color = xml.readElementText();
                m_config->m_hashGlobalColorPixmap.insert(static_cast<uint>(state), QColor(color));
            }
            else
                xml.skipCurrentElement();
        }
        else if (xml.name() == QStringLiteral("pixmap") && m_config->m_hashGlobalColorPixmap.size() != 0)
        {
            QString strType = xml.attributes().value(QStringLiteral("id")).toString();
            QString namePng = xml.readElementText();
            m_config->m_hashGlobalStandardPixmap[strType].append(namePng);
        }
        else
            xml.skipCurrentElement();
    }
}

/*! \internal */
void StyleConfigReader::readWidgetPixmap(StandardPixmapHash& pixmapHash)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("pixmaps"));
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("pixmap"))
        {
            bool isHaveTheme = false;
            QString strTheme = xml.attributes().value(QStringLiteral("theme")).toString();
            if (!strTheme.isEmpty())
            {
                QStringList strThemeLst = strTheme.split(QLatin1Char(','));
                isHaveTheme = strThemeLst.contains(m_themeName);
            }
            if (isHaveTheme)
            {
                QString strType = xml.attributes().value(QStringLiteral("id")).toString();
                QString fullPath = m_pathRes;
                if (m_themeName != QStringLiteral(_qtn_SingleTheme))
                {
                    fullPath += m_themeName;
                    fullPath += QLatin1Char('/');
                }
                fullPath += xml.readElementText();
                pixmapHash[strType].append(fullPath);
            }
            else
                xml.skipCurrentElement();
        }
        else
            xml.skipCurrentElement();
    }
}

/*! \internal */
void StyleConfigReader::readPixelMetric(PixelMetricHash& pixelMetric)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("pixelMetric"));
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("metric"))
        {
            PixelMetricData* item = new PixelMetricData;
            QString strType = xml.attributes().value(QStringLiteral("number")).toString();

            QString strTheme = xml.attributes().value(QStringLiteral("scalableDpi")).toString();
            if (!strTheme.isEmpty())
                item->scalableDpi = toBool(strTheme);
            item->value = static_cast<uint>(xml.readElementText().toInt());
            pixelMetric.insert(strType, PixelMetricData(*item));
            delete item;
        }
        else
            xml.skipCurrentElement();
    }
}

/*! \internal */
void StyleConfigReader::readStyleHint(StyleHintHash& styleHint)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("styleHint"));
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("hint"))
        {
            QString strType = xml.attributes().value(QStringLiteral("number")).toString();
            styleHint[strType] = xml.readElementText().toInt();
        }
        else
            xml.skipCurrentElement();
    }
}

/*! \internal */
void StyleConfigReader::readSubControl(SubControlHash& subControlHash)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("subControl"));
    QString idName = xml.attributes().value(QStringLiteral("id")).toString();
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("palette"))
        {
            PaletteHash hashPalette;
            readPalette(hashPalette);
            if (!hashPalette.empty())
                subControlHash[idName] = hashPalette;
        }
    }
}

/*! \internal */
void StyleConfigReader::readPalette(PaletteHash& hashPalette)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("palette"));
    bool isHaveTheme = false;
    QString strTheme = xml.attributes().value(QStringLiteral("theme")).toString();
    if (!strTheme.isEmpty())
    {
        QStringList strThemeLst = strTheme.split(QLatin1Char(','));
        isHaveTheme = strThemeLst.contains(m_themeName);
    }

    if (!isHaveTheme)
    {
        xml.skipCurrentElement();
        return;
    }

    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("colors"))
            readColors(hashPalette);
        else
            xml.skipCurrentElement();
    }
}

/*! \internal */
void StyleConfigReader::readColors(PaletteHash& hashPalette)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QStringLiteral("colors"));

    QHash<QPalette::ColorRole, QBrush> hashBrush;
    QString colorGroup = xml.attributes().value(QStringLiteral("colorGroup")).toString();
    QPalette::ColorGroup group = qtn_stringToGroup(colorGroup);
    while (xml.readNextStartElement())
    {
        if (xml.name() == QStringLiteral("color"))
        {
            QString colorRole = xml.attributes().value(QStringLiteral("colorRole")).toString();
            QPalette::ColorRole role = qtn_stringToRole(colorRole);
            QString strColor = xml.readElementText();
            hashBrush[role] = QBrush(QColor(strColor));
        }
        else
            xml.skipCurrentElement();
    }
    hashPalette[group] = hashBrush;
}



/* StyleConfig */
StyleConfig::StyleConfig()
{
}

/*! \internal */
bool StyleConfig::load(const QString& pathRes, const QString& themeName)
{
    QPixmapCache::clear();
    clear();
    QString fileName = pathRes;
    if (fileName.isEmpty())
        return false;
    if (themeName == QStringLiteral("Default"))
        fileName += QStringLiteral("default_styleconfig.xml");
    else
        fileName += QStringLiteral("styleconfig.xml");

    QFileInfo fileInfo(fileName);
    if (!fileInfo.completeSuffix().isEmpty() && !fileInfo.exists())
        return false;

    QFile file(fileInfo.absoluteFilePath());
    file.open(QIODevice::ReadOnly);

    StyleConfigReader reader(this, pathRes, themeName);
    if (!reader.read(&file))
        return false;
    return true;
}

/*! \internal */
void StyleConfig::setWidgetAttribute(QWidget* wd, const WidgetData& data)
{
    if (data.hover != -1)
        wd->setAttribute(Qt::WA_Hover, data.hover);
    if (data.autoFillBackground != -1)
        wd->setAutoFillBackground(data.autoFillBackground);
}

/*! \internal */
const QByteArray StyleConfig::getMetaObjectClassName(const QString& xmlClassName) const
{
    QString strClassName = xmlClassName;
    int index = strClassName.indexOf(QLatin1Char('Q'));
    if (index == 0)
        return strClassName.toLatin1();
    else
    {
#if !defined(QTITAN_NAMESPACE)
        return strClassName.toLatin1();
#else
        QString strQtitanClassName = QLatin1String("Qtitan::");
        strClassName = strQtitanClassName + strClassName;
        return strClassName.toLatin1();
#endif
    }
}

QPalette StyleConfig::getWidgetPalette(const QString& className) const
{
    QPalette palette;
    WidgetDataHash::const_iterator it = m_hashWidgetsData.constFind(className);
    if (it == m_hashWidgetsData.constEnd())
        return palette;

    const PaletteHash& paletteHash = it->m_paletteHash;
    for (uint group = 0; group <= QPalette::All; ++group)
    {
        PaletteHash::const_iterator group_it = paletteHash.constFind(static_cast<QPalette::ColorGroup>(group));
        if (group_it == paletteHash.constEnd())
            continue;
        const QHash<QPalette::ColorRole, QBrush>& groupHash = group_it.value();
        for (uint role = QPalette::WindowText; role <= QPalette::NColorRoles; ++role)
        {
            QHash<QPalette::ColorRole, QBrush>::const_iterator role_it = groupHash.constFind(static_cast<QPalette::ColorRole>(role));
            if (role_it == groupHash.constEnd())
                continue;
            const QBrush& brush = role_it.value();
            QPalette_setBrush(palette, static_cast<QPalette::ColorGroup>(group), static_cast<QPalette::ColorRole>(role), brush);
        }
    }
    return palette;
}

QPalette StyleConfig::getWidgetPalette(QWidget* widget) const
{
    QStringList list;
    const QMetaObject* metaObject = widget->metaObject();
    while (metaObject)
    {
        if (metaObject->className() != QObject::staticMetaObject.className())
            list << QString(QLatin1String(metaObject->className())).remove(QLatin1String(QTITAN_META_CLASSNAME("")));
        metaObject = metaObject->superClass();
    }

    foreach(QString className, list)
    {
        QPalette palette = getWidgetPalette(className);
        if (!palette.isCopyOf(QPalette()))
            return palette;
    }
    return QPalette();
}

#if 0
/*! \internal */
void StyleConfig::updateWidgetPalette(const QPalette& appPalette)
{
    QList<QString> classNames = m_hashWidgetsData.keys();
    for (QList<QString>::const_iterator it = classNames.cbegin(); it != classNames.cend(); ++it)
    {
        QString className = *it;
        QPalette palette = getWidgetPalette(className);
        QApplication::setPalette(palette, getMetaObjectClassName(className));
    }
}
#endif

/*! \internal */
void StyleConfig::updateWidgetAttribute(QWidget* widget)
{
    QStringList list;
    const QMetaObject* metaObject = widget->metaObject();
    while (metaObject)
    {
#if !defined(QTITAN_NAMESPACE)
        list.insert(0, QLatin1String(metaObject->className()));
#else
        list.insert(0, QString(QLatin1String(metaObject->className())).remove(QLatin1String(QTITAN_META_CLASSNAME(""))));
#endif
        metaObject = metaObject->superClass();
    }

    foreach(QString className, list)
    {
        if (!m_hashWidgetsData.contains(className))
            continue;
        const WidgetData& data = m_hashWidgetsData.value(className);
        setWidgetAttribute(widget, data);
    }
}

/*! \internal */
void StyleConfig::setPaletteFromGlobal(QPalette::ColorGroup gr, QPalette::ColorRole cr, QPalette& palette) const
{
    if (!m_hashGlobalPalette.contains(gr))
        return;
    const RoleBrushHash& hashPalette = m_hashGlobalPalette[gr];
    if (!hashPalette.contains(cr))
        return;
    QPalette_setBrush(palette, gr, cr, hashPalette.value(cr));
}

int StyleConfig::pixelMetric(const QWidget* widget, const QString& strWidget, const QString& numMetric, int defaultVal, bool* ok) const
{
    if (ok != Q_NULL)
        *ok = false;

    int val = defaultVal;
    if (numMetric.isEmpty())
        return val;

    PixelMetricData data;
    if (m_hashWidgetMetricsData.contains(strWidget) && m_hashWidgetMetricsData.value(strWidget).contains(numMetric))
        data = m_hashWidgetMetricsData.value(strWidget).value(numMetric);

    if (data.value == static_cast<uint>(CommonStylePrivate::InvalidMetric) && m_hashGlobalMetricsData.contains(numMetric))
        data = m_hashGlobalMetricsData.value(numMetric);

    if (data.value != static_cast<uint>(CommonStylePrivate::InvalidMetric))
    {
        val = static_cast<int>(data.value);
        if (data.scalableDpi)
            val = CommonStylePrivate::dpiScaled(val, widget);

        if (ok != Q_NULL)
            *ok = true;
    }
    return val;
}

QString StyleConfig::standardPixmapName(const QString& numPixmap, const QWidget* wd) const
{
    if (m_hashGlobalStandardPixmap.contains(numPixmap))
    {
        QString suffix = QtnPrivate::qtn_bestPngSize(wd);
        if (suffix.size() == 0)
            return QString();

        QStringList listName = m_hashGlobalStandardPixmap.value(numPixmap);
        foreach(QString name, listName)
        {
            if (name.contains(suffix))
                return name;
        }
    }
    return QString();
}

QString StyleConfig::widgetPixmapName(const QString& className, const QString& pixmapId) const
{
    if (!m_hashWidgetPixmap.contains(className))
        return QString();

    StandardPixmapHash pixmapHash = m_hashWidgetPixmap.value(className);
    if (!pixmapHash.contains(pixmapId))
        return QString();

    QStringList listName = pixmapHash.value(pixmapId);
    if (listName.count() > 0)
        return listName.at(0);
    return QString();
}

int StyleConfig::styleHint(const QString& strWidget, const QString& numMetric, int defaultVal) const
{
    int val = defaultVal;
    if (numMetric.isEmpty())
        return val;

    int data = CommonStylePrivate::InvalidHint;
    if (m_hashWidgetStyleHint.contains(strWidget) && m_hashWidgetStyleHint.value(strWidget).contains(numMetric))
        data = m_hashWidgetStyleHint.value(strWidget).value(numMetric);

    if (data == CommonStylePrivate::InvalidHint && m_hashGlobalStyleHint.contains(numMetric))
        data = m_hashGlobalStyleHint.value(numMetric);

    if (data != CommonStylePrivate::InvalidMetric)
        val = data;
    return val;
}

QColor StyleConfig::standardColorPixmap(uint state) const
{
    if (!m_hashGlobalColorPixmap.contains(state))
        return QColor();
    return m_hashGlobalColorPixmap.value(state);
}

/*! \internal */
QBrush StyleConfig::brushSubControl(const QString& className, const QString& subControlId,
    QPalette::ColorGroup group, QPalette::ColorRole role) const
{
    bool isHas = false;
    QBrush brush;
    if (className.isEmpty() || subControlId.isEmpty())
    {
        Q_ASSERT(false);
        return brush;
    }

    if (m_hashWidgetsData.contains(className))
    {
        const WidgetData& data = m_hashWidgetsData.value(className);
        PaletteHash hash = data.m_subControlHash.value(subControlId);
        if (hash.contains(group))
        {
            QHash<QPalette::ColorRole, QBrush> hashGroup = hash[group];
            if (hashGroup.contains(role))
            {
                brush = hashGroup[static_cast<QPalette::ColorRole>(role)];
                isHas = true;
            }
        }
    }
    if (!isHas)
    {
        QPalette pal = QApplication::palette(className.toLatin1());
        brush = pal.brush(group, role);
    }
    return brush;
}

/*! \internal */
QColor StyleConfig::colorSubControl(const QString& className, const QString& subControlId,
    QPalette::ColorGroup group, QPalette::ColorRole role) const
{
    QColor retColor;
    if (className.isEmpty() || subControlId.isEmpty())
    {
        Q_ASSERT(false);
        return retColor;
    }
    if (m_hashWidgetsData.contains(className))
    {
        const WidgetData& data = m_hashWidgetsData.value(className);
        PaletteHash hash = data.m_subControlHash.value(subControlId);
        if (hash.contains(group))
        {
            QHash<QPalette::ColorRole, QBrush> hashGroup = hash[group];
            if (hashGroup.contains(role))
                retColor = hashGroup[static_cast<QPalette::ColorRole>(role)].color();
        }
    }
    return retColor;
}

/*! \internal */
void StyleConfig::clear()
{
    m_hashGlobalPalette.clear();
    m_hashGlobalColorPixmap.clear();
    m_hashGlobalStandardPixmap.clear();
    m_hashGlobalStyleHint.clear();
    m_hashGlobalMetricsData.clear();

    m_hashWidgetPixmap.clear();
    m_hashWidgetStyleHint.clear();
    m_hashWidgetMetricsData.clear();
    m_hashWidgetsData.clear();
}

/*!
\class StandardSymbolIcon
\internal
*/
StandardSymbolIcon::StandardSymbolIcon()
{
}

/*! \internal */
SegoeMDL2Font::IconSymbol StandardSymbolIcon::symbolStandardPixmap(QStyle::StandardPixmap sp)
{
    switch (sp) 
    {
        case QStyle::SP_TitleBarMinButton:    return SegoeMDL2Font::ChromeMinimize;
        case QStyle::SP_TitleBarMaxButton:    return SegoeMDL2Font::ChromeMaximize;
        case QStyle::SP_TitleBarCloseButton:  return SegoeMDL2Font::ChromeClose;
        case QStyle::SP_TitleBarNormalButton: return SegoeMDL2Font::ChromeRestore;
        case QStyle::SP_TitleBarContextHelpButton: return SegoeMDL2Font::Help;
        case QStyle::SP_ArrowDown:            return SegoeMDL2Font::ChevronDown;
        default: break;
    }

    switch (static_cast<CommonStyle::StandardPixmapEx>(sp))
    {
        case CommonStyle::SP_TitleBarPinButton:        return SegoeMDL2Font::Pin;
        case CommonStyle::SP_TitleBarPinFillButton:    return SegoeMDL2Font::PinFill;
        case CommonStyle::SP_TitleBarPinnedButton:     return SegoeMDL2Font::Pinned;
        case CommonStyle::SP_TitleBarPinnedFillButton: return SegoeMDL2Font::PinnedFill;
        case CommonStyle::SP_NavigationBackButton:     return SegoeMDL2Font::ChromeBack;
        default: break;
    }
    return SegoeMDL2Font::None;
}

/*! \internal */
QPixmap StandardSymbolIcon::standardSymbolPixmap(QStyle::StandardPixmap sp, const QStyleOption* option, const QWidget* widget) const
{
    QColor symbolColor = Qt::black;
    if (option)
        symbolColor = option->state & QStyle::State_MouseOver ? option->palette.color(QPalette::HighlightedText) : option->palette.color(QPalette::WindowText);
    QSize s = option != Q_NULL && option->rect.size().isValid() ? option->rect.size() : QtnPrivate::qtn_bestImageSize(widget);
    return SegoeMDL2Font::pixmap(symbolStandardPixmap(sp), s, symbolColor);
}

QColor CommonStylePrivate::m_clrCloseButton          = QColor(199, 30, 30);
QColor CommonStylePrivate::m_clrCloseButtonHighlight = QColor(224, 67, 67);

/*!
\class CommonStylePrivate
\internal
*/
CommonStylePrivate::CommonStylePrivate()
    : m_config()
    , m_animationEnabled(false)
    , m_ignoreDialogs(false)
    , m_ignoreScrollBars(false)
{
}

CommonStylePrivate::~CommonStylePrivate()
{
}

/*! \internal */
void CommonStylePrivate::init()
{
    // resources initialization for rendering
    qtn_initializeDrawingResources();
    initBaseResource();
    SegoeMDL2Font();

    QTN_P(CommonStyle);
    p.setProperty("qtn_win32_glow", static_cast<uint>(2));

    if (!isQtitanStyle())
    {
        p.setStyleConfig(QStringLiteral(":/res/Base/"), QStringLiteral("Default"));

        static QColor colorNormalBkDark = QColor(255, 255, 255);
        static QColor colorDisableBkDark = QColor(154, 154, 154);
        static QColor colorDisable = QColor(90, 90, 90);
        m_stdIconsBlackHash[QStringLiteral("Image_GroupOption")] = CommonStylePrivate::loadIcon(Image_NameGroupOption, Q_NULL);
        m_stdIconsGrayHash[QStringLiteral("Image_GroupOption")] = QtnPrivate::qtn_ColorizedImage(m_stdIconsBlackHash[QStringLiteral("Image_GroupOption")], Qt::black, colorDisable);

        m_stdIconsBlackHash[QStringLiteral("Image_RibbonButtonPopupMenu")] = CommonStylePrivate::loadIcon(Image_NameRibbonButtonPopupMenu, Q_NULL);
        m_stdIconsGrayHash[QStringLiteral("Image_RibbonButtonPopupMenu")] = QtnPrivate::qtn_ColorizedImage(m_stdIconsBlackHash[QStringLiteral("Image_RibbonButtonPopupMenu")], Qt::black, colorDisable);
        m_stdIconsWhiteHash[QStringLiteral("Image_RibbonButtonPopupMenu")] = QtnPrivate::qtn_ColorizedImage(m_stdIconsBlackHash[QStringLiteral("Image_RibbonButtonPopupMenu")], Qt::black, colorNormalBkDark);
        m_stdIconsLtGrayHash[QStringLiteral("Image_RibbonButtonPopupMenu")] = QtnPrivate::qtn_ColorizedImage(m_stdIconsBlackHash[QStringLiteral("Image_RibbonButtonPopupMenu")], Qt::black, colorDisableBkDark);

        m_stdIconsBlackHash[QStringLiteral("Image_ShadedArrowDown")] = CommonStylePrivate::loadIcon(Image_NameShadedArrowDown, Q_NULL);
        m_stdIconsGrayHash[QStringLiteral("Image_ShadedArrowDown")] = QtnPrivate::qtn_ColorizedImage(m_stdIconsBlackHash[QStringLiteral("Image_ShadedArrowDown")], Qt::black, colorDisable);

        m_stdIconsBlackHash[QStringLiteral("Image_ShadedArrowUp")] = CommonStylePrivate::loadIcon(Image_NameShadedArrowUp, Q_NULL);
        m_stdIconsGrayHash[QStringLiteral("Image_ShadedArrowUp")] = QtnPrivate::qtn_ColorizedImage(m_stdIconsBlackHash[QStringLiteral("Image_ShadedArrowUp")], Qt::black, colorDisable);
    }
}

/*! \internal */
QPixmap CommonStylePrivate::standardPixmap(const QString& indxIcon, ImageState state, const QWidget* wd) const
{
    CommonStylePrivate* that = const_cast<CommonStylePrivate*>(this);
    that->updateStandardPixmap(indxIcon, wd);
    switch (state)
    {
        case Black:
            if (m_stdIconsBlackHash.contains(indxIcon))
                return m_stdIconsBlackHash.value(indxIcon);
            break;
        case Gray:
            if (m_stdIconsGrayHash.contains(indxIcon))
                return m_stdIconsGrayHash.value(indxIcon);
            break;
        case DkGray:
            if (m_stdIconsDkGrayHash.contains(indxIcon))
                return m_stdIconsDkGrayHash.value(indxIcon);
            break;
        case LtGray:
            if (m_stdIconsLtGrayHash.contains(indxIcon))
                return m_stdIconsLtGrayHash.value(indxIcon);
            break;
        case White:
            if (m_stdIconsWhiteHash.contains(indxIcon))
                return m_stdIconsWhiteHash.value(indxIcon);
            break;
        case Black2:
            if (m_stdIconsBlack2Hash.contains(indxIcon))
                return m_stdIconsBlack2Hash.value(indxIcon);
            break;
        case Color0:
            if (m_stdIconsColor0Hash.contains(indxIcon))
                return m_stdIconsColor0Hash.value(indxIcon);
            break;
        case Color1:
            if (m_stdIconsColor1Hash.contains(indxIcon))
                return m_stdIconsColor1Hash.value(indxIcon);
            break;
        case Color2:
            if (m_stdIconsColor2Hash.contains(indxIcon))
                return m_stdIconsColor2Hash.value(indxIcon);
            break;
        case Color3:
            if (m_stdIconsColor3Hash.contains(indxIcon))
                return m_stdIconsColor3Hash.value(indxIcon);
            break;
        default: break;
    }
    return QPixmap();
}

/*! \internal */
QString CommonStylePrivate::widgetPixmapName(const QString& className, const QString& pixmapId) const
{
    return m_config.widgetPixmapName(className, pixmapId);
}

/*! \internal */
void CommonStylePrivate::initStandardPalette(QPalette& palette) const
{
    for (int role = 0; role < (int)QPalette::NColorRoles; ++role)
    {
        for (int group = 0; group < (int)QPalette::NColorGroups; ++group)
        {
            QPalette::ColorGroup cg = static_cast<QPalette::ColorGroup>(group);
            QPalette::ColorRole cr = static_cast<QPalette::ColorRole>(role);
            setPaletteFromGlobal(cg, cr, palette);
        }
    }
#if 0 
    // QPalette::Active
    setPaletteFromGlobal(QPalette::Active, QPalette::Button, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::WindowText, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Window, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Base, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::AlternateBase, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Text, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::BrightText, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::ButtonText, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Light, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Midlight, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Dark, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Mid, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Shadow, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::Highlight, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::HighlightedText, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::ToolTipBase, palette);
    setPaletteFromGlobal(QPalette::Active, QPalette::ToolTipText, palette);
    // QPalette::Inactive
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Button, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::WindowText, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Window, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Base, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::AlternateBase, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Text, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::BrightText, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::ButtonText, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Light, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Midlight, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Dark, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Mid, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Shadow, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::Highlight, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::HighlightedText, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::ToolTipBase, palette);
    setPaletteFromGlobal(QPalette::Inactive, QPalette::ToolTipText, palette);
    // QPalette::Disabled
    setPaletteFromGlobal(QPalette::Disabled, QPalette::Button, palette);
    setPaletteFromGlobal(QPalette::Disabled, QPalette::WindowText, palette);
    setPaletteFromGlobal(QPalette::Disabled, QPalette::Window, palette);
    setPaletteFromGlobal(QPalette::Disabled, QPalette::ToolTipBase, palette);
    setPaletteFromGlobal(QPalette::Disabled, QPalette::ToolTipText, palette);
#endif
}

/*! \internal */
void CommonStylePrivate::updateStyle()
{
    m_stdIconsBlackHash.clear();
    m_stdIconsDkGrayHash.clear();
    m_stdIconsGrayHash.clear();
    m_stdIconsLtGrayHash.clear();
    m_stdIconsWhiteHash.clear();
    m_stdIconsBlack2Hash.clear();
    m_stdIconsColor0Hash.clear();
    m_stdIconsColor1Hash.clear();
    m_stdIconsColor2Hash.clear();
    m_stdIconsColor3Hash.clear();

    m_colorIconBlack  = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::Black));
    m_colorIconGray   = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::Gray));
    m_colorIconLtGray = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::LtGray));
    m_colorIconWhite  = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::White));
    m_colorIconDkGray = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::DkGray));
    m_colorIconBlack2 = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::Black2));
    m_colorIconColor0 = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::Color0));
    m_colorIconColor1 = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::Color1));
    m_colorIconColor2 = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::Color2));
    m_colorIconColor3 = m_config.standardColorPixmap(static_cast<uint>(CommonStylePrivate::Color3));
}

/*! \internal */
void CommonStylePrivate::updateStandardPixmap(const QString& indxIcon, const QWidget* wd)
{
    if (m_colorIconBlack.isValid() && !m_stdIconsBlackHash.contains(indxIcon))
        m_stdIconsBlackHash.insert(indxIcon, loadIcon(indxIcon, m_colorIconBlack, wd));

    if (m_colorIconGray.isValid() && !m_stdIconsGrayHash.contains(indxIcon))
        m_stdIconsGrayHash.insert(indxIcon, loadIcon(indxIcon, m_colorIconGray, wd));

    if (m_colorIconDkGray.isValid() && !m_stdIconsDkGrayHash.contains(indxIcon))
        m_stdIconsDkGrayHash.insert(indxIcon, loadIcon(indxIcon, m_colorIconDkGray, wd));

    if (m_colorIconLtGray.isValid() && !m_stdIconsLtGrayHash.contains(indxIcon))
        m_stdIconsLtGrayHash.insert(indxIcon, loadIcon(indxIcon, m_colorIconLtGray, wd));

    if (m_colorIconWhite.isValid() && !m_stdIconsWhiteHash.contains(indxIcon))
        m_stdIconsWhiteHash.insert(indxIcon, loadIcon(indxIcon, m_colorIconWhite, wd));

    if (m_colorIconBlack2.isValid() && !m_stdIconsBlack2Hash.contains(indxIcon))
        m_stdIconsBlack2Hash.insert(indxIcon, loadIcon(indxIcon, m_colorIconBlack2, wd));

    if (m_colorIconColor0.isValid() && !m_stdIconsColor0Hash.contains(indxIcon))
        m_stdIconsColor0Hash.insert(indxIcon, loadIcon(indxIcon, m_colorIconColor0, wd));

    if (m_colorIconColor1.isValid() && !m_stdIconsColor1Hash.contains(indxIcon))
        m_stdIconsColor1Hash.insert(indxIcon, loadIcon(indxIcon, m_colorIconColor1, wd));

    if (m_colorIconColor2.isValid() && !m_stdIconsColor2Hash.contains(indxIcon))
        m_stdIconsColor2Hash.insert(indxIcon, loadIcon(indxIcon, m_colorIconColor2, wd));

    if (m_colorIconColor3.isValid() && !m_stdIconsColor3Hash.contains(indxIcon))
        m_stdIconsColor3Hash.insert(indxIcon, loadIcon(indxIcon, m_colorIconColor3, wd));
}

/*! \internal */
void CommonStylePrivate::setPaletteFromGlobal(QPalette::ColorGroup gr, QPalette::ColorRole cr, QPalette& palette) const
{
    m_config.setPaletteFromGlobal(gr, cr, palette);
}

/*! \internal */
bool CommonStylePrivate::isUse96Dpi()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCoreApplication::instance()->testAttribute(Qt::AA_Use96Dpi);
#else
    return false;
#endif
}

/*! \internal */
QPixmap CommonStylePrivate::preparePixmap(const QIcon& icon, const QSize& iconSize, QIcon::Mode mode, QIcon::State state, const QWidget* wd)
{
    QPixmap pm;
    QSize pmSize = iconSize;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    pm = icon.pixmap(QtnPrivate::qtn_getWindow(wd), iconSize, mode, state);
#else
    Q_UNUSED(wd)
    pm = icon.pixmap(iconSize, mode, state);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qreal pixelRatio = wd ? wd->devicePixelRatioF() : pm.devicePixelRatioF();
    pmSize = QSize(qRound(pm.width() / pixelRatio), qRound(pm.height() / pixelRatio));//pm.size() / pm.devicePixelRatio();
#else
    pmSize = pm.size();
#endif
#if QTN_DEPRECATED_CODE
    if (pmSize.width() * pmSize.height() < iconSize.width() * iconSize.height())
        pm = pm.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
#endif
    return pm;
}

QPixmap CommonStylePrivate::loadIcon(const QString& name, const QWidget* wd)
{
    QString strIcon(QStringLiteral(":/res/Base/"));
    QString suffix = QtnPrivate::qtn_bestPngSize(wd);
    QString fileName = QString(strIcon + name + QStringLiteral("-") + suffix);
    return QPixmap(fileName);
}

QPixmap CommonStylePrivate::loadIcon(const QString& indxIcon, const QColor& color, const QWidget* wd) const
{
    QPixmap icon;
    QString fileName = m_config.standardPixmapName(indxIcon, wd);
    if (fileName.isEmpty())
        return icon;
    QString strIcon(QStringLiteral(":/res/Base/"));
    QPixmap px(strIcon + fileName);
    Q_ASSERT(!px.isNull());
    icon = QtnPrivate::qtn_ColorizedImage(px, Qt::black, color);
    return icon;
}

bool CommonStylePrivate::buttonVisible(const QStyle::SubControl sc, const QStyleOptionTitleBar* tb) 
{
    bool isMinimized = tb->titleBarState & Qt::WindowMinimized;
    bool isMaximized = tb->titleBarState & Qt::WindowMaximized;
    const uint flags = static_cast<uint>(tb->titleBarFlags);
    bool retVal = false;
    switch (sc) {
    case QStyle::SC_TitleBarContextHelpButton:
        if (flags & Qt::WindowContextHelpButtonHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarMinButton:
        if (!isMinimized && (flags & Qt::WindowMinimizeButtonHint))
            retVal = true;
        break;
    case QStyle::SC_TitleBarNormalButton:
        if (isMinimized && (flags & Qt::WindowMinimizeButtonHint))
            retVal = true;
        else if (isMaximized && (flags & Qt::WindowMaximizeButtonHint))
            retVal = true;
        break;
    case QStyle::SC_TitleBarMaxButton:
        if (!isMaximized && (flags & Qt::WindowMaximizeButtonHint))
            retVal = true;
        break;
    case QStyle::SC_TitleBarShadeButton:
        if (!isMinimized &&  flags & Qt::WindowShadeButtonHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarUnshadeButton:
        if (isMinimized && flags & Qt::WindowShadeButtonHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarCloseButton:
        if (flags & Qt::WindowSystemMenuHint)
            retVal = true;
        break;
    case QStyle::SC_TitleBarSysMenu:
        if (flags & Qt::WindowSystemMenuHint)
            retVal = true;
        break;
    default:
        retVal = true;
    }
    return retVal;
}

/*! \internal */
int CommonStylePrivate::dpiScaled(int value, const QWidget* widget)
{
    if (qtn_isDPIScalingActive())
        return value;
#ifdef Q_OS_MAC
    Q_UNUSED(widget)
    // On mac the DPI is always 72 so we should not scale it
    return value;
#else
    const qreal scale = static_cast<qreal>(qtn_getWindowDPI(widget)) / 96.0;
    return qRound(value * scale);
#endif
}

/*! \internal */
qreal CommonStylePrivate::dpiScaledF(qreal value, const QWidget* widget)
{
    if (qtn_isDPIScalingActive())
        return value;
#ifdef Q_OS_MAC
    Q_UNUSED(widget)
        // On mac the DPI is always 72 so we should not scale it
        return value;
#else
    const qreal scale = static_cast<qreal>(qtn_getWindowDPI(widget)) / 96.0;
    return value * scale;
#endif
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#ifdef Q_OS_MAC
static const qreal qtnstyleBaseDpi = 72;
#else
static const qreal qtnstyleBaseDpi = 96;
#endif

static qreal qtn_dpi(const QStyleOption* option)
{
#ifndef Q_OS_MAC
    // Prioritize the application override, except for on macOS where
    // we have historically not supported the AA_Use96Dpi flag.
    if (QCoreApplication::testAttribute(Qt::AA_Use96Dpi))
        return 96;
#endif
    // Expect that QStyleOption::QFontMetrics::QFont has the correct DPI set
    if (option)
        return option->fontMetrics.fontDpi();
    return qtnstyleBaseDpi;
}

static QPixmap qtn_pixmap(const QIcon& icon, const QRect& rect, const QSize& iconSize, QStyle::State state, const QWidget* widget)
{
    if (!icon.isNull())
    {
        QSize pmSize = iconSize;
        QIcon::State stateIcon = state & QStyle::State_On ? QIcon::On : QIcon::Off;
        QIcon::Mode mode;
        if (!(state & QStyle::State_Enabled))
            mode = QIcon::Disabled;
        else if ((state & QStyle::State_MouseOver) && (state & QStyle::State_AutoRaise))
            mode = QIcon::Active;
        else
            mode = QIcon::Normal;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QPixmap pm = widget ? icon.pixmap(QtnPrivate::qtn_getWindow(widget), rect.size().boundedTo(iconSize), mode, stateIcon) :
            icon.pixmap(rect.size().boundedTo(iconSize), mode, stateIcon);
#else
        Q_UNUSED(widget);
        QPixmap pm = icon.pixmap(rect.size().boundedTo(iconSize), mode, stateIcon);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        pmSize = pm.size() / pm.devicePixelRatio();
#else
        pmSize = pm.size();
#endif
        if (pmSize.width() * pmSize.height() < iconSize.width() * iconSize.height())
            pm = pm.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return pm;
    }
    return QPixmap();
}


/*! \internal */
qreal CommonStylePrivate::dpiScaled(qreal value, const QStyleOption* option)
{
    return value * qtn_dpi(option) / qtnstyleBaseDpi;
}
#endif // QT_VERSION

/*! \internal */
bool CommonStylePrivate::isQtitanStyle() const
{ return false; }

/*! \internal */
QPalette CommonStylePrivate::getWidgetPalette(QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (!widgetStyleSupport(widget))
    {
        if (p.isScrollBarsIgnored() && widget->inherits("QScrollBar"))
            return m_systemPalette;
        if (p.isDialogsIgnored() && widget->inherits("QDialog"))
            return m_systemPalette;
        return QPalette();
    }

    QPalette palette;
    if (widget->inherits(QTITAN_META_CLASSNAME("GridBase")))
        palette = m_config.getWidgetPalette(QStringLiteral("QHeaderView"));
    else
        palette = m_config.getWidgetPalette(widget);
    
    
    if (isQtitanStyle())
        return palette;

    if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
    {
        QPalette_setBrush(palette, QPalette::Active, QPalette::WindowText, palette.color(QPalette::Dark).darker(240));
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::WindowText, palette.color(QPalette::Dark).darker(240));
        QPalette_setBrush(palette, QPalette::Active, QPalette::ButtonText, palette.color(QPalette::Dark).darker(240));
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::ButtonText, palette.color(QPalette::Dark).darker(240));
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        QPalette_setBrush(palette, QPalette::All, QPalette::Button, p.CommonStyle::standardPalette().button());
#endif
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("RibbonTabBar")))
    {
        QPalette_setBrush(palette, QPalette::Active, QPalette::HighlightedText, palette.color(QPalette::ButtonText));
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu")))
    {
        QPalette_setBrush(palette, QPalette::All, QPalette::Window, darkBackgroundLight(palette));
        QPalette_setBrush(palette, QPalette::All, QPalette::Button, darkBackgroundLight(palette));
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("DockTitleBar")))
    {
        QPalette_setBrush(palette, QPalette::Active, QPalette::Window, palette.highlight().color().lighter());
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Window, p.standardPalette().window());
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("DockWidgetTitleButton")))
    {
        QPalette_setBrush(palette, QPalette::All, QPalette::HighlightedText, palette.buttonText().color());
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("DockWidgetMarkerWidget")))
    {
        QPalette_setBrush(palette, QPalette::Active, QPalette::Mid, palette.highlight());
        QPalette_setBrush(palette, QPalette::Active, QPalette::Highlight, palette.highlight());
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Mid, QColor(StyleHelper::colorMakeDarker(palette.window().color().rgb(), 0.5)));
        QPalette_setBrush(palette, QPalette::Inactive, QPalette::Highlight, QColor(StyleHelper::colorMakeDarker(palette.window().color().rgb(), 0.5)));
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("DockWidgetPhantomWidget")))
    {
        QPalette_setBrush(palette, QPalette::Active, QPalette::Highlight, QColor(StyleHelper::colorMakeLighter(palette.highlight().color().rgb(), 0.85)));
    }
    return palette;
}

QFont CommonStylePrivate::getWidgetFont(const QString& className) const
{
    if (className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBar")) || 
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonStatusBar")) ||
        className == QLatin1String(QTITAN_META_CLASSNAME("RibbonBackstageView")))
        return QApplication::font("QMenuBar");
    return QFont();
}

QFont CommonStylePrivate::getWidgetFont(QWidget* widget) const
{
    QStringList list;
    const QMetaObject* metaObject = widget->metaObject();
    while (metaObject)
    {
        if (metaObject->className() != QObject::staticMetaObject.className())
            list << QString(QLatin1String(metaObject->className()));// .remove(QLatin1String(QTITAN_META_CLASSNAME("")));
        metaObject = metaObject->superClass();
    }

    foreach(QString className, list)
    {
        QFont font = getWidgetFont(className);
        if (!font.isCopyOf(QFont()))
            return font;
    }
    return QFont();
}

/*! \internal */
static QStyle* qtn_getStyle(QStyle* currentStyle)
{
    if (currentStyle->inherits("QStyleSheetStyle"))
    {
        if (CommonStyle* commonStyle = currentStyle->findChild<CommonStyle*>())
            currentStyle = commonStyle;
    }
    return currentStyle;
}

/*! \internal */
void CommonStylePrivate::beginUpdate(bool polishNeeded)
{
    QTN_P(CommonStyle);
    if (!polishNeeded)
        return;

    QWidgetList all = qApp->allWidgets();
    // clean up the old style
    for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it)
    {
        QWidget* w = *it;
        if (!(w->windowType() == Qt::Desktop) && w->testAttribute(Qt::WA_WState_Polished) && qtn_getStyle(w->style()) == &p)
            p.unpolish(w);
    }
}

/*! \internal */
void CommonStylePrivate::endUpdate(bool polishNeeded)
{
    QTN_P(CommonStyle);
/* TODO
    QStyle* style = qtn_getStyle(QApplication::style());
    if (style == &p)
    {
        m_standardPalette = p.QProxyStyle::standardPalette();
        QApplication::setPalette(QPalette());

        m_systemPalette = QApplication::palette(); //Palette from the base style.
        int resolve_mask = 0;
        for (int role = 0; role < (int)QPalette::NColorRoles; role++)
            resolve_mask |= (1 << role);

        QPalette_setResolveMask(m_systemPalette, resolve_mask);

        initStandardPalette(m_standardPalette);
        QApplication::setPalette(m_standardPalette);
    }
*/

    if (polishNeeded)
    {
        if (qtn_getStyle(QApplication::style()) == &p)
            QApplication::setPalette(QApplication::palette());
    }

    if (polishNeeded)
    {
        QWidgetList all = qApp->allWidgets();
        // re-polish existing widgets if necessary
        for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it)
        {
            QWidget* w = *it;
            if (w->windowType() != Qt::Desktop && w->testAttribute(Qt::WA_WState_Polished) && qtn_getStyle(w->style()) == &p)
                p.polish(w);
        }
        
        for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it)
        {
            QWidget* w = *it;
            if (w->windowType() != Qt::Desktop && !w->testAttribute(Qt::WA_SetStyle) && qtn_getStyle(w->style()) == &p)
            {
                QEvent e(QEvent::StyleChange);
                QApplication::sendEvent(w, &e);
            }
        }
    }
}

/*! \internal */
bool CommonStylePrivate::drawHighDpiDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    // DockWidgetMarkerWidget
    if (const DockMarkerStyleOption* marker = qstyleoption_cast<const DockMarkerStyleOption*>(opt))
    {
        bool ok = false;
        CommonStyle::StandardPixmapEx sp = static_cast<CommonStyle::StandardPixmapEx>(marker->sp);
        if (sp == CommonStyle::SP_DockMarkerBasis || sp == CommonStyle::SP_DockMarkerBasisEx)
        {
            drawHighDpiBasisDockMarker(opt, painter, widget);
            ok = true;
        }
        else if (sp == CommonStyle::SP_DockMarkerLeft || sp == CommonStyle::SP_DockMarkerRight || 
                 sp == CommonStyle::SP_DockMarkerTop  || sp == CommonStyle::SP_DockMarkerBottom)
        {
            drawHighDpiBasisCenterDockMarker(opt, painter, widget);
            drawHighDpiAreaDockMarker(opt, painter, widget);
            drawHighDpiWindowDockMarker(opt, painter, widget);
            drawHighDpiArrowDockMarker(opt, painter, widget);
            ok = true;
        }
        else if (sp == CommonStyle::SP_DockMarkerBasisLeft  || sp == CommonStyle::SP_DockMarkerBasisRight   ||
                 sp == CommonStyle::SP_DockMarkerBasisTop   || sp == CommonStyle::SP_DockMarkerBasisBottom  || 
                 sp == CommonStyle::SP_DockMarkerCenterLeft || sp == CommonStyle::SP_DockMarkerCenterRight  || 
                 sp == CommonStyle::SP_DockMarkerCenterTop  || sp == CommonStyle::SP_DockMarkerCenterBottom ||
                 sp == CommonStyle::SP_DockMarkerCenterSplitLeft || sp == CommonStyle::SP_DockMarkerCenterSplitRight  ||
                 sp == CommonStyle::SP_DockMarkerCenterSplitTop  || sp == CommonStyle::SP_DockMarkerCenterSplitBottom ||
                 sp == CommonStyle::SP_DockMarkerCenter)
        {
            qreal value40 = dpiScaledF(40.0, widget);
            DockMarkerStyleOption option = *marker;
            option.rect.setSize(QSize(qRound(value40), qRound(value40)));
            option.rect.moveTopLeft(marker->position.toPoint());
            drawHighDpiWindowDockMarker(&option, painter, widget);
            drawHighDpiArrowDockMarker(&option, painter, widget);
            ok = true;
        }
        return ok;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawHighDpiBasisDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const DockMarkerStyleOption* marker = qstyleoption_cast<const DockMarkerStyleOption*>(opt))
    {
        QColor shadowColor = marker->palette.window().color();
        QColor borderColor = marker->palette.dark().color();
        QRectF markRect(marker->rect);
        QSize size = marker->rect.adjusted(0, 0, -1, -1).size();
        const qreal sz = static_cast<qreal>(qMax(size.width(), size.height())) / 2.0;
        const qreal width = dpiScaled(20.0, widget);
        const qreal height = dpiScaled(30.0, widget);

        QPainterPath path;
        path.moveTo(width, -sz);  path.lineTo(width, -height);  path.lineTo(height, -width);  path.lineTo(sz, -width);
        path.lineTo(sz, width);   path.lineTo(height, width);   path.lineTo(width, height);   path.lineTo(width, sz);
        path.lineTo(-width, sz);  path.lineTo(-width, height);  path.lineTo(-height, width);  path.lineTo(-sz, width);
        path.lineTo(-sz, -width); path.lineTo(-height, -width); path.lineTo(-width, -height); path.lineTo(-width, -sz);
        path.closeSubpath();

        path.translate(markRect.center());
        shadowColor.setAlphaF(0.6f);
        painter->fillPath(path, shadowColor);
        painter->setPen(borderColor);
        painter->drawPath(path);
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawHighDpiBasisCenterDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* /*widget*/) const
{
    // DockWidgetMarkerWidget
    if (const DockMarkerStyleOption* marker = qstyleoption_cast<const DockMarkerStyleOption*>(opt))
    {
        painter->save();
        QColor shadowColor = marker->palette.window().color();
        QRectF markRect(marker->rect);
        QColor borderColor = marker->palette.dark().color();

        QPainterPath path;
        path.moveTo(markRect.left(), markRect.top()); path.lineTo(markRect.right(), markRect.top()); 
        path.lineTo(markRect.right(), markRect.bottom()); path.lineTo(markRect.left(), markRect.bottom());
        path.closeSubpath();

        shadowColor.setAlphaF(0.6f);
        painter->fillPath(path, shadowColor);
        painter->setPen(borderColor);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawPath(path);
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawHighDpiAreaDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(opt)
    Q_UNUSED(painter)
    Q_UNUSED(widget)
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawHighDpiWindowDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    // DockWidgetMarkerWidget
    if (const DockMarkerStyleOption* marker = qstyleoption_cast<const DockMarkerStyleOption*>(opt))
    {
        painter->save();
        const CommonStyle::StandardPixmapEx sp = static_cast<CommonStyle::StandardPixmapEx>(marker->sp);
        QColor wndColor = opt->palette.highlight().color();
        QRectF markRect(opt->rect);
        QPointF pntOffset = qtn_DPIScaledF(markRect.topLeft(), widget);
        qreal radius = 0.0;// 0.5 * scale;
        painter->setPen(wndColor);

        drawHighDpiAreaDockMarker(opt, painter, widget);
        drawHighDpiWindowClientDockMarker(opt, painter, widget);

        qreal value8 = CommonStylePrivate::dpiScaledF(8.0, widget);
        qreal value11 = CommonStylePrivate::dpiScaledF(11.0, widget);
        qreal value19 = CommonStylePrivate::dpiScaledF(19.0, widget);
        qreal value19_5 = CommonStylePrivate::dpiScaledF(19.5, widget);
        qreal value20 = CommonStylePrivate::dpiScaledF(20.0, widget);
        qreal value21 = CommonStylePrivate::dpiScaledF(21.0, widget);
        qreal value23 = CommonStylePrivate::dpiScaledF(23.0, widget);
        qreal value31 = CommonStylePrivate::dpiScaledF(31.0, widget);

        if (sp == CommonStyle::SP_DockMarkerLeft || sp == CommonStyle::SP_DockMarkerBasisLeft || sp == CommonStyle::SP_DockMarkerCenterLeft)
        {
            QPainterPath pathWnd;
            pathWnd.addRoundedRect(QRectF(QPointF(value8, value8), QPointF(value19, value31)), radius, radius, Qt::AbsoluteSize);
            pathWnd.translate(pntOffset);
            painter->drawPath(pathWnd);

            QPainterPath pathTitle;
            pathTitle.addRect(QRectF(QPointF(value8, value8), QPointF(value19, value11)));
            pathTitle.translate(pntOffset);
            painter->fillPath(pathTitle, wndColor);
        }
        else if (sp == CommonStyle::SP_DockMarkerRight || sp == CommonStyle::SP_DockMarkerBasisRight || sp == CommonStyle::SP_DockMarkerCenterRight)
        {
            QPainterPath pathWnd;
            pathWnd.addRoundedRect(QRectF(QPointF(value20, value8), QPointF(value31, value31)), radius, radius, Qt::AbsoluteSize);
            pathWnd.translate(pntOffset);
            painter->drawPath(pathWnd);

            QPainterPath pathTitle;
            pathTitle.addRect(QRectF(QPointF(value20, value8), QPointF(value31, value11)));
            pathTitle.translate(pntOffset);
            painter->fillPath(pathTitle, wndColor);
        }
        else if (sp == CommonStyle::SP_DockMarkerTop || sp == CommonStyle::SP_DockMarkerBasisTop || sp == CommonStyle::SP_DockMarkerCenterTop)
        {
            QPainterPath pathWnd;
            pathWnd.addRoundedRect(QRectF(QPointF(value8, value8), QPointF(value31, value19)), radius, radius, Qt::AbsoluteSize);
            pathWnd.translate(pntOffset);
            painter->drawPath(pathWnd);
             
            QPainterPath pathTitle;
            pathTitle.addRect(QRectF(QPointF(value8, value8), QPointF(value31, value11)));
            pathTitle.translate(pntOffset);
            painter->fillPath(pathTitle, wndColor);
        }
        else if (sp == CommonStyle::SP_DockMarkerBottom || sp == CommonStyle::SP_DockMarkerBasisBottom || sp == CommonStyle::SP_DockMarkerCenterBottom)
        {
            QPainterPath pathWnd;
            pathWnd.addRoundedRect(QRectF(QPointF(value8, value20), QPointF(value31, value31)), radius, radius, Qt::AbsoluteSize);
            pathWnd.translate(pntOffset);
            painter->drawPath(pathWnd);

            QPainterPath pathTitle;
            pathTitle.addRect(QRectF(QPointF(value8, value20), QPointF(value31, value23)));
            pathTitle.translate(pntOffset);
            painter->fillPath(pathTitle, wndColor);
        }
        else if (sp == CommonStyle::SP_DockMarkerCenter || sp == CommonStyle::SP_DockMarkerCenterSplitLeft || sp == CommonStyle::SP_DockMarkerCenterSplitRight ||
                 sp == CommonStyle::SP_DockMarkerCenterSplitTop || sp == CommonStyle::SP_DockMarkerCenterSplitBottom)
        {
            QPainterPath pathWnd;
            pathWnd.addRoundedRect(QRectF(QPointF(value8, value8), QPointF(value31, value31)), radius, radius, Qt::AbsoluteSize);
            pathWnd.translate(pntOffset);
            painter->drawPath(pathWnd);

            QPainterPath pathTitle;
            pathTitle.addRect(QRectF(QPointF(value8, value8), QPointF(value31, value11)));
            pathTitle.translate(pntOffset);
            painter->fillPath(pathTitle, wndColor);
            if (sp == CommonStyle::SP_DockMarkerCenterSplitLeft || sp == CommonStyle::SP_DockMarkerCenterSplitRight)
            {
                QPainterPath pathSplit;
                pathSplit.moveTo(value19_5, value11);
                pathSplit.lineTo(value19_5, value31);
                pathSplit.translate(pntOffset);
                painter->setPen(QPen(wndColor, 1, Qt::DotLine));
                painter->drawPath(pathSplit);
            }
            else if (sp == CommonStyle::SP_DockMarkerCenterSplitTop || sp == CommonStyle::SP_DockMarkerCenterSplitBottom)
            {
                QPainterPath pathSplit;
                pathSplit.moveTo(value8, value21);
                pathSplit.lineTo(value31, value21);
                pathSplit.translate(pntOffset);
                painter->setPen(QPen(wndColor, 1, Qt::DotLine));
                painter->drawPath(pathSplit);
            }
        }
        painter->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawHighDpiWindowClientDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(opt)
    Q_UNUSED(painter)
    Q_UNUSED(widget)
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawHighDpiArrowDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const DockMarkerStyleOption* marker = qstyleoption_cast<const DockMarkerStyleOption*>(opt))
    {
        painter->save();
        const CommonStyle::StandardPixmapEx sp = static_cast<CommonStyle::StandardPixmapEx>(marker->sp);
        const bool vertical = sp == CommonStyle::SP_DockMarkerLeft || sp == CommonStyle::SP_DockMarkerRight || 
            sp == CommonStyle::SP_DockMarkerBasisLeft || sp == CommonStyle::SP_DockMarkerBasisRight ||
            sp == CommonStyle::SP_DockMarkerCenterLeft || sp == CommonStyle::SP_DockMarkerCenterRight;

        QColor wndColor = opt->palette.mid().color();
        QRectF markRect(opt->rect);
        QPointF pntOffset = qtn_DPIScaledF(markRect.topLeft(), widget);
        QRectF arrowRect;
        qreal value4 = CommonStylePrivate::dpiScaled(4.0, widget);
        qreal value5 = CommonStylePrivate::dpiScaled(5.0, widget);
        qreal value8 = CommonStylePrivate::dpiScaled(8.0, widget);
        if (vertical)
        {
            qreal arrowWidth = value4;
            qreal arrowHeight = value8;
            arrowRect.setWidth(arrowWidth);
            arrowRect.setHeight(arrowHeight * value5 / arrowWidth);
        }
        else
        {
            qreal arrowWidth = value8;
            qreal arrowHeight = value4;
            arrowRect.setWidth(arrowWidth * value5 / arrowHeight);
            arrowRect.setHeight(arrowHeight);
        }

        QPainterPath pathArrow;
        QPolygonF triangle;
        triangle.reserve(3);

        qreal value10 = CommonStylePrivate::dpiScaled(10, widget);
        qreal value26 = CommonStylePrivate::dpiScaled(26, widget);

        if (sp == CommonStyle::SP_DockMarkerLeft || sp == CommonStyle::SP_DockMarkerBasisLeft || sp == CommonStyle::SP_DockMarkerCenterLeft)
        {
            arrowRect.moveTo(value26, (markRect.height() - arrowRect.height()) / 2.0);
            triangle << arrowRect.topRight() << arrowRect.bottomRight() << QPointF(arrowRect.left(), arrowRect.center().y());
        }
        else if (sp == CommonStyle::SP_DockMarkerRight || sp == CommonStyle::SP_DockMarkerBasisRight || sp == CommonStyle::SP_DockMarkerCenterRight)
        {
            arrowRect.moveTo(value10, (markRect.height() - arrowRect.height()) / 2.0);
            triangle << arrowRect.topLeft() << arrowRect.bottomLeft() << QPointF(arrowRect.right(), arrowRect.center().y());
        }
        else if (sp == CommonStyle::SP_DockMarkerTop || sp == CommonStyle::SP_DockMarkerBasisTop || sp == CommonStyle::SP_DockMarkerCenterTop)
        {
            arrowRect.moveTo((markRect.width() - arrowRect.width()) / 2.0, value26);
            triangle << arrowRect.bottomLeft() << arrowRect.bottomRight() << QPointF(arrowRect.center().x(), arrowRect.top());
        }
        else if (sp == CommonStyle::SP_DockMarkerBottom || sp == CommonStyle::SP_DockMarkerBasisBottom || sp == CommonStyle::SP_DockMarkerCenterBottom)
        {
            arrowRect.moveTo((markRect.width() - arrowRect.width()) / 2.0, value10);
            triangle << arrowRect.topLeft() << arrowRect.topRight() << QPointF(arrowRect.center().x(), arrowRect.bottom());
        }

        pathArrow.addPolygon(triangle);
        pathArrow.closeSubpath();
        pathArrow.translate(pntOffset);
        painter->setPen(Qt::NoPen);
        painter->setBrush(wndColor);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawPath(pathArrow);
        painter->restore();
        return true;
    }
    return false;
}


// for standard controls
/*! \internal */
bool CommonStylePrivate::drawFrame(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawFrameWindow(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const QStyleOptionFrame* frm = qstyleoption_cast<const QStyleOptionFrame *>(option))
    {
        if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstagePage")))
        {
            painter->fillRect(option->rect, option->palette.color(QPalette::Base));
            return true;
        }
        else if (qtn_hasWindowTitleBar(widget))
        {
            if (const FrameStyleOption* frmOpt = qstyleoption_cast<const FrameStyleOption*>(option))
            {
                const bool isActive = frm->state & QStyle::State_Active;
                const int fwidth = frm->lineWidth + frm->midLineWidth;
                QColor frameColor = titleBarFrameColor(option->palette, isActive, widget);
                if (frmOpt->section == Qt::NoSection)
                {
                    painter->fillRect(QRect(option->rect.x(), option->rect.y(), option->rect.width(), fwidth), frameColor); // top
                    painter->fillRect(QRect(option->rect.x(), option->rect.y(), fwidth, option->rect.height()), frameColor); // left
                    painter->fillRect(QRect(option->rect.width() - fwidth, option->rect.y(), option->rect.width(), option->rect.height()), frameColor); // right
                    painter->fillRect(QRect(option->rect.x(), option->rect.height() - fwidth, option->rect.width(), option->rect.height()), frameColor); // bottom
                }
                else if (frmOpt->section == Qt::TitleBarArea)
                {
                    QRect rect = QRect(option->rect.x(), option->rect.y(), option->rect.x(), option->rect.height());
                    painter->fillRect(rect, frameColor);
                }
                else if (frmOpt->section == Qt::TopSection)
                {
                    QRect rect = QRect(option->rect.x(), option->rect.y(), option->rect.width(), fwidth);
                    painter->fillRect(rect, frameColor);
                }
                else if (frmOpt->section == Qt::LeftSection)
                {
                    QRect rect = QRect(option->rect.x(), option->rect.y(), fwidth, option->rect.height());
                    painter->fillRect(rect, frameColor);
                }
                else if (frmOpt->section == Qt::BottomSection)
                {
                    QRect rect = QRect(option->rect.x(), option->rect.height() - fwidth, option->rect.width(), fwidth);
                    painter->fillRect(rect, frameColor);
                }
                else if (frmOpt->section == Qt::RightSection)
                {
                    QRect rect = QRect(option->rect.x(), option->rect.y(), fwidth, option->rect.height());
                    painter->fillRect(rect, frameColor);
                }
            }
            return true;
        }
        else if (widget != Q_NULL && widget->inherits("QMdiSubWindow"))
        {
            if (isQtitanStyle())
            {
                if (const QStyleOptionFrame* frame = qstyleoption_cast<const QStyleOptionFrame *>(option))
                {
                    QColor clrFrameWindow = frame->state & QStyle::State_Active ? 
                        colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiFrameWindow"), QPalette::Active, QPalette::Dark) :
                        colorSubControl(QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiFrameWindow"), QPalette::Inactive, QPalette::Dark);
                    if (clrFrameWindow.isValid())
                    {
                        QtnPen pen(painter, clrFrameWindow, frame->lineWidth);
                        qtn_drawRect(painter, frame->rect);
                        return true;
                    }
                }
            }
            else
            {
                p.QProxyStyle::drawPrimitive(QStyle::PE_FrameWindow, option, painter, widget);
                return true;
            }
        }
    }
    return false;
}

bool CommonStylePrivate::drawPanelMenu(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* widget) const
{ 
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("PopupMenuView")) && 
        QtnPrivate::qtn_isAncestor(widget, QTITAN_META_CLASSNAME("RibbonSystemMenu")))
        return true;
    return false; 
}

bool CommonStylePrivate::drawIndicatorArrow(QStyle::PrimitiveElement /*pe*/, const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

bool CommonStylePrivate::drawFrameGroupBox(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

bool CommonStylePrivate::drawIndicatorCheckBox(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

bool CommonStylePrivate::drawIndicatorItemCheck(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawIndicatorRadioButton(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawIndicatorMenuCheckMark(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawIndicatorButtonDropDown(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
static void drawSymbolButton(QPainter* painter, SegoeMDL2Font::IconSymbol symbol, const QRect& rect, const QColor& color)
{
    SegoeMDL2Font font;
    font.paint(painter, SegoeMDL2Font::code(symbol), rect, color);
}

/*! \internal */
bool CommonStylePrivate::drawIndicatorTabClose(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{ 
    bool qtitanIcon = isQtitanStyle() && (widget && widget->inherits("CloseButton"));
    if (widget && (qtitanIcon || widget->inherits(QTITAN_META_CLASSNAME("AbstractTab"))))
    {
        QRect rect = opt->rect;
        bool highlight = opt->state & QStyle::State_Raised;
        QColor color = highlight ? Qt::red : opt->palette.color(QPalette::WindowText);

        if (qtitanIcon)
        {
            int adjust = qtn_DPIScaled(3, widget);
            rect.adjust(adjust, adjust, -adjust, -adjust);
        }
        
        drawSymbolButton(painter, SegoeMDL2Font::ChromeClose, rect, color);
        return true;
    }
    return false; 
}

/*! \internal */
bool CommonStylePrivate::drawIndicatorToolBarHandle(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawIndicatorToolBarSeparator(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawIndicatorHeaderArrow(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawIndicatorBranch(const QStyleOption* option, QPainter* painter, const QWidget* /*w*/) const
{ 
    Q_UNUSED(option);
    Q_UNUSED(painter);
#if 0
    int mid_h = option->rect.x() + option->rect.width() / 2;
    int mid_v = option->rect.y() + option->rect.height() / 2;
    int bef_h = mid_h;
    int bef_v = mid_v;
    int aft_h = mid_h;
    int aft_v = mid_v;

    //const QSizeF size = themeSize.size() * QWindowsStylePrivate::nativeMetricScaleFactor(widget);
    //decoration_size = qRound(qMax(size.width(), size.height()));
    //int delta = decoration_size / 2;
    //bef_h -= delta + 2;
    //bef_v -= delta + 2;
    //aft_h += delta - 2;
    //aft_v += delta - 2;

    QBrush brush(option->palette.dark().color(), Qt::Dense4Pattern);
    
    if (option->state & QStyle::State_Item) {
        if (option->direction == Qt::RightToLeft)
            painter->fillRect(option->rect.left(), mid_v, bef_h - option->rect.left(), 1, brush);
        else
            painter->fillRect(aft_h, mid_v, option->rect.right() - aft_h + 1, 1, brush);
    }

    if (option->state & QStyle::State_Sibling
        //&& option->rect.bottom() > aft_v
        )
    {
        painter->fillRect(mid_h, aft_v, 1, option->rect.bottom() - aft_v + 1, brush);
    }

    if (option->state & (QStyle::State_Open | QStyle::QStyle::State_Children | QStyle::State_Item | QStyle::State_Sibling)
        //&& (bef_v > option->rect.y())
        )
    {
        painter->fillRect(mid_h, option->rect.y(), 1, bef_v - option->rect.y(), brush);
    }
#endif

    return false; 
}

/*! \internal */
bool CommonStylePrivate::drawDockMarker(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    drawHighDpiDockMarker(opt, painter, widget);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawDockWidgetPhantom(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const DockPhantomStyleOption* phantom = qstyleoption_cast<const DockPhantomStyleOption*>(opt))
    {
        if (phantom->drawFrame)
        {
            QRect rc = phantom->rect;
            if (!rc.isEmpty())
            {
                rc.adjust(-1, -1, 0, 0);
                QBrush brush = painter->brush();
                QBrush brushTmp(Qt::gray, Qt::Dense3Pattern);
                painter->setBrush(brushTmp);
                painter->drawRect(rc);
                painter->setBrush(brush);
            }
            return true;
        }
        QRect rectFill = phantom->rect;
        QPalette _palette = phantom->palette;
        _palette.setCurrentColorGroup(QPalette::Active);

        if (phantom->posTab == Qtitan::TopArea)
            rectFill.adjust(phantom->borderWidth, phantom->rectTab.height() + phantom->borderWidth, -phantom->borderWidth, -phantom->borderWidth);
        else if (phantom->posTab == Qtitan::BottomArea)
            rectFill.adjust(phantom->borderWidth, phantom->borderWidth, -phantom->borderWidth, -phantom->rectTab.height());
        else
            rectFill.adjust(phantom->borderWidth, phantom->borderWidth, -phantom->borderWidth, -phantom->borderWidth);

        if (phantom->rectTab.isValid())
        {
            QRect rectPiece = rectFill;
            if (phantom->posTab == Qtitan::TopArea)
                rectPiece = QRect(QPoint(rectPiece.left(), phantom->rect.top() + phantom->borderWidth), 
                    QSize(phantom->rectTab.width(), phantom->rectTab.height() + phantom->borderWidth));
            else if (phantom->posTab == Qtitan::BottomArea)
                rectPiece = QRect(QPoint(rectPiece.left(), (phantom->rect.bottom() - phantom->rectTab.height()) - (phantom->borderWidth - 1)), 
                    QSize(phantom->rectTab.width(), phantom->rectTab.height()));
            painter->fillRect(rectPiece, _palette.color(QPalette::Highlight));
        }
        painter->fillRect(rectFill, _palette.color(QPalette::Highlight));

        QBrush brush(Qt::gray);
        QPen pen(brush, phantom->borderWidth);
        painter->setPen(pen);
        int mg = qRound(phantom->borderWidth / 2.0);
        QRect rectBorder = phantom->rect.adjusted(mg, mg, -mg, -mg);

        // draw page
        if (phantom->rectTab.isValid() && phantom->posTab == Qtitan::TopArea)
        {
            QPolygon polygon(7);
            polygon.setPoint(0, rectBorder.left(), rectBorder.top());
            polygon.setPoint(1, rectBorder.left() + phantom->rectTab.width() + phantom->borderWidth, rectBorder.top());
            polygon.setPoint(2, rectBorder.left() + phantom->rectTab.width() + phantom->borderWidth, rectBorder.top() + phantom->rectTab.height());
            polygon.setPoint(3, rectBorder.right(), rectBorder.top() + phantom->rectTab.height());
            polygon.setPoint(4, rectBorder.right(), rectBorder.bottom());
            polygon.setPoint(5, rectBorder.left(), rectBorder.bottom());
            polygon.setPoint(6, rectBorder.left(), rectBorder.top());
            painter->drawPolyline(polygon);
        }
        else if (phantom->rectTab.isValid() && phantom->posTab == Qtitan::BottomArea)
        {
            QPolygon polygon(7);
            polygon.setPoint(0, rectBorder.left(), rectBorder.top());
            polygon.setPoint(1, rectBorder.left(), rectBorder.bottom());
            polygon.setPoint(2, rectBorder.left() + phantom->rectTab.width(), rectBorder.bottom());
            polygon.setPoint(3, rectBorder.left() + phantom->rectTab.width(), rectBorder.bottom() - phantom->rectTab.height() + phantom->borderWidth);

            polygon.setPoint(4, rectBorder.right(), rectBorder.bottom() - phantom->rectTab.height() + phantom->borderWidth);
            polygon.setPoint(5, rectBorder.right(), rectBorder.top());
            polygon.setPoint(6, rectBorder.left(), rectBorder.top());
            painter->drawPolyline(polygon);
        }
        else
        {
            QPolygon polygon(5);
            polygon.setPoint(0, rectBorder.topLeft());
            polygon.setPoint(1, rectBorder.bottomLeft());
            polygon.setPoint(2, rectBorder.bottomRight());
            polygon.setPoint(3, rectBorder.topRight());
            polygon.setPoint(4, rectBorder.topLeft());
            painter->drawPolyline(polygon);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawFrameFocusRect(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(painter);
    Q_UNUSED(w);
#if 0
    QTN_P(const CommonStyle);
    if (const QStyleOptionFocusRect *fropt = qstyleoption_cast<const QStyleOptionFocusRect *>(opt))
    {
        //### check for d->alt_down
        if (!(fropt->state & QStyle::State_KeyboardFocusChange) && !p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt))
            return true;
        QRect r = opt->rect;
        painter->save();
        painter->setBackgroundMode(Qt::TransparentMode);
        QColor bg_col = fropt->backgroundColor;
        if (!bg_col.isValid())
            bg_col = painter->background().color();
        painter->setBrush(QBrush(bg_col, Qt::Dense4Pattern));
        painter->setBrushOrigin(r.topLeft());
        painter->setPen(Qt::NoPen);
        painter->drawRect(r.left(), r.top(), r.width(), 1);    // Top
        painter->drawRect(r.left(), r.bottom(), r.width(), 1); // Bottom
        painter->drawRect(r.left(), r.top(), 1, r.height());   // Left
        painter->drawRect(r.right(), r.top(), 1, r.height());  // Right
        painter->restore();
    }
    return true;
#endif
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawFrameDockWidget(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (widget && (widget->inherits(QTITAN_META_CLASSNAME("DockTabbedLayoutPanel")) || widget->inherits(QTITAN_META_CLASSNAME("DockBarSite"))))
    {
        if (const QStyleOptionFrame* frame = qstyleoption_cast<const QStyleOptionFrame *>(option))
        {
            painter->fillRect(frame->rect, option->palette.window());
            return true;
        }
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawFrameMenu(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* widget) const
{
    if (widget && QtnPrivate::qtn_isAncestor(widget, QTITAN_META_CLASSNAME("RibbonSystemMenu")))
        return true;
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawFrameTabBarBase(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawPanelItemViewItem(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*widget*/) const
{ return false; }

/*! \internal */
bool CommonStylePrivate::drawPanelTipLabel(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(opt)
    Q_UNUSED(p)
    Q_UNUSED(widget)
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawPanelButtonTool(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonGroup")))
    {
        if (opt->state & QStyle::State_MouseOver)
        {
            painter->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.window());
            painter->setPen(buttonHighlightColor(opt->palette, true/*highlightDn*/, true/*border*/, widget));
            qtn_drawRect(painter, opt->rect);
            return true;
        }
        return false;
    }
    else if (!qtn_hasWindowTitleBar(widget))
        return false;

    QBrush fill;
    const bool isHighlight = opt->state & QStyle::State_MouseOver;
    fill = opt->palette.color(QPalette::Active, QPalette::Button);
    if (isHighlight)
        fill = opt->palette.color(QPalette::Active, QPalette::Highlight);
    painter->fillRect(opt->rect, fill);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawGroupControlEntry(const QStyleOption*, QPainter*, const QWidget*) const 
{ 
    return false; 
}

/*! \internal */
bool CommonStylePrivate::drawPanelButtonCommand(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget && widget->inherits("QMdiArea"))
    {
        if (!QtnPrivate::qtn_findChild(widget->parentWidget(), QTITAN_META_CLASSNAME("RibbonBar")))
            return false;

        if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton *>(opt))
        {
            const bool enabled = btn->state & QStyle::State_Enabled;
            const bool checked = btn->state & QStyle::State_On;
            const bool highlighted = btn->state & QStyle::State_MouseOver && enabled;
            const bool pressed = btn->state & QStyle::State_Sunken;

            QColor colorFill;
            QColor colorBorder;
            if (!enabled) colorFill = QColor();

            if (checked && !highlighted && !pressed)
            {
                colorFill = buttonHighlightColor(btn->palette, true, false, widget); // highlightDn
                colorBorder = buttonHighlightColor(btn->palette, true, true, widget); // highlightDn
            }
            else if (pressed)
            {
                colorFill = buttonHighlightColor(btn->palette, true, false, widget); // highlightDn
                colorBorder = buttonHighlightColor(btn->palette, true, true, widget); // highlightDn
            }
            else if (highlighted)
            {
                colorFill = buttonHighlightColor(btn->palette, false, false, widget); // highlight
                colorBorder = buttonHighlightColor(btn->palette, false, true, widget); // highlightDn
            }

            if (colorFill.isValid())
            {
                QRect rc = btn->rect;
                QtnBrush brush(painter, colorFill);
                QtnPen pen(painter, colorBorder, 1);
                qtn_drawRect(painter, rc);
            }
            return true;
        }
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawFrameLineEdit(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawPanelLineEdit(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawFrameStatusBarItem(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* widget) const
{
    if (widget && QtnPrivate::qtn_isAncestor(widget, "QStatusBar"))
        return true;
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawPanelStatusBar(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*widget*/) const
{
    return false;
}

/*! \internal */
void CommonStylePrivate::drawfillTitleBar(const QStyleOption* option, QPainter* painter, const QWidget* widget, const QColor& frameColor, const QColor& windowColor) const
{
    QColor titleBarColor = windowColor;
    QTN_P(const CommonStyle);
    if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
    {
        if (option->palette.window() == Qt::transparent)
            titleBarColor = Qt::transparent; // Special case then we paint title bar in extendViewIntoTitleBar mode.

        QRect fullRect = tb->rect;
#ifdef QTN_MAC_TITLEBAR
        if (isQtitanStyle())
        {
            painter->fillRect(fullRect, titleBarColor);
        }
        else if (titleBarColor != Qt::transparent)
        {
            QLinearGradient gradient;
            if (tb->state & QStyle::State_Active)
                gradient = QtnPrivate::qtn_titleBarGradientActive();
            else
                gradient = QtnPrivate::qtn_titleBarGradientInactive();
            gradient.setStart(0, fullRect.top());
            gradient.setFinalStop(0, fullRect.bottom());
            painter->fillRect(fullRect, gradient);
    }
#else
        painter->fillRect(fullRect, titleBarColor);
#endif
        if (frameColor.isValid())
        {
            const int fwidth = p.proxy()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, Q_NULL, widget);
            if (fwidth > 0)
            {
                //Mac OS has a bug if we tried to paint the fame with the 0 pixel width.
                painter->setPen(qtn_createPen(frameColor, fwidth));
                QRect r = qtn_adjustRect(painter->pen(), fullRect);
                QPainterPath path;
                path.moveTo(r.x(), r.y() + r.height());
                path.lineTo(r.x(), r.y());
                path.lineTo(r.x() + r.width(), r.y());
                path.lineTo(r.x() + r.width(), r.y() + r.height());
                painter->drawPath(path);
            }
        }
    }
}

/*! \internal */
bool CommonStylePrivate::drawTitleBar(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (widget && widget->inherits("QMdiSubWindow"))
    {
        p.QProxyStyle::drawComplexControl(QStyle::CC_TitleBar, option, painter, widget);
        return true;
    }
    else if (!qtn_hasWindowTitleBar(widget))
        return false;

    if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
    {
        QRect ir;
        const bool isActive = tb->state & QStyle::State_Active;
        if (option->subControls & QStyle::SC_TitleBarLabel)
        {
            QColor frameColor = titleBarFrameColor(option->palette, isActive, widget);
            QColor windowColor = titleBarWindowColor(option->palette, isActive, widget);
            drawfillTitleBar(option, painter, widget, frameColor, windowColor);

            const int frameWidth = p.proxy()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, option, widget);
            if (tb->rect.height() == frameWidth)
                return true;

            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarLabel, widget);
            painter->setPen(isActive ? option->palette.text().color() : option->palette.color(QPalette::Disabled, QPalette::Text));
            painter->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, tb->text);
        }
        bool down = false;
        bool highlight = false;
        QPixmap pm;
        QPalette savePalette = tb->palette;
        QStyleOptionTitleBar tool = *tb;

        if (tb->subControls & QStyle::SC_TitleBarCloseButton && tb->titleBarFlags & Qt::WindowSystemMenuHint)
        {
            const bool closeHint = tb->titleBarFlags & Qt::WindowCloseButtonHint;
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarCloseButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarCloseButton) && (option->state & QStyle::State_Sunken);
            highlight = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            if (closeHint)
            {
                QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, Qt::white);
                QPalette_setColor(tool.palette, QPalette::Active, QPalette::Button, m_clrCloseButton);
                QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, m_clrCloseButtonHighlight);
                QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, Qt::white);
                p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);
            }
            else
            {
                QPalette_setColor(tool.palette, QPalette::Active, QPalette::WindowText, Qt::gray);
                QPalette_setColor(tool.palette, QPalette::Active, QPalette::HighlightedText, Qt::gray);
            }

            int margin = static_cast<int>(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QColor color = highlight ? tool.palette.color(QPalette::HighlightedText) : tool.palette.color(QPalette::WindowText);
            drawSymbolButton(painter, SegoeMDL2Font::ChromeClose, tool.rect, color);
        }
 
        tool.palette = savePalette;
        if (tb->subControls & QStyle::SC_TitleBarMaxButton && tb->titleBarFlags & Qt::WindowMaximizeButtonHint && !(tb->titleBarState & Qt::WindowMaximized)) 
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMaxButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarMaxButton) && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarMaxButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Button, Qt::transparent);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, tb->palette.color(QPalette::Active, QPalette::Highlight).darker(120));
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = static_cast<int>(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QColor color = highlight ? tool.palette.color(QPalette::HighlightedText) : tool.palette.color(QPalette::WindowText);
            drawSymbolButton(painter, SegoeMDL2Font::ChromeMaximize, tool.rect, color);
        }

        if (tb->subControls & QStyle::SC_TitleBarMinButton && tb->titleBarFlags & Qt::WindowMinimizeButtonHint && !(tb->titleBarState & Qt::WindowMinimized))
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMinButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarMinButton) && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarMinButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;

            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Button, Qt::transparent);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, tb->palette.color(QPalette::Active, QPalette::Highlight).darker(120));
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = static_cast<int>(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QColor color = highlight ? tool.palette.color(QPalette::HighlightedText) : tool.palette.color(QPalette::WindowText);
            drawSymbolButton(painter, SegoeMDL2Font::ChromeMinimize, tool.rect, color);
        }

        bool drawNormalButton = (tb->subControls & QStyle::SC_TitleBarNormalButton) && (((tb->titleBarFlags & Qt::WindowMinimizeButtonHint) &&
            (tb->titleBarState & Qt::WindowMinimized)) || ((tb->titleBarFlags & Qt::WindowMaximizeButtonHint) && (tb->titleBarState & Qt::WindowMaximized)));
        if (drawNormalButton) 
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarNormalButton, widget);
            down = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarNormalButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Button, Qt::transparent);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, tb->palette.color(QPalette::Active, QPalette::Highlight).darker(120));
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = static_cast<int>(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QColor color = highlight ? tool.palette.color(QPalette::HighlightedText) : tool.palette.color(QPalette::WindowText);
            drawSymbolButton(painter, SegoeMDL2Font::ChromeRestore, tool.rect, color);
        }
        if (tb->subControls & QStyle::SC_TitleBarShadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint && !(tb->titleBarState & Qt::WindowMinimized)) 
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarShadeButton, widget);
            down = (tb->activeSubControls & QStyle::SC_TitleBarShadeButton && (option->state & QStyle::State_Sunken));
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarShadeButton) && (option->state & QStyle::State_MouseOver);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Button, Qt::transparent);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, tb->palette.color(QPalette::Active, QPalette::Highlight).darker(120));
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = static_cast<int>(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QColor color = highlight ? tool.palette.color(QPalette::HighlightedText) : tool.palette.color(QPalette::WindowText);
            drawSymbolButton(painter, SegoeMDL2Font::ChromeRestore, tool.rect, color);
        }

        if (tb->subControls & QStyle::SC_TitleBarContextHelpButton && tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarContextHelpButton, widget);

            down = tb->activeSubControls & QStyle::SC_TitleBarContextHelpButton && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarContextHelpButton) && (option->state & QStyle::State_MouseOver);

            tool.rect = ir;
            
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) tool.state |= QStyle::State_Active;
            if (highlight) tool.state |= QStyle::State_MouseOver;

            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Button, Qt::transparent);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, tb->palette.color(QPalette::Active, QPalette::Highlight).darker(120));
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);

            int margin = static_cast<int>(tool.rect.height() / marginRatio);
            tool.rect.adjust(margin, margin, -margin, -margin);
            QPixmap px = p.proxy()->standardPixmap(QStyle::SP_TitleBarContextHelpButton, &tool, widget);
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, px);
        }

        if (tb->subControls & QStyle::SC_TitleBarUnshadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint && tb->titleBarState & Qt::WindowMinimized)
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarUnshadeButton, widget);

            down = tb->activeSubControls & QStyle::SC_TitleBarUnshadeButton && (option->state & QStyle::State_Sunken);
            highlight = (tb->activeSubControls & QStyle::SC_TitleBarUnshadeButton) && (option->state & QStyle::State_MouseOver);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            pm = p.proxy()->standardIcon(QStyle::SP_TitleBarUnshadeButton, &tool, widget).pixmap(QtnPrivate::qtn_getWindow(widget), QSize(10, 10));
#else
            pm = p.proxy()->standardIcon(QStyle::SP_TitleBarUnshadeButton, &tool, widget).pixmap(QSize(10, 10));
#endif
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            if (isActive) 
                tool.state |= QStyle::State_Active;
            if (highlight) 
                tool.state |= QStyle::State_MouseOver;
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Button, Qt::transparent);
            QPalette_setColor(tool.palette, QPalette::Active, QPalette::Highlight, tb->palette.color(QPalette::Active, QPalette::Highlight).darker(120));
            p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, painter, widget);
            painter->save();
            if (down)
                painter->translate(p.proxy()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, tb, widget), p.proxy()->pixelMetric(QStyle::PM_ButtonShiftVertical, tb, widget));
            p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, pm);
            painter->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarSysMenu && tb->titleBarFlags & Qt::WindowSystemMenuHint)
        {
            ir = p.proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarSysMenu, widget);
            if (!tb->icon.isNull()) 
                tb->icon.paint(painter, ir);
            else 
            {
                int iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, tb, widget);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                pm = p.proxy()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, widget).pixmap(QtnPrivate::qtn_getWindow(widget), QSize(iconSize, iconSize));
#else
                pm = p.proxy()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, widget).pixmap(QSize(iconSize, iconSize));
#endif
                tool.rect = ir;
                painter->save();
                p.proxy()->drawItemPixmap(painter, ir, Qt::AlignCenter, pm);
                painter->restore();
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawMdiControls(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const QMdiArea* area = qobject_cast<const QMdiArea*>(widget))
    {
        if (!(QtnPrivate::qtn_findChild(area->parentWidget(), QTITAN_META_CLASSNAME("RibbonBar")) ||
              QtnPrivate::qtn_isAncestor(area, QTITAN_META_CLASSNAME("DockMainWindow"))))
            return false;
    }

    QStyleOptionButton btnOpt;
    btnOpt.QStyleOption::operator = (*opt);
    int bsx = 0, bsy = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    const int buttonIconMetric = p.proxy()->pixelMetric(QStyle::PM_TitleBarButtonIconSize, &btnOpt, widget);
#else
    const int buttonIconMetric = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, &btnOpt, widget);
#endif
    const QSize buttonIconSize(buttonIconMetric, buttonIconMetric);
    if (opt->subControls & QStyle::SC_MdiCloseButton) 
    {
        QStyleOptionButton btnOptClose;
        btnOptClose.QStyleOption::operator = (*opt);
        if (opt->activeSubControls & QStyle::SC_MdiCloseButton && (opt->state & QStyle::State_Sunken)) 
        {
            btnOptClose.state &= ~QStyle::State_MouseOver;
            btnOptClose.state |= QStyle::State_Sunken;
            btnOptClose.state &= ~QStyle::State_Raised;
            bsx = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftHorizontal);
            bsy = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftVertical);
        }
        else if (btnOpt.state & QStyle::State_MouseOver && (opt->activeSubControls & QStyle::SC_MdiCloseButton))
        {
            btnOptClose.state |= QStyle::State_Raised;
            btnOptClose.state &= ~QStyle::State_Sunken;
        }
        else 
        {
            btnOptClose.state &= ~QStyle::State_MouseOver;
            btnOptClose.state |= QStyle::State_Raised;
            btnOptClose.state &= ~QStyle::State_Sunken;
            bsx = 0;
            bsy = 0;
        }
        btnOptClose.rect = p.proxy()->subControlRect(QStyle::CC_MdiControls, opt, QStyle::SC_MdiCloseButton, widget);
        p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &btnOptClose, painter, widget);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QPixmap pm = p.proxy()->standardIcon(QStyle::SP_TitleBarCloseButton).pixmap(QtnPrivate::qtn_getWindow(widget), buttonIconSize);
#else
        QPixmap pm = p.proxy()->standardIcon(QStyle::SP_TitleBarCloseButton).pixmap(buttonIconSize);
#endif

        p.proxy()->drawItemPixmap(painter, btnOptClose.rect.translated(bsx, bsy), Qt::AlignCenter, pm);
    }

    if (opt->subControls & QStyle::SC_MdiNormalButton) 
    {
        QStyleOptionButton btnOptNormal;
        btnOptNormal.QStyleOption::operator = (*opt);
        if (opt->activeSubControls & QStyle::SC_MdiNormalButton && (opt->state & QStyle::State_Sunken)) 
        {
            btnOptNormal.state &= ~QStyle::State_MouseOver;
            btnOptNormal.state |= QStyle::State_Sunken;
            btnOptNormal.state &= ~QStyle::State_Raised;
            bsx = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftHorizontal);
            bsy = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftVertical);
        }
        else if (btnOpt.state & QStyle::State_MouseOver && (opt->activeSubControls & QStyle::SC_MdiNormalButton))
        {
            btnOptNormal.state |= QStyle::State_Raised;
            btnOptNormal.state &= ~QStyle::State_Sunken;
        }
        else
        {
            btnOptNormal.state &= ~QStyle::State_MouseOver;
            btnOptNormal.state |= QStyle::State_Raised;
            btnOptNormal.state &= ~QStyle::State_Sunken;
            bsx = 0;
            bsy = 0;
        }
        btnOptNormal.rect = p.proxy()->subControlRect(QStyle::CC_MdiControls, opt, QStyle::SC_MdiNormalButton, widget);
        p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &btnOptNormal, painter, widget);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QPixmap pm = p.proxy()->standardIcon(QStyle::SP_TitleBarNormalButton).pixmap(QtnPrivate::qtn_getWindow(widget), buttonIconSize);
#else
        QPixmap pm = p.proxy()->standardIcon(QStyle::SP_TitleBarNormalButton).pixmap(buttonIconSize);
#endif
        p.proxy()->drawItemPixmap(painter, btnOptNormal.rect.translated(bsx, bsy), Qt::AlignCenter, pm);
    }

    if (opt->subControls & QStyle::SC_MdiMinButton) 
    {
        QStyleOptionButton btnOptMin;
        btnOptMin.QStyleOption::operator = (*opt);
        if (opt->activeSubControls & QStyle::SC_MdiMinButton && (opt->state & QStyle::State_Sunken)) 
        {
            btnOptMin.state &= ~QStyle::State_MouseOver;
            btnOptMin.state |= QStyle::State_Sunken;
            btnOptMin.state &= ~QStyle::State_Raised;
            bsx = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftHorizontal);
            bsy = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftVertical);
        }
        else if (btnOpt.state & QStyle::State_MouseOver && (opt->activeSubControls & QStyle::SC_MdiMinButton))
        {
            btnOptMin.state |= QStyle::State_Raised;
            btnOptMin.state &= ~QStyle::State_Sunken;
        }
        else 
        {
            btnOptMin.state &= ~QStyle::State_MouseOver;
            btnOptMin.state |= QStyle::State_Raised;
            btnOptMin.state &= ~QStyle::State_Sunken;
            bsx = 0;
            bsy = 0;
        }
        btnOptMin.rect = p.proxy()->subControlRect(QStyle::CC_MdiControls, opt, QStyle::SC_MdiMinButton, widget);
        p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &btnOptMin, painter, widget);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QPixmap pm = p.proxy()->standardIcon(QStyle::SP_TitleBarMinButton).pixmap(QtnPrivate::qtn_getWindow(widget), buttonIconSize);
#else
        QPixmap pm = p.proxy()->standardIcon(QStyle::SP_TitleBarMinButton).pixmap(buttonIconSize);
#endif
        p.proxy()->drawItemPixmap(painter, btnOptMin.rect.translated(bsx, bsy), Qt::AlignCenter, pm);
    }
    return true;
}

bool CommonStylePrivate::drawComboBox(const QStyleOptionComplex* /*option*/, QPainter* /*painter*/, const QWidget* /*widget*/) const
{
    return false;
}

bool CommonStylePrivate::drawSlider(const QStyleOptionComplex* /*option*/, QPainter* /*painter*/, const QWidget* /*widget*/) const
{
    return false;
}

bool CommonStylePrivate::drawSpinBox(const QStyleOptionComplex* /*option*/, QPainter* /*painter*/, const QWidget* /*widget*/) const
{
    return false;
}

bool CommonStylePrivate::drawToolButton(const QStyleOptionComplex* /*option*/, QPainter* /*painter*/, const QWidget* /*widget*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawScrollBar(const QStyleOptionComplex* /*opt*/, QPainter* /*painter*/, const QWidget* /*widget*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawMenuBarEmptyArea(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawShapedFrame(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*widget*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawMenuBarItem(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawMenuItem(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void CommonStylePrivate::tabLayout(const QStyleOptionTab* opt, const QWidget* widget, QRect* textRect, QRect* iconRect) const
#else
void CommonStylePrivate::tabLayout(const QStyleOptionTabV3* opt, const QWidget* widget, QRect* textRect, QRect* iconRect) const
#endif
{
    Q_ASSERT(textRect);
    Q_ASSERT(iconRect);
    QTN_P(const CommonStyle);
    QRect tr = opt->rect;
    bool verticalTabs = opt->shape == QTabBar::RoundedEast
        || opt->shape == QTabBar::RoundedWest
        || opt->shape == QTabBar::TriangularEast
        || opt->shape == QTabBar::TriangularWest;
    if (verticalTabs)
        tr.setRect(0, 0, tr.height(), tr.width()); // 0, 0 as we will have a translate transform

    int verticalShift = p.proxy()->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
    int horizontalShift = p.proxy()->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);
    int hpadding = p.proxy()->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
    int vpadding = p.proxy()->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;
    if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
        verticalShift = -verticalShift;
    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);
    bool selected = opt->state & QStyle::State_Selected;
    if (selected)
    {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty())
        tr.setLeft(tr.left() + 4 + (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    // right widget
    if (!opt->rightButtonSize.isEmpty())
        tr.setRight(tr.right() - 4 - (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));

    // icon
    if (!opt->icon.isNull())
    {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid())
        {
            int iconExtent = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(iconSize, (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
            (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
        // High-dpi icons do not need adjustment; make sure tabIconSize is not larger than iconSize
        tabIconSize = QSize(qMin(tabIconSize.width(), iconSize.width()), qMin(tabIconSize.height(), iconSize.height()));

        *iconRect = QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2, tabIconSize.width(), tabIconSize.height());
        if (!verticalTabs)
            *iconRect = p.proxy()->visualRect(opt->direction, opt->rect, *iconRect);
        tr.setLeft(tr.left() + tabIconSize.width() + 4);
    }

    if (!verticalTabs)
        tr = p.proxy()->visualRect(opt->direction, opt->rect, tr);

    *textRect = tr;
}

/*! \internal */
bool CommonStylePrivate::drawTabBarTabLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("AbstractTab")))
    {
        if (const StyleOptionTab* tab = qstyleoption_cast<const StyleOptionTab*>(opt))
        {
            QRect tr = tab->rect;
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                alignment |= Qt::TextHideMnemonic;

            QRect iconRect;
            tabLayout(tab, widget, &tr, &iconRect);
            tr = p.proxy()->subElementRect(QStyle::SE_TabBarTabText, tab, widget);

            if (!tab->icon.isNull())
            {
                painter->setPen(tab->palette.windowText().color());
                tab->icon.paint(painter, iconRect, Qt::AlignCenter, (tab->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                    (tab->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
            }

            p.proxy()->drawItemText(painter, tr, alignment, tab->palette, tab->state & QStyle::State_Enabled, tab->text, QPalette::WindowText);
            if (!tab->rightButtonSize.isNull())
            {
                QStyleOption optButton = *tab;
                QRect rect = p.proxy()->subElementRect(QStyle::SE_TabBarTabRightButton, tab, widget);
                optButton.rect = rect.adjusted(10, 0, 0, 0);
                if (tab->hoverClose)
                    optButton.state |= QStyle::State_Raised;
                p.proxy()->drawPrimitive(QStyle::PE_IndicatorTabClose, &optButton, painter, widget);
            }

            if (tab->state & QStyle::State_HasFocus)
            {
                const int OFFSET = 1 + p.pixelMetric(QStyle::PM_DefaultFrameWidth);

                int x1, x2;
                x1 = tab->rect.left();
                x2 = tab->rect.right() - 1;

                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*tab);
                fropt.rect.setRect(x1 + 1 + OFFSET, tab->rect.y() + OFFSET,
                    x2 - x1 - 2 * OFFSET, tab->rect.height() - 2 * OFFSET);
                p.drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, painter, widget);
            }
        }
        return true;
    }
    return false;
}


/*! \internal */
bool CommonStylePrivate::drawItemViewItem(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*widget*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawSplitter(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockPanelSplitter")))
    {
        p->fillRect(opt->rect, opt->palette.window());
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawProgressBarContents(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (!isQtitanStyle())
        return false;
    QRect rect = option->rect;
    painter->save();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QStyleOptionProgressBar* bar = qstyleoption_cast<const QStyleOptionProgressBar*>(option))
#else
    if (const QStyleOptionProgressBarV2* bar = qstyleoption_cast<const QStyleOptionProgressBarV2*>(option))
#endif
    {
        bool vertical = false;
        bool inverted = false;
        bool indeterminate = (bar->minimum == 0 && bar->maximum == 0);
        bool complete = bar->progress == bar->maximum;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        vertical = (bar->orientation == Qt::Vertical);
#endif
        inverted = bar->invertedAppearance;
        if (vertical) 
        {
            rect = QRect(rect.left(), rect.top(), rect.height(), rect.width());
            QTransform m = QTransform::fromTranslate(rect.height() - 1, -1.0);
            m.rotate(90.0);
            painter->setTransform(m, true);
        }

        int maxWidth = rect.width();
        const auto progress = qMax(bar->progress, bar->minimum);
        const auto totalSteps = qMax(Q_INT64_C(1), qint64(bar->maximum) - bar->minimum);
        const auto progressSteps = qint64(progress) - bar->minimum;
        const auto progressBarWidth = progressSteps * maxWidth / totalSteps;
        int width = indeterminate ? maxWidth : progressBarWidth;

        bool reverse = (!vertical && (bar->direction == Qt::RightToLeft)) || vertical;
        if (inverted)
            reverse = !reverse;

        QRect progressBar;
        QColor outline = option->palette.window().color().darker(140);
        QColor highlight = option->palette.color(QPalette::Highlight);
        QColor highlightedoutline = highlight.darker(140);
        if (qGray(outline.rgb()) > qGray(highlightedoutline.rgb()))
            outline = highlightedoutline;

        if (!indeterminate) 
        {
            QColor innerShadow(Qt::black);
            innerShadow.setAlpha(35);
            painter->setPen(innerShadow);
            if (!reverse) 
            {
                progressBar.setRect(rect.left(), rect.top(), width - 1, rect.height() - 1);
                if (!complete) 
                {
                    painter->drawLine(progressBar.topRight() + QPoint(2, 1), progressBar.bottomRight() + QPoint(2, 0));
                    painter->setPen(QPen(highlight.darker(140)));
                    painter->drawLine(progressBar.topRight() + QPoint(1, 1), progressBar.bottomRight() + QPoint(1, 0));
                }
            }
            else 
            {
                progressBar.setRect(rect.right() - width - 1, rect.top(), width + 2, rect.height() - 1);
                if (!complete) 
                {
                    painter->drawLine(progressBar.topLeft() + QPoint(-2, 1), progressBar.bottomLeft() + QPoint(-2, 0));
                    painter->setPen(QPen(highlight.darker(140)));
                    painter->drawLine(progressBar.topLeft() + QPoint(-1, 1), progressBar.bottomLeft() + QPoint(-1, 0));
                }
            }
        }
        else 
            progressBar.setRect(rect.left(), rect.top(), rect.width() - 1, rect.height() - 1);

        if (indeterminate || bar->progress > bar->minimum) 
        {
            painter->setPen(QPen(outline));
            QColor highlightedGradientStartColor = highlight;//.lighter(120);
            QColor highlightedGradientStopColor = highlight;
            QLinearGradient gradient(rect.topLeft(), QPoint(rect.bottomLeft().x(), rect.bottomLeft().y()));
            gradient.setColorAt(0, highlightedGradientStartColor);
            gradient.setColorAt(1, highlightedGradientStopColor);
            painter->setBrush(gradient);

            QRect fillRect = progressBar.adjusted(!indeterminate && !complete && reverse ? -2 : 0, 0,
                indeterminate || complete || reverse ? 0 : 2, 0);
            painter->drawRect(fillRect);
        }
    }
    painter->restore();
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawProgressBarLabel(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (!isQtitanStyle())
        return false;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QStyleOptionProgressBar* bar = qstyleoption_cast<const QStyleOptionProgressBar*>(option))
#else
    if (const QStyleOptionProgressBarV2* bar = qstyleoption_cast<const QStyleOptionProgressBarV2*>(option))
#endif
    {
        QRect leftRect;
        QRect rect = bar->rect;
        QColor textColor = option->palette.text().color();
        QColor alternateTextColor = option->palette.color(QPalette::HighlightedText);
        painter->save();
        bool vertical = false, inverted = false;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        vertical = (bar->orientation == Qt::Vertical);
#endif
        inverted = bar->invertedAppearance;
        if (vertical)
            rect = QRect(rect.left(), rect.top(), rect.height(), rect.width());
        const auto totalSteps = qMax(Q_INT64_C(1), qint64(bar->maximum) - bar->minimum);
        const auto progressSteps = qint64(bar->progress) - bar->minimum;
        const auto progressIndicatorPos = progressSteps * rect.width() / totalSteps;
        if (progressIndicatorPos >= 0 && progressIndicatorPos <= rect.width())
            leftRect = QRect(rect.left(), rect.top(), progressIndicatorPos, rect.height());
        if (vertical)
            leftRect.translate(rect.width() - progressIndicatorPos, 0);

        bool flip = (!vertical && (((bar->direction == Qt::RightToLeft) && !inverted) ||
            ((bar->direction == Qt::LeftToRight) && inverted)));

        QRegion rightRect = rect;
        rightRect = rightRect.subtracted(leftRect);
        painter->setClipRegion(rightRect);
        painter->setPen(flip ? alternateTextColor : textColor);
        painter->drawText(rect, bar->text, QTextOption(Qt::Alignment(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter)));
        if (!leftRect.isNull()) 
        {
            painter->setPen(flip ? textColor : alternateTextColor);
            painter->setClipRect(leftRect);
            painter->drawText(rect, bar->text, QTextOption(Qt::Alignment(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter)));
        }
        painter->restore();
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawGroupBox(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*widget*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawToolBoxTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (w == Q_NULL || !w->inherits(QTITAN_META_CLASSNAME("GridBase")))
        return false;
    if (const QStyleOptionToolBox* toolBoxOption = qstyleoption_cast<const QStyleOptionToolBox *>(opt))
    {
        QColor accentColor;
        if (toolBoxOption->state & QStyle::State_Sunken && toolBoxOption->state & QStyle::State_MouseOver)
            accentColor = toolBoxOption->palette.color(QPalette::Dark);
        else
        {
            if (toolBoxOption->state & QStyle::State_MouseOver)
                accentColor = toolBoxOption->palette.color(QPalette::Light);
            else
                accentColor = toolBoxOption->palette.color(QPalette::Mid);
        }
        if (accentColor != Qt::transparent)
        {
            QPixmap texture(QStringLiteral(":res/glass_background.png"));
            QPixmap temp(texture.width(), texture.height());
            temp.fill(accentColor);
            temp.setMask(texture.createHeuristicMask());
            {
                QPainter p(&temp);
                p.drawPixmap(0, 0, texture);
            }
            qtn_draw_pixmap_texture(p, toolBoxOption->rect, temp, 2, 2, 2, 2);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawToolButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (widget != Q_NULL && widget->parentWidget() != Q_NULL && widget->parentWidget()->inherits(QTITAN_META_CLASSNAME("RibbonQuickAccessBar")))
        return drawToolButtonLabelDp(opt, painter, widget);

    if (widget == Q_NULL || !widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemButton")))
        return false;

    QTN_P(const CommonStyle);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) 
    {
        QRect rect = toolbutton->rect;
        int shiftX = 0;
        int shiftY = 0;
        if (toolbutton->state & (QStyle::State_Sunken | QStyle::State_On))
        {
            shiftX = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, toolbutton, widget);
            shiftY = p.proxy()->pixelMetric(QStyle::PM_ButtonShiftVertical, toolbutton, widget);
        }
        // Arrow type always overrules and is always shown
        bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) 
        {
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                alignment |= Qt::TextHideMnemonic;
            rect.translate(shiftX, shiftY);
            painter->setFont(toolbutton->font);
            p.proxy()->drawItemText(painter, rect, alignment, toolbutton->palette, opt->state & QStyle::State_Enabled, toolbutton->text, QPalette::ButtonText);
        }
        else 
        {
            QSize pmSize = toolbutton->iconSize;
            QPixmap pm = qtn_pixmap(toolbutton->icon, toolbutton->rect, toolbutton->iconSize, toolbutton->state, widget);
            if (!pm.isNull())
                pmSize = pm.size();

            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) 
            {
                painter->setFont(toolbutton->font);
                QRect pr = rect,
                    tr = rect;
                int alignment = Qt::TextShowMnemonic;
                if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                    alignment |= Qt::TextHideMnemonic;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) 
                {
                    pr.setHeight(pmSize.height() + 6);
                    tr.adjust(0, pr.height() - 1, 0, -1);
                    pr.translate(shiftX, shiftY);
                    if (!hasArrow) 
                        p.proxy()->drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                    else 
                        QtnPrivate::drawArrow(p.proxy(), toolbutton, pr, painter, widget);
                    alignment |= Qt::AlignCenter;
                }
                else {
                    pr.setWidth(pmSize.width() + 8);
                    tr.adjust(pr.width(), 0, 0, 0);
                    pr.translate(shiftX, shiftY);
                    if (!hasArrow) 
                        p.proxy()->drawItemPixmap(painter, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                    else 
                        QtnPrivate::drawArrow(p.proxy(), toolbutton, pr, painter, widget);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                tr.translate(shiftX, shiftY);
                p.proxy()->drawItemText(painter, QStyle::visualRect(opt->direction, rect, tr), alignment, toolbutton->palette, 
                    toolbutton->state & QStyle::State_Enabled, toolbutton->text, QPalette::ButtonText);
            }
            else {
                rect.translate(shiftX, shiftY);
                if (hasArrow) 
                    QtnPrivate::drawArrow(p.proxy(), toolbutton, rect, painter, widget);
                else
                    p.proxy()->drawItemPixmap(painter, rect, Qt::AlignCenter, pm);
            }
        }
        return true;
    }
    return false;
}

bool CommonStylePrivate::drawSizeGrip(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
    return false;
}

bool CommonStylePrivate::drawToolBar(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* widget) const
{
    if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("RibbonQuickAccessBar")))
    {
        return true;
    }
    return false;
}

void CommonStylePrivate::drawMenuItemSeparator(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
}

bool CommonStylePrivate::drawScrollBarAddSubLine(QStyle::ControlElement /*ce*/, const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{
    return false;
}

bool CommonStylePrivate::drawScrollBarAddSubPage(QStyle::ControlElement /*ce*/, const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{
    return false;
}

bool CommonStylePrivate::drawScrollBarSlider(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*w*/) const
{
    return false;
}


/*! \internal */
bool CommonStylePrivate::drawTabBarTabShape(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("DockAutoHideLayoutPanel")))
    {
        if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab *>(option))
        {
            painter->fillRect(tab->rect, option->palette.window());
            return true;
        }
    }
    else if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("AbstractTab")))
    {
        if (const StyleOptionTab* tab = qstyleoption_cast<const StyleOptionTab*>(option))
        {
            const bool mouseOver = tab->state & QStyle::State_MouseOver;
            const bool selected = tab->state & QStyle::State_Selected;
            const bool firstTab = tab->position == QStyleOptionTab::Beginning;
            Q_UNUSED(firstTab);
            const bool lastTab = tab->position == QStyleOptionTab::End;
 
            QRect rect = tab->rect;
            painter->fillRect(rect, selected ? option->palette.base() : option->palette.dark());
            if (!tab->previewPixmap.isNull() && !tab->previewRect.isEmpty())
            {
                rect.setHeight(rect.height() + tab->previewRect.height());
                const QRect previewRect = tab->previewRect;
                painter->fillRect(previewRect, option->palette.window());
                int space = CommonStylePrivate::dpiScaled(3, widget);
                const QRect previewPixmapRect = previewRect.adjusted(0, space, 0, -space);
                painter->drawPixmap(previewPixmapRect, tab->previewPixmap);
                if (!selected)
                {
                    QColor color = tab->palette.dark().color();
                    color.setAlphaF(0.5);
                    painter->fillRect(previewRect, color);
                }
            }
            painter->setPen(option->palette.shadow().color());
            painter->setClipRect(rect);
            rect = rect.adjusted(0, 0, lastTab ? -1 : 0, -1);
            painter->drawRect(rect);

            if (widget->inherits(QTITAN_META_CLASSNAME("NavigationTab")))
            {
                QRect r = tab->rect;
                if (mouseOver)
                    qtn_paintLight(widget, painter, r);
                if (mouseOver)
                    qtn_paintLightFrame(widget, painter, r);
            }

            return true;
        }
    }
    return false;
}

bool CommonStylePrivate::drawHeader(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
#if 0
    if (w && w->inherits(QTITAN_META_CLASSNAME("GridBase")))
    {
        if (const GridHeaderStyleOption* header = qstyleoption_cast<const GridHeaderStyleOption *>(opt))
        {
            QTN_P(const CommonStyle);
            QRegion clipRegion = painter->clipRegion();
            painter->setClipRect(header->rect);
            p.proxy()->drawControl(QStyle::CE_HeaderSection, header, painter, w);
            QStyleOptionHeader subopt = *header;
            subopt.rect = p.proxy()->subElementRect(QStyle::SE_HeaderLabel, header, w);
            subopt.rect.adjust(header->margins.left(), 
                header->margins.top(), -header->margins.right(), -header->margins.bottom());
            if (subopt.rect.isValid())
                p.proxy()->drawControl(QStyle::CE_HeaderLabel, &subopt, painter, w);
            /*if (header->sortIndicator != QStyleOptionHeader::None) {
                subopt.rect = subElementRect(SE_HeaderArrow, opt, widget);
                proxy()->drawPrimitive(PE_IndicatorHeaderArrow, &subopt, p, widget);
            }*/
            painter->setClipRegion(clipRegion);
            return true;
        }
    }
#endif
    Q_UNUSED(opt);
    Q_UNUSED(painter);
    Q_UNUSED(w);
    return false;
}

bool CommonStylePrivate::drawHeaderSection(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(w);
    return false;
}

bool CommonStylePrivate::drawHeaderLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (w && w->inherits(QTITAN_META_CLASSNAME("GridBase")))
    {
        QVariant v = w->property(QTN_HEADER_OPTION_PROPERTY);
        GridHeaderStyleOption* option = v.isValid() ? static_cast<GridHeaderStyleOption *>(v.value<void *>()) : Q_NULL;
        if (option != Q_NULL)
        {
            QTN_P(const CommonStyle);
            QRect rect = option->rect;
            QPalette palette = opt->palette/*CSS override this palette!*/;
            QPen pen = painter->pen();
            pen.setBrush(palette.brush(QPalette::ButtonText));
            painter->setPen(pen);

            int margin = qRound(p.proxy()->pixelMetric(QStyle::PM_HeaderMargin, Q_NULL, w) * option->scaleFactor);
            int iconSize = qRound(p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULL, w) * option->scaleFactor);
            QRect textRect = qtn_paint_icon_and_text(*painter, option->direction, palette,
                rect, option->icon, QSize(iconSize, iconSize), option->state & QStyle::State_Enabled, 
                option->iconAlignment, option->text, option->textAlignment, option->textElideMode, margin);

            QTextDocument* textDocument = option->textDocument;
            if (textDocument != Q_NULL)
            {
                //Draw HTML if needed
                option->rect = textRect;
                if (painter->font() != textDocument->defaultFont())
                    textDocument->setDefaultFont(painter->font());
                if (textRect.width() != qRound(textDocument->textWidth()))
                    textDocument->setTextWidth(textRect.width());
                if (textDocument->defaultTextOption().alignment() != option->textAlignment)
                    textDocument->setDefaultTextOption(QTextOption(option->textAlignment));

                painter->translate(textRect.left(), textRect.top());
                textDocument->drawContents(painter, QRect(QPoint(0, 0), textRect.size()));
            }
            return true;
        }
    }
    return false;
}

bool CommonStylePrivate::drawDockWidgetTitle(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    if (w && w->inherits(QTITAN_META_CLASSNAME("DockTitleBar")))
    {
        if (const DockTitleBarStyleOption* dwOpt = qstyleoption_cast<const DockTitleBarStyleOption*>(option))
        {
            painter->save();
            const QRect rc = dwOpt->rect;
            const bool active = option->state & QStyle::State_Active;

            QColor clr = active ? StyleHelper::colorMakeLighter(dwOpt->palette.window().color().rgb(), 0.5) : 
                dwOpt->palette.window().color().darker(110);
            painter->setBrush(clr);
            painter->setPen(dwOpt->palette.window().color().darker(130));
            qtn_drawRect(painter, rc);

            if (!dwOpt->icon.isNull())
            {
                    QRect rc = dwOpt->rcIcon;
                    QPixmap pixmap = dwOpt->icon.pixmap(p.pixelMetric(QStyle::PM_SmallIconSize, option, w));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                    const int pixw = qRound(static_cast<qreal>(pixmap.width()) / pixmap.devicePixelRatio());
                    const int pixh = qRound(static_cast<qreal>(pixmap.height()) / pixmap.devicePixelRatio());
#else
                    const int pixw = pixmap.width();
                    const int pixh = pixmap.height();
#endif
                    rc = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(pixw, pixh), rc);
                    if (!pixmap.isNull())
                            painter->drawPixmap(rc, pixmap);
            }

            QRect titleRect = dwOpt->rcText;
            const QString text = dwOpt->fontMetrics.elidedText(dwOpt->title, Qt::ElideMiddle, titleRect.width() - 6);
            p.proxy()->drawItemText(painter, titleRect.adjusted(6, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                dwOpt->palette, true, text, QPalette::WindowText);
            painter->restore();
            return true;
        }
    }
#if 0
    else if (w && w->inherits(QTITAN_META_CLASSNAME("DockTabbedLayoutPanel")))
    {
        if (const DockPaneStyleOption* dwOpt = qstyleoption_cast<const DockPaneStyleOption*>(option))
        {
            QRect rect = dwOpt->rect;
            const bool verticalTitleBar = dwOpt->verticalTitleBar;
            const bool active = (dwOpt->state & QStyle::State_Active);
            const QColor highlight = option->palette.highlight().color().lighter();

            if (verticalTitleBar)
            {
                painter->translate(rect.left() - 1, rect.top() + rect.width());
                painter->rotate(-90);
                painter->translate(-rect.left() + 1, -rect.top());
            }
            painter->setBrush(active ? highlight : dwOpt->palette.window().color().darker(110));
            painter->setPen(dwOpt->palette.window().color().darker(130));
            qtn_drawRect(painter, rect);
            return true;
        }
    }
#endif
    return false;
}

bool CommonStylePrivate::drawRibbonBar(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const RibbonStyleOption* styleOption = qstyleoption_cast<const RibbonStyleOption*>(opt))
    {
        QPalette::ColorRole role = widget == Q_NULL ? QPalette::Window : widget->backgroundRole();
        painter->fillRect(opt->rect, opt->palette.color(role));
        if (styleOption->minimized)
        {
            QPainterPath path;
            path.moveTo(opt->rect.left(), opt->rect.bottom());
            path.lineTo(opt->rect.right(), opt->rect.bottom());
            const QPen savePen = painter->pen();
            painter->setPen(styleOption->palette.dark().color().lighter(135));
            painter->drawPath(path);
            painter->setPen(savePen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonFrameGroups(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const RibbonStyleOption* styleOption = qstyleoption_cast<const RibbonStyleOption*>(option))
    {
        QRect rect = styleOption->rect;

        p->fillRect(rect, styleOption->palette.button().color());
        //return true;
        QRect rectActiveTab = styleOption->rectCurrentTab;
        if (styleOption->tabBarPosition == 1)
        {
            QPainterPath path;
            path.moveTo(rect.left(), rect.top());
            path.lineTo(rectActiveTab.left(), rect.top());
            path.moveTo(rectActiveTab.right(), rect.top());
            path.lineTo(rect.right(), rect.top());

            const QPen savePen = p->pen();
            QColor color = CommonStyle::contextColorToColor(styleOption->pageColor);
            p->setPen(color.isValid() ? color : option->palette.midlight().color());
            p->drawPath(path);

            QPainterPath path1;
            path1.moveTo(rect.right(), rect.top());
            path1.lineTo(rect.right(), rect.bottom());
            path1.lineTo(rect.left(), rect.bottom());
            path1.lineTo(rect.left(), rect.top());

            p->setPen(option->palette.midlight().color());
            p->drawPath(path1);
            p->setPen(savePen);
        }
        else if (styleOption->tabBarPosition == 2)
        {
            QPainterPath path;
            path.moveTo(rect.left(), rect.bottom());
            path.lineTo(rectActiveTab.left(), rect.bottom());
            path.moveTo(rectActiveTab.right(), rect.bottom());

            path.lineTo(rect.right(), rect.bottom());
            path.lineTo(rect.right(), rect.top());
            path.lineTo(rect.left(), rect.top());
            path.lineTo(rect.left(), rect.bottom());

            const QPen savePen = p->pen();
            QColor color = CommonStyle::contextColorToColor(styleOption->pageColor);
            p->setPen(QPen(color.isValid() ? color : option->palette.midlight().color(), 1));
            p->drawPath(path);
            p->setPen(savePen);
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonFrameGroup(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget)
    QRect rect = option->rect;
    const QPen savePen = p->pen();
    p->setPen(option->palette.midlight().color());
    p->drawLine(QPoint(rect.right() - 1, rect.top()), QPoint(rect.right() - 1, rect.bottom() - 6));
    p->setPen(savePen);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonFrameGallery(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    if (const QtStyleOptionFrame* frameGallery = qstyleoption_cast<const QtStyleOptionFrame*>(opt))
    {
        const QMenu* parentMenu = qobject_cast<const QMenu*>(widget ? widget->parentWidget() : Q_NULL);
        if (parentMenu)
        {
            QPalette pal = parentMenu->palette();
            painter->fillRect(opt->rect, darkBackgroundLight(pal).lighter());
            return true;
        }
        if (frameGallery->features & QtStyleOptionFrame::Flat)
        {
            bool enabled = opt->state & QStyle::State_Enabled;
            bool selected = opt->state & QStyle::State_MouseOver;
            painter->fillRect(opt->rect, selected && enabled ? StyleHelper::colorMakeDarker(opt->palette.color(QPalette::Base).rgb(), 0.01) :
                StyleHelper::colorMakeDarker(opt->palette.color(QPalette::Base).rgb(), 0.03));
//            painter->fillRect(opt->rect, selected && enabled ? darkBackgroundLight(opt->palette).light() : darkBackgroundLight(opt->palette));
        }
        if (frameGallery->lineWidth > 0)
        {
            const QPen savePen = painter->pen();
            painter->setPen(outlineMidlight(opt->palette));
            qtn_drawRect(painter, opt->rect);
            painter->setPen(savePen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonSysFrameMenu(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const RibbonFrameMenuStyleOption* frameMenu = qstyleoption_cast<const RibbonFrameMenuStyleOption*>(opt))
    {
        QRect rect = opt->rect;
        const QPen savePen = painter->pen();
        painter->setPen(opt->palette.brush(QPalette::Mid).color());
        painter->fillRect(rect, darkBackground(opt->palette));
        qtn_drawRect(painter, rect);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        rect -= frameMenu->margins;
#endif
        painter->drawLine(rect.left(), rect.top(), rect.right(), rect.top());
        painter->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
        rect.setWidth(frameMenu->actionsWidth);
        painter->fillRect(rect.adjusted(0, 1, 0, -1), darkBackgroundLight(frameMenu->palette));
        if (frameMenu->isAdditionalPages)
        {
            painter->setPen(outlineMidlight(opt->palette));
            painter->drawLine(rect.right(), rect.top() + 1, rect.right(), rect.bottom() - 1);
            painter->setPen(outlineLight(opt->palette));
            painter->drawLine(rect.right() + 1, rect.top() + 1, rect.right() + 1, rect.bottom() - 1);
        }
        painter->setPen(savePen);
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonSysFramePagePopupList(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    QFont saveFont = painter->font();
    QFont font = saveFont;
    font.setBold(true);
    painter->setFont(font);

    QFontMetrics fm(font);
    QRect rcCaption = p.proxy()->subElementRect(static_cast<QStyle::SubElement>(CommonStyle::SE_RibbonSysHeaderLabelPopupList), opt, widget);

    const int hMarginCaption = p.proxy()->pixelMetric(static_cast<QStyle::PixelMetric>(CommonStyle::PM_RibbonHMarginCaptionSysMenu), opt, widget);
    QRect rcText = rcCaption.adjusted(hMarginCaption, 0, 0, 0);

    QPalette palette = opt->palette;
    QPalette_setColor(palette, QPalette::WindowText, palette.color(QPalette::Dark).darker(150));
    int alignment = Qt::AlignVCenter | Qt::TextSingleLine;
    p.proxy()->drawItemText(painter, rcText, alignment, palette, true, widget->windowTitle(), QPalette::WindowText);
    painter->setFont(saveFont);

    painter->setPen(outlineMidlight(opt->palette));
    painter->drawLine(rcCaption.left(), rcCaption.bottom(), rcCaption.right(), rcCaption.bottom());
    painter->setPen(outlineLight(opt->palette));
    painter->drawLine(rcCaption.left(), rcCaption.bottom() + 1, rcCaption.right(), rcCaption.bottom() + 1);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonSysFramePagePopup(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    Q_UNUSED(widget);
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        QRect rect = opt->rect;
        const QPen savePen = painter->pen();
        painter->setPen(outlineMidlight(opt->palette));
        painter->fillRect(rect, darkBackgroundLight(opt->palette));
        qtn_drawRect(painter, rect);
        painter->setPen(savePen);

        QRect rcPopupLabel = menuitem->menuRect;
        painter->fillRect(rcPopupLabel, darkBackground(menuitem->palette));
        painter->fillRect(rcPopupLabel.left(), rcPopupLabel.bottom() - 1, rcPopupLabel.width(), 1, outlineMidlight(menuitem->palette));

        QFont fontSave = painter->font();
        QFont font = fontSave;
        font.setBold(true);
        painter->setFont(font);

        QRect rcText(rcPopupLabel);
        rcText.adjust(7, 0, 0, 0);

        QPalette palette = menuitem->palette;
        QPalette_setColor(palette, QPalette::WindowText, palette.color(QPalette::Dark).darker(150));

        int flags = 0;
        flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;

        QString text = menuitem->text;
        text = painter->fontMetrics().elidedText(text, Qt::ElideRight, rcText.adjusted(2, 0, -2, 0).width());
        // draw text
        p.proxy()->drawItemText(painter, rcText, flags, palette, opt->state & QStyle::State_Enabled, text, QPalette::WindowText);
        painter->setFont(fontSave);
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonPanelButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool checked = opt->state & QStyle::State_On;
        const bool highlighted = opt->state & QStyle::State_MouseOver && enabled;
        const bool mouseInSplit = opt->state & QStyle::State_MouseOver && (toolbutton->activeSubControls & QStyle::SC_ToolButton);
        const bool mouseInSplitDropDown = opt->state & QStyle::State_MouseOver && (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu);
        const bool pressed = opt->state & QStyle::State_Sunken;
        const bool popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

        if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup))
        {
            bool autoRaise = toolbutton->state & QStyle::State_AutoRaise;
            if (!autoRaise && !qobject_cast<const QStatusBar*>(w->parentWidget()))
            {
                QStyleOptionButton tmpBtn;// = *btn;
                tmpBtn.state = toolbutton->state;
                tmpBtn.rect = toolbutton->rect;
                p.proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &tmpBtn, painter, w);
            }
            else
            {
                QColor colorFill;
                QColor colorBorder;
                if (!enabled) colorFill = QColor();

                else if (popuped)
                {
                    colorFill = buttonHighlightColor(opt->palette, true, false, w); //highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true, w); // highlightDn
                }
                else if (checked && !highlighted && !pressed)
                {
                    colorFill = buttonHighlightColor(opt->palette, true, false, w); // highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true, w); // highlightDn
                }
                else if (pressed)
                {
                    colorFill = buttonHighlightColor(opt->palette, true, false, w); // highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true, w); // highlightDn
                }
                else if (highlighted)
                {
                    colorFill = buttonHighlightColor(opt->palette, false, false, w); // highlight
                    colorBorder = buttonHighlightColor(opt->palette, false, true, w); // highlightDn
                }

                if (colorFill.isValid())
                {
                    QRect rc = opt->rect;
                    QtnBrush brush(painter, colorFill);
                    QtnPen pen(painter, colorBorder, 1);
                    qtn_drawRect(painter, rc);
                }
            }
            return true;
        }

        QRect rcButton = opt->rect;

        if (highlighted)
        {
            QtnPen pen(painter, buttonHighlightColor(opt->palette, pressed, true));
            qtn_drawRect(painter, rcButton);
        }

        if (mouseInSplit || checked)
        {
            QColor colorSplitFill;
            QColor colorBorder;
            if (!enabled)
            {
                colorSplitFill = QColor();
            }
            else if (checked)
            {
                if (popuped)
                {
                    colorSplitFill = buttonHighlightColor(opt->palette, true); // highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true); // highlightDn
                }
                else if (highlighted && !pressed && !checked)
                {
                    colorSplitFill = buttonHighlightColor(opt->palette, false); // highlight
                    colorBorder = buttonHighlightColor(opt->palette, false, true); // highlightDn
                }
                else if (highlighted && pressed)
                {
                    colorSplitFill = buttonHighlightColor(opt->palette, true); // highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true); // highlightDn
                }
                else if (pressed || checked)
                {
                    colorSplitFill = buttonHighlightColor(opt->palette, true); // highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true); // highlightDn
                }
                else if (highlighted || pressed)
                {
                    colorSplitFill = !mouseInSplit ? buttonHighlightColor(opt->palette, true) : QColor(); // highlightDn
                    colorBorder = !mouseInSplit ? buttonHighlightColor(opt->palette, true, true) : QColor(); // highlightDn
                }
            }
            else
            {
                if (popuped)
                {
                    colorSplitFill = buttonHighlightColor(opt->palette, true); // highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true); // highlightDn
                }
                else if (pressed)
                {
                    colorSplitFill = buttonHighlightColor(opt->palette, true); // highlightDn
                    colorBorder = buttonHighlightColor(opt->palette, true, true); // highlightDn
                }
                else if (highlighted)
                {
                    colorSplitFill = buttonHighlightColor(opt->palette, false); // highlight
                    colorBorder = buttonHighlightColor(opt->palette, false, true); // highlightDn
                }
            }
            if (colorSplitFill.isValid())
            {
                QRect buttonRectUp = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButton, w);
                QtnBrush brush(painter, colorSplitFill);
                QtnPen pen(painter, colorBorder);
                qtn_drawRect(painter, buttonRectUp);
            }
        }
        if ((mouseInSplitDropDown || popuped || checked) && enabled)
        {
            if (!popuped)
            {
                QRect popupRectDown = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, w);
                QtnBrush brush(painter, buttonHighlightColor(opt->palette, pressed));
                QtnPen pen(painter, buttonHighlightColor(opt->palette, pressed, true));
                qtn_drawRect(painter, popupRectDown);
            }
            else if (popuped || checked)
            {
                QtnBrush brush(painter, buttonHighlightColor(opt->palette, true));
                QtnPen pen(painter, buttonHighlightColor(opt->palette, true, true));
                qtn_drawRect(painter, rcButton);
            }
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonOptionButton(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    drawHighlightButton(option, painter, w);
    QStyleOption opt = *option;
    int space = CommonStylePrivate::dpiScaled(3.0, w);
    opt.rect = opt.rect.adjusted(space, space, -space, -space);
    drawIconDp(p.proxy()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_RibbonButtonGroupOption), option, w), &opt, painter, w);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonQuickAccessButton(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    drawHighlightButton(option, painter, w);
    QStyleOption opt = *option;
    int space = 0;//(CommonStylePrivate::dpiScaled(2.0, w));
    opt.rect = opt.rect.adjusted(space, space, -space, -space);
    drawIconDp(p.proxy()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_RibbonButtonPopupMenu), option, w), &opt, painter, w);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonBackstageFrame(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const RibbonBackstageStyleOption* optBackstage = qstyleoption_cast<const RibbonBackstageStyleOption*>(opt))
    {
        painter->fillRect(optBackstage->rect, darkBackground(optBackstage->palette));
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonBackstagePanelButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const BackstageButtonStyleOption* btnStyle = qstyleoption_cast<const BackstageButtonStyleOption*>(opt))
    {
        const bool enabled = opt->state & QStyle::State_Enabled;
        const bool checked = opt->state & QStyle::State_On;
        const bool highlighted = opt->state & QStyle::State_MouseOver;
        const bool pressed = opt->state & QStyle::State_Sunken;
        const bool popuped = (btnStyle->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

        QColor colorFill = opt->palette.button().color();//Qt::white;
        QColor colorBorder(outlineMidlight(btnStyle->palette).darker(110));
        if (!enabled)
        {
            colorFill = QColor();
            colorBorder = outlineLight(btnStyle->palette);
        }
        else if (popuped)
        {
            colorFill = buttonHighlightColor(btnStyle->palette, true/*highlightDn*/, false/*border*/);
            colorBorder = buttonHighlightColor(btnStyle->palette, true/*highlightDn*/, true/*border*/);
        }
        else if (checked && !highlighted && !pressed)
        {
            colorFill = buttonHighlightColor(btnStyle->palette, true/*highlightDn*/, false/*border*/);
            colorBorder = buttonHighlightColor(btnStyle->palette, true/*highlightDn*/, true/*border*/);
        }
        else if (pressed)
        {
            colorFill = buttonHighlightColor(btnStyle->palette, true/*highlightDn*/, true/*border*/);
            colorBorder = buttonHighlightColor(btnStyle->palette, true/*highlightDn*/, true/*border*/);
        }
        else if (highlighted)
        {
            colorFill = buttonHighlightColor(btnStyle->palette, false/*highlightDn*/, false/*border*/);
            colorBorder = buttonHighlightColor(btnStyle->palette, false/*highlightDn*/, true/*border*/);
        }

        if (colorFill.isValid())
            p->fillRect(opt->rect.adjusted(0, 0, btnStyle->tabStyle ? -2 : -1, -1), colorFill);

        if (!btnStyle->tabStyle)
        {
            const QPen savePen = p->pen();
            p->setPen(colorBorder);
            qtn_drawRect(p, btnStyle->rect);
            p->setPen(savePen);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonBackstageCloseButton(const QStyleOption* /*opt*/, QPainter* /*p*/, const QWidget* /*widget*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonGroupScrollButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        QTN_P(const CommonStyle);
        QRect rc = toolbutton->rect;
        const bool enabled = toolbutton->state & QStyle::State_Enabled;
        const bool highlighted = toolbutton->state & QStyle::State_MouseOver;
        const bool pressed = toolbutton->state & QStyle::State_Sunken;

        if (pressed && enabled)
            painter->fillRect(rc, opt->palette.window());
        else if ((pressed || highlighted) && enabled)
            painter->fillRect(rc, opt->palette.brush(QPalette::Highlight).color().lighter(210));
        else
            painter->fillRect(rc, opt->palette.window());

        QColor clrBorder = !enabled ? opt->palette.mid().color().lighter(130) : (highlighted || pressed) ?
            opt->palette.highlight().color().lighter(180) : opt->palette.mid().color().lighter(130);
        QPen oldPen = painter->pen();
        painter->setPen(clrBorder);
        qtn_drawRect(painter, rc);
        painter->setPen(oldPen);

        QStyleOption tool(0);
        tool.palette = opt->palette;
        tool.state = tool.state;
        if (enabled) tool.state |= QStyle::State_Enabled;
        const int indent = 3;
        int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, &tool, w) - indent;
        tool.rect = QRect(QPoint(rc.x() + (rc.width() - mbi) / 2, rc.y() + (rc.height() - mbi) / 2), QSize(mbi, mbi));
        
        QStyle::PrimitiveElement prim = QStyle::PE_IndicatorArrowLeft;
        if (toolbutton->arrowType == Qt::RightArrow)
            prim = QStyle::PE_IndicatorArrowRight;
        else if (toolbutton->arrowType == Qt::UpArrow)
            prim = QStyle::PE_IndicatorArrowUp;
        else if (toolbutton->arrowType == Qt::DownArrow)
            prim = QStyle::PE_IndicatorArrowDown;
        p.proxy()->drawPrimitive(prim, &tool, painter, w);
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonPopupBarButton(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawWorkspace(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    painter->fillRect(opt->rect, opt->palette.color(QPalette::Shadow).lighter(165));
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawPanelButtonBevel(const QStyleOption* /*opt*/, QPainter* /*painter*/, const QWidget* /*w*/) const
{
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonSliderButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    const bool left = opt->direction != Qt::RightToLeft;
    const bool highlighted = opt->state & QStyle::State_MouseOver;
    const bool pressed = opt->state & QStyle::State_Sunken;
    const bool enabled = opt->state & QStyle::State_Enabled;

    QPoint ptCenter = opt->rect.center();
    int radius = CommonStylePrivate::dpiScaled(7, w);
    QRect rectSign(QPoint(ptCenter.x() - radius / 2, ptCenter.y() - radius / 2), QSize(radius, radius));

    QColor clrLine;
    if (!enabled)
        clrLine = opt->palette.color(QPalette::ButtonText);
    else if (pressed && highlighted)
        clrLine = opt->palette.color(QPalette::ButtonText).darker(120);
    else if (highlighted)
        clrLine = opt->palette.color(QPalette::ButtonText).darker(120);
    else
        clrLine = opt->palette.color(QPalette::ButtonText);

    const QPen savePen = painter->pen();
    painter->setPen(clrLine);

    QPainterPath path;
    path.moveTo(rectSign.left(), ptCenter.y());
    path.lineTo(rectSign.right(), ptCenter.y());
    if (left)
    {
        path.moveTo(ptCenter.x(), rectSign.top());
        path.lineTo(ptCenter.x(), rectSign.bottom());
    }
    painter->drawPath(path);
    painter->setPen(savePen);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawKeyTip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QLinearGradient line(opt->rect.topLeft(), opt->rect.bottomLeft());
    line.setColorAt(0, opt->palette.color(QPalette::Base));
    line.setColorAt(1, opt->palette.color(QPalette::Midlight));
    painter->fillRect(opt->rect, QBrush(line));
    painter->setPen(opt->palette.color(QPalette::Dark));
    qtn_drawRect(painter, opt->rect);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonTabShapeLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (const RibbonOptionHeaderStyleOption* tabOpt = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        QTN_P(const CommonStyle);
        int flags = static_cast<int>(tabOpt->textAlignment);
        flags |= Qt::TextShowMnemonic;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;

        QString text = tabOpt->text;
        if (p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), opt, w))
            text = text.toUpper();

        // draw text
        p.proxy()->drawItemText(painter, tabOpt->rect, flags, opt->palette,
            opt->state & QStyle::State_Enabled, text, QPalette::WindowText);
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonTab(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const RibbonOptionHeaderStyleOption* tabOpt = qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(opt))
    {
        const QRect tabRect = tabOpt->rect;
        const bool isHighlight = tabOpt->state & QStyle::State_MouseOver;
        const bool isSelected = tabOpt->state & QStyle::State_Selected;

        if (isSelected || isHighlight)
        {
            painter->fillRect(tabOpt->rect, !isSelected && isHighlight ?
                tabOpt->palette.mid().color().lighter(158) : tabOpt->palette.button().color());

            QPainterPath path;
            if (tabOpt->tabBarPosition == 1)
            {
                path.moveTo(tabRect.left(), tabRect.bottom());
                path.lineTo(tabRect.left(), tabRect.top());
                path.lineTo(tabRect.right(), tabRect.top());
                path.lineTo(tabRect.right(), tabRect.bottom());
            }
            else if (tabOpt->tabBarPosition == 2)
            {
                path.moveTo(tabRect.left(), tabRect.top());
                path.lineTo(tabRect.left(), tabRect.bottom());
                path.lineTo(tabRect.right(), tabRect.bottom());
                path.lineTo(tabRect.right(), tabRect.top());
            }
            else
                return true;

            const QPen savePen = painter->pen();
            QColor color = CommonStyle::contextColorToColor(tabOpt->pageColor);
            painter->setPen(color.isValid() ? color : (isHighlight ? tabOpt->palette.midlight().color() : tabOpt->palette.midlight().color()));
            painter->drawPath(path);
            painter->setPen(savePen);
        }
        else if (tabOpt->state & QStyle::State_Small)
        {
            QPainterPath path;
            path.moveTo(tabRect.right(), tabRect.top());
            path.lineTo(tabRect.right(), tabRect.bottom());
            const QPen savePen = painter->pen();
            painter->setPen(tabOpt->palette.dark().color().lighter(135));
            painter->drawPath(path);
            painter->setPen(savePen);
        }
        else
        {
            if (tabOpt->pageColor != Qtitan::ContextColorNone && tabOpt->firstTab)
            {
                QRect rcSeparator(QPoint(tabRect.left() - 1, tabRect.top()), QPoint(tabRect.left() + 1, tabRect.bottom() - 2));
                QLinearGradient gradient(rcSeparator.topLeft(), rcSeparator.bottomLeft());
                gradient.setColorAt(0, tabOpt->palette.dark().color().lighter(110));
                gradient.setColorAt(1, Qt::white);

                painter->setBrush(gradient);
                painter->setPen(Qt::transparent);
                qtn_drawRect(painter, rcSeparator); //TODO draw rect transparent!!!
            }

            if (tabOpt->pageColor != Qtitan::ContextColorNone && tabOpt->lastTab)
            {
                QRect rcSeparator(QPoint(tabRect.right(), tabRect.top()), QPoint(tabRect.right() + 1, tabRect.bottom() - 2));

                QLinearGradient gradient(rcSeparator.topLeft(), rcSeparator.bottomLeft());
                gradient.setColorAt(0, tabOpt->palette.dark().color().lighter(110));
                gradient.setColorAt(1, Qt::white);

                painter->setBrush(gradient);
                painter->setPen(Qt::transparent);
                qtn_drawRect(painter, rcSeparator); //TODO draw rect transparent!!!
            }
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonGroupLabel(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const RibbonGroupStyleOption* optGroupBox = qstyleoption_cast<const RibbonGroupStyleOption*>(opt))
    {
        int flags = static_cast<int>(optGroupBox->textAlignment);
        flags |= Qt::TextHideMnemonic;

        QRect rcEntryCaption = optGroupBox->rect;
        rcEntryCaption.setTop(rcEntryCaption.bottom() - optGroupBox->heightCaption);
        rcEntryCaption.adjust(-2, 0, -optGroupBox->widthOptButton, 0);
        // draw text
        QString text = optGroupBox->fontMetrics.elidedText(optGroupBox->text, optGroupBox->textElideMode, rcEntryCaption.width());
        if (!text.isEmpty())
        {
            const QPen savePen = painter->pen();
            painter->setPen(optGroupBox->palette.color(QPalette::WindowText).lighter());
            painter->drawText(rcEntryCaption, flags, text);
            painter->setPen(savePen);
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonSysMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const StyleOptionMenuScrollItem* menuitem = qstyleoption_cast<const StyleOptionMenuScrollItem *>(opt))
    {
        QString textItem = menuitem->text;
        if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup")))
            textItem = textItem + QLatin1Char('\n') + menuitem->toolTip;

        int x, y, w, h;
        menuitem->rect.getRect(&x, &y, &w, &h);
        int tab = menuitem->tabWidth;
        const bool dis = !(menuitem->state & QStyle::State_Enabled);
        const bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable ? menuitem->checked : false;
        const bool selected = menuitem->state & QStyle::State_Selected;

        const bool isSystemMenu = widget && (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")) || widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup")));
        // windows always has a check column, regardless whether we have an icon or not
        int iconSize = p.proxy()->pixelMetric(isSystemMenu ? QStyle::PM_ToolBarIconSize : QStyle::PM_SmallIconSize, opt, widget);
        int checkcol = isSystemMenu ? iconSize : qMax<int>(menuitem->maxIconWidth, windowsCheckMarkWidth);

        QBrush fill;
        if (selected)
        {
            painter->setPen(buttonHighlightColor(menuitem->palette, false/*highlightDn*/, true/*border*/));
            fill = buttonHighlightColor(menuitem->palette, false/*highlightDn*/, false/*border*/);
            painter->fillRect(menuitem->rect, fill);
            QRect rect = qtn_adjustRect(painter->pen(), menuitem->rect);
            painter->drawRect(rect);
            if (textItem.count(QStringLiteral(_qtn_SplitActionPopup)) != 0)
            {
                int dim = (h - 2 * windowsItemFrame) / 2;
                int xpos = x + w - windowsArrowHMargin - windowsItemFrame - dim;
                painter->drawLine(rect.left() + xpos, rect.top(), rect.left() + xpos, rect.bottom());
            }
        }
        else
        {
            fill = Qt::transparent;
            painter->fillRect(menuitem->rect.adjusted(0, 0, -1, 0), fill);
        }

        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            int yoff = y - 1 + h / 2;
            int xIcon = iconSize;
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu")))
                xIcon = 0;
            painter->setPen(opt->palette.brush(QPalette::Midlight).color().darker(105));
            painter->drawLine(x + 2 + xIcon, yoff, x + w - 4, yoff);
            painter->setPen(opt->palette.brush(QPalette::Light).color());
            painter->drawLine(x + 2 + xIcon, yoff + 1, x + w - 4, yoff + 1);
            return true;
        }

        if (isSystemMenu)
        {
            iconSize = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > opt->rect.height())
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);
            checkcol = iconSize + 6;
        }

        // On Windows Style, if we have a checkable item and an icon we
        // draw the icon recessed to indicate an item is checked. If we
        // have no icon, we draw a checkmark instead.
        if (!menuitem->icon.isNull())
        {
            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            if (selected && !dis)
                mode = QIcon::Active;
                
            int iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);
            QPixmap pixmap = preparePixmap(menuitem->icon, QSize(iconSize, iconSize), mode, checked ? QIcon::On : QIcon::Off, widget);

            if (isSystemMenu)
            {
                QRect vCheckRect = p.visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));
                QRect pmr(QPoint(0, 0), QSize(iconSize, iconSize));
                pmr.moveCenter(vCheckRect.center());
                painter->drawPixmap(pmr.topLeft(), pixmap);
            }
            else
            {
                QRect pmr(0, 0, pixmap.width(), pixmap.height());
                QRect vCheckRect = p.visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));
                pmr.moveCenter(vCheckRect.center());
                painter->setPen(menuitem->palette.text().color());
                painter->drawPixmap(pmr.topLeft(), pixmap);
            }
        }

        QPalette pal = opt->palette;
        painter->setPen(pal.color(QPalette::WindowText));

        QColor discol;
        if (dis)
        {
            discol = pal.color(QPalette::Disabled, QPalette::WindowText);
            painter->setPen(discol);
        }

        int xm = int(windowsItemFrame) + checkcol + int(windowsItemHMargin);
        int xpos = menuitem->rect.x() + xm;

        QStringList split = textItem.split(QStringLiteral("\n"));
        QString s = split.count() > 0 ? split.at(0) : textItem;
        if (split.count() > 1)
        {
            int height = menuitem->fontMetrics.height();
            QRect textRect(xpos, y + windowsItemVMargin + 2, w - xm - windowsRightBorder - tab + 1, height);
            QRect vTextRect = p.visualRect(opt->direction, menuitem->rect, textRect);

            QFont font = menuitem->font;
            QFont saveFont = font;
            font.setBold(true);
            painter->setFont(font);

            int t = s.indexOf(QLatin1Char('\t'));
            int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, menuitem, widget))
                text_flags |= Qt::TextHideMnemonic;
            text_flags |= Qt::AlignLeft;

            // draw hotkeys
            if (t >= 0)
            {
                QRect vShortcutRect = p.visualRect(opt->direction, menuitem->rect, QRect(textRect.topRight(),
                    QPoint(menuitem->rect.right(), textRect.bottom())));

                painter->setPen(opt->state & QStyle::State_Enabled ? pal.color(QPalette::Normal, QPalette::WindowText) : discol);
                painter->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                s = s.left(t);
            }

            painter->setPen(opt->state & QStyle::State_Enabled ? pal.color(QPalette::Normal, QPalette::WindowText) : discol);
            painter->drawText(vTextRect, text_flags, s.left(t));
            painter->setFont(saveFont);

            if (split.count() > 1)
            {
                text_flags |= Qt::TextWordWrap;
                QRect textTipRect(xpos, y + vTextRect.height(), w - xm - 1, opt->rect.height());
                QRect rc = menuitem->fontMetrics.boundingRect(textTipRect, text_flags, split.at(1));
                QRect vTextTipRect = p.visualRect(opt->direction, menuitem->rect, rc);
                vTextTipRect.setTop(y + vTextRect.height());
                vTextTipRect.setWidth(textTipRect.width());
                painter->setPen(opt->state & QStyle::State_Enabled ? pal.color(QPalette::Normal, QPalette::WindowText) : discol);
                painter->drawText(vTextTipRect, text_flags, split.at(1));
            }
        }
        else
        {
            QRect textRect(xpos, y + windowsItemVMargin,
                w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
            QRect vTextRect = p.visualRect(opt->direction, menuitem->rect, textRect);

            QString s(textItem);
            s = s.remove(QStringLiteral(_qtn_SplitActionPopup));
            // draw text
            if (!s.isEmpty())
            {
                int t = s.indexOf(QLatin1Char('\t'));
                int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
                if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, menuitem, widget))
                    text_flags |= Qt::TextHideMnemonic;
                text_flags |= Qt::AlignLeft;

                if (t >= 0)
                {
                    QRect vShortcutRect = p.visualRect(opt->direction, menuitem->rect, QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
                    const QString textToDraw = s.mid(t + 1);
                    if (dis && !selected && p.proxy()->styleHint(QStyle::SH_EtchDisabledText, opt, widget))
                    {
                        painter->setPen(menuitem->palette.light().color());
                        painter->drawText(vShortcutRect.adjusted(1, 1, 1, 1), text_flags, textToDraw);
                        painter->setPen(discol);
                    }
                    painter->drawText(vShortcutRect, text_flags, textToDraw);
                    s = s.left(t);
                }

                QFont saveFont = menuitem->font;
                QFont font = saveFont;
                if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                    font.setBold(true);
                painter->setFont(font);

                const QString textToDraw = s.left(t);
                if (dis && !selected && p.proxy()->styleHint(QStyle::SH_EtchDisabledText, opt, widget))
                {
                    painter->setPen(menuitem->palette.light().color());
                    painter->drawText(vTextRect.adjusted(1, 1, 1, 1), text_flags, textToDraw);
                    painter->setPen(discol);
                }
                painter->drawText(vTextRect, text_flags, textToDraw);
                painter->setFont(saveFont);
            }
        }
        // draw sub menu arrow
        if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            int dim = (h - 2 * windowsItemFrame) / 3;
            QStyle::PrimitiveElement arrow = (opt->direction == Qt::RightToLeft) ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight;
            xpos = x + w - windowsArrowHMargin - windowsItemFrame - dim;
            QRect  subMenuRect = p.visualRect(opt->direction, menuitem->rect, QRect(xpos, y + h / 2 - dim / 2, dim, dim));
            QStyleOptionMenuItem newMI = *menuitem;
            newMI.rect = subMenuRect;
            newMI.state = dis ? QStyle::State_None : QStyle::State_Enabled;
            if (selected)
                QPalette_setColor(newMI.palette, QPalette::ButtonText, newMI.palette.highlightedText().color());
            p.proxy()->drawPrimitive(arrow, &newMI, painter, widget);
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonSysRecentFileItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    Q_UNUSED(widget)
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
    {
        const bool selected = menuitem->state & QStyle::State_Selected;
        if (selected)
        {
            painter->setPen(buttonHighlightColor(menuitem->palette, false/*highlightDn*/, true/*border*/));
            QColor fill = buttonHighlightColor(menuitem->palette, false/*highlightDn*/, false/*border*/);
            painter->fillRect(menuitem->rect, fill);
            qtn_drawRect(painter, menuitem->rect);
        }

        QRect rect = menuitem->rect;
        int alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine | Qt::TextShowMnemonic;
        rect.translate(7, 0); // move

        QPalette::ColorRole textRole = selected ? QPalette::HighlightedText : QPalette::WindowText;
        QString text = menuitem->fontMetrics.elidedText(menuitem->text, Qt::ElideRight, rect.adjusted(2, 0, -2, 0).width());
        p.proxy()->drawItemText(painter, rect, alignment, opt->palette, true, text, textRole);
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        const bool enabled = toolbutton->state & QStyle::State_Enabled;
        painter->setFont(toolbutton->font);
        QRect rect = toolbutton->rect;
        // Arrow type always overrules and is always shown
        const bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;

        QString text = toolbutton->text;
        QPalette::ColorRole textRole = QPalette::ButtonText;
        const bool isAncestorTabBar = QtnPrivate::qtn_isAncestor(w, QTITAN_META_CLASSNAME("RibbonTabBar"));
        if (isAncestorTabBar && p.proxy()->styleHint(static_cast<QStyle::StyleHint>(CommonStyle::SH_RibbonItemUpperCase), opt, w))
            text = text.toUpper();

        QPalette palText = toolbutton->palette;
        if (isAncestorTabBar && isQtitanStyle())
        {
            QColor clrText = brushSubControl(QStringLiteral("RibbonTabBar"), QStringLiteral("RibbonButton"),
                enabled ? QPalette::Active : QPalette::Disabled, QPalette::ButtonText).color();
            QPalette_setBrush(palText, QPalette::ButtonText, clrText);
            clrText = brushSubControl(QStringLiteral("RibbonTabBar"), QStringLiteral("RibbonButton"),
                QPalette::Active, QPalette::HighlightedText).color();
            QPalette_setBrush(palText, QPalette::HighlightedText, clrText);
        }

        if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly)
        {
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
                alignment |= Qt::TextHideMnemonic;

            if ((opt->state & QStyle::State_MouseOver || opt->state & QStyle::State_Sunken) && enabled)
                textRole = QPalette::HighlightedText;

            QRect rectText = rect;
            if ((toolbutton->subControls & QStyle::SC_ToolButtonMenu) || (toolbutton->features & QStyleOptionToolButton::HasMenu))
            {
                int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, opt, w);
                rectText.adjust(0, 0, -mbi, 0);
            }
            p.proxy()->drawItemText(painter, rectText, alignment, palText, enabled, text, textRole);
        }
        else
        {
            QPixmap pm;
            if (!toolbutton->icon.isNull())
            {
                QIcon::State state = toolbutton->state & QStyle::State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!enabled)
                    mode = QIcon::Disabled;
                else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;
                pm = preparePixmap(toolbutton->icon, toolbutton->rect.size().boundedTo(toolbutton->iconSize), mode, state, w);
            }

            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
            {
                painter->setFont(toolbutton->font);
                QRect pr = rect, tr = rect;
                QRect popupr = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, w);

                int alignment = Qt::TextShowMnemonic;
                if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
                    alignment |= Qt::TextHideMnemonic;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    pr.setBottom(pr.bottom() - popupr.height());
                    if (!hasArrow)
                        p.proxy()->drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                    else
                        QtnPrivate::drawArrow(&p, toolbutton, pr, painter, w);
                    alignment |= Qt::AlignHCenter | Qt::AlignTop;
                }
                else
                {
                    pr.setWidth(qMax(toolbutton->iconSize.width(), pr.height()));
                    tr.adjust(pr.width(), 0, 0, 0);
                    if (!hasArrow)
                        p.proxy()->drawItemPixmap(painter, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                    else
                        QtnPrivate::drawArrow(&p, toolbutton, pr, painter, w);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    if (w && w->property("wordWrap").toBool())
                        alignment |= Qt::TextWordWrap;

                    const int space = 1;
                    tr.setTop(tr.bottom() - popupr.height());
                    QRect rcText(QStyle::visualRect(opt->direction, rect, tr));
                    rcText.adjust(space, 0, -space, 0);
                    p.proxy()->drawItemText(painter, rcText, alignment, palText, enabled, text, textRole);
                }
                else
                    p.proxy()->drawItemText(painter, QStyle::visualRect(opt->direction, rect, tr), alignment, palText, enabled, text, textRole);
            }
            else
            {
                if (hasArrow)
                {
                    QtnPrivate::drawArrow(&p, toolbutton, rect, painter, w);
                }
                else
                {
                    QRect pr = rect;
                    const int sz = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, w);
                    const bool largeIconOnly = (sz == toolbutton->iconSize.width() && toolbutton->toolButtonStyle == Qt::ToolButtonIconOnly);
                    if (largeIconOnly)
                    {
                        QRect popupr = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, w);
                        pr.setBottom(pr.bottom() - popupr.height());
                    }
                    else if ((toolbutton->subControls & QStyle::SC_ToolButtonMenu) || (toolbutton->features & QStyleOptionToolButton::HasMenu))
                    {
                        int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, opt, w);
                        pr.setWidth(pr.width() - mbi);
                    }
                    p.proxy()->drawItemPixmap(painter, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                }
            }
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonGalleryItem(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    if (const RibbonGalleryItemStyleOption* optItem = qstyleoption_cast<const RibbonGalleryItemStyleOption*>(opt))
    {
        bool enabled = opt->state & QStyle::State_Enabled;
        QRect rectItem = optItem->rectItem;
        if (optItem->separator)
        {
            painter->fillRect(rectItem, darkBackground(optItem->palette));
            painter->fillRect(rectItem.left(), rectItem.bottom() - 1, rectItem.width(), 1, outlineMidlight(optItem->palette));

            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter;

            QPalette palette = optItem->palette;
            QColor colText = textGalleryItem(optItem->palette);
            QPalette_setColor(palette, QPalette::WindowText, colText);

            rectItem.adjust(10, 0, 0, 0);
            const QFont& saveFont = painter->font();
            QFont font(saveFont);
            font.setBold(true);
            painter->setFont(font);
            p.proxy()->drawItemText(painter, rectItem, alignment, palette, true, optItem->caption, QPalette::WindowText);
            painter->setFont(saveFont);
            return true;
        }
        if (!optItem->icon.isNull())
        {
            QStyleOption optTmp = *optItem;
            optTmp.rect = optItem->rectItem;
            drawHighlightItemGallery(&optTmp, painter, w);

            QList<QSize> lstSz = optItem->icon.availableSizes(enabled ? QIcon::Normal : QIcon::Disabled);
            QSize iconSize = lstSz.size() > 0 ? lstSz[0] : rectItem.size();

            iconSize = QSize(CommonStylePrivate::dpiScaled(iconSize.width(), w),
                             CommonStylePrivate::dpiScaled(iconSize.height(), w));
            QPixmap pm = preparePixmap(optItem->icon, iconSize, enabled ? QIcon::Normal : QIcon::Disabled, QIcon::Off, w);
            p.proxy()->drawItemPixmap(painter, rectItem, Qt::AlignCenter, pm);
        }
        else
        {
            QStyleOption optTmp = *optItem;
            optTmp.rect = optItem->rectItem;
            drawHighlightButton(&optTmp, painter, w);

            QRect rcText(rectItem);
            rcText.setLeft(rcText.left() + 3);
            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter | Qt::TextHideMnemonic;
            QPalette palette;
            p.proxy()->drawItemText(painter, rcText, alignment, palette, enabled, optItem->caption, QPalette::WindowText);
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonBackstageMenuItem(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        const bool act = menuitem->state & QStyle::State_Selected;
        const bool focus = menuitem->state & QStyle::State_HasFocus;
        const bool dis = !(menuitem->state & QStyle::State_Enabled);

        QRect rcText(opt->rect);
        rcText.adjust(15, 0, 0, 0);
        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = p.proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
            int yoff = (y - 1 + h / 2);
            painter->setPen(opt->palette.brush(QPalette::Midlight).color().darker(105));
            painter->drawLine(x + 2 + iconSize, yoff, x + w - 4, yoff);
            painter->setPen(opt->palette.brush(QPalette::Light).color());
            painter->drawLine(x + 2 + iconSize, yoff + 1, x + w - 4, yoff + 1);
            return true;
        }

        if (!dis && (act || focus))
        {
            painter->setPen(buttonHighlightColor(opt->palette, focus/*highlightDn*/, true, widget));
            QBrush fill = buttonHighlightColor(opt->palette, focus/*highlightDn*/, false, widget);
            painter->fillRect(opt->rect.adjusted(1, 0, -2, 0), fill);
            painter->drawRect(opt->rect.adjusted(1, 0, -2, -1));
        }

        int iconWidth = 0;
        int indent = menuitem->menuItemType == QStyleOptionMenuItem::SubMenu ? 6 : 0;
        if (!menuitem->icon.isNull())
        {
            int iconSize = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                iconSize = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);

            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            QPixmap pixmap = preparePixmap(menuitem->icon, QSize(iconSize, iconSize), mode, QIcon::Off, widget);
            QPoint pmr(rcText.left() + indent, (rcText.top() + rcText.bottom() - iconSize) / 2);
            painter->drawPixmap(pmr, pixmap);
            iconWidth = iconSize;
        }

        indent += !menuitem->icon.isNull() ? 7 : 0;
        rcText.setLeft(rcText.left() + iconWidth + indent);
        rcText.setHeight(qMax(rcText.height(), iconWidth));

        if (!menuitem->text.isEmpty())
        {
            QPalette pal = opt->palette;
            int flags = 0;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;
            int index = menuitem->text.indexOf(QStringLiteral("&"));
            if (index != -1)
            {
                index = menuitem->text.indexOf(QStringLiteral(" "), index);
                if (index != -1)
                    flags &= ~Qt::TextHideMnemonic;
            }
            // draw text
            p.proxy()->drawItemText(painter, rcText, flags, pal, opt->state & QStyle::State_Enabled, menuitem->text, QPalette::WindowText);
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonBackstageButtonLabel(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    if (const BackstageButtonStyleOption* toolbutton = qstyleoption_cast<const BackstageButtonStyleOption*>(opt))
    {
        QTN_P(const CommonStyle);
        QRect rect = toolbutton->rect;
        // Arrow type always overrules and is always shown
        bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly)
        {
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
                alignment |= Qt::TextHideMnemonic;
        }
        else
        {
            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (pmSize.width() > qMin(rect.width(), rect.height()))
            {
                const int iconExtent = p.proxy()->pixelMetric(QStyle::PM_SmallIconSize);
                pmSize = QSize(iconExtent, iconExtent);
            }

            if (!toolbutton->icon.isNull())
            {
                QIcon::State state = toolbutton->state & QStyle::State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(toolbutton->state & QStyle::State_Enabled))
                    mode = QIcon::Disabled;
                else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;
                pm = preparePixmap(toolbutton->icon, toolbutton->rect.size().boundedTo(pmSize), mode, state, w);
                pmSize = pm.size();
            }
            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
            {
                int alignment = Qt::TextShowMnemonic;
                if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
                    alignment |= Qt::TextHideMnemonic;

                painter->setFont(toolbutton->font);
                QRect pr = rect;
                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    alignment |= Qt::AlignHCenter | Qt::TextWordWrap;
                    pr.moveTop((toolbutton->fontMetrics.height() / 2 + 2) / 2);
                    if (!hasArrow)
                        p.drawItemPixmap(painter, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignTop | Qt::AlignHCenter, pm);
                }
                else
                {
                    QRect pmRect(QPoint(4, (rect.height() - pmSize.height()) / 2), pmSize);
                    if (!hasArrow)
                        p.QStyle::drawItemPixmap(painter, QStyle::visualRect(opt->direction, rect, pmRect), Qt::AlignCenter, pm);

                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;

                    QSize sizeText = toolbutton->fontMetrics.size(Qt::TextSingleLine, toolbutton->text);
                    QRect tcText(QPoint(pmRect.right() + 4, (rect.height() - sizeText.height()) / 2), sizeText);
                    p.proxy()->drawItemText(painter, tcText, alignment, toolbutton->palette, /*toolbutton->state & State_Enabled*/true, toolbutton->text, QPalette::ButtonText);
                }
            }
            else
            {
                QRect pr = rect;
                if ((toolbutton->subControls & QStyle::SC_ToolButtonMenu) || (toolbutton->features & QStyleOptionToolButton::HasMenu))
                {
                    int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, opt, w);
                    pr.setWidth(pr.width() - mbi);
                }
                p.QStyle::drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonReducedGroup(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    if (const RibbonGroupStyleOption* optGroup = qstyleoption_cast<const RibbonGroupStyleOption*>(opt))
    {
        QRect rcEntry(opt->rect);
        QStyleOptionToolButton optButton;
        optButton.QStyleOption::operator = (*opt);
        optButton.rect = opt->rect.adjusted(0, 0, -2, 0);
        optButton.features = optGroup->features;
        optButton.iconSize = optGroup->iconSize;
        optButton.icon = optGroup->icon;
        optButton.subControls = optGroup->subControls;
        p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &optButton, painter, w);
        // ----
        QFontMetrics fm(optGroup->fontMetrics);
        int flags = Qt::TextSingleLine;

        flags |= Qt::TextShowMnemonic;
        if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, optGroup, w))
            flags |= Qt::TextHideMnemonic;

        if (!optGroup->icon.isNull())
        {
            QPixmap pm;
            QIcon::State stateIcon = optGroup->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(optGroup->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                mode = QIcon::Active;
            else
                mode = QIcon::Normal;
            pm = preparePixmap(optGroup->icon, optGroup->rect.size().boundedTo(optGroup->iconSize), mode, stateIcon, w);

            int indent = CommonStylePrivate::dpiScaled(6, w);
            QRect rectImage(QPoint(rcEntry.center().x() - pm.width() / 2, rcEntry.top() + indent * 2), pm.size());

            rectImage.adjust(-indent, -indent, indent, indent);
            QColor color = opt->palette.light().color(); color.setAlpha(50);
            painter->fillRect(rectImage, color);
            const QPen savePen = painter->pen();
            painter->setPen(opt->palette.brush(QPalette::Highlight).color().lighter(185));
            painter->drawRect(rectImage);
            painter->setPen(savePen);
            p.drawItemPixmap(painter, rectImage, Qt::AlignCenter, pm);
        }
        //===========================================================================================
        QSize sz = optGroup->fontMetrics.boundingRect(optGroup->rect, flags | Qt::AlignVCenter | Qt::TextWordWrap, optGroup->text).size();
        bool twoLines = sz.height() / optGroup->fontMetrics.height() == 2;

        QRect rcText = rcEntry;
        const int textHeightTotal = optGroup->rect.height() / 2;
        rcText.setHeight(sz.height());
        rcText.moveTo(0, textHeightTotal);

        QString strText = optGroup->text;
        const int menuArrowWidth = twoLines ? p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, optGroup, w) : 0;
        if (menuArrowWidth > 0)
            strText += QStringLiteral("   ");

        p.proxy()->drawItemText(painter, rcText, flags | Qt::TextWordWrap | Qt::AlignHCenter, optGroup->palette,
            optGroup->state & QStyle::State_Enabled, strText, QPalette::WindowText);
        //===========================================================================================

        QStyleOption tool(0);
        tool.palette = opt->palette;
        tool.state |= QStyle::State_Enabled;

        const int indent = 5;
        int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, &tool, w) - indent;

        QPoint pt;
        if (twoLines)
        {
            QRect r = rcText;
            r.setLeft(rcText.left() + (rcText.width() - sz.width()) / 2);
            r.setWidth(sz.width());
            pt = QPoint(r.right(), (r.bottom() - (optGroup->fontMetrics.height() / 2 + mbi / 2)) + optGroup->fontMetrics.descent() - 1);
        }
        else
            pt = QPoint(rcText.center().x() - mbi / 2, (rcText.top() + optGroup->fontMetrics.height() + optGroup->fontMetrics.descent() + 1));
        tool.rect = QRect(pt, QSize(mbi, mbi));
        p.proxy()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, painter, w);
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawOfficePopupSizeGrip(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QStyleOptionSizeGrip* sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip*>(opt))
    {
        painter->fillRect(sizeGrip->rect, opt->palette.button());
        QPoint pt(sizeGrip->rect.right() - 3, sizeGrip->rect.bottom() - 3);
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 3 - y; x++)
                painter->fillRect(QRect(QPoint(pt.x() + 0 - x * 4, pt.y() + 0 - y * 4), QSize(2, 2)), opt->palette.midlight());
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawNavigationBackButton(const QStyleOption* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    QStyleOption temp = *opt;
    QPalette_setColor(temp.palette, QPalette::Button, Qt::transparent);
    QPalette_setColor(temp.palette, QPalette::Highlight, opt->palette.color(QPalette::Highlight).darker(120));

    const bool isHighlight = opt->state & QStyle::State_MouseOver;
    QBrush fill = isHighlight ? temp.palette.highlight() : temp.palette.button();
    painter->fillRect(opt->rect, fill);

    int margin = static_cast<int>(temp.rect.height() / marginRatio);
    temp.rect.adjust(margin, margin, -margin, -margin);
    QPixmap px = p.proxy()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_NavigationBackButton), &temp, w);
    p.proxy()->drawItemPixmap(painter, opt->rect, Qt::AlignCenter, px);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonFileButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const SystemToolButtonStyleOption* optButton = qstyleoption_cast<const SystemToolButtonStyleOption*>(opt))
    {
        const bool popuped = (optButton->activeSubControls & QStyle::SC_ToolButtonMenu) && (optButton->state & QStyle::State_Sunken);
        const bool isHighlighted = optButton->state & QStyle::State_MouseOver;

        bool usePalette = false;
        uint resolve_mask = QPalette_resolveMask(optButton->palette);
        if (resolve_mask & (1 << QPalette::Button))
            usePalette = true;

        QColor fillColor = usePalette ? optButton->palette.color(QPalette::Button) : clrFileButton;

        if (isHighlighted && !popuped)
            fillColor = fillColor.lighter(125);
        else if (isHighlighted && popuped)
            fillColor = fillColor.lighter(70);

        painter->fillRect(optButton->rect, fillColor);

        if (optButton->toolButtonStyle != Qt::ToolButtonIconOnly)
        {
            int alignment = Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, optButton, widget))
                alignment |= Qt::TextHideMnemonic;
            alignment |= Qt::AlignCenter;
            alignment |= Qt::TextSingleLine;
            QPalette::ColorRole role = widget == Q_NULL ? QPalette::ButtonText : widget->foregroundRole();
            p.proxy()->drawItemText(painter, optButton->rect, alignment, optButton->palette,
                optButton->state & QStyle::State_Enabled, optButton->text, role);
        }
        else if (optButton->toolButtonStyle == Qt::ToolButtonIconOnly && !optButton->icon.isNull())
        {
            bool enabled = optButton->state & QStyle::State_Enabled;
            QRect rc = optButton->rect;
            QPixmap pxIcon = optButton->icon.pixmap(optButton->iconSize,
                enabled ? QIcon::Normal : QIcon::Disabled, isHighlighted ? QIcon::On : QIcon::Off);
            QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
            painter->drawPixmap(ptIcon, pxIcon);
        }

        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        QRect button, menuarea;
        button = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButton, widget);
        menuarea = p.proxy()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, widget);

        QStyle::State flags = toolbutton->state & ~QStyle::State_Sunken;

        if (flags & QStyle::State_AutoRaise)
        {
            if (!(flags & QStyle::State_MouseOver) || !(flags & QStyle::State_Enabled))
                flags &= ~QStyle::State_Raised;
        }
        QStyle::State mflags = flags;
        if (toolbutton->state & QStyle::State_Sunken)
        {
            if (toolbutton->activeSubControls & QStyle::SC_ToolButton)
                flags |= QStyle::State_Sunken;
            mflags |= QStyle::State_Sunken;
        }

        p.proxy()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(CommonStyle::PE_RibbonPanelButton), toolbutton, painter, widget);

        QStyleOptionToolButton label = *toolbutton;
        label.state = flags;

        p.proxy()->drawControl(static_cast<QStyle::ControlElement>(CommonStyle::CE_RibbonButtonLabel), &label, painter, widget);

        if (toolbutton->subControls & QStyle::SC_ToolButtonMenu || toolbutton->features & QStyleOptionToolButton::HasMenu)
        {
            QStyleOption tool(0);
            tool.palette = toolbutton->palette;
            QRect ir = menuarea, rcArrow;
            const int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget) - dpiScaled(4.0, widget);
            if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
            {
                int heightFont = 0;
                if (canWordWrap(toolbutton->rect, toolbutton->fontMetrics, toolbutton->text, &heightFont))
                    rcArrow = QRect(QPoint(ir.right() - mbi - 1, ir.top() + heightFont + (heightFont - mbi) / 2), QSize(mbi, mbi));
                else
                    rcArrow = QRect(QPoint((ir.width() - mbi) / 2 , ir.top() + heightFont + (heightFont - mbi) / 2), QSize(mbi, mbi));
            }
            else
                rcArrow = QRect(QPoint((ir.left() + (ir.width() - mbi) / 2) - 1, ir.y() + (ir.height() - mbi) / 2), QSize(mbi, mbi));

            tool.rect = rcArrow;
            tool.state = mflags;
            p.proxy()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, painter, widget);
        }
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonTitleBarWidget(const QStyleOptionComplex* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    QTN_P(const CommonStyle);
    if (const TitleBarStyleOption* styleOption = qstyleoption_cast<const TitleBarStyleOption*>(option))
    {
        for (QVector<TitleBarStyleOption::ContextualTabInfo>::const_iterator it = styleOption->contextualTabs.constBegin();
            it != styleOption->contextualTabs.constEnd(); ++it)
        {
            QRect rc = it->rect;
            QColor color = CommonStyle::contextColorToColor(it->color);
            painter->fillRect(rc, color);

            QPalette pal = option->palette;
            painter->setPen(pal.color(QPalette::Dark).darker(240));
            painter->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine, painter->fontMetrics().elidedText(it->strTitle, Qt::ElideRight, rc.width()));
        }
        painter->setFont(styleOption->fontTextTitle);
        QString text = QFontMetrics(styleOption->fontTextTitle).elidedText(styleOption->text, Qt::ElideRight, styleOption->rcTitleText.width());
        p.proxy()->drawItemText(painter, styleOption->rcTitleText, styleOption->alignment, styleOption->palette, true/*enabled*/, text, QPalette::WindowText);
    }
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawRibbonGalleryButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (widget == Q_NULL)
        return false;

    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        QPixmap px;
        if (widget->property(_qtn_PopupButtonGallery).toBool())
            px = p.proxy()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_RibbonButtonPopupMenu), opt, widget);
        else if (widget->property(_qtn_ScrollUpButtonGallery).toBool())
            px = p.proxy()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_RibbonShadedArrowUp), opt, widget);
        else if (widget->property(_qtn_ScrollDownButtonGallery).toBool())
            px = p.proxy()->standardPixmap(static_cast<QStyle::StandardPixmap>(CommonStyle::SP_RibbonShadedArrowDown), opt, widget);

        if (!px.isNull())
        {
            painter->fillRect(opt->rect, darkBackground(toolbutton->palette));
            painter->setPen(outlineMidlight(toolbutton->palette));
            qtn_drawRect(painter, opt->rect);
            drawHighlightButton(opt, painter, widget);

            QStyleOption option = *opt;
            const int indent = isQtitanStyle() ? dpiScaled(3.0, widget) : dpiScaled(2.0, widget);
            const int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget) - indent;
            option.rect = QRect(QPoint((opt->rect.left() + (opt->rect.width() - mbi) / 2), opt->rect.top() + (opt->rect.height() - mbi) / 2), QSize(mbi, mbi));
            drawIconDp(px, &option, painter, widget);
            return true;
        }
    }
    return false;
}

static void qtn_QTextDocument_draw(QPainter& painter, QTextDocument* document, const QPalette& palette, const QRect& rect)
{
    painter.save();
    QPalette pal = palette;
    QAbstractTextDocumentLayout::PaintContext ctx;
    painter.translate(rect.left(), rect.top());
    ctx.clip = QRect(0, 0, rect.width(), rect.height());
    painter.setClipRect(ctx.clip, Qt::IntersectClip);
    ctx.palette = pal;
    document->documentLayout()->draw(&painter, ctx);
    painter.restore();
}

/*! \internal */
bool CommonStylePrivate::drawRibbonBackstageButton(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    if (const BackstageButtonStyleOption* toolbutton = qstyleoption_cast<const BackstageButtonStyleOption*>(opt))
    {
        QTN_P(const CommonStyle);
        QSize iconSize = toolbutton->iconSize;
        QStyle::State flags = toolbutton->state & ~QStyle::State_Sunken;

        if (flags & QStyle::State_AutoRaise)
        {
            if (!(flags & QStyle::State_MouseOver) || !(flags & QStyle::State_Enabled))
                flags &= ~QStyle::State_Raised;
        }
        QStyle::State mflags = flags;
        if (toolbutton->state & QStyle::State_Sunken)
        {
            if (toolbutton->activeSubControls & QStyle::SC_ToolButton)
                flags |= QStyle::State_Sunken;
            mflags |= QStyle::State_Sunken;
        }

        BackstageButtonStyleOption thisOpt = *toolbutton;
        thisOpt.iconSize = iconSize;
        p.proxy()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(CommonStyle::PE_RibbonBackstagePanelButton), &thisOpt, painter, widget);

        BackstageButtonStyleOption label = *toolbutton;
        label.state = flags;
        p.proxy()->drawControl(static_cast<QStyle::ControlElement>(CommonStyle::CE_RibbonBackstageButtonLabel), &label, painter, widget);

        if (thisOpt.toolButtonStyle == Qt::ToolButtonTextUnderIcon ||
            thisOpt.toolButtonStyle == Qt::ToolButtonTextOnly)
        {
            QRect rcText = thisOpt.rect;
            const int indentation = thisOpt.icon.isNull() ? 0 :
                thisOpt.iconSize.height() + (opt->fontMetrics.height() / 2 + 2) / 2;
            const int ret = CommonStylePrivate::dpiScaled(2, widget);
            rcText.adjust(ret, indentation, -ret, 0);
            rcText.setHeight(thisOpt.rect.height() - indentation);

            QPalette_setColor(thisOpt.palette, QPalette::Text, thisOpt.palette.color(QPalette::ButtonText));
            thisOpt.document->setTextWidth(static_cast<qreal>(rcText.width()));
            QSize szDoc = thisOpt.document->size().toSize();
            QRect rcTxt = rcText;

            rcTxt.translate(0, (rcText.height() - szDoc.height()) / 2);
            rcTxt.setHeight(rcTxt.height() - (rcText.height() - szDoc.height()) / 2);
            ::qtn_QTextDocument_draw(*painter, thisOpt.document, thisOpt.palette, rcTxt);

            if (thisOpt.features & QStyleOptionToolButton::HasMenu)
            {
                QTextBlock block = thisOpt.document->firstBlock();
                if (block.isValid())
                {
                    const QTextLayout* tl = block.layout();
                    int count = tl ? tl->lineCount() : 0;
                    if (count > 0)
                    {
                        QTextLine tline = tl->lineAt(count - 1);
                        if (tline.isValid())
                        {
                            QRect rectRow = rcTxt;
                            const int minWidth = static_cast<int>(tl->minimumWidth());
                            rectRow.translate((thisOpt.rect.width() - minWidth) / 2, 0);
                            rectRow.adjust(0, thisOpt.fontMetrics.height() * (count - 1), 0, 0);
                            rectRow.setWidth(minWidth);
                            const int retArrow = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget) - 5;
                            if (count == 1)
                                thisOpt.rect = QRect(QPoint(rectRow.center().x() - retArrow / 2, 
                                    rectRow.top() + szDoc.height()), QSize(retArrow, retArrow));
                            else
                                thisOpt.rect = QRect(QPoint(rectRow.right() + qtn_horizontalAdvanceFont(thisOpt.fontMetrics, QStringLiteral("x")) - retArrow / 2,
                                    rectRow.top() + (rectRow.height() - retArrow) / 2), QSize(retArrow, retArrow));

                            p.proxy()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &thisOpt, painter, widget);
                        }
                    }
                }
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawToggleSwitch(const QStyleOptionComplex* opt, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    if (const ToggleSwitchStyleOption* optSwitch = qstyleoption_cast<const ToggleSwitchStyleOption*>(opt))
    {
        QRectF rect = optSwitch->rect;
        qreal margin = optSwitch->rect.height() - optSwitch->sizeIndicator.height();
        rect = rect.adjusted(0, margin / 2.0, 0, 0);
        rect.setHeight(optSwitch->sizeIndicator.height());

        if (rect.isEmpty())
            return true;

        const bool isDrawFocusFrame = false;//optSwitch->state & QStyle::State_HasFocus;
        const bool enabled = optSwitch->state & QStyle::State_Enabled;
        const bool isOn = optSwitch->state & QStyle::State_On;
        const bool isAnimated = optSwitch->isAnimated;

        if (optSwitch->alignment == Qt::AlignLeft)
            rect.setLeft(rect.x() + (rect.width() - optSwitch->sizeIndicator.width()));
        rect.setWidth(optSwitch->sizeIndicator.width());

        bool drawFocus = isDrawFocusFrame;
        Q_UNUSED(drawFocus);
        qreal ctrlIndent = 2.0;//drawFocus ? 2.0 : 0.5;
        rect.adjust(ctrlIndent, ctrlIndent, -ctrlIndent, -ctrlIndent);

        QColor clrText = optSwitch->palette.color(QPalette::WindowText);
        painter->setPen(clrText);
        QRectF rectText = optSwitch->rect;
        int spacing = p.proxy()->pixelMetric((QStyle::PixelMetric)CommonStyle::PM_ToggleSwitchLabelSpacing, Q_NULL, w);
        if (optSwitch->alignment == Qt::AlignLeft)
            rectText = optSwitch->rect.adjusted(0, 0, -rect.width(), -1);
        else if (optSwitch->alignment == Qt::AlignRight)
            rectText = optSwitch->rect.adjusted(rect.width() + spacing, 0, -1, -1);
        painter->drawText(rectText, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine, optSwitch->text);

        QRectF rectThumb = rect;
        qreal indent = 0.0;
        qreal thumbsSize = 0.0;
        qreal animOffset = 0.0;

        QRectF rectLabel = rect;
        bool isVertical = false;
        Q_UNUSED(isVertical);
        if (rect.width() >= rect.height())
        {
            thumbsSize = (optSwitch->shape != ToggleSwitch::Circle && optSwitch->shape != ToggleSwitch::CircleSmallGroove) ?
                rectThumb.height() * 2.0 / 3.0 : rectThumb.height() - 1;

            indent = qMax(2.0, thumbsSize / 6.0);
            rectLabel.adjust(indent, indent, -indent, -indent);
            if (isOn)
            {
                rectThumb.setLeft(rectThumb.right() - thumbsSize);
                rectLabel.setRight(rectThumb.left() - 1);
                if (isAnimated)
                    animOffset = (rectThumb.width() - rect.width()) * optSwitch->animationOffset;
            }
            else
            {
                rectThumb.setRight(rectThumb.left() + thumbsSize);
                rectLabel.setLeft(rectThumb.right() + 1);

                if (isAnimated)
                    animOffset = (rect.width() - rectThumb.width()) * optSwitch->animationOffset;
            }

            if (animOffset != 0.)
                rectThumb.translate(animOffset, 0.0);
        }
        else
        {
            isVertical = true;

            thumbsSize = (optSwitch->shape != ToggleSwitch::Circle && optSwitch->shape != ToggleSwitch::CircleSmallGroove) ? 
                rectThumb.width() * 2.0 / 3.0 : rectThumb.width();
            indent = qMax(2.0, thumbsSize / 6.0);
            rect.adjust(indent, indent, -indent, -indent);

            if (isOn)
            {
                rectThumb.setTop(rectThumb.bottom() - thumbsSize);
                rectLabel.setBottom(rectThumb.top() - 1);
                if (isAnimated)
                    animOffset = (rectThumb.height() - rect.height()) * optSwitch->animationOffset;
            }
            else
            {
                rectThumb.setBottom(rectThumb.top() + thumbsSize);
                rectLabel.setTop(rectThumb.bottom() + 1);
                if (isAnimated)
                    animOffset = (rect.height() - rectThumb.height()) * optSwitch->animationOffset;
            }

            if (animOffset != 0.0)
                rectThumb.translate(0., animOffset);
        }
        // Draw Groove
        if (optSwitch->subControls.testFlag(CommonStyle::SC_ToggleSwitchGroove))
        {
            QRectF rectFill = rect;
            QColor clrOutline = isOn ? optSwitch->palette.color(QPalette::Light) : optSwitch->palette.color(QPalette::Mid);
            QColor clrFill = isOn ? optSwitch->palette.color(QPalette::Highlight) : optSwitch->palette.color(QPalette::Button);

            if (!enabled)
            {
                clrOutline.setAlphaF(optSwitch->disabledOpacity * clrOutline.alphaF());
                clrFill.setAlphaF(optSwitch->disabledOpacity * clrFill.alphaF());
            }

            if (optSwitch->shape == ToggleSwitch::Rectangle || optSwitch->shape == ToggleSwitch::RectangleSmallGroove)
            {
                if (optSwitch->shape == ToggleSwitch::RectangleSmallGroove)
                {
                    qreal delta = indent  * 1.5;
                    rectFill.adjust(delta, delta, -delta, -delta);

                    QPainterPath path;
                    path.addRect(rectFill);
                    QPen pen(clrOutline, 1);
                    painter->setPen(pen);
                    painter->fillPath(path, clrFill);
                    painter->drawPath(path);
                }
                else
                {
                    QPainterPath path1;
                    path1.addRect(rect);
                    QPen pen(clrOutline, 1);
                    painter->setPen(pen);
                    painter->fillPath(path1, clrFill);
                    painter->drawPath(path1);

                    rectFill.adjust(indent, indent, -indent, -indent);
                    QPainterPath path2;
                    path2.addRect(rect);
                    painter->fillPath(path2, clrFill);
                    painter->drawPath(path2);
                }
            }
            else
            {
                qreal radius = 0.0;
                switch (optSwitch->shape)
                {
                    case ToggleSwitch::Circle:
                        radius = 0.5 * thumbsSize;
                        break;

                    case ToggleSwitch::CircleSmallGroove:
                        radius = 0.25 * thumbsSize;
                        rectFill.adjust(indent, indent, -indent, -indent);
                        break;

                    case ToggleSwitch::Rounded:
                        radius = indent * 2.0;
                        break;

                    case ToggleSwitch::RoundedSmallGroove:
                        radius = indent * 1.5;
                        rectFill.adjust(radius, radius, -radius, -radius);
                        break;
                    default:
                        break;
                }
                painter->save();
                QRectF rectFillRounded = rectFill;
                painter->setRenderHint(QPainter::Antialiasing);
                QPainterPath path;
                path.addRoundedRect(rectFillRounded, radius, radius);
                QPen pen(clrOutline, 1.0);
                painter->setPen(pen);
                painter->fillPath(path, clrFill);
                painter->drawPath(path);
                painter->restore();
            }
        }
        // Draw Text
        if (optSwitch->subControls.testFlag(CommonStyle::SC_ToggleSwitchText))
        {
            rectLabel.adjust(1.0, 1.0, -1.0, -1.0);
            if (optSwitch->orientation == Qt::Horizontal)
                rectLabel.setRight(rectLabel.right() - 1);
            else
                rectLabel.setBottom(rectLabel.bottom() - 1);

            QColor clrTextSwitch = isOn ? optSwitch->palette.color(QPalette::ButtonText) :
                optSwitch->palette.color(QPalette::Text);

            qreal textOpacity = clrTextSwitch.alphaF();
            if (!enabled)
                clrTextSwitch.setAlphaF(optSwitch->disabledOpacity * textOpacity);
            else if (optSwitch->isAnimated)
                clrTextSwitch.setAlphaF((1.0 - optSwitch->animationOffset) * textOpacity);

            painter->setPen(clrTextSwitch);
            painter->drawText(rectLabel, Qt::AlignCenter | Qt::TextSingleLine, optSwitch->statusText);
        }

        // Draw Thumb
        bool dontDrawThum = isOn ? optSwitch->dontDrawThumbOn : optSwitch->dontDrawThumbOff;
        if (!dontDrawThum)
        {
            int thSize = optSwitch->shape == ToggleSwitch::Circle ? p.proxy()->pixelMetric((QStyle::PixelMetric)CommonStyle::PM_ToggleSwitchThumbSize, Q_NULL, w) : 0;
            rectThumb.adjust(thSize, thSize, -thSize, -thSize);

            qreal roundedRectCornerRadius = indent * 1.5;
            QColor clrOutlineThumb = isOn ? optSwitch->palette.color(QPalette::Dark) : optSwitch->palette.color(QPalette::Midlight);
            QColor clrFillThumb    = isOn ? optSwitch->palette.color(QPalette::Base) : isQtitanStyle() ? 
                optSwitch->palette.color(QPalette::Window) : optSwitch->palette.color(QPalette::Dark);

            if (!enabled)
            {
                clrOutlineThumb.setAlphaF(optSwitch->disabledOpacity * clrOutlineThumb.alphaF());
                clrFillThumb.setAlphaF(optSwitch->disabledOpacity * clrFillThumb.alphaF());
            }

            switch (optSwitch->shape)
            {
                case ToggleSwitch::Circle:
                case ToggleSwitch::CircleSmallGroove:
                {
                    painter->save();
                    painter->setRenderHint(QPainter::Antialiasing);
                    QPainterPath path;
                    path.addEllipse(rectThumb);
                    QPen pen(clrOutlineThumb, 1);
                    painter->setPen(pen);
                    painter->fillPath(path, clrFillThumb);
                    painter->drawPath(path);
                    painter->restore();
                }
                break;
                case ToggleSwitch::Rounded:
                case ToggleSwitch::RoundedSmallGroove:
                {
                    painter->save();
                    painter->setRenderHint(QPainter::Antialiasing);
                    QPainterPath path;
                    path.addRoundedRect(rectThumb, roundedRectCornerRadius, roundedRectCornerRadius);
                    QPen pen(clrOutlineThumb, 1);
                    painter->setPen(pen);
                    painter->fillPath(path, clrFillThumb);
                    painter->drawPath(path);
                    painter->restore();
                }
                break;
                default:
                {
                    QPainterPath path;
                    path.addRect(rectThumb);
                    QPen pen(clrOutlineThumb, 1);
                    painter->setPen(pen);
                    painter->fillPath(path, clrFillThumb);
                    painter->drawPath(path);
                }
                break;
            }
        }
        if (isDrawFocusFrame)
        {
            QStyleOptionFocusRect optFocus;
            optFocus.QStyleOption::operator=(*optSwitch);
            QRectF rectFrame = rect;
            rectFrame.adjust(0.1, 0.1, 0.5, 0.5);
            optFocus.rect = rectFrame.toRect();
            p.proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &optFocus, painter, w);
        }
        return true;
    }
    return false;
}

/*! \internal */
QRect CommonStylePrivate::ribbonSubControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QStyle::SubControl sc, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (p.baseStyle() && p.baseStyle()->inherits(QTITAN_META_CLASSNAME("OfficeStyle")))
        return p.QProxyStyle::subControlRect(cc, opt, sc, widget);

    QRect retRect;
    if (!(widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonButton"))))
        return retRect;

    switch (cc)
    {
        case QStyle::CC_ToolButton:
            if (const QStyleOptionToolButton* tb = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
            {
                retRect = tb->rect;
                const int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, tb, widget);
                switch (sc)
                {
                    case QStyle::SC_ToolButton:
                        if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay)) == QStyleOptionToolButton::MenuButtonPopup)
                        {
                            const int sz = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, tb, widget);
                            const bool largeIconOnly = (sz == tb->iconSize.width() && tb->toolButtonStyle == Qt::ToolButtonIconOnly);
                            if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon || largeIconOnly)
                            {
                                QRect popupRect = p.proxy()->subControlRect(cc, opt, QStyle::SC_ToolButtonMenu, widget);
                                retRect.adjust(0, 0, 0, -popupRect.height());
                            }
                            else
                                retRect.adjust(0, 0, -mbi, 0);
                        }
                        break;
                    case QStyle::SC_ToolButtonMenu:
                    {
                        if (widget && !widget->inherits(QTITAN_META_CLASSNAME("RibbonGroupPopupButton")))
                        {
                            const int sz = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, tb, widget);
                            const bool largeIconOnly = (sz == tb->iconSize.width() && tb->toolButtonStyle == Qt::ToolButtonIconOnly);
                            if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon || largeIconOnly)
                            {
                                QRect rect = opt->rect;
                                int alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextHideMnemonic;
                                rect.setWidth(qtn_horizontalAdvanceFont(tb->fontMetrics, QStringLiteral("XXXXXX")));
                                QSize textSize = tb->fontMetrics.boundingRect(rect, alignment, QStringLiteral("XXXXXX XXXXXX")).size();
                                retRect.adjust(0, opt->rect.bottom() - textSize.height(), 0, 0);
                            }
                            else
                                retRect.adjust(retRect.width() - mbi, 0, 0, 0);
                        }
                        break;
                    }
                    default:
                        break;
                }
                retRect = p.visualRect(tb->direction, tb->rect, retRect);
            }
            break;
        default:
            break;
    }
    return retRect;
}

/*! \internal */
QSize CommonStylePrivate::ribbonSizeFromContents(QStyle::ContentsType ct, const QStyleOption* opt, const QSize& csz, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    QSize sz = csz;
    switch (ct)
    {
        case QStyle::CT_MdiControls:
        {
            if (const QMdiArea* area = qobject_cast<const QMdiArea*>(widget))
            {
                if (QtnPrivate::qtn_findChild(area->parentWidget(), QTITAN_META_CLASSNAME("RibbonBar")))
                {
                    if (const QStyleOptionComplex *styleOpt = qstyleoption_cast<const QStyleOptionComplex *>(opt))
                    {
                        int width = 0;
                        if (styleOpt->subControls & QStyle::SC_MdiMinButton)
                            width += csz.height() + 1;
                        if (styleOpt->subControls & QStyle::SC_MdiNormalButton)
                            width += csz.height() + 1;
                        if (styleOpt->subControls & QStyle::SC_MdiCloseButton)
                            width += csz.height() + 1;
                        sz = QSize(width, csz.height());
                    }
                }
                else
                    sz = QSize();
            }
        }
        break;
        case QStyle::CT_TabBarTab:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonTabBar")) && !sz.isEmpty())
                sz.setHeight(widget->height());
            break;
        case QStyle::CT_MenuBarItem:
        {
            if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")) && !sz.isEmpty())
                sz += QSize(windowsItemHMargin * 5 + 1, 5);
            else
                sz = QSize();
        }
        break;
        case QStyle::CT_MenuItem:
        {
            bool ourMenu = (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")));
            if (!ourMenu && (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup"))))
                ourMenu = true;
            if (ourMenu)
            {
                if (const QStyleOptionMenuItem* mi = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
                {
                    int w = sz.width();
                    sz = p.QProxyStyle::sizeFromContents(ct, opt, csz, widget);
                    if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                    {
                        sz = QSize(10, windowsSepHeight);
                    }
                    else if (mi->icon.isNull())
                    {
                        sz.setHeight(qMax(sz.height() - 2, p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget)));
                        w -= 6;
                    }

                    if (mi->menuItemType != QStyleOptionMenuItem::Separator && !mi->icon.isNull())
                    {
                        int iconExtent = p.proxy()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
                        sz.setHeight(qMax(sz.height(), iconExtent + 2 * windowsItemFrame));
                    }
                    int maxpmw = mi->maxIconWidth;
                    int tabSpacing = 20;//use2000style ? 20 : windowsTabSpacing;
                    if (mi->text.contains(QLatin1Char('\t')))
                        w += tabSpacing;
                    else if (mi->menuItemType == QStyleOptionMenuItem::SubMenu)
                        w += 2 * windowsArrowHMargin;
                    else if (mi->menuItemType == QStyleOptionMenuItem::DefaultItem)
                    {
                        // adjust the font and add the difference in size.
                        // it would be better if the font could be adjusted in the initStyleOption qmenu func!!
                        QFontMetrics fm(mi->font);
                        QFont fontBold = mi->font;
                        fontBold.setBold(true);
                        QFontMetrics fmBold(fontBold);
                        w += qtn_horizontalAdvanceFont(fmBold, mi->text) - qtn_horizontalAdvanceFont(fm, mi->text);
                    }
                    // Windows always shows a check column
                    int checkcol = qMax(maxpmw, windowsCheckMarkWidth);
                    w += checkcol;
                    w += windowsRightBorder + 12;
                    sz.setWidth(w);

                    if (mi->menuItemType != QStyleOptionMenuItem::Separator && 
                        (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup"))))
                    {
                        QFontMetrics fm(mi->font);
                        sz.setHeight(qMax(sz.height(), fm.height() * 3 + fm.height() / 2 + windowsItemVMargin * 2));
                    }

//                    if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup")))
//                        sz.setWidth(widget->width());

                }
            }
            else if (widget && widget->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu")))
            {
                if (const QStyleOptionMenuItem* mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                {
                    sz = p.QProxyStyle::sizeFromContents(ct, opt, csz, widget);
                    if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                        sz.setHeight(csz.height());
                }
            }
            else
            {
                sz = p.QProxyStyle::sizeFromContents(ct, opt, csz, widget);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
                sz.setWidth(sz.width() + sz.height());
#endif
                if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                {
                    if (menuitem->menuItemType != QStyleOptionMenuItem::Separator)
                    {
                        sz.setHeight(qMax(sz.height(), 22));
                        if (menuitem->checkType != QStyleOptionMenuItem::NotCheckable)
                            sz.setWidth(sz.width() + sz.height());
                    }
                    else if (menuitem->menuItemType == QStyleOptionMenuItem::Separator && !menuitem->text.isEmpty())
                    {
                        int w = 0;
                        QFontMetrics fm(menuitem->font);
                        QFont fontBold = menuitem->font;
                        fontBold.setBold(true);
                        QFontMetrics fmBold(fontBold);
                        w += qtn_horizontalAdvanceFont(fmBold, menuitem->text) + 6 + 6;
                        int h = menuitem->fontMetrics.height() + 6;
                        sz.setHeight(qMax(h, 22));
                        sz.setWidth(qMax(w, sz.width()));
                    }
                }
            }
        }
        break;
        default:
            sz = QSize();
            break;
    }
    return sz;
}

/*! \internal */
QRect CommonStylePrivate::ribbonSubElementRect(QStyle::SubElement se, const QStyleOption* option, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (p.baseStyle() && p.baseStyle()->inherits(QTITAN_META_CLASSNAME("OfficeStyle")))
        return p.QProxyStyle::subElementRect(se, option, widget);

    QRect retRect;
    switch (static_cast<CommonStyle::SubElementEx>(se))
    {
        case CommonStyle::SE_RibbonSysHeaderLabelPopupList:
        {
            QFontMetrics fm = option->fontMetrics;
            QRect rcTextMetrics = fm.boundingRect(QStringLiteral("XXX"));
            const int vMarginCaption = p.proxy()->pixelMetric(static_cast<QStyle::PixelMetric>(CommonStyle::PM_RibbonVMarginCaptionSysMenu), option, widget);
            retRect = QRect(option->rect.left(), option->rect.top(), option->rect.right(), (option->rect.top() + rcTextMetrics.height() + vMarginCaption * 2));
        }
        break;
    }
    return retRect;
}

/*! \internal */
QRect CommonStylePrivate::navigationEdgeSubElementRect(QStyle::SubElement se, const QStyleOption* option, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    QRect retRect = QRect();
    switch (se)
    {
    case QStyle::SE_TabBarTabLeftButton:
    case QStyle::SE_TabBarTabRightButton:
    {
        if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("NavigationTabBar")))
        {
            if (const StyleOptionTab* tab = qstyleoption_cast<const StyleOptionTab*>(option))
            {
                bool selected = tab->state & QStyle::State_Selected;
                int verticalShift = p.proxy()->pixelMetric(QStyle::PM_TabBarTabShiftVertical, tab, widget);
                int horizontalShift = p.proxy()->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, tab, widget);
                int hpadding = p.proxy()->pixelMetric(QStyle::PM_TabBarTabHSpace, option, widget) / 3;
                hpadding = qMax(hpadding, 4);
                QRect tr = tab->rect;
                tr.adjust(0, 0, horizontalShift, verticalShift);
                if (selected)
                {
                    tr.setBottom(tr.bottom() - verticalShift);
                    tr.setRight(tr.right() - horizontalShift);
                }

                QSize size = (se == QStyle::SE_TabBarTabLeftButton) ? tab->leftButtonSize : tab->rightButtonSize;
                int w = size.width();
                int h = size.height();
                int midHeight = static_cast<int>(ceil(float(tr.height() - h) / 2));
                if (tab->shape == QTabBar::RoundedNorth)
                {
                    if (se == QStyle::SE_TabBarTabLeftButton)
                        retRect = QRect(tab->rect.x() + hpadding, midHeight, w, h);
                    else
                        retRect = QRect(tab->rect.right() - w - hpadding, midHeight, w, h);
                    retRect = QStyle::visualRect(tab->direction, tab->rect, retRect);
                }
            }
        }
    }
    break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    case QStyle::SE_TabBarScrollLeftButton:
    {
        if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("NavigationTabBar")))
        {
            const Qt::LayoutDirection ld = widget->layoutDirection();
            const int buttonWidth = p.proxy()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, nullptr, widget);
            const int buttonOverlap = p.proxy()->pixelMetric(QStyle::PM_TabBar_ScrollButtonOverlap, nullptr, widget);
            Q_UNUSED(buttonOverlap);
            retRect = QStyle::visualRect(ld, option->rect, QRect(/*option->rect.width() - (buttonWidth * 2) + buttonOverlap*/0, 0, buttonWidth, option->rect.height()));
        }
    }
    break;
    case QStyle::SE_TabBarScrollRightButton:
    {
        if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("NavigationTabBar")))
        {
            const Qt::LayoutDirection ld = widget->layoutDirection();
            const int buttonWidth = p.proxy()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, nullptr, widget);
            retRect = QStyle::visualRect(ld, option->rect, QRect(option->rect.width() /*- buttonWidth*/, 0, buttonWidth, option->rect.height()));
//                retRect = QRect(option->rect.left() /*- buttonWidth*/, 0, buttonWidth, option->rect.height());
        }
    }
    break;
#endif
    default:
        break;
    }
    return retRect;
}

/*! \internal */
int CommonStylePrivate::navigationEdgePixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    Q_UNUSED(option);
    int val = CommonStylePrivate::InvalidMetric;
    switch (metric)
    {
        case QStyle::PM_TabBarTabVSpace:
        {
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("NavigationTabBar")))
                val = CommonStylePrivate::dpiScaled(10, widget);
        }
        break;
        case QStyle::PM_TabCloseIndicatorWidth:
        case QStyle::PM_TabCloseIndicatorHeight:
        {
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("AbstractTabCloseButton")))
                val = CommonStylePrivate::dpiScaled(10, widget);
        }
        break;
        case QStyle::PM_TabBarTabShiftHorizontal:
        case QStyle::PM_TabBarTabShiftVertical:
        {
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("NavigationTabBar")))
                val = 0;
        }
        break;
        case QStyle::PM_TabBarTabHSpace:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("NavigationTabBar")))
                val = CommonStylePrivate::dpiScaled(24, widget);
            break;
        case QStyle::PM_TabBarIconSize:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("NavigationTabBar")))
                val = CommonStylePrivate::dpiScaled(16, widget);
            break;
        default:
            break;
    }
    return val;
}

/*! \internal */
QRect CommonStylePrivate::dockSubElementRect(QStyle::SubElement se, const QStyleOption* option, const QWidget* widget) const
{
    Q_UNUSED(se)
    Q_UNUSED(option)
    Q_UNUSED(widget)
    return QRect();
}

/*! \internal */
int CommonStylePrivate::basePixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    Q_UNUSED(option);
    bool ok = false;
    int defaultValue = CommonStylePrivate::InvalidMetric;
    int val = CommonStylePrivate::InvalidMetric;
    switch (static_cast<CommonStyle::PixelMetricEx>(metric))
    {
        case CommonStyle::PM_BaseScrollButtonItem:
            {
                defaultValue = CommonStylePrivate::dpiScaled(12, widget);
                val = m_config.pixelMetric(widget, QStringLiteral("BaseScrollButtonItem"), QStringLiteral("ScrollSize"), defaultValue, &ok);
            }
            break;
        case CommonStyle::PM_ToggleSwitchThumbSize:
            {
                defaultValue = 0;
                val = m_config.pixelMetric(widget, QString(), QStringLiteral("ToggleSwitchThumbSize"), defaultValue, &ok);
            }
            break;
        case CommonStyle::PM_ToggleSwitchMargin:
            {
                defaultValue = CommonStylePrivate::dpiScaled(3, widget);
                val = m_config.pixelMetric(widget, QString(), QStringLiteral("ToggleSwitchMargin"), defaultValue, &ok);
            }
            break;
        case CommonStyle::PM_ToggleSwitchLabelSpacing:
            defaultValue = CommonStylePrivate::dpiScaled(6, widget);
            val = m_config.pixelMetric(widget, QString(), QStringLiteral("ToggleSwitchLabelSpacing"), defaultValue, &ok);
            break;
        default: break;
    }
    return val;
}

/*! \internal */
int CommonStylePrivate::ribbonPixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    bool ok = false;
    int defaultValue = 0;
    int val = CommonStylePrivate::InvalidMetric;
    switch (metric)
    {
        case QStyle::PM_MenuPanelWidth:
//            if (widget && (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")) ||
//                widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup"))))
//                val = 0;
//            else 
                if ((widget && widget->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu"))) ||
                (QApplication::activePopupWidget() && 
                    QApplication::activePopupWidget()->inherits(QTITAN_META_CLASSNAME("OfficePopupMenu"))))
                val = 1;
            break;
        case QStyle::PM_MenuHMargin:
            if (widget && (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")) ||
                widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup"))))
                val = 0;
            break;
        case QStyle::PM_MenuVMargin:
            if (widget && (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")) ||
                widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup"))))
                val = 0;
            break;
        case QStyle::PM_ToolBarIconSize:
            if (widget && (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")) ||
                widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup"))))
                val = isQtitanStyle() ? CommonStylePrivate::dpiScaled(32.0, widget) : 32;
            break;
        case QStyle::PM_SmallIconSize:
            if (widget && (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemMenu")) ||
                widget->inherits(QTITAN_META_CLASSNAME("RibbonPageSystemPopup"))))
                val = isQtitanStyle() ? CommonStylePrivate::dpiScaled(32.0, widget) : 32;
            break;
        case QStyle::PM_ScrollBarExtent:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonGallery")))
                val = CommonStylePrivate::dpiScaled(14.0, widget);//isQtitanStyle() ? qRound(CommonStylePrivate::dpiScaled(14.0, widget)) : 14;
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
        case QStyle::PM_TitleBarButtonSize:
            if (const QMdiArea* area = qobject_cast<const QMdiArea*>(widget))
            {
                if (const QWidget* ribbonTabBar = qobject_cast<const QWidget*>(QtnPrivate::qtn_findChild(area->parentWidget(), QTITAN_META_CLASSNAME("RibbonTabBar"))))
                    val = qRound(static_cast<double>(ribbonTabBar->height()) * 0.7);
            }
            break;
#endif
        default:
            break;
    }

    if (val == CommonStylePrivate::InvalidMetric)
    {
        switch (static_cast<CommonStyle::PixelMetricEx>(metric))
        {
            case CommonStyle::PM_RibbonQuickAccessBarMargin:
                val = m_config.pixelMetric(widget, QStringLiteral("RibbonQuickAccessBar"), QStringLiteral("RibbonQuickAccessBarMargin"), defaultValue, &ok);
                break;
            case CommonStyle::PM_RibbonGroupContentMargin:
                val = CommonStylePrivate::dpiScaled(3, widget);
                break;
            case CommonStyle::PM_RibbonLayoutHorizontalSpacing:
                val = CommonStylePrivate::dpiScaled(2, widget);
                break;
            case CommonStyle::PM_RibbonOptionButtonIconSize:
                val = CommonStylePrivate::dpiScaled(10, widget);
                break;
            case CommonStyle::PM_RibbonPopupMenuGripHeight:
                val = CommonStylePrivate::dpiScaled(12, widget);
                break;
            case CommonStyle::PM_RibbonVMarginCaptionSysMenu:
                val = CommonStylePrivate::dpiScaled(3, widget);
                break;
            case CommonStyle::PM_RibbonHMarginCaptionSysMenu:
                val = CommonStylePrivate::dpiScaled(6, widget);
                break;
#if 0
            case CommonStyle::PM_RibbonHeightCaptionGroup:
                if (const RibbonGroupStyleOption* optGroup = qstyleoption_cast<const RibbonGroupStyleOption*>(option))
                {
                    const bool titleGroupsVisible = widget ? widget->property(_qtn_TitleGroupsVisible).toBool() : false;
                    val = titleGroupsVisible ? optGroup->fontMetrics.height() + (optGroup->fontMetrics.descent() + optGroup->fontMetrics.ascent()) / 3 : 0;
                }
                break;
#endif
            case CommonStyle::PM_RibbonGroupReducedWidth:
                if (const RibbonGroupStyleOption* optBut = qstyleoption_cast<const RibbonGroupStyleOption*>(option))
                {
                    int flags = Qt::TextWordWrap;
                    flags |= Qt::TextShowMnemonic;
                    if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, optBut, widget))
                        flags |= Qt::TextHideMnemonic;
                    QRect rect = optBut->rect;
                    QString str = optBut->text.left(optBut->text.indexOf(QStringLiteral(" ")));
                    int width = optBut->fontMetrics.boundingRect(rect, flags | Qt::AlignVCenter, str).width();
                    str = optBut->text.right(optBut->text.indexOf(QStringLiteral(" ")));
                    width = qMax(width, optBut->fontMetrics.boundingRect(rect, flags | Qt::AlignVCenter, str).width());
                    const int mbi = p.proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator, Q_NULL, widget);
                    val = width + qtn_horizontalAdvanceFont(optBut->fontMetrics, QStringLiteral("XXX")) + mbi;
                }
                break;
             default:
                break;
        }
    }
    return val;
}

/*! \internal */
int CommonStylePrivate::gridPixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    int val = CommonStylePrivate::InvalidMetric;
    switch (static_cast<CommonStyle::PixelMetricEx>(metric))
    {
        case CommonStyle::PM_GridGroupPanelHeight:
            if (option != Q_NULL)
                val = option->fontMetrics.height() * 3;
            else
                val = qApp->fontMetrics().height() * 3;
            break;
        case CommonStyle::PM_GridSmallToolButtonSize:
            val = qtn_DPIScaled(9, widget);
        break;
        default:
            break;
    }
    return val;
}

// pixelMetric
/*! \internal */
int CommonStylePrivate::ribbonStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    Q_UNUSED(widget)
    QTN_P(const CommonStyle);
    int ret = 0;
    switch (static_cast<CommonStyle::StyleHintEx>(stylehint))
    {
        case CommonStyle::SH_WindowTitleBarPalette:
            if (QStyleHintReturnVariant* vret = qstyleoption_cast<QStyleHintReturnVariant*>(returnData))
            {
                if (isQtitanStyle())
                {
                    vret->variant = p.standardPalette();
                    ret = true;
                }
            }
            break;
        case CommonStyle::SH_RecentFileListBackground:
            return opt ? static_cast<int>(darkBackground(opt->palette).rgb()) : 0;
        case CommonStyle::SH_RibbonBackstageHideTabs:
            return false;
        case CommonStyle::SH_RibbonItemUpperCase:
            return m_config.styleHint(QString(), QStringLiteral("RibbonItemUpperCase"), false);
        case CommonStyle::SH_RibbonStyledFrame:
#ifdef Q_OS_WIN
            return m_config.styleHint(QString(), QStringLiteral("RibbonStyledFrame"), ret);
#else // Q_OS_WIN
            return true;
#endif // Q_OS_WIN
        case CommonStyle::SH_RibbonsSysMenuButtonVisible:
            return m_config.styleHint(QString(), QStringLiteral("RibbonsSysMenuButtonVisible"), true);
        case CommonStyle::SH_RibbonSysFrameMenu:
            if (HintReturnSysFrameMenuStyleOption* frameMenu = qstyleoption_cast<HintReturnSysFrameMenuStyleOption*>(returnData))
            {
                const int sysPopupBorderTop = CommonStylePrivate::dpiScaled(systemPopupBorderTop, widget);
                const int sysPopupBorderBottom = CommonStylePrivate::dpiScaled(systemPopupBorderBottom, widget);
                const int sysPopupBorderRight = CommonStylePrivate::dpiScaled(systemPopupBorderRight, widget);
                const int sysPopupBorderLeft = CommonStylePrivate::dpiScaled(systemPopupBorderLeft, widget);
                const int sysPopupBorderTopChosen = (sysPopupBorderTop != frameMenu->margins.top()) ? frameMenu->margins.top() : sysPopupBorderTop;
                frameMenu->margins = QMargins(sysPopupBorderLeft, sysPopupBorderTopChosen + 2, sysPopupBorderRight, sysPopupBorderBottom);
                return 1;
            }
            break;
        default:
            break;
    }
    return ret;
}

/*! \internal */
int CommonStylePrivate::dockStyleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    Q_UNUSED(opt)
    Q_UNUSED(widget)
    Q_UNUSED(returnData)
    int ret = 0;
    switch (static_cast<CommonStyle::StyleHintEx>(stylehint))
    {
        case CommonStyle::SH_DockSysMenuButtonVisible:
            return m_config.styleHint(QString(), QStringLiteral("DockSysMenuButtonVisible"), ret);
        case CommonStyle::SH_DockStyledFrame:
#ifdef Q_OS_WIN
            return m_config.styleHint(QString(), QStringLiteral("DockStyledFrame"), ret);
#else // Q_OS_WIN
            return true;
#endif // Q_OS_WIN
        default:
            break;
    }
    return ret;
}

// standardPixmap
/*! \internal */
QPixmap CommonStylePrivate::ribbonStandardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    Q_UNUSED(widget)
    QPixmap px;
    switch (static_cast<CommonStyle::StandardPixmapEx>(sp))
    {
        case CommonStyle::SP_RibbonButtonPopupMenu:
            {
                ImageState state = (opt && opt->state & QStyle::State_Enabled) ? Black : Gray;
                if (!isQtitanStyle() && widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonQuickAccessButton")))
                {
                    QWidget* parentWidget = widget->parentWidget();
                    if (parentWidget && parentWidget->inherits(QTITAN_META_CLASSNAME("RibbonQuickAccessBar")))
                    {
                        parentWidget = parentWidget->parentWidget();
                        if (parentWidget && parentWidget->inherits(QTITAN_META_CLASSNAME("RibbonTitleBarWidget")))
                        {
                            if (!qtn_isColorLight(opt->palette.color(QPalette::Window)))
                                state = (opt && opt->state & QStyle::State_Enabled) ? White : LtGray;
                        }
                    }
                }
                px = CommonStylePrivate::standardPixmap(QStringLiteral("Image_RibbonButtonPopupMenu"), state, widget);
            }
            break;
        case CommonStyle::SP_RibbonButtonGroupOption:
            px = CommonStylePrivate::standardPixmap(QStringLiteral("Image_GroupOption"), (opt && opt->state & QStyle::State_Enabled) ? Black : Gray, widget);
            break;
        case CommonStyle::SP_RibbonShadedArrowUp:
            px = CommonStylePrivate::standardPixmap(QStringLiteral("Image_ShadedArrowUp"), (opt && opt->state & QStyle::State_Enabled) ? Black : Gray, widget);
            break;
        case CommonStyle::SP_RibbonShadedArrowDown:
            px = CommonStylePrivate::standardPixmap(QStringLiteral("Image_ShadedArrowDown"), (opt && opt->state & QStyle::State_Enabled) ? Black : Gray, widget);
            break;
        default:
            break;
    }
    return px;
}

/*! \internal */
QPixmap CommonStylePrivate::navigationStandardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    Q_UNUSED(widget)
    QPixmap px;
    switch (static_cast<CommonStyle::StandardPixmapEx>(sp))
    {
        case CommonStyle::SP_NavigationBackButton:
            px = m_standardSymbol.standardSymbolPixmap(sp, opt, widget);
            break;
        default:
            break;
    }
    return px;
}

/*! \internal */
bool CommonStylePrivate::drawDockAutoHideFrameWindow(const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const AutoHideWindowStyleOption* dwOpt = qstyleoption_cast<const AutoHideWindowStyleOption*>(option))
    {
        QRect rc = option->rect;
        painter->fillRect(option->rect, dwOpt->palette.window());

        switch (dwOpt->position)
        {
            case Qtitan::LeftArea:
                QtnPrivate::draw3dRect(*painter, rc.right() - 1, rc.top(), 1, rc.height(), dwOpt->palette.shadow().color(), dwOpt->palette.shadow().color());
                QtnPrivate::draw3dRect(*painter, rc.right() - 2, rc.top(), 1, rc.height(), dwOpt->palette.dark().color(), dwOpt->palette.dark().color());
                break;
            case Qtitan::TopArea:
                QtnPrivate::draw3dRect(*painter, rc.left(), rc.bottom() - 1, rc.width(), 1, dwOpt->palette.shadow().color(), dwOpt->palette.shadow().color());
                QtnPrivate::draw3dRect(*painter, rc.left(), rc.bottom() - 2, rc.width(), 1, dwOpt->palette.dark().color(), dwOpt->palette.dark().color());
                break;
            case Qtitan::RightArea:
                QtnPrivate::draw3dRect(*painter, rc.left() + 1, rc.top(), 1, rc.height(), dwOpt->palette.light().color(), dwOpt->palette.light().color());
                break;
            case Qtitan::BottomArea:
                QtnPrivate::draw3dRect(*painter, rc.left(), rc.top() + 1, rc.width(), 1, dwOpt->palette.light().color(), dwOpt->palette.light().color());
                break;
            default:
                break;
        }
        return true;
    }
    return false;
}

/*! \internal */
QPixmap CommonStylePrivate::dockStandardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    Q_UNUSED(widget)
    switch (static_cast<CommonStyle::StandardPixmapEx>(sp))
    {
        case CommonStyle::SP_TitleBarPinButton:
            return m_standardSymbol.standardSymbolPixmap(sp, opt, widget);
        case CommonStyle::SP_TitleBarPinnedButton:
            return m_standardSymbol.standardSymbolPixmap(sp, opt, widget);
        default:
            break;
    }
    return QPixmap();
}

/*! \internal */
void CommonStylePrivate::roundedAngle(QPainterPath& path, const QRectF& rect, qreal xRadius, qreal yRadius, bool drawTopPart) const
{
    QRectF r = rect.normalized();
    if (r.isNull())
        return;

    qreal width = r.width() / 2.0;
    qreal height = r.height() / 2.0;

    if (width == 0.0)
        xRadius = 0.0;
    else
        xRadius = 100.0 * qMin(xRadius, width) / width;

    if (height == 0.0)
        yRadius = 0.0;
    else
        yRadius = 100.0 * qMin(yRadius, height) / height;

    qreal x = r.x();
    qreal y = r.y();
    qreal w = r.width();
    qreal h = r.height();
    qreal rxx2 = w * xRadius / 100.0;
    qreal ryy2 = h * yRadius / 100.0;

    path.arcMoveTo(x, y, rxx2, ryy2, 180.0);
    path.arcTo(x, y, rxx2, ryy2, 180, -90.0);
    path.arcTo(x + w - rxx2, y, rxx2, ryy2, 90.0, -90.0);

    if (!drawTopPart)
    {
        path.arcTo(x + w - rxx2, y + h - ryy2, rxx2, ryy2, 0.0, -90.0);
        path.arcTo(x, y + h - ryy2, rxx2, ryy2, 270.0, -90.0);
        path.closeSubpath();
    }
}

#if 0
void CommonStylePrivate::drawRectDPI(QPainter* painter, const QRect& rect)
{
//  QPaintDevice* device = painter.device();
//  Q_ASSERT(device != Q_NULL);
    qreal factor = 1.0;//device->devicePixelRatioF();
    qreal penWidth = painter->pen().widthF() * factor;
    QRectF rc(QPointF(static_cast<qreal>(rect.left()) + penWidth / 2.0, static_cast<qreal>(rect.top()) + penWidth / 2.0),
              QSizeF(static_cast<qreal>(rect.width()) - (penWidth + 1.0) / 2.0, static_cast<qreal>(rect.height()) - (penWidth + 1.0) / 2.0));
    painter->drawRect(rc);
}
#endif

/*! \internal */
void CommonStylePrivate::drawLineDPI(QPainter* painter, const QRect& rect, bool left, bool top, bool right, bool bottom)
{
//    QPaintDevice* device = painter.device();
    qreal factor = 1.0;// device->devicePixelRatioF();
    qreal penWidth = painter->pen().widthF() * factor;
    if (left)
        painter->drawLine(QPointF(static_cast<qreal>(rect.left()) + penWidth / 2.0, static_cast<qreal>(rect.top()) + penWidth / 2.0), 
            QPointF(static_cast<qreal>(rect.left()) + penWidth / 2.0, static_cast<qreal>(rect.height()) - penWidth / 2.0));
    if (top)
        painter->drawLine(QPointF(static_cast<qreal>(rect.left()) - penWidth / 2.0, static_cast<qreal>(rect.top()) + penWidth / 2.0),
            QPointF(static_cast<qreal>(rect.width()) - penWidth / 2.0, static_cast<qreal>(rect.top()) + penWidth / 2.0));
    if (right)
        painter->drawLine(QPointF(static_cast<qreal>(rect.width()) - penWidth / 2.0, static_cast<qreal>(rect.top()) + penWidth / 2.0),
            QPointF(static_cast<qreal>(rect.width()) - penWidth / 2.0, static_cast<qreal>(rect.height()) - penWidth / 2.0));
    if (bottom)
        painter->drawLine(QPointF(static_cast<qreal>(rect.left()) + penWidth / 2.0, static_cast<qreal>(rect.height()) - penWidth / 2.0),
            QPointF(static_cast<qreal>(rect.width()) + penWidth / 2.0, static_cast<qreal>(rect.height()) - penWidth / 2.0));
}

/*! \internal */
void CommonStylePrivate::drawIcon(const QPixmap& px, QPainter* painter, const QRect& rect) const
{
    painter->drawPixmap(rect.topLeft(), px);
}

/*! \internal */
void CommonStylePrivate::drawHighlightButton(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    const bool highlighted = option->state & QStyle::State_MouseOver;
    const bool pressed = option->state & QStyle::State_Sunken;
    const bool enabled = option->state & QStyle::State_Enabled;
    if (enabled && (highlighted || pressed))
    {
        const QPen savePen = painter->pen();
        painter->setPen(buttonHighlightColor(option->palette, pressed, true, widget));
        QBrush fill = buttonHighlightColor(option->palette, pressed, false, widget);
        painter->fillRect(option->rect.adjusted(1, 0, -2, 0), fill);
        painter->drawRect(option->rect.adjusted(1, 0, -2, -1));
        painter->setPen(savePen);
    }
}

/*! \internal */
void CommonStylePrivate::drawHighlightItemGallery(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    const bool highlighted = option->state & QStyle::State_MouseOver;
    const bool pressed = option->state & QStyle::State_Sunken;
    const bool enabled = option->state & QStyle::State_Enabled;
    const bool checked = option->state & QStyle::State_On;
    if (enabled && (highlighted || pressed || checked))
    {
        const QPen savePen = painter->pen();
        painter->setPen(buttonHighlightColor(option->palette, pressed, true));
        QBrush fill = buttonHighlightColor(option->palette, pressed, false);
        painter->fillRect(option->rect.adjusted(1, 0, -2, 0), fill);
        painter->drawRect(option->rect.adjusted(1, 0, -2, -1));
        painter->setPen(savePen);
    }
}

/*! \internal */
void CommonStylePrivate::drawIconDp(const QPixmap& px, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget)
    QPixmap pixmap = px;
    if (!pixmap.isNull())
    {
        QIcon icon(pixmap);
        QRect rect = option->rect;
        QRect arrowRect;
        const int arrowWidth = CommonStylePrivate::dpiScaled(16, widget);
        const int arrowHeight = CommonStylePrivate::dpiScaled(12, widget);
        const int arrowMax = qMin(arrowHeight, arrowWidth);
        const int rectMax = qMin(rect.height(), rect.width());
        const int sqsize = qMin(arrowMax, rectMax);

        arrowRect.setWidth(sqsize);
        arrowRect.setHeight(sqsize);

        if (pixmap.width() > pixmap.height())
            arrowRect.setHeight(pixmap.height() * sqsize / pixmap.width());
        else
            arrowRect.setWidth(pixmap.width() * sqsize / pixmap.height());

        arrowRect.moveTopLeft(rect.center() - arrowRect.center());
        painter->save();
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(arrowRect, pixmap);
        painter->restore();
    }
}

/*! \internal */
bool CommonStylePrivate::drawIndicatorArrowDp(QStyle::PrimitiveElement pe, const QStyleOption* option, QPainter* painter, const QWidget* w) const
{
    QTN_P(const CommonStyle);
    if (option->rect.width() <= 1 || option->rect.height() <= 1)
        return true;
    QPixmap arrow;
    int rotation = 0;
    switch (pe)
    {
        case QStyle::PE_IndicatorArrowDown:
            rotation = 180;
            break;
        case QStyle::PE_IndicatorArrowRight:
            rotation = 90;
            break;
        case QStyle::PE_IndicatorArrowLeft:
            rotation = -90;
            break;
        default:
            break;
    }
    arrow = p.proxy()->standardPixmap(QStyle::SP_ArrowUp, option, w);
    if (rotation != 0)
    {
        QTransform transform;
        transform.translate(-arrow.width() / 2, -arrow.height() / 2);
        transform.rotate(rotation);
        transform.translate(arrow.width() / 2, arrow.height() / 2);
        arrow = arrow.transformed(transform);
    }

    if (arrow.isNull())
        return false;

    drawIconDp(arrow, option, painter, w);
    return true;
}

/*! \internal */
bool CommonStylePrivate::drawToolButtonLabelDp(const QStyleOption* opt, QPainter* painter, const QWidget* widget) const
{
    QTN_P(const CommonStyle);
    if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
    {
        QRect rect = toolbutton->rect;
        // Arrow type always overrules and is always shown
        bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly)
        {
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                alignment |= Qt::TextHideMnemonic;
            painter->setFont(toolbutton->font);
            p.proxy()->drawItemText(painter, rect, alignment, toolbutton->palette, 
                opt->state & QStyle::State_Enabled, toolbutton->text, QPalette::ButtonText);
        }
        else
        {
            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (!toolbutton->icon.isNull())
            {
                QIcon::State state = toolbutton->state & QStyle::State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(toolbutton->state & QStyle::State_Enabled))
                    mode = QIcon::Disabled;
                else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;
                pm = preparePixmap(toolbutton->icon, pmSize, mode, state, widget);
                pmSize = pm.size();
            }

            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
            {
                painter->setFont(toolbutton->font);
                QRect pr = rect, tr = rect;
                int alignment = Qt::TextShowMnemonic;
                if (!p.proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                    alignment |= Qt::TextHideMnemonic;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) 
                {
                    pr.setHeight(pmSize.height() + 4);
                    tr.adjust(0, pr.height() - 1, 0, -1);
                    if (!hasArrow)
                        p.proxy()->drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                    else
                        QtnPrivate::drawArrow(p.proxy(), toolbutton, pr, painter, widget);
                    alignment |= Qt::AlignCenter;
                }
                else
                {
                    pr.setWidth(pmSize.width() + 4);
                    tr.adjust(pr.width(), 0, 0, 0);
                    if (!hasArrow)
                        p.proxy()->drawItemPixmap(painter, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                    else
                        QtnPrivate::drawArrow(p.proxy(), toolbutton, pr, painter, widget);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                const QString text = toolButtonElideText(toolbutton, tr, alignment);
                p.proxy()->drawItemText(painter, QStyle::visualRect(opt->direction, rect, tr), alignment, toolbutton->palette,
                    toolbutton->state & QStyle::State_Enabled, text, QPalette::ButtonText);
            }
            else
            {
                if (hasArrow)
                    QtnPrivate::drawArrow(p.proxy(), toolbutton, rect, painter, widget);
                else
                    p.proxy()->drawItemPixmap(painter, rect, Qt::AlignCenter, pm);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::paintAnimation(TypePaint type, int nPrim, const QStyleOption* option, QPainter* painter, const QWidget* widget, int iDurationOver) const
{
    QTN_P(const CommonStyle);
    if (!p.isAnimationEnabled())// && widget && widget->isVisible())
        return false;

    bool bAnimation = qstyleoption_cast<const QStyleOptionToolButton*>(option) ||
        qstyleoption_cast<const QStyleOptionButton*>(option) ||
        qstyleoption_cast<const QStyleOptionGroupBox*>(option) ||
        qstyleoption_cast<const QStyleOptionHeader*>(option) ||
        qstyleoption_cast<const QStyleOptionComplex*>(option) ||
        qstyleoption_cast<const RibbonGroupStyleOption*>(option) ||
        qstyleoption_cast<const RibbonOptionHeaderStyleOption*>(option) ||
        qstyleoption_cast<const QStyleOptionFrame*>(option);

    int state = static_cast<int>(option->state);

    QRect oldRect;
    QRect newRect;

    bool wasDetermined = widget && widget->property("_qtn_isDetermined").toBool();
    if (widget && !wasDetermined && bAnimation)
    {
        QWidget* w = const_cast<QWidget*> (widget);
        int oldState = w->property("_q_stylestate").toInt();
        oldRect = w->property("_q_stylerect").toRect();
        newRect = w->rect();

        w->setProperty("_q_stylestate", static_cast<int>(option->state));
        w->setProperty("_q_stylerect", w->rect());
        w->setProperty("_qtn_isDetermined", true);

        // Determine the animated transition
        bool doTransition = ((state & QStyle::State_Sunken) != (oldState & QStyle::State_Sunken) ||
            (state & QStyle::State_On) != (oldState & QStyle::State_On) ||
            (state & QStyle::State_MouseOver) != (oldState & QStyle::State_MouseOver));

        QStyleOption* thisOption = const_cast<QStyleOption*>(option);
        CommonStylePrivate* thisStyle = const_cast<CommonStylePrivate*>(this);

        if (oldRect != newRect ||
            (state & QStyle::State_Enabled) != (oldState & QStyle::State_Enabled) ||
            (state & QStyle::State_Active) != (oldState & QStyle::State_Active))
            thisStyle->stopAnimation(widget);

        if (doTransition)
        {
            QRect rect(option->rect);
            QImage startImage(rect.size(), QImage::Format_ARGB32_Premultiplied);
            QImage endImage(rect.size(), QImage::Format_ARGB32_Premultiplied);

            QtitanAnimation* anim = widgetAnimation(widget);

            startImage.fill(0);
            QtitanTransition* transition = new QtitanTransition;
            transition->setWidget(w);

            thisOption->rect.setRect(0, 0, rect.width(), rect.height());

            QPainter startPainter(&startImage);
            if (!anim)
            {
                thisOption->state = static_cast<QStyle::State>(oldState);
                switch (type)
                {
                    case tp_PrimitiveElement:
                        p.drawPrimitive(static_cast<QStyle::PrimitiveElement>(nPrim), option, &startPainter, widget);
                        break;
                    case tp_ControlElement:
                        p.drawControl(static_cast<QStyle::ControlElement>(nPrim), option, &startPainter, widget);
                        break;
                    case tp_ComplexControl:
                    {
                        if (const QStyleOptionComplex* pOptionComplex = qstyleoption_cast<const QStyleOptionComplex*>(option))
                            p.drawComplexControl(static_cast<QStyle::ComplexControl>(nPrim), pOptionComplex, &startPainter, widget);
                        break;
                    }
                }
            }
            else
            {
                anim->paint(&startPainter, option);
                thisStyle->stopAnimation(widget);
            }

            transition->setStartImage(startImage);
            endImage.fill(0);

            thisOption->state = static_cast<QStyle::State>(state);

            QPainter endPainter(&endImage);
            switch (type)
            {
                case tp_PrimitiveElement:
                    p.drawPrimitive(static_cast<QStyle::PrimitiveElement>(nPrim), option, &endPainter, widget);
                    break;
                case tp_ControlElement:
                    p.drawControl(static_cast<QStyle::ControlElement>(nPrim), option, &endPainter, widget);
                    break;
                case tp_ComplexControl:
                {
                    if (const QStyleOptionComplex* pOptionComplex = qstyleoption_cast<const QStyleOptionComplex*>(option))
                        p.drawComplexControl(static_cast<QStyle::ComplexControl>(nPrim), pOptionComplex, &endPainter, widget);
                    break;
                }
            }
            thisOption->rect = rect;
            transition->setEndImage(endImage);

            if (option->state & QStyle::State_MouseOver || option->state & QStyle::State_Sunken)
                transition->setDuration(iDurationOver);
            else
                transition->setDuration(300);

            thisStyle->startAnimation(transition);
            transition->setStartTime(QTime::currentTime());
        }
        w->setProperty("_qtn_isDetermined", false);
    }

    if (QtitanAnimation* anim = widgetAnimation(widget))
    {
        anim->paint(painter, option);
        return true;
    }
    return false;
}

/*! \internal */
void CommonStylePrivate::doTimer()
{
    for (int i = m_animations.size() - 1; i >= 0; --i)
    {
        QtitanAnimation* pItem = m_animations[i];
        QWidget* pWidget = pItem->widget();
        if (pWidget)
            pWidget->update();

        if (!pWidget ||
            !pWidget->isEnabled() ||
            !pWidget->isVisible() ||
            pWidget->window()->isMinimized() ||
            !pItem->running())
        {
            QtitanAnimation* a = m_animations.takeAt(i);
            delete a;
        }
    }

    if (m_animations.size() == 0 && m_animationTimer.isActive())
        m_animationTimer.stop();
}

/*! \internal */
void CommonStylePrivate::startAnimation(QtitanAnimation* t)
{
    QTN_P(CommonStyle);
    stopAnimation(t->widget());
    m_animations.append(t);
    if (m_animations.size() > 0 && !m_animationTimer.isActive())
        m_animationTimer.start(45, &p);
}

/*! \internal */
void CommonStylePrivate::stopAnimation(const QWidget* w) 
{
    for (int i = m_animations.size() - 1; i >= 0; --i)
    {
        if (m_animations[i]->widget() == w)
        {
            QtitanAnimation* a = m_animations.takeAt(i);
            delete a;
            break;
        }
    }
}

/*! \internal */
QtitanAnimation* CommonStylePrivate::widgetAnimation(const QWidget* widget) const
{
    if (!widget)
        return Q_NULL;
    foreach(QtitanAnimation* a, m_animations)
    {
        if (a->widget() == widget)
            return a;
    }
    return Q_NULL;
}

/*! \internal */
bool CommonStylePrivate::widgetStyleSupport(const QWidget* widget) const
{
    if (widget == Q_NULL)
        return true;

    if (m_ignoreScrollBars && widget->inherits("QScrollBar"))
        return false;

    if (m_ignoreDialogs && (widget->inherits("QDialog") || QtnPrivate::qtn_isAncestor(widget, "QDialog")))
        return false;
    return true;
}

/*! \internal */
bool CommonStylePrivate::showToolTip(const QPoint& pos, QWidget* widget)
{
    if (QtnPrivate::qtn_isAncestor(widget, QTITAN_META_CLASSNAME("RibbonBar")))
        return false;

    QPoint p = pos;
    p += QPoint(2,
#ifdef Q_WS_WIN
        21
#else
        16
#endif
    );

    int posX = p.x();
    int posY = p.y();
    QString strTipText = widget->toolTip();
    if (!strTipText.isEmpty())
    {
        QPoint globalPos(posX, posY);
        ToolTip::showToolTip(globalPos, QString(), strTipText, QIcon(), widget);
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::canWordWrap(const QRect& rect, const QFontMetrics& fm, const QString& str, int* heightLine) const
{
    int alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextHideMnemonic;
    QSize sz = fm.boundingRect(rect, alignment, str).size();
    const int heightOneLine = fm.height();
    if (heightOneLine)
        *heightLine = heightOneLine;
    int count = qRound(static_cast<qreal>(sz.height()) / static_cast<qreal>(heightOneLine));
    return count == 2;
}

/*! \internal */
QBrush CommonStylePrivate::brushSubControl(const QString& className, const QString& subControlId, 
    QPalette::ColorGroup group, QPalette::ColorRole role) const
{
    return m_config.brushSubControl(className, subControlId, group, role);
}

/*! \internal */
QColor CommonStylePrivate::colorSubControl(const QString& className, const QString& subControlId,
    QPalette::ColorGroup group, QPalette::ColorRole role) const
{
    return m_config.colorSubControl(className, subControlId, group, role);
}

/*! \internal */
QPixmap CommonStylePrivate::scale(int w, int h, const QPixmap &pixmap, const QMargins& margins)
{
    QPixmap result(w, h);
    const QColor transparent(0, 0, 0, 0);
    result.fill(transparent);
    QPainter p(&result);
    bool wasSmoothPixmapTransform = p.renderHints() & QPainter::SmoothPixmapTransform;
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    qDrawBorderPixmap(&p, result.rect(), margins, pixmap, pixmap.rect(), margins/*, QTileRules()*/);

    p.setRenderHint(QPainter::SmoothPixmapTransform, wasSmoothPixmapTransform);

    return result;
}

/*! \internal */
QRect CommonStylePrivate::sourceRectImage(const QRect& rcSrc, int state, int count)
{
    QRect rcImage(0, 0, rcSrc.width(), rcSrc.height() / count);
    rcImage.translate(0, state * rcImage.height());
    return rcImage;
}

/*! \internal */
QPixmap CommonStylePrivate::getCachedPixmap(const QString& className, const QString& image, const QSize& size, 
    const QMargins& margins, const QColor& clrTransparent) const
{
    const QString sizeString = QString::number(size.width()) % QLatin1Char('*') % QString::number(size.height());
    const QString key = className % image % QLatin1Char('@') % sizeString;
    QPixmap result;
    if (!QPixmapCache::find(key, &result))
    {
        QString imageName = widgetPixmapName(className, image);
        if (imageName.isEmpty())
            return result;

        QPixmap source(imageName);
        if (clrTransparent.isValid())
        {
            QImage img = source.toImage();
            img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
            source = QPixmap::fromImage(img);
        }

        QSize sizeImage = size;
        if (sizeImage.isNull())
            sizeImage = source.size();

        result = CommonStylePrivate::scale(sizeImage.width(), sizeImage.height(), source, margins);
        QPixmapCache::insert(key, result);
    }
    return result;
}

/*! \internal */
QPixmap CommonStylePrivate::getCachedPixmap(const QString& className, const QString& image, const QSize& size, 
    int state, int count, const QMargins& margins, const QColor& clrTransparent) const
{
    const QString sizeString = QString::number(size.width()) % QLatin1Char('*') % QString::number(size.height());
    const QString key = className % image % QLatin1Char('@')
        % QString::number(state) % QString::number(state) % QLatin1Char('@') % sizeString;
    QPixmap result;
    if (!QPixmapCache::find(key, &result))
    {
        QString imageName = widgetPixmapName(className, image);
        if (imageName.isEmpty())
            return result;

        QPixmap source(imageName);
        QRect rcSrc = CommonStylePrivate::sourceRectImage(source.rect(), state, count);
        source = source.copy(rcSrc);

        if (clrTransparent.isValid())
        {
            QImage img = source.toImage();
            img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
            source = QPixmap::fromImage(img);
        }
        QSize sz = size;
        if (sz.isNull() || !sz.isValid())
            sz = source.size();
        result = CommonStylePrivate::scale(sz.width(), sz.height(), source, margins);
        QPixmapCache::insert(key, result);
    }
    return result;
}

/*! \internal */
bool CommonStylePrivate::drawWidgetPixmap(const QString& className, const QString& image, const QRect& rect, 
    QPainter* painter, const QMargins& margins, const QColor& clrTransparent) const
{
    const QPixmap pix = getCachedPixmap(className, image, rect.size(), margins, clrTransparent);
    if (!pix.isNull())
    {
        painter->drawPixmap(rect, pix);
        return true;
    }
    return false;
}

/*! \internal */
bool CommonStylePrivate::drawWidgetPixmap(const QString& className, const QString& image, const QRect& rect, 
    QPainter* painter, int state, int count, const QMargins& margins, const QColor& clrTransparent) const
{
    const QPixmap pix = getCachedPixmap(className, image, rect.size(), state, count, margins, clrTransparent);
    if (!pix.isNull())
    {
        painter->drawPixmap(rect, pix);
        return true;
    }
    return false;
}

static QSizeF qtn_viewItemTextLayout(QTextLayout &textLayout, int lineWidth, int maxHeight = -1, int *lastVisibleLine = nullptr)
{
    if (lastVisibleLine)
        *lastVisibleLine = -1;
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    int i = 0;
    while (true) 
    {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        if (maxHeight > 0 && lastVisibleLine && height + line.height() > maxHeight) {
            const QTextLine nextLine = textLayout.createLine();
            *lastVisibleLine = nextLine.isValid() ? i : -1;
            break;
        }
        ++i;
    }
    textLayout.endLayout();
    return QSizeF(widthUsed, height);
}

/*! \internal */
QString CommonStylePrivate::calculateElidedText(const QString& text, const QTextOption& textOption, const QFont& font, const QRect& textRect, const Qt::Alignment valign,
    Qt::TextElideMode textElideMode, int flags, bool lastVisibleLineShouldBeElided, QPointF* paintStartPosition) const
{
    QTextLayout textLayout(text, font);
    textLayout.setTextOption(textOption);

    const bool vAlignmentOptimization = paintStartPosition && valign.testFlag(Qt::AlignVCenter);

    int lastVisibleLine = -1;
    qtn_viewItemTextLayout(textLayout, textRect.width(), vAlignmentOptimization ? textRect.height() : -1, &lastVisibleLine);

    const QRectF boundingRect = textLayout.boundingRect();
    const QRect layoutRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, valign,
        boundingRect.size().toSize(), textRect);

    if (paintStartPosition)
        *paintStartPosition = QPointF(textRect.x(), layoutRect.top());

    QString ret;
    qreal height = 0;
    const int lineCount = textLayout.lineCount();
    for (int i = 0; i < lineCount; ++i) 
    {
        const QTextLine line = textLayout.lineAt(i);
        height += line.height();

        if (height + layoutRect.top() <= textRect.top()) 
        {
            if (paintStartPosition)
                paintStartPosition->ry() += line.height();
            continue;
        }

        const int start = line.textStart();
        const int length = line.textLength();
        const bool drawElided = line.naturalTextWidth() > textRect.width();
        bool elideLastVisibleLine = lastVisibleLine == i;
        if (!drawElided && i + 1 < lineCount && lastVisibleLineShouldBeElided) 
        {
            const QTextLine nextLine = textLayout.lineAt(i + 1);
            const int nextHeight = qRound(height + nextLine.height() / 2.0);
            if (nextHeight + layoutRect.top() > textRect.height() + textRect.top())
                elideLastVisibleLine = true;
        }

        QString text = textLayout.text().mid(start, length);
        if (drawElided || elideLastVisibleLine) 
        {
            if (elideLastVisibleLine) 
            {
                if (text.endsWith(QChar::LineSeparator))
                    text.chop(1);
                text += QChar(0x2026);
            }

            QFontMetrics fm(font);
            ret += fm.elidedText(text, textElideMode, textRect.width(), flags);

            if (i < lineCount - 1 &&
                !ret.endsWith(QChar::LineSeparator))
                ret += QChar::LineSeparator;
        }
        else 
            ret += text;

        if ((height + layoutRect.top() >= textRect.bottom()) ||
            (lastVisibleLine >= 0 && lastVisibleLine == i))
            break;
    }
    return ret;
}

/*! \internal */
QString CommonStylePrivate::toolButtonElideText(const QStyleOptionToolButton *option, const QRect &textRect, int flags) const
{
    if (qtn_horizontalAdvanceFont(option->fontMetrics, option->text) <= textRect.width())
        return option->text;
    QString text = option->text;
    text.replace(QLatin1Char('\n'), QChar::LineSeparator);
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::ManualWrap);
    textOption.setTextDirection(option->direction);
    return calculateElidedText(text, textOption, option->font, textRect, Qt::AlignTop, Qt::ElideMiddle, flags, false, nullptr);
}

/*! \internal */
QColor CommonStylePrivate::darkBackground(const QPalette& pal) const
{
    return pal.window().color();
}

/*! \internal */
QColor CommonStylePrivate::darkBackgroundLight(const QPalette& pal) const
{
    return pal.color(QPalette::Base).lighter(99);
}

/*! \internal */
QColor CommonStylePrivate::outlineMidlight(const QPalette& pal) const
{
    return pal.brush(QPalette::Midlight).color().darker(105);
}

/*! \internal */
QColor CommonStylePrivate::outlineLight(const QPalette& pal) const
{
    return pal.brush(QPalette::Light).color();
}

/*! \internal */
QColor CommonStylePrivate::textGalleryItem(const QPalette& pal) const
{
    return pal.brush(QPalette::Window).color().darker();
}

/*! \internal */
QColor CommonStylePrivate::buttonHighlightColor(const QPalette& pal, bool highlightDn, bool border, const QWidget* wd) const
{
    Q_UNUSED(wd)
    Q_UNUSED(pal)
    QTN_P(const CommonStyle);
    QImage image(3, 3, QImage::Format_ARGB32_Premultiplied);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    image.fill(Qt::white);
#else
    image.fill(Qt::transparent);
#endif
    QPainter painter(&image);
    QStyleOption opt;
    opt.state |= QStyle::State_Enabled;
    if (highlightDn)
        opt.state |= QStyle::State_Sunken;
    else
        opt.state |= QStyle::State_MouseOver;
    opt.rect = QRect(QPoint(0, 0), QSize(3, 3));
    p.proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &painter, Q_NULL);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    if (border)
        return image.pixelColor(2, 2);
    QPoint pnt = opt.rect.center();
    return image.pixelColor(pnt.x(), pnt.y());
#else
    if (border)
        return image.pixel(2, 2);
    QPoint pnt = opt.rect.center();
    return image.pixel(pnt.x(), pnt.y());
#endif
}

/*! \internal */
QColor CommonStylePrivate::titleBarWindowColor(const QPalette& pal, bool isActive, const QWidget* wd) const
{ Q_UNUSED(wd); return pal.color(isActive ? QPalette::Active : QPalette::Inactive, QPalette::Window); }

/*! \internal */
QColor CommonStylePrivate::titleBarFrameColor(const QPalette& pal, bool isActive, const QWidget* wd) const
{ Q_UNUSED(wd); return pal.color(isActive ? QPalette::Active : QPalette::Inactive, QPalette::Shadow); }

#if defined(Q_OS_WASM) || defined(Q_OS_IOS)
int CommonStylePrivate::pixelMetricFromSystemDp(QStyle::PixelMetric pm, const QStyleOption* option, const QWidget* widget)
{
    Q_UNUSED(option)
        Q_UNUSED(widget)
        switch (pm)
        {
        case QStyle::PM_TitleBarHeight:
            return CommonStylePrivate::dpiScaled(24, widget);
        default:
            break;
        }
    return CommonStylePrivate::InvalidMetric;
}
#endif

/*! \internal */
int CommonStylePrivate::colorBrightness(const QColor& clr) const
{
    int r = clr.red();
    int g = clr.green();
    int b = clr.blue();
    int val = (((r * 299) + (g * 587) + (b * 114)) / 1000);
    return val;
}

/*! \internal */
bool CommonStylePrivate::isDarkColor(const QColor& clr) const
{
    return colorBrightness(clr) < 143;
}

/*!
\class CommonStyle
\inmodule QtitanBase
\brief CommonStyle is an intermediate class between Qt styles and Qtitan styles. 
It implements a set of basic mechanisms for the work for styles used in the components of Qtitan. 
It is used for working with the palette, reading XML config files, basic functions and settings.
*/
/*!
Constuctor of CommonStyle class. You can't use this class directly. 
*/
CommonStyle::CommonStyle(QStyle* style)
    : QProxyStyle(style)
{
    QTN_INIT_PRIVATE(CommonStyle)
    QTN_D(CommonStyle);
    d.init();
#ifdef QTITAN_DEMO
    START_DEMO
#endif
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
CommonStyle::CommonStyle(const QString& key)
    : QProxyStyle(key)
{
    QTN_INIT_PRIVATE(CommonStyle)
    QTN_D(CommonStyle);
    d.init();
}
#endif

CommonStyle::CommonStyle(CommonStylePrivate& d)
{
    qtn_d_ptr = &d; 
    qtn_d_ptr->setPublic(this);
    d.init();
}

CommonStyle::~CommonStyle()
{
    QTN_FINI_PRIVATE()
}

/*!
\property CommonStyle::animationEnabled
Sets the animation for widgets and controls when mouse over the elements to \a enable state. By default the animation is enabled.
 */
void CommonStyle::setAnimationEnabled(bool enable/*= true*/)
{
    QTN_D(CommonStyle);
    d.m_animationEnabled = enable;
}

/*!
Returns animation state for Microsoft Office's controls when mouse over the elements.
 */
bool CommonStyle::isAnimationEnabled() const
{
    QTN_D(const CommonStyle);
    return d.m_animationEnabled;
}

/*!
\property CommonStyle::ignoreDialogs
Sets \a ignore dialog flag. If this flag is true that can ignore the current Ribbon or Office Style for dialogs and standard Qt style will be used for dialog. By default this flag is false.
*/
void CommonStyle::setDialogsIgnored(bool ignore)
{
    QTN_D(CommonStyle);
    if (d.m_ignoreDialogs != ignore)
    {
        d.beginUpdate();
        d.m_ignoreDialogs = ignore;
        d.endUpdate();
    }
}

/*!
Returns the ignore dialog flag.
*/
bool CommonStyle::isDialogsIgnored() const
{
    QTN_D(const CommonStyle);
    return d.m_ignoreDialogs;
}

/*!
\property CommonStyle::ignoreScrollBars
Sets ignore scroll bars flag. It is usefull if necessary to create an application more natively. By default this value is false.
*/
void CommonStyle::setScrollBarsIgnored(bool ignore /*= false*/)
{
    QTN_D(CommonStyle);
    if (d.m_ignoreScrollBars != ignore)
    {
        d.beginUpdate();
        d.m_ignoreScrollBars = ignore;
        d.endUpdate();
    }
}

/*!
Returns the value of ignore scroll bars flag.
*/
bool CommonStyle::isScrollBarsIgnored() const
{
    QTN_D(const CommonStyle);
    return d.m_ignoreScrollBars;
}

CommonStyle* CommonStyle::ensureStyle()
{
    QStyle* style = QApplication::style();
    CommonStyle* commonStyle = qobject_cast<CommonStyle*>(style);
    if (commonStyle != Q_NULL)
        return commonStyle;
    QString className = QLatin1String(style->metaObject()->className());
    if (className == QStringLiteral("QStyleSheetStyle"))
    {
#ifdef QTN_PRIVATE_HEADERS
        commonStyle = qobject_cast<CommonStyle*>(static_cast<QStyleSheetStyle *>(style)->base);
        if (commonStyle == Q_NULL)
        {
            commonStyle = new CommonStyle(static_cast<QStyleSheetStyle*>(style)->base);
            static_cast<QStyleSheetStyle*>(style)->base = commonStyle;
            commonStyle->setParent(style);
        }
        return commonStyle;
        
#else
            return Q_NULL;
#endif
    }
    QProxyStyle* proxyStyle = qobject_cast<QProxyStyle *>(style);
    if (proxyStyle != Q_NULL)
    {
        commonStyle = qobject_cast<CommonStyle*>(proxyStyle->baseStyle());
        if (commonStyle != Q_NULL)
            return commonStyle;
    }
    commonStyle = new CommonStyle(style);
    QApplication::setStyle(commonStyle);
    return commonStyle;
}

/*! \reimp */
QPalette CommonStyle::standardPalette() const
{
    return QProxyStyle::standardPalette();
}

/*! \reimp */
void CommonStyle::polish(QPalette& palette)
{
    QTN_D(CommonStyle);
    QProxyStyle::polish(palette);
    d.m_systemPalette = palette;
    d.initStandardPalette(palette);
}

/*! \reimp */
void CommonStyle::polish(QApplication* app)
{
    QTN_D(CommonStyle);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    bool isQtitanStyle = d.isQtitanStyle();
    bool application = app->desktopSettingsAware();
    if (isQtitanStyle) app->setDesktopSettingsAware(false);
    QProxyStyle::polish(app);
    if (isQtitanStyle) app->setDesktopSettingsAware(application);
#else
    QProxyStyle::polish(app);
#endif
#if 0 //TODO
    QTN_D(CommonStyle);
    d.beginUpdate(false);
    d.endUpdate(false);
#endif
    app->setFont(d.getWidgetFont(
        QLatin1String(QTITAN_META_CLASSNAME("RibbonBar"))), QTITAN_META_CLASSNAME("RibbonBar"));
    app->setFont(d.getWidgetFont(
        QLatin1String(QTITAN_META_CLASSNAME("RibbonStatusBar"))), QTITAN_META_CLASSNAME("RibbonStatusBar"));
    app->setFont(d.getWidgetFont(
        QLatin1String(QTITAN_META_CLASSNAME("RibbonBackstageView"))), QTITAN_META_CLASSNAME("RibbonBackstageView"));

    app->installEventFilter(this);
}

/*! \reimp */
void CommonStyle::unpolish(QApplication* app)
{
    QProxyStyle::unpolish(app);
    if (app != Q_NULL)
        app->removeEventFilter(this);
}

static QPalette qtn_palette_add(const QPalette& originalPalette, const QPalette& otherPalette)
{
    QPalette palette = originalPalette;

    for (int role = 0; role < (int)QPalette::NColorRoles; ++role)
    {
        for (int group = 0; group < (int)QPalette::NColorGroups; ++group)
        {
            QPalette::ColorGroup cg = static_cast<QPalette::ColorGroup>(group);
            QPalette::ColorRole cr = static_cast<QPalette::ColorRole>(role);
            if (!originalPalette.isBrushSet(cg, cr) && otherPalette.isBrushSet(cg, cr))
            {
                QBrush brush = otherPalette.brush(cg, cr);
                QPalette_setBrush(palette, cg, cr, brush);
            }
        }
    }
    return palette;
}

/*! \reimp */
void CommonStyle::polish(QWidget* widget)
{
    QTN_D(CommonStyle);
    QProxyStyle::polish(widget);
    if (widget == Q_NULL)
        return;
    d.stopAnimation(widget);
#ifdef Q_OS_WASM
    if (qobject_cast<ScrollWidgetBar *>(widget) != Q_NULL)
    {
        //Wasm compiler has a bug - the class is not exported if it is not referenced inside the shared library,
        //therefore we do it forcibly.
    }
#endif
    if (widget->inherits("QTipLabel"))
    {
        widget->setPalette(QApplication::palette());
        return;
    }

    if (widget->inherits("QToolBoxButton"))
    {
        //Qt-bug: Detach palette.
        QPalette p = widget->palette();
        QColor color = p.color(QPalette::Active, QPalette::Window);
        color.setAlpha(254);
        p.setColor(QPalette::Active, QPalette::Window, color);
        widget->setPalette(p);
    }

    if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
    {
        widget->setBackgroundRole(d.isQtitanStyle() ? QPalette::Window : QPalette::Base);
    }
    else if (widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemButton")))
        widget->setForegroundRole(d.isQtitanStyle() ? QPalette::ButtonText : QPalette::BrightText);

#if 0
    QFont font = d.getWidgetFont(widget);
    if (!font.isCopyOf(QFont()))
        widget->setFont(font);
#endif

    QPalette palette = d.getWidgetPalette(widget);
    if (QPalette_resolveMask(palette) != 0)
    {
#if 0
        uint resolve_mask = widget->palette().resolve();
        resolve_mask |= palette.resolve();
        palette = widget->palette().resolve(palette);
        palette.resolve(resolve_mask);
        widget->setPalette(palette);
#else       
        widget->setPalette(qtn_palette_add(widget->palette(), palette));
#endif
    }
    d.m_config.updateWidgetAttribute(widget);
}

/*! \reimp */
void CommonStyle::unpolish(QWidget* widget)
{
    QTN_D(CommonStyle);
    QProxyStyle::unpolish(widget);
    if (widget == Q_NULL)
        return;
    d.stopAnimation(widget);

#if 0
    QFont font = d.getWidgetFont(widget);
    if (!font.isCopyOf(QFont()))
        widget->setFont(QFont()); //Clear font setting if it was initiated before.
#endif

    QPalette original_palette = d.getWidgetPalette(widget);

    if (QPalette_resolveMask(original_palette) != 0)
    {
        QPalette palette = widget->palette();
        int mask = QPalette_resolveMask(palette);
        {
            for (int role = 0; role < (int)QPalette::NColorRoles; ++role)
                for (int group = 0; group < (int)QPalette::NColorGroups; ++group)
                    if (original_palette.isBrushSet((QPalette::ColorGroup)group, (QPalette::ColorRole)role))
                    {
                        if (original_palette.brush((QPalette::ColorGroup)group, (QPalette::ColorRole)role) ==
                            palette.brush((QPalette::ColorGroup)group, (QPalette::ColorRole)role))
                            mask &= ~(1 << role);
                    }
        }
        QPalette_setResolveMask(palette, mask);
        widget->setPalette(palette);
    }
    if (!d.isQtitanStyle())
    {
        if (widget->inherits(QTITAN_META_CLASSNAME("DockDocumentLayoutPanel")) ||
            widget->inherits(QTITAN_META_CLASSNAME("DockCentralPanel")))
            widget->setAutoFillBackground(false);
    }
}

/*! \reimp */
void CommonStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(w))
    {
        QProxyStyle::drawPrimitive(pe, opt, p, w);
        return;
    }

    bool draw = false;
    switch (static_cast<CommonStyle::PrimitiveElementEx>(pe))
    {
        case CommonStyle::PE_PanelRibbonBar:        draw = d.drawRibbonBar(opt, p, w); break;
        case CommonStyle::PE_RibbonFrameGroups:     draw = d.drawRibbonFrameGroups(opt, p, w); break;
        case CommonStyle::PE_RibbonFrameGroup:
            draw = d.paintAnimation(CommonStylePrivate::tp_PrimitiveElement, PE_RibbonFrameGroup, opt, p, w, 300); if (draw) break;
            draw = d.drawRibbonFrameGroup(opt, p, w); break;
        case CommonStyle::PE_RibbonFrameGallery:    draw = d.drawRibbonFrameGallery(opt, p, w); break;
        case CommonStyle::PE_RibbonSysFrameMenu:    draw = d.drawRibbonSysFrameMenu(opt, p, w); break;
        case CommonStyle::PE_RibbonSysFramePagePopupList: draw = d.drawRibbonSysFramePagePopupList(opt, p, w); break;
        case CommonStyle::PE_RibbonSysFramePagePopup:    draw = d.drawRibbonSysFramePagePopup(opt, p, w); break;
        case CommonStyle::PE_RibbonPanelButton:
            draw = d.paintAnimation(CommonStylePrivate::tp_PrimitiveElement, PE_RibbonPanelButton, opt, p, w); if (draw) break;
            draw = d.drawRibbonPanelButton(opt, p, w); break;
        case CommonStyle::PE_RibbonOptionButton:         draw = d.drawRibbonOptionButton(opt, p, w); break;
        case CommonStyle::PE_RibbonQuickAccessButton:    draw = d.drawRibbonQuickAccessButton(opt, p, w); break;
        case CommonStyle::PE_RibbonBackstageFrame:       draw = d.drawRibbonBackstageFrame(opt, p, w); break;
        case CommonStyle::PE_RibbonBackstagePanelButton: draw = d.drawRibbonBackstagePanelButton(opt, p, w); break;
        case CommonStyle::PE_RibbonBackstageCloseButton: draw = d.drawRibbonBackstageCloseButton(opt, p, w); break;
        case CommonStyle::PE_RibbonGroupScrollButton:    draw = d.drawRibbonGroupScrollButton(opt, p, w); break;
        case CommonStyle::PE_RibbonPopupBarButton:       draw = d.drawRibbonPopupBarButton(opt, p, w); break;
        case CommonStyle::PE_DockAutoHideFrameWindow:    draw = d.drawDockAutoHideFrameWindow(opt, p, w); break;
        case CommonStyle::PE_RibbonSliderButton:         draw = d.drawRibbonSliderButton(opt, p, w); break;
        case CommonStyle::PE_RibbonKeyTip:               draw = d.drawKeyTip(opt, p, w); break;
        case CommonStyle::PE_DockMarker:                 draw = d.drawDockMarker(opt, p, w); break;
        case CommonStyle::PE_DockWidgetPhantom:          draw = d.drawDockWidgetPhantom(opt, p, w); break;
        case CommonStyle::PE_Workspace:                  draw = d.drawWorkspace(opt, p, w); break;
        default: draw = false; break;
    }

    if (!draw)
    {
        switch (pe)
        {
            case PE_Frame:                  draw = d.drawFrame(opt, p, w); break;
            case PE_FrameTabWidget:
            case PE_FrameWindow:            draw = d.drawFrameWindow(opt, p, w); break;
            case PE_FrameDockWidget:        draw = d.drawFrameDockWidget(opt, p, w); break;
            case PE_FrameMenu:              draw = d.drawFrameMenu(opt, p, w); break;
            case PE_PanelMenu:              draw = d.drawPanelMenu(opt, p, w); break;
            case PE_IndicatorArrowUp:
            case PE_IndicatorArrowDown:
            case PE_IndicatorArrowRight:
            case PE_IndicatorArrowLeft:     draw = d.drawIndicatorArrow(pe, opt, p, w); break;
            case PE_IndicatorMenuCheckMark: draw = d.drawIndicatorMenuCheckMark(opt, p, w); break;
            case PE_IndicatorButtonDropDown: draw = d.drawIndicatorButtonDropDown(opt, p, w); break;
            case PE_IndicatorTabClose:      draw = d.drawIndicatorTabClose(opt, p, w); break;
            case PE_PanelTipLabel:          draw = d.drawPanelTipLabel(opt, p, w); break;
            case PE_FrameGroupBox:          draw = d.drawFrameGroupBox(opt, p, w); break;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            case PE_IndicatorViewItemCheck: draw = d.drawIndicatorItemCheck(opt, p, w); break;
#endif
            case PE_IndicatorCheckBox:
                draw = d.paintAnimation(CommonStylePrivate::tp_PrimitiveElement, PE_IndicatorCheckBox, opt, p, w); if (draw) break;
                draw = d.drawIndicatorCheckBox(opt, p, w); break;
            case PE_IndicatorRadioButton:   
                draw = d.paintAnimation(CommonStylePrivate::tp_PrimitiveElement, PE_IndicatorRadioButton, opt, p, w); if (draw) break;
                draw = d.drawIndicatorRadioButton(opt, p, w); break;
            case PE_IndicatorToolBarHandle: draw = d.drawIndicatorToolBarHandle(opt, p, w); break;
            case PE_IndicatorToolBarSeparator: draw = d.drawIndicatorToolBarSeparator(opt, p, w); break;
            case PE_IndicatorHeaderArrow:   draw = d.drawIndicatorHeaderArrow(opt, p, w); break;
            case PE_IndicatorBranch:        draw = d.drawIndicatorBranch(opt, p, w); break;
            case PE_FrameFocusRect:         draw = d.drawFrameFocusRect(opt, p, w); break;
            case PE_PanelButtonTool:        draw = d.drawPanelButtonTool(opt, p, w); break;
            case PE_PanelButtonCommand:     draw = d.drawPanelButtonCommand(opt, p, w); break;
            case PE_FrameLineEdit:          draw = d.drawFrameLineEdit(opt, p, w); break;
            case PE_PanelLineEdit:          draw = d.drawPanelLineEdit(opt, p, w); break;
            case PE_FrameStatusBarItem:     draw = d.drawFrameStatusBarItem(opt, p, w); break;
            case PE_PanelStatusBar:         draw = d.drawPanelStatusBar(opt, p, w); break;
            case PE_PanelButtonBevel:       draw = d.drawPanelButtonBevel(opt, p, w); break;
            case PE_FrameTabBarBase:        draw = d.drawFrameTabBarBase(opt, p, w);  break;   // draw a line along the tabs
            case PE_PanelItemViewItem:      draw = d.drawPanelItemViewItem(opt, p, w);  break;
            default: draw = false; break;
        }
    }
    if (!draw)
    {
        if (d.isQtitanStyle())
            QCommonStyle::drawPrimitive(pe, opt, p, w);
        else
            QProxyStyle::drawPrimitive(pe, opt, p, w);
    }
}

/*! \reimp */
void CommonStyle::drawControl(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(w))
    {
        QProxyStyle::drawControl(element, opt, p, w);
        return;
    }

    bool draw = false;
    switch (static_cast<CommonStyle::ControlElementEx>(element))
    {
        case CommonStyle::CE_RibbonTabShapeLabel: draw = d.drawRibbonTabShapeLabel(opt, p, w); break;
        case CommonStyle::CE_RibbonTab:
            draw = d.paintAnimation(CommonStylePrivate::tp_ControlElement, static_cast<int>(CE_RibbonTab), opt, p, w, 300); if (draw) break;
            draw = d.drawRibbonTab(opt, p, w); break;
#if 0
        case CommonStyle::CE_RibbonTabBar:        draw = d.drawRibbonTabBar(opt, p, w); break;
#endif
        case CommonStyle::CE_RibbonGroupLabel:    draw = d.drawRibbonGroupLabel(opt, p, w); break;
        case CommonStyle::CE_RibbonSysMenuItem:   draw = d.drawRibbonSysMenuItem(opt, p, w); break;
        case CommonStyle::CE_RibbonSysRecentFileItem: draw = d.drawRibbonSysRecentFileItem(opt, p, w); break;
        case CommonStyle::CE_RibbonButtonLabel:   draw = d.drawRibbonButtonLabel(opt, p, w); break;
        case CommonStyle::CE_RibbonGalleryItem:   draw = d.drawRibbonGalleryItem(opt, p, w); break;
        case CommonStyle::CE_RibbonBackstageMenuItem: draw = d.drawRibbonBackstageMenuItem(opt, p, w); break;
        case CommonStyle::CE_RibbonBackstageButtonLabel: draw = d.drawRibbonBackstageButtonLabel(opt, p, w); break;
        case CommonStyle::CE_RibbonReducedGroup:  draw = d.drawRibbonReducedGroup(opt, p, w); break;
        case CommonStyle::CE_RibbonGripBar:       draw = d.drawOfficePopupSizeGrip(opt, p, w); break;
        case CommonStyle::CE_NavigationBackButton: draw = d.drawNavigationBackButton(opt, p, w); break;
    }
    if (!draw)
    {
        switch (element)
        {
            case CE_MenuBarEmptyArea:   draw = d.drawMenuBarEmptyArea(opt, p, w); break;
            case CE_ShapedFrame:        draw = d.drawShapedFrame(opt, p, w); break;
            case CE_MenuBarItem:        draw = d.drawMenuBarItem(opt, p, w); break;
            case CE_MenuItem:           draw = d.drawMenuItem(opt, p, w); break;
            case CE_ScrollBarSubLine:
            case CE_ScrollBarAddLine:   draw = d.drawScrollBarAddSubLine(element, opt, p, w); break;
            case CE_ScrollBarAddPage:
            case CE_ScrollBarSubPage:   draw = d.drawScrollBarAddSubPage(element, opt, p, w); break;
            case CE_ScrollBarSlider:    draw = d.drawScrollBarSlider(opt, p, w); break;
            case CE_TabBarTabShape:     draw = d.drawTabBarTabShape(opt, p, w); break;
            case CE_Header:             draw = d.drawHeader(opt, p, w); break;
            case CE_HeaderSection:      draw = d.drawHeaderSection(opt, p, w); break;
            case CE_HeaderLabel:        draw = d.drawHeaderLabel(opt, p, w); break;
            case CE_DockWidgetTitle:    draw = d.drawDockWidgetTitle(opt, p, w); break;
            case CE_ToolBoxTabShape:    draw = d.drawToolBoxTabShape(opt, p, w); break;
            case CE_ToolButtonLabel:    draw = d.drawToolButtonLabel(opt, p, w); break;
            case CE_SizeGrip:           draw = d.drawSizeGrip(opt, p, w); break;
            case CE_ToolBar:            draw = d.drawToolBar(opt, p, w); break;
            case CE_TabBarTabLabel:     draw = d.drawTabBarTabLabel(opt, p, w); break;
            case CE_ItemViewItem:       draw = d.drawItemViewItem(opt, p, w); break;
            case CE_Splitter:           draw = d.drawSplitter(opt, p, w); break;
            case CE_ProgressBarContents:draw = d.drawProgressBarContents(opt, p, w); break;
            case CE_ProgressBarLabel:   draw = d.drawProgressBarLabel(opt, p, w); break;
            default:                    draw = false; break;
        }
    }
    if (!draw)
    {
        if (d.isQtitanStyle())
            QCommonStyle::drawControl(element, opt, p, w);
        else
            QProxyStyle::drawControl(element, opt, p, w);
    }
}

/*! \reimp */
void CommonStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(w))
    {
        QProxyStyle::drawComplexControl(cc, opt, p, w);
        return;
    }

    bool draw = false;
    switch (static_cast<CommonStyle::ComplexControlEx>(cc))
    {
        case CommonStyle::CC_RibbonFileButton:      draw = d.drawRibbonFileButton(opt, p, w); break;
        case CommonStyle::CC_RibbonButton:          draw = d.drawRibbonButton(opt, p, w); break;
        case CommonStyle::CC_ToggleSwitch:          draw = d.drawToggleSwitch(opt, p, w); break;
        case CommonStyle::CC_RibbonTitleBarWidget:  draw = d.drawRibbonTitleBarWidget(opt, p, w); break;
        case CommonStyle::CC_RibbonGalleryButton:   draw = d.drawRibbonGalleryButton(opt, p, w); break;
        case CommonStyle::CC_RibbonBackstageButton: draw = d.drawRibbonBackstageButton(opt, p, w); break;
    }
    if (!draw)
    {
        switch (cc)
        {
            case CC_TitleBar:    draw = d.drawTitleBar(opt, p, w); break;
            case CC_MdiControls: draw = d.drawMdiControls(opt, p, w); break;
            case CC_ComboBox:    draw = d.drawComboBox(opt, p, w); break;
            case CC_Slider:      draw = d.drawSlider(opt, p, w); break;
            case CC_SpinBox:     draw = d.drawSpinBox(opt, p, w); break;
            case CC_ToolButton:  draw = d.drawToolButton(opt, p, w); break;
            case CC_ScrollBar:   draw = d.drawScrollBar(opt, p, w); break;
            case CC_GroupBox:    draw = d.drawGroupBox(opt, p, w); break;
            default:             draw = false; break;
        }
    }
    if (!draw)
    {
        if (d.isQtitanStyle())
            QCommonStyle::drawComplexControl(cc, opt, p, w);
        else
            QProxyStyle::drawComplexControl(cc, opt, p, w);
    }
}

/*! \reimp */
QRect CommonStyle::itemTextRect(const QFontMetrics& fm, const QRect& r, int flags, bool enabled, const QString& text) const
{
    QTN_D(const CommonStyle);
    if (d.isQtitanStyle())
        return QCommonStyle::itemTextRect(fm, r, flags, enabled, text);
    else
        return QProxyStyle::itemTextRect(fm, r, flags, enabled, text);
}

/*! \reimp */
QRect CommonStyle::itemPixmapRect(const QRect& r, int flags, const QPixmap& pixmap) const
{
    QTN_D(const CommonStyle);
    if (d.isQtitanStyle())
        return QCommonStyle::itemPixmapRect(r, flags, pixmap);
    else
        return QProxyStyle::itemPixmapRect(r, flags, pixmap);
}

/*! \reimp */
QRect CommonStyle::subElementRect(QStyle::SubElement se, const QStyleOption* option, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    if (se == QStyle::SE_ComboBoxLayoutItem)
        se = QStyle::SE_ComboBoxLayoutItem;
    if (!d.widgetStyleSupport(widget))
        return QProxyStyle::subElementRect(se, option, widget);
    QRect retRect = d.ribbonSubElementRect(se, option, widget);
    if (retRect.isEmpty())
        retRect = d.navigationEdgeSubElementRect(se, option, widget);
    if (retRect.isEmpty())
        retRect = d.dockSubElementRect(se, option, widget);
    if (retRect.isEmpty())
    {
        if (d.isQtitanStyle())
            retRect = QCommonStyle::subElementRect(se, option, widget);
        else
            retRect = QProxyStyle::subElementRect(se, option, widget);
    }
    return retRect;
}

/*! \reimp */
QStyle::SubControl CommonStyle::hitTestComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, const QPoint& pt, const QWidget* w) const
{
    return QProxyStyle::hitTestComplexControl(cc, opt, pt, w);
}

/*! \reimp */
QRect CommonStyle::subControlRect(QStyle::ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(widget))
        return QProxyStyle::subControlRect(control, option, subControl, widget);

    bool done = false;
    QRect retRect = d.ribbonSubControlRect(control, option, subControl, widget); 
    done = !retRect.isEmpty();
    if (!done)
    {
        switch (control)
        {
            case CC_TitleBar:
                if (!qtn_hasWindowTitleBar(widget))
                    break;
                if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
                {
                    if (!CommonStylePrivate::buttonVisible(subControl, tb))
                        return retRect;
                    const bool isToolTitle = false;
                    const int frameWidth = proxy()->pixelMetric(PM_MdiSubWindowFrameWidth, option, widget);
                    const int height = pixelMetric(QStyle::PM_TitleBarHeight, Q_NULL, widget) - frameWidth * 2;
                    const int width = tb->rect.width();
                    int buttonHeight = height;
                    int buttonWidth = height + (height * 3 / 4);
                    const int delta = buttonWidth;
                    const bool sysmenuHint = (tb->titleBarFlags & Qt::WindowSystemMenuHint) != 0;
                    const bool minimizeHint = (tb->titleBarFlags & Qt::WindowMinimizeButtonHint) != 0;
                    const bool maximizeHint = (tb->titleBarFlags & Qt::WindowMaximizeButtonHint) != 0;
                    const bool contextHint = (tb->titleBarFlags & Qt::WindowContextHelpButtonHint) != 0;
                    const bool shadeHint = (tb->titleBarFlags & Qt::WindowShadeButtonHint) != 0;
                    bool isMinimized = tb->titleBarState & Qt::WindowMinimized;
                    bool isMaximized = tb->titleBarState & Qt::WindowMaximized;
                    int offset = 0;

                    switch (subControl) 
                    {
                        case SC_TitleBarLabel:
                            retRect = QRect(frameWidth, 0, width - (buttonWidth + frameWidth + 10), height);
                            if (isToolTitle) {
                                if (sysmenuHint) {
                                    retRect.adjust(0, 0, -buttonWidth - 3, 0);
                                }
                                if (minimizeHint || maximizeHint)
                                    retRect.adjust(0, 0, -buttonWidth - 2, 0);
                            }
                            else {
                                if (sysmenuHint) 
                                {
                                    const int leftOffset = height - 8;
                                    retRect.adjust(leftOffset, 0, 0, 0);
                                }
                                if (minimizeHint)
                                    retRect.adjust(0, 0, -buttonWidth - 2, 0);
                                if (maximizeHint)
                                    retRect.adjust(0, 0, -buttonWidth - 2, 0);
                                if (contextHint)
                                    retRect.adjust(0, 0, -buttonWidth - 2, 0);
                                if (shadeHint)
                                    retRect.adjust(0, 0, -buttonWidth - 2, 0);
                            }
                            break;

                        case SC_TitleBarContextHelpButton:
                            if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                                offset += delta;
                            //fall through
                            QTN_FALLTHROUGH();
                        case SC_TitleBarMinButton:
                            if (!isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                                offset += delta;
                            else if (subControl == SC_TitleBarMinButton)
                                break;
                            //fall through
                            QTN_FALLTHROUGH();
                        case SC_TitleBarNormalButton:
                            if (isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                                offset += delta;
                            else if (isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                                offset += delta;
                            else if (subControl == SC_TitleBarNormalButton)
                                break;
                            //fall through
                            QTN_FALLTHROUGH();
                        case SC_TitleBarMaxButton:
                            if (!isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                                offset += delta;
                            else if (subControl == SC_TitleBarMaxButton)
                                break;
                            //fall through
                            QTN_FALLTHROUGH();
                        case SC_TitleBarShadeButton:
                            if (!isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                                offset += delta;
                            else if (subControl == SC_TitleBarShadeButton)
                                break;
                            //fall through
                            QTN_FALLTHROUGH();
                        case SC_TitleBarUnshadeButton:
                            if (isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                                offset += delta;
                            else if (subControl == SC_TitleBarUnshadeButton)
                                break;
                            //fall through
                            QTN_FALLTHROUGH();
                        case SC_TitleBarCloseButton:
                            if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                                offset += delta;
                            else if (subControl == SC_TitleBarCloseButton)
                                break;
                            retRect.setRect(width - offset - frameWidth, frameWidth, buttonWidth, buttonHeight);
                            break;
                        case SC_TitleBarSysMenu:
                        {
                            const int controlTop = 6;
                            const int controlHeight = height - controlTop - 3;
                            const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                            QSize iconSize = tb->icon.actualSize(QSize(iconExtent, iconExtent));
                            if (tb->icon.isNull())
                                iconSize = QSize(controlHeight, controlHeight);
                            int hPad = (controlHeight - iconSize.height()) / 2;
                            int vPad = (controlHeight - iconSize.width()) / 2;
                            retRect = QRect(frameWidth + hPad, controlTop + vPad, iconSize.width(), iconSize.height());
                        }
                        break;
                        default:
                            break;
                    }
                    done = true;
                }
            break;
            default: break;
        }
    }
    if (!done)
    {
        if (d.isQtitanStyle())
            retRect = QCommonStyle::subControlRect(control, option, subControl, widget);
        else
            retRect = QProxyStyle::subControlRect(control, option, subControl, widget);
    }
    return retRect;
}

/*! \reimp */
QSize CommonStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(widget))
        return QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);
    QSize sz = d.ribbonSizeFromContents(ct, opt, contentsSize, widget);
    if (sz.isEmpty())
    {
        if (d.isQtitanStyle())
            return QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
        else
            return QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);
    }
    return sz;
}

inline QString qtn_className(const QWidget* widget)
{
#if !defined(QTITAN_NAMESPACE)
    QString className = widget ? QLatin1String(widget->metaObject()->className()) : QString();
#else
    QString className = widget ?
        QString(QLatin1String(widget->metaObject()->className())).remove(QLatin1String(QTITAN_META_CLASSNAME(""))) : QString();
#endif
    return className;
}

/*! \reimp */
int CommonStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(widget))
        return QProxyStyle::pixelMetric(metric, opt, widget);

    if (metric == PM_TitleBarHeight && widget != Q_NULL && 
        (widget->inherits(QTITAN_META_CLASSNAME("DockWindow")) || widget->inherits(QTITAN_META_CLASSNAME("DockBarWindow"))))
        return pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, opt, widget);

    int val = CommonStylePrivate::pixelMetricFromSystemDp(metric, opt, widget);
    if (val != CommonStylePrivate::InvalidMetric)
        return val;

    if (val == CommonStylePrivate::InvalidMetric)
        val = d.basePixelMetric(metric, opt, widget);

    if (val == CommonStylePrivate::InvalidMetric)
        val = d.ribbonPixelMetric(metric, opt, widget);

    if (val == CommonStylePrivate::InvalidMetric)
        val = d.navigationEdgePixelMetric(metric, opt, widget);

    if (val == CommonStylePrivate::InvalidMetric)
        val = d.gridPixelMetric(metric, opt, widget);

    if (val != CommonStylePrivate::InvalidMetric)
        return val;

    int defaultValue = d.isQtitanStyle() ? QCommonStyle::pixelMetric(metric, opt, widget) : QProxyStyle::pixelMetric(metric, opt, widget);

    bool ok = false;
    switch (metric)
    {
        case PM_SplitterWidth:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("SplitterWidth"), defaultValue, &ok);
            break;
        case PM_MenuButtonIndicator:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("MenuButtonIndicator"), defaultValue, &ok);
            break;
        case PM_ButtonIconSize:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("ButtonIconSize"), defaultValue, &ok);
            break;
        case PM_SmallIconSize:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonBackstageViewMenu")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("RibbonBackstageViewMenu"), QStringLiteral("SmallIconSize"), defaultValue, &ok);
            else
                val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("SmallIconSize"), defaultValue, &ok);
            break;
        case PM_LargeIconSize:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("LargeIconSize"), defaultValue, &ok);
            break;
        case PM_ExclusiveIndicatorHeight:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("ExclusiveIndicatorHeight"), defaultValue, &ok);
            break;
        case PM_IndicatorHeight:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("IndicatorHeight"), defaultValue, &ok);
            break;
        case PM_ExclusiveIndicatorWidth:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("ExclusiveIndicatorWidth"), defaultValue, &ok);
            break;
        case QStyle::PM_ScrollBarExtent:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("ScrollBarExtent"), defaultValue, &ok);
            break;
        case PM_IndicatorWidth:
            val = d.m_config.pixelMetric(widget, QString(), QStringLiteral("IndicatorWidth"), defaultValue, &ok);
            break;
        case PM_TabBarTabHSpace:
            if (widget != Q_NULL)
            {
                if (widget->inherits(QTITAN_META_CLASSNAME("DockDocumentAreaTabBar")))
                    val = d.m_config.pixelMetric(widget, QStringLiteral("DockDocumentAreaTabBar"), QStringLiteral("TabBarTabHSpace"), defaultValue, &ok);
                else if (widget->inherits(QTITAN_META_CLASSNAME("AbstractTabBar")))
                    val = d.m_config.pixelMetric(widget, QStringLiteral("AbstractTabBar"), QStringLiteral("TabBarTabHSpace"), defaultValue, &ok);
                else
                    val = d.m_config.pixelMetric(widget, QStringLiteral("QTabBar"), QStringLiteral("TabBarTabHSpace"), defaultValue, &ok);
            }
            break;
        case PM_TabBarTabVSpace:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("TabWidgetAutoHideBar")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("TabWidgetAutoHideBar"), QStringLiteral("TabBarTabVSpace"), defaultValue, &ok);
            else if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockDocumentAreaTabBar")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("DockDocumentAreaTabBar"), QStringLiteral("TabBarTabVSpace"), defaultValue, &ok);
            else
                val = d.m_config.pixelMetric(widget, QStringLiteral("QTabBar"), QStringLiteral("TabBarTabVSpace"), defaultValue, &ok);
            break;
        case PM_ButtonShiftHorizontal:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QToolButton"), QStringLiteral("ButtonShiftHorizontal"), defaultValue, &ok);
            break;
        case PM_ButtonShiftVertical:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QToolButton"), QStringLiteral("ButtonShiftVertical"), defaultValue, &ok);
            break;
        case PM_SliderTickmarkOffset:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSliderSlider")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("RibbonSliderSlider"), QStringLiteral("SliderTickmarkOffset"), defaultValue, &ok);
            else
                val = d.m_config.pixelMetric(widget, QStringLiteral("QSlider"), QStringLiteral("SliderTickmarkOffset"), defaultValue, &ok);
            break;
        case PM_SliderThickness:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSliderSlider")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("RibbonSliderSlider"), QStringLiteral("SliderThickness"), defaultValue, &ok);
            else
                val = d.m_config.pixelMetric(widget, QStringLiteral("QSlider"), QStringLiteral("SliderThickness"), defaultValue, &ok);
            break;
        case PM_SliderLength:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonSliderSlider")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("RibbonSliderSlider"), QStringLiteral("SliderLength"), defaultValue, &ok) / 2;
            else
                val = d.m_config.pixelMetric(widget, QStringLiteral("QSlider"), QStringLiteral("SliderLength"), defaultValue, &ok);
            break;
        case PM_DockWidgetTitleMargin:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockTitleBar")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("DockTitleBar"), QStringLiteral("DockWidgetTitleMargin"), defaultValue, &ok);
            else if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockBarWindow")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("DockBarWindow"), QStringLiteral("DockWidgetTitleMargin"), 0, &ok);
            else
                val = d.m_config.pixelMetric(widget, QStringLiteral("QDockWidget"), QStringLiteral("DockWidgetTitleMargin"), d.isQtitanStyle() ? 1 : defaultValue, &ok);
            ok = true;
            break;
        case PM_MenuPanelWidth:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QMenu"), QStringLiteral("MenuPanelWidth"), defaultValue, &ok);
            break;
        case PM_MenuBarItemSpacing:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QMenuBar"), QStringLiteral("MenuBarItemSpacing"), defaultValue, &ok);
            break;
        case PM_MenuBarVMargin:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QMenuBar"), QStringLiteral("MenuBarVMargin"), defaultValue, &ok);
            break;
        case PM_MenuBarHMargin:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QMenuBar"), QStringLiteral("MenuBarHMargin"), defaultValue, &ok);
            break;
        case PM_MenuBarPanelWidth:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QMenuBar"), QStringLiteral("MenuBarPanelWidth"), defaultValue, &ok);
            break;
        case PM_ToolBarHandleExtent:
        {
            const QString className = widget != Q_NULL ? qtn_className(widget) : QStringLiteral("QToolBar");
            val = d.m_config.pixelMetric(widget, className, QStringLiteral("ToolBarHandleExtent"), defaultValue, &ok);
            break;
        }
        case PM_ToolBarItemSpacing:
        {
            const QString className = widget != Q_NULL ? qtn_className(widget) : QStringLiteral("QToolBar");
            val = d.m_config.pixelMetric(widget, className, QStringLiteral("ToolBarItemMargin"), defaultValue, &ok);
            break;
        }
        case PM_ToolBarFrameWidth:
        {
            if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("RibbonQuickAccessBar")))
                defaultValue = 0;
            const QString className = widget != Q_NULL ? qtn_className(widget) : QStringLiteral("QToolBar");
            val = d.m_config.pixelMetric(widget, className, QStringLiteral("ToolBarFrameWidth"), defaultValue, &ok);
            break;
        }
        case PM_ToolBarItemMargin:
        {
            const QString className = widget != Q_NULL ? qtn_className(widget) : QStringLiteral("QToolBar");

            //            if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockToolBar")))
            //                val = d.m_config.pixelMetric(widget, className.isEmpty() ? QStringLiteral("DockToolBar") : className, QStringLiteral("ToolBarItemMargin"), defaultValue, &ok);
            //            else
            val = d.m_config.pixelMetric(widget, className, QStringLiteral("ToolBarItemMargin"), defaultValue, &ok);
            break;
        }
        case PM_DockWidgetTitleBarButtonMargin:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockTitleBar")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("DockTitleBar"), QStringLiteral("DockWidgetTitleBarButtonMargin"), 0.0, &ok);
            else
                val = d.m_config.pixelMetric(widget, QStringLiteral("QDockWidget"), QStringLiteral("DockWidgetTitleBarButtonMargin"), d.isQtitanStyle() ? 5 : defaultValue, &ok);
            ok = true;
            break;
#if (QT_VERSION > QT_VERSION_CHECK(5, 8, 0))
        case PM_TitleBarButtonSize:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockTitleBar")))
                val = d.m_config.pixelMetric(widget, QStringLiteral("DockTitleBar"), QStringLiteral("TitleBarButtonSize"), CommonStylePrivate::dpiScaled(14.0, widget), &ok);
            else
                val = defaultValue;
            break;
#endif
        case PM_ToolBarIconSize:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QToolBar"), QStringLiteral("ToolBarIconSize"), defaultValue, &ok);
            break;
        case PM_SubMenuOverlap:
            val = d.m_config.pixelMetric(widget, QStringLiteral("QMenu"), QStringLiteral("SubMenuOverlap"), defaultValue, &ok);
            break;
        case PM_MdiSubWindowFrameWidth:
            {
                if (qtn_hasWindowTitleBar(widget))
                {
#ifdef Q_OS_MAC
                    val = 0;
#else
                    const int defaultWidth = 1;
                    val = d.m_config.pixelMetric(widget, QStringLiteral("QMainWindow"), QStringLiteral("MdiSubWindowFrameWidth"), defaultWidth, &ok);
#endif
                }
                else
                {
                    if (widget && widget->inherits(QTITAN_META_CLASSNAME("DockMainWindow")))
                        defaultValue = 1;
                    val = d.m_config.pixelMetric(widget, QStringLiteral("QMdiSubWindow"), QStringLiteral("MdiSubWindowFrameWidth"), defaultValue, &ok);
                }
            }
            break;
        case PM_MessageBoxIconSize:
            val = QProxyStyle::pixelMetric(metric, opt, widget);
            break;
        default:
            val = defaultValue;
            break;
    }

    return val;
}

/*! \reimp */
int CommonStyle::styleHint(QStyle::StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(widget))
        return QProxyStyle::styleHint(hint, option, widget, returnData);

    if (static_cast<StyleHintEx>(hint) >= CommonStyle::SH_RibbonBegin && static_cast<StyleHintEx>(hint) <= CommonStyle::SH_RibbonEnd)
        return d.ribbonStyleHint(hint, option, widget, returnData);

    if (static_cast<StyleHintEx>(hint) >= CommonStyle::SH_DockStyledFrame && static_cast<StyleHintEx>(hint) <= CommonStyle::SH_DockSysMenuButtonVisible)
        return d.dockStyleHint(hint, option, widget, returnData);

    int defaultValue = 0;
    if (d.isQtitanStyle())
        defaultValue = QCommonStyle::styleHint(hint, option, widget, returnData);
    else
        defaultValue = QProxyStyle::styleHint(hint, option, widget, returnData);

    int val = CommonStylePrivate::InvalidMetric;
    switch (hint) 
    {
        case SH_ToolButtonStyle:
            {
            if (widget != Q_NULL && widget->inherits(QTITAN_META_CLASSNAME("RibbonSystemButton")))
                val = Qt::ToolButtonTextOnly;
            }
            break;
        case SH_DitherDisabledText:
            val = d.m_config.styleHint(QString(), QStringLiteral("DitherDisabledText"), defaultValue);
            break;
        case SH_Menu_Scrollable:
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("PopupMenu")))
                val = 0;
            break;
        case SH_UnderlineShortcut:
        {
            if (widget && widget->inherits(QTITAN_META_CLASSNAME("RibbonTab")))
                defaultValue = 0;
            val = d.m_config.styleHint(QString(), QStringLiteral("UnderlineShortcut"), defaultValue);
        }
            break;
        case SH_MenuBar_AltKeyNavigation:
            if (widget->inherits(QTITAN_META_CLASSNAME("RibbonBar")))
                val = true;
            break;
        case SH_ComboBox_ListMouseTracking:
            val = d.m_config.styleHint(QString(), QStringLiteral("ComboBox_ListMouseTracking"), defaultValue);
            break;
        case SH_TitleBar_AutoRaise:
            val = d.m_config.styleHint(QString(), QStringLiteral("TitleBar_AutoRaise"), defaultValue);
            break;
        case SH_Slider_SnapToValue:
        case SH_PrintDialog_RightAlignButtons:
        case SH_FontDialog_SelectAssociatedText:
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        case SH_ScrollBar_StopMouseOverSlider:
#endif
        case SH_ScrollBar_MiddleClickAbsolutePosition:
        case SH_EtchDisabledText:
            val = d.m_config.styleHint(QString(), QStringLiteral("EtchDisabledText"), defaultValue);
            break;
        case SH_MenuBar_MouseTracking:
            val = d.m_config.styleHint(QStringLiteral("QMenuBar"), QStringLiteral("MenuBar_MouseTracking"), defaultValue);
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        case SH_Menu_SupportsSections:
            val = d.m_config.styleHint(QStringLiteral("QMenu"), QStringLiteral("Menu_SupportsSections"), defaultValue);
            break;
#endif
        case SH_Menu_MouseTracking:
            val = d.m_config.styleHint(QStringLiteral("QMenu"), QStringLiteral("Menu_MouseTracking"), defaultValue);
            break;
#if defined(Q_OS_IOS)
        case SH_ComboBox_UseNativePopup:
            val = 1;
            break;
#endif
        default:
            val = defaultValue;
        break;
    }
    return val;
}

/*! \reimp */
QIcon CommonStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(widget) || (widget && widget->inherits("QDockWidget")))
        return QProxyStyle::standardIcon(standardIcon, opt, widget);

    QPixmap px = standardPixmap(standardIcon, opt, widget);
    if (px.isNull())
    {
        if (d.isQtitanStyle())
            return QCommonStyle::standardIcon(standardIcon, opt, widget);
        else
            return QProxyStyle::standardIcon(standardIcon, opt, widget);
    }
    return px;
}

/*! \reimp */
QPixmap CommonStyle::standardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget) const
{
    QTN_D(const CommonStyle);
    if (!d.widgetStyleSupport(widget))
        return QProxyStyle::standardPixmap(sp, opt, widget);

    QPixmap px = d.ribbonStandardPixmap(sp, opt, widget);
    if (!px.isNull())
        return px;
    px = d.dockStandardPixmap(sp, opt, widget);
    if (!px.isNull())
        return px;
    px = d.navigationStandardPixmap(sp, opt, widget);
    if (!px.isNull())
        return px;

    switch (sp)
    {
        case SP_MessageBoxInformation:
        case SP_MessageBoxWarning:
        case SP_MessageBoxCritical:
        case SP_MessageBoxQuestion:
            px = QProxyStyle::standardPixmap(sp, opt, widget);
            break;
        case SP_TitleBarCloseButton:
        case SP_TitleBarMinButton:
        case SP_TitleBarContextHelpButton:
        case SP_TitleBarMaxButton:
        case SP_TitleBarNormalButton:
#ifdef QTN_MAC_BUTTONS
            if (const QStyleOptionTitleBar *titlebar = qstyleoption_cast<const QStyleOptionTitleBar *>(opt))
                px = qtn_titlebar_standard_pixmap(sp, titlebar, widget);
            else
#endif
            if (widget && widget->inherits("QDockWidget"))
                px = QProxyStyle::standardPixmap(sp, opt, widget);
            else
                px = d.m_standardSymbol.standardSymbolPixmap(sp, opt, widget);
            break;
        case SP_ArrowUp:
            px = d.standardPixmap(QStringLiteral("Image_ArowUpMenu"), (opt && opt->state & QStyle::State_Enabled) ? CommonStylePrivate::Black : CommonStylePrivate::Gray, widget);
            break;
        case SP_ArrowDown:
            px = d.m_standardSymbol.standardSymbolPixmap(sp, opt, widget);
            break;
        default:
            break;
    }

    if (px.isNull())
    {
        if (d.isQtitanStyle())
            px = QCommonStyle::standardPixmap(sp, opt, widget);
        else
            px = QProxyStyle::standardPixmap(sp, opt, widget);
    }
    return px;
}

/*! \reimp */
QPixmap CommonStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap& pixmap, const QStyleOption* opt) const
{
    return QProxyStyle::generatedIconPixmap(iconMode, pixmap, opt);
}

/*! \reimp */
int CommonStyle::layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, 
    Qt::Orientation orientation, const QStyleOption *option, const QWidget *widget) const
{
    return QProxyStyle::layoutSpacing(control1, control2, orientation, option, widget);
}

void CommonStyle::setStyleConfig(const QString& fileName, const QString& themeName)
{
    QTN_D(CommonStyle);
    d.m_config.load(fileName, themeName);
    if (themeName != QStringLiteral("Default"))
        d.updateStyle();
}

QRgb CommonStyle::lightColor(const QRgb& rgb, int tint)
{
    return StyleHelper::lightColor(rgb, tint);
}

QRgb CommonStyle::darkColor(const QRgb& rgb, int tint)
{
    return StyleHelper::darkColor(rgb, tint);
}

QColor CommonStyle::contextColorToColor(Qtitan::ContextColor color)
{
    switch (color)
    {
        case Qtitan::ContextColorRed:
            return QColor(255, 160, 160);

        case Qtitan::ContextColorOrange:
            return QColor(239, 189, 55);

        case Qtitan::ContextColorYellow:
            return QColor(253, 229, 27);

        case Qtitan::ContextColorGreen:
            return QColor(113, 190, 89);

        case Qtitan::ContextColorBlue:
            return QColor(128, 181, 196);

        case Qtitan::ContextColorCyan:
            return QColor(114, 163, 224);

        case Qtitan::ContextColorPurple:
            return QColor(214, 178, 209);
        default:
            break;
    }
    return QColor();
}

QString CommonStyle::contextColorToString(Qtitan::ContextColor color)
{
    switch (color)
    {
        case Qtitan::ContextColorBlue:
            return QStringLiteral("Blue");
        case Qtitan::ContextColorYellow:
            return QStringLiteral("Yellow");
        case Qtitan::ContextColorGreen:
            return QStringLiteral("Green");
        case Qtitan::ContextColorRed:
            return QStringLiteral("Red");
        case Qtitan::ContextColorPurple:
            return QStringLiteral("Purple");
        case Qtitan::ContextColorCyan:
            return QStringLiteral("Cyan");
        case Qtitan::ContextColorOrange:
            return QStringLiteral("Orange");
        default:
            break;
    }
    return QString();
}

QImage CommonStyle::addaptColors(const QImage& imageSrc, QRgb clrBase, QRgb clrTone, bool clampHue)
{
    return StyleHelper::addaptColors(imageSrc, clrBase, clrTone, clampHue);
}

QImage CommonStyle::invertColors(const QImage& pxSrc)
{
    return StyleHelper::invertColors(pxSrc);
}

/*! \reimp */
bool CommonStyle::event(QEvent* event)
{
    QTN_D(CommonStyle);
    switch (event->type())
    {
        case QEvent::Timer:
        {
            QTimerEvent* pTimerEvent = static_cast<QTimerEvent*>(event);
            if (d.m_animationTimer.timerId() == pTimerEvent->timerId())
            {
                d.doTimer();
                event->accept();
                return true;
            }
        }
        break;
        default:
            break;
    }
    return QProxyStyle::event(event);
}

/*! \reimp */
bool CommonStyle::eventFilter(QObject* watched, QEvent* event)
{
    QTN_D(CommonStyle);
    switch (event->type())
    {
        case QEvent::Destroy:
        case QEvent::Create:
            if (QWidget* widget = qobject_cast<QWidget*>(watched))
                d.stopAnimation(widget);
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        case QEvent::ThemeChange:
            d.beginUpdate(false);
            d.endUpdate(false);
            break;
#endif
        case QEvent::ToolTip:
            if (QWidget* widget = qobject_cast<QWidget*>(watched))
            {
                if (d.isQtitanStyle() && d.showToolTip(static_cast<QHelpEvent*>(event)->globalPos(), widget))
                    return true;
            }
            break;
        default:
            break;
    }
    return QProxyStyle::eventFilter(watched, event);
}

/*!
    \internal
    Returns rectangle of text.
*/
QRect QTITAN_PREPEND_NAMESPACE(qtn_paint_icon_and_text)(QPainter& painter, Qt::LayoutDirection direction, const QPalette& palette, const QRect& r,
    const QIcon& icon, const QSize& iconSize, bool enabled, Qt::Alignment iconAlignment, const QString& text, Qt::Alignment textAlignment, Qt::TextElideMode textElideMode,
    int margin, bool* textFullyVisible, const QString& selectedText, Qt::CaseSensitivity selectCaseSensitivity)
{
    if (textFullyVisible)
        *textFullyVisible = false;

    QRect paintRect = r;

    if (!icon.isNull())
    {
        QRect alignedRect = QStyle::alignedRect(direction, iconAlignment, iconSize, paintRect);
        icon.paint(&painter, alignedRect, Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled);
        if (direction == Qt::LeftToRight)
            paintRect.setLeft(paintRect.left() + iconSize.width() + margin);
        else
            paintRect.setRight(paintRect.right() - iconSize.width() - margin);
    }
    
    if (paintRect.isEmpty())
        return paintRect;

    QRect textRect = paintRect;

    QVector<QTextLayout::FormatRange> selections;
    if (selectedText.size() > 0)
    {
        int i = 0;
        while ((i = text.indexOf(selectedText, i, selectCaseSensitivity)) != -1)
        {
            QTextLayout::FormatRange range;
            range.start = i;
            range.length = selectedText.size();
            i += selectedText.size();

            range.format.setBackground(palette.brush(QPalette::Highlight));
            range.format.setForeground(palette.brush(QPalette::HighlightedText));
            selections.append(range);
        }
    }

    QString newText = text;
    {
        QRect textBoundingRect = painter.fontMetrics().boundingRect(newText);
        textBoundingRect.adjust(0, 0, 1, 0);

        if (textFullyVisible)
            *textFullyVisible = textBoundingRect.width() <= textRect.width();

        if (textBoundingRect.width() > textRect.width())
            newText = painter.fontMetrics().elidedText(newText, textElideMode, textRect.width());
    }

    QRect newTextRect = textRect;
    if (!newText.isEmpty())
    {
        Qt::Alignment temp = textAlignment | Qt::AlignVCenter;
        //Error rendering text under WinXP
        //temp &= ~Qt::AlignRight;
        //temp &= ~Qt::AlignHCenter;
        //temp |= Qt::AlignLeft;
        QTextOption textOption(temp);
        textOption.setWrapMode(QTextOption::NoWrap);

        if (selections.size() == 0)
        {
            painter.drawText(newTextRect, newText, textOption);
        }
        else
        {
            QTextLayout textLayout(newText, painter.font());
            textLayout.setTextOption(textOption);

            int leading = painter.fontMetrics().leading();
            qreal height = 0;
            qreal width = 0;
            textLayout.beginLayout();
            while (1) {
                QTextLine line = textLayout.createLine();
                if (!line.isValid())
                    break;

                line.setLineWidth(newTextRect.width());
                height += leading;
                line.setPosition(QPointF(0, height));
                height += line.height();
                width = qMax(width, line.naturalTextWidth());
            }
            textLayout.endLayout();


            qreal yoffset = 0;
            qreal xoffset = 0;
            if (textOption.alignment() & Qt::AlignBottom)
                yoffset = newTextRect.height() - height;
            else if (textOption.alignment() & Qt::AlignVCenter)
                yoffset = (newTextRect.height() - height) / 2;
#if 0
            if (textOption.alignment() & Qt::AlignRight)
                xoffset = newTextRect.width() - width;
            else if (textOption.alignment() & Qt::AlignHCenter)
                xoffset = (newTextRect.width() - width) / 2;
#endif                
            textLayout.draw(&painter, newTextRect.topLeft() + QPoint(static_cast<int>(xoffset), static_cast<int>(yoffset)), selections);
        }
    }

    return textRect;
}

void QTITAN_PREPEND_NAMESPACE(qtn_draw_pixmap_texture)(QPainter* painter, const QRect& rect, const QPixmap& texture, int left, int top, int right, int bottom)
{
    painter->save();
    painter->setClipRect(rect, Qt::IntersectClip);
    painter->drawPixmap(QPoint(rect.x(), rect.y()), texture, QRect(0, 0, left, top));
    painter->drawPixmap(QPoint(rect.x(), rect.y() + rect.height() - bottom), texture, QRect(0, texture.height() - bottom, left, bottom));
    painter->drawPixmap(QPoint(rect.x() + rect.width() - right, rect.y()), texture, QRect(texture.width() - right, 0, right, top));
    painter->drawPixmap(QPoint(rect.x() + rect.width() - right, rect.y() + rect.height() - bottom), texture, QRect(texture.width() - right, texture.height() - bottom, right, bottom));

    QPixmap temp = texture.copy(left, 0, texture.width() - right - left, top);
    temp = temp.scaled(rect.width() - left - right, top, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->drawPixmap(QPoint(rect.x() + left, rect.y()), temp);

    temp = texture.copy(0, top, left, texture.height() - top - bottom);
    temp = temp.scaled(left, rect.height() - top - bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->drawPixmap(QPoint(rect.x(), rect.y() + top), temp);

    temp = texture.copy(texture.width() - right, top, right, texture.height() - top - bottom);
    temp = temp.scaled(right, rect.height() - top - bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->drawPixmap(QPoint(rect.x() + rect.width() - right, rect.y() + top), temp);

    temp = texture.copy(right, texture.height() - bottom, texture.width() - left - right, bottom);
    temp = temp.scaled(rect.width() - left - right, bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->drawPixmap(QPoint(rect.x() + left, rect.y() + rect.height() - bottom), temp);

    temp = texture.copy(right, top, texture.width() - left - right, texture.height() - top - bottom);
    temp = temp.scaled(rect.width() - left - right, rect.height() - top - bottom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->drawPixmap(QPoint(rect.x() + left, rect.y() + top), temp);
    painter->restore();
}

void QTITAN_PREPEND_NAMESPACE(qtn_paint_keyboard_focus)(QPainter* painter, const QRect& rect, const QColor& color)
{
    QRect r = rect;
    int thickness = 3;
    QPen pen(color, thickness, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    painter->setPen(pen);
    qtn_drawRect(painter, r);
}

bool QTITAN_PREPEND_NAMESPACE(qtn_isColorLight)(const QColor& color)
{
    return ((color.red() * 0.299 + color.green() * 0.587 + color.blue() * 0.114) > 150);
}

void QTITAN_PREPEND_NAMESPACE(qtn_drawRect)(QPainter* painter, const QRect& rect)
{
    painter->drawRect(qtn_adjustRect(painter->pen(), rect));
}

QPen QTITAN_PREPEND_NAMESPACE(qtn_createPen)(const QColor& color, int thickness)
{
    return QPen(color, thickness, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
}

QRect QTITAN_PREPEND_NAMESPACE(qtn_adjustRect)(const QPen& pen, const QRect& rect)
{
    const int thickness = pen.width();
    return rect.adjusted(thickness / 2, thickness / 2, -((thickness + 1) / 2), -((thickness + 1) / 2));
}

QRectF QTITAN_PREPEND_NAMESPACE(qtn_adjustRectF)(const QPen& pen, const QRectF& rect)
{
    const qreal thickness = pen.widthF();
    return rect.adjusted(thickness / 2, thickness / 2, -(thickness / 2), -(thickness / 2));
}

int QTITAN_PREPEND_NAMESPACE(qtn_DPIScaled)(int value, const QWidget* widget)
{
    return CommonStylePrivate::dpiScaled(value, widget);
}

qreal QTITAN_PREPEND_NAMESPACE(qtn_DPIScaledF)(qreal value, const QWidget* widget)
{
    return CommonStylePrivate::dpiScaledF(value, widget);
}

QPointF QTITAN_PREPEND_NAMESPACE(qtn_DPIScaledF)(const QPointF& point, const QWidget* widget)
{
    return QPointF(CommonStylePrivate::dpiScaledF(point.x(), widget),
        CommonStylePrivate::dpiScaledF(point.y(), widget));
}

QSize QTITAN_PREPEND_NAMESPACE(qtn_DPIScaled)(const QSize& size, const QWidget* widget)
{
    return QSize(CommonStylePrivate::dpiScaled(size.width(), widget), 
        CommonStylePrivate::dpiScaled(size.height(), widget));
}

QSizeF QTITAN_PREPEND_NAMESPACE(qtn_DPIScaledF)(const QSizeF& size, const QWidget* widget)
{
    return QSizeF(CommonStylePrivate::dpiScaledF(size.width(), widget),
        CommonStylePrivate::dpiScaledF(size.height(), widget));
}

int QTITAN_PREPEND_NAMESPACE(qtn_getPixelMetric)(int metric, const QWidget* widget)
{
    return widget->style()->pixelMetric(static_cast<QStyle::PixelMetric>(metric), Q_NULL, widget);
}
