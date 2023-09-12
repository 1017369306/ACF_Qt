/****************************************************************************
**
** Qtitan Library by Developer Machines (Dock Windows and Bars component for Qt.C++)
**
** Copyright (c) 2009-2021 Developer Machines (https://www.devmachines.com)
**           ALL RIGHTS RESERVED
**
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and
**  criminal penalties and will be prosecuted to the maximum extent
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/

#ifndef QTN_DOCKWIDGETPANEL_H
#define QTN_DOCKWIDGETPANEL_H

#include <QIcon>
#include "QtnDockPanelBase.h"

class QMenu;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QStyleOptionFrame;
#else
class QStyleOptionFrameV3;
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

QTITAN_BEGIN_NAMESPACE

class DockWidgetPanelPrivate;
/* DockWidgetPanel */
class QTITAN_EXPORT DockWidgetPanel : public DockPanelBase
{
    Q_OBJECT
    Q_FLAGS(DockPanelFeatures)
    Q_PROPERTY(DockPanelFeatures features READ features WRITE setFeatures NOTIFY featuresChanged)
    Q_PROPERTY(Qtitan::DockPanelAreas allowedAreas READ allowedAreas WRITE setAllowedAreas NOTIFY allowedAreasChanged)
    Q_PROPERTY(QString caption READ caption WRITE setCaption)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool closed READ isClosed)
    Q_PROPERTY(bool autoHide READ isAutoHide)
    Q_PROPERTY(bool isFloat READ isFloat WRITE setFloat)
public:
    explicit DockWidgetPanel(DockPanelManager* manager);
    explicit DockWidgetPanel(const QString& caption, DockPanelManager* manager);
    virtual ~DockWidgetPanel();
public:
    const QString& caption() const;

    QWidget* widget() const;
    void setWidget(QWidget* widget);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QMenu* menuButton() const;
    void setMenuButton(QMenu* menu);

    enum DockPanelFeature 
    {
        DockPanelClosable   = 0x0001,
        DockPanelHideable   = 0x0002,
        DockPanelFloatable  = 0x0004,
        DockPanelNoCaption   = 0x0008,
        DockPanelMenuButton = 0x0010,

        DockPanelDockable   = 0x0020,
        DockPanelFloatableByTabDoubleClick = 0x0040,
        DockPanelFloatableByCaptionDoubleClick = 0x0080,
        DockPanelHoverShow = 0x0400,

        DockPanelFeatureMask = 0x0fff,
        AllDockPanelFeatures = DockPanelClosable | DockPanelHideable | DockPanelFloatable,
        NoDockPanelFeatures = 0x0000,
    };
    Q_DECLARE_FLAGS(DockPanelFeatures, DockPanelFeature)
    Q_FLAG(DockPanelFeatures)

    void setFeatures(DockPanelFeatures features);
    DockPanelFeatures features() const;

    bool isValid() const;

    void showPanel();
    bool isClosed() const;
    void closePanel();

    bool isAutoHide() const;
    void setAutoHide(bool set);

    bool isActive() const;
    void activate();
    void deactivate();

    bool isFloat() const;
    void setFloat(bool set);

    Qtitan::DockPanelAreas allowedAreas() const;
    bool isAreaAllowed(Qtitan::DockPanelAreas area) const;
    void setAllowedAreas(Qtitan::DockPanelAreas areas);

    QAction* visibleAction() const;
    QWidget* titleBar() const;
    void setTitleBar(QWidget* widget);
public:
    virtual void setParentPanel(DockPanelBase* panelBase);
public Q_SLOTS:
    void setCaption(const QString& caption);
Q_SIGNALS:
    void featuresChanged(DockWidgetPanel::DockPanelFeatures features);
    void allowedAreasChanged(Qtitan::DockPanelAreas allowedAreas);
protected:
    DockWidgetPanel(DockWidgetPanelPrivate& dd);
protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
protected:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    void initStyleOption(QStyleOptionFrame* option) const;
#else
    void initStyleOption(QStyleOptionFrameV3* option) const;
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
private:
    QTN_DECLARE_EX_PRIVATE(DockWidgetPanel);
    Q_DISABLE_COPY(DockWidgetPanel);
};

QTITAN_END_NAMESPACE

Q_DECLARE_OPERATORS_FOR_FLAGS(DockWidgetPanel::DockPanelFeatures)

#endif //QTN_DOCKWIDGETPANEL_H
