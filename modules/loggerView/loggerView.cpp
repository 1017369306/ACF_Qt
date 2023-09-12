//解决中文乱码
#pragma execution_character_set("utf-8")

#include "loggerView.h"
#include "ui_loggerView.h"
#include <pluginManager.h>
#include <QListView>
#include <globalhelper.h>
#include <QStandardItem>

LoggerView::LoggerView(QWidget *parent) :
    ModuleBase(parent),
    ui(new Ui::LoggerView)
{
    ui->setupUi(this);

    this->init();
    this->connectAllSignal();
}

LoggerView::~LoggerView()
{
    delete ui;
}

void LoggerView::disModule(){

}

/**
 * @brief init 初始化UI
 */
void LoggerView::init(){
    ui->label_dateTime->setText("");
    ui->label_loggerLevel->setText("");
    ui->label_content->setText("");

    ui->queryControl->setKeys(QList<QString>{"日志类型", "时间"});
    ui->queryControl->setVisible(false);

    QStringList comboBoxKeys = {"普通", "警告", "错误", "严重"};
    QStringList comboBoxValues = {"INFO", "WARN", "ERROR", "FATAL"};
    for (int i = 0; i < comboBoxKeys.count(); i++) {
        ui->comboBox->addItem(comboBoxKeys.at(i), comboBoxValues.at(i));
    }
    ui->comboBox->setCurrentIndex(0);
//    ui->comboBox->addItems(array);
//    ui->comboBox->setCurrentText("普通");
    ui->comboBox->setView(new QListView());

    ui->dateEdit_start->setDateTime(QDateTime::currentDateTime());
    ui->dateEdit_end->setDateTime(QDateTime::currentDateTime());

    m_model = new QStandardItemModel();
    QStringList colNameList = {"记录时间", "日志类型", "描述"};
    for(int i = 0; i < colNameList.size(); i++)
    {
        QStandardItem *itemValue = new QStandardItem;
        itemValue->setData(colNameList.at(i), Qt::DisplayRole);
        itemValue->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_model->setHorizontalHeaderItem(i, itemValue);
    }
    ui->tableView->setModel(m_model);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &LoggerView::slot_tableCurrentRowChanged);

    ui->tableView->setContentsMargins(0,0,0,0);
    QHeaderView *headview = ui->tableView->horizontalHeader();
    headview->setSectionResizeMode(QHeaderView::Stretch);
    headview->setDefaultAlignment(Qt::AlignCenter);//表头信息居中
    headview->setHighlightSections(false);
    headview->setSectionsMovable(false);//用户不可调整列的位置
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0, 200);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(1, 100);

    //    headview->setStretchLastSection(true);//

    ui->tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableView->setAutoFillBackground(true);
    ui->tableView->setShowGrid(true);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//选中模式为整行
    ui->tableView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);//不能编辑表格
    ui->tableView->setSortingEnabled(true);//打开排序
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setWordWrap(true);
    ui->tableView->setMouseTracking(true);

    this->initIcon();
}

void LoggerView::initIcon(){

}

/**
 * @brief connectAllSignal 建立所有信号和槽
 */
void LoggerView::connectAllSignal(){
//    connect(ui->queryControl, &queryHeader::query, this, &LoggerView::slot_queryLog);
    connect(ui->pushButton_query, &QPushButton::clicked, this, &LoggerView::slot_queryLog);

}

void LoggerView::slot_addPlugin(bool checked){

}

void LoggerView::slot_removePlugin(bool checked){

}

void LoggerView::slot_clearPlugin(bool checked){

}

void LoggerView::slot_textChanged(const QString &text){

}

void LoggerView::slot_queryLog(/*const QList<QueryCondition> &conditions*/){
    //清空表格
    if(m_model)
    {
        int rowCount = m_model->rowCount();
        if(rowCount > 0){
            m_model->removeRows(0, rowCount);
        }
    }

//    foreach (QueryCondition item, conditions) {
//        if(item.key == "日志类型"){
//        }
//        else if(item.key == "时间"){

//        }
//    }

    QString type = ui->comboBox->currentData().toString();
    QString logPath(QApplication::applicationDirPath() + "/logs/"+ type +"/"+ type.toLower() +".log");
    QList<LogDataModel> entitys = LogReaderHelper::read(logPath);
    if(m_model){
        foreach (LogDataModel model, entitys) {
            int rowcount = m_model->rowCount();
            m_model->setRowCount(rowcount + 1);
            m_model->setData(m_model->index(rowcount, 0), model.recordTime.toString("yyyy-MM-dd HH:mm:ss.zzz"), Qt::DisplayRole);
            m_model->setData(m_model->index(rowcount, 1), model.logLevel, Qt::DisplayRole);
            m_model->setData(m_model->index(rowcount, 2), model.msg, Qt::DisplayRole);
        }
    }
}

void LoggerView::slot_tableCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous){
    int rowcount = m_model->rowCount();
    if(current.row() < rowcount){
        if(m_model->item(current.row(), 0) != nullptr)
            ui->label_dateTime->setText(m_model->item(current.row(), 0)->text());
        if(m_model->item(current.row(), 1) != nullptr)
            ui->label_loggerLevel->setText(m_model->item(current.row(), 1)->text());
        if(m_model->item(current.row(), 2) != nullptr)
            ui->label_content->setText(m_model->item(current.row(), 2)->text());
    }
}
