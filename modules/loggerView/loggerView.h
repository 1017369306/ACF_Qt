#ifndef LoggerView_H
#define LoggerView_H

#include "queryItem.h"

#include <QStandardItem>
#include <QWidget>
#include <moduleBase.h>
#include <QPushButton>

#define CUSTOMDATA "customData"

namespace Ui {
class LoggerView;
}

class LoggerView : public ModuleBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "loggerView.json")
    Q_INTERFACES(IPlugIn)
public:
    explicit LoggerView(QWidget *parent = nullptr);
    ~LoggerView();

    IPlugIn *createNewPlugin() override {return this;}

    QWidget *getWidget(QWidget *parent = nullptr) override {return this;}

    void disModule() override;

private:

//    void slot_queryLog(const QList<QueryCondition> &conditions);
    void slot_queryLog();

    void slot_addPlugin(bool checked = false);

    void slot_removePlugin(bool checked = false);

    void slot_clearPlugin(bool checked = false);

    void slot_textChanged(const QString &text);

    void slot_tableCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

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
    Ui::LoggerView *ui;

    QStandardItemModel *m_model = nullptr;

};

#endif // LoggerView_H
