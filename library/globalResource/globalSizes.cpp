#include "globalSizes.h"
#include <QtMath>

GlobalSizes *GlobalSizes::m_instance = nullptr;

const uint GlobalSizes::Const_PixWidth = 1280;
const uint GlobalSizes::Const_PixHeight = 1024;

const uint GlobalSizes::Const_DefaultControlHeight = 36;

const uint GlobalSizes::Const_LeftTopX = 72;
const uint GlobalSizes::Const_LeftTopY = 27;

GlobalSizes::GlobalSizes(){
    this->loadConstProperty();
}

void GlobalSizes::loadConstProperty(){
    if(m_allConstPropertys.size() == 0){
        const QMetaObject *metaObj = this->metaObject();
        if(metaObj != nullptr){
            int propertyCount = metaObj->propertyCount();
            int propertyOffset = metaObj->propertyOffset();
            for(int i = propertyOffset; i < propertyCount; i++)
            {
                QMetaProperty property = metaObj->property(i);
                const char *name = property.name();
//                QVariant value = this->property(name);
                QVariant value = property.read(this);
                this->m_allConstPropertys.insert(name, value);
            }
        }
    }
}

void GlobalSizes::updateSize(const double &rate){
    if(m_allConstPropertys.size() > 0){
        //        const QMetaObject *metaObj = this->metaObject();
        foreach (QString key, m_allConstPropertys.keys()) {
            QVariant oldValue = m_allConstPropertys.value(key);
            QVariant newValue;
//            QMetaProperty property = metaObj->property(metaObj->indexOfProperty(key.toLatin1()));
            switch (oldValue.type()) {
            case QVariant::Type::Double:
            case QVariant::Type::Int:
            case QVariant::Type::UInt:
            case QVariant::Type::LongLong:
            case QVariant::Type::ULongLong:
            {
                double doubleValue = oldValue.toDouble();
                //qss样式中，有些属性的值必须为int才有效，这里全部都用取小的（qFloor）int。
                newValue = qFloor(doubleValue * rate);
                break;
            }
            case QVariant::Type::RectF:
            {
                QRectF rect = oldValue.toRectF();
                newValue = QRectF(qFloor(rect.x() * rate), qFloor(rect.y() * rate), qFloor(rect.width() * rate), qFloor(rect.height() * rate));
                break;
            }
            default:
            {
                break;
            }
            }
            this->setProperty(key.toLatin1(), newValue);
        }
    }
}

uint GlobalSizes::DefaultMarginTop() const
{
    return m_defaultMarginTop;
}

void GlobalSizes::setDefaultMarginTop(const uint &defaultMarginTop)
{
    m_defaultMarginTop = defaultMarginTop;
}

uint GlobalSizes::DefaultControlHeight() const
{
    return m_defaultControlHeight;
}

void GlobalSizes::setDefaultControlHeight(uint DefaultControlHeight)
{
    m_defaultControlHeight = DefaultControlHeight;
}

QRectF GlobalSizes::DefaultControlPadding() const
{
    return m_defaultControlPadding;
}

void GlobalSizes::setDefaultControlPadding(const QRectF &defaultControlPadding)
{
    m_defaultControlPadding = defaultControlPadding;
}

QRectF GlobalSizes::DefaultInputPadding() const
{
    return m_defaultInputPadding;
}

void GlobalSizes::setDefaultInputPadding(const QRectF &defaultInputPadding)
{
    m_defaultInputPadding = defaultInputPadding;
}

QRectF GlobalSizes::DefaultCornerRadius() const
{
    return m_defaultCornerRadius;
}

void GlobalSizes::setDefaultCornerRadius(const QRectF &defaultCornerRadius)
{
    m_defaultCornerRadius = defaultCornerRadius;
}

uint GlobalSizes::DefaultIconHeight() const
{
    return m_defaultIconHeight;
}

void GlobalSizes::setDefaultIconHeight(uint defaultIconHeight)
{
    m_defaultIconHeight = defaultIconHeight;
}

uint GlobalSizes::DefaultFontSize() const
{
    return m_defaultFontSize;
}

void GlobalSizes::setDefaultFontSize(uint defaultFontSize)
{
    m_defaultFontSize = defaultFontSize;
}

uint GlobalSizes::MiddleFontSize() const
{
    return m_middleFontSize;
}

void GlobalSizes::setMiddleFontSize(uint middleFontSize)
{
    m_middleFontSize = middleFontSize;
}

uint GlobalSizes::LargeFontSize() const
{
    return m_largeFontSize;
}

void GlobalSizes::setLargeFontSize(uint largeFontSize)
{
    m_largeFontSize = largeFontSize;
}

uint GlobalSizes::DefaultScrollerWidth() const
{
    return m_defaultScrollerWidth;
}

void GlobalSizes::setDefaultScrollerWidth(uint defaultScrollerWidth)
{
    m_defaultScrollerWidth = defaultScrollerWidth;
}
