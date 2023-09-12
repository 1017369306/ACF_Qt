#ifndef CUSTOMDOCKWINDOWBASE_H
#define CUSTOMDOCKWINDOWBASE_H

#include <QWidget>
#include <QSettings>

#include <DevMachines/QtitanBase>
#include <DevMachines/QtitanDocking>
#include <DevMachines/QtitanStyle>

class CustomDockWindowBase : public DockMainWindow
{
    Q_OBJECT

public:
    explicit CustomDockWindowBase(QWidget *parent = nullptr);
    ~CustomDockWindowBase();

private Q_SLOTS:
    void saveToolBarToXML();
    void loadToolBarFromXML();
    void savePanelToXML();
    void loadPanelFromXML();

private:
    void loadDockPanelSettings(const QSettings& settings);
    void saveDockPanelSettings(QSettings& settings);
    void readSettings();
    void writeSettings();

protected Q_SLOTS:
    void styleChanged(QAction* action);
    void styleIndexChanged(int index);
    void about();


};

#endif // CUSTOMDOCKWINDOWBASE_H
