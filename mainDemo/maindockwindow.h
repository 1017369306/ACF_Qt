#ifndef MainDockWindow_H
#define MainDockWindow_H

#include <popuplistwidget.h>
#include <IPlugIn.h>
#include <QMouseEvent>
#include <customlistitem.h>
#include <QString>
#include <moduleBase.h>

#include <DevMachines/QtitanBase>
#include <DevMachines/QtitanDocking>
#include <DevMachines/QtitanStyle>
#include "mainviewpanels.h"
#include "customdockwindowbase.h"

//Dock控件中的Property名
#define _qtn_Type_Button       "Type_Button"

/**
 * @brief 主内容区域，主要包含了所有的停靠窗体
 */
class MainDockWindow : public CustomDockWindowBase
{
    Q_OBJECT

public:
    MainDockWindow(QWidget *parent = nullptr);
    ~MainDockWindow();

private slots:

    void slot_itemClicked(QListWidgetItem *item);

    void slot_languageChanged();

    void slot_cssStyleChanged();

    void slot_rightMenuEvent();

    void slot_aboutToShow(DockPanelBase* panel, bool& handled);
    void slot_aboutToClose(DockPanelBase* panel, bool& handled);
    void slot_aboutToAutoHide(DockPanelBase* panel, bool& handled);
    void slot_aboutToFloat(DockPanelBase* panel, bool& handled);

    void slot_dockPanelActivated(DockWidgetPanel* panel);
    void slot_dockPanelDeactivated(DockWidgetPanel* panel);

    void showEvent(QShowEvent* event) override;

private:

    /**
     * @brief 创建主界面的停靠窗体
     */
    void createDockPanels();

    /**
     * @brief 查找Dock窗体的具体内容面板
     * @param name
     * @return
     */
    DockWidgetPanel* findDockChild(const QString& name) const;

    /**
     * @brief 获取此名称Dock面板的个数
     * @param Dock面板唯一name
     * @return
     */
    int findDockPanelCount(const QString& name) const;

    /**
     * @brief 打开功能模块（如果没有此功能模块，先执行loadModule加载功能模块）
     * @param 功能模块插件
     * @return
     */
    bool openModule(IPlugIn& plugin);

    /**
     * @brief 加载功能模块
     * @param 功能模块插件
     * @param 此名称Dock面板已打开的个数
     * @return
     */
    bool loadModule(IPlugIn& plugin, const int &openCount = 0);

    /**
     * @brief connectAllSignal 建立所有信号和槽
     */
    void connectAllSignal();

    /**
     * @brief initMenu 初始化菜单栏
     */
    void initMenu();

    /**
     * @brief initIcon 重新设置所以的icon
     */
    void initIcon();

    /**
     * @brief 根据Dock面板，获取内容元素的插件信息
     * @param panel
     * @return
     */
    IPlugIn *getPlugIn(DockPanelBase* panel);

private:

    bool m_isFirstLoad = true;

    /**
     * @brief 右键测试菜单
     */
    QAction *m_testAction = nullptr;

    DockWidgetPanelCreator* m_moduleListView = nullptr;

    DockWidgetPanelCreator* m_replayModuleListView = nullptr;

    DockWidgetPanelCreator *m_outputView = nullptr;

    DockWidgetPanelCreator* m_propertyGridView = nullptr;

};
#endif // MainDockWindow_H
