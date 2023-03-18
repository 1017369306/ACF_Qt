#ifndef PLUGININFOVIEW_H
#define PLUGININFOVIEW_H

#include <QWidget>
#include <IPlugIn.h>

namespace Ui {
class pluginInfoView;
}

class PluginInfoView : public QWidget
{
    Q_OBJECT

public:
    explicit PluginInfoView(QWidget *parent = nullptr);
    ~PluginInfoView();

    void setInfo(const PlugInProperty &property, const bool &isLoad);

signals:

    void pluginStatusChanging(PluginInfoView *sender, const PlugInProperty &property, const bool &isLoad);

private slots:

    void slot_btnInstallClicked(bool checked = false);

private:
    Ui::pluginInfoView *ui;

    PlugInProperty m_property;

    bool m_isLoad = false;

};

#endif // PLUGININFOVIEW_H
