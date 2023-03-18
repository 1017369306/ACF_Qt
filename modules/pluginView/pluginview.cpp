#include "pluginview.h"
#include "ui_pluginview.h"
#include <pluginManager.h>
#include <QListView>

PluginView::PluginView(QWidget *parent) :
    ModuleBase(parent),
    ui(new Ui::PluginView)
{
    ui->setupUi(this);

    this->initUI();
    this->connectAllSignal();
    this->loadAllPlugins();
}

PluginView::~PluginView()
{
    delete ui;
}

void PluginView::disModule(){

}

/**
 * @brief init 初始化UI
 */
void PluginView::initUI(){
//    ui->listWidget->setViewMode(QListView::IconMode); //设置Item图标显示
//    ui->listWidget->setFlow(QListView::LeftToRight);
//    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setDragEnabled(false);            //控件不允许拖动
    ui->listWidget->setEditTriggers(QListView::EditTrigger::NoEditTriggers);
    ui->listWidget->setCursor(Qt::CursorShape::PointingHandCursor);
    ui->listWidget->setMouseTracking(true);//鼠标跟踪生效
    ui->listWidget->setSpacing(6);
}

void PluginView::loadAllPlugins(){
    QList<PlugInProperty> modules = PluginManager::instance()->allPluginsProperty();
    int length = modules.length();
    for (int i = 0; i < length; i++) {
        PlugInProperty property = modules.at(i);
        if(property.level == PlugInLevel::MustLoad){
            QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);
            if(loader == nullptr)
                continue;

            PluginInfoView *info = new PluginInfoView();
//            info->setFixedSize(QSize(400, 200));
            info->setInfo(property, loader->isLoaded());
            connect(info, &PluginInfoView::pluginStatusChanging, this, &PluginView::slot_pluginStatusChanging);

            QListWidgetItem *listItem = new QListWidgetItem();
//            listItem->setSizeHint(QSize(400, 200));
            //此多种方式才是默认设置
            listItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);

            ui->listWidget->addItem(listItem);
            ui->listWidget->setItemWidget(listItem, info);
        }
    }
}

/**
 * @brief connectAllSignal 建立所有信号和槽
 */
void PluginView::connectAllSignal(){
    connect(ui->btn_add, &QPushButton::clicked, this, &PluginView::slot_addPlugin);
    connect(ui->btn_remove, &QPushButton::clicked, this, &PluginView::slot_removePlugin);
    connect(ui->btn_clear, &QPushButton::clicked, this, &PluginView::slot_clearPlugin);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &PluginView::slot_textChanged);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &PluginView::slot_currentRowChanged);
}

void PluginView::slot_addPlugin(bool checked){

}

void PluginView::slot_removePlugin(bool checked){

}

void PluginView::slot_clearPlugin(bool checked){

}

void PluginView::slot_textChanged(const QString &text){

}

void PluginView::slot_currentRowChanged(int currentRow){

}

void PluginView::slot_pluginStatusChanging(PluginInfoView *sender, const PlugInProperty &property, const bool &isLoad){
    QPluginLoader *loader = PluginManager::instance()->getPlugin(property.name);
    if(loader != nullptr)
    {
        bool newStatus = isLoad;
        if(isLoad && loader->isLoaded()){
            if(loader->unload()){
                newStatus = !isLoad;
            }
        }
        else if(!isLoad && !loader->isLoaded()){
            if(loader->load()){
                newStatus = !isLoad;
            }
        }
        if(newStatus != isLoad){
            sender->setInfo(property, newStatus);
        }
    }
}