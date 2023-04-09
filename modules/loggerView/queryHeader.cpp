#include "queryHeader.h"
#include "ui_queryHeader.h"
#include <globalSizes.h>
#include <IPlugIn.h>
#include <pluginManager.h>
#include <globalEnums.h>

queryHeader::queryHeader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::queryHeader)
{
    ui->setupUi(this);

    this->init();
}

queryHeader::~queryHeader()
{
    delete ui;
}

void queryHeader::init(){
    ui->listWidget->setViewMode(QListView::IconMode); //设置Item图标显示
    ui->listWidget->setFlow(QListView::LeftToRight);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setDragEnabled(false);            //控件不允许拖动
    ui->listWidget->setEditTriggers(QListView::EditTrigger::NoEditTriggers);
    ui->listWidget->setCursor(Qt::CursorShape::PointingHandCursor);
    ui->listWidget->setMouseTracking(true);//鼠标跟踪生效
    ui->listWidget->setSpacing(GlobalSizes::instance()->DefaultIconHeight());

    connect(ui->pushButton_add, &QPushButton::clicked, this, &queryHeader::slot_addClicked);
    connect(ui->pushButton_minus, &QPushButton::clicked, this, &queryHeader::slot_minusClicked);
    connect(ui->pushButton_query, &QPushButton::clicked, this, &queryHeader::slot_queryClicked);
    connect(ui->pushButton_reset, &QPushButton::clicked, this, &queryHeader::slot_resetClicked);

    this->initIcon();
}

void queryHeader::initIcon(){
    ui->pushButton_query->setIcon(QIcon("./appPics/svgs/theme/query.svg"));
    ui->pushButton_reset->setIcon(QIcon("./appPics/svgs/theme/reset.svg"));
    ui->pushButton_add->setIcon(QIcon("./appPics/svgs/theme/add.svg"));
    ui->pushButton_minus->setIcon(QIcon("./appPics/svgs/theme/minus.svg"));
}

QList<QueryCondition> queryHeader::getQueryConditions(){
    QList<QueryCondition> conditions;
    int count = ui->listWidget->count();
    for (int i = 0; i < count; i++) {
        QWidget *widget = ui->listWidget->itemWidget(ui->listWidget->item(i));
        if(widget){
            queryItem *item = static_cast<queryItem *>(widget);
            if(item){
                conditions.append(item->getQueryCondition());
            }
        }
    }
    return conditions;
}

QList<QString> queryHeader::getKeys() const
{
    return m_keys;
}

void queryHeader::setKeys(const QList<QString> &keys)
{
    m_keys = keys;
}

void queryHeader::slot_addClicked(bool checked){
    queryItem *item = new queryItem();
    item->setKeys(m_keys);
    item->setRelationVisible(true);

    QListWidgetItem *listItem = new QListWidgetItem();
    //此多种方式才是默认设置
    listItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);

    ui->listWidget->addItem(listItem);
    ui->listWidget->setItemWidget(listItem, item);
}

void queryHeader::slot_minusClicked(bool checked){
    if(ui->listWidget->count() > 1){
        ui->listWidget->removeItemWidget(ui->listWidget->item(ui->listWidget->count() - 1));
    }
    else{
        IPlugIn *m_notify = PluginManager::instance()->getIPlugin("notifyLib");
        if(m_notify != nullptr){
            m_notify->sendData(QVariant::fromValue(NotifyStruct(NotifyLevel::WarnLevel, "警告", "删除查询条件失败，至少需要1个查询条件！")));
        }
    }
}

void queryHeader::slot_queryClicked(bool checked){
    emit query(this->getQueryConditions());
}

void queryHeader::slot_resetClicked(bool checked){
    int count = ui->listWidget->count();
    for (int i = 0; i < count; i++) {
        QWidget *widget = ui->listWidget->itemWidget(ui->listWidget->item(i));
        if(widget){
            queryItem *item = static_cast<queryItem *>(widget);
            if(item){
                item->reset();
            }
        }
    }
}
