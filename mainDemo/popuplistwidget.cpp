#include "popuplistwidget.h"
#include <QVBoxLayout>

PopupListWidget::PopupListWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint| Qt::Popup | Qt::WindowStaysOnTopHint);//去除边框和任务栏图标
    this->initUI();
}

PopupListWidget::~PopupListWidget()
{

}

void PopupListWidget::initUI()
{
    this->m_listWidget = new QListWidget();
    this->m_listWidget->setObjectName("m_listWidget");
    this->m_listWidget->setResizeMode(QListView::Adjust);
    this->m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //连接信号
    connect(m_listWidget, &QListWidget::currentRowChanged, this, &PopupListWidget::slot_currentRowChanged);

    auto mainLay = new QVBoxLayout();
    mainLay->addWidget(m_listWidget, 1);
    mainLay->setSpacing(0);
    mainLay->setContentsMargins(9,9,9,9);
    this->setLayout(mainLay);
}

void PopupListWidget::clearItems()
{
    if(this->m_listWidget != nullptr)
    {
//        for (int i = 0; i < this->m_listWidget->count(); i++) {
//            QListWidgetItem *item = this->m_listWidget->item(i);
//        }
        this->m_listWidget->clear();
    }
}

void PopupListWidget::setItems(const QList<QString> &items){
    this->clearItems();
    int length = items.length();
    for (int i = 0; i < length; i++) {
        CustomListItem *button = new CustomListItem();
        button->setInfo(ListItemProperty(items.at(i)));
//        button->setProperty(CUSTOMDATA, items.at(i));
        button->setAttribute(Qt::WA_TransparentForMouseEvents);
        button->setAttribute(Qt::WA_TranslucentBackground);

        QListWidgetItem *listItem = new QListWidgetItem();
        //此多种方式才是默认设置
        listItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);

        this->m_listWidget->addItem(listItem);
        this->m_listWidget->setItemWidget(listItem, button);
    }
}

void PopupListWidget::setItems(const QList<ListItemProperty> &items){
    this->clearItems();
    int length = items.length();
    for (int i = 0; i < length; i++) {
        ListItemProperty property = items.at(i);

        CustomListItem *button = new CustomListItem();
        button->setInfo(property);
        button->setProperty(CUSTOMDATA, property.customData);
        button->setAttribute(Qt::WA_TransparentForMouseEvents);
        button->setAttribute(Qt::WA_TranslucentBackground);

        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setToolTip(property.toolTip);
        //此多种方式才是默认设置
        listItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);

        //设置item的widget背景色不透明
//        widget->setAutoFillBackground(true);

        this->m_listWidget->addItem(listItem);
        this->m_listWidget->setItemWidget(listItem, button);
    }
}

void PopupListWidget::slot_currentRowChanged(int currentRow){
    QWidget *widget = this->m_listWidget->itemWidget(this->m_listWidget->currentItem());
    if(widget){
        QVariant customData = widget->property(CUSTOMDATA);
        emit this->selectedItem(currentRow, customData);
    }
    else{
        emit this->selectedItem(currentRow, QVariant::Invalid);
    }
}

