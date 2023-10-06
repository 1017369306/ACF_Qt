#ifndef MainWindow_H
#define MainWindow_H

#include "popuplistwidget.h"

#include <IPlugIn.h>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSizeGrip>
#include "maindockwindow.h"

#define VERSION "V 1.0.0.1"

static const int CODE_RESTART = 773;

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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
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

    void slot_minSize(bool checked = false);

    void slot_maxSize(bool checked = false);

    void slot_exit(bool checked = false);

    void slot_theme(bool checked = false);

    void slot_language(bool checked = false);

    void slot_themeSelectedItem(const int &index, const QVariant property);

    void slots_menuTriggered(QAction *action);

    void slot_cssStyleChanged();

private:

    /**
     * @brief loadAllPlugins 加载指定目录下的所有插件
     */
    void loadAllPlugins();

    /**
     * @brief loadAllTheme 获取有哪些主题样式
     */
    void loadAllTheme();

    /**
     * @brief connectAllSignal 建立所有信号和槽
     */
    void connectAllSignal();

    /**
     * @brief initWidgetTop 初始化顶部
     */
    void initWidgetTop();

    /**
     * @brief 初始化鼠标改变窗体大小的区域
     */
    void initResizeArea();

    /**
     * @brief 初始化快捷键区域
     */
    void initShortcutKey();

    /**
     * @brief initMenu 初始化菜单栏
     */
    void initMenu();

    /**
     * @brief initReSizeArea 初始化右上角改变大小区域
     */
    void initReSizeArea();

    /**
     * @brief initIcon 重新设置所以的icon
     */
    void initIcon();


private:
    Ui::MainWindow *ui;

    bool m_isFirstOpenThemePopup = true;
    PopupListWidget *m_themeListWidget = nullptr;
    PopupListWidget *m_languageListWidget = nullptr;

    double m_currentPixWidth;
    double m_currentPixHeight;

    QMenu *m_pluginMenu = nullptr;

    QString m_oldThemeName;

    QPoint m_pressPoint;
    bool m_isPressInDragArea = false;

    QList<QWidget *> m_alwaysOpenModules;

    QMenuBar *m_menuBar = nullptr;
    QList<QMenu *> m_allMenus;

    //仅仅在第一次手动改变缩放比例时，才设置tabwidget的tabbar为自适应
//    int m_changeRateCount = 0;

    MainDockWindow *m_mainDockWindow = nullptr;

    //-------鼠标拖动改变窗体大小的变量集--------

//    QSizeGrip *m_pSizeGrip = nullptr;
    QStatusBar *m_pStatusBar = nullptr;

//    bool m_bLeftMousePressed = false;  // 鼠标左键是否被按下
//    QPoint m_lastPoint;               // 鼠标上次按下的点

    //-------鼠标拖动改变窗体大小的变量集--------

};

#endif // MainWindow_H
