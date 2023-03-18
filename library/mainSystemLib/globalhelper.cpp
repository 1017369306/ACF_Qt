#include "globalhelper.h"
#include <QDebug>
#include <QLayout>
#include <QLayoutItem>
#include <QApplication>
#include <QPainter>
//#include <QtSvg/QSvgRenderer>
#include <QtSvg>

void GlobalMethods::removeLayoutAllItem(QWidget *widget)
{
    if(widget == nullptr)
        return;
    QLayout *layout = widget->layout();
    if(layout == nullptr)
        return;
//    if(layout != nullptr){
//        for (int i = 0; i < layout->count(); i++) {
//            QLayoutItem *itemLayout = layout->itemAt(i);
//            layout->removeItem(itemLayout);
//            QWidget *itemWidget = itemLayout->widget();
//            if(itemWidget != nullptr){
//                itemWidget->setParent(nullptr);
//            }
//        }
//    }
//    delete layout;
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != 0) {
        if(child->widget()){
            child->widget()->setParent(NULL);
            delete child->widget();
        }
        delete child;
    }
    delete layout;
}

void GlobalMethods::foreachUpward(QWidget *widget){
    if(widget == nullptr)
        return;
    GlobalMethods::debugInfo(widget);
    QWidget *parentQWidget = widget->parentWidget();
    if(parentQWidget != nullptr){
        GlobalMethods::foreachUpward(parentQWidget);
    }
}

void GlobalMethods::foreachDownward(QWidget *widget){
    if(widget == nullptr)
        return;
    GlobalMethods::debugInfo(widget);
    foreach (auto item, widget->children()) {
        if(item->isWidgetType()){
            QWidget *widgetTemp = qobject_cast<QWidget *>(item);
            if(widgetTemp !=nullptr){
                foreachDownward(widgetTemp);
            }
        }
    }
}

void GlobalMethods::debugInfo(QWidget *widget){
    if(widget == nullptr)
        return;
    QString widgetName = widget->metaObject()->className();
    qDebug() << "widget name:" << widgetName;
    qDebug() << "widget width:" << widget->size().width() << " height:" << widget->size().height();

    QObject *parent = widget->parent();
    QWidget *parentQWidget = widget->parentWidget();
    QWidget *nativeParentWidget = widget->nativeParentWidget();
    if(parent != nullptr){
        QString className = parent->metaObject()->className();
        qDebug() << "parent name:" << className;
    }
    if(parentQWidget != nullptr){
        QString className = parentQWidget->metaObject()->className();
        qDebug() << "parentQWidget name:" << className;
        qDebug() << "parentQWidget width:" << parentQWidget->size().width() << " height:" << parentQWidget->size().height();
    }
    if(nativeParentWidget != nullptr){
        QString className = nativeParentWidget->metaObject()->className();
        qDebug() << "nativeParentWidget name:" << className;
    }
}

QFileInfoList GlobalMethods::foreachDir(const QString &path, const QStringList &nameFilters){
    QFileInfoList list;
    QDir dir(path);
    if(!dir.exists()){
        return list;
    }

    dir.setFilter(QDir::Dirs | QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList infos = dir.entryInfoList(nameFilters);

    if(infos.size() > 0){
        int i = 0;
        do{
            QFileInfo fileinfo = infos.at(i);
            if(fileinfo.fileName() == "." | fileinfo.fileName() == ".."){
                i++;
                continue;
            }
            bool isDir = fileinfo.isDir();
            if(isDir){
                QFileInfoList childList = GlobalMethods::foreachDir(fileinfo.filePath(), nameFilters);
                list.append(childList);
            }
            else{
                list.append(fileinfo);
            }
            i++;
        }while (i < infos.size());
    }

    return list;
}

bool GlobalMethods::getIPAndPort(const QString &uri, QString &ip, int &port){
    if(uri.isEmpty())
        return false;

    QStringList list = uri.split("//");
    if(list.size() >= 2){
        QString ipAndPort = list[1];
        QStringList ipAndPortList = ipAndPort.split(":");
        if(ipAndPortList.size() >= 2){
            ip = ipAndPortList[0];
            QString portStr = ipAndPortList[1];
            port = portStr.toInt();
            return true;
        }
    }
    return false;
}

QString GlobalMethods::stdToQString(const std::wstring &str){
#ifdef _MSC_VER
    return QString::fromUtf16((const ushort *)str.c_str());
#else
    return QString::fromStdWString(str);
#endif
}

QMainWindow *GlobalMethods::getMainWindow(){
    foreach (QWidget *w, qApp->topLevelWidgets()) {
        if(QMainWindow *mainWin = qobject_cast<QMainWindow *>(w))
            return mainWin;
    }
    return nullptr;
}

QPixmap PixmapHelper::loadSvg(const QString &path, const int &width, const int &height){
    int origWidth = width;
    int origHeight = height;

    const auto ratio = qApp->devicePixelRatio();
//    if ( 2 == ratio || 3 == ratio) {
//        origWidth += width;
//        origHeight += height;
//    }
    QPixmap pixmap(origWidth, origHeight);
    QSvgRenderer renderer(path);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}
