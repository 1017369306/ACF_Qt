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
#ifndef QTN_TOGGLESWITCH_H
#define QTN_TOGGLESWITCH_H

#include <QAbstractButton>
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class ToggleSwitchPrivate;
class QTITAN_BASE_EXPORT ToggleSwitch : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    enum Shape
    {
        Rectangle,
        RectangleSmallGroove,
        Rounded,
        RoundedSmallGroove,
        Circle,
        CircleSmallGroove,
    };
public:
    explicit ToggleSwitch(QWidget* widget = Q_NULL);
    virtual ~ToggleSwitch();
public:
    bool isOn() const;

    Qt::Alignment alignmentText() const;
    void setAlignmentText(Qt::Alignment alignment);

    QString labelOff() const;
    void setLabelOff(const QString& off);

    QString labelOn() const;
    void setLabelOn(const QString& on);

    bool isAnimated() const;
    void setAnimated(bool animated);

    bool isVisibleLabels() const;
    void setVisibleLabels(bool visible = true);

    Shape shape() const;
    void setShape(Shape style);

    QAction* defaultAction() const;
    void setDefaultAction(QAction* action);
public Q_SLOTS:
    void setOn(bool set = true);
public:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;
protected:
    virtual bool event(QEvent* event) override;
    virtual void changeEvent(QEvent* event) override;
    virtual void actionEvent(QActionEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
private:
    QTN_DECLARE_PRIVATE(ToggleSwitch)
    Q_DISABLE_COPY(ToggleSwitch)
};

QTITAN_END_NAMESPACE

#endif // QTN_TOGGLESWITCH_H
