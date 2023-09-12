#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <moduleBase.h>

#if defined(__MINGW32__)
#include <ActiveQt/QAxWidget>
#else
#include <QWebEngineView>
#endif

namespace Ui {
class MainView;
}

class MainView : public ModuleBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "mapModule.json")
    Q_INTERFACES(IPlugIn)

public:
    explicit MainView(QWidget *parent = nullptr);
    ~MainView();

    IPlugIn *createNewPlugin() override {return this;}

    QWidget *getWidget(QWidget *parent = nullptr) override {return this;}

    void disModule() override;

private slots:

    void slot_loadFinished(bool ok);

private:

    /**
     * @brief initUI 初始化
     */
    void init();

    void initIcon();

    /**
     * @brief connectAllSignal 建立所有信号和槽
     */
    void connectAllSignal();

private:
    Ui::MainView *ui;

#if defined(__MINGW32__)
    QAxWidget *axWidget = nullptr;
#else
    QWebEngineView *m_view = nullptr;
    QWebEnginePage *m_webPage = nullptr;
#endif

};

#endif // MAINVIEW_H
