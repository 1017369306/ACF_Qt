//解决中文乱码
#pragma execution_character_set("utf-8")

#ifndef FRAMEWORKTOOL_H
#define FRAMEWORKTOOL_H

#include "mainSystemLib_global.h"
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
#include <globalEnums.h>

/**
 * @brief 框架工具类，提供了框架相关的属性，函数
 */
class MAINSYSTEMLIB_EXPORT FrameworkTool : public QObject
{
    Q_OBJECT

public:
    explicit FrameworkTool(QObject *parent = nullptr);
    ~FrameworkTool();

    static FrameworkTool *instance(){
        if(m_instance == nullptr)
            m_instance = new FrameworkTool();
        return m_instance;
    }

//信号
public:
Q_SIGNALS:

    /**
     * @brief 样式改变信号
     */
    void cssStyleChanged();

    /**
     * @brief 系统分辨率改变信号
     */
    void geometryChanged();

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
    static void appendCustomCss(const QString &value);

    /**
     * @brief getAppCss 整个系统的css样式
     * @return
     */
    static QString getAppCss();

    /**
     * @brief 获取是否启用分辨率因子（启用时，控件、字体大小在基础上乘以此因子，如：设计时的标准为1280 x 1024，控件默认高度为36，
     * 此时的分辨率为2560 x 1600，则此因子值为1600 / 1024 * 36 = 56.25），默认false
     */
    static bool getResolutionScaleable() {return m_resolutionScaleable;}
    /**
     * @brief 设置是否启用分辨率因子（启用时，控件、字体大小在基础上乘以此因子，如：设计时的标准为1280 x 1024，控件默认高度为36，
     * 此时的分辨率为2560 x 1600，则此因子值为1600 / 1024 * 36 = 56.25），默认false
     * @param value
     */
    static void setResolutionScaleable(const bool& value) {m_resolutionScaleable = value;}

    /**
     * @brief 获取DPI缩放比例（当开启了AA_EnableHighDpiScaling时，获取的DPI缩放比例都是1）
     * @return
     */
    static double getDPIScaleFactor()
    {
        double dpiScaleFactor = QGuiApplication::primaryScreen()->logicalDotsPerInch() / 96.0;
        return dpiScaleFactor;
    }

public:

    static QString getCssStyle();

    /**
     * @brief reLoadCssStyle 重新加载系统的css样式
     */
    static void reLoadCssStyle();

    /**
     * @brief reLoadTheme 重新加载系统的主题（框架+自定义css）
     * @param themePath
     */
    static void reLoadTheme(const QString &themePath);

    /**
     * @brief initResolution 此分辨率下，初始化元素的相对大小
     */
    static void initResolution();

private slots:

    //屏幕分辩改变信号
    void slot_availableGeometryChanged(const QRect &geometry);
    void slot_logicalDotsPerInchChanged(qreal dpi);

//    void slot_physicalSizeChanged(const QSizeF &size);
//    void slot_physicalDotsPerInchChanged(qreal dpi);
//    void slot_virtualGeometryChanged(const QRect &rect);
//    void slot_primaryOrientationChanged(Qt::ScreenOrientation orientation);
//    void slot_orientationChanged(Qt::ScreenOrientation orientation);
//    void slot_refreshRateChanged(qreal refreshRate);

//    void slot_desktopwidgetResized(int value);
//    void slot_desktopwidgetWorkAreaResized(int value);
//    void slot_desktopwidgetPrimaryScreenChanged();

private:

    static FrameworkTool *m_instance;

    /**
     * @brief m_currentPixWidth 当前的像素宽度
     */
    static double m_currentPixWidth;
    /**
     * @brief m_currentPixHeight 当前的像素高度
     */
    static double m_currentPixHeight;

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

    /**
     * @brief 是否启用分辨率因子（启用时，控件、字体大小在基础上乘以此因子，如：设计时的标准为1280 x 1024，控件默认高度为36，
     * 此时的分辨率为2560 x 1600，则此因子值为1600 / 1024 * 36 = 56.25），默认false
     */
    static bool m_resolutionScaleable;

};

#endif // FRAMEWORKTOOL_H
