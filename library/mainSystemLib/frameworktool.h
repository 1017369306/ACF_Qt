#ifndef FRAMEWORKTOOL_H
#define FRAMEWORKTOOL_H

#include <QObject>
#include "globalhelper.h"
#include "globalhelper.h"
#include "pluginManager.h"
#include <QStringList>
#include <globalColors.h>
#include <QMetaProperty>
#include <QSettings>
#include <globalSizes.h>
#include <QMainWindow>
#include <QGuiApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QApplication>

/**
 * @brief The frameworkTool class 框架工具类，提供了框架相关的属性，函数
 */
class frameworkTool : public QObject
{
    Q_OBJECT

public:
    explicit frameworkTool(QObject *parent = nullptr);
    ~frameworkTool();

    static frameworkTool *instance(){
        if(m_instance == nullptr)
            m_instance = new frameworkTool();
        return m_instance;
    }

//信号
public:
Q_SIGNALS:

    /**
     * @brief cssStyleChanged 样式改变信号
     */
    void cssStyleChanged();

public:

    /**
     * @brief getFrameworkTheme 框架主题文件
     * @return
     */
    static QString getFrameworkTheme() { return frameworkTheme;}
    static void setFrameworkTheme(const QString &value) { frameworkTheme = value;}

    /**
     * @brief getCustomCssList 框架以外，自定义的资源文件
     * @return
     */
    static QStringList getCustomCssList();
    static void setCustomCssList(const QStringList &value);

    /**
     * @brief getAppCss 整个系统的css样式
     * @return
     */
    static QString getAppCss();

private:

    /**
     * @brief frameworkTheme 框架主题文件
     */
    static QString frameworkTheme;

    /**
     * @brief customCssList 框架以外，自定义的资源文件
     */
    static QStringList customCssList;

    /**
     * @brief appCss 整个系统的css样式
     */
    static QString appCss;

public:

    /**
     * @brief reLoadCssStyle 重新加载系统的css样式
     */
    static void reLoadCssStyle(){
        QMainWindow *mainWindows = GlobalMethods::getMainWindow();
        if(mainWindows != nullptr){
            QStringList cssList;
            cssList.append(frameworkTheme);
            if(customCssList.length() > 0){
                cssList.append(customCssList);
            }

            auto style = FileHelper::readlFiles(cssList);
            if(!style.isEmpty())
            {
                QString css(style);
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
     * @brief reLoadTheme 重新整个系统的主题（框架+自定义css）
     * @param themePath
     */
    static void reLoadTheme(const QString &themePath){
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
    static void initResolution(){
        int scaleRate = 0;
        QScreen *screen = QGuiApplication::primaryScreen();
        if(screen->geometry().x() != 0){
            scaleRate = screen->geometry().x() / GlobalSizes::Const_LeftTopX;
        }
        resolutionChanged(scaleRate);
    }

    /**
     * @brief resolutionChanged 屏幕分辩改变信号
     * @param scale 缩放比例
     */
    static void resolutionChanged(int scale = 0){
        QDesktopWidget *desktop = QApplication::desktop();
        double rate = 1;
        double widthRate = 0, heigthRate = 0;
        //计算分辨率的缩放比例：当前的屏幕宽度/标准宽度
        if(m_currentPixWidth != desktop->width()){
            m_currentPixWidth = desktop->width();

            widthRate = m_currentPixWidth / GlobalSizes::Const_PixWidth;
        }
        if(m_currentPixHeight != desktop->height()){
            m_currentPixHeight = desktop->height();

            heigthRate = m_currentPixHeight / GlobalSizes::Const_PixHeight;
        }
        if(widthRate != 0){
            rate = qMin(widthRate, heigthRate);
        }

        //最终缩放比例为：计算分辨率的缩放比例 * 放大比例
        if(scale != 0){
            rate = rate * scale;
        }
        if(rate != 0){
            //按理应该乘以多少倍，但实际情况来看，要小一点好一点，这个乘以个系数，这个系数最好可在配置文件中配置
            if(rate > 1){
                rate = rate * 0.75;
            }
            GlobalSizes::instance()->updateSize(rate);
            reLoadCssStyle();

        }
    }

private:

    /**
     * @brief m_currentPixWidth 当前的像素宽度
     */
    static double m_currentPixWidth;
    /**
     * @brief m_currentPixHeight 当前的像素高度
     */
    static double m_currentPixHeight;

    static frameworkTool *m_instance;

};

#endif // FRAMEWORKTOOL_H
