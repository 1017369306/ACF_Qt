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

#include <QColorDialog>
#include <QPainter>
#include <QAbstractItemView>

#include "QtnCommonStylePrivate.h"
#include "QtnColorButton.h"
#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

#define SIZE_COLOR_BUTTON 50

/* ColorButton */
ColorButton::ColorButton(QWidget* parent)
    : QAbstractButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setMouseTracking(true);
    QPalette p = palette();
    QPalette_setColor(p, QPalette::Light, Qt::white);
    setPalette(p);
    m_color = Qt::white;
    m_sunken = false;
}

QSize ColorButton::sizeHint() const
{
    return QSize(SIZE_COLOR_BUTTON, SIZE_COLOR_BUTTON);
}

const QColor& ColorButton::color() const
{
    return m_color;
}

void ColorButton::setColor(const QColor& color)
{
    m_color = color;
}

void ColorButton::setToolTip(const QString &tip)
{
    QWidget::setToolTip(tip);
}

void ColorButton::mousePressEvent(QMouseEvent *event)
{
    QAbstractButton::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
        qtn_clickLight(this);
}

void ColorButton::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractButton::mouseMoveEvent(event);
    update();
}

void ColorButton::paintEvent(QPaintEvent* p)
{
    Q_UNUSED(p);
    QPainter painter(this);
    QRect r = QRect(0, 0, width(), height());
    painter.fillRect(r, color());
    if (underMouse())
        qtn_paintLight(this, &painter, r);

    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(m_sunken ? color().darker(150) : color().darker(150));
    painter.setPen(pen);
    painter.drawRect(r.adjusted(0, 0, -1, -1));
    
    if (underMouse())
        qtn_paintLightFrame(this, &painter, r);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void ColorButton::enterEvent(QEnterEvent* event)
#else
void ColorButton::enterEvent(QEvent* event)
#endif
{
    QAbstractButton::enterEvent(event);
    m_sunken = true;
    update();
}

void ColorButton::leaveEvent(QEvent* event)
{
    QAbstractButton::leaveEvent(event);
    m_sunken = false;
    update();
}


/* ColorPickerButton */
ColorPickerButton::ColorPickerButton(const QColor& color, QWidget* parent) :
    QPushButton(parent), m_color(color)
{
    this->setMinimumWidth(50);
    connect(this, SIGNAL(clicked()), this, SLOT(selectColor()));
}

const QColor& ColorPickerButton::color() const
{
    return m_color;
}

void ColorPickerButton::setColor(const QColor& color)
{
    if (m_color == color)
        return;
    m_color = color;
    colorChanged(m_color);
}

void ColorPickerButton::selectColor()
{
    QColor color = QColorDialog::getColor(m_color, this);
    if (color.isValid())
    {
        setColor(color);
        colorSelected(color);
    }
}

void ColorPickerButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    int colorPadding = 5;

    QRect rect = event->rect();
    QPainter painter(this);
    painter.setBrush(QBrush(color()));
    painter.setPen("#CECECE");
    rect.adjust(colorPadding, colorPadding, -1 - colorPadding, -1 - colorPadding);
    painter.drawRect(rect);
}

/* TextLinkButton */
TextLinkButton::TextLinkButton(QWidget* parent)
    : QAbstractButton(parent), m_webStyle(false)
{
}

TextLinkButton::~TextLinkButton()
{
}

void TextLinkButton::setTextColor(const QColor& color)
{
    QPalette p = palette();
    QPalette_setColor(p, QPalette::ButtonText, color);
    setPalette(p);
}

QSize TextLinkButton::sizeHint() const
{
    QString text = paintText();
    QFontMetrics metrics(font());
    QRect r = metrics.boundingRect(text);
    return r.size() + QSize(4, 0);
}

void TextLinkButton::setWebStyle(bool webStyle)
{
    m_webStyle = webStyle;
    if (m_webStyle)
        setCursor(Qt::PointingHandCursor);
    else
        setCursor(QCursor());
}

QString TextLinkButton::paintText() const
{
    if (m_webStyle)
        return text();
    QString ret = QString(QStringLiteral("[%1]")).arg(text());
    return ret;
}

void TextLinkButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QString text = paintText();
    painter.setPen(palette().buttonText().color());
    QFont f = painter.font();
    if (m_webStyle)
        f.setUnderline(true);
    else
        f.setBold(true);
    painter.setFont(f);
    painter.drawText(rect(), Qt::AlignCenter, text);
}

bool TextLinkButton::event(QEvent* e)
{
    if (e->type() == QEvent::PaletteChange)
    {
    }
    return QAbstractButton::event(e);
}

TextLinkComboBox::TextLinkComboBox(QWidget* parent)
    : QComboBox(parent), m_useQuotes(false)
{
    setUseQuotes(false);
}

TextLinkComboBox::~TextLinkComboBox()
{
}

void TextLinkComboBox::setTextColor(const QColor& color)
{
    QPalette p = palette();
    QPalette_setColor(p, QPalette::ButtonText, color);
    setPalette(p);
}

void TextLinkComboBox::setUseQuotes(bool quotes)
{
    m_useQuotes = quotes;
    if (m_useQuotes)
        setCursor(Qt::PointingHandCursor);
    else
        setCursor(QCursor());
}

void TextLinkComboBox::showPopup()
{
    int popupMinimumWidth = qMin(250, QComboBox::sizeHint().width());
    QWidget* container = view()->parentWidget();
    container->setMinimumWidth(popupMinimumWidth);
    QComboBox::showPopup();
}

QString TextLinkComboBox::paintText() const
{
    QString text = m_useQuotes ? QString(QStringLiteral("'%1'")).arg(currentText()) : currentText();
    return text;
}

QSize TextLinkComboBox::sizeHint() const
{
    QString text = paintText();
    QFontMetrics metrics(font());
    QRect r = metrics.boundingRect(text);
    return r.size() + QSize(4, 0);
}

void TextLinkComboBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QString text = paintText();
    painter.setPen(palette().buttonText().color());
    QFont f = painter.font();
    if (m_useQuotes)
        f.setUnderline(true);
    painter.setFont(f);
    painter.drawText(rect(), Qt::AlignCenter, text);
}

