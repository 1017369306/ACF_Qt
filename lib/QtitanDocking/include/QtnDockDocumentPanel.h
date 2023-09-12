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

#ifndef QTN_DOCKDOCUMENTPANEL_H
#define QTN_DOCKDOCUMENTPANEL_H

#include "QtnDockWidgetPanel.h"

QTITAN_BEGIN_NAMESPACE

class DockDocumentPanelPrivate;
/* DockDocumentPanel */
class QTITAN_EXPORT DockDocumentPanel : public DockWidgetPanel
{
    Q_OBJECT
public:
    explicit DockDocumentPanel(DockPanelManager* manager);
    virtual ~DockDocumentPanel();
public:
    virtual void setParentPanel(DockPanelBase* panelBase);
protected:
    virtual void resizeEvent(QResizeEvent* event);
protected:
    DockDocumentPanel(DockDocumentPanelPrivate& dd);
private:
    QTN_DECLARE_EX_PRIVATE(DockDocumentPanel);
    Q_DISABLE_COPY(DockDocumentPanel);
};

QTITAN_END_NAMESPACE

#endif // QTN_DOCKDOCUMENTPANEL_H
