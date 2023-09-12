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

#ifndef QTN_DOCKPANELMANAGER_H
#define QTN_DOCKPANELMANAGER_H

#include <QWidget>
#include <QMainWindow>
#include "QtnDockWindow.h"
#include "QtnDockWidgetPanel.h"
#include "QtnDockDocumentPanel.h"

inline void initDockResource() { Q_INIT_RESOURCE(QtnDockResources); }

QTITAN_BEGIN_NAMESPACE

class DockPanelManagerPrivate;

class QTITAN_EXPORT DockPanelRequestedArgs : public BaseEventArgs
{
public:
    DockPanelRequestedArgs(const QString& id, bool document);
    const QString& id() const;
    bool isDocument() const;
    DockWidgetPanel* panel() const;
    void setPanel(DockWidgetPanel* panel);
private:
    QString m_id;
    bool m_document;
    DockWidgetPanel* m_panel;
};

class QTITAN_EXPORT DockPanelManager : public QObject
{
    Q_OBJECT
public:
    explicit DockPanelManager(QWidget* widget);
    explicit DockPanelManager(QMainWindow* widget);
    virtual ~DockPanelManager();
public:
    QWidget* managedWidget() const;
    DockLayoutPanel* layoutPanel() const;
    DockLayoutPanel* createDocumentLayout() const;

    QList<DockWidgetPanel*> widgetPanelList() const;
    QList<DockDocumentPanel*> documentPanelList() const;

    DockWindow* addDockWindow(DockPanelBase* panel, const QRect& rect);
    DockDocumentPanel* addDocumentPanel(const QString& title);
    //add by zhangyang 20230911 新增，未使用
//    DockDocumentPanel* addDocumentPanel(const QString& title, const QString& tooltip, const DockWidgetPanel::DockPanelFeature& feature);
    void insertDocumentPanel(DockDocumentPanel* documentPanel);
    DockWidgetPanel* addDockPanel(const QString& title, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    DockWidgetPanel* addDockPanel(const QString& title, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    void insertDockPanel(DockWidgetPanel* panel, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    void insertDockPanel(DockWidgetPanel* panel, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel = Q_NULL);
    bool restoreDockPanel(DockWidgetPanel* panel);
    void removeDockPanel(DockWidgetPanel* panel);
    void forgetDockPanel(const QString& id);
    bool isDockPanelFloat(const DockWidgetPanel* panel) const;
    void setDockPanelFloat(DockWidgetPanel* panel, bool set);
    bool isDockPanelAutoHide(const DockWidgetPanel* panel) const;
    void setDockPanelAutoHide(DockWidgetPanel* panel, bool set);
    void showDockPanel(DockWidgetPanel* panel, bool setFocus = true);
    bool isDockPanelClosed(DockWidgetPanel* panel) const;
    void closeDockPanel(DockWidgetPanel* panel);
    void clearDockPanels();

    int margin() const;
    void setMargin(int margin);

    DockWidgetPanel* activeDockPanel() const;

    QWidget* centralWidget() const;
    void setCentralWidget(QWidget* widget);

    const QString& dockWindowTitle() const;
    void setDockWindowTitle(const QString& title);

    bool isDockPanelFullContentsWhileDraggingShown() const;
    void setDockPanelFullContentsWhileDraggingShown(bool show);

    bool isDockPanelTransparentWhileDragging() const;
    void setDockPanelTransparentWhileDragging(bool transparent);

    bool isArrowMarkersShown() const;
    void setArrowMarkersShown(bool show);

    DockWidgetPanel::DockPanelFeatures defaultPaneFeatures() const;
    void setDefaultPaneFeatures(DockWidgetPanel::DockPanelFeatures features);

    void bestFit();

    void beginUpdate();
    void endUpdate();
public:
    bool saveStateToFile(const QString& fileName, bool autoFormatting = true);
    bool saveStateToDevice(QIODevice* device, bool autoFormatting = true);
    bool loadStateFromFile(const QString& fileName);
    bool loadStateFromDevice(QIODevice* device);
Q_SIGNALS:
    void aboutToShow(DockPanelBase* panel, bool& handled);
    void aboutToClose(DockPanelBase* panel, bool& handled);
    void aboutToAutoHide(DockPanelBase* panel, bool& handled);
    void aboutToFloat(DockPanelBase* panel, bool& handled);

    void dockPanelActivated(DockWidgetPanel* panel);
    void dockPanelDeactivated(DockWidgetPanel* panel);
    void panelRequested(DockPanelRequestedArgs* args);
protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);
private:
    QTN_DECLARE_PRIVATE(DockPanelManager);
    Q_DISABLE_COPY(DockPanelManager);
};

/* DockManagerUpdater */
class DockManagerUpdater
{
public:
    DockManagerUpdater(DockPanelManager* manager)
        : m_manager(manager)
    { m_manager->beginUpdate(); }

    ~DockManagerUpdater()
    { m_manager->endUpdate(); }
private:
    DockPanelManager* m_manager;
};

QTITAN_END_NAMESPACE

#endif //QTN_DOCKPANELMANAGER_H
