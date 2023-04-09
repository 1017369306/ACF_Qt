#include "frameworktool.h"

frameworkTool* frameworkTool::m_instance = nullptr;
QString frameworkTool::frameworkTheme = ":/theme/qss/theme.css";
QStringList frameworkTool::customCssList = QStringList();
QString frameworkTool::appCss = "";

double frameworkTool::m_currentPixWidth = 0;
double frameworkTool::m_currentPixHeight = 0;

frameworkTool::frameworkTool(QObject *parent) : QObject(parent)
{

}

frameworkTool::~frameworkTool()
{

}

QStringList frameworkTool::getCustomCssList()
{
    return customCssList;
}

void frameworkTool::setCustomCssList(const QStringList &value)
{
    customCssList = value;
}

QString frameworkTool::getAppCss()
{
    return appCss;
}
