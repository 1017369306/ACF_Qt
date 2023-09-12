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
    setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon("./appPics/icons/rabbit.ico"));

    //初始化顶部
    this->initWidgetTop();

    //先初始化框架功级参数类单例
    ACFProperty::instance();

    //初始化所有插件
    this->loadAllPlugins();

    m_mainDockWindow = new MainDockWindow();
    ui->main_centerLayout->addWidget(m_mainDockWindow, 1);

    //此分辨率下，初始化元素的相对大小
    frameworkTool::initResolution();

    //获取有哪些主题样式
    this->loadAllTheme();

    //建立所有信号和槽
    this->connectAllSignal();

    ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::INFO_LogLevel, "测试信息！！！今天的天气真好，真象出去玩一玩！")));

    //窗体首次加载时，居中显示
    QScreen *screen = QGuiApplication::primaryScreen();
    double widthTemp = screen->geometry().width() * 0.6;
    double heightTemp = screen->geometry().height() * 0.6;
    double xTemp = (screen->geometry().width() - widthTemp) / 2.0;
    double yTemp = (screen->geometry().height() - heightTemp) / 2.0;
    this->setGeometry(xTemp, yTemp, widthTemp, heightTemp);
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
    frameworkTool::appendCustomCss(":/qss/customTheme.css");

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

    connect(frameworkTool::instance(), &frameworkTool::cssStyleChanged, this, &MainWindow::slot_cssStyleChanged);

//    //检测屏幕分辨率的变化
//    QDesktopWidget *desktopwidget = QApplication::desktop();
//    connect(desktopwidget, SIGNAL(resized(int)), this, SLOT(slot_desktopwidgetResized()));
//    connect(desktopwidget, SIGNAL(workAreaResized(int)), this, SLOT(slot_desktopwidgetWorkAreaResized()));
//    connect(desktopwidget, SIGNAL(primaryScreenChanged()), this, SLOT(slot_desktopwidgetPrimaryScreenChanged()));

//    //    QList<QScreen *> listScreen = QGuiApplication::screens();
//    QScreen *screen = QGuiApplication::primaryScreen();
//    //    foreach (QScreen *screen, listScreen)
//    {
//        connect(screen, &QScreen::availableGeometryChanged, this, &MainWindow::slot_availableGeometryChanged);
//        connect(screen, &QScreen::physicalDotsPerInchChanged, this, &MainWindow::slot_physicalDotsPerInchChanged);
//        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::slot_logicalDotsPerInchChanged);
//        connect(screen, &QScreen::virtualGeometryChanged, this, &MainWindow::slot_virtualGeometryChanged);
//        connect(screen, &QScreen::physicalSizeChanged, this, &MainWindow::slot_physicalSizeChanged);
//        connect(screen, &QScreen::primaryOrientationChanged, this, &MainWindow::slot_primaryOrientationChanged);
//        connect(screen, &QScreen::orientationChanged, this, &MainWindow::slot_orientationChanged);
//        connect(screen, &QScreen::refreshRateChanged, this, &MainWindow::slot_refreshRateChanged);
//    }

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
                this->m_themeListWidget->setStyleSheet(frameworkTool::getAppCss());
                this->m_themeListWidget->getListWidget()->setStyleSheet(frameworkTool::getAppCss());

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
        frameworkTool::reLoadTheme(themePath);
    }
}

/**
 * @brief MainWindow::slot_cssStyleChanged 系统的css样式改变后，主界面的额外处理
 */
void MainWindow::slot_cssStyleChanged(){
    if(m_menuBar)
        this->m_menuBar->setStyleSheet(frameworkTool::getAppCss());
    if(this->m_themeListWidget){
        this->m_themeListWidget->setStyleSheet(frameworkTool::getAppCss());
        this->m_themeListWidget->getListWidget()->setStyleSheet(frameworkTool::getAppCss());
    }
    if(this->m_languageListWidget){
        this->m_languageListWidget->setStyleSheet(frameworkTool::getAppCss());
        this->m_languageListWidget->getListWidget()->setStyleSheet(frameworkTool::getAppCss());
    }

    foreach (QMenu *menu, m_allMenus) {
        menu->setStyleSheet(frameworkTool::getAppCss());
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

