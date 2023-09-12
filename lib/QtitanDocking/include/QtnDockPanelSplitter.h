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

#ifndef QTN_DOCKSPLITTERDRAGDROP_H
#define QTN_DOCKSPLITTERDRAGDROP_H

#include "QtnDockLayoutPanelPrivate.h"

QTITAN_BEGIN_NAMESPACE

class DockPanelSplitterPrivate;
/* DockPanelSplitter */
class DockPanelSplitter : public QWidget
{
    Q_OBJECT
protected:
    explicit DockPanelSplitter(DockSplitLayoutPanel* container, DockPanelBase* first,
        DockPanelBase* second, QWidget* parent);
    virtual ~DockPanelSplitter();
public:
    bool isHorizontal() const;
    bool isPressed() const;
    DockSplitLayoutPanel* layoutPanel() const;
protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
private:
    QTN_DECLARE_PRIVATE(DockPanelSplitter)
    Q_DISABLE_COPY(DockPanelSplitter)
    friend class DockSplitLayoutPanelPrivate;
    friend class DockLayoutEngine;
};

QTITAN_END_NAMESPACE


#endif // QTN_DOCKSPLITTERDRAGDROP_H
