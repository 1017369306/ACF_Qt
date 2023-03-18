#include "queryItem.h"
#include "ui_queryItem.h"

queryItem::queryItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::queryItem)
{
    ui->setupUi(this);

    this->init();
}

queryItem::~queryItem()
{
    delete ui;
}

void queryItem::init(){
    QStringList items{"并且", "或者"};
    for (int i = 0; i < items.length(); i++) {
        ui->comboBox->addItem(items.at(i), i);
    }
    //关系下拉框默认不可见，且默认为并且
    ui->comboBox->setVisible(false);
    ui->comboBox->setCurrentIndex(0);

    QStringList equalItems{"=", "!=", ">", ">=", "<", "<=", "包含", "不包含"};
    for (int i = 0; i < equalItems.length(); i++) {
        ui->comboBox_equal->addItem(equalItems.at(i), i);
    }
    ui->comboBox_equal->setCurrentIndex(0);

    connect(ui->pushButton_close, &QPushButton::clicked, this, &queryItem::slot_closeClicked);

    this->initIcon();
}

void queryItem::initIcon(){
    ui->pushButton_close->setIcon(QIcon("./appPics/svgs/theme/close.svg"));
}

QList<QString> queryItem::getKeys() const
{
    return m_keys;
}

void queryItem::setKeys(const QList<QString> &keys)
{
    m_keys = keys;

    for (int i = 0; i < m_keys.length(); i++) {
        ui->comboBox_key->addItem(m_keys.at(i), i);
    }
    ui->comboBox_key->setCurrentIndex(0);
}

void queryItem::slot_closeClicked(bool checked){
    this->close();
}

void queryItem::setRelationVisible(bool visible){
    ui->comboBox->setVisible(visible);
}

QueryCondition queryItem::getQueryCondition(){
    QueryCondition queryCondition;
    queryCondition.queryRelation = (QueryRelation)ui->comboBox->currentData().toInt();
    queryCondition.key = ui->comboBox_key->currentText();
    queryCondition.queryCompare = (QueryCompare)ui->comboBox_equal->currentData().toInt();
    queryCondition.value = ui->lineEdit->text();
    return queryCondition;
}

void queryItem::reset(bool onlyResetValue){
    if(!onlyResetValue){
        ui->comboBox_key->setCurrentIndex(0);
        ui->comboBox_equal->setCurrentIndex(0);
    }
    ui->lineEdit->setText("");
}

