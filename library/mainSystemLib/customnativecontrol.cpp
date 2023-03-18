#include "customnativecontrol.h"
#include <globalSizes.h>
#include <QDebug>

CustomTabBar::CustomTabBar(QWidget *parent) : QTabBar(parent)
{

}

QSize CustomTabBar::tabSizeHint(int index) const {
    // 获取系统默认的大小
    QSize newSize = QTabBar::tabSizeHint(index);

    //todo 张洋 奇怪，修改分辨率后，这里获取的font的字体大小仍然为原来的字体大小，但是UI上字体是发生了改变的
//    qDebug() << "font:" << font().weight() << "-" << font().pixelSize() << "-" << font().pointSize() << "-" << font().pixelSize();
    // tabBar选项卡宽度自适应
    QFont fontTemp(font().family(), GlobalSizes::instance()->DefaultFontSize());
    QFontMetrics fontMetricsTemp(fontTemp);
    int pixelsWidth = fontMetricsTemp.width(this->tabText(index));
    double realWidth = pixelsWidth + GlobalSizes::instance()->DefaultInputPadding().x() + GlobalSizes::instance()->DefaultInputPadding().width();
    if(!this->tabIcon(index).isNull()){
        realWidth += this->iconSize().width();
    }
    if(this->tabButton(index, QTabBar::RightSide) != nullptr){
        realWidth += this->tabButton(index, QTabBar::RightSide)->width();
    }
    newSize.setWidth(realWidth);

    //直接在这里更新图标大小
    QSize iconSize(GlobalSizes::instance()->DefaultIconHeight(), GlobalSizes::instance()->DefaultIconHeight());
    if(this->tabButton(index, QTabBar::RightSide) != nullptr){
        this->tabButton(index, QTabBar::RightSide)->setFixedSize(iconSize);
    }

    return newSize;
}
