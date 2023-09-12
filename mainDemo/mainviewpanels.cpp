#include <QApplication>
#include <QLabel>
#include <QHeaderView>
#include <QListWidget>
#include <QToolButton>
#include <frameworktool.h>
#include "mainviewpanels.h"
#include <IPlugIn.h>
#include <pluginManager.h>
#include <moduleBase.h>
#include <customlistitem.h>
#include <popuplistwidget.h>

static QIcon getIconToolBox(const QPixmap& pixmap, int index)
{
    QRect rcImage(index * 16, 0, 16, 16);
    QImage copyImage = pixmap.copy(rcImage.left(), rcImage.top(), 16, 16).toImage();
    copyImage.setAlphaChannel(copyImage.createMaskFromColor(QColor(0, 255, 0).rgb(), Qt::MaskOutColor));
    return QIcon(QPixmap::fromImage(copyImage));
}

DockWidgetPanelCreator::DockWidgetPanelCreator(const QString& caption, DockMainWindow* parent)
    : QObject(parent)
{
    m_dockWidgetPanel = new DockWidgetPanel(caption, parent->dockPanelManager());
}

DockWidgetPanelCreator::~DockWidgetPanelCreator()
{

}


/* ReplayModuleListViewPanelCreator */
ReplayModuleListViewPanelCreator::ReplayModuleListViewPanelCreator(DockMainWindow* parent)
    : DockWidgetPanelCreator(tr("任务回放"), parent)
    , baseLayoutWidget(Q_NULL)
    , baseLayout(Q_NULL)
{
    baseLayoutWidget = new QWidget();
    baseLayout = new QVBoxLayout;
    baseLayout->setContentsMargins(0, 0, 0, 0);
    baseLayout->setSpacing(0);
    baseLayoutWidget->setFocusPolicy(Qt::NoFocus);
    baseLayoutWidget->setLayout(baseLayout);
    m_dockWidgetPanel->setWidget(baseLayoutWidget);
    fillToolBar();
    fillFileView();
}

ReplayModuleListViewPanelCreator::~ReplayModuleListViewPanelCreator()
{
}

void ReplayModuleListViewPanelCreator::fillToolBar()
{
/*
    DockToolBar* viewToolBar = new DockToolBar(m_dockWidgetPanel);
    viewToolBar->setFocusPolicy(Qt::NoFocus);
    QAction* act = viewToolBar->addAction(QIcon(QStringLiteral(":/res/newfile16x16.png")), tr("Properties"));
    act->setToolTip(tr("Show Properties"));
    baseLayout->addWidget(viewToolBar);
*/
}

void ReplayModuleListViewPanelCreator::fillFileView()
{
    QTreeWidget* treeWidget = new QTreeWidget;
    treeWidget->setFrameShape(QFrame::NoFrame);
    baseLayoutWidget->setFocusProxy(treeWidget);
    treeWidget->setFocusPolicy(Qt::StrongFocus);
    treeWidget->setAlternatingRowColors(false);

    treeWidget->setMinimumWidth(190);
    treeWidget->setHeaderHidden(true);

    QStringList strings;
    strings.append(tr("Solution 'qtitandocking'"));
    QTreeWidgetItem* mainItem = new QTreeWidgetItem(strings);
    mainItem->setExpanded(true);
    mainItem->setIcon(0, QIcon(QStringLiteral(":/res/solution.svg")));
    treeWidget->insertTopLevelItem(0, mainItem);
    treeWidget->expandItem(mainItem);

    strings.clear();
    strings.append(tr("DockApplication"));
    QTreeWidgetItem* itemAppl = new QTreeWidgetItem(mainItem, strings);
    QFont font = itemAppl->font(0);
    font.setBold(true);
    itemAppl->setFont(0, font);
    itemAppl->setIcon(0, QIcon(QStringLiteral(":/res/cpp_project.png")));
    itemAppl->setExpanded(true);
    treeWidget->insertTopLevelItem(1, itemAppl);
    treeWidget->expandItem(itemAppl);

    ////////////////////////////////////////////////////////////////////
    strings.clear();
    strings.append(tr("Source Files"));
    QTreeWidgetItem* itemSource = new QTreeWidgetItem(itemAppl, strings);
    itemSource->setIcon(0, QIcon(QStringLiteral(":/res/filterfolderOpen.png")));
    itemSource->setExpanded(true);
    treeWidget->insertTopLevelItem(1, itemSource);
    treeWidget->expandItem(itemAppl);

    strings.clear();
    strings.append(tr("main.cpp"));
    QTreeWidgetItem* item = new QTreeWidgetItem(itemSource, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    strings.clear();
    strings.append(tr("mainwindow.cpp"));
    item = new QTreeWidgetItem(itemSource, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    strings.clear();
    strings.append(tr("mdichild.cpp"));
    item = new QTreeWidgetItem(itemSource, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    strings.clear();
    strings.append(tr("fileviewpanel.cpp"));
    item = new QTreeWidgetItem(itemSource, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    ////////////////////////////////////////////////////////////////////
    strings.clear();
    strings.append(tr("Header Files"));
    QTreeWidgetItem* itemHeader = new QTreeWidgetItem(itemAppl, strings);
    itemHeader->setIcon(0, QIcon(QStringLiteral(":/res/filterfolderOpen.png")));
    itemHeader->setExpanded(true);
    treeWidget->insertTopLevelItem(1, itemHeader);
    treeWidget->expandItem(itemAppl);

    strings.clear();
    strings.append(tr("mainwindow.h"));
    item = new QTreeWidgetItem(itemHeader, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/header.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    strings.clear();
    strings.append(tr("mdichild.h"));
    item = new QTreeWidgetItem(itemHeader, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/header.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    strings.clear();
    strings.append(tr("fileviewpanel.h"));
    item = new QTreeWidgetItem(itemHeader, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/header.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    ////////////////////////////////////////////////////////////////////
    strings.clear();
    strings.append(tr("Resource Files"));
    QTreeWidgetItem* itemResource = new QTreeWidgetItem(itemAppl, strings);
    itemResource->setIcon(0, QIcon(QStringLiteral(":/res/filterfolderOpen.png")));
    itemResource->setExpanded(true);
    treeWidget->insertTopLevelItem(1, itemResource);
    treeWidget->expandItem(itemAppl);

    strings.clear();
    strings.append(tr("documentdemo.qrc"));
    item = new QTreeWidgetItem(itemResource, strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/header.png")));
    treeWidget->insertTopLevelItem(1, item);
    treeWidget->expandItem(item);

    baseLayout->addWidget(treeWidget, 1);
}

/* ModuleListViewPanelCreator */
ModuleListViewPanelCreator::ModuleListViewPanelCreator(DockMainWindow* parent)
    : DockWidgetPanelCreator(tr("功能视图"), parent)
{
    baseLayoutWidget = new QWidget();
    baseLayout = new QVBoxLayout;
    baseLayout->setContentsMargins(0, 0, 0, 0);
    baseLayout->setSpacing(6);
    baseLayoutWidget->setFocusPolicy(Qt::NoFocus);
    baseLayoutWidget->setLayout(baseLayout);
    m_dockWidgetPanel->setWidget(baseLayoutWidget);
    fillToolBar();
    fillModule();
}

ModuleListViewPanelCreator::~ModuleListViewPanelCreator()
{
//    if(m_lineEdit != nullptr){
//        delete m_lineEdit;
//        m_lineEdit = nullptr;
//    }
//    if(m_listWidget != nullptr){
//        delete m_listWidget;
//        m_listWidget = nullptr;
//    }
}

void ModuleListViewPanelCreator::fillToolBar()
{
    DockToolBar* viewToolBar = new DockToolBar(m_dockWidgetPanel);
    viewToolBar->setFocusPolicy(Qt::NoFocus);
    QAction* act = viewToolBar->addAction(tr("快捷按钮1"));
    act->setToolTip(tr("快捷按钮1"));
    viewToolBar->addSeparator();
    act = viewToolBar->addAction(tr("快捷按钮2"));
    act->setToolTip(tr("快捷按钮2"));
    viewToolBar->setContentsMargins(0, 0, 0, 0);

    baseLayout->addWidget(viewToolBar);
}

void ModuleListViewPanelCreator::fillModule()
{
    m_lineEdit = new QLineEdit();
    m_lineEdit->setContentsMargins(0, 0, 10, 0);
    baseLayout->addWidget(m_lineEdit);

    m_listWidget = new QListWidget();
    m_listWidget->setObjectName("listWidget");
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    baseLayoutWidget->setFocusProxy(m_listWidget);

    connect(m_listWidget, &QListWidget::itemClicked, this, &ModuleListViewPanelCreator::itemClicked);

    baseLayout->addWidget(m_listWidget, 1);
}

void ModuleListViewPanelCreator::loadAllPlugins(){
    if(m_listWidget != nullptr){
        m_listWidget->clear();

        //所有插件的信息
        QList<PlugInProperty> plugIns = PluginManager::instance()->allPluginsProperty();
        //功能视图插件的信息
        QList<ModuleProperty> modules;
        //非功能视图插件的信息
        QList<PlugInProperty> commonPlugIns;
        int length = plugIns.length();
        for (int i = 0; i < length; i++) {
            PlugInProperty property = plugIns.at(i);
            ModuleProperty moduleProperty = ModuleBase::convertFrom(property);
            if(moduleProperty.multipleInstance > 0){
                modules.append(moduleProperty);
            }
            else{
                commonPlugIns.append(property);
            }
        }
        //排序
        std::sort(modules.begin(), modules.end(), ModuleProperty::compareIndex);
        int moduleLength = modules.length();
        //当有功能视图默认加载时，其对应ListWidget中的下标，用于选中（如有多个视图默认加载，仅选中第一个视图）
        int selectIndex = -1;
        for (int i = 0; i < moduleLength; i++) {
            ModuleProperty property = modules.at(i);
            createListItem(property);

            PlugInProperty plugInProperty = PluginManager::instance()->getPluginProperty(property.name);
            if(plugInProperty.level == PlugInLevel::MustLoad){
                if(selectIndex < 0){
                    selectIndex = i;
                }
            }
        }

        //为功能视图添加右键菜单
        //    int commonLength = commonPlugIns.length();
        //    for (int i = 0; i < commonLength; i++) {
        //        PlugInProperty property = commonPlugIns.at(i);
        //        QAction *action = new QAction();
        //        action->setProperty(CUSTOMDATA, QVariant::fromValue(property));
        //        action->setText(property.displayName);
        //        if(!property.icon.isEmpty()){
        //            action->setIcon(QIcon(property.icon));
        //        }
        //        action->setToolTip(property.description);
        //        action->setCheckable(true);
        //        QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);
        //        if(loader != nullptr){
        //            action->setChecked(loader->isLoaded());
        //        }

        //        m_pluginMenu->addAction(action);
        //        m_pluginMenu->addSeparator();
        //    }
    }
}

void ModuleListViewPanelCreator::createListItem(ModuleProperty &property){
    PlugInProperty plugInProperty = PluginManager::instance()->getPluginProperty(property.name);

    CustomListItem *button = new CustomListItem();
    button->setAttribute(Qt::WA_TransparentForMouseEvents);
    button->setAttribute(Qt::WA_TranslucentBackground);
    button->setInfo(ListItemProperty(property.displayName, property.description, property.icon));
    button->setProperty(CUSTOMDATA, QVariant::fromValue(property));

    QListWidgetItem *listItem = new QListWidgetItem();
    listItem->setToolTip(property.description);
    //此多种方式才是默认设置
    listItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);

//    QWidget *widget = new QWidget;
//    auto hlay = new QHBoxLayout;
//    hlay->addWidget(button);
//    hlay->setContentsMargins(0,0,0,0);
//    widget->setLayout(hlay);
//    widget->setAttribute(Qt::WA_TransparentForMouseEvents);
//    widget->setAttribute(Qt::WA_TranslucentBackground, true);

    m_listWidget->addItem(listItem);
    m_listWidget->setItemWidget(listItem, button);

    if(plugInProperty.level == PlugInLevel::MustLoad){
        emit itemClicked(listItem);
    }
}

/* ResourceViewPanelCreator */
ResourceViewPanelCreator::ResourceViewPanelCreator(DockMainWindow* parent)
    : DockWidgetPanelCreator(tr("Resource View"), parent)
{
    baseLayoutWidget = new QWidget();
    baseLayout = new QVBoxLayout;
    baseLayout->setContentsMargins(0, 0, 0, 0);
    baseLayout->setSpacing(0);
    baseLayoutWidget->setFocusPolicy(Qt::NoFocus);
    baseLayoutWidget->setLayout(baseLayout);
    m_dockWidgetPanel->setWidget(baseLayoutWidget);
    fillResourceView();
}

ResourceViewPanelCreator::~ResourceViewPanelCreator()
{
}

void ResourceViewPanelCreator::fillResourceView()
{
    QTreeWidget* treeWidget = new QTreeWidget;
    treeWidget->setFrameShape(QFrame::NoFrame);
    baseLayoutWidget->setFocusProxy(treeWidget);
    treeWidget->setFocusPolicy(Qt::StrongFocus);
    treeWidget->setAlternatingRowColors(false);

    treeWidget->setMinimumWidth(190);
    treeWidget->setHeaderHidden(true);

    QStringList strings;
    strings.append(tr("Docking_DocumentDemo"));
    QTreeWidgetItem* item = new QTreeWidgetItem(strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp_project.png")));
    treeWidget->insertTopLevelItem(0, item);

    strings.clear();
    strings.append(tr("Docking_DynamicPanels"));
    item = new QTreeWidgetItem(strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp_project.png")));
    treeWidget->insertTopLevelItem(0, item);

    strings.clear();
    strings.append(tr("Docking_MDIWindowDemo"));
    item = new QTreeWidgetItem(strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp_project.png")));
    treeWidget->insertTopLevelItem(0, item);

    strings.clear();
    strings.append(tr("Docking_SimplePanels"));
    item = new QTreeWidgetItem(strings);
    item->setIcon(0, QIcon(QStringLiteral(":/res/cpp_project.png")));
    treeWidget->insertTopLevelItem(0, item);

    baseLayout->addWidget(treeWidget);
}

OutputViewPanelCreator::OutputViewPanelCreator(DockMainWindow* parent)
    : DockWidgetPanelCreator(tr("输出/调试"), parent)
{
    m_textEdit = new QTextEdit();
    m_textEdit->setReadOnly(true);
    m_textEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
    m_textEdit->setAutoFillBackground(true);
    m_textEdit->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);
//    QPalette palette = m_textEdit->palette();
//    palette.setColor(m_textEdit->backgroundRole(), palette.color(QPalette::Base));
//    m_textEdit->setPalette(palette);

    m_textEdit->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_dockWidgetPanel->setWidget(m_textEdit);
}

OutputViewPanelCreator::~OutputViewPanelCreator()
{
//    if(m_textEdit != nullptr){
//        delete m_textEdit;
//        m_textEdit = nullptr;
//    }
}

/* WatchPanelCreator */
WatchPanelCreator::WatchPanelCreator(DockMainWindow* parent)
    : DockWidgetPanelCreator(tr("Watch"), parent)
{
    m_watchList = new QTableWidget(m_dockWidgetPanel);
    m_dockWidgetPanel->setWidget(m_watchList);

    m_watchList->setColumnCount(3);
    m_watchList->setRowCount(6);
    m_watchList->setShowGrid(true);
    m_watchList->setWordWrap(true);
    QHeaderView* vHeader = m_watchList->verticalHeader();
    vHeader->setVisible(false);

    m_watchList->setShowGrid(false);
    m_watchList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_watchList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_watchList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_watchList->horizontalHeader()->setStretchLastSection(true);

    m_watchList->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Variable")));
    m_watchList->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Value")));
    m_watchList->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Type")));

    int height = m_watchList->horizontalHeader()->sizeHint().height();
    m_watchList->setRowHeight(0, height);
    m_watchList->setItem(0, 0, new QTableWidgetItem(tr("m_size")));
    m_watchList->setItem(0, 1, new QTableWidgetItem(tr("120")));

    m_watchList->setItem(1, 0, new QTableWidgetItem(tr("strName")));
    m_watchList->setItem(1, 1, new QTableWidgetItem(tr("\"John\"")));
}

WatchPanelCreator::~WatchPanelCreator()
{
}

/* WatchPanel */
PropertyGridViewPanelCreator::PropertyGridViewPanelCreator(DockMainWindow* parent)
    : DockWidgetPanelCreator(tr("ToolBox"), parent)
{
    m_toolBox = new QTreeWidget();
    m_toolBox->setFrameShape(QFrame::NoFrame);
    m_toolBox->setHeaderHidden(true);
    m_dockWidgetPanel->setWidget(m_toolBox);

    ///////////////////////////////////////////////////////////////////////
    QPixmap pixmap1(QStringLiteral(":/res/toolbox1.png"));

    QStringList strings;
    strings.append(tr("Windows Forms"));
    QTreeWidgetItem* formItem = new QTreeWidgetItem(strings);
    m_toolBox->insertTopLevelItem(0, formItem);
    m_toolBox->expandItem(formItem);

    QString sourceForms = tr("Pointer\nLabel\nButton\nText Box\nMain Menu\nCheck Box\nRadio Button\nGroup Box\nPicture Box\nPanel\nData Grid\nList Box\nChecked List Box\nCombo Box\nList View\nTree View\nTab Control\nDate Time Picker\nMonth Calendar\nHorizontal Scroll Bar\nVertical Scroll Bar\nTimer\nSplitter\nDomain UpDown\nNumeric UpDown\nTrack Bar\nProgress Bar\nRich Text Box");
    QStringList lstForms = sourceForms.split(QLatin1Char('\n'));
    int index = 0;
    for (QStringList::const_iterator it = lstForms.constBegin(); it != lstForms.constEnd(); ++it)
    {
        strings.clear();
        strings.append((*it));
        QTreeWidgetItem* item = new QTreeWidgetItem(formItem, strings);
        item->setIcon(0, getIconToolBox(pixmap1, index));
        m_toolBox->insertTopLevelItem(1, item);
        index++;
    }
    ///////////////////////////////////////////////////////////////////////
    QPixmap pixmap2(QStringLiteral(":/res/toolbox2.png"));

    strings.clear();
    strings.append(tr("Components"));
    QTreeWidgetItem* componentsItem = new QTreeWidgetItem(strings);
    m_toolBox->insertTopLevelItem(1, componentsItem);
    m_toolBox->expandItem(componentsItem);

    QString sourceComponents = tr("Pointer\nData Set\nOleDb Data Adapter\nOleDb Connection");
    QStringList lstComponents = sourceComponents.split(QLatin1Char('\n'));
    index = 0;
    for (QStringList::const_iterator it = lstComponents.constBegin(); it != lstComponents.constEnd(); ++it)
    {
        strings.clear();
        strings.append((*it));
        QTreeWidgetItem* item = new QTreeWidgetItem(componentsItem, strings);
        item->setIcon(0, getIconToolBox(pixmap2, index));
        m_toolBox->insertTopLevelItem(1, item);
        index++;
    }
    ///////////////////////////////////////////////////////////////////////
    QPixmap pixmap3(QStringLiteral(":/res/toolbox3.png"));

    strings.clear();
    strings.append(tr("Data"));
    QTreeWidgetItem* dataItem = new QTreeWidgetItem(strings);
    m_toolBox->insertTopLevelItem(2, dataItem);
    m_toolBox->expandItem(dataItem);

    QString sourceData = tr("Pointer\nData Set\nOleDb Data Adapter\nOleDb Connection");
    QStringList lstData = sourceComponents.split(QLatin1Char('\n'));
    index = 0;
    for (QStringList::const_iterator it = lstData.constBegin(); it != lstData.constEnd(); ++it)
    {
        strings.clear();
        strings.append((*it));
        QTreeWidgetItem* item = new QTreeWidgetItem(dataItem, strings);
        item->setIcon(0, getIconToolBox(pixmap3, index));
        m_toolBox->insertTopLevelItem(1, item);
        index++;
    }
}

PropertyGridViewPanelCreator::~PropertyGridViewPanelCreator()
{
}
