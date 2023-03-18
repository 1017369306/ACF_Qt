#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <popuplistwidget.h>
#include <IPlugIn.h>
#include <QMouseEvent>
#include <customlistitem.h>

#define VERSION "V 1.0.0.1"
static const int CODE_RESTART = 773;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief The ActionProperty struct 菜单项的属性
 */
struct ActionProperty
{
    QString text;
    QString icon;
    QString tooltip;
    bool checkable;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    //鼠标拖动窗体
    //鼠标按下
    void mousePressEvent(QMouseEvent* event);
    //鼠标移动
    void mouseMoveEvent(QMouseEvent* event);
    //鼠标释放
    void mouseReleaseEvent(QMouseEvent* event);

private slots:

    //屏幕分辩改变信号
    void slot_availableGeometryChanged(const QRect &geometry);
    void slot_physicalSizeChanged(const QSizeF &size);
    void slot_physicalDotsPerInchChanged(qreal dpi);
    void slot_logicalDotsPerInchChanged(qreal dpi);
    void slot_virtualGeometryChanged(const QRect &rect);
    void slot_primaryOrientationChanged(Qt::ScreenOrientation orientation);
    void slot_orientationChanged(Qt::ScreenOrientation orientation);
    void slot_refreshRateChanged(qreal refreshRate);

    void slot_desktopwidgetResized(int value);
    void slot_desktopwidgetWorkAreaResized(int value);
    void slot_desktopwidgetPrimaryScreenChanged();

    void slot_minSize(bool checked = false);

    void slot_maxSize(bool checked = false);

    void slot_exit(bool checked = false);

    void slot_theme(bool checked = false);

    void slot_language(bool checked = false);

    void slot_themeSelectedItem(const int &index, const QVariant property);

//    void slot_closeTab();

    void slot_tabCloseRequested(int index);

    void slot_tabCurrentChanged(int index);

    void slot_listWidgetItemClicked(QListWidgetItem *item);

    void slots_menuTriggered(QAction *action);

private:

    /**
     * @brief initResolution 此分辨率下，初始化元素的相对大小
     */
    void initResolution();

    /**
     * @brief loadAllTheme 获取有哪些主题样式
     */
    void loadAllTheme();

    /**
     * @brief loadAllPlugins 加载指定目录下的所有插件
     */
    void loadAllPlugins();

    /**
     * @brief connectAllSignal 建立所有信号和槽
     */
    void connectAllSignal();

    /**
     * @brief initWidgetTop 初始化顶部
     */
    void initWidgetTop();

    /**
     * @brief initMenu 初始化菜单栏
     */
    void initMenu();

    /**
     * @brief initModules 初始化功能列表
     */
    void initModules();

    /**
     * @brief initReSizeArea 初始化右上角改变大小区域
     */
    void initReSizeArea();

    /**
     * @brief initIcon 重新设置所以的icon
     */
    void initIcon();

    void reLoadCssStyle();

    /**
     * @brief addTab 新增tab页
     * @param plugIn 插件信息
     */
    void addTab(IPlugIn *plugIn);

    /**
     * @brief resolutionChanged 屏幕分辩改变信号
     * @param scale 缩放比例
     */
    void resolutionChanged(int scale = 0);

    /**
     * @brief getTabBarWidth 获取此选项卡的宽度
     * @param tabBar
     * @param text
     * @return
     */
    double getTabBarWidth(QTabBar *tabBar, const QString &text);

private:
    Ui::MainWindow *ui;

    PopupListWidget *m_themeListWidget = nullptr;
    PopupListWidget *m_languageListWidget = nullptr;

    double m_currentPixWidth;
    double m_currentPixHeight;

    //tab是否正在关闭中
    bool m_tabIsClosing = false;

    QMenu *m_pluginMenu = nullptr;

    QString m_oldThemeName;

    QPoint m_pressPoint;
    bool m_isPressInDragArea = false;
    QHash<QString, PlugInProperty> m_tabPropertys;
    QHash<QString, int> m_tabs;

    QList<QWidget *> m_alwaysOpenModules;

    QMenuBar *m_menuBar = nullptr;

//    QMenu *m_systemMenu = nullptr;
//    QMenu *m_codeMenu = nullptr;
//    QMenu *m_aboutMenu = nullptr;
    QList<QMenu *> m_allMenus;

    //仅仅在第一次手动改变缩放比例时，才设置tabwidget的tabbar为自适应
//    int m_changeRateCount = 0;

};
#endif // MAINWINDOW_H
