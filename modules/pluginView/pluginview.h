#ifndef PLUGINVIEW_H
#define PLUGINVIEW_H

#include <QWidget>
#include <moduleBase.h>
#include <plugininfoview.h>

#define CUSTOMDATA "customData"

namespace Ui {
class PluginView;
}

class PluginView : public ModuleBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "pluginView.json")
    Q_INTERFACES(IPlugIn)
public:
    explicit PluginView(QWidget *parent = nullptr);
    ~PluginView();

    IPlugIn *createNewPlugin() override {return this;}

    QWidget *getWidget(QWidget *parent = nullptr) override {return this;}

    void disModule() override;

private:

    void slot_addPlugin(bool checked = false);

    void slot_removePlugin(bool checked = false);

    void slot_clearPlugin(bool checked = false);

    void slot_textChanged(const QString &text);

    void slot_currentRowChanged(int currentRow);

    void slot_pluginStatusChanging(PluginInfoView *sender, const PlugInProperty &property, const bool &isLoad);

private:

    /**
     * @brief initUI 初始化UI
     */
    void initUI();

    /**
     * @brief loadAllPlugins 加载所有的插件
     */
    void loadAllPlugins();

    /**
     * @brief connectAllSignal 建立所有信号和槽
     */
    void connectAllSignal();

private:
    Ui::PluginView *ui;

};

#endif // PLUGINVIEW_H
