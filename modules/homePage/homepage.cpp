#include "homepage.h"

#include "ui_homepage.h"
#include <QPushButton>
#include <QDebug>

HomePage::HomePage(QWidget* parent) : ModuleBase(parent), ui(new Ui::homePage) {
    ui->setupUi(this);

    connect(ui->pushButton_createException, &QPushButton::clicked, this, &HomePage::slot_createException);
}

HomePage::~HomePage() { delete ui; }

void HomePage::disModule() {}

void HomePage::slot_createException(bool checked){
    QObject *obj = nullptr;
    QString name = obj->objectName();

    qDebug() << "objectName: " << name;
}
