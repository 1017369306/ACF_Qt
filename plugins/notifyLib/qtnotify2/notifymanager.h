#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QtCore>

class NotifyWnd;
class NotifyCountWnd;
class NotifyManager : public QObject
{
    Q_OBJECT

public:
    explicit NotifyManager( QObject *parent = 0);

    void notify(const QString &title, const QString &body, const QVariantMap &data = QVariantMap());

    void setMaxCount(int count);

    int displayTime() const;
    void setDisplayTime(int displayTime);

    int animateTime() const;
    void setAnimateTime(int animateTime);

    int spacing() const;
    void setSpacing(int spacing);

    QPoint cornerPos() const;
    void setCornerMargins(int right, int bottom);

    QSize notifyWndSize() const;
    void setNotifyWndSize(int width, int height);

    QString defaultIcon() const;
    void setDefaultIcon(const QString &defaultIcon);

    QString styleSheet(const QString &theme = "default") const;
    void setStyleSheet(const QString &styleSheet, const QString &theme = "default");

    void setShowQueueCount(bool isShowQueueCount);

    QSize iconSize() const;
    void setIconSize(const QSize &iconSize);

signals:
    void notifyDetail(const QVariantMap &data);

private:
    void showNext();
    void showQueueCount();

    QQueue<QVariantMap> m_dataQueue;
    QList<NotifyWnd *> m_notifyList;
    NotifyCountWnd *m_notifyCount;

    int m_maxCount;
    bool m_isShowQueueCount;
    int m_displayTime;
    int m_animateTime;
    int m_spacing;
    QPoint m_cornerPos;
    QSize m_notifyWndSize;
    QString m_defaultIcon;
    QMap<QString, QString> m_styleSheets;

    QSize m_iconSize;

};

#endif // NOTIFYMANAGER_H
