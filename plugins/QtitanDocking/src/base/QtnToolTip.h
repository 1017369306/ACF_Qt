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
#ifndef QTN_TOOLTIP_H
#define QTN_TOOLTIP_H

#include <QFrame>
#include <QTimer>
#include <QIcon>
#include <QEvent>
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class ToolTipPrivate;
class QTITAN_BASE_EXPORT ToolTip : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(int margin READ margin WRITE setMargin)
public:
    ToolTip(const QString& title, const QString& text, const QIcon& icon = QIcon(), QWidget* parent = Q_NULL);
    virtual ~ToolTip();
    static ToolTip* instance();
public:
    static void showToolTip(const QPoint& pos, const QString& title, const QString& text, const QIcon& icon = QIcon(), QWidget* parent = Q_NULL);
    static void hideToolTip();
    static bool isToolTipVisible();

    enum WrapMode
    {
        NoWrap = 0, 
        NativeWrap = NoWrap,  
        AutoWrap,
    };

    static void setWrapMode(WrapMode mode);
    static WrapMode wordWrap();

    static void setIconVisible(bool visible);
    static bool isIconVisible();
public:
    int margin() const;
    void setMargin(int);

    const QString& title() const;
    const QString& text() const;

    void setIcon(const QIcon &icon);
    const QIcon& icon() const;
public:
    void reuseTip(const QString& textTitle, const QString& text);
    void hideTip();
    void closeToolTip();
    void setTipRect(QWidget* w, const QRect& r);
    void restartExpireTimer();
    bool tipChanged(const QPoint& pos, const QString& text, const QString& textTitle, QObject* o);
    void placeTip(const QPoint& pos, QWidget* w);
public Q_SLOTS:
    void setTitle(const QString&);
    void setText(const QString&);
public:
    virtual QSize sizeHint() const;
protected:
    virtual bool event(QEvent* event);
    virtual bool eventFilter(QObject*, QEvent* event);
    virtual void timerEvent(QTimerEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
public:
    bool fadingOut() const;
private:
    QTN_DECLARE_PRIVATE(ToolTip)
    Q_DISABLE_COPY(ToolTip)
};

QTITAN_END_NAMESPACE

#endif // QTN_TOOLTIP_H
