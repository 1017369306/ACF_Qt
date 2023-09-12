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

#ifndef QTN_COLORBUTTON_H
#define QTN_COLORBUTTON_H

#include <QPushButton>
#include <QPaintEvent>
#include <QComboBox>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class QTITAN_BASE_EXPORT ColorButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget* parent);
public:
    virtual QSize sizeHint() const;
public:
    const QColor& color() const;
    void setColor(const QColor& color);
    void setToolTip(const QString &tip);
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent* event);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    virtual void enterEvent(QEnterEvent* event);
#else
    virtual void enterEvent(QEvent* event);
#endif
    virtual void leaveEvent(QEvent* event);
private:
    QColor m_color;
    bool m_sunken;
private:
    Q_DISABLE_COPY(ColorButton)
};

class QTITAN_BASE_EXPORT ColorPickerButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorPickerButton(const QColor & color = Qt::darkGray, QWidget* parent = Q_NULL);
    const QColor& color() const;
Q_SIGNALS:
    void colorChanged(const QColor& color);
    void colorSelected(const QColor& color);
public Q_SLOTS:
    void setColor(const QColor& color);
    void selectColor();
    void paintEvent(QPaintEvent *event);
private:
    QColor m_color;
};

class QTITAN_BASE_EXPORT TextLinkButton : public QAbstractButton
{
    Q_OBJECT
public:
    TextLinkButton(QWidget* parent);
    virtual ~TextLinkButton();
    void setTextColor(const QColor& color);
    virtual QSize sizeHint() const;
    void setWebStyle(bool webStyle);
protected:
    QString paintText() const;
    virtual void paintEvent(QPaintEvent* event);
    virtual bool event(QEvent* e);
private:
    bool m_webStyle;
};

class QTITAN_BASE_EXPORT TextLinkComboBox : public QComboBox
{
    Q_OBJECT
public:
    TextLinkComboBox(QWidget* parent);
    virtual ~TextLinkComboBox();
    void setTextColor(const QColor& color);
    void setUseQuotes(bool quotes);
    virtual void showPopup();
    QString paintText() const;
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    bool m_useQuotes;
};

QTITAN_END_NAMESPACE

#endif //QTN_COLORBUTTON_H


