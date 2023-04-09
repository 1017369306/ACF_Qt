#include "mainwindow.h"
#include "ui_mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon("./appPics/icons/rabbit.ico"));

    //初始化顶部
    this->initWidgetTop();

    //初始化功能列表
    this->initModules();

    //此分辨率下，初始化元素的相对大小
    frameworkTool::initResolution();

    //获取有哪些主题样式
    this->loadAllTheme();

    //加载指定目录下的所有插件
    this->loadAllPlugins();

    //建立所有信号和槽
    this->connectAllSignal();

    m_notify = PluginManager::instance()->getIPlugin(notifyName);

    if(m_notify != nullptr){
        m_notify->sendData(QVariant::fromValue(NotifyStruct(NotifyLevel::SuccessLevel, "测试", "测试信息！！！今天的天气真好，真象出去玩一玩！")));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidgetTop(){
    this->initMenu();

    this->initReSizeArea();

    ui->tabWidget->setTabsClosable(true);
}

void MainWindow::initMenu(){
    QList<ActionProperty> systemActions{
        {tr("插件管理"), "",tr("插件管理"),false},
        {tr("日志管理"), "",tr("日志管理"),false},
        {tr("重启系统"), "",tr("重启系统"),false},
        {tr("退出"), "",tr("退出"),false}
    };
    QList<ActionProperty> codeActions{
        {tr("github"), "",tr("github"),false},
        {tr("gitee"), "",tr("gitee"),false}
    };
    QList<ActionProperty> aboutActions{
        {tr("博客"), "",tr("博客"),false},
        {tr("关于"), "",tr("关于"),false}
    };

    //菜单中的系统
    QMenu *systemMenu = new QMenu();
    systemMenu->setTitle(tr("系统"));
    systemMenu->setAttribute(Qt::WA_TranslucentBackground);
    //连接信号槽
    connect(systemMenu, SIGNAL(triggered(QAction*)), this, SLOT(slots_menuTriggered(QAction*)));
    for (int i = 0; i < systemActions.length(); i++) {
        ActionProperty property = systemActions.at(i);
        if(i == 0){
            QMenu *menu = new QMenu();
            menu->setTitle(property.text);
            if(!property.icon.isEmpty()){
                menu->setIcon(QIcon(property.icon));
            }
            menu->setToolTip(property.tooltip);
            systemMenu->addMenu(menu);
            systemMenu->addSeparator();
            m_pluginMenu = menu;
            m_allMenus.append(menu);
        }
        else{
            QAction *action = new QAction();
            action->setText(property.text);
            if(!property.icon.isEmpty()){
                action->setIcon(QIcon(property.icon));
            }
            action->setToolTip(property.tooltip);
            action->setCheckable(property.checkable);

            systemMenu->addAction(action);
            systemMenu->addSeparator();
        }
    }

    //菜单中的代码仓库
    QMenu *codeMenu = new QMenu();
    codeMenu->setTitle(tr("代码仓库"));
    codeMenu->setAttribute(Qt::WA_TranslucentBackground);
    //连接信号槽
    connect(codeMenu, SIGNAL(triggered(QAction*)), this, SLOT(slots_menuTriggered(QAction*)));
    for (int i = 0; i < codeActions.length(); i++) {
        ActionProperty property = codeActions.at(i);
        QAction *action = new QAction();
        action->setText(property.text);
        if(!property.icon.isEmpty()){
            action->setIcon(QIcon(property.icon));
        }
        action->setToolTip(property.tooltip);
        action->setCheckable(property.checkable);

        codeMenu->addAction(action);
        codeMenu->addSeparator();
    }

    //菜单中的关于
    QMenu *aboutMenu = new QMenu();
    aboutMenu->setTitle(tr("关于"));
    aboutMenu->setAttribute(Qt::WA_TranslucentBackground);
    //连接信号槽
    connect(aboutMenu, SIGNAL(triggered(QAction*)), this, SLOT(slots_menuTriggered(QAction*)));
    for (int i = 0; i < aboutActions.length(); i++) {
        ActionProperty property = aboutActions.at(i);
        QAction *action = new QAction();
        action->setText(property.text);
        if(!property.icon.isEmpty()){
            action->setIcon(QIcon(property.icon));
        }
        action->setToolTip(property.tooltip);
        action->setCheckable(property.checkable);

        aboutMenu->addAction(action);
        aboutMenu->addSeparator();
    }

    m_menuBar = new QMenuBar();
    m_menuBar->addMenu(systemMenu);
    m_menuBar->addMenu(codeMenu);
    m_menuBar->addMenu(aboutMenu);

    ui->horizontalLayout_menuBar->addWidget(m_menuBar);

    m_allMenus.append(systemMenu);
    m_allMenus.append(codeMenu);
    m_allMenus.append(aboutMenu);

}

void MainWindow::initModules(){
    ui->listWidget->setObjectName("listWidget");
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

void MainWindow::slots_menuTriggered(QAction *action){
    QVariant customData = action->property(CUSTOMDATA);
    if(customData.isValid()){
        PlugInProperty property = action->property(CUSTOMDATA).value<PlugInProperty>();
        QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);

        if(loader == nullptr){
            return;
        }
        if(!action->isChecked())
        {
            if(loader->isLoaded()){
                loader->unload();
            }
        }
        else{
            if(!loader->isLoaded()){
                if(loader->load()){
                    IPlugIn *plugin = qobject_cast<IPlugIn *>(loader->instance());
                    if(plugin)
                    {
                        plugin->initModule();
                    }
                }
            }
        }
    }
    else{
        QString text = action->text();

        if(text == "日志管理"){

        }
        else if(text == "退出"){
            this->close();
        }
        else if(text == "重启系统"){
            qApp->exit(CODE_RESTART);
        }
        else if(text == "github"){
            QDesktopServices::openUrl(QUrl("https://github.com/"));
        }
        else if(text == "gitee"){
            QDesktopServices::openUrl(QUrl("https://gitee.com/"));
        }
        else if(text == "博客"){
            QMessageBox messageBox(QMessageBox::Warning, "警告", "暂时无博客！");
            messageBox.setStyleSheet(this->styleSheet());
            messageBox.setStandardButtons(QMessageBox::Ok);// | QMessageBox::Cancel
            messageBox.setButtonText(QMessageBox::Ok, QString("确 定"));
//            messageBox.setButtonText(QMessageBox::Cancel, QString("取 消"));
            int retFlag = messageBox.exec();
        }
        else if(text == "关于"){
            aboutDialog *about = new aboutDialog();
            about->setStyleSheet(this->styleSheet());
            about->setFixedSize(GlobalSizes::instance()->DefaultControlHeight() * 16, GlobalSizes::instance()->DefaultControlHeight() * 9);
            about->exec();
        }
    }

}

void MainWindow::initReSizeArea(){

    this->initIcon();
    this->m_themeListWidget = new PopupListWidget(this);

}

void MainWindow::initIcon(){ 
    SvgHelper::updateSvg("./appPics/svgs/theme/info.svg", GlobalColors::instance()->InfoColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/success.svg", GlobalColors::instance()->SuccessColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/warn.svg", GlobalColors::instance()->WarningColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/error.svg", GlobalColors::instance()->DangerColor());
    SvgHelper::updateSvg("./appPics/svgs/theme/fatal.svg", GlobalColors::instance()->PrimaryTextColor());

    ui->btn_min->setIcon(QIcon("./appPics/svgs/theme/minSize.svg"));
    ui->btn_max->setIcon(QIcon("./appPics/svgs/theme/maxSize.svg"));
    ui->btn_exit->setIcon(QIcon("./appPics/svgs/theme/close.svg"));

    int moduleCount = ui->listWidget->count();
    for (int i = 0; i < moduleCount; i++) {
        QWidget *widget = ui->listWidget->itemWidget(ui->listWidget->item(i));
        if(widget){
            CustomListItem *customItem = qobject_cast<CustomListItem *>(widget);
            if(customItem){
                ModuleProperty property = widget->property(CUSTOMDATA).value<ModuleProperty>();
                customItem->icon()->setIcon(QIcon(property.icon));
            }
        }
    }
    int tabCount = ui->tabWidget->count();
    for (int i = 0; i < tabCount; i++) {
        QString text = ui->tabWidget->tabText(i);
        if(m_tabPropertys.contains(text)){
            ui->tabWidget->setTabIcon(i, QIcon(m_tabPropertys.value(text).icon));
        }

        QWidget *rightSide = ((QTabBar*)(ui->tabWidget->tabBar()))->tabButton(i, QTabBar::RightSide);
        if(rightSide){
            QPushButton *closeBtn = static_cast<QPushButton *>(rightSide);
            if(closeBtn){
                closeBtn->setIcon(QIcon("./appPics/svgs/theme/close.svg"));
            }
        }
    }

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
    QSize iconSize(GlobalSizes::instance()->DefaultIconHeight(), GlobalSizes::instance()->DefaultIconHeight());
    for (int i = 0; i < tabCount; i++)
    {
        ui->tabWidget->setIconSize(iconSize);
        if(ui->tabWidget->tabBar()->tabButton(i, QTabBar::RightSide) != nullptr){
            ui->tabWidget->tabBar()->tabButton(i, QTabBar::RightSide)->setFixedSize(iconSize);
        }
    }
    ui->tabWidget->update();

}

void MainWindow::loadAllTheme(){
    QList<ListItemProperty> themeItems;

    m_oldThemeName = "default";
    int oldThemeIndex = 0;
    QDir dir("./config/themes");
    if(dir.exists()){
        QFileInfoList fileList = dir.entryInfoList(QDir::Files);
        int fileCount = fileList.length();
        for (int i = 0; i < fileCount; i++) {
            QFileInfo fileInfo = fileList.at(i);
            QSettings *config = new QSettings(fileInfo.filePath(), QSettings::IniFormat);

            ListItemProperty property;
            QVariant themeName = config->value("themeName");
            if(themeName.isValid()){
                property.text = themeName.toString();

                if(m_oldThemeName == property.text){
                    oldThemeIndex = i;
                }

                QVariant themeIcon = config->value("themeIcon");
                if(themeIcon.isValid()){
                    property.icon = themeIcon.toString();
                }
                QVariant themeTooltip = config->value("themeTooltip");
                if(themeTooltip.isValid()){
                    property.toolTip = themeTooltip.toString();
                }
                property.customData = fileInfo.filePath();

                themeItems.append(property);
            }  
        }
    }

    if(themeItems.length() > 0){
        this->m_themeListWidget->setItems(themeItems);
        this->m_themeListWidget->getListWidget()->setCurrentRow(oldThemeIndex);

        //更新为当前选择的主题（默认主题）
        this->slot_themeSelectedItem(oldThemeIndex, themeItems.at(oldThemeIndex).customData);
    }
}

void MainWindow::loadAllPlugins(){
    QString path = qApp->applicationDirPath() + "/plugins";
    //设置插件的路径
    PluginManager::instance()->setPluginDir(path);
    //获取此路径下的所有插件的信息
    PluginManager::instance()->loadAllPlugins();

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

//        QWidget *widget = new QWidget;
//        auto hlay = new QHBoxLayout;
//        hlay->addWidget(button);
//        hlay->setContentsMargins(0,0,0,0);
//        widget->setLayout(hlay);
//        widget->setAttribute(Qt::WA_TransparentForMouseEvents);
//        widget->setAttribute(Qt::WA_TranslucentBackground, true);

        ui->listWidget->addItem(listItem);
        ui->listWidget->setItemWidget(listItem, button);

        if(plugInProperty.level == PlugInLevel::MustLoad){
            this->slot_listWidgetItemClicked(listItem);

            if(selectIndex < 0){
                selectIndex = i;
            }
        }
    }

    int commonLength = commonPlugIns.length();
    for (int i = 0; i < commonLength; i++) {
        PlugInProperty property = commonPlugIns.at(i);
        QAction *action = new QAction();
        action->setProperty(CUSTOMDATA, QVariant::fromValue(property));
        action->setText(property.displayName);
        if(!property.icon.isEmpty()){
            action->setIcon(QIcon(property.icon));
        }
        action->setToolTip(property.description);
        action->setCheckable(true);
        QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);
        if(loader != nullptr){
           action->setChecked(loader->isLoaded());
        }

        m_pluginMenu->addAction(action);
        m_pluginMenu->addSeparator();
    }

//    IPlugIn *homePage = new HomePage();
//    this->addTab(homePage);

    //加载首页模块
    if(selectIndex >= 0){
        ui->listWidget->setCurrentRow(selectIndex);
        ui->tabWidget->setCurrentIndex(selectIndex);
    }
}

void MainWindow::connectAllSignal(){
    connect(ui->btn_min, &QPushButton::clicked, this, &MainWindow::slot_minSize);
    connect(ui->btn_max, &QPushButton::clicked, this, &MainWindow::slot_maxSize);
    connect(ui->btn_exit, &QPushButton::clicked, this, &MainWindow::slot_exit);
    connect(ui->btn_theme, &QPushButton::clicked, this, &MainWindow::slot_theme);
    connect(ui->btn_language, &QPushButton::clicked, this, &MainWindow::slot_language);

    connect(this->m_themeListWidget, &PopupListWidget::selectedItem, this, &MainWindow::slot_themeSelectedItem);

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::slot_listWidgetItemClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slot_tabCurrentChanged);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::slot_tabCloseRequested);

    connect(frameworkTool::instance(), &frameworkTool::cssStyleChanged, this, &MainWindow::slot_cssStyleChanged);

    //检测屏幕分辨率的变化
    QDesktopWidget *desktopwidget = QApplication::desktop();
    connect(desktopwidget, SIGNAL(resized(int)), this, SLOT(slot_desktopwidgetResized()));
    connect(desktopwidget, SIGNAL(workAreaResized(int)), this, SLOT(slot_desktopwidgetWorkAreaResized()));
    connect(desktopwidget, SIGNAL(primaryScreenChanged()), this, SLOT(slot_desktopwidgetPrimaryScreenChanged()));

//    QList<QScreen *> listScreen = QGuiApplication::screens();
    QScreen *screen = QGuiApplication::primaryScreen();
//    foreach (QScreen *screen, listScreen)
    {
        connect(screen, &QScreen::availableGeometryChanged, this, &MainWindow::slot_availableGeometryChanged);
        connect(screen, &QScreen::physicalDotsPerInchChanged, this, &MainWindow::slot_physicalDotsPerInchChanged);
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::slot_logicalDotsPerInchChanged);
        connect(screen, &QScreen::virtualGeometryChanged, this, &MainWindow::slot_virtualGeometryChanged);
        connect(screen, &QScreen::physicalSizeChanged, this, &MainWindow::slot_physicalSizeChanged);
        connect(screen, &QScreen::primaryOrientationChanged, this, &MainWindow::slot_primaryOrientationChanged);
        connect(screen, &QScreen::orientationChanged, this, &MainWindow::slot_orientationChanged);
        connect(screen, &QScreen::refreshRateChanged, this, &MainWindow::slot_refreshRateChanged);
    }

}

void MainWindow::slot_availableGeometryChanged(const QRect &geometry){
    qDebug() << "Screen availableGeometryChanged:" << geometry;

    int scaleRate = 0;
    if(geometry.x() != 0){
        scaleRate = geometry.x() / GlobalSizes::Const_LeftTopX;
    }
    //屏幕分辩改变信号
    frameworkTool::resolutionChanged(scaleRate);
}

void MainWindow::slot_physicalSizeChanged(const QSizeF &size){
    qDebug() << "Screen physicalSizeChanged(width, height):" << size.width() << size.height();

    //屏幕分辩改变信号
    frameworkTool::resolutionChanged();
}

void MainWindow::slot_physicalDotsPerInchChanged(qreal dpi){
    qDebug() << "Screen physicalDotsPerInchChanged:" << dpi;

}

void MainWindow::slot_logicalDotsPerInchChanged(qreal dpi){
    qDebug() << "Screen logicalDotsPerInchChanged:" << dpi;

}

void MainWindow::slot_virtualGeometryChanged(const QRect &rect){
    qDebug() << "Screen virtualGeometryChanged:" << rect;

}

void MainWindow::slot_primaryOrientationChanged(Qt::ScreenOrientation orientation){
    qDebug() << "Screen primaryOrientationChanged:" << orientation;

}

void MainWindow::slot_orientationChanged(Qt::ScreenOrientation orientation){
    qDebug() << "Screen orientationChanged:" << orientation;

}

void MainWindow::slot_refreshRateChanged(qreal refreshRate){
    qDebug() << "Screen refreshRateChanged:" << refreshRate;

}

void MainWindow::slot_desktopwidgetResized(int value){
    qDebug() << "Screen Resized:" << value;

}

void MainWindow::slot_desktopwidgetWorkAreaResized(int value){
    qDebug() << "Screen WorkAreaResized:" << value;

}

void MainWindow::slot_desktopwidgetPrimaryScreenChanged(){
    qDebug() << "Screen PrimaryScreenChanged!";

}

double MainWindow::getTabBarWidth(QTabBar *tabBar, const QString &text){
    int pixelsWidth = tabBar->fontMetrics().width(text);
    return pixelsWidth;
}

void MainWindow::slot_minSize(bool checked){
    this->showMinimized();
}

void MainWindow::slot_maxSize(bool checked){
    if(this->windowState() == Qt::WindowMaximized){
        this->showNormal();
    }
    else{
        this->showMaximized();
    }
}

void MainWindow::slot_exit(bool checked){
    this->close();
}

void MainWindow::slot_theme(bool checked){
    if(this->m_themeListWidget->isVisible())
    {
        this->m_themeListWidget->hide();
    }
    else
    {
        QPoint mousePoint(ui->btn_theme->mapToGlobal(QPoint(0, 0)));
        this->m_themeListWidget->move(mousePoint.x(), mousePoint.y() + ui->btn_theme->height());
        this->m_themeListWidget->setVisible(true);
        this->m_themeListWidget->raise();
    }
}

void MainWindow::slot_language(bool checked){

}

void MainWindow::slot_themeSelectedItem(const int &index, const QVariant property){
    if(index >= 0 && property.isValid()){
        QString themePath = property.toString();

        //调用框架的方法进行更换主题
        frameworkTool::reLoadTheme(themePath);
    }
}

void MainWindow::addTab(IPlugIn *plugIn){
    PlugInProperty property = plugIn->getPlugInProperty();
    ModuleProperty moduleProperty = ModuleBase::convertFrom(property);

    if(moduleProperty.multipleInstance <= 0){
        return;
    }

    QWidget *widget = plugIn->getWidget();
    int index = ui->tabWidget->indexOf(widget);
    if(index < 0)
    {
        if(property.icon.isEmpty()){
            index = ui->tabWidget->addTab(widget, property.displayName);
        }
        else{
            index = ui->tabWidget->addTab(widget, QIcon(property.icon), property.displayName);
        }
    }
    else{
        if(moduleProperty.multipleInstance > 1){
            if(m_tabs.contains(property.name)){
                if(m_tabs.value(property.name) >= moduleProperty.multipleInstance){
                    if(m_notify != nullptr){
                        m_notify->sendData(QVariant::fromValue(NotifyStruct(NotifyLevel::WarnLevel, "警告", "功能模块【" + property.displayName + "】已加载了 " + QString::number(m_tabs.value(property.name)) + " 个，不能再加载新实例了！")));
                    }
                    return;
                }
            }

            int newIndex = -1;
            widget = plugIn->createNewPlugin()->getWidget();
            if(property.icon.isEmpty()){
                newIndex = ui->tabWidget->addTab(widget, property.displayName);
            }
            else{
                newIndex = ui->tabWidget->addTab(widget, QIcon(property.icon), property.displayName);
            }
            if(newIndex == index){
                if(m_notify != nullptr){
                    m_notify->sendData(QVariant::fromValue(NotifyStruct(NotifyLevel::ErrorLevel, "错误", "功能模块【" + property.displayName + "】加载异常！此功能可加载多个实例，但当前实例和已打开的此功能模块为同一实例。")));
                }
                ui->tabWidget->setCurrentIndex(index);
                return;
            }
            else{
                index = newIndex;
            }
        }
        else{
            if(m_notify != nullptr){
                m_notify->sendData(QVariant::fromValue(NotifyStruct(NotifyLevel::WarnLevel, "警告", "功能模块【" + property.displayName + "】重复加载！此功能仅可加载1个实例，已跳转到已打开的此功能模块。")));
            }
            ui->tabWidget->setCurrentIndex(index);
            return;
        }
    }
    ui->tabWidget->setCurrentIndex(index);
    ui->tabWidget->tabBar()->setProperty(CUSTOMDATA, QVariant::fromValue(property));

    //加上关闭按钮
    bool canClose = true;
    if(moduleProperty.multipleInstance > 0){
        canClose = moduleProperty.canClose;
        //此插件的内容不可关闭时
        if(!canClose){
            ((QTabBar*)(ui->tabWidget->tabBar()))->setTabButton(index, QTabBar::RightSide, NULL);
            m_alwaysOpenModules.append(widget);
        }
    }
    if(canClose){
//        QPushButton *closeBtn = new QPushButton();
//        closeBtn->setProperty("isIconNoPadding", "true");
//        closeBtn->setProperty(CUSTOMDATA, QVariant::fromValue(property));
//        closeBtn->setIcon(QIcon("./appPics/svgs/theme/close.svg"));
//        closeBtn->setProperty("index", index);
//        ((QTabBar*)(ui->tabWidget->tabBar()))->setTabButton(index,QTabBar::RightSide, closeBtn);
//        connect(closeBtn, &QPushButton::clicked, this, &MainWindow::slot_closeTab);

        foreach (QWidget *widget, m_alwaysOpenModules) {
            if(widget){
                int index = ui->tabWidget->indexOf(widget);
                ((QTabBar*)(ui->tabWidget->tabBar()))->setTabButton(index, QTabBar::RightSide, NULL);
            }
        }
    }

    if(!m_tabs.contains(property.name)){
        m_tabs.insert(property.name, 1);
    }
    else{
        m_tabs[property.name]++;
    }
    m_tabPropertys.insert(property.displayName, property);

    if(m_notify != nullptr){
        m_notify->sendData(QVariant::fromValue(NotifyStruct(NotifyLevel::SuccessLevel, "成功", "功能模块【" + property.displayName + "】加载成功！")));
    }

}

//tab的关闭按钮被点击的槽
void MainWindow::slot_tabCloseRequested(int index)
{
    QWidget *widget = ui->tabWidget->widget(index);
    if(widget){
        IPlugIn *plugin = qobject_cast<IPlugIn *>(widget);
        if(plugin){
            PlugInProperty property = plugin->getPlugInProperty();
            m_tabIsClosing = true;
            ui->tabWidget->removeTab(index);
            if(m_tabs.contains(property.name)){
                m_tabs[property.name]--;
            }
            delete widget;
            m_tabIsClosing = false;
        }
    }
}

void MainWindow::slot_tabCurrentChanged(int index){
    if(index >= 0){
        if(m_tabIsClosing){
            auto widget = ui->tabWidget->widget(index);
//            IUiModule *currentUI = dynamic_cast<IUiModule *>(widget);
//            if(currentUI && currentUI != nullptr){
//                ILogicModule *logicModule = currentUI->getLogicModule();
//                if(logicModule && logicModule != nullptr){
//                    ModuleProperty moduleInfo = logicModule->getModuleProperty();
//                    if(logicModule->getTaskObject()->getTaskType() == TaskType::E_ReplayTask){
//                        moduleInfo.tag = Globalstatisdata::instance()->getReplayStr();
//                    }
//                    this->slot_SelectedFunction(moduleInfo);
//                }
//            }
        }
        else{
//            this->m_controlBarWidget->getControl()->setCurrentItem(index);
        }
    }
}

void MainWindow::slot_listWidgetItemClicked(QListWidgetItem *item){
    QWidget *widget = ui->listWidget->itemWidget(item);
    if(widget != nullptr){
        ModuleProperty property = widget->property(CUSTOMDATA).value<ModuleProperty>();

        QString path = PluginManager::instance()->getPath(property.name);
        PluginManager::instance()->loadPlugin(path);
        QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);
        if(loader != nullptr){
            IPlugIn *plugin = qobject_cast<IPlugIn *>(loader->instance());
            if(plugin)
            {
                this->addTab(plugin);
            }
        }
    }
}

/**
 * @brief MainWindow::slot_cssStyleChanged 系统的css样式改变后，主界面的额外处理
 */
void MainWindow::slot_cssStyleChanged(){
    this->m_menuBar->setStyleSheet(frameworkTool::getAppCss());
    foreach (QMenu *menu, m_allMenus) {
        menu->setStyleSheet(frameworkTool::getAppCss());
    }

    this->initIcon();
}

void MainWindow::mousePressEvent(QMouseEvent *event)//判断鼠标点击时是否小于标题栏高度
{
    //判断鼠标在顶部区域内
    if(ui->widget_top->geometry().contains(event->pos()))
    {
        m_pressPoint = event->globalPos();//获取坐标
        m_isPressInDragArea = true;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)//鼠标移动，实时修改窗口的坐标
{
    //判断鼠标在顶部区域内
    if(m_isPressInDragArea)
    {
        int dx = event->globalX() - m_pressPoint.x();
        int dy = event->globalY() - m_pressPoint.y();
        m_pressPoint = event->globalPos();
        this->move(this->x() + dx, this->y() + dy);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)//鼠标释放
{
    m_isPressInDragArea = false;
}
