/****************************************************************************
**
** Qtitan Library by Developer Machines
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

#ifndef QTN_POPUPMENU_H
#define QTN_POPUPMENU_H

#include <QMenu>
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class PopupMenuPrivate;

class QTITAN_BASE_EXPORT PopupMenu : public QMenu
{
    Q_OBJECT
public:
    explicit PopupMenu(QWidget* parent);
    ~PopupMenu() override;
public:
    QWidget* previewWidget() const;
    void setPreviewWidget(QWidget* preview);
    void setMaxItemCount(int maxCount);
    int maxItemCount() const;
    QSize sizeHint() const override;
protected:
    virtual void paintMenuItem(QPainter* painter, QStyleOptionMenuItem* option);
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
protected:
    explicit PopupMenu(PopupMenuPrivate& d, QWidget* parent);
protected:
    QTN_DECLARE_PRIVATE(PopupMenu)
private:
    friend class PopupMenuView;
    Q_DISABLE_COPY(PopupMenu)
};

QTITAN_END_NAMESPACE

#endif // QTN_POPUPMENU_H


