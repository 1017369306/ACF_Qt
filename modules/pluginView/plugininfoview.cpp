#include "plugininfoview.h"
#include "ui_plugininfoview.h"
#include <QPushButton>
#include <globalSizes.h>

PluginInfoView::PluginInfoView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pluginInfoView)
{
    ui->setupUi(this);

    ui->label_key->setProperty("PrimaryColor", true);
    ui->label_name->setProperty("PrimaryColor", true);
    ui->label_category->setProperty("PrimaryColor", true);
    ui->label_type->setProperty("PrimaryColor", true);
    ui->label_author->setProperty("PrimaryColor", true);
    ui->label_version->setProperty("PrimaryColor", true);
    ui->textEdit_description->setProperty("PrimaryColor", true);
    ui->label_copyright->setProperty("PrimaryColor", true);

    ui->label_icon->setFixedWidth(GlobalSizes::instance()->DefaultControlHeight() * 1.5);
    connect(ui->btn_install, &QPushButton::clicked, this, &PluginInfoView::slot_btnInstallClicked);
}

PluginInfoView::~PluginInfoView()
{
    delete ui;
}

void PluginInfoView::setInfo(const PlugInProperty &property, const bool &isLoad){
    m_property = property;
    m_isLoad = isLoad;

    ui->label_key->setText(property.name);
    ui->label_name->setText(property.displayName);
    ui->label_category->setText(property.category);
    //插件等级：0：默认、按需加载；1：必装插件、启动就加载
    if(property.level == PlugInLevel::NeedLoad){
        ui->label_type->setText("按需加载");
    }
    else if(property.level == PlugInLevel::MustLoad){
        ui->label_type->setText("启动加载");
    }
    ui->label_author->setText(property.author);
    ui->label_version->setText(property.version);
    ui->textEdit_description->setText(property.description);
    ui->label_copyright->setText(property.copyright);

    QIcon icon(property.icon);
    QPixmap pixmap(property.icon);
    ui->label_icon->setPixmap(pixmap);

    if(isLoad){
        ui->btn_install->setText("卸载");
    }
    else{
        ui->btn_install->setText("安装");
    }

}

void PluginInfoView::slot_btnInstallClicked(bool checked){
    emit pluginStatusChanging(this, m_property, m_isLoad);
}
