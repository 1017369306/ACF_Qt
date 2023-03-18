#ifndef QUERYHEADER_H
#define QUERYHEADER_H

#include <QWidget>
#include <queryItem.h>

namespace Ui {
class queryHeader;
}

class queryHeader : public QWidget
{
    Q_OBJECT

public:
    explicit queryHeader(QWidget *parent = nullptr);
    ~queryHeader();

public:
    QList<QString> getKeys() const;
    void setKeys(const QList<QString> &keys);

signals:

    void query(const QList<QueryCondition> &conditions);

private:

    void slot_addClicked(bool checked = false);
    void slot_minusClicked(bool checked = false);
    void slot_queryClicked(bool checked = false);
    void slot_resetClicked(bool checked = false);

private:

    void init();

    void initIcon();

    /**
     * @brief getQueryConditions 获取当前的查询条件
     * @return
     */
    QList<QueryCondition> getQueryConditions();

private:
    Ui::queryHeader *ui;

    QList<QString> m_keys;

};

#endif // QUERYHEADER_H
