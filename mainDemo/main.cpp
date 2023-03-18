#include "mainwindow.h"

#include <QApplication>
#include <stdio.h>
#include <iostream>
#include <QProcess>

static const char black[]  = {0x1b, '[', '1', ';', '3', '0', 'm', 0};
static const char red[]    = {0x1b, '[', '1', ';', '3', '1', 'm', 0};
static const char green[]  = {0x1b, '[', '1', ';', '3', '2', 'm', 0};
static const char yellow[] = {0x1b, '[', '1', ';', '3', '3', 'm', 0};
static const char blue[]   = {0x1b, '[', '1', ';', '3', '4', 'm', 0};
static const char purple[] = {0x1b, '[', '1', ';', '3', '5', 'm', 0};
static const char normal[] = {0x1b, '[', '0', ';', '3', '9', 'm', 0};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString appPath = QApplication::applicationDirPath();

//    std::cout << black << "[DEBUG]" << normal << ' ';

//    char *msg = new char[100];
//    size_t totalLength = 0;
//    size_t length = strlen(red);
//    memcpy(msg, red, length);
//    totalLength += length;

//    size_t length1 = strlen("[DEBUG]");
//    memcpy(msg + totalLength, "[DEBUG]", length1);
//    totalLength += length1;

//    size_t length2 = strlen(normal);
//    memcpy(msg + totalLength, normal, length2);
//    totalLength += length2;

//    char *temp = new char{' '};
//    size_t length3 = strlen(temp);
//    memcpy(msg + totalLength, temp, length3);
//    totalLength += length3;

//    size_t n = fwrite(msg, 1, totalLength, stdout);

    MainWindow w;
    w.show();
    int e = a.exec();
    if(e == CODE_RESTART)
    {
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
        return 0;
    }
    return e;
}
