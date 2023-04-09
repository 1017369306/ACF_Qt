#ifndef NATIVECONTROLSVIEW_H
#define NATIVECONTROLSVIEW_H

#include <QWidget>
#include <moduleBase.h>

#define CUSTOMDATA "customData"

namespace Ui {
class NativeControlsView;
}

class NativeControlsView : public ModuleBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "nativeControls.json")
    Q_INTERFACES(IPlugIn)
public:
    explicit NativeControlsView(QWidget *parent = nullptr);
    ~NativeControlsView();

    IPlugIn *createNewPlugin() override {
        NativeControlsView *view = new NativeControlsView();
        view->setPlugInProperty(this->getPlugInProperty());
        return view;
    }

    QWidget *getWidget(QWidget *parent = nullptr) override {return this;}

    void disModule() override;

    /**
     * @brief ThemeChanged 主题改变通知
     */
    void cssStyleChanged() override;

    /**
     * @brief LanguageChanged 语言改变通知
     */
    void LanguageChanged() override;

private:

    void slot_iconToBig(bool checked = false);

    void slot_iconToSmall(bool checked = false);

    void slot_sliderValueChanged(int value);

private:

    /**
     * @brief initIcon 初始化Icon
     */
    void initIcon();

    /**
     * @brief initData 初始化数据
     */
    void initData();

private:
    Ui::NativeControlsView *ui;

};


#endif // NATIVECONTROLSVIEW_H
