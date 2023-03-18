#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <moduleBase.h>

namespace Ui {
class homePage;
}

class HomePage : public ModuleBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "homePage.json")
    Q_INTERFACES(IPlugIn)
public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

    IPlugIn *createNewPlugin() override {return this;}

    QWidget *getWidget(QWidget *parent = nullptr) override {return this;}

    void disModule() override;

private:
    Ui::homePage *ui;

};

#endif // HOMEPAGE_H
