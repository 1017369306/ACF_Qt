#ifndef CUSTOMNATIVECONTROL_H
#define CUSTOMNATIVECONTROL_H

#include "mainSystemLib_global.h"
#include <QObject>
#include <QTabWidget>
#include <QTabBar>

/**
 * @brief The CustomTabBar class 重写QTabBar，用于实现选项卡宽度自适应
 */
class MAINSYSTEMLIB_EXPORT CustomTabBar : public QTabBar
{
public:
    CustomTabBar(QWidget *parent = nullptr);

protected:

    /**
     * @brief tabSizeHint 重写此函数，以实现：QTabWidget中的tabBar选项卡宽度自适应
     * @param index
     * @return
     */
    QSize tabSizeHint(int index) const;

};

/**
 * @brief The CustomTabWidget class 重写QTabWidget，用于设置自定义的tabbar
 */
class CustomTabWidget : public QTabWidget
{
public:
    CustomTabWidget(QWidget *parent = 0) : QTabWidget(parent){
        m_TabBar = new CustomTabBar();
        //只有实现QTabWidget的子类，才能设置tabbar
        setTabBar(m_TabBar);
    }
    ~CustomTabWidget(){
        delete m_TabBar;
    }
private:
    CustomTabBar *m_TabBar;

};

#endif // CUSTOMNATIVECONTROL_H
