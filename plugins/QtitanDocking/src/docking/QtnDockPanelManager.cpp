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

#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <qevent.h>
#include <QXmlStreamWriter>

#include "QtnDockMainWindow.h"
#include "QtnDockLayoutPanelPrivate.h"
#include "QtnDockPanelManagerPrivate.h"
#include "QtnDockDocumentPanelPrivate.h"
#include "QtnDockMainWindowLayout.h"
#include "QtnDockWidgetDragDrop.h"
#include "QtnDockWindow.h"
#include "QtnDockWindowPrivate.h"
#include "QtnDockLayoutPrivate.h"
#include "QtnCommonStyle.h"
#include "QtnPlatform.h"
#include "QtnDockWidgetPanelPrivate.h"

#ifdef QTN_MEMORY_DEBUG
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

static QString qtn_generateLayoutPanelId(int i)
{
    return QString(QLatin1String("Layout%1")).arg(i);
}

void qtn_ensurePanelIDs(DockLayoutEngine* engine)
{
    if (engine->centralPanel() != Q_NULL)
        engine->centralPanel()->setId(QLatin1String("CentralPanel-id"));

    const QList<DockWidgetPanel*>& panels = engine->widgetPanelList();
    for (int i = 0; i < panels.count(); ++i)
    {
        DockWidgetPanel* panel = panels[i];
        if (!panel->id().isEmpty())
            continue;
        DockWidgetPanelPrivate::_get(panel)->panelInfo(); //Generates unique id related to panelInfo.
    }

    const QList<DockLayoutPanel*>& layouts = engine->layoutPanelList();
    for (int i = 0; i < layouts.count(); ++i)
    {
        DockLayoutPanel* panel = layouts[i];
        panel->setId(qtn_generateLayoutPanelId(i));
    }
}

extern int qtn_areaToIndex(Qtitan::LayoutArea area)
{
    if (area == Qtitan::LeftArea)
        return 0;
    if (area == Qtitan::RightArea)
        return 1;
    if (area == Qtitan::TopArea)
        return 2;
    if (area == Qtitan::BottomArea)
        return 3;
    return 0;
}

//MainLayout
#define QTN_DOCK_TAGNAME_DOCKPANELAYOUTS    QStringLiteral("DockPanelLayouts")
#define QTN_DOCK_TAGNAME_PANELS             QStringLiteral("Panels")
#define QTN_DOCK_TAGNAME_WINDOWS            QStringLiteral("Windows") 
#define QTN_DOCK_TAGNAME_LAYOUT_NUMBER      QStringLiteral("Layout_%1")
#define QTN_DOCK_TAGNAME_PANEL              QStringLiteral("Panel")
#define QTN_DOCK_TAGNAME_TOPLAYOUT          QStringLiteral("TopLayout")
#define QTN_DOCK_TAGNAME_CLIENT             QStringLiteral("Client")

#define QTN_DOCK_TAGNAME_TRACKSIZE          QStringLiteral("TrackSize")
#define QTN_DOCK_TAGNAME_MINIMUM            QStringLiteral("Minimum")
#define QTN_DOCK_TAGNAME_MAXIMUM            QStringLiteral("Maximum")

#define QTN_DOCK_ATTRNAME_ID                QStringLiteral("ID")
#define QTN_DOCK_ATTRNAME_COUNT             QStringLiteral("Count")
#define QTN_DOCK_ATTRNAME_TYPE              QStringLiteral("Type")
#define QTN_DOCK_ATTRNAME_DOCKINGCX         QStringLiteral("DockCX")
#define QTN_DOCK_ATTRNAME_DOCKINGCY         QStringLiteral("DockCY")
#define QTN_DOCK_ATTRNAME_HORIZONTAL        QStringLiteral("Horizontal") 

#define QTN_DOCK_ATTRNAME_DIRRECTION        QStringLiteral("Dirrection") 

#define QTN_DOCK_ATTRNAME_WINDOW_RECTTOP    QStringLiteral("WindowRectTop") 
#define QTN_DOCK_ATTRNAME_WINDOW_RECTBOTTOM QStringLiteral("WindowRectBottom") 
#define QTN_DOCK_ATTRNAME_WINDOW_RECTLEFT   QStringLiteral("WindowRectLeft") 
#define QTN_DOCK_ATTRNAME_WINDOW_RECTRIGHT  QStringLiteral("WindowRectRight") 

#define QTN_DOCK_ATTRVALUE_TRUE             QStringLiteral("True")
#define QTN_DOCK_ATTRVALUE_FALSE            QStringLiteral("False") 

#define QTN_DOCK_TAGNAME_LAYOUTS            QStringLiteral("Layouts")
#define QTN_DOCK_TAGNAME_DOCKINGLAYOUT      QStringLiteral("DockingLayout")
#define QTN_DOCK_TAGNAME_FLOATINGLAYOUT     QStringLiteral("FloatingLayout")
#define QTN_DOCK_TAGNAME_LASTLAYOUT         QStringLiteral("LastLayout")

QTITAN_BEGIN_NAMESPACE
/* DockCentralPanelPrivate */
class DockCentralPanelPrivate : public DockLayoutPanelPrivate
{
    QTN_DECLARE_EX_PUBLIC(DockCentralPanel)
public:
    explicit DockCentralPanelPrivate(DockLayoutEngine* layoutEngine) : DockLayoutPanelPrivate(layoutEngine) {}
    virtual ~DockCentralPanelPrivate() {}
public:
    bool isEmpty() const override
    {
        return !DockPanelManagerPrivate::_get(dockManager())->isCentralPanelVisible();
    }
    void setPanelGeometry(const QRect& rect) override
    {
        QTN_P(DockCentralPanel);
        DockLayoutPanelPrivate::setPanelGeometry(rect);
        p.setVisible(true);
    }
protected:
    QBoxLayout* m_layoutBox;
};
QTITAN_END_NAMESPACE

/* DockCentralPanel */
DockCentralPanel::DockCentralPanel(DockLayoutEngine* layoutEngine)
    : DockLayoutPanel(*new DockCentralPanelPrivate(layoutEngine))
{
    QTN_D(DockCentralPanel);
    d.m_layoutBox = new QBoxLayout(QBoxLayout::LeftToRight, this);
    d.m_layoutBox->setSpacing(0);
    d.m_layoutBox->setContentsMargins(0, 0, 0, 0);
}

DockCentralPanel::~DockCentralPanel()
{
}

DockLayoutPanel::LayoutType DockCentralPanel::layoutType() const
{
    return DockLayoutPanel::Central;
}

bool DockCentralPanel::canDock(Qtitan::DockPanelArea area, DockPanelBase* dragPanel, bool center)
{
    Q_UNUSED(area);
    Q_UNUSED(dragPanel);
    Q_UNUSED(center);
    return false;
}

QWidget* DockCentralPanel::widget() const
{
    QTN_D(const DockCentralPanel);
    return d.m_layoutBox->count() > 0 ? d.m_layoutBox->itemAt(0)->widget() : Q_NULL;
}

void DockCentralPanel::setWidget(QWidget* widget)
{
    QTN_D(DockCentralPanel);
    QWidget* oldWidget = this->widget();
    if (oldWidget && oldWidget != widget)
    {
        this->removeWidget(oldWidget);
        oldWidget->setParent(Q_NULL);
    }

    if (widget != Q_NULL)
        d.m_layoutBox->addWidget(widget);
}

void DockCentralPanel::removeWidget(QWidget* widget)
{
    QTN_D(DockCentralPanel);
    if (widget == Q_NULL)
        return;
    d.m_layoutBox->removeWidget(widget);
}

/*! \reimp */
bool DockCentralPanel::event(QEvent* event)
{
    bool result = DockPanelBase::event(event);
    return result;
}


/*! \reimp */
void DockCentralPanel::paintEvent(QPaintEvent* event)
{
    DockPanelBase::paintEvent(event);
#if __DEBUG_PAINTER
    QPainter p(this);
    QRect rect = this->rect();
    QColor clr = QColor(Qt::cyan);
    clr.setAlpha(30);
    p.fillRect(rect, clr);
    p.setPen(QPen(Qt::darkYellow, 6 + 7));
    p.drawRect(rect.adjusted(1, 1, -1, -1));
#endif
}


/* DockLayoutEngine */
DockLayoutEngine::DockLayoutEngine(DockPanelManager* manager)
    : m_manager(manager)
    , m_rootSplitPanel(Q_NULL)
    , m_centralPanel(Q_NULL)
    , m_dirty(false)
    , m_destroying(false)
{
    memset(m_wndAutoHide, 0, sizeof(m_wndAutoHide));
}

DockLayoutEngine::~DockLayoutEngine()
{
    finalization();
    destroyPanels(true);
}

static DockLayoutPanel* createCentralPanel(DockLayoutEngine* layoutEngine, bool documentLayout)
{
    DockLayoutPanel* newCentralPanel = Q_NULL;
    if (documentLayout)
        newCentralPanel = new DockDocumentLayoutPanel(layoutEngine);
    else
        newCentralPanel = new DockCentralPanel(layoutEngine);

    if (DockPanelBase* centralPanel = layoutEngine->centralPanel())
    {
        DockSplitLayoutPanel* parentSplitLayoutPanel = qobject_cast<DockSplitLayoutPanel*>(centralPanel->parentPanel());
        Q_ASSERT(parentSplitLayoutPanel != Q_NULL);
        parentSplitLayoutPanel->replacePanel(centralPanel, newCentralPanel);
        centralPanel->setParentPanel(Q_NULL);
        Q_DELETE_AND_NULL(centralPanel);
    }
    else
        layoutEngine->rootSplitPanel()->initialization(newCentralPanel, true);
    return newCentralPanel;
}

void DockLayoutEngine::initNew(bool documentLayout)
{
    if (m_rootSplitPanel == Q_NULL)
        setRootSplitPanel(new DockSplitLayoutPanel(this));
    m_centralPanel = createCentralPanel(this, documentLayout);
}

void DockLayoutEngine::finalization()
{
    m_destroying = true;
}

DockLayoutPanel* DockLayoutEngine::createLayoutPanel(const QString& type)
{
    DockLayoutPanel* layoutPanel = Q_NULL;
    if (type.contains(QStringLiteral("Qtitan")))
    {
        // centralPanelSupport_Old_Version - remove when layout record version changes
        const bool centralPanelSupport_Old_Version = type == QStringLiteral("DockCentralWidget") || 
            type == QStringLiteral("Qtitan::DockCentralWidget");

        if (centralPanelSupport_Old_Version || 
            type == QLatin1String(DockCentralPanel::staticMetaObject.className()))
            layoutPanel = new DockCentralPanel(this);
        else if (type == QLatin1String(DockDocumentLayoutPanel::staticMetaObject.className()))
                layoutPanel = new DockDocumentLayoutPanel(this);
            else if (type == QLatin1String(DockSplitLayoutPanel::staticMetaObject.className()))
                layoutPanel = new DockSplitLayoutPanel(this);
            else if (type == QLatin1String(DockTabbedLayoutPanel::staticMetaObject.className()))
                layoutPanel = new DockTabbedLayoutPanel(this);
            else if (type == QLatin1String(DockAutoHideLayoutPanel::staticMetaObject.className()))
                layoutPanel = new DockAutoHideLayoutPanel(this);
            else if (type == QLatin1String(DockDocumentTabbedLayoutPanel::staticMetaObject.className()))
                layoutPanel = new DockDocumentTabbedLayoutPanel(this);
    }

    if (layoutPanel == Q_NULL)
    {
        if (type.toUInt() == DockLayoutPanel::Central)
            layoutPanel = new DockCentralPanel(this);
        else if (type.toUInt() == DockLayoutPanel::Split)
            layoutPanel = new DockSplitLayoutPanel(this);
        else if (type.toUInt() == DockLayoutPanel::Tabbed)
            layoutPanel = new DockTabbedLayoutPanel(this);
        else if (type.toUInt() == DockLayoutPanel::AutoHide)
            layoutPanel = new DockAutoHideLayoutPanel(this);
        else if (type.toUInt() == DockLayoutPanel::Document)
            layoutPanel = new DockDocumentLayoutPanel(this);
        else if (type.toUInt() == DockLayoutPanel::DocumentTabbed)
            layoutPanel = new DockDocumentTabbedLayoutPanel(this);
    }
    return layoutPanel;
}

void DockLayoutEngine::setGeometry(const QRect& rect)
{
    m_rect = rect;
    updateGeometry();
}

void DockLayoutEngine::updateGeometry()
{
    updateChildrenGeometry(m_rect);
}

DockWidgetPanel* DockLayoutEngine::findPanelById(const QString& id) const
{
    for (QList<DockWidgetPanel*>::const_iterator it = m_widgetPanelList.begin(); it != m_widgetPanelList.end(); ++it)
    {
        DockWidgetPanel* panel = *it;
        Q_ASSERT(panel != Q_NULL);
        if (panel == Q_NULL)
            continue;
        if (panel->id() == id)
            return panel;
    }
    return Q_NULL;
}

void DockLayoutEngine::setRootSplitPanel(DockSplitLayoutPanel* rootPanel)
{
    Q_ASSERT(rootPanel != Q_NULL);
    m_rootSplitPanel = rootPanel;
    m_rootSplitPanel->setParent(m_manager->managedWidget());
    if (m_rootSplitPanel->isHidden())
        m_rootSplitPanel->setVisible(true);
}

Qtitan::LayoutArea DockLayoutEngine::calcDockPanelArea(const DockPanelBase* panelBase) const
{
    DockPanelBase* parentLayoutPanel = panelBase->parentPanel();
    if (!parentLayoutPanel || qobject_cast<const DockCentralPanel*>(panelBase))
        return Qtitan::LayoutArea::LeftArea;

    if (qtn_checkLayoutType(parentLayoutPanel, DockLayoutPanel::AutoHide))
    {
        const DockAutoHideLayoutPanel* autoHideLayout = static_cast<const DockAutoHideLayoutPanel*>(parentLayoutPanel);
        return autoHideLayout->layoutArea();
    }

    if (qtn_checkLayoutType(parentLayoutPanel, DockLayoutPanel::Split))
    {
        DockSplitLayoutPanel* dockSplitterLayout = static_cast<DockSplitLayoutPanel*>(parentLayoutPanel);
        int index = DockPanelBasePrivate::_get(dockSplitterLayout)->containsPanel(m_centralPanel);
        if (index != -1)
        {
            if (dockSplitterLayout->beforeByPosition(panelBase, index))
                return dockSplitterLayout->isHorizontal() ? Qtitan::LeftArea : Qtitan::TopArea;
            else
                return dockSplitterLayout->isHorizontal() ? Qtitan::RightArea : Qtitan::BottomArea;
        }
    }
    Q_ASSERT(qobject_cast<DockPanelBase*>(parentLayoutPanel));
    return calcDockPanelArea(parentLayoutPanel);
}

void DockLayoutEngine::saveStateToDevice(QIODevice* device, bool autoFormatting)
{
    QXmlStreamWriter xmlwriter(device);
    xmlwriter.setAutoFormatting(autoFormatting);
    xmlwriter.writeStartDocument(QStringLiteral("1.0"));
    xmlwriter.writeNamespace(QStringLiteral("https://www.devmachines.com/qt/qtitan"), QStringLiteral("Qtitan"));
    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_DOCKPANELAYOUTS);
    xmlwriter.writeAttribute(QStringLiteral("type"), QStringLiteral("layoutPanel"));
    xmlwriter.writeAttribute(QStringLiteral("version"), QStringLiteral("1.3"));

    setLayoutDirty();
    deleteEmptyDockWidgets();

    qtn_ensurePanelIDs(this);

    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANELS);
    xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_COUNT, QString::number(widgetPanelList().size() + layoutPanelList().size()));

    //Save layout panels
    for (QList<DockLayoutPanel*>::const_iterator it = layoutPanelList().constBegin(); it != layoutPanelList().constEnd(); ++it)
    {
        DockLayoutPanel* panel = *it;
        Q_ASSERT(panel != Q_NULL);
        if (panel == Q_NULL)
            continue; 

        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANEL);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, panel->id());

        if (qobject_cast<DockDocumentTabbedLayoutPanel*>(panel))
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_TYPE, QString::number(DockLayoutPanel::DocumentTabbed));
        else
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_TYPE, QString::number(panel->layoutType()));

        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_DOCKINGCX, QString::number(DockPanelBasePrivate::_get(panel)->m_sizeDocking.width()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_DOCKINGCY, QString::number(DockPanelBasePrivate::_get(panel)->m_sizeDocking.height()));
        
        if (qtn_checkLayoutType(panel, DockLayoutPanel::Split) || qtn_checkLayoutType(panel, DockLayoutPanel::Document))
        {
            DockSplitLayoutPanel* splitPanel = static_cast<DockSplitLayoutPanel*>(panel);
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_HORIZONTAL, splitPanel->isHorizontal() ? QTN_DOCK_ATTRVALUE_TRUE : QTN_DOCK_ATTRVALUE_FALSE);
            xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANELS);
            
            const QList<DockPanelBase*>& childPanels = splitPanel->panels();
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_COUNT, QString::number(childPanels.size()));
            for (QList<DockPanelBase*>::const_iterator it = childPanels.constBegin(); it != childPanels.constEnd(); ++it)
            {
                DockPanelBase* child = *it;
                xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANEL);
                xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, child->id());
                xmlwriter.writeEndElement();
            }
            xmlwriter.writeEndElement();
        }
        else if (qtn_checkLayoutType(panel, DockLayoutPanel::Tabbed))
        {
            DockTabbedLayoutPanel* tabbedPanel = static_cast<DockTabbedLayoutPanel*>(panel);
            xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANELS);

            const QList<DockPanelBase*>& childPanels = tabbedPanel->panels();
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_COUNT, QString::number(childPanels.size()));
            for (QList<DockPanelBase*>::const_iterator it = childPanels.constBegin(); it != childPanels.constEnd(); ++it)
            {
                DockPanelBase* child = *it;
                xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANEL);
                xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, child->id());
                xmlwriter.writeEndElement();
            }
            xmlwriter.writeEndElement();
        }
        else if (qtn_checkLayoutType(panel, DockLayoutPanel::AutoHide))
        {
            DockAutoHideLayoutPanel* autoHidePanel = static_cast<DockAutoHideLayoutPanel*>(panel);
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_DIRRECTION, QString::number(autoHidePanel->layoutArea()));
            xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANELS);

            const QList<DockPanelBase*>& childPanels = autoHidePanel->panels();
            xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_COUNT, QString::number(childPanels.size()));

            int index = 1;
            for (QList<DockPanelBase*>::const_iterator it = childPanels.constBegin(); it != childPanels.constEnd(); ++it)
            {
                DockPanelBase* child = *it;
                QString src = QString(QTN_DOCK_TAGNAME_LAYOUT_NUMBER).arg(index++);
                xmlwriter.writeStartElement(src);
                xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, child->id());
                xmlwriter.writeEndElement();
            }
            xmlwriter.writeEndElement();
        }
        xmlwriter.writeEndElement();
    }

    //Save widget panels
    for (QList<DockWidgetPanel*>::const_iterator it = widgetPanelList().constBegin(); it != widgetPanelList().constEnd(); ++it)
    {
        DockWidgetPanel* panel = *it;
        Q_ASSERT(panel != Q_NULL);
        if (panel == Q_NULL)
            continue;
        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANEL);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, panel->id());

        QString className(QLatin1String(panel->metaObject()->className()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_TYPE, className);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_DOCKINGCX, QString::number(DockPanelBasePrivate::_get(panel)->m_sizeDocking.width()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_DOCKINGCY, QString::number(DockPanelBasePrivate::_get(panel)->m_sizeDocking.height()));

        const DockPanelInfo& panelInfo = DockWidgetPanelPrivate::_get(panel)->panelInfo();

        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_LAYOUTS);

        QString id = panelInfo.dockingOwner != Q_NULL ? panelInfo.dockingOwner->id() : QString();
        xmlwriter.writeEmptyElement(QTN_DOCK_TAGNAME_DOCKINGLAYOUT);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, id);

        id = panelInfo.floatingOwner != Q_NULL ? panelInfo.floatingOwner->id() : QString();
        xmlwriter.writeEmptyElement(QTN_DOCK_TAGNAME_FLOATINGLAYOUT);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, id);

        id = panelInfo.lastOwner != Q_NULL ? panelInfo.lastOwner->id() : QString();
        xmlwriter.writeEmptyElement(QTN_DOCK_TAGNAME_LASTLAYOUT);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, id);

        xmlwriter.writeEndElement(); // QTN_DOCK_TAGNAME_LAYOUTS
        xmlwriter.writeEndElement(); //QTN_DOCK_TAGNAME_PANEL
    }


    xmlwriter.writeEndElement(); // END - QTN_DOCK_TAGNAME_PANELS

    QList<DockWindow*> windowList;
    for (int i = 0, count = m_windowList.count(); count > i; ++i)
    {
        DockWindow* dockWindow = m_windowList[i];
        if (dockWindow && dockWindow->layoutPanel())
            windowList.push_front(dockWindow);
    }

    // create a new tag for floating windows
    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_WINDOWS); // BEGIN
    // save the number of floating windows
    xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_COUNT, QString::number(windowList.size()));
    // save all floating window data
    for (int i = 0, count = windowList.count(); count > i; ++i)
    {
        DockWindow* dockWindow = windowList[i];
        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_PANEL);

        QString type(QLatin1String(dockWindow->metaObject()->className()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_TYPE, type);

        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_DOCKINGCX, QString::number(DockWindowPrivate::_get(dockWindow)->m_sizeDocking.width()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_DOCKINGCY, QString::number(DockWindowPrivate::_get(dockWindow)->m_sizeDocking.height()));

        QRect rect = dockWindow->geometry();
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_WINDOW_RECTTOP, QString::number(rect.top()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_WINDOW_RECTBOTTOM, QString::number(rect.bottom()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_WINDOW_RECTLEFT, QString::number(rect.left()));
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_WINDOW_RECTRIGHT, QString::number(rect.right()));

        xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_TOPLAYOUT);
        xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, dockWindow->layoutPanel() != Q_NULL ? 
            dockWindow->layoutPanel()->id() : QString());
        xmlwriter.writeEndElement();

        xmlwriter.writeEndElement();
    }
    xmlwriter.writeEndElement(); // END - QTN_DOCK_TAGNAME_WINDOWS

    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_TOPLAYOUT); // BEGIN
    xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, m_rootSplitPanel->id());
    xmlwriter.writeEndElement(); // END - QTN_DOCK_TAGNAME_TOPLAYOUT

    xmlwriter.writeStartElement(QTN_DOCK_TAGNAME_CLIENT);
    xmlwriter.writeAttribute(QTN_DOCK_ATTRNAME_ID, m_centralPanel->id());
    xmlwriter.writeEndElement(); // QTN_DOCK_TAGNAME_CLIENT

    xmlwriter.writeEndElement();
    xmlwriter.writeEndDocument();
}

static Qtitan::LayoutArea qtn_convertPositionToAreaSupport_Old_Version(QInternal::DockPosition pos)
{
    if (pos == QInternal::LeftDock)
        return Qtitan::LeftArea;
    if (pos == QInternal::RightDock)
        return Qtitan::RightArea;
    if (pos == QInternal::TopDock)
        return Qtitan::TopArea;
    if (pos == QInternal::BottomDock)
        return Qtitan::BottomArea;
    return Qtitan::DefaultArea;
}

static bool qtn_isWidgetPanel(const QString& type, bool& document)
{
    document = false;
    if (type == QLatin1String("Qtitan::DockWidgetPanel"))
        return true;
    if (type == QLatin1String("Qtitan::DockDocumentPanel"))
    {
        document = true;
        return true;
    }
    return false;
}

bool DockLayoutEngine::loadStateFromDevice(QIODevice* device)
{
    QXmlStreamReader xmlreader(device);
    if (xmlreader.readNext() != QXmlStreamReader::StartDocument)
        return false;
    if (!xmlreader.readNextStartElement())
        return false;
    if (xmlreader.name() != QTN_DOCK_TAGNAME_DOCKPANELAYOUTS)
        return false;

    QXmlStreamAttributes attrs = xmlreader.attributes();
    QString version = attrs.hasAttribute(QStringLiteral("version")) ? attrs.value(QStringLiteral("version")).toString() : QString();
    if (version != QStringLiteral("1.1") && version != QStringLiteral("1.2") && version != QStringLiteral("1.3"))
        return false;

    if (!xmlreader.readNextStartElement())
        return false;
    if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_PANELS)
        return false;
    QString value = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_COUNT).toString();
    if (value.isEmpty())
        return false;

    const int panelCount = value.toInt();
    if (panelCount < 2)
        return false;

    DockPanelManagerPrivate* manager = DockPanelManagerPrivate::_get(m_manager);
    QMap<QString, DockPanelBase*> allPanels;

    m_extraLoadData.clear();

    for (int i = 0; i < panelCount; ++i)
    {
        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_PANEL)
            return false;
        QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
        if (id.isEmpty())
            continue;
        QString type = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_TYPE).toString();
        if (type.isEmpty())
            continue;
        
        bool document;
        const bool isWidgetPanel = qtn_isWidgetPanel(type, document);
        if (isWidgetPanel)
        {
            DockWidgetPanel* panel = manager->dockLayoutEngine()->findPanelById(id);
            // supporting old version 1.1
            if (panel == Q_NULL)
                panel = manager->dockLayoutEngine()->findPanelById(QTN_DOCK_TAGNAME_PANEL + QStringLiteral("_") + id);
            // supporting old version 1.2
            if (panel == Q_NULL)
                panel = manager->dockLayoutEngine()->findPanelById(QTN_DOCK_TAGNAME_PANEL + id);
            if (panel != Q_NULL)
                allPanels.insert(id, panel);

            m_extraLoadData[Q_NULL].append(id);
        }
        else
        {
            DockLayoutPanel* panel = createLayoutPanel(type);
            if (panel != Q_NULL)
            {
                panel->setId(id);
                allPanels.insert(id, panel);
            }
        }
        xmlreader.skipCurrentElement();
    }

    xmlreader.clear();
    device->reset();
    xmlreader.setDevice(device);

    if (xmlreader.readNext() != QXmlStreamReader::StartDocument)
        return false;
    if (!xmlreader.readNextStartElement())
        return false;
    if (xmlreader.name() != QTN_DOCK_TAGNAME_DOCKPANELAYOUTS)
        return false;
    if (!xmlreader.readNextStartElement()) // QTN_DOCK_TAGNAME_PANELS
        return false;

    for (int i = 0; i < panelCount; ++i)
    {
        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_PANEL)
            return false;
        QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
        if (id.isEmpty())
            continue;
        QString type = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_TYPE).toString();
        if (type.isEmpty())
            continue;

        DockPanelBase* panelBase = Q_NULL;
        if (allPanels.contains(id))
            panelBase = allPanels[id];

#if 0
        if (panelBase == Q_NULL)
        {
            xmlreader.skipCurrentElement();
            continue;
        }
#endif
        if (panelBase != Q_NULL)
        {
            const int dockCX = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_DOCKINGCX).toString().toInt();
            const int dockCY = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_DOCKINGCY).toString().toInt();
            DockPanelBasePrivate::_get(panelBase)->m_sizeDocking = QSize(dockCX, dockCY);
        }

        bool document;
        const bool isWidgetPanel = qtn_isWidgetPanel(type, document);
        if (isWidgetPanel)
        {
            if (!xmlreader.readNextStartElement()) // QTN_DOCK_TAGNAME_LAYOUTS 
                return false;

            if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_LAYOUTS)
                return false;

            {
                DockPanelInfo panelInfo(document);
                {
                    if (!xmlreader.readNextStartElement()) // QTN_DOCK_TAGNAME_DOCKINGLAYOUT
                        return false;
                    if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_DOCKINGLAYOUT)
                        return false;
                    QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
                    panelInfo.dockingOwner = allPanels.value(id);
                    xmlreader.skipCurrentElement();
                }

                {
                    if (!xmlreader.readNextStartElement()) // QTN_DOCK_TAGNAME_FLOATINGLAYOUT
                        return false;
                    if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_FLOATINGLAYOUT)
                        return false;
                    QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
                    panelInfo.floatingOwner = allPanels.value(id);
                    xmlreader.skipCurrentElement();
                }

                {
                    if (!xmlreader.readNextStartElement()) // QTN_DOCK_TAGNAME_FLOATINGLAYOUT
                        return false;
                    if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_LASTLAYOUT)
                        return false;
                    QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
                    panelInfo.lastOwner = allPanels.value(id);
                    xmlreader.skipCurrentElement();
                }

                if (m_panelInfoMap.contains(id))
                {
                    Q_ASSERT(false);
                    return false;
                }
                m_panelInfoMap.insert(id, panelInfo);
            }

            xmlreader.skipCurrentElement();

            if (version == QStringLiteral("1.1") || version == QStringLiteral("1.2"))
            {
                if (!xmlreader.readNextStartElement()) //QTN_DOCK_TAGNAME_TRACKSIZE
                    return false;
                if (!xmlreader.readNextStartElement()) //QTN_DOCK_TAGNAME_MINIMUM
                    return false;
                xmlreader.skipCurrentElement();
                if (!xmlreader.readNextStartElement()) //QTN_DOCK_TAGNAME_MAXIMUM
                    return false;
                xmlreader.skipCurrentElement();
                xmlreader.skipCurrentElement();
            }
        }
        else if (qtn_checkLayoutType(panelBase, DockLayoutPanel::Split) || qtn_checkLayoutType(panelBase, DockLayoutPanel::Document))
        {
            DockSplitLayoutPanel* splitPanel = static_cast<DockSplitLayoutPanel*>(panelBase);
            QString strDirection = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_HORIZONTAL).toString();
            splitPanel->setHorizontal(strDirection == QTN_DOCK_ATTRVALUE_TRUE ? true : false);

            if (!xmlreader.readNextStartElement())
                return false;
            if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_PANELS)
                return false;

            int countSplit = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_COUNT).toString().toInt();
            for (int iSplit = 1; countSplit >= iSplit; iSplit++)
            {
                if (!xmlreader.readNextStartElement())
                    return false;

                QString strPaneNumber = xmlreader.name().toString();
                if (strPaneNumber.isEmpty())
                    return false;

                QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
                if (!allPanels.contains(id))
                {
                    xmlreader.skipCurrentElement();
                    continue;
                }

                DockPanelBase* panel = allPanels.value(id);
                if (panel == Q_NULL)
                {
                    xmlreader.skipCurrentElement();
                    continue;
                }
                panel->setParentPanel(splitPanel);
                xmlreader.skipCurrentElement();
            }
            xmlreader.skipCurrentElement();
        }
        else if (qtn_checkLayoutType(panelBase, DockLayoutPanel::Tabbed))
        {
            DockTabbedLayoutPanel* tabbedPanel = static_cast<DockTabbedLayoutPanel*>(panelBase);

            if (!xmlreader.readNextStartElement())
                return false;
            if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_PANELS)
                return false;

            const int childPanelCount = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_COUNT).toString().toInt();
            if (childPanelCount == 0)
                tabbedPanel->setPreserved(true);

            for (int i = 0; i < childPanelCount; ++i)
            {
                if (!xmlreader.readNextStartElement())
                    return false;
                QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
                if (id.isEmpty())
                    return false;
                xmlreader.skipCurrentElement();

                m_extraLoadData[tabbedPanel].append(id);
                m_extraLoadData[Q_NULL].removeAll(id);
            }

            xmlreader.skipCurrentElement();
        }
        else if (qtn_checkLayoutType(panelBase, DockLayoutPanel::AutoHide))
        {
            DockAutoHideLayoutPanel* autoHidePanel = static_cast<DockAutoHideLayoutPanel*>(panelBase);
            QString value = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_DIRRECTION).toString();

            Qtitan::LayoutArea layoutArea = static_cast<Qtitan::LayoutArea>(value.toInt());
            if (version == QStringLiteral("1.1"))
                layoutArea = qtn_convertPositionToAreaSupport_Old_Version(static_cast<QInternal::DockPosition>(value.toInt()));
            autoHidePanel->setLayoutArea(layoutArea);

            if (!xmlreader.readNextStartElement())
                return false;
            if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_PANELS)
                return false;

            const int childPanelCount = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_COUNT).toString().toInt();
            for (int i = 0; i < childPanelCount; ++i)
            {
                if (!xmlreader.readNextStartElement())
                    return false;

                QString strPaneNumber = xmlreader.name().toString();
                if (strPaneNumber.isEmpty())
                    return false;

                QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
                if (version == QStringLiteral("1.1") || version == QStringLiteral("1.2"))
                    id = id.remove(QTN_DOCK_TAGNAME_PANEL);

                if (!allPanels.contains(id))
                {
                    xmlreader.skipCurrentElement();
                    continue;
                }

                DockPanelBase* panelBase = allPanels.value(id);
                if (panelBase == Q_NULL)
                {
                    xmlreader.skipCurrentElement();
                    continue;
                }
                panelBase->setParentPanel(autoHidePanel);
                panelBase->setGeometry(QRect(0, 0, 0, 0));
                xmlreader.skipCurrentElement();
            }

            int index = qtn_areaToIndex(autoHidePanel->layoutArea());
            if (index < 0 || index >= 4)
            {
                Q_ASSERT(false);
                return false;
            }
            if (m_wndAutoHide[index] != Q_NULL)
            {
                Q_ASSERT(false);
                return false;
            }
            m_wndAutoHide[index] = autoHidePanel;

            xmlreader.skipCurrentElement();
        }
        xmlreader.skipCurrentElement();
    }
    xmlreader.skipCurrentElement();

    if (!xmlreader.readNextStartElement())
        return false;
    if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_WINDOWS)
        return false;
    value = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_COUNT).toString();
    if (value.isEmpty())
        return false;

    const int windowsCount = value.toInt();
    for (int i = 0; i < windowsCount; i++)
    {
        if (!xmlreader.readNextStartElement())
            return false;

        QString tagName = xmlreader.name().toString();
        if (tagName.isEmpty())
            return false;

        QString type = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_TYPE).toString();
        DockWindow* dockWindow = new DockWindow(this);

        const int dockCX = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_DOCKINGCX).toString().toInt();
        const int dockCY = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_DOCKINGCY).toString().toInt();
        DockWindowPrivate::_get(dockWindow)->m_sizeDocking = QSize(dockCX, dockCY);

        int rectTop = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_WINDOW_RECTTOP).toString().toInt();
        int rectBottom = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_WINDOW_RECTBOTTOM).toString().toInt();
        int rectLeft = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_WINDOW_RECTLEFT).toString().toInt();
        int rectRight = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_WINDOW_RECTRIGHT).toString().toInt();
        QRect rect(QPoint(rectLeft, rectTop), QPoint(rectRight, rectBottom));
        dockWindow->setGeometry(rect);

        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_TOPLAYOUT)
            return false;

        QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
        if (!allPanels.contains(id))
            return false;

        DockPanelBase* panel = allPanels.value(id);
        DockSplitLayoutPanel* rootPanel = qobject_cast<DockSplitLayoutPanel*>(panel);
        if (rootPanel == Q_NULL)
            return false;
        DockWindowPrivate::_get(dockWindow)->setRootPanel(rootPanel);
        xmlreader.skipCurrentElement();
        xmlreader.skipCurrentElement();
    }
    xmlreader.skipCurrentElement();

    {
        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_TOPLAYOUT)
            return false;
        QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
        DockPanelBase* panel = allPanels.value(id);
        m_rootSplitPanel = qobject_cast<DockSplitLayoutPanel *>(panel);
        if (m_rootSplitPanel == Q_NULL)
            return false;
        xmlreader.skipCurrentElement();
    }

    {
        if (!xmlreader.readNextStartElement())
            return false;
        if (xmlreader.name().toString() != QTN_DOCK_TAGNAME_CLIENT)
            return false;
        QString id = xmlreader.attributes().value(QTN_DOCK_ATTRNAME_ID).toString();
        DockPanelBase* panel = allPanels.value(id);
        m_centralPanel = qobject_cast<DockLayoutPanel*>(panel);
        if (m_centralPanel == Q_NULL)
            return false;
        xmlreader.skipCurrentElement();
    }
    xmlreader.skipCurrentElement();
    if (xmlreader.readNext() != QXmlStreamReader::EndDocument)
        return false;

    return true;
}

void DockLayoutEngine::addWidgetPanel(DockWidgetPanel* panel)
{
    Q_ASSERT(!m_widgetPanelList.contains(panel));
    m_widgetPanelList.append(panel);
}

void DockLayoutEngine::removeWidgetPanel(DockWidgetPanel* panel)
{
    Q_ASSERT(m_widgetPanelList.contains(panel));
    m_widgetPanelList.removeOne(panel);
    removePanelInfo(panel->id());
}

void DockLayoutEngine::addLayoutPanel(DockLayoutPanel* panel)
{
    Q_ASSERT(!m_layoutPanelList.contains(panel));
    m_layoutPanelList.append(panel);
}

void DockLayoutEngine::removeLayoutPanel(DockLayoutPanel* panel)
{
    Q_ASSERT(m_layoutPanelList.contains(panel));
    int index = m_layoutPanelList.indexOf(panel);
    if (index != -1)
        m_layoutPanelList[index] = Q_NULL;

    if (!isDestroying())
    {
        for (QHash<QString, DockPanelInfo>::iterator it = m_panelInfoMap.begin();
            it != m_panelInfoMap.end(); ++it)
        {
            DockPanelInfo& panelInfo = it.value();
            if (panelInfo.dockingOwner == panel)
                panelInfo.dockingOwner = Q_NULL;
            if (panelInfo.floatingOwner == panel)
                panelInfo.floatingOwner = Q_NULL;
            if (panelInfo.lastOwner == panel)
                panelInfo.lastOwner = Q_NULL;
        }
    }
}

void DockLayoutEngine::attachDockWindow(DockWindow* window)
{
    if (!m_windowList.contains(window))
        m_windowList.append(window);
}

void DockLayoutEngine::resetDockWindow(DockWindow* window)
{
    int index = m_windowList.indexOf(window);
    if (index != -1)
        m_windowList[index] = Q_NULL;
}

bool DockLayoutEngine::deleteDockWindow(DockWindow* window)
{
    Q_ASSERT(window != Q_NULL);
    if (window->layoutPanel() == Q_NULL)
    {
        Q_DELETE_AND_NULL(window)
        return true;
    }
    return false;
}

void DockLayoutEngine::stackUnderDockWindow(DockWindow* window)
{
    int index = m_windowList.indexOf(window);
    if (index != -1 && index != 0)
    {
        m_windowList.removeAt(index);
        m_windowList.prepend(window);
    }
}

void DockLayoutEngine::destroyPanels(bool all)
{
    m_rootSplitPanel = Q_NULL;
    QList<DockWidgetPanel*> panels = widgetPanelList();
    for (QList<DockWidgetPanel*>::iterator it = panels.begin(); it != panels.end(); ++it)
        (*it)->setParentPanel(Q_NULL);

    QList<DockLayoutPanel*> layouts = layoutPanelList();
    for (QList<DockLayoutPanel*>::iterator it = layouts.begin(); it != layouts.end(); ++it)
        (*it)->setParentPanel(Q_NULL);

    while (layouts.count() > 0)
        delete layouts.takeFirst();

    // TODO: after removing all layout-panels must reset the auto-hide-panels to zero
    memset(m_wndAutoHide, 0, sizeof(m_wndAutoHide));

    // removing dock-windows
    while (!m_windowList.isEmpty())
        delete m_windowList.takeFirst();

    if (all)
    {
        // removing widget-panels
        while (panels.count() > 0)
            delete panels.takeFirst();
    }
}

void DockLayoutEngine::copyLayoutEngine(DockLayoutEngine* oldlayoutEngine)
{
    setRootSplitPanel(m_rootSplitPanel);

    //oldlayoutEngine->centralPanel()->setParentPanel(Q_NULL);
    QWidget* centralWidget = Q_NULL;
    if (DockCentralPanel* oldCentralPanel = qobject_cast<DockCentralPanel*>(oldlayoutEngine->centralPanel()))
    {
        centralWidget = oldCentralPanel->widget();
        oldCentralPanel->setWidget(Q_NULL);
    }

    if (DockCentralPanel* newCentralPanel = qobject_cast<DockCentralPanel*>(centralPanel()))
        newCentralPanel->setWidget(centralWidget);

    QList<DockWidgetPanel*> panels = oldlayoutEngine->widgetPanelList();
    for (QList<DockWidgetPanel*>::const_iterator it = panels.constBegin(); it != panels.constEnd(); ++it)
    {
        DockWidgetPanel* panel = *it;
        panel->setParentPanel(Q_NULL);
        DockWidgetPanelPrivate::_get(panel)->setLayoutEngine(this);
    }

    for (QHash<DockLayoutPanel*, QStringList>::const_iterator it = m_extraLoadData.constBegin(); it != m_extraLoadData.constEnd(); ++it)
    {
        DockLayoutPanel* parentPanel = it.key();
        for (QStringList::const_iterator jt = it.value().constBegin(); jt != it.value().constEnd(); ++jt)
        {
            QString id = *jt;
            QHash<QString, DockPanelInfo>::iterator it = m_panelInfoMap.find(id);
            if (it == m_panelInfoMap.end())
            {
                //We have no info loaded about this child panel.
                continue;
            }

            DockWidgetPanel* panel = findPanelById(id);
            if (panel == Q_NULL)
            {
                DockPanelRequestedArgs args(id, it.value().document);
                emit dockManager()->panelRequested(&args);
                if (args.isHandled())
                {
                    panel = args.panel();
                    panel->setId(id);
                }
                if (panel == Q_NULL)
                {
                    it.value().lastOwner = parentPanel;
                    continue;
                }
            }
            panel->setParentPanel(parentPanel);
        }
    }
    m_extraLoadData.clear();
}

bool DockLayoutEngine::deleteTabbedLayout(const QMap<DockPanelBase*, bool>& map, DockTabbedLayoutPanel* panel)
{
    DockTabbedLayoutPanel* tabbedLayout = panel;
    Q_ASSERT(tabbedLayout != Q_NULL);
    if (!map.contains(tabbedLayout))
    {
        tabbedLayout->setParentPanel(Q_NULL);
        Q_DELETE_AND_NULL(tabbedLayout);
        return true;
    }
    return false;
}

bool DockLayoutEngine::deleteSplitLayout(DockSplitLayoutPanel* panel)
{
    Q_ASSERT(panel != Q_NULL);

    DockPanelBase* parentPanel = panel->parentPanel();
    if (m_centralPanel == parentPanel)
        return false;

    if (panel->childCount() == 0)
    {
        panel->setParentPanel(Q_NULL);
        Q_DELETE_AND_NULL(panel)
        return true;
    }
    else if (parentPanel && (qtn_checkLayoutType(panel, DockLayoutPanel::Split)) &&
             (((DockSplitLayoutPanel*)panel)->childCount() == 1) &&
             (((DockSplitLayoutPanel*)parentPanel)->childCount() == 1))
    {
        DockSplitLayoutPanelPrivate::_get(panel)->replaceChildToParent();
        Q_DELETE_AND_NULL(panel);
        return true;
    }
    return false;
}

bool DockLayoutEngine::deleteDocumentTabbedLayout(const QMap<DockPanelBase*, bool>& map, DockDocumentTabbedLayoutPanel* tabbedDocumentLayout)
{
    Q_ASSERT(tabbedDocumentLayout != Q_NULL);
    DockTabbedLayoutPanel* tabbedLayout = tabbedDocumentLayout;
    if (!map.contains(tabbedLayout))
    {
        tabbedLayout->setParentPanel(Q_NULL);
        Q_DELETE_AND_NULL(tabbedLayout);
        return true;
    }
    return false;
}

bool DockLayoutEngine::deleteDocumentLayout(DockDocumentLayoutPanel* documentLayout)
{
    Q_ASSERT(documentLayout != Q_NULL);
    if (m_centralPanel == documentLayout)
        return false;

    if (qobject_cast<DockWindow*>(documentLayout->topDockWindow()) != Q_NULL && !documentLayout->isEmpty())
        return false;

    DockPanelBase* parentPanel = documentLayout->parentPanel();
    if (documentLayout->childCount() == 0)
    {
        documentLayout->setParentPanel(Q_NULL);
        Q_DELETE_AND_NULL(documentLayout);
        return true;
    }
    else if ((qtn_checkLayoutType(documentLayout->parentPanel(), DockLayoutPanel::Document)) &&
            (((DockSplitLayoutPanel*)documentLayout)->childCount() == 1) &&
            (((DockSplitLayoutPanel*)parentPanel)->childCount() == 1))
    {
        DockSplitLayoutPanelPrivate::_get(documentLayout)->replaceChildToParent();
        Q_DELETE_AND_NULL(documentLayout)
        return true;
    }
    return false;
}

void DockLayoutEngine::deleteEmptyDockWidgets()
{
    if (!m_dirty)
        return;

    QMap<DockPanelBase*, bool> map;
    for (QList<DockWidgetPanel*>::iterator it = m_widgetPanelList.begin(); it != m_widgetPanelList.end(); ++it)
    {
        DockWidgetPanel* widgetPanel = (*it);
        if (widgetPanel->parentPanel() != Q_NULL)
            map.insert(widgetPanel->parentPanel(), true);
    }

    for (QHash<QString, DockPanelInfo>::const_iterator it = m_panelInfoMap.constBegin(); it != m_panelInfoMap.constEnd(); ++it)
    {
        const DockPanelInfo& panelInfo = it.value();
        if (panelInfo.lastOwner != Q_NULL)
            map.insert(panelInfo.lastOwner, true);
        if (panelInfo.dockingOwner != Q_NULL)
            map.insert(panelInfo.dockingOwner, true);
        if (panelInfo.floatingOwner != Q_NULL)
            map.insert(panelInfo.floatingOwner, true);
    }

    bool deleteLayout = true;
    while (deleteLayout)
    {
        deleteLayout = false;
        for (QList<DockLayoutPanel*>::const_iterator it = m_layoutPanelList.constBegin(); it != m_layoutPanelList.constEnd(); ++it)
        {
            DockLayoutPanel* panel = (*it);
            if (panel == Q_NULL)
                continue;
            if (qtn_checkLayoutType(panel, DockLayoutPanel::Tabbed) && !qobject_cast<DockDocumentTabbedLayoutPanel*>(panel))
                deleteLayout = deleteTabbedLayout(map, qobject_cast<DockTabbedLayoutPanel*>(panel));
            else if (qtn_checkLayoutType(panel, DockLayoutPanel::Split) && !qobject_cast<DockDocumentLayoutPanel*>(panel))
                deleteLayout = deleteSplitLayout(qobject_cast<DockSplitLayoutPanel*>(panel));
            else if (DockDocumentTabbedLayoutPanel* documentTabbed = qobject_cast<DockDocumentTabbedLayoutPanel*>(panel))
                deleteLayout = deleteDocumentTabbedLayout(map, documentTabbed);
            else if (DockDocumentLayoutPanel* documentSplitLayout = qobject_cast<DockDocumentLayoutPanel*>(panel))
                deleteLayout = deleteDocumentLayout(documentSplitLayout);
        }
    }

    for (QList<DockWindow*>::const_iterator it = m_windowList.constBegin(); it != m_windowList.constEnd(); ++it)
    {
        DockWindow* window = (*it);
        if (window == Q_NULL)
            continue;
        DockWindowPrivate::_get(window)->checkToFinalize();
        deleteDockWindow(window);
    }

    // Remove all null layouts panel
    m_layoutPanelList.removeAll(Q_NULL);
    // Remove all null windows panel
    m_windowList.removeAll(Q_NULL);
    m_dirty = false;
}

void DockLayoutEngine::addDockWidgetsTo(DockTabbedLayoutPanel* tabbed, QList<DockPanelBase*>& lst, int features)
{
    for (QList<DockPanelBase*>::iterator it = lst.begin(); it != lst.end(); ++it)
    {
        DockWidgetPanel* panel = (DockWidgetPanel*)(*it);
        if ((panel->features() & features) != 0)
            panel->setParentPanel(tabbed);
    }
}

void DockLayoutEngine::updateChildrenGeometry(const QRect& rect)
{
    if (isDestroying())
        return;

    QRect rc = rect;
    if (DockAutoHideLayoutPanel* autoHidePanel = m_wndAutoHide[0])
        rc = autoHidePanel->setPanelGeometryEx(rc);
    if (DockAutoHideLayoutPanel* autoHidePanel = m_wndAutoHide[1])
        rc = autoHidePanel->setPanelGeometryEx(rc);
    if (DockAutoHideLayoutPanel* autoHidePanel = m_wndAutoHide[2])
        rc = autoHidePanel->setPanelGeometryEx(rc);
    if (DockAutoHideLayoutPanel* autoHidePanel = m_wndAutoHide[3])
        rc = autoHidePanel->setPanelGeometryEx(rc);

    int margin = m_manager->margin();
    QRect rcClientMargins(margin, margin, margin, margin);
    repositionMargins(rc, rcClientMargins);

    if (m_rootSplitPanel)
        DockPanelBasePrivate::_get(m_rootSplitPanel)->setPanelGeometry(rc);
}

bool DockLayoutEngine::findTabbedPaneToHide(DockAutoHideLayoutPanel* autoHide, DockPanelBase* panel)
{
    QList<DockPanelBase*> lst = panel->findPanels(DockWidgetPanel::staticMetaObject.className());
    if (lst.isEmpty())
        return false;

    DockWidgetPanel* dockPrimary = (DockWidgetPanel*)lst.at(0);
    const DockPanelInfo& panelInfo = DockWidgetPanelPrivate::_get(dockPrimary)->panelInfo();
    if (panelInfo.dockingOwner == Q_NULL)
        return false;

    const QList<DockPanelBase*>& lstDockWidgets = autoHide->panels();
    for (int i = 0, count = lstDockWidgets.count(); count > i; i++)
    {
        DockTabbedLayoutPanel* tabbed = (DockTabbedLayoutPanel*)lstDockWidgets.at(i);
        if (tabbed->panels().count() == 0)
            continue;
        if (DockWidgetPanel* child = (DockWidgetPanel*)tabbed->panels().at(0))
        {
            const DockPanelInfo& childPanelInfo = DockWidgetPanelPrivate::_get(child)->panelInfo();
            if (childPanelInfo.dockingOwner == panelInfo.dockingOwner)
            {
                addDockWidgetsTo(tabbed, lst, DockWidgetPanel::DockPanelHideable);
                return true;
            }
        }
    }
    return false;
}

void DockLayoutEngine::autoHideDockPanel(DockPanelBase* panelBase)
{
    Qtitan::LayoutArea layoutArea = calcDockPanelArea(panelBase);
    const int index = qtn_areaToIndex(layoutArea);
    if (m_wndAutoHide[index] == Q_NULL)
    {
        m_wndAutoHide[index] = new DockAutoHideLayoutPanel(this);
        m_wndAutoHide[index]->setLayoutArea(layoutArea);
    }

    if (findTabbedPaneToHide(m_wndAutoHide[index], panelBase))
        return;

    if (DockWidgetPanel* dockWidget = qobject_cast<DockWidgetPanel*>(panelBase))
    {
        panelBase->setParentPanel(Q_NULL);
        DockTabbedLayoutPanel* tabbed = new DockTabbedLayoutPanel(this);
        tabbed->initialization(dockWidget);
        panelBase = tabbed;
    }
    else if (DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(panelBase)) 
    {
        Q_ASSERT(qtn_checkLayoutType(panelBase, DockLayoutPanel::Tabbed));
        panelBase = layoutPanel->createCopyInstance();
        Q_ASSERT(panelBase != Q_NULL);
    }
    else
        Q_ASSERT(false);
    panelBase->setParentPanel(m_wndAutoHide[index]);
    panelBase->hide();
}

void DockLayoutEngine::repositionMargins(QRect& rect, const QRect& rcClientMargins)
{
    if (rcClientMargins.isNull())
        return;

    QRect rcClient(rect);
    rcClient.adjust(rcClientMargins.left(), rcClientMargins.top(), -rcClientMargins.right(), -rcClientMargins.bottom());
    rect = rcClient;
}

#if 0
void DockLayoutEngine::clearDockPanel(DockPanelBase* panel)
{
    if (panel == Q_NULL)
        return;

    Q_ASSERT(false);
    for (int i = 0, count = m_widgetPanelList.count(); count > i; ++i)
    {
        if (m_widgetPanelList[i] == panel)
        {
            m_widgetPanelList.removeAt(i);
            break;
        }
    }

    for (int i = 0, count = m_layoutPanelList.count(); count > i; ++i)
    {
        if (m_layoutPanelList.at(i) == panel)
        {
            m_layoutPanelList.removeAt(i);
            break;
        }
    }
}
#endif

void DockLayoutEngine::detachDockPanel(DockPanelBase* panelBase)
{
    Q_ASSERT(panelBase != Q_NULL);
    if (panelBase == Q_NULL)
        return;
    panelBase->setParentPanel(Q_NULL);
    //    if Panel.DockWindow is TFireDockWindow then TODO
    //        TFireDockWindow(Panel.DockWindow).SetLayoutPanel(nil);
}

QString DockLayoutEngine::generatePanelId(const QString& name) const
{
    QString ret = QString(QLatin1String("panel_%1")).arg(name.toLower().replace(QLatin1String(" "), QLatin1String("_")));
    if (!m_panelInfoMap.contains(ret) && findPanelById(ret) == Q_NULL)
        return ret;

    int i = 0;
    while (true)
    {
        QString s = QString(QLatin1String("%1_%2")).arg(ret).arg(i);
        if (!m_panelInfoMap.contains(s) && findPanelById(s) == Q_NULL)
        {
            ret = s;
            break;
        }
        ++i;
    }
    return ret;
}

bool DockLayoutEngine::setPanelId(const DockWidgetPanel* panel, const QString& oldId, const QString& newId)
{
    Q_UNUSED(panel);
    if (newId.isEmpty() || findPanelById(newId) != Q_NULL)
    {
        Q_ASSERT(false); // Can't change id. Panel id is not unique or is empty.
        return false;
    }

    QHash<QString, DockPanelInfo>::iterator it = m_panelInfoMap.find(oldId);
    if (it != m_panelInfoMap.end())
    {
        DockPanelInfo info = it.value();
        m_panelInfoMap.erase(it);
        m_panelInfoMap.insert(newId, info);
    }
    return true;
}

DockPanelInfo& DockLayoutEngine::getPanelInfo(const DockWidgetPanel* panel, const QString& id)
{
    QHash<QString, DockPanelInfo>::iterator it = m_panelInfoMap.find(id);
    if (it == m_panelInfoMap.end())
    {
        Q_ASSERT(panel != Q_NULL);
        DockPanelInfo panelInfo(qobject_cast<const DockDocumentPanel *>(panel) != Q_NULL);
        m_panelInfoMap.insert(id, panelInfo);
        it = m_panelInfoMap.find(id);
    }
    return it.value();
}

bool DockLayoutEngine::hasPanelInfo(const QString& id) const
{
    return m_panelInfoMap.contains(id);
}

void DockLayoutEngine::removePanelInfo(const QString& id)
{
    m_panelInfoMap.remove(id);
}

/* DockPanelManagerPrivate */
DockPanelManagerPrivate::DockPanelManagerPrivate()
    : m_layoutEngine(Q_NULL)
    , m_dockMasterDrag(Q_NULL)
    , m_activePanel(Q_NULL)
    , m_lastActiveDocPanel(Q_NULL)
    , m_widgetLastFocus(Q_NULL)
    , m_margin(0)
    , m_defaultPanelFeatures(DockWidgetPanel::NoDockPanelFeatures)
    , m_stateManager(Normal)
    , m_attachingPane(false)
    , m_showDockWindowFullContentsWhileDragging(true)
    , m_showMarkers(true)
    , m_panelTransparent(true)
    , m_centralPanelVisible(true)
    , m_useSplitterTracker(false)
    , m_lockSplitters(false)
    , m_useThemedFloatingFrames(false)
    , m_layoutCreated(false)
    , m_splitterGap(22)
    , m_closeGroupOnButtonClick(false)
    , m_hideGroupOnButtonClick(true)
    , m_ptMinClientSize(QSize(60, 60))
    , m_updateLayoutCount(0)
{
}

DockPanelManagerPrivate::~DockPanelManagerPrivate()
{
    disconnect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
    Q_DELETE_AND_NULL(m_dockMasterDrag);
    Q_DELETE_AND_NULL(m_layoutEngine);
}

/*! \internal */
void DockPanelManagerPrivate::initialization()
{
    QTN_P(DockPanelManager);
    CommonStyle::ensureStyle();
    if (QWidget* managedWidget = p.managedWidget())
    {
        QLayout* otherLayout = managedWidget->layout();
        DockMainWindowLayout* mainWindowLayout = qobject_cast<DockMainWindowLayout*>(otherLayout);
        if (mainWindowLayout == Q_NULL)
        {
            if (otherLayout != Q_NULL && qobject_cast<QMainWindow*>(otherLayout->parentWidget()) == Q_NULL)
                Q_DELETE_AND_NULL(otherLayout);
            DockMainWindowLayout* layout = new DockMainWindowLayout(managedWidget);
            layout->setDockPanelManager(&p);
        }
        else if (!mainWindowLayout->dockPanelManager())
            mainWindowLayout->setDockPanelManager(&p);
    }
    m_layoutEngine = new DockLayoutEngine(&p);
    m_layoutEngine->initNew(false);
    m_dockMasterDrag = new DockWidgetDragDrop(&p);
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
}

/*! \internal */
void DockPanelManagerPrivate::setUseThemedFloatingFrames(bool themedFloatingFrames)
{
    m_useThemedFloatingFrames = themedFloatingFrames;
}

/*! \internal */
DockLayoutPanel* DockPanelManagerPrivate::centralPanel() const
{
    if (DockLayoutEngine* layoutEngine = dockLayoutEngine())
        return layoutEngine->centralPanel();
    return Q_NULL;
}

/*! \internal */
DockLayoutEngine* DockPanelManagerPrivate::dockLayoutEngine() const
{
    return static_cast<DockLayoutEngine*>(m_layoutEngine);
}

/*! \internal */
void DockPanelManagerPrivate::setLayoutEngine(DockLayoutEngine* layoutEngine)
{
    QTN_P(DockPanelManager);
    Q_ASSERT(layoutEngine != Q_NULL);
    if (layoutEngine == Q_NULL)
        return;

    DockLayoutEngine* oldLayoutEngine = m_layoutEngine;
    m_layoutEngine = layoutEngine;
    m_layoutEngine->copyLayoutEngine(oldLayoutEngine);
    Q_DELETE_AND_NULL(oldLayoutEngine);

    updateFocusPanels();
    if (QLayout* layout = p.managedWidget()->layout())
    {
        layout->setGeometry(layout->geometry());
        layout->update();
    }
    updatePanels();
}

/*! \internal */
QRect DockPanelManagerPrivate::calculateResultDockingRect(DockPanelBase* panel, Qtitan::DockPanelArea area, DockPanelBase* adjacent)
{
    QTN_P(DockPanelManager);
    if (!adjacent)
        adjacent = p.layoutPanel();

    Q_ASSERT(adjacent);
    if (!adjacent)
        return QRect(0, 0, 0, 0);

    bool horiz = (area == Qtitan::LeftDockPanelArea || area == Qtitan::RightDockPanelArea);
    DockWindow* dockWindow = qobject_cast<DockWindow*>(adjacent->dockWindow());
    if (adjacent == p.layoutPanel() || (dockWindow && adjacent == dockWindow->layoutPanel()))
    {
        DockSplitLayoutPanel* splitter = (DockSplitLayoutPanel*)adjacent;
        return DockSplitLayoutPanel::calculatePanelsRectChild(splitter, panel, area);
    }
    else
    {
        Q_ASSERT(adjacent->parentPanel() != Q_NULL);
        if (adjacent->parentPanel() == Q_NULL)
            return QRect(0, 0, 0, 0);

        if (qtn_checkLayoutType(adjacent->parentPanel(), DockLayoutPanel::Tabbed))
        {
            adjacent = adjacent->parentPanel();
            Q_ASSERT(adjacent->parentPanel() != Q_NULL);
            if (adjacent->parentPanel() == Q_NULL)
                return QRect(0, 0, 0, 0);
        }

        Q_ASSERT(adjacent->parentPanel() != Q_NULL);
        Q_ASSERT(qtn_checkLayoutType(adjacent->parentPanel(), DockLayoutPanel::Split));

        DockSplitLayoutPanel* splitter = (DockSplitLayoutPanel*)adjacent->parentPanel();
        if (splitter->isHorizontal() == horiz)
            return splitter->calculatePanelsRectSelf(panel, area, adjacent);
        return DockSplitLayoutPanel::calculatePanelsRectChild(adjacent, panel, area);
    }
}

/*! \internal */
void DockPanelManagerPrivate::internal_insertDockPanel(DockWidgetPanel* panel, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    QTN_P(DockPanelManager);
    QSize size = sizeHint;
    if (size.isNull() || size.isEmpty())
    {
        if (QWidget* widget = p.managedWidget())
        {
            QFontMetrics fm(widget->font());
            size = fm.boundingRect(QStringLiteral("XXXXXXXXXXXXXXXXXXXXXXXXX")).size() + QSize(24, 24);
            size.setWidth(qMax(100, size.width()));
            size.setHeight(qMax(100, size.width()));

            if (area == Qtitan::InsideDockPanelArea)
                size = p.layoutPanel()->geometry().size();
        }
    }

    panel->setGeometry(QRect(QPoint(0, 0), size));
    DockPanelBasePrivate::_get(panel)->m_sizeDocking = panel->size();

    //dockWidgetList().append(panel);

    internal_insertDockPanel(panel, area, targetPanel);
    preservePanelState();
}

/*! \internal */
void DockPanelManagerPrivate::internal_insertDockPanel(DockPanelBase* panel, Qtitan::DockPanelArea area, DockPanelBase* before)
{
    QTN_P(DockPanelManager);
    Q_ASSERT(panel->parentPanel() == Q_NULL);
    if (before == Q_NULL || (qobject_cast<DockWidgetPanel*>(before) && (isDockPanelHidden((DockWidgetPanel*)before) || p.isDockPanelClosed((DockWidgetPanel*)before))))
        before = p.layoutPanel();

    Q_ASSERT(before != Q_NULL);
    if (before == Q_NULL)
        return;

    DockManagerUpdater managerUpdater(&p);
    bool horiz = (area == Qtitan::LeftDockPanelArea  || area == Qtitan::RightDockPanelArea);
    bool after = (area == Qtitan::RightDockPanelArea || area == Qtitan::BottomDockPanelArea);

    DockWidgetPanel* dockWidget = qobject_cast<DockWidgetPanel*>(panel);
    if (dockWidget)
    {
        DockTabbedLayoutPanel* layout = new DockTabbedLayoutPanel(m_layoutEngine);
        layout->initialization(dockWidget);
        panel = layout;
    }

    DockWindow* dockWindow = qobject_cast<DockWindow*>(before->dockWindow());
    if (dockWindow && before == dockWindow->layoutPanel())
    {
        DockSplitLayoutPanel* splitter = (DockSplitLayoutPanel*)before;
        if (splitter->isHorizontal() == horiz)
        {
            DockSplitLayoutPanelPrivate::_get(splitter)->extraInsertParam(Q_NULL, after);
            panel->setParentPanel(splitter);
        }
        else
        {
            DockSplitLayoutPanel* childSplitter = new DockSplitLayoutPanel(m_layoutEngine);
            childSplitter->initialization(splitter, horiz);
            DockSplitLayoutPanelPrivate::_get(childSplitter)->extraInsertParam(Q_NULL, after);
            panel->setParentPanel(childSplitter);
            DockWindowPrivate::_get(dockWindow)->setRootPanel(childSplitter);
        }
    }
    else if (before == p.layoutPanel())
    {
        DockSplitLayoutPanel* splitter = (DockSplitLayoutPanel*)before;
        if (splitter->isHorizontal() == horiz)
        {
            DockSplitLayoutPanelPrivate::_get(splitter)->extraInsertParam(Q_NULL, after);
            panel->setParentPanel(splitter);
        }
        else
        {
            DockSplitLayoutPanel* childSplitter = new DockSplitLayoutPanel(m_layoutEngine);
            DockSplitLayoutPanelPrivate* splitLayoutPanelPrivate = DockSplitLayoutPanelPrivate::_get(childSplitter);
            childSplitter->initialization(splitter, horiz);
            splitLayoutPanelPrivate->extraInsertParam(Q_NULL, after);
            panel->setParentPanel(childSplitter);
            splitLayoutPanelPrivate->setPanelGeometry(QRect(0, 0, 0, 0));
            m_layoutEngine->setRootSplitPanel(childSplitter);
        }
    }
    else
    {
        Q_ASSERT(before->parentPanel() != Q_NULL);
        if (before->parentPanel() == Q_NULL)
            return;

        if (qtn_checkLayoutType(before->parentPanel(), DockLayoutPanel::Tabbed))
        {
            before = (DockTabbedLayoutPanel*)before->parentPanel();
            Q_ASSERT(before->parentPanel() != Q_NULL);
            if (before->parentPanel() == Q_NULL)
                return;
        }

        Q_ASSERT(before->parentPanel());
        Q_ASSERT(qtn_checkLayoutType(before->parentPanel(), DockLayoutPanel::Split) ||
                 qtn_checkLayoutType(before->parentPanel(), DockLayoutPanel::Document));

        DockSplitLayoutPanel* splitter = (DockSplitLayoutPanel*)before->parentPanel();
        if (splitter->isHorizontal() == horiz)
        {
            DockSplitLayoutPanelPrivate::_get(splitter)->extraInsertParam(before, after);
            panel->setParentPanel(splitter);
        }
        else
        {
            DockSplitLayoutPanel* childSplitter = new DockSplitLayoutPanel(m_layoutEngine);
            splitter->replacePanel(before, childSplitter);
            childSplitter->initialization(before, horiz);
            DockSplitLayoutPanelPrivate::_get(childSplitter)->extraInsertParam(before, after);
            panel->setParentPanel(childSplitter);
        }
    }
}

/*! \internal */
void DockPanelManagerPrivate::internal_attachDockWidget(DockPanelBase* target, DockPanelBase* panel)
{
    Q_ASSERT(panel != Q_NULL);
    if (panel == Q_NULL)
        return;

    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panel))
    {
        panel = widgetPanel->parentPanel();
        Q_ASSERT(panel != Q_NULL);
    }

    if (panel == Q_NULL)
        return;

    Q_ASSERT(qtn_checkLayoutType(panel, DockLayoutPanel::Tabbed) || qtn_checkLayoutType(panel, DockLayoutPanel::Document));

    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(target))
    {
        DockTabbedLayoutPanel* tabbedLayoutPanel = qobject_cast<DockTabbedLayoutPanel*>(panel);
        Q_ASSERT(tabbedLayoutPanel != Q_NULL);
        widgetPanel->setParentPanel(tabbedLayoutPanel);
    }
    else
    {
        Q_ASSERT(qtn_checkLayoutType(target, DockLayoutPanel::Split) || 
                 qtn_checkLayoutType(target, DockLayoutPanel::Document) || 
                 qtn_checkLayoutType(target, DockLayoutPanel::Tabbed));
        QList<DockPanelBase*> listPanel;
        listPanel << target->findPanels(DockWidgetPanel::staticMetaObject.className());
        Q_ASSERT(listPanel.count() > 0);
        for (QList<DockPanelBase*>::iterator it = listPanel.begin(); it != listPanel.end(); ++it)
        {
            if (DockWidgetPanel* dockPanel = qobject_cast<DockWidgetPanel*>(*it))
            {
                internal_removeDockPanel(dockPanel);
                if (DockDocumentLayoutPanel* dockDocumentLayoutPanel = qobject_cast<DockDocumentLayoutPanel*>(panel))
                {
                    DockDocumentTabbedLayoutPanel* tabbedLayoutPanel = new DockDocumentTabbedLayoutPanel(m_layoutEngine);
                    tabbedLayoutPanel->initialization(dockPanel);
                    tabbedLayoutPanel->setParentPanel(dockDocumentLayoutPanel);
                }
                else if (DockTabbedLayoutPanel* dockTabbedLayoutPanel = qobject_cast<DockTabbedLayoutPanel*>(panel))
                    dockPanel->setParentPanel(dockTabbedLayoutPanel);
            }
        }
    }
    m_widgetLastFocus = Q_NULL;
}

/*! \internal */
void DockPanelManagerPrivate::internal_removeDockPanel(DockPanelBase* panel)
{
    if (!panel || !panel->parentPanel())
        return;
    panel->setParentPanel(Q_NULL);
}

/*! \internal */
void DockPanelManagerPrivate::internal_detachDockPanel(DockWidgetPanel* widgetPanel)
{
    QTN_P(DockPanelManager);
    DockManagerUpdater managerUpdater(&p);
    DockLayoutEngine* layoutEngine = dockLayoutEngine();
    Q_ASSERT(layoutEngine != Q_NULL);
    layoutEngine->detachDockPanel(widgetPanel);
}

/*! \internal */
void DockPanelManagerPrivate::insertCenterDockPanel(DockPanelBase* panel, Qtitan::DockPanelArea area, DockPanelBase* before)
{
    QList<DockPanelBase*> listPanel;
    listPanel << panel->findPanels(DockWidgetPanel::staticMetaObject.className());
    Q_ASSERT(listPanel.count() > 0);
    for (QList<DockPanelBase*>::iterator it = listPanel.begin(); it != listPanel.end(); ++it)
    {
        DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(*it);
        internal_removeDockPanel(widgetPanel);
        DockTabbedLayoutPanel* tabbedLayout = new DockDocumentTabbedLayoutPanel(m_layoutEngine);
        DockPanelBasePrivate::_get(before)->m_sizeDocking = QSize(100, 100);
        tabbedLayout->initialization(widgetPanel);
        DockPanelBasePrivate::_get(tabbedLayout)->m_sizeDocking = QSize(100, 100);
        internal_insertDockPanel(tabbedLayout, area, before);
    }
    m_widgetLastFocus = Q_NULL;
}

/*! \internal */
void DockPanelManagerPrivate::insertDockPanelBefore(DockPanelBase* panelBase, Qtitan::DockPanelArea area, DockPanelBase* before)
{
    QTN_P(DockPanelManager);
    if (qtn_checkLayoutType(panelBase, DockLayoutPanel::Split))
    {
        QList<DockPanelBase*> lstTabbed = panelBase->findPanels(DockTabbedLayoutPanel::staticMetaObject.className());
        if (lstTabbed.count() == 1)
            panelBase = lstTabbed.at(0);
    }

    DockManagerUpdater managerUpdater(&p);
    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panelBase))
        internal_removeDockPanel(widgetPanel);
    else if (DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(panelBase))
        panelBase = layoutPanel->createCopyInstance();
    else
        Q_ASSERT(false);

    internal_insertDockPanel(panelBase, area, before);
    m_widgetLastFocus = Q_NULL;
}

/*! \internal */
void DockPanelManagerPrivate::tabifyDockPanel(DockPanelBase* target, DockPanelBase* panel)
{
    QTN_P(DockPanelManager);
    DockManagerUpdater managerUpdater(&p);
    internal_attachDockWidget(target, panel);
    preservePanelState();
}

/*! \internal */
void DockPanelManagerPrivate::switchDocking(DockPanelBase* panelBase)
{
    QTN_P(DockPanelManager);
    Q_ASSERT(panelBase != Q_NULL);
    if (panelBase == Q_NULL)
        return;

    DockManagerUpdater managerUpdater(&p);
    bool floating = qobject_cast<DockWindow*>(panelBase->topDockWindow()) != Q_NULL;
    bool hidden = DockPanelBasePrivate::_get(panelBase)->isAutoHide();
    bool docking = !floating && !hidden;

    if (qtn_checkLayoutType(panelBase, DockLayoutPanel::Split) || qtn_checkLayoutType(panelBase, DockLayoutPanel::Tabbed))
    {
        QList<DockPanelBase*> listPanel = panelBase->findPanels(DockWidgetPanel::staticMetaObject.className());
        DockWidgetPanel* activatedPanel = Q_NULL, *focusedPanel = Q_NULL;
        for (QList<DockPanelBase*>::iterator it = listPanel.begin(); it != listPanel.end(); ++it)
        {
            DockWidgetPanel* dockPanel = (DockWidgetPanel*)(*it);
            if (dockPanel->isActive())
                activatedPanel = dockPanel;
            if (dockPanel->hasFocus())
                focusedPanel = dockPanel;
        }

        if (qtn_checkLayoutType(panelBase, DockLayoutPanel::Tabbed) && listPanel.count() > 1)
        {
            DockTabbedLayoutPanel* ownerPanel = Q_NULL;
            for (QList<DockPanelBase*>::iterator it = listPanel.begin(); it != listPanel.end(); ++it)
            {
                DockWidgetPanel* dockPanel = (DockWidgetPanel*)(*it);
                ownerPanel = ownerPanel == Q_NULL ? qobject_cast<DockTabbedLayoutPanel*>(DockWidgetPanelPrivate::_get(dockPanel)->lastOwner(docking)) : ownerPanel;
                if (switchDocking(dockPanel, ownerPanel))
                    ownerPanel = qobject_cast<DockTabbedLayoutPanel*>(dockPanel->parentPanel());
            }
        }
        else
        {
            DockPanelBase* lastOwnerPanel = Q_NULL;
            for (QList<DockPanelBase*>::iterator it = listPanel.begin(); it != listPanel.end(); ++it)
            {
                DockWidgetPanel* dockPanel = (DockWidgetPanel*)(*it);
                DockTabbedLayoutPanel* ownerPanel = qobject_cast<DockTabbedLayoutPanel*>(DockWidgetPanelPrivate::_get(dockPanel)->lastOwner(docking));
                if (!switchDocking(dockPanel, ownerPanel ? ownerPanel : lastOwnerPanel))
                {
                    if (dockPanel == activatedPanel)
                        activatedPanel = Q_NULL;
                    if (dockPanel == focusedPanel)
                        focusedPanel = Q_NULL;
                }
                lastOwnerPanel = ownerPanel ? ownerPanel : qobject_cast<DockPanelBase*>(dockPanel->parentPanel());
            }
        }

        if (focusedPanel != Q_NULL)
            p.showDockPanel(focusedPanel);
        else if (activatedPanel != Q_NULL)
            p.showDockPanel(activatedPanel);
    }
    else if (DockWidgetPanel* dockPanel = qobject_cast<DockWidgetPanel*>(panelBase))
    {
        DockTabbedLayoutPanel* ownerPanel = qobject_cast<DockTabbedLayoutPanel*>(DockWidgetPanelPrivate::_get(dockPanel)->lastOwner(docking));
        switchDocking(dockPanel, ownerPanel);
    }
    else
    {
//        Q_ASSERT(false);
    }
    m_widgetLastFocus = Q_NULL;
}

/*! \internal */
bool DockPanelManagerPrivate::switchDocking(DockWidgetPanel* widgetPanel, DockPanelBase* ownerPanel)
{
    QTN_P(DockPanelManager);
    DockWindowBase* parentWindow = widgetPanel->topDockWindow();
    bool floating = qobject_cast<DockWindow*>(widgetPanel->topDockWindow()) != Q_NULL;
    bool hidden = widgetPanel->isAutoHide();
    bool docking = !floating && !hidden;

    if (floating && !(widgetPanel->features() & DockWidgetPanel::DockPanelDockable))
        return false;
    if (docking && !(widgetPanel->features() & DockWidgetPanel::DockPanelFloatable))
        return false;

    if (ownerPanel)
    {
        m_attachingPane = true;
        tabifyDockPanel(widgetPanel, ownerPanel);
        m_attachingPane = false;
    }
    else
    {
        QRect rect = widgetPanel->parentPanel()->geometry();
        rect.moveTopLeft(parentWindow ? parentWindow->mapToGlobal(rect.topLeft()) : 
            p.managedWidget()->mapToGlobal(rect.topLeft()));
        p.addDockWindow(widgetPanel, rect);
    }
    return true;
}

/*! \internal */
void DockPanelManagerPrivate::autoHideDockPanel(DockPanelBase* panelBase)
{
    if (panelBase == Q_NULL)
        return;
    DockManagerUpdater managerUpdater(&qtn_p());
    if (qobject_cast<DockWidgetPanel*>(panelBase))
        panelBase = panelBase->parentPanel();

    if (panelBase == Q_NULL || (panelBase->parentPanel() == Q_NULL && panelBase->dockWindow() == Q_NULL))
        return;

    if (qtn_checkLayoutType(panelBase->parentPanel(), DockLayoutPanel::AutoHide))
    {
        DockAutoHideLayoutPanel* autoHide = static_cast<DockAutoHideLayoutPanel*>(panelBase->parentPanel());
        autoHide->closeActiveWindow();
        return;
    }
    m_layoutEngine->autoHideDockPanel(panelBase);
}

/*! \internal */
void DockPanelManagerPrivate::preservePanelState()
{
    const QList<DockWidgetPanel*>& list = m_layoutEngine->widgetPanelList();
    for (QList<DockWidgetPanel*>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        DockWidgetPanel* widgetPanel = *it;
        DockPanelBase* panelBase = widgetPanel->parentPanel();
        if (widgetPanel->topLevelWidget() != Q_NULL && panelBase != Q_NULL)
        {
            if (panelBase->parentPanel() == Q_NULL || !::qtn_checkLayoutType(panelBase->parentPanel(), DockLayoutPanel::AutoHide))
            {
                DockPanelInfo& panelInfo = DockWidgetPanelPrivate::_get(widgetPanel)->panelInfo();

                if (qobject_cast<DockWindow*>(widgetPanel->topLevelWidget()))
                    panelInfo.lastOwner = panelInfo.floatingOwner = panelBase;
                else
                    panelInfo.lastOwner = panelInfo.dockingOwner = panelBase;
            }
        }
    }
}

/*! \internal */
void DockPanelManagerPrivate::ensureVisible(DockPanelBase* panelBase)
{
    QTN_P(DockPanelManager);
    if (panelBase == Q_NULL)
        return;
    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panelBase))
    {
        p.showDockPanel(widgetPanel, false);
        panelBase = qobject_cast<DockPanelBase*>(widgetPanel->parentPanel());
    }
#if 0
    if (qtn_checkLayoutType(panelBase, DockLayoutPanel::Tabbed))
        ((DockTabbedLayoutPanel*)panelBase)->showDockPanel(false);
#endif
}

/*! \internal */
QRect DockPanelManagerPrivate::updateLayoutGeometry(const QRect& rect)
{
    QTN_P(DockPanelManager);
    if (p.layoutPanel() != Q_NULL)
    {
        m_layoutEngine->setGeometry(rect);
        preservePanelState();
        m_layoutCreated = true;
    }
    return centralPanel() ? centralPanel()->geometry() : QRect();
}

/*! \internal */
void DockPanelManagerPrivate::changedActiveDockPanel(DockWidgetPanel* widgetPanel, bool active)
{
    QTN_P(DockPanelManager);
    if (active && m_activePanel != widgetPanel)
    {
        if (m_activePanel)
            emit p.dockPanelDeactivated(m_activePanel);

        m_activePanel = widgetPanel;

        if (DockDocumentPanel* documentPanel = qobject_cast<DockDocumentPanel*>(m_activePanel))
            m_lastActiveDocPanel = documentPanel;

        if (m_activePanel)
            emit p.dockPanelActivated(m_activePanel);
    }
    else if (!active && widgetPanel && m_activePanel == widgetPanel)
    {
        m_activePanel = Q_NULL;
        emit p.dockPanelDeactivated(widgetPanel);
    }
}

/*! \internal */
void DockPanelManagerPrivate::recalcLayout(DockPanelBase* panelBase)
{
    Q_UNUSED(panelBase)
    if (!isNormalState() || m_layoutEngine->isDestroying())
        return;
    m_layoutEngine->deleteEmptyDockWidgets();
    m_layoutEngine->updateGeometry();
    preservePanelState();
}

/*! \internal */
void DockPanelManagerPrivate::updatePanelProperties(DockWidgetPanel* widgetPanel)
{
    if (DockLayoutPanel* layoutpanel = qobject_cast<DockLayoutPanel*>(widgetPanel->parentPanel()))
        layoutpanel->updatePanelProperties(widgetPanel);
}

/*! \internal */
void DockPanelManagerPrivate::updateFocusPanels()
{
    QWidget* focus = QApplication::focusWidget();
    if (m_widgetLastFocus == focus)
        return;

    m_widgetLastFocus = focus;
    const QList<DockLayoutPanel*>& layouts = m_layoutEngine->layoutPanelList();
    for (QList<DockLayoutPanel*>::const_iterator it = layouts.constBegin(); it != layouts.constEnd(); ++it)
    {
        DockLayoutPanel* panel = *it;
        if (qtn_checkLayoutType(panel, DockLayoutPanel::Tabbed))
        {
            DockTabbedLayoutPanel* layoutPanel = static_cast<DockTabbedLayoutPanel*>(panel);
            layoutPanel->focusChanged();
        }
    }

    const QList<DockWindow*>& windows = m_layoutEngine->windowList();
    for (QList<DockWindow*>::const_iterator it = windows.constBegin(); it != windows.end(); ++it)
    {
        DockWindow* dockWindow = *it;
        if (dockWindow && !dockWindow->isHidden())
            DockWindowPrivate::_get(dockWindow)->focusChanged();
    }
}

/*! \internal */
void DockPanelManagerPrivate::updatePanels()
{
    const QList<DockWidgetPanel*>& panels = m_layoutEngine->widgetPanelList();
    for (QList<DockWidgetPanel*>::const_iterator it = panels.constBegin(); it != panels.constEnd(); ++it)
    {
        DockWidgetPanel* widgetPanel = *it;
        if (DockPanelBase* basePanel = qobject_cast<DockPanelBase*>(widgetPanel->parentPanel()))
            DockPanelBasePrivate::_get(basePanel)->updatePanel(false);
        else if (DockWindow* window = qobject_cast<DockWindow*>(widgetPanel->dockWindow()))
            DockWindowPrivate::_get(window)->updatePanel(false);
    }
}

void DockPanelManagerPrivate::focusChanged(QWidget* oldWidget, QWidget* nowWidget)
{
    if (oldWidget && nowWidget)
        updateFocusPanels();
}

void DockPanelManagerPrivate::setCentralPanelVisible(bool visible)
{
    QTN_P(DockPanelManager);
    if (m_centralPanelVisible == visible)
        return;
    DockManagerUpdater managerUpdater(&p);
    m_centralPanelVisible = visible;
}

/*!
\fn void DockPanelManager::dockPanelActivated(DockWidgetPanel* panel);
Triggered if the panel become an active.
*/

/*!
\fn void DockPanelManager::dockPanelDeactivated(DockWidgetPanel* panel);
Triggered if the panel become an unactive.
*/

/*!
\fn void DockPanelManager::panelRequested(DockPanelRequestedArgs* args);
Triggered at the time of loading the layout from the XML if the panel has not yet been created in the manager.
The \a args contains the "id" and other attributes of the requested panel.
You can create a panel with a given args->id() or ignore the signal and create a panel after. To restore its position after, use the restoreDockPanel().
*/

/*!
\class DockPanelRequestedArgs
\inmodule QtitanDocking
\brief Implements parameters for a signal DockPanelManager::panelRequested().
*/
DockPanelRequestedArgs::DockPanelRequestedArgs(const QString& id, bool document)
    : BaseEventArgs(), m_id(id), m_document(document)
{
}

/*!
Returns id for the requested panel.
*/
const QString& DockPanelRequestedArgs::id() const
{
    return m_id;
}

/*!
Returns true if the requested panel is document panel, otherwise it is widget panel.
*/
bool DockPanelRequestedArgs::isDocument() const
{
    return m_document;
}

/*!
Returns the requested panel.
*/
DockWidgetPanel* DockPanelRequestedArgs::panel() const
{
    return m_panel;
}

/*!
Sets the requested \a panel.
*/
void DockPanelRequestedArgs::setPanel(DockWidgetPanel* panel)
{
    m_panel = panel;
    setHandled(true);
}

/*!
Constructs the class with the given \a widget on which the docking engine will be deployed.
*/
DockPanelManager::DockPanelManager(QWidget* widget)
    : QObject(widget)
{
    initDockResource();
    QTN_INIT_PRIVATE(DockPanelManager);
    QTN_D(DockPanelManager);
    d.initialization();
    managedWidget()->installEventFilter(this);
}

/*!
\class DockPanelManager
\inmodule QtitanDocking
\brief Class DockPanelManager is used to add Microsoft-like dockable panels to the QWidget.

The example below demonstrates the simplest use of the manager with QMainWindow and regular QWidget.
\code
QMainWindow window;
window.setCentralWidget(new QTextEdit());
DockPanelManager* dockPanelManager = new DockPanelManager(&window);
dockPanelManager->setDockPanelTransparentWhileDragging(true);
dockPanelManager->setArrowMarkersShown(true);
dockPanelManager->setDockPanelFullContentsWhileDraggingShown(true);
DockWidgetPanel* panel1 = dockPanelManager->addDockPanel("Simple panel 1", Qtitan::LeftDockPanelArea);
DockWidgetPanel* panel2 = dockPanelManager->addDockPanel("Simple panel 2", Qtitan::BottomDockPanelArea);
window.show();
\endcode

\code
QWidget* widget = new QWidget;
DockPanelManager* manager = new DockPanelManager(widget);
DockWidgetPanel* dockPanel = manager->addDockPanel("My Panel", Qt::LeftDockWidgetArea);
widget->show();
\endcode

DockPanelManager can be used with DockBarManager on the same QWidget together.
Normally, you do not need to create it manually. Use DockMainWindow to get the main window, which contains the DockPanelManager and a set of functions for panels creation.
\sa DockBarManager
*/
/*!
Constructs the class with the given \a widget on which the docking engine will be deployed.
*/
DockPanelManager::DockPanelManager(QMainWindow* mainWindow)
    : DockPanelManager(new QWidget())
{
    QLayout* mainLayout = mainWindow->layout();
    Q_ASSERT(mainLayout != Q_NULL);
    if (QWidget* centralWidget = mainWindow->centralWidget())
    {
        mainLayout->removeWidget(centralWidget);
        mainWindow->setCentralWidget(managedWidget());
        setCentralWidget(centralWidget);
    }
}

DockPanelManager::~DockPanelManager()
{
    managedWidget()->removeEventFilter(this);
    QTN_FINI_PRIVATE()
}

/*!
Returns the managed widget.
*/
QWidget* DockPanelManager::managedWidget() const
{
    Q_ASSERT(qobject_cast<QWidget*>(parent()) != Q_NULL);
    return qobject_cast<QWidget*>(parent());
}

/*!
Returns the root layout panel for the managed widget.
*/
DockLayoutPanel* DockPanelManager::layoutPanel() const
{
    QTN_D(const DockPanelManager);
    return d.m_layoutEngine ? d.m_layoutEngine->rootSplitPanel() : Q_NULL;
}

/*!
Creates and returns DockDocumentLayoutPanel that can holds DockDocumentPanel. 
*/
DockLayoutPanel* DockPanelManager::createDocumentLayout() const
{
    QTN_D(const DockPanelManager);
    if (d.m_layoutEngine == Q_NULL)
        return Q_NULL;
    if (qobject_cast<DockDocumentLayoutPanel*>(d.m_layoutEngine->centralPanel()) == Q_NULL)
        d.m_layoutEngine->initNew(true);
    return qobject_cast<DockDocumentLayoutPanel*>(d.m_layoutEngine->centralPanel());
}

/*!
Returns list of all widget panels that were added earlier.
*/
QList<DockWidgetPanel*> DockPanelManager::widgetPanelList() const
{
    QTN_D(const DockPanelManager);
    QList<DockWidgetPanel*> ret;
    const QList<DockWidgetPanel*>& list = d.m_layoutEngine->widgetPanelList();
    for (QList<DockWidgetPanel*>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        if (qobject_cast<DockDocumentPanel*>(*it) == Q_NULL)
            ret.append(*it);
    }
    return ret;
}

/*!
Returns the list of all document panels that were added earlier.
*/
QList<DockDocumentPanel*> DockPanelManager::documentPanelList() const
{
    QTN_D(const DockPanelManager);
    QList<DockDocumentPanel*> ret;
    const QList<DockWidgetPanel*>& list = d.m_layoutEngine->widgetPanelList();
    for (QList<DockWidgetPanel*>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        if (DockDocumentPanel* document = qobject_cast<DockDocumentPanel*>(*it))
            ret.append(document);
    }
    return ret;
}

/*!
Creates floating window (DockWindow) and adds \a panel to it with the given \a rect.
*/
DockWindow* DockPanelManager::addDockWindow(DockPanelBase* panelBase, const QRect& rect)
{
    QTN_D(DockPanelManager);
    bool handled = true;
    emit aboutToFloat(panelBase, handled);
    if (!handled)
        return Q_NULL;

    DockManagerUpdater managerUpdater(this);
    Q_ASSERT(qobject_cast<DockWidgetPanel*>(panelBase) || qtn_checkLayoutType(panelBase, DockLayoutPanel::Tabbed));

    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panelBase))
        d.internal_removeDockPanel(widgetPanel);
    else if (DockLayoutPanel* layoutPanel = qobject_cast<DockLayoutPanel*>(panelBase))
        panelBase = layoutPanel->createCopyInstance();
    else
        Q_ASSERT(false);

    DockWindow* dockWindow = new DockWindow(d.m_layoutEngine);
    dockWindow->initializationWindow(panelBase);
#if 0
    if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panelBase))
    {
        QList<DockWidgetPanel*>&  listData = d.dockWidgetList();
        if (!listData.contains(widgetPanel))
            listData.append(widgetPanel);
    }
#endif

    DockWindowPrivate::_get(dockWindow)->showOnScreen(rect);
    d.preservePanelState();
    d.m_widgetLastFocus = Q_NULL;
    return dockWindow;
}

/*!
Creates and returns pointer to new DockDocumentPanel. Parameter \a title is a document name that will be displayed in tab.
*/
DockDocumentPanel* DockPanelManager::addDocumentPanel(const QString& title)
{
    DockDocumentPanel* documentPanel = new DockDocumentPanel(this);
    documentPanel->setCaption(title);
    insertDocumentPanel(documentPanel);
    return documentPanel;
}

//DockDocumentPanel* DockPanelManager::addDocumentPanel(const QString& title, const QString& tooltip, const DockWidgetPanel::DockPanelFeature& feature){
//    DockDocumentPanel* documentPanel = new DockDocumentPanel(this);
//    documentPanel->setCaption(title);
//    documentPanel->setFeatures(feature);
//    documentPanel->setToolTip(tooltip);
//    insertDocumentPanel(documentPanel);
//    return documentPanel;
//}

/*!
Inserts existing \a documentPanel to DockDocumentLayoutPanel. Parameter documentPanel is a pointer to DockDocumentPanel.
*/
void DockPanelManager::insertDocumentPanel(DockDocumentPanel* documentPanel)
{
    QTN_D(DockPanelManager);
    DockManagerUpdater managerUpdater(this);
    DockDocumentLayoutPanel* documentLayoutPanel = qobject_cast<DockDocumentLayoutPanel*>(createDocumentLayout());
    Q_ASSERT(documentLayoutPanel != Q_NULL);

    // TODO: m_lastActiveDocPanel - m_activePanel
    DockDocumentTabbedLayoutPanel* tabbedLayoutPanel = documentLayoutPanel->activTabbedLayout();

    if (DockDocumentPanel* activeDocPanel = d.m_lastActiveDocPanel)
        if (DockDocumentTabbedLayoutPanel* parentActiveDocument = qobject_cast<DockDocumentTabbedLayoutPanel*>(activeDocPanel->parentPanel()))
            tabbedLayoutPanel = parentActiveDocument;

    if (tabbedLayoutPanel == Q_NULL)
    {
        tabbedLayoutPanel = new DockDocumentTabbedLayoutPanel(DockPanelManagerPrivate::_get(this)->dockLayoutEngine());
        //add by zhangyang 20230910 新增，未使用
//        tabbedLayoutPanel->setTabClosable(documentPanel->features());
//        tabbedLayoutPanel->setTabTooltip(documentPanel->toolTip());

        tabbedLayoutPanel->setParentPanel(documentLayoutPanel);

        //add by zhangyang 20230910 新增，未使用
//        tabbedLayoutPanel->initialization(documentPanel);
    }
    //add by zhangyang 20230910 新增，未使用
//    tabbedLayoutPanel->setFeatures(documentPanel->features());
//    tabbedLayoutPanel->setTabTooltip(documentPanel->toolTip());

    tabbedLayoutPanel->initialization(documentPanel);
    d.m_lastActiveDocPanel = documentPanel;
}

/*!
Creates and returns the dock panel with the given \a title in the given \a area. Parameter \a before used to specify after which panel you want to add the current panel. 
If the parameter is 0, then the panel is added to the end.

\code
DockWidgetPanel* panel_1 = dockPanelManager()->addDockPanel("Panel 1", Qtitan::LeftDockPanelArea);
DockWidgetPanel* panel_2 = dockPanelManager()->addDockPanel("Panel 2", Qtitan::LeftDockPanelArea);
\endcode
\image docking_addpanel1.png


\code
DockWidgetPanel* panel_1 = dockPanelManager()->addDockPanel("Panel 1", Qtitan::LeftDockPanelArea);
DockWidgetPanel* panel_2 = dockPanelManager()->addDockPanel("Panel 2", Qtitan::BottomDockPanelArea, m_panel_1);
\endcode
\image docking_addpanel2.png


\code
DockWidgetPanel* panel_1 = dockPanelManager()->addDockPanel("Panel 1", Qtitan::LeftDockPanelArea);
DockWidgetPanel* panel_2 = dockPanelManager()->addDockPanel("Panel 2", Qtitan::InsideDockPanelArea, m_panel_1);
\endcode
\image docking_addpanel3.png

*/
DockWidgetPanel* DockPanelManager::addDockPanel(const QString& title, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    return addDockPanel(title, QSize(), area, targetPanel);
}

/*!
Creates and returns the dock panel with the given \a title in the given \a area. Parameter \a before used to specify after which panel you want to add the current panel.
If the parameter is 0, then the panel is added to the end. Parameter \a sizeHint is used to specify the size of the new panel.
*/
DockWidgetPanel* DockPanelManager::addDockPanel(const QString& title, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    DockWidgetPanel* retPanel = new DockWidgetPanel(this);
    retPanel->setCaption(title);
    insertDockPanel(retPanel, sizeHint, area, targetPanel);
    return retPanel;
}

/*!
Inserts an existing dock \a panel to the dock layout to the given \a area. Parameter \a before used to specify after which panel you want to add the current panel.
If the parameter is 0, then the panel is added to the end.
*/
void DockPanelManager::insertDockPanel(DockWidgetPanel* panel, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    insertDockPanel(panel, QSize(), area, targetPanel);
}

/*!
Inserts an existing dock \a panel to the dock layout to the given \a area. Parameter \a before used to specify after which panel you want to add the current panel.
If the parameter is 0, then the panel is added to the end. Parameter \a sizeHint is used to specify the size of the panel.
*/
void DockPanelManager::insertDockPanel(DockWidgetPanel* panel, const QSize& sizeHint, Qtitan::DockPanelArea area, DockPanelBase* targetPanel)
{
    QTN_D(DockPanelManager);
    Q_ASSERT(layoutPanel() != Q_NULL);
    d.internal_insertDockPanel(panel, sizeHint, area, targetPanel);
    // Moves the dock panel panel to the dock panel target.
    if (area == Qtitan::InsideDockPanelArea && targetPanel)
        d.tabifyDockPanel(panel, targetPanel);
}

/*!
Restores the state and position of \a panel if it is created after calling loadStateFromFile() or loadStateFromDevice(). 
Returns true if the state was restored, otherwise returns false.
*/
bool DockPanelManager::restoreDockPanel(DockWidgetPanel* panel)
{
    QTN_D(DockPanelManager);
    bool result = d.m_layoutEngine->hasPanelInfo(panel->id());
    if (result)
        showDockPanel(panel);
    return result;
}

/*!
Removes dock \a panel from the manager and clear all internal structures regarding this panel.
Note that the function does not destroy the instance of \a panel. After calling this function, you must delete the \a panel manually 
using the operator 'delete'.
*/
void DockPanelManager::removeDockPanel(DockWidgetPanel* panel)
{
    QTN_D(DockPanelManager);
    d.internal_detachDockPanel(panel);
}

/*!
Removes any information regarding history, location, restoring for the panel by its \a id.
*/
void DockPanelManager::forgetDockPanel(const QString& id)
{
    QTN_D(DockPanelManager);
    DockManagerUpdater managerUpdater(this);
    d.m_layoutEngine->removePanelInfo(id);
}

/*!
Removes all dock panels from the manager and destroy its instanceses.
Note: All panel's references become invalid after this call.
*/
void DockPanelManager::clearDockPanels()
{
    QTN_D(DockPanelManager);
    QList<DockWidgetPanel*> list = d.m_layoutEngine->widgetPanelList();
    for (int i = 0; i < list.count() - 1; ++i)
        d.internal_detachDockPanel(list[i]);
}

/*!
Returns a default title for the dock window.
*/
const QString& DockPanelManager::dockWindowTitle() const
{
    QTN_D(const DockPanelManager);
    return d.m_dockWindowTitle;
}

/*!
Sets a default title for the dock window.
*/
void DockPanelManager::setDockWindowTitle(const QString& title)
{
    QTN_D(DockPanelManager);
    d.m_dockWindowTitle = title;
}

/*!
Returns true if panel's contents shown while it dragging. Otherwise it is false.
\sa isDockPanelTransparentWhileDragging
*/
bool DockPanelManager::isDockPanelFullContentsWhileDraggingShown() const
{
    QTN_D(const DockPanelManager);
    return d.m_showDockWindowFullContentsWhileDragging;
}

/*!
Shows or hides the panel's contents while dragging depending on a\ show flag.
\sa setDockPanelTransparentWhileDragging
*/
void DockPanelManager::setDockPanelFullContentsWhileDraggingShown(bool show)
{
    QTN_D(DockPanelManager);
    d.m_showDockWindowFullContentsWhileDragging = show;
}

/*!
Returns true if the panel's transparent while it dragging. Otherwise it is false.
\sa isDockPanelFullContentsWhileDraggingShown
*/
bool DockPanelManager::isDockPanelTransparentWhileDragging() const
{
    QTN_D(const DockPanelManager);
    return d.m_panelTransparent;
}

/*!
Sets transparent panel while it dragging to \a transparent.
\sa setDockPanelFullContentsWhileDraggingShown
*/
void DockPanelManager::setDockPanelTransparentWhileDragging(bool transparent)
{
    QTN_D(DockPanelManager);
    d.m_panelTransparent = transparent;
}

/*!
Returns true if special arrow-markers shown while dragging dockpanel. Otherwise it returns false.
*/
bool DockPanelManager::isArrowMarkersShown() const
{
    QTN_D(const DockPanelManager);
    return d.m_showMarkers;
}

/*!
Shows or hides depending on \a show the special arrow markers that indicate dockable areas while dragging the dock panel.
*/
void DockPanelManager::setArrowMarkersShown(bool show)
{
    QTN_D(DockPanelManager);
    d.m_showMarkers = show;
}

/*!
Returns the panel features.
*/
DockWidgetPanel::DockPanelFeatures DockPanelManager::defaultPaneFeatures() const
{
    QTN_D(const DockPanelManager);
    return d.m_defaultPanelFeatures;
}

/*!
Sets the panel features.
*/
void DockPanelManager::setDefaultPaneFeatures(DockWidgetPanel::DockPanelFeatures features)
{
    QTN_D(DockPanelManager);
    d.m_defaultPanelFeatures = features;
}

/*!
Returns the current active dock panel.
*/
DockWidgetPanel* DockPanelManager::activeDockPanel() const
{
    QTN_D(const DockPanelManager);
    return d.m_activePanel;
}

bool DockPanelManager::isDockPanelFloat(const DockWidgetPanel* panel) const
{
    Q_ASSERT(panel != Q_NULL);
    return qobject_cast<DockWindow*>(panel->topDockWindow()) != Q_NULL;
}

void DockPanelManager::setDockPanelFloat(DockWidgetPanel* panel, bool set)
{
    QTN_D(DockPanelManager);
    if (panel == Q_NULL)
        return;

    bool floating = qobject_cast<DockWindow*>(panel->topDockWindow()) != Q_NULL;
    bool hidden = DockPanelBasePrivate::_get(panel)->isAutoHide();
    bool docking = !floating && !hidden;
    if (set)
    {
        QWidget* managedWidget = this->managedWidget();
        if (!floating && docking)
        {
            QRect rect = panel->parentPanel() != Q_NULL ? panel->parentPanel()->geometry() : QRect(0, 0, 0, 0);
            if (!rect.isNull())
                rect.moveTopLeft(managedWidget->mapToGlobal(rect.topLeft()));
            else
            {
                rect = QRect(QPoint(0, 0), QSize(250, 350));
                QRect geom = qtn_availableGeometry(managedWidget);
                QPoint centr = geom.center();
                rect.moveTo(centr.x() - (rect.width() / 2), centr.y() - (rect.height() / 2));
            }
            DockPanelBase* parentPanelBase = qobject_cast<DockPanelBase*>(panel->parentPanel());
            DockPanelBase* addPanelBase = parentPanelBase ? parentPanelBase : panel;
            addDockWindow(addPanelBase, rect);
            d.m_widgetLastFocus = Q_NULL;
        }
        else
        {
            QRect rect(QPoint(100, 100), QSize(250, 350));
            if (managedWidget != Q_NULL)
            {
                QRect rc = managedWidget->geometry();
                QPoint centr = rc.center();
                rect.moveTo(centr.x() - (rect.width() / 2), centr.y() - (rect.height() / 2));
            }
            addDockWindow(panel, rect);
            d.m_widgetLastFocus = Q_NULL;
        }
    }
    else
    {
        if ((hidden || floating) && !docking)
        {
            if (hidden)
                setDockPanelAutoHide(panel, false);

            if (DockWindow* dockWindow = qobject_cast<DockWindow*>(panel->topDockWindow()))
                d.switchDocking(dockWindow->layoutPanel());
        }
    }
}

bool DockPanelManager::isDockPanelAutoHide(const DockWidgetPanel* panel) const
{
    Q_ASSERT(panel != Q_NULL);
    return panel != Q_NULL && DockPanelBasePrivate::_get(panel)->isAutoHide();
}

void DockPanelManager::setDockPanelAutoHide(DockWidgetPanel* panel, bool set)
{
    QTN_D(DockPanelManager);
    Q_ASSERT(panel != Q_NULL);
    if (!set && DockPanelBasePrivate::_get(panel)->isAutoHide())
    {
        d.switchDocking(panel->parentPanel());
    }
    else
    {
        bool handled = true;
        emit aboutToAutoHide(panel, handled);
        if (handled)
            d.autoHideDockPanel(panel);
    }
}

/*!
Raises dock panel \a panel and set focus to the first widget on it if \a setFocus is true.
*/
void DockPanelManager::showDockPanel(DockWidgetPanel* widgetPanel, bool setFocus)
{
    QTN_D(DockPanelManager);
    if (widgetPanel == Q_NULL || d.m_layoutEngine == Q_NULL)
        return;

    bool handled = true;
    emit aboutToShow(widgetPanel, handled);
    if (!handled)
        return;

    DockManagerUpdater managerUpdater(this);
    DockPanelBase* parentBase = widgetPanel->parentPanel();
    if (parentBase == Q_NULL)
    {
        const DockPanelInfo& panelInfo = DockWidgetPanelPrivate::_get(widgetPanel)->panelInfo();
        if (DockTabbedLayoutPanel* layoutPanel = qobject_cast<DockTabbedLayoutPanel*>(panelInfo.lastOwner))
        {
            widgetPanel->setParentPanel(layoutPanel);
            layoutPanel->setActivePanel(widgetPanel, setFocus);

            if (DockWindow* dockWindow = qobject_cast<DockWindow*>(layoutPanel->topDockWindow()))
                dockWindow->setVisible(true);
        }
    }
    else if (DockTabbedLayoutPanel* tabbedLayout = qobject_cast<DockTabbedLayoutPanel*>(parentBase))
        tabbedLayout->showDockPanel(widgetPanel, setFocus);

    parentBase = widgetPanel->parentPanel();
    if (parentBase == Q_NULL)
    {
#if 0
        d.dockLayoutEngine()->attachDockPanel(widgetPanel);
#endif
        setDockPanelFloat(widgetPanel, true);
    }
        
    if (setFocus && d.m_layoutCreated)
        widgetPanel->setVisible(true);

    if (setFocus)
        widgetPanel->setFocus();
}

void DockPanelManager::closeDockPanel(DockWidgetPanel* widgetPanel)
{
    if (widgetPanel != Q_NULL)
    {
        DockManagerUpdater managerUpdater(this);

        bool handled = true;
        QCloseEvent event;
        event.accept();
        if (widgetPanel->widget() != Q_NULL)
            QCoreApplication::sendEvent(widgetPanel->widget(), &event);

        if (!event.isAccepted())
            handled = false;
        else
            emit aboutToClose(widgetPanel, handled);

        if (handled)
            widgetPanel->setParentPanel(Q_NULL);
    }
}

bool DockPanelManager::isDockPanelClosed(DockWidgetPanel* panel) const
{
    return panel == Q_NULL || panel->isClosed();
}

/*!
Returns the margin for for the dock layout.
*/
int DockPanelManager::margin() const
{
    QTN_D(const DockPanelManager);
    return d.m_margin;
}

/*!
Sets the margin for for the dock layout.
*/
void DockPanelManager::setMargin(int margin)
{
    QTN_D(DockPanelManager);
    d.m_margin = margin;
}

/*!
Returns the central widget for the dock layout.
*/
QWidget* DockPanelManager::centralWidget() const
{
    QTN_D(const DockPanelManager);
    Q_ASSERT(managedWidget() != Q_NULL);
    if (DockCentralPanel* centralPanel = qobject_cast<DockCentralPanel*>(d.centralPanel()))
        return centralPanel->widget();
    else if (CustomLayout* layout = qobject_cast<CustomLayout*>(managedWidget()->layout()))
        return layout->centralWidget();
    return Q_NULL;
}

/*!
Sets the central \a widget for the dock layout. Actualy it is query the layout from the managedWidget() and sets the central widget to it.
If you have DockPanelManager and DockBarManager on the same QWidget then DockPanelManager::setCentralWidget() and DockBarManager::setCentralWidget() do the identical thing.
*/
void DockPanelManager::setCentralWidget(QWidget* widget)
{
    QTN_D(DockPanelManager);
    Q_ASSERT(managedWidget() != Q_NULL);
    DockCentralPanel* centralPanel = qobject_cast<DockCentralPanel*>(d.centralPanel());
    if (centralPanel == Q_NULL)
    {
        Q_ASSERT(false);
        return; //Can't set central widget.
    }
    centralPanel->setWidget(widget);
    d.setCentralPanelVisible(widget != Q_NULL);
}

/*!
Fits the size of the dock panels to provide the user with a comfortable view.
*/
void DockPanelManager::bestFit()
{
    QTN_D(DockPanelManager);
    const QList<DockLayoutPanel*>& layouts = d.dockLayoutEngine()->layoutPanelList();
    for (QList<DockLayoutPanel*>::const_iterator it = layouts.constBegin(); it != layouts.constEnd(); ++it)
    {
        if (DockSplitLayoutPanel* splitter = qobject_cast<DockSplitLayoutPanel*>(*it))
            splitter->adjustDockingSize();
    }
}

/*!
Starts update the layout.
*/
void DockPanelManager::beginUpdate()
{
    QTN_D(DockPanelManager);
    d.m_updateLayoutCount++;
}

/*!
Ends update the layout.
*/
void DockPanelManager::endUpdate()
{
    QTN_D(DockPanelManager);
    if (d.m_updateLayoutCount == 0)
        return;
    d.m_updateLayoutCount--;
    if (d.m_updateLayoutCount == 0)
        d.recalcLayout();
}

/*!
Save layout of the dock panels internals to the file given by \a fileName with given \a autoFormatting.
\sa saveStateToDevice()
*/
bool DockPanelManager::saveStateToFile(const QString& fileName, bool autoFormatting)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    return saveStateToDevice(&file, autoFormatting);
}

/*!
Load layout of the internals dock panels from the given \a fileName.
\sa loadStateFromDevice()
*/
bool DockPanelManager::loadStateFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    return loadStateFromDevice(&file);
}

/*!
Saves the dock panels state to the \a device with given \a autoFormatting.
\sa saveStateToFile()
*/
bool DockPanelManager::saveStateToDevice(QIODevice* device, bool autoFormatting)
{
    QTN_D(DockPanelManager);
    d.m_stateManager = DockPanelManagerPrivate::Save;
    d.m_layoutEngine->saveStateToDevice(device, autoFormatting);
    d.m_stateManager = DockPanelManagerPrivate::Normal;
    return true;
}

/*!
Restores the dock panels state from the \a device.
*/
bool DockPanelManager::loadStateFromDevice(QIODevice* device)
{
    QTN_D(DockPanelManager);
    DockManagerUpdater updater(this);

    d.m_stateManager = DockPanelManagerPrivate::Read;
    qtn_ensurePanelIDs(d.m_layoutEngine);

    DockLayoutEngine* newLayoutEngine = new DockLayoutEngine(this);
    if (newLayoutEngine->loadStateFromDevice(device))
    {
        d.setLayoutEngine(newLayoutEngine);
        d.m_stateManager = DockPanelManagerPrivate::Normal;
        return true;
    }
    Q_DELETE_AND_NULL(newLayoutEngine);
    d.m_stateManager = DockPanelManagerPrivate::Normal;
    return false;
}

bool DockPanelManager::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != managedWidget())
        return QObject::eventFilter(watched, event);
    if (event->type() == QEvent::StyleChange)
        CommonStyle::ensureStyle();
    return QObject::eventFilter(watched, event);
}

