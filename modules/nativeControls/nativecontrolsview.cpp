#include "nativecontrolsview.h"
#include "ui_nativecontrolsview.h"
#include <QListView>
#include <globalhelper.h>
#include <globalColors.h>
#include <QDebug>

NativeControlsView::NativeControlsView(QWidget *parent) :
    ModuleBase(parent),
    ui(new Ui::NativeControlsView)
{
    ui->setupUi(this);

    this->initIcon();
    this->initData();

    connect(ui->pushButton_iconToBig, &QPushButton::clicked, this, &NativeControlsView::slot_iconToBig);
    connect(ui->pushButton_iconToSmall, &QPushButton::clicked, this, &NativeControlsView::slot_iconToSmall);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &NativeControlsView::slot_sliderValueChanged);
    connect(ui->verticalSlider, &QSlider::valueChanged, this, &NativeControlsView::slot_sliderValueChanged);

}

NativeControlsView::~NativeControlsView()
{
    delete ui;
}

void NativeControlsView::disModule(){

}

void NativeControlsView::initIcon(){
    ui->pushButton_iconAndText->setIcon(QIcon("./appPics/svgs/theme/add.svg"));
    ui->pushButton_icon->setIcon(QIcon("./appPics/svgs/theme/all.svg"));

    ui->toolButton_iconAndText->setIcon(QIcon("./appPics/svgs/theme/all1.svg"));
    QIcon icon;
    icon.addPixmap(QPixmap("./appPics/svgs/theme/play.svg"), QIcon::Normal, QIcon::Off);
    icon.addPixmap(QPixmap("./appPics/svgs/theme/stop.svg"), QIcon::Active, QIcon::On);
    ui->toolButton_icon->setIcon(icon);

    ui->toolButton_iconTransparent->setIcon(icon);

    ui->listWidget->setFixedHeight(150);
    ui->listWidget->addItems(QStringList{"项目1", "项目2", "项目3"});
    ui->listWidget_disEnable->setFixedHeight(150);
    ui->listWidget_disEnable->addItems(QStringList{"项目1", "项目2", "项目3"});

}

void NativeControlsView::initData(){

    QStringList array = {"-10X", "-5X", "-2X", "1X", "2X"};
    ui->comboBox->addItems(array);
    ui->comboBox->setCurrentText("1X");
    ui->comboBox->setView(new QListView());

    ui->comboBox_disEnable->addItems(array);
    ui->comboBox_disEnable->setCurrentText("1X");
    ui->comboBox_disEnable->setView(new QListView());

    ui->dateEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateEdit_isDrop->setDateTime(QDateTime::currentDateTime());
    ui->dateEdit_isDrop->setVisible(false);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_disEnable->setDateTime(QDateTime::currentDateTime());
    ui->calendarWidget->setSelectedDate(ui->dateEdit->date());
    ui->timeEdit->setDateTime(QDateTime::currentDateTime());
}

/**
 * @brief ThemeChanged 主题改变通知
 */
void NativeControlsView::cssStyleChanged(){
    this->initIcon();
}

/**
 * @brief LanguageChanged 语言改变通知
 */
void NativeControlsView::LanguageChanged(){

}

void NativeControlsView::slot_iconToBig(bool checked){
    qDebug() << "pushButton_icon width:" << ui->pushButton_icon->iconSize().width() << " height:" << ui->pushButton_icon->iconSize().height();

    ui->pushButton_iconAndText->setFixedSize(ui->pushButton_iconAndText->width() * 2, ui->pushButton_iconAndText->height() * 2);
    ui->pushButton_iconAndText->setIconSize(QSize(ui->pushButton_iconAndText->iconSize().width() * 2, ui->pushButton_iconAndText->iconSize().height() * 2));

    ui->pushButton_icon->setFixedSize(ui->pushButton_icon->width() * 2, ui->pushButton_icon->height() * 2);
    ui->pushButton_icon->setIconSize(QSize(ui->pushButton_icon->iconSize().width() * 2, ui->pushButton_icon->iconSize().height() * 2));

}

void NativeControlsView::slot_iconToSmall(bool checked){
    qDebug() << "pushButton_icon width:" << ui->pushButton_icon->iconSize().width() << " height:" << ui->pushButton_icon->iconSize().height();

    ui->pushButton_iconAndText->setFixedSize(ui->pushButton_iconAndText->width() / 2, ui->pushButton_iconAndText->height() / 2);
    ui->pushButton_iconAndText->setIconSize(QSize(ui->pushButton_iconAndText->iconSize().width() / 2, ui->pushButton_iconAndText->iconSize().height() / 2));

    ui->pushButton_icon->setFixedSize(ui->pushButton_icon->width() / 2, ui->pushButton_icon->height() / 2);
    ui->pushButton_icon->setIconSize(QSize(ui->pushButton_icon->iconSize().width() / 2, ui->pushButton_icon->iconSize().height() / 2));

}

void NativeControlsView::slot_sliderValueChanged(int value){
    ui->horizontalSlider->setValue(value);
    ui->verticalSlider->setValue(value);

    ui->progressBar->setValue(value);
    ui->progressBar_disEnable->setValue(value);

}
