#include "aboutDialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <QUrl>
#include <frameworktool.h>
#include <globalEnums.h>
#include <qdesktopservices.h>
#include <acfproperty.h>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /** 设置主窗体的属性
     *
    **/
    setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon("./appPics/icons/rabbit.ico"));

    //窗体首次加载时，居中显示
    QScreen *screen = QGuiApplication::primaryScreen();
    double widthTemp = screen->geometry().width() * 0.6;
    double heightTemp = screen->geometry().height() * 0.6;
    double xTemp = (screen->geometry().width() - widthTemp) / 2.0;
    double yTemp = (screen->geometry().height() - heightTemp) / 2.0;
    this->setGeometry(xTemp, yTemp, widthTemp, heightTemp);

    this->setMinimumSize(screen->geometry().width() * 0.4, screen->geometry().height() * 0.4);

    QCoreApplication::processEvents();

    //初始化顶部
    this->initWidgetTop();

    //初始化鼠标改变窗体大小的区域
    initResizeArea();

    //插件的额外处理（如进行关联）
    this->loadAllPlugins();

    QCoreApplication::processEvents();

    m_mainDockWindow = new MainDockWindow();
    ui->main_centerLayout->addWidget(m_mainDockWindow, 1);

    QCoreApplication::processEvents();

    //获取有哪些主题样式
    this->loadAllTheme();

    //建立所有信号和槽
    this->connectAllSignal();

    qDebug() << "测试消息，用于验证日志插件是否捕获到了qDebug()的输出！";
    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::INFO_LogLevel, "测试信息！！！今天的天气真好，真象出去玩一玩！")));

    QCoreApplication::processEvents();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidgetTop(){
    this->initMenu();

    this->initShortcutKey();

    this->initReSizeArea();
}

void MainWindow::initResizeArea(){
//    setMouseTracking(true);
    // 创建QSizeGrip对象，该对象就是用来调整窗体大小的
//    m_pSizeGrip = new QSizeGrip(this);
//    m_pSizeGrip->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    m_pSizeGrip->setCursor(Qt::SizeAllCursor);
//    m_pSizeGrip->setToolTip("改变窗口大小");
//    ui->main_bottom->layout()->addWidget(m_pSizeGrip);// 将QSizeGrip对象加入窗体右下角

    // 也可以加入一个状态栏来实现拖动窗体右下角改变窗体大小
    m_pStatusBar = new QStatusBar();
    m_pStatusBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_pStatusBar->setToolTip("改变窗口大小");
    m_pStatusBar->setSizeGripEnabled(true);
    ui->main_bottom->layout()->addWidget(m_pStatusBar);
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

void MainWindow::initShortcutKey(){
    QStringList shortcutKeys {"启动任务", "暂停任务", "停止任务", "截图", "恢复布局", "导出报表"};
    foreach (QString shortcutKey, shortcutKeys) {
        QToolButton *toolBtn = new QToolButton();
        toolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        toolBtn->setIcon(QIcon("./appPics/svgs/theme/close.svg"));
        toolBtn->setText(shortcutKey);
        toolBtn->setCheckable(true);

        ui->horizontalLayout_shortcutKey->addWidget(toolBtn);

        if(shortcutKey == "停止任务"){
            QFrame *vframe = new QFrame();
            vframe->setFrameShape(QFrame::VLine);      // 设置垂直方向
            ui->horizontalLayout_shortcutKey->addWidget(vframe);
        }
    }
}

void MainWindow::slots_menuTriggered(QAction *action){
    QVariant customData = action->property(CUSTOMDATA);
    if(customData.isValid()){
        PlugInProperty property = action->property(CUSTOMDATA).value<PlugInProperty>();
//        QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);

//        if(loader == nullptr){
//            return;
//        }
        if(!action->isChecked())
        {
            IPlugIn *plugin = PluginManager::instance()->getIPlugin(property.name);
            if(plugin)
            {
                plugin->disModule();
            }
            PluginManager::instance()->unloadPlugin(PluginManager::instance()->getPath(property.name));

//            if(loader->isLoaded()){
//                loader->unload();
//            }
        }
        else{
            PluginManager::instance()->loadPlugin(PluginManager::instance()->getPath(property.name));
            IPlugIn *plugin = PluginManager::instance()->getIPlugin(property.name);
            if(plugin)
            {
                plugin->initModule();
            }
//            if(!loader->isLoaded()){
//                if(loader->load()){
//                    IPlugIn *plugin = qobject_cast<IPlugIn *>(loader->instance());
//                    if(plugin)
//                    {
//                        plugin->initModule();
//                    }
//                }
//            }
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

    //TODO

}

void MainWindow::loadAllPlugins(){
//    QString path = qApp->applicationDirPath() + "/plugins";
//    //设置插件的路径
//    PluginManager::instance()->setPluginDir(path);
//    //获取此路径下的所有插件的信息
//    PluginManager::instance()->loadAllPlugins();

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
}

void MainWindow::loadAllTheme(){
    //设置框架外的自定义样式
    FrameworkTool::appendCustomCss(":/qss/customTheme.css");

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

    slot_cssStyleChanged();
}

void MainWindow::connectAllSignal(){
    connect(ui->btn_min, &QPushButton::clicked, this, &MainWindow::slot_minSize);
    connect(ui->btn_max, &QPushButton::clicked, this, &MainWindow::slot_maxSize);
    connect(ui->btn_exit, &QPushButton::clicked, this, &MainWindow::slot_exit);
    connect(ui->btn_theme, &QPushButton::clicked, this, &MainWindow::slot_theme);
    connect(ui->btn_language, &QPushButton::clicked, this, &MainWindow::slot_language);

    connect(this->m_themeListWidget, &PopupListWidget::selectedItem, this, &MainWindow::slot_themeSelectedItem);

    connect(FrameworkTool::instance(), &FrameworkTool::cssStyleChanged, this, &MainWindow::slot_cssStyleChanged);

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
        //此控件默认未显示出来，此时设置它的样式不生效，故在第一次打开时设置其样式
        if(m_isFirstOpenThemePopup){
            if(this->m_themeListWidget){
                this->m_themeListWidget->setStyleSheet(FrameworkTool::getAppCss());
                this->m_themeListWidget->getListWidget()->setStyleSheet(FrameworkTool::getAppCss());

                m_isFirstOpenThemePopup = false;
            }
        }
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
        FrameworkTool::reLoadTheme(themePath);
    }
}

/**
 * @brief MainWindow::slot_cssStyleChanged 系统的css样式改变后，主界面的额外处理
 */
void MainWindow::slot_cssStyleChanged(){
    if(m_menuBar)
        this->m_menuBar->setStyleSheet(FrameworkTool::getAppCss());
    if(this->m_themeListWidget){
        this->m_themeListWidget->setStyleSheet(FrameworkTool::getAppCss());
        this->m_themeListWidget->getListWidget()->setStyleSheet(FrameworkTool::getAppCss());
    }
    if(this->m_languageListWidget){
        this->m_languageListWidget->setStyleSheet(FrameworkTool::getAppCss());
        this->m_languageListWidget->getListWidget()->setStyleSheet(FrameworkTool::getAppCss());
    }

    foreach (QMenu *menu, m_allMenus) {
        menu->setStyleSheet(FrameworkTool::getAppCss());
    }

    initIcon();

    MainWindow::setIconSize(QSize(GlobalSizes::instance()->DefaultIconHeight(), GlobalSizes::instance()->DefaultIconHeight()));
}

void MainWindow::mousePressEvent(QMouseEvent *event)//判断鼠标点击时是否小于标题栏高度
{
    //判断鼠标在顶部区域内
    if(ui->main_top->geometry().contains(event->pos()))
    {
        m_pressPoint = event->globalPos();//获取坐标
        m_isPressInDragArea = true;
    }
//    else if(m_pSizeGrip != nullptr && m_pSizeGrip->geometry().contains(event->pos())){
//        m_lastPoint = event->globalPos();
//        m_bLeftMousePressed = true;
//    }
//    else if(m_pStatusBar != nullptr && m_pStatusBar->geometry().contains(event->pos())){
//        m_lastPoint = event->globalPos();
//        m_bLeftMousePressed = true;
//    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)//鼠标移动，实时修改窗口的坐标
{
//    int minWidth = this->minimumWidth();
    //判断鼠标在顶部区域内
    if(m_isPressInDragArea)
    {
        int dx = event->globalX() - m_pressPoint.x();
        int dy = event->globalY() - m_pressPoint.y();
        m_pressPoint = event->globalPos();
        this->move(this->x() + dx, this->y() + dy);
    }

//    if(m_bLeftMousePressed){
//        auto curMousePt = event->globalPos(); // 注意：这里记录是窗体在屏幕上的坐标位置
//        auto offsetPt = curMousePt - m_lastPoint; // 计算和上次移动点的坐标差值
//        move(pos() + offsetPt);
//        m_lastPoint = curMousePt;  // 记录本次窗体所在位置，以便下次计算位置
//    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)//鼠标释放
{
    m_isPressInDragArea = false;
//    m_bLeftMousePressed = false;
}

