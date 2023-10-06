//解决中文乱码
#pragma execution_character_set("utf-8")

#include "login.h"
#include "ui_login.h"

#include <QDebug>

Login::Login(QPixmap pixmap, QWidget *parent) : QSplashScreen(parent, pixmap),
    ui(new Ui::SplashScreen)
{
    ui->setupUi(this);
}

Login::~Login()
{
    if(m_loginThread != nullptr){
        m_loginThread->quit();
        m_loginThread->wait();
        m_loginThread->deleteLater();
    }
    //这里要手动stop，不然即使delete ui后，movie仍然占用cpu资源，造成卡顿
    if(ui->loginLabel->movie() != nullptr){
        ui->loginLabel->movie()->stop();
    }
    delete ui->loginLabel;
    if(ui->waveLabel->movie() != nullptr){
        ui->waveLabel->movie()->stop();
    }
    delete ui->waveLabel;

    delete ui;
}

void Login::init(){
    // 设置事件发送
    installEventFilter(this);

    // 创建一个QMovie对象，设置其源为GIF文件
    QMovie *loginMovie = new QMovie(":/appPics/login/login.gif");
    QSize loginSize(ui->loginLabel->height(), ui->loginLabel->height());
    loginMovie->setScaledSize(loginSize);
    ui->loginLabel->setMovie(loginMovie);
    ui->loginLabel->setAlignment(Qt::AlignCenter);
    loginMovie->start();

    // 线条绘制及文本显示
    QMovie *lineMovie = new QMovie(":/appPics/login/wave.gif");
    QSize waveSize(this->width() - loginSize.width(), ui->waveLabel->height());
    lineMovie->setScaledSize(waveSize);
    ui->waveLabel->setMovie(lineMovie);
    ui->waveLabel->setAlignment(Qt::AlignCenter);
    lineMovie->start();

    ui->companyLabel->setText("XXXXXX公司");
    ui->companyLabel->setAlignment(Qt::AlignCenter);

    ui->dynacommLabel->setText("XXXXXX软件");
    ui->dynacommLabel->setAlignment(Qt::AlignCenter);

    // 设置版本号及其他描述信息
    QString versionInfoStr = "V1.0.0.1\nCopyright © XXXXXXX XXXX";
    ui->versionLabel->setText(versionInfoStr);
    ui->versionLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    ui->numLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    ui->progressLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    //设置元素的样式
//    this->setProperty("transparent", true);
    ui->contentWidget->setProperty("transparent", true);
    ui->gifWidget->setProperty("transparent", true);
    ui->numLabel->setProperty("login", true);
    ui->progressLabel->setProperty("login", true);
    ui->versionLabel->setProperty("login", true);
    ui->dynacommLabel->setProperty("loginMiddle", true);
    ui->companyLabel->setProperty("loginLarge", true);

    m_loginThread = new LoginThread();
    m_loginThread->setStepMs(50);
    connect(m_loginThread, &LoginThread::currentProcessChanged, this, &Login::slot_currentProcessChanged);
    m_loginThread->start(QThread::TimeCriticalPriority);

    //    QtConcurrent::run(this, &SplashScreen::simulateLoad, 99);
    //    QObject::connect(LoadTool::instance(), &LoadTool::currentProcessChanged, &splash, &SplashScreen::slot_processChanged);

}

void Login::updateProcess(const int& value, const QString& message){
    // 更新进度条进度
    ui->progressBar->setValue(value);
    // 更新进度条读数
    ui->numLabel->setText(QString::number(value)+"%");

    // 进度信息描述
    if(!message.isEmpty()){
        ui->progressLabel->setText(message);
    }
}

void Login::slot_currentProcessChanged(const int& value){
    qDebug() << "currentProcessChanged threadID: " << QThread::currentThreadId() << " dateTime: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");

    QString message = "";
    if(m_processHash.contains(LoginTool::instance()->getMilestoneProcess())){
        message = m_processHash.value(LoginTool::instance()->getMilestoneProcess());
    }
    updateProcess(value, message);

    //强制立即处理所有的事件，达到立即更新UI的效果
    QCoreApplication::processEvents();

    if(value >= 100){
        this->finish(this->parentWidget());
    }
}

bool Login::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    // 使用事件过滤器，防止进度条在加载中，用户点击图片造成窗口被隐藏
    if(event->type()==QEvent::Type::MouseButtonPress || event->type()==QEvent::Type::MouseButtonDblClick)
    {
        return true;
    }
    return false;
}
