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

#ifndef QTN_DOCKTOOLBAR_H
#define QTN_DOCKTOOLBAR_H

#include "QtnDockBarBase.h"
#include "QtnDockDef.h"

class QAction;
class QIcon;
class QStyleOptionToolBar;

QTITAN_BEGIN_NAMESPACE

class DockToolBarPrivate;
class QTITAN_EXPORT DockToolBar : public DockBarBase
{
    Q_OBJECT
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle NOTIFY toolButtonStyleChanged)
    Q_PROPERTY(QString caption READ caption WRITE setCaption)
public:
    explicit DockToolBar(const QString& caption, QWidget* parent = Q_NULL);
    explicit DockToolBar(QWidget* parent = Q_NULL);
    ~DockToolBar();
public:
    virtual void setVisible(bool visible);
public:
    void clear();

    using QWidget::addAction;
    QAction* addAction(const QString& text);
    QAction* addAction(const QIcon& icon, const QString& text);
    QAction* addAction(const QString& text, const QObject* receiver, const char* member);
    QAction* addAction(const QIcon& icon, const QString& text, const QObject* receiver, const char* member);

    QAction* addSeparator();
    QAction* insertSeparator(QAction* before);

    QAction* addWidget(QWidget* widget);
    QAction* insertWidget(QAction* before, QWidget* widget);

    QRect actionGeometry(QAction* action) const;
    QAction* actionAt(const QPoint& p) const;
    QAction* actionAt(int x, int y) const;
    QWidget* widgetForAction(QAction *action) const;

    QSize iconSize() const;
    Qt::ToolButtonStyle toolButtonStyle() const;
public Q_SLOTS:
    void setIconSize(const QSize& iconSize);
    void setToolButtonStyle(Qt::ToolButtonStyle toolButtonStyle);
Q_SIGNALS:
    void actionTriggered(QAction* action);
    void iconSizeChanged(const QSize& iconSize);
    void toolButtonStyleChanged(Qt::ToolButtonStyle toolButtonStyle);
protected:
    virtual bool event(QEvent* event);
    virtual void actionEvent(QActionEvent* event);
    virtual void changeEvent(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void initStyleOption(QStyleOptionToolBar* option) const;
private:
    QTN_DECLARE_EX_PRIVATE(DockToolBar)
    Q_DISABLE_COPY(DockToolBar)
    friend class DockToolBarLayout;
    friend class DockBarAreaPrivate;
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKTOOLBAR_H
