#include "frameworktool.h"
#include "acfproperty.h"

FrameworkTool* FrameworkTool::m_instance = nullptr;
QString FrameworkTool::frameworkTheme = ":/theme/qss/theme.css";
QStringList FrameworkTool::customCssList = QStringList();
QString FrameworkTool::appCss = "";
bool FrameworkTool::m_resolutionScaleable = false;

double FrameworkTool::m_currentPixWidth = 0;
double FrameworkTool::m_currentPixHeight = 0;

FrameworkTool::FrameworkTool(QObject *parent) : QObject(parent)
{
    //检测屏幕分辨率的变化
    QDesktopWidget *desktopwidget = QApplication::desktop();
    connect(desktopwidget, SIGNAL(resized(int)), this, SLOT(slot_desktopwidgetResized()));
    connect(desktopwidget, SIGNAL(workAreaResized(int)), this, SLOT(slot_desktopwidgetWorkAreaResized()));
    connect(desktopwidget, SIGNAL(primaryScreenChanged()), this, SLOT(slot_desktopwidgetPrimaryScreenChanged()));

    //    QList<QScreen *> listScreen = QGuiApplication::screens();
    QScreen *screen = QGuiApplication::primaryScreen();
    //    foreach (QScreen *screen, listScreen)
    {
        connect(screen, &QScreen::availableGeometryChanged, this, &FrameworkTool::slot_availableGeometryChanged);
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &FrameworkTool::slot_logicalDotsPerInchChanged);

//        connect(screen, &QScreen::physicalDotsPerInchChanged, this, &frameworkTool::slot_physicalDotsPerInchChanged);
//        connect(screen, &QScreen::virtualGeometryChanged, this, &frameworkTool::slot_virtualGeometryChanged);
//        connect(screen, &QScreen::physicalSizeChanged, this, &frameworkTool::slot_physicalSizeChanged);
//        connect(screen, &QScreen::primaryOrientationChanged, this, &frameworkTool::slot_primaryOrientationChanged);
//        connect(screen, &QScreen::orientationChanged, this, &frameworkTool::slot_orientationChanged);
//        connect(screen, &QScreen::refreshRateChanged, this, &frameworkTool::slot_refreshRateChanged);
    }
}

FrameworkTool::~FrameworkTool()
{

}

QStringList FrameworkTool::getCustomCssList()
{
    return customCssList;
}

void FrameworkTool::appendCustomCss(const QString &value)
{
    if(customCssList.contains(value)){
        customCssList.removeOne(value);
    }
    customCssList.append(value);
}

QString FrameworkTool::getAppCss()
{
    return appCss;
}

QString FrameworkTool::getCssStyle(){
    QString css = "";

    QStringList cssList;
    cssList.append(frameworkTheme);
    if(customCssList.length() > 0){
        cssList.append(customCssList);
    }

    auto style = FileHelper::readlFiles(cssList);
    if(!style.isEmpty())
    {
        css = QString(style);
        QList<QObject *> instances{GlobalColors::instance(), GlobalSizes::instance()};
        int length = instances.length();
        for (int i = 0; i < length; i++) {
            QObject *obj = instances.at(i);
            int propertyCount = obj->metaObject()->propertyCount();
            //propertyOffset返回类的属性开始的整数索引（假设超类可能有自己的属性）。如果您也希望获得超类的属性，您可以从0开始。
            int propertyOffset = obj->metaObject()->propertyOffset();
            for(int j = propertyOffset; j < propertyCount; j++){
                QMetaProperty metaProperty = obj->metaObject()->property(j);
                QString value = GlobalMethods::getShowValue(metaProperty.read(obj));
                css.replace("$" + QString(metaProperty.name()), value);
            }
        }
    }

    qDebug() << "获取的样式为: " << css;
    return css;
}

/**
* @brief reLoadCssStyle 重新加载系统的css样式
*/
void FrameworkTool::reLoadCssStyle(){
    QMainWindow *mainWindows = GlobalMethods::getMainWindow();
    if(mainWindows != nullptr){
        QString css = getCssStyle();
        if(!css.isEmpty()){
            appCss = css;
            qDebug() << "系统重新加载样式成功！Theme:" << appCss;

            //1.触发样式改变信号
            emit instance()->cssStyleChanged();

            //2.通知所有插件，css样式发生了改变
            QList<PlugInProperty> plugIns = PluginManager::instance()->allPluginsProperty();
            int plugCount = plugIns.length();
            for (int i = 0; i < plugCount; i++) {
                PlugInProperty property = plugIns.at(i);
                QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);
                if(loader && loader->isLoaded()){
                    IPlugIn *plugin = qobject_cast<IPlugIn *>(loader->instance());
                    if(plugin){
                        //给每一个加载了的插件发送主题改变通知
                        plugin->cssStyleChanged();
                    }
                }
            }

            //3.更新主程序的样式
            mainWindows->setStyleSheet(appCss);

            //4.最后，更新界面
            mainWindows->repaint();
        }
    }
}

/**
* @brief reLoadTheme 重新加载系统的主题（框架+自定义css）
* @param themePath
*/
void FrameworkTool::reLoadTheme(const QString &themePath){
    if(!themePath.isEmpty()){
        if(QFile::exists(themePath)){
            GlobalColors *colors = GlobalColors::instance();
            QSettings *config = new QSettings(themePath, QSettings::IniFormat);
            QStringList keys = config->allKeys();
            int count = keys.length();
            for (int i = 0; i < count; i++) {
                QVariant value = colors->property(keys.at(i).toLatin1());
                if(value.isValid()){
                    colors->setProperty(keys.at(i).toLatin1(), config->value(keys.at(i)));
                }
            }

            //更新svg中的颜色
            //QApplication::applicationDirPath()
            SvgHelper::updateAllSvg("./appPics/svgs/", colors->PrimaryTextColor(), colors->PrimaryColor());

            //更新qss样式
            reLoadCssStyle();
        }
    }
}

/**
* @brief initResolution 此分辨率下，初始化元素的相对大小
*/
void FrameworkTool::initResolution(){
    bool isEnableHighDpiScaling = QGuiApplication::testAttribute(Qt::AA_EnableHighDpiScaling);
    if(isEnableHighDpiScaling){
        QString info = "本程序设置了AA_EnableHighDpiScaling，故框架自带的分辨率自适应功能将不会启用！";
        ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, info.toStdString())));
        return;
    }

    //注意：当开启AA_EnableHighDpiScaling时，获取的availableGeometry、QDesktopWidget都是缩放后的分辨率（如实际分辨率是2560*1600，缩放比例200%，此时获取到的分辨率是1280*800）
    //且logicalDotsPerInch恒为96，即获取不到缩放比例

    //最终的缩放比例：计算分辨率的缩放比例 * 放大比例
    double rate = 1;

    QScreen *screen = QGuiApplication::primaryScreen();
    m_currentPixWidth = screen->availableGeometry().width();
    m_currentPixHeight = screen->availableGeometry().height();

    //1、计算分辨率的缩放比例：当前的屏幕宽度/标准宽度
    if(m_resolutionScaleable){
//        QDesktopWidget *desktop = QApplication::desktop();
//        m_currentPixWidth = desktop->width();
//        m_currentPixHeight = desktop->height();

        double widthRate = 0, heigthRate = 0;
        widthRate = m_currentPixWidth / GlobalSizes::Const_PixWidth;
        heigthRate = m_currentPixHeight / GlobalSizes::Const_PixHeight;
        if(widthRate != 0){
            rate = qMin(widthRate, heigthRate);
        }
    }

    //2、在ubuntu系统中，根据左上角的X位置获取缩放比例
    int scale = 0;
    if(screen->geometry().x() != 0){
        scale = screen->geometry().x() / GlobalSizes::Const_LeftTopX;
    }
    if(scale != 0){
        rate = rate * scale;//乘以分辨率的变化因子
    }

    //3、获取缩放比例的因子
    qreal dpiScaleFactor = getDPIScaleFactor();
    if(dpiScaleFactor != 0){
        rate = rate * dpiScaleFactor;//乘以缩放比例的因子
    }

    if(rate != 0){
        //按理应该乘以多少倍，但实际情况来看，要小一点好一点，这个乘以个系数，这个系数最好可在配置文件中配置
//        if(rate > 1){
//            rate = rate * 1.5;
//        }
        GlobalSizes::instance()->updateSize(rate);
        reLoadCssStyle();

        QString info = "检测到系统分辨率更改，设计时分辨率：" + QString::number(GlobalSizes::Const_PixWidth) + " X " + QString::number(GlobalSizes::Const_PixHeight) + "，设计时控件高度："+ QString::number(GlobalSizes::Const_DefaultControlHeight);
        info.append("；此时分辨率：" + QString::number(m_currentPixWidth) + " X " + QString::number(m_currentPixHeight) + "，此时控件高度：" + QString::number(GlobalSizes::instance()->DefaultControlHeight()) + "；此时的相对系数：" + QString::number(rate));
        ACFProperty::instance()->getLogPlugIn()->sendData(QVariant::fromValue(LoggerBaseStruct(LoggerBaseLevel::DEBUG_LogLevel, info.toStdString())));
    }
}

//availableGeometryChanged 这个是可视分辨率变化
void FrameworkTool::slot_availableGeometryChanged(const QRect &geometry){
    qDebug() << "Screen availableGeometryChanged:" << geometry;

    //屏幕分辩改变信号
    FrameworkTool::initResolution();
}

//这个是dpi变化
void FrameworkTool::slot_logicalDotsPerInchChanged(qreal dpi){
    qDebug() << "Screen logicalDotsPerInchChanged:" << dpi;

    //屏幕分辩改变信号
    FrameworkTool::initResolution();
}

//void frameworkTool::slot_physicalSizeChanged(const QSizeF &size){
//    qDebug() << "Screen physicalSizeChanged(width, height):" << size.width() << size.height();

//    //屏幕分辩改变信号
//    frameworkTool::resolutionChanged();
//}

//void frameworkTool::slot_physicalDotsPerInchChanged(qreal dpi){
//    qDebug() << "Screen physicalDotsPerInchChanged:" << dpi;

//}

//void frameworkTool::slot_virtualGeometryChanged(const QRect &rect){
//    qDebug() << "Screen virtualGeometryChanged:" << rect;

//}

//void frameworkTool::slot_primaryOrientationChanged(Qt::ScreenOrientation orientation){
//    qDebug() << "Screen primaryOrientationChanged:" << orientation;

//}

//void frameworkTool::slot_orientationChanged(Qt::ScreenOrientation orientation){
//    qDebug() << "Screen orientationChanged:" << orientation;

//}

//void frameworkTool::slot_refreshRateChanged(qreal refreshRate){
//    qDebug() << "Screen refreshRateChanged:" << refreshRate;

//}

//void frameworkTool::slot_desktopwidgetResized(int value){
//    qDebug() << "Screen Resized:" << value;

//}

//void frameworkTool::slot_desktopwidgetWorkAreaResized(int value){
//    qDebug() << "Screen WorkAreaResized:" << value;

//}

//void frameworkTool::slot_desktopwidgetPrimaryScreenChanged(){
//    qDebug() << "Screen PrimaryScreenChanged!";

//}
