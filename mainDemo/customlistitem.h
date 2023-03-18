#ifndef CUSTOMLISTITEM_H
#define CUSTOMLISTITEM_H

#include <QObject>
#include <QPushButton>
#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QPushButton>

struct ListItemProperty
{
public:

    ListItemProperty(){}
    ListItemProperty(const QString &text, const QString &toolTip = "", const QString &icon = "", const QString &icon1 = "", QVariant customData = QVariant::Invalid)
    {
        this->text = text;
        this->toolTip = toolTip;
        this->icon = icon;
        this->icon1 = icon1;
        this->customData = customData;
    }

//    QString text() const {return m_text;}

//    QString icon() const {return m_icon;}

//    QString icon1() const {return m_icon1;}

//    QString toolTip() const {return m_toolTip;}

//    QVariant customData() const {return m_customData;}

    QString text;
    QString icon;
    QString icon1;
    QString toolTip;
    QVariant customData;

};

class CustomListItem : public QWidget
{
    Q_OBJECT
public:

    explicit CustomListItem(QWidget *parent = nullptr);
    ~CustomListItem();

    void setInfo(const ListItemProperty &info);

    QPushButton *icon() const {return m_icon;}
    QPushButton *icon1() const {return m_icon1;}

private:

    QPushButton *m_icon = nullptr;
    QLabel *m_text = nullptr;
    QPushButton *m_icon1 = nullptr;

};

#endif // CUSTOMLISTITEM_H
