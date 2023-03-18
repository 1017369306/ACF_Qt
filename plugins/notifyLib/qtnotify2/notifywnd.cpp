#include "notifywnd.h"
#include "notifymanager.h"
#include <QBoxLayout>
#include <QGraphicsDropShadowEffect>

NotifyWnd::NotifyWnd (NotifyManager *manager, QWidget *parent)
    : ArrangedWnd(manager, parent)
{
    background = new QFrame(this);
    background->setGeometry(3, 3, width()-6, height()-6);
    background->setObjectName("notify-background");

    QHBoxLayout *mainLayout = new QHBoxLayout(background);
    QVBoxLayout *contentLayout = new QVBoxLayout();
    QHBoxLayout *titleLayout = new QHBoxLayout();

    iconLabel = new QLabel(background);
    iconLabel->setFixedSize(40, 40);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setWordWrap(true);

    titleLabel = new QLabel(background);
    titleLabel->setObjectName("notify-title");

    bodyLabel = new QLabel(background);
    bodyLabel->setObjectName("notify-body");
    bodyLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    bodyLabel->setWordWrap(true);

    closeBtn = new QPushButton("");
    closeBtn->setObjectName("notify-close-btn");
//    closeBtn->setFixedSize(24, 24);
//    closeBtn->move(background->width() - closeBtn->width(), 0);
    connect(closeBtn, &QPushButton::clicked, this, &QObject::deleteLater);

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch(1);
    titleLayout->addWidget(closeBtn);

    contentLayout->addLayout(titleLayout);
    contentLayout->addWidget(bodyLabel);
    contentLayout->setStretch(0,1);

    mainLayout->addWidget(iconLabel);
    mainLayout->addLayout(contentLayout);
    mainLayout->setAlignment(iconLabel, Qt::AlignTop);

    setStyleSheet(m_manager->styleSheet());

    // 边框阴影
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setBlurRadius(5);
    background->setGraphicsEffect(shadow);

    connect(this, &ArrangedWnd::visibleChanged, [this](bool visible){
        if (visible)
        {
            m_timerWorking = true;
            //仅有错误和严重消息才取消自动隐藏，其他消息到时间了自动隐藏
            if(m_level <= 2){
                int displayTime = m_data.value("displayTime", m_manager->displayTime()).toInt();
                QTimer::singleShot(displayTime, this, [this](){
                    //张洋 20221226 增加一个判断，当鼠标在此消息窗体上时，到时间了不隐藏此消息窗体
                    if(!this->underMouse())
                    {
                        showArranged(0);
                    }
                    m_timerWorking = false;
                });
            }
        }
        else deleteLater();
    });
}

void NotifyWnd::leaveEvent(QEvent *event){
    //离开后再次调用上面的QTimer::singleShot开始隐藏倒计时
    if (!m_timerWorking && isVisible())
    {
        emit visibleChanged(true);
    }
}

QSize NotifyWnd::iconSize() const
{
    return m_iconSize;
}

void NotifyWnd::setIconSize(const QSize &iconSize)
{
    m_iconSize = iconSize;
}

QVariantMap NotifyWnd::data() const
{
    return m_data;
}

// 需显示后调用
void NotifyWnd::setData(const QVariantMap &data)
{
    m_data = data;

    QVariant levelTemp = m_data.value("level");
    if(levelTemp.isValid()){
        bool isInt = false;
        int level = levelTemp.toInt(&isInt);
        if(isInt){
            m_level = (NotifyLevel)level;
        }
    }

    QPixmap icon;
    QVariant iconv = data.value("icon");
    if (iconv.type() == QVariant::Pixmap) icon = iconv.value<QPixmap>();
    if (iconv.type() == QVariant::String) icon = QPixmap(iconv.toString());
    else icon = QPixmap(m_manager->defaultIcon());
    icon = icon.scaled(m_iconSize, Qt::KeepAspectRatio);
    iconLabel->setPixmap(icon);

    QString title = data.value("title").toString();
    titleLabel->setText(title);

    // 计算可显示行数及长度
    QString body = m_data.value("body").toString();
    QSize s1 = bodyLabel->size();
    QSize s2 = bodyLabel->fontMetrics().size(Qt::TextSingleLine, body);
    int linecount = s1.height()/s2.height();
    int charcount = qFloor(1.0*body.size()*s1.width()/s2.width()) * linecount;
    QString bodyElid = charcount > body.size() ? body : (body.left(charcount-1)+"…");
    bodyLabel->setText(bodyElid);

    if (data.contains("styleSheet"))
        setStyleSheet(data.value("styleSheet").toString());
    else if (data.contains("theme"))
        setStyleSheet(m_manager->styleSheet(data.value("theme").toString()));
}

NotifyCountWnd::NotifyCountWnd(NotifyManager *manager, QWidget *parent)
    : ArrangedWnd(manager, parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    iconLabel = new QLabel(this);
    iconLabel->setFixedSize(40, 40);
    iconLabel->setAlignment(Qt::AlignCenter);

    countLabel = new QLabel(this);
    countLabel->setObjectName("notify-count");
    countLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(countLabel);

    // 文字阴影
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(2, 2);
    shadow->setBlurRadius(5);
    setGraphicsEffect(shadow);

    setStyleSheet("#notify-count {"
                  "font: 20px Verdana;"
                  "color: #dd424d;"
                  "}");

    QPixmap icon = QPixmap(m_manager->defaultIcon());
    icon = icon.scaled(m_iconSize, Qt::KeepAspectRatio);
    iconLabel->setPixmap(icon);

    flickerAnim = new QPropertyAnimation(this, "windowOpacity", this);
    flickerAnim->setStartValue(1);
    flickerAnim->setKeyValueAt(0.25, 0.1);
    flickerAnim->setKeyValueAt(0.5, 1);
    flickerAnim->setEndValue(1);
    flickerAnim->setDuration(2000);
    flickerAnim->setLoopCount(-1);

    connect(this, &ArrangedWnd::visibleChanged, [this](bool visible){
        if (visible) flickerAnim->start();
        else flickerAnim->stop();
    });
}

void NotifyCountWnd::setCount(int count)
{
    countLabel->setNum(count);
}

QSize NotifyCountWnd::iconSize() const
{
    return m_iconSize;
}

void NotifyCountWnd::setIconSize(const QSize &iconSize)
{
    m_iconSize = iconSize;
}
