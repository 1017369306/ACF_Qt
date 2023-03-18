#ifndef QUERYITEM_H
#define QUERYITEM_H

#include <QWidget>

enum QueryRelation{
    //并且
    And=0,
    //或者
    Or
};

//QStringList equalItems{"=", "!=", ">", ">=", "<", "<=", "包含", "不包含"};
enum QueryCompare{
    Equal=0,
    NotEqual,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,
    Contains,
    NotContains
};

/**
 * @brief The QueryCondition struct 查询条件
 */
struct QueryCondition
{
    QueryRelation queryRelation;
    QString key;
    QueryCompare queryCompare;
    QString value;
};

namespace Ui {
class queryItem;
}

class queryItem : public QWidget
{
    Q_OBJECT

public:
    explicit queryItem(QWidget *parent = nullptr);
    ~queryItem();

    /**
     * @brief setRelationVisible 设置关系下拉框是否可见，默认不可见
     * @param visible
     */
    void setRelationVisible(bool visible);

    /**
     * @brief getQueryCondition 获取当前的查询条件
     * @return
     */
    QueryCondition getQueryCondition();

    /**
     * @brief reset 重置此查询条件
     * @param onlyResetValue 是否仅重置值
     */
    void reset(bool onlyResetValue = true);

    QList<QString> getKeys() const;
    void setKeys(const QList<QString> &keys);

private:

    void slot_closeClicked(bool checked = false);

private:

    void init();

    void initIcon();

private:
    Ui::queryItem *ui;

    QList<QString> m_keys;

};

#endif // QUERYITEM_H
