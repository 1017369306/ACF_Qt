#ifndef VIEWPANELS_H
#define VIEWPANELS_H

#include <QHBoxLayout>
#include <QTableWidget>
#include <QTreeWidget>
#include <QToolBox>
#include <QToolBar>
#include <QListWidget>
#include <moduleBase.h>
//#include <DevMachines/QtitanBase>
#include <DevMachines/QtitanDocking>
//#include <DevMachines/QtitanStyle>
#include <QTextEdit>
#include <QLineEdit>

/**
 * @brief 视图面本的基类
 */
class DockWidgetPanelCreator : public QObject
{
public:
    explicit DockWidgetPanelCreator(const QString& caption, DockMainWindow* parent);
    virtual ~DockWidgetPanelCreator();
public:
    DockWidgetPanel* widgetPanel() const { return m_dockWidgetPanel; }
protected:
    DockWidgetPanel* m_dockWidgetPanel;
};

/**
 * @brief 回放视图的面板创建器
 */
class ReplayModuleListViewPanelCreator : public DockWidgetPanelCreator
{
public:
    explicit ReplayModuleListViewPanelCreator(DockMainWindow* parent);
    virtual ~ReplayModuleListViewPanelCreator();
protected:
    void fillToolBar();
    void fillFileView();
protected:
    QWidget* baseLayoutWidget;
    QVBoxLayout* baseLayout;
private:
    Q_DISABLE_COPY(ReplayModuleListViewPanelCreator)
};

/**
 * @brief 功能视图的面板创建器
 */
class ModuleListViewPanelCreator : public DockWidgetPanelCreator
{
    Q_OBJECT
public:
    explicit ModuleListViewPanelCreator(DockMainWindow* parent);
    virtual ~ModuleListViewPanelCreator();

    /**
     * @brief 加载所有的插件
     */
    void loadAllPlugins();

public:
signals:
    void itemClicked(QListWidgetItem *item);

protected:
    void fillToolBar();
    void fillModule();
    void createListItem(ModuleProperty &property);

private:
    QWidget *baseLayoutWidget = nullptr;
    QVBoxLayout *baseLayout = nullptr;
    QLineEdit *m_lineEdit = nullptr;
    QListWidget *m_listWidget = nullptr;

private:
    Q_DISABLE_COPY(ModuleListViewPanelCreator)
};

/* ResourceViewPanelCreator */
class ResourceViewPanelCreator : public DockWidgetPanelCreator
{
public:
    explicit ResourceViewPanelCreator(DockMainWindow* parent);
    virtual ~ResourceViewPanelCreator();
protected:
    void fillResourceView();
private:
    QWidget* baseLayoutWidget;
    QVBoxLayout* baseLayout;
private:
    Q_DISABLE_COPY(ResourceViewPanelCreator)
};

/**
 * @brief 输出/打印视图的面板创建器
 */
class OutputViewPanelCreator : public DockWidgetPanelCreator
{
public:
    explicit OutputViewPanelCreator(DockMainWindow* parent);
    virtual ~OutputViewPanelCreator();

    /**
     * @brief 获取编辑器
     * @return
     */
    QTextEdit *getTextEdit() const{
        return m_textEdit;
    }

private:
    QTextEdit *m_textEdit = nullptr;
    Q_DISABLE_COPY(OutputViewPanelCreator)
};

/* WatchPanelCreator */
class WatchPanelCreator : public DockWidgetPanelCreator
{
public:
    explicit WatchPanelCreator(DockMainWindow* parent);
    virtual ~WatchPanelCreator();
protected:
    QTableWidget* m_watchList;
private:
    Q_DISABLE_COPY(WatchPanelCreator)
};

/**
 * @brief PropertyGrid参数视图的面板创建器
 */
class PropertyGridViewPanelCreator : public DockWidgetPanelCreator
{
public:
    explicit PropertyGridViewPanelCreator(DockMainWindow* parent);
    virtual ~PropertyGridViewPanelCreator();
protected:
    QTreeWidget* m_toolBox;
private:
    Q_DISABLE_COPY(PropertyGridViewPanelCreator)
};

#endif /* VIEWPANELS_H */
