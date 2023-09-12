#include "mainview.h"
#include "ui_mainview.h"

MainView::MainView(QWidget *parent) :
    ModuleBase(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);

    this->init();
    this->connectAllSignal();
}

MainView::~MainView()
{
    delete ui;
}

void MainView::disModule(){

}

/**
 * @brief init 初始化UI
 */
void MainView::init(){
#if defined(__MINGW32__)
    axWidget = new QAxWidget();
    //此处的{8856F961-340A-11D0-A96B-00C04FD705A2}就是Microsoft Web Browser控件的UUID
    axWidget->setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}"));
    axWidget->setObjectName("web 浏览器");//设置控件的名称
    axWidget->setFocusPolicy(Qt::StrongFocus);//设置控件接收键盘焦点的方式：鼠标单击、Tab键
    axWidget->setProperty("DisplayAlerts", false); //不显示任何警告信息。
    axWidget->setProperty("DisplayScrollBars", true); // 显示滚动条
    ui->mainVerticalLayout->addWidget(axWidget, 1);
    axWidget->dynamicCall("Navigate(const QString&)", "www.baidu.com");//显示网页
#else
    m_view = new QWebEngineView();
    ui->mainVerticalLayout->addWidget(m_view, 1);

    m_webPage = new QWebEnginePage(this);
    //m_webPage->setGeometry(this->frameGeometry().width()*0.1,this->frameGeometry().height()*0.1,this->frameGeometry().width()/8,this->frameGeometry().height()/8);
    m_webPage->setUrl(QUrl("http://www.baidu.com"));
//    m_webPage->load(QUrl("http://www.baidu.com")

    m_view->setPage(m_webPage);
    m_view->show();

#endif
}

/**
 * @brief connectAllSignal 建立所有信号和槽
 */
void MainView::connectAllSignal(){
#if defined(__MINGW32__)
    if(axWidget != nullptr){

    }
#else
    if(m_webPage != nullptr){
        connect(m_webPage, &QWebEnginePage::loadFinished, this, &MainView::slot_loadFinished);
    }

#endif
}

void MainView::slot_loadFinished(bool ok){

}
