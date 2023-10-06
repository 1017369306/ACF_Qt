#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include <QWidget>
#include <QMovie>
#include <QTime>
#include <loginthread.h>

namespace Ui {
class SplashScreen;
}

/**
 * @brief 创建启动画面的类
 */
class Login : public QSplashScreen
{
    Q_OBJECT

public:
    explicit Login(QPixmap pixmap, QWidget *parent = nullptr);
    ~Login();

    /**
     * @brief 初始化元素布局（必须在设置Size和show后调用）
     */
    void init();

public:

    /**
     * @brief 获取所有进度-提示消息的键值对
     * @return
     */
    QHash<int, QString> getProcessHash() const {return m_processHash;}
    /**
     * @brief 设置所有进度-提示消息的键值对
     * @param 所有进度-提示消息的键值对
     */
    void setProcessHash(const QHash<int, QString>& value) {
        m_processHash = value;
    }

//    void simulateLoad(int secs);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void updateProcess(const int& value, const QString& message);

private slots:
    void slot_currentProcessChanged(const int& value);

private:
    Ui::SplashScreen *ui;

    QHash<int, QString> m_processHash;
    LoginThread *m_loginThread = nullptr;

};

#endif // SPLASHSCREEN_H
