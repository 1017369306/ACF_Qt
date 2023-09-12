#ifndef GLOBALSIZES_H
#define GLOBALSIZES_H

#include "globalResource_global.h"
#include <QObject>
#include <QVariant>
#include <QRectF>
#include <QMetaObject>
#include <QMetaProperty>

class GLOBALRESOURCE_EXPORT GlobalSizes : public QObject
{
    Q_OBJECT

    Q_PROPERTY(uint DefaultMarginTop READ DefaultMarginTop WRITE setDefaultMarginTop)
    Q_PROPERTY(uint DefaultControlHeight READ DefaultControlHeight WRITE setDefaultControlHeight)
    Q_PROPERTY(QRectF DefaultControlPadding READ DefaultControlPadding WRITE setDefaultControlPadding)
    Q_PROPERTY(QRectF DefaultInputPadding READ DefaultInputPadding WRITE setDefaultInputPadding)
    Q_PROPERTY(QRectF DefaultCornerRadius READ DefaultCornerRadius WRITE setDefaultCornerRadius)
    Q_PROPERTY(uint DefaultIconHeight READ DefaultIconHeight WRITE setDefaultIconHeight)
    Q_PROPERTY(uint DefaultFontSize READ DefaultFontSize WRITE setDefaultFontSize)
    Q_PROPERTY(uint MiddleFontSize READ MiddleFontSize WRITE setMiddleFontSize)
    Q_PROPERTY(uint LargeFontSize READ LargeFontSize WRITE setLargeFontSize)
    Q_PROPERTY(uint DefaultScrollerWidth READ DefaultScrollerWidth WRITE setDefaultScrollerWidth)

public:

    static GlobalSizes *instance(){
        if(m_instance == nullptr)
        {
            m_instance = new GlobalSizes();
        }
        return m_instance;
    }

    /**
     * @brief Const_PixWidth 元素的大小是以1280 * 1024的分辨率为标准
     */
    static const uint Const_PixWidth;
    static const uint Const_PixHeight;

    /**
     * @brief 控件的默认高度，设计时设定值
     */
    static const uint Const_DefaultControlHeight;

    /**
     * @brief Const_LeftTopX ubuntu系统下，桌面区域的左上角点的位置，以100%缩放为标准
     */
    static const uint Const_LeftTopX;
    static const uint Const_LeftTopY;

    /**
     * @brief updateSize 更新全局的大小值的属性值
     * @param rate 相对于Const_PixWidth，新的缩放比例
     */
    void updateSize(const double &rate);

    /**
     * @brief DefaultMarginTop 默认的Margin-Top值
     * @return
     */
    uint DefaultMarginTop() const;
    void setDefaultMarginTop(const uint &defaultMarginTop);

    /**
     * @brief DefaultControlHeight 控件的默认高度
     * @return
     */
    uint DefaultControlHeight() const;
    void setDefaultControlHeight(uint defaultControlHeight);

    /**
     * @brief DefaultControlPadding 控件默认的padding
     * @return
     */
    QRectF DefaultControlPadding() const;
    void setDefaultControlPadding(const QRectF &defaultControlPadding);

    /**
     * @brief DefaultInputPadding 输入控件默认的padding
     * @return
     */
    QRectF DefaultInputPadding() const;
    void setDefaultInputPadding(const QRectF &defaultInputPadding);

    /**
     * @brief DefaultCornerRadius 控件默认的圆角
     * @return
     */
    QRectF DefaultCornerRadius() const;
    void setDefaultCornerRadius(const QRectF &defaultCornerRadius);

    /**
     * @brief DefaultIconHeight 图标默认的高度
     * @return
     */
    uint DefaultIconHeight() const;
    void setDefaultIconHeight(uint DefaultIconHeight);

    /**
     * @brief DefaultFontSize 默认字体大小
     * @return
     */
    uint DefaultFontSize() const;
    void setDefaultFontSize(uint DefaultFontSize);

    /**
     * @brief MiddleFontSize 中等字体大小
     * @return
     */
    uint MiddleFontSize() const;
    void setMiddleFontSize(uint middleFontSize);

    /**
     * @brief LargeFontSize 大号字体大小
     * @return
     */
    uint LargeFontSize() const;
    void setLargeFontSize(uint largeFontSize);

    /**
     * @brief DefaultScrollerWidth QTabBar::scroller的默认宽度（一般为DefaultControlHeight的2倍）
     * @return
     */
    uint DefaultScrollerWidth() const;
    void setDefaultScrollerWidth(uint defaultScrollerWidth);

private:

    GlobalSizes();

    void loadConstProperty();

private:

    static GlobalSizes *m_instance;

    QHash<QString, QVariant> m_allConstPropertys;

    uint m_defaultMarginTop = 8;
    uint m_defaultControlHeight = 36;
    uint m_defaultIconHeight = 22;
    uint m_defaultFontSize = 12;
    uint m_middleFontSize = 16;
    uint m_largeFontSize = 20;
    uint m_defaultScrollerWidth = 72;

    QRectF m_defaultControlPadding = QRectF(10, 0, 10, 0);
    QRectF m_defaultInputPadding = QRectF(8, 0, 8, 0);
    QRectF m_defaultCornerRadius = QRectF(4, 4, 4, 4);

};

#endif // GLOBALSIZES_H
