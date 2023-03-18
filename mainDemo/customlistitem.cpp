#include "customlistitem.h"

CustomListItem::CustomListItem(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hlay = new QHBoxLayout;
    m_icon = new QPushButton();
    m_icon->setProperty("isIconTransparent", true);
    m_text = new QLabel();
    m_icon1 = new QPushButton();
    m_icon1->setProperty("isIconTransparent", true);
    m_icon->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_icon->setAttribute(Qt::WA_TranslucentBackground);
    m_icon1->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_icon1->setAttribute(Qt::WA_TranslucentBackground);
    m_text->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_text->setAttribute(Qt::WA_TranslucentBackground);

    hlay->addWidget(m_icon);
    hlay->addWidget(m_text);
    hlay->addWidget(m_icon1);
    hlay->addStretch(1);

//    hlay->setSpacing(4);
    hlay->setContentsMargins(6,0,6,0);
    this->setLayout(hlay);
//    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

CustomListItem::~CustomListItem(){

}

void CustomListItem::setInfo(const ListItemProperty &info){
    m_text->setText(info.text);
    if(!info.icon.isEmpty()){
        m_icon->setIcon(QIcon(info.icon));
    }
    m_icon->setVisible(!info.icon.isEmpty());
    if(!info.icon1.isEmpty()){
        m_icon1->setIcon(QIcon(info.icon1));
    }
    m_icon1->setVisible(!info.icon1.isEmpty());
    this->setToolTip(info.toolTip);
}
