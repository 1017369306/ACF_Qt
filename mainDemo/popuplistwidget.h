#ifndef POPUPLISTWIDGET_H
#define POPUPLISTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVariant>
#include <QListWidget>
#include "customlistitem.h"

#define CUSTOMDATA "customData"

class PopupListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PopupListWidget(QWidget *parent = nullptr);
    ~PopupListWidget();

    QListWidget *getListWidget() const { return m_listWidget;}

    void setItems(const QList<QString> &items);

    void setItems(const QList<ListItemProperty> &items);

public:
signals:
    /**
     * @brief selectedItem 发出选中项的信号
     * @param property 选中项的信息
     */
    void selectedItem(const int &index, const QVariant property);

protected:

    void slot_currentRowChanged(int currentRow);

private:

    void initUI();

    void clearItems();

private:

    QListWidget *m_listWidget = nullptr;

};


#endif // POPUPLISTWIDGET_H
