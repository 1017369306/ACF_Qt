#ifndef NOTIFYWND_H
#define NOTIFYWND_H

#include "arrangedwnd.h"
#include <QLabel>
#include <QPushButton>

// 提示框
class NotifyWnd : public ArrangedWnd
{
    Q_OBJECT

public:
    /**
     * @brief 弹窗等级
     */
    enum NotifyLevel
    {
        InfoLevel = 0,
        SuccessLevel,
        WarnLevel,
        ErrorLevel,
        FatalLevel
    };
    Q_ENUM(NotifyLevel)

    explicit NotifyWnd(NotifyManager *manager, QWidget *parent = 0);

    QVariantMap data() const;
    void setData(const QVariantMap &data);

    NotifyLevel level() const {return m_level;}

    QSize iconSize() const;
    void setIconSize(const QSize &iconSize);

private:

    virtual void leaveEvent(QEvent *event);

private:
    QVariantMap m_data;
    QFrame *background;
    QLabel *iconLabel;
    QLabel *titleLabel;
    QLabel *bodyLabel;
    QPushButton *closeBtn;

    bool m_timerWorking = false;
    NotifyLevel m_level = NotifyLevel::InfoLevel;

    QSize m_iconSize;

};

// 队列提示数
class NotifyCountWnd : public ArrangedWnd
{
    Q_OBJECT

public:
    explicit NotifyCountWnd(NotifyManager *manager, QWidget *parent = 0);
    void setCount(int count);

    QSize iconSize() const;
    void setIconSize(const QSize &iconSize);

private:
    QLabel *iconLabel;
    QLabel *countLabel;
    QPropertyAnimation *flickerAnim;

    QSize m_iconSize;

};

#endif // NOTIFYWND_H
