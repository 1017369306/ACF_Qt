#include "customdockwindowbase.h"

#include <QBuffer>
#include <QDir>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QStyleFactory>

static const QString DockBarSetting = QStringLiteral("DockBarLayout");
static const QString DockPanelSetting = QStringLiteral("DockPanelLayout");

CustomDockWindowBase::CustomDockWindowBase(QWidget *parent) :
    DockMainWindow(parent)
{

}

CustomDockWindowBase::~CustomDockWindowBase()
{

}

void CustomDockWindowBase::styleChanged(QAction* action)
{
//    AppStyle appStyle = Default;
//    if (m_fusionStyleAction == action)
//        appStyle = Fusion;
//    else if (m_themeBlueAction == action)
//        appStyle = VS2019Blue;
//    else if (m_themeLightAction == action)
//        appStyle = VS2019Light;
//    else if (m_themeDarkAction == action)
//        appStyle = VS2019Dark;
//    setCustomStyle(appStyle);
}

void CustomDockWindowBase::styleIndexChanged(int index)
{
//    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
//    Q_ASSERT(comboBox != Q_NULL);
//    const QString caption = comboBox->itemText(index);
//    qApp->setStyle(QStyleFactory::create(caption));
}

void CustomDockWindowBase::about()
{
//    AboutDialog::show(this, tr("About QtitanDocking Component"), tr("QtitanDocking"), QStringLiteral(QTN_VERSION_DOCKING_STR));
}

void CustomDockWindowBase::saveToolBarToXML()
{
    const QString path = QDir::currentPath() + QStringLiteral("/toolbar-layout");
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save ToolBar's Layout to XML File"), path, tr("Config Files (*.xml)"));
    if (fileName.isEmpty())
        return;
    dockBarManager()->saveStateToFile(fileName);
}

void CustomDockWindowBase::loadToolBarFromXML()
{
    const QString path = QDir::currentPath() + QStringLiteral("/");
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open ToolBar's Layout XML File"), path, tr("Config Files (*.xml)"));
    if (fileName.isEmpty())
        return;
    dockBarManager()->loadStateFromFile(fileName);
}

void CustomDockWindowBase::savePanelToXML()
{
    const QString path = QDir::currentPath() + QStringLiteral("/panel-layout");
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Panel's Layout to XML File"), path, tr("Config Files (*.xml)"));
    if (fileName.isEmpty())
        return;
    dockPanelManager()->saveStateToFile(fileName);
}

void CustomDockWindowBase::loadPanelFromXML()
{
    const QString path = QDir::currentPath() + QStringLiteral("/");
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Panel's Layout XML File"), path, tr("Config Files (*.xml)"));
    if (fileName.isEmpty())
        return;
    dockPanelManager()->loadStateFromFile(fileName);
}

void CustomDockWindowBase::loadDockPanelSettings(const QSettings& settings)
{
    QByteArray data = settings.value(DockPanelSetting).toByteArray();
    if (!data.isEmpty())
    {
        QBuffer buffer(&data);
        if (!buffer.open(QIODevice::ReadOnly))
            Q_ASSERT(false);
        loadStateFromDevice(Q_NULL, &buffer);
    }
}

void CustomDockWindowBase::saveDockPanelSettings(QSettings& settings)
{
    QByteArray data;
    QBuffer buffer(&data);
    if (!buffer.open(QIODevice::WriteOnly))
        Q_ASSERT(false);
    saveStateToDevice(Q_NULL, &buffer);
    settings.setValue(DockPanelSetting, QVariant(data));
}

void CustomDockWindowBase::readSettings()
{
//    QSettings settings(this);
//    bool autoSave = settings.value(QStringLiteral("autosave"), false).toBool();
//    m_autoSaveAction->setChecked(autoSave);
//    if (!autoSave)
//        return;
//    AppStyle appStyle = static_cast<AppStyle>(settings.value(QStringLiteral("style"), static_cast<int>(Default)).toInt());
//    setCustomStyle(appStyle);

//    QByteArray byte = settings.value(QStringLiteral("geometry")).toByteArray();
//    if (!byte.isEmpty())
//        restoreGeometry(byte);

//    loadDockToolBarSettings(settings);
//    loadDockPanelSettings(settings);
}

void CustomDockWindowBase::writeSettings()
{
//    QSettings settings(this);
//    settings.setValue(QStringLiteral("autosave"), m_autoSaveAction->isChecked());
//    if (!m_autoSaveAction->isChecked())
//    {
//        settings.remove(QStringLiteral("style"));
//        settings.remove(QStringLiteral("geometry"));
//        settings.remove(DockBarSetting);
//        settings.remove(DockPanelSetting);
//        return;
//    }
//    settings.setValue(QStringLiteral("style"), QString::number(static_cast<int>(m_appStyle)));
//    QByteArray byte = saveGeometry();
//    settings.setValue(QStringLiteral("geometry"), byte);
//    saveDockToolBarSettings(settings);
//    saveDockPanelSettings(settings);
}

