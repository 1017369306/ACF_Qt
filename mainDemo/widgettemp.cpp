#include "widgettemp.h"
#include "ui_widgettemp.h"

widgetTemp::widgetTemp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetTemp)
{
    ui->setupUi(this);
}

widgetTemp::~widgetTemp()
{
    delete ui;
}
