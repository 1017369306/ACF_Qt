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

#ifndef QTN_CUSTOMLAYOUT_H
#define QTN_CUSTOMLAYOUT_H

#include <QLayout>
#include <QStatusBar>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class QTITAN_BASE_EXPORT CustomLayout : public QLayout
{
    Q_OBJECT
public:
    CustomLayout(QWidget* parent = Q_NULL);
    virtual ~CustomLayout();
public:
    QWidget* centralWidget() const;
    void setCentralWidget(QWidget* widget);

    QWidget* customBar() const;
    void setCustomBar(QWidget* widget, Qtitan::LayoutArea area = Qtitan::DefaultArea);

    Qtitan::LayoutArea customBarArea() const;
    void setCustomBarArea(Qtitan::LayoutArea area);

    QStatusBar* statusBar() const;
    void setStatusBar(QStatusBar* statusBar);

    static int calcItemHeight(const QWidget* wd);
public:
    virtual void addItem(QLayoutItem *item);
    virtual void setGeometry(const QRect& r);
    virtual QLayoutItem *itemAt(int index) const;
    virtual QLayoutItem *takeAt(int index);
    virtual int count() const;
    virtual QSize sizeHint() const;
    virtual QSize minimumSize() const;
    virtual void invalidate();
protected:
    void deleteCentralWidgetItem();
protected:
    Qtitan::LayoutArea m_customBarArea;
    QLayoutItem* m_customBar;
    QLayoutItem* m_centralWidget;
    QLayoutItem* m_statusBar;
    QRect m_centralWidgetRect;
    mutable QSize m_szHint;
    mutable QSize m_minSize;
private:
    Q_DISABLE_COPY(CustomLayout)
};

QTITAN_END_NAMESPACE

#endif /* QTN_CUSTOMLAYOUT_H */
