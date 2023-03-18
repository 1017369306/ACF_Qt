#include "homepage.h"

#include "ui_homepage.h"

HomePage::HomePage(QWidget* parent) : ModuleBase(parent), ui(new Ui::homePage) { ui->setupUi(this); }

HomePage::~HomePage() { delete ui; }

void HomePage::disModule() {}
