//解决中文乱码
#pragma execution_character_set("utf-8")

#include "maindockwindow.h"
#include <QFileInfo>
#include <QDebug>
#include <QListView>
#include <globalColors.h>
#include <globalSizes.h>
#include <QMetaProperty>
#include <QDir>
#include <QSettings>
#include <QDesktopWidget>
#include <QtMath>
#include <QMenuBar>
#include <QAction>
#include <globalhelper.h>
#include <pluginManager.h>
#include <loggerBase.h>
#include <moduleBase.h>
#include <QDebug>
#include <QResource>
#include <QLabel>
#include <aboutDialog.h>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QScreen>
#include <customnativecontrol.h>
#include <frameworktool.h>
#include <QToolButton>

MainDockWindow::MainDockWindow(QWidget *parent)
    : CustomDockWindowBase(parent)
{
    //初始化菜单功能
    this->initMenu();

    //初始化Dock窗
    createDockPanels();

    //建立所有信号和槽
    this->connectAllSignal();

//    if(dockBarManager()->managedWidget() != nullptr){
//        dockBarManager()->managedWidget()->setStyleSheet(frameworkTool::getAppCss());
//    }
//    if(dockBarManager()->centralWidget() != nullptr){
//        dockBarManager()->centralWidget()->setStyleSheet(frameworkTool::getAppCss());
//    }

}

MainDockWindow::~MainDockWindow()
{

}

void MainDockWindow::createDockPanels()
{
    m_moduleListView = new ModuleListViewPanelCreator(this);
    dockPanelManager()->insertDockPanel(m_moduleListView->widgetPanel(), Qtitan::LeftDockPanelArea);
//    connect(m_classViewAction, SIGNAL(triggered(bool)), m_panelClassView->widgetPanel()->visibleAction(), SIGNAL(triggered(bool)));
    ModuleListViewPanelCreator *moduleListViewPanelCreator = (ModuleListViewPanelCreator *)m_moduleListView;
    connect(moduleListViewPanelCreator, &ModuleListViewPanelCreator::itemClicked, this, &MainDockWindow::slot_itemClicked);
    moduleListViewPanelCreator->loadAllPlugins();

    m_replayModuleListView = new ReplayModuleListViewPanelCreator(this);
    dockPanelManager()->insertDockPanel(m_replayModuleListView->widgetPanel(), Qtitan::InsideDockPanelArea, m_moduleListView->widgetPanel());
//    connect(m_solutionExplorerAction, SIGNAL(triggered(bool)), m_panelSolutionExplorer->widgetPanel()->visibleAction(), SIGNAL(triggered(bool)));

    m_outputView = new OutputViewPanelCreator(this);
    dockPanelManager()->insertDockPanel(m_outputView->widgetPanel(), Qtitan::BottomDockPanelArea);
//    dockPanelManager()->insertDockPanel(m_outputView->widgetPanel(), Qtitan::InsideDockPanelArea, m_replayModuleListView->widgetPanel());
//    dockPanelManager()->insertDockPanel(m_outputView->widgetPanel(), Qtitan::InsideDockPanelArea, m_moduleListView->widgetPanel());
    //很奇怪，这里setAutoHide(true)会导致第一次加载时样式异常，手动换主题后又正常
    //    m_outputView->widgetPanel()->setAutoHide(true);
    OutputViewPanelCreator *outputViewPanelCreator = (OutputViewPanelCreator *)m_outputView;
    //这里设置输出/调试信息的容器，便于在写日志时追加日志到此容器
    ACFProperty::instance()->setOutputText(outputViewPanelCreator->getTextEdit());

    m_propertyGridView = new PropertyGridViewPanelCreator(this);
    dockPanelManager()->insertDockPanel(m_propertyGridView->widgetPanel(), Qtitan::RightDockPanelArea);

    m_moduleListView->widgetPanel()->activate();

}

IPlugIn *MainDockWindow::getPlugIn(DockPanelBase* panel){
    DockWidgetPanel *dockWidgetPanel = qobject_cast<DockWidgetPanel *>(panel);
    if(dockWidgetPanel != nullptr){
        QWidget* widget = dockWidgetPanel->widget();
        if(widget != nullptr){
            IPlugIn *plugin = qobject_cast<IPlugIn *>(widget);
            return plugin;
        }
    }
    return nullptr;
}

void MainDockWindow::slot_aboutToShow(DockPanelBase* panel, bool& handled){
    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, "进入slot_aboutToShow")));
}
void MainDockWindow::slot_aboutToClose(DockPanelBase* panel, bool& handled){
    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, "进入slot_aboutToClose")));
    IPlugIn *plugin = getPlugIn(panel);
    if(plugin != nullptr){
        PlugInProperty property = plugin->getPlugInProperty();
//        delete widget;
    }
}
void MainDockWindow::slot_aboutToAutoHide(DockPanelBase* panel, bool& handled){
    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, "进入slot_aboutToAutoHide")));

}
void MainDockWindow::slot_aboutToFloat(DockPanelBase* panel, bool& handled){
    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, "进入slot_aboutToFloat")));

}
void MainDockWindow::slot_dockPanelActivated(DockWidgetPanel* panel){
    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, "进入slot_dockPanelActivated")));

    //设置参数区域、快捷键区域
    IPlugIn *plugin = getPlugIn(panel);
    if(plugin != nullptr){
        PlugInProperty property = plugin->getPlugInProperty();
//        delete widget;
    }
}
void MainDockWindow::slot_dockPanelDeactivated(DockWidgetPanel* panel){
    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, "进入slot_dockPanelDeactivated")));

    //TODO
    //清空、重置参数区域、快捷键区域
}

DockWidgetPanel* MainDockWindow::findDockChild(const QString& name) const
{
    QList<DockDocumentPanel*> lstPanel = findChildren<DockDocumentPanel*>();
//    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    foreach (DockPanelBase* panel, lstPanel)
    {
        if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panel))
        {
            QWidget* child = widgetPanel->widget();
            if (child != nullptr){
                ModuleProperty property = child->property(CUSTOMDATA).value<ModuleProperty>();
                if(property.name == name){
                    return widgetPanel;
                }
            }
        }
    }
    return Q_NULL;
}

int MainDockWindow::findDockPanelCount(const QString& name) const{
    int openDockCount = 0;
    QList<DockDocumentPanel*> lstPanel = findChildren<DockDocumentPanel*>();
    foreach (DockPanelBase* panel, lstPanel)
    {
        if (DockWidgetPanel* widgetPanel = qobject_cast<DockWidgetPanel*>(panel))
        {
            QWidget* child = widgetPanel->widget();
            if (child != nullptr){
                ModuleProperty property = child->property(CUSTOMDATA).value<ModuleProperty>();
                if(property.name == name){
                    openDockCount++;
                }
            }
        }
    }
    return openDockCount;
}

bool MainDockWindow::openModule(IPlugIn& plugin){
    PlugInProperty property = plugin.getPlugInProperty();
    ModuleProperty moduleProperty = ModuleBase::convertFrom(property);
    //仅允许1个实例的情况下
    if(moduleProperty.multipleInstance == 1){
        if (DockWidgetPanel* panel = findDockChild(property.name))
        {
            panel->activate();
            return true;
        }
    }

    //获取此名称Dock面板已打开的个数
    int openCount = findDockPanelCount(property.name);
    if(openCount >= moduleProperty.multipleInstance){
        ACFProperty::instance()->getNotifyPlugIn()->sendData(QVariant::fromValue(NotifyStruct(NotifyLevel::WarnLevel, "操作", QString("此功能打开的窗体已达到上限[%1]！").arg(moduleProperty.multipleInstance))));

        return false;
    }

    const bool succeeded = loadModule(plugin, openCount);
    return succeeded;
}

bool MainDockWindow::loadModule(IPlugIn& plugin, const int &openCount)
{
    PlugInProperty property = plugin.getPlugInProperty();
    ModuleProperty moduleProperty = ModuleBase::convertFrom(property);
    if(moduleProperty.multipleInstance <= 0){
        return false;
    }

    QWidget *widget = nullptr;
    if(openCount == 0){
        widget = plugin.getWidget();
    }
    else if(openCount > 0){
        widget = plugin.createNewPlugin()->getWidget();
    }
    widget->setProperty(CUSTOMDATA, QVariant::fromValue(moduleProperty));
    DockWidgetPanel::DockPanelFeature feature = DockWidgetPanel::NoDockPanelFeatures;
    if(moduleProperty.canClose){
        feature = DockWidgetPanel::AllDockPanelFeatures;
    }
    else{
        feature = DockWidgetPanel::NoDockPanelFeatures;
    }

    //最后一个功能窗体的DockPanelBase
//    DockPanelBase *parent = nullptr;
//    QList<DockWidgetPanel*> widgetPanels = dockPanelManager()->widgetPanelList();
//    foreach (DockWidgetPanel* panel, widgetPanels)
//    {
//        QWidget* child = panel->widget();
//        if (child != nullptr){
//            ModuleProperty propertyTemp = child->property(CUSTOMDATA).value<ModuleProperty>();
//            if(!propertyTemp.name.isEmpty()){
//                parent = panel;
//            }
//        }
//    }

//    DockWidgetPanel* documentPanel = dockPanelManager()->addDocumentPanel(moduleProperty.displayName, moduleProperty.description, feature); //addDocumentPanel(moduleProperty.displayName);
    QString titleName = moduleProperty.displayName;
    if(openCount > 0){
        titleName += QString("-窗体%1").arg(openCount);
    }
    DockWidgetPanel* documentPanel = addDocumentPanel(titleName);
    documentPanel->setWidget(widget);

//    documentPanel->setIcon(QIcon(moduleProperty.icon));

//    DockWidgetPanel *dockWidgetPanel = new DockWidgetPanel(moduleProperty.displayName, dockPanelManager());
//    dockWidgetPanel->setWidget(widget);
//    dockWidgetPanel->titleBar()->setToolTip(moduleProperty.description);
//    dockWidgetPanel->setFeatures(feature);
//    dockWidgetPanel->setAllowedAreas(Qtitan::InsideDockPanelArea);
//    dockWidgetPanel->setTitleBar(Q_NULL);
//    dockPanelManager()->insertDockPanel(documentPanel, Qtitan::InsideDockPanelArea);

    return true;
}

void MainDockWindow::initMenu(){
    m_testAction = new QAction(tr("测试"), this);
    m_testAction->setStatusTip(tr("测试的状态提示"));
    m_testAction->setToolTip(tr("测试的提示"));
    m_testAction->setPriority(QAction::LowPriority);
    m_testAction->setShortcut(QKeySequence::New);
    connect(m_testAction, &QAction::triggered, this, &MainDockWindow::slot_rightMenuEvent);
}

void MainDockWindow::slot_rightMenuEvent(){
    QMessageBox messageBox(QMessageBox::Information, "提示", "响应右键菜单成功！");
    messageBox.setStyleSheet(this->styleSheet());
    messageBox.setStandardButtons(QMessageBox::Ok);// | QMessageBox::Cancel
    messageBox.setButtonText(QMessageBox::Ok, QString("确 定"));
    int retFlag = messageBox.exec();
}

void MainDockWindow::initIcon(){
    SvgHelper::updateSvg("./appPics/svgs/theme/info.svg", GlobalColors::instance()->InfoColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/success.svg", GlobalColors::instance()->SuccessColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/warn.svg", GlobalColors::instance()->WarningColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/error.svg", GlobalColors::instance()->DangerColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/fatal.svg", GlobalColors::instance()->PrimaryTextColor());

//    int moduleCount = ui->listWidget->count();
//    for (int i = 0; i < moduleCount; i++) {
//        QWidget *widget = ui->listWidget->itemWidget(ui->listWidget->item(i));
//        if(widget){
//            CustomListItem *customItem = qobject_cast<CustomListItem *>(widget);
//            if(customItem){
//                ModuleProperty property = widget->property(CUSTOMDATA).value<ModuleProperty>();
//                customItem->icon()->setIcon(QIcon(property.icon));
//            }
//        }
//    }
//    int tabCount = ui->tabWidget->count();
//    for (int i = 0; i < tabCount; i++) {
//        QString text = ui->tabWidget->tabText(i);
//        if(m_tabPropertys.contains(text)){
//            ui->tabWidget->setTabIcon(i, QIcon(m_tabPropertys.value(text).icon));
//        }

//        QWidget *rightSide = ((QTabBar*)(ui->tabWidget->tabBar()))->tabButton(i, QTabBar::RightSide);
//        if(rightSide){
//            QPushButton *closeBtn = static_cast<QPushButton *>(rightSide);
//            if(closeBtn){
//                closeBtn->setIcon(QIcon("./appPics/svgs/theme/close.svg"));
//            }
//        }
//    }

    //20230317验证多次改变分辨率后有bug
    //        if(m_changeRateCount == 1){
    //            //直接将tabBar的最小值设置为qtabwidget的最小值，就可以自适应tabBar的宽度。
    //            int tabWidgetWidth = this->ui->tabWidget->width();
    //            this->ui->tabWidget->tabBar()->setMinimumWidth(tabWidgetWidth);
    //        }
    //        m_changeRateCount++;

    //设置好图像后，发现这个图标的大小不是我想要的，想调整一下，首先想到的办法使用样式表，但是
    //QTabBar::close-button {image: url(://close.svg);width: 56;height 56;}
    //无效。
    //上网搜了许多也搜到结果，自己总结了如下：
    //如果是qss设置的图标，可以这样批量修改图标，也可单独修改。
//    QSize iconSize(GlobalSizes::instance()->DefaultIconHeight(), GlobalSizes::instance()->DefaultIconHeight());
//    for (int i = 0; i < tabCount; i++)
//    {
//        ui->tabWidget->setIconSize(iconSize);
//        if(ui->tabWidget->tabBar()->tabButton(i, QTabBar::RightSide) != nullptr){
//            ui->tabWidget->tabBar()->tabButton(i, QTabBar::RightSide)->setFixedSize(iconSize);
//        }
//    }
//    ui->tabWidget->update();

}

void MainDockWindow::connectAllSignal(){
    connect(frameworkTool::instance(), &frameworkTool::cssStyleChanged, this, &MainDockWindow::slot_cssStyleChanged);

    connect(dockPanelManager(), &DockPanelManager::aboutToShow, this, &MainDockWindow::slot_aboutToShow);
    connect(dockPanelManager(), &DockPanelManager::aboutToClose, this, &MainDockWindow::slot_aboutToClose);
    connect(dockPanelManager(), &DockPanelManager::aboutToFloat, this, &MainDockWindow::slot_aboutToFloat);
    connect(dockPanelManager(), &DockPanelManager::aboutToAutoHide, this, &MainDockWindow::slot_aboutToAutoHide);
    connect(dockPanelManager(), &DockPanelManager::dockPanelActivated, this, &MainDockWindow::slot_dockPanelActivated);
    connect(dockPanelManager(), &DockPanelManager::dockPanelDeactivated, this, &MainDockWindow::slot_dockPanelDeactivated);

}

void MainDockWindow::slot_itemClicked(QListWidgetItem *current){
    QWidget *widget = current->listWidget()->itemWidget(current);
    if(widget != nullptr){
        ModuleProperty property = widget->property(CUSTOMDATA).value<ModuleProperty>();

        QString path = PluginManager::instance()->getPath(property.name);
        PluginManager::instance()->loadPlugin(path);
        QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);
        if(loader != nullptr){
            IPlugIn *plugin = qobject_cast<IPlugIn *>(loader->instance());
            if(plugin)
            {
                openModule(*plugin);
//                this->addTab(plugin);
            }
        }
    }
}

void MainDockWindow::slot_languageChanged(){

}

/**
 * @brief MainDockWindow::slot_cssStyleChanged 系统的css样式改变后，主界面的额外处理
 */
void MainDockWindow::slot_cssStyleChanged(){
    this->initIcon();

    QList<DockWidgetPanel*> widgetPanels = dockPanelManager()->widgetPanelList();
    foreach (DockWidgetPanel* panel, widgetPanels)
    {
        panel->setStyleSheet(frameworkTool::getAppCss());
        if(panel->widget() != nullptr){
            panel->widget()->setStyleSheet(frameworkTool::getAppCss());
        }
    }
    QList<DockDocumentPanel*> documentPanels = dockPanelManager()->documentPanelList();
    foreach (DockDocumentPanel* panel, documentPanels)
    {
        panel->setStyleSheet(frameworkTool::getAppCss());
        if(panel->widget() != nullptr){
            panel->widget()->setStyleSheet(frameworkTool::getAppCss());
        }
    }
    if(m_outputView != nullptr){
        m_outputView->widgetPanel()->widget()->setStyleSheet(frameworkTool::getAppCss());
        m_outputView->widgetPanel()->menuButton()->setStyleSheet(frameworkTool::getAppCss());
        m_outputView->widgetPanel()->titleBar()->setStyleSheet(frameworkTool::getAppCss());
    }
    QList<DockToolBar*> toolBars = dockBarManager()->toolBars();
    foreach (DockToolBar* toolBar, toolBars)
    {
        toolBar->setStyleSheet(frameworkTool::getAppCss());
    }
    QList<DockBarBase*> dockBars = dockBarManager()->dockBars();
    foreach (DockBarBase* dockBar, dockBars)
    {
        dockBar->setStyleSheet(frameworkTool::getAppCss());
    }

    QList<QToolButton *> toolBtns = findChildren<QToolButton *>();
    foreach (QToolButton* toolBtn, toolBtns)
    {
        toolBtn->setStyleSheet(frameworkTool::getAppCss());
        QString className = toolBtn->metaObject()->className();
        qDebug() << "className: " << className;
        if(className.contains("DockWidgetTitleButton")){
//            toolBtn->setFixedSize(GlobalSizes::instance()->DefaultControlHeight(), GlobalSizes::instance()->DefaultControlHeight());
//            toolBtn->setIconSize(QSize(GlobalSizes::instance()->DefaultIconHeight() * 0.4, GlobalSizes::instance()->DefaultIconHeight() * 0.4));
//            toolBtn->setProperty("isIconTransparent", true);

//            if(toolBtn->defaultAction() != nullptr){
//                QVariant btnType = toolBtn->defaultAction()->property(_qtn_Type_Button);
//                if(btnType.isValid()){
//                    bool ok = false;
//                    int typeButton = btnType.toInt(&ok);
//                    if(ok){
//                        if (typeButton == 0 /*AutoHideButton*/)
//                            toolBtn->setIcon(QIcon("./appPics/svgs/theme/minSize.svg"));
//                        else if (typeButton == 1 /*CloseButton*/)
//                            toolBtn->setIcon(QIcon("./appPics/svgs/theme/close.svg"));
//                        else if (typeButton == 2 /*MenuButton*/)
//                            toolBtn->setIcon(QIcon("./appPics/svgs/theme/maxSize.svg"));
//                    }
//                }
//            }
        }
    }

}

void MainDockWindow::showEvent(QShowEvent* event){
    if(m_outputView != nullptr && m_isFirstLoad){
        m_outputView->widgetPanel()->setAutoHide(true);
        m_isFirstLoad = false;
    }
    return CustomDockWindowBase::showEvent(event);
}
