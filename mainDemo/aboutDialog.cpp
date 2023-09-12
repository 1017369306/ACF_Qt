//解决中文乱码
#pragma execution_character_set("utf-8")

#include "aboutDialog.h"
#include "ui_aboutDialog.h"
#include <globalSizes.h>
#include <mainwindow.h>

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    this->init();
}

void aboutDialog::init(){
    ui->pushButton_icon->setIconSize(QSize(GlobalSizes::instance()->DefaultControlHeight() * 2, GlobalSizes::instance()->DefaultControlHeight() * 2));
    ui->pushButton_icon->setFixedSize(ui->pushButton_icon->iconSize());
    ui->pushButton_icon->setIcon(QIcon("./appPics/icons/rabbit.ico"));

    ui->label_displayName->setText("Qt 通用客户端框架示例程序");
    ui->label_description->setText("此示例程序用于展示通用客户端框架的效果和能力，主要具备多主题，分辨率自适应，插件化的能力，并且内置基础控件qss样式及2套（白色和黑色）主题样式，样式和主题均可自定义扩展。");
    ui->label_version->setText(VERSION);
    ui->label_copyright->setText("CopyRight @ by 尘缘, qq:1017369306");
}

aboutDialog::~aboutDialog()
{
    delete ui;
}
