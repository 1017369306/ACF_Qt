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

#ifndef QTN_DOCKTITLEBAR_H
#define QTN_DOCKTITLEBAR_H

#include <QWidget>

#include "QtnDockLayoutPrivate.h"
#include "QtnStyleOption.h"
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockTitleBarLayout;
class DockTitleBarPrivate;

/* DockTitleBar */
class DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTitleBar(QWidget* parent);
    virtual ~DockTitleBar();
public:
    bool isTitleActive() const;

    QString text() const;
    void setText(const QString& txt);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    bool isCloseButtonVisible() const;
    void setCloseButtonVisible(bool visible);

    bool isAutoHideButtonVisible() const;

    bool isMenuButtonVisible() const;
    void setMenuButtonVisible(bool visible);
    void setNoDrawGrip(bool draw);
    void updateFeatures(DockWidgetPanel::DockPanelFeatures features);
Q_SIGNALS:
    void closeClicked();
    void autoHideClicked();
    void menuClicked();
public Q_SLOTS:
    void setAutoHideButtonVisible(bool visible);
    void setAutoHideButtonState(bool state);
    void setTitleActive(bool state);
public:
    virtual QSize sizeHint() const { return minimumSizeHint(); }
    virtual QSize minimumSizeHint() const;
protected:
    void initStyleOption(DockTitleBarStyleOption* option) const;
    bool checkSetFocus();
    void updateButtons();
protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void actionEvent(QActionEvent* event);
private:
    friend class DockTitleBarLayout;
    QTN_DECLARE_PRIVATE(DockTitleBar)
    Q_DISABLE_COPY(DockTitleBar)
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKTITLEBAR_H
