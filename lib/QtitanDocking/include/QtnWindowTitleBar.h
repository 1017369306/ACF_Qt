/****************************************************************************
**
** Qtitan Library by Developer Machines (Components for Qt.C++)
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


#ifndef QTN_WINDOWTITLEBAR_H
#define QTN_WINDOWTITLEBAR_H

#include <QWidget>
#include <QStyleOptionTitleBar>
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class WindowTitleBarPrivate;

class QTITAN_BASE_EXPORT WindowTitleBar : public QObject
{
    Q_OBJECT
protected:
    WindowTitleBar(QWidget* window);
public:
    enum SysButtonKind
    {
        SysButtonHidden,
        SysMenuButton,
        BackButton
    };
    enum WidgetAlign
    {
        AlignLeft,
        AlignRight,
        AlignClient
    };
    static WindowTitleBar* get(QWidget* window);
    static WindowTitleBar* find(const QWidget* window);
    virtual ~WindowTitleBar();
public:
    void setVisible(bool visible);
    bool isVisible() const;
    void setResizible(bool resizible);
    bool isResizible() const;
    void show();
    void hide();
    void update();
    void setWidget(QWidget* widget, WidgetAlign align = AlignLeft);
    QWidget* widget() const;
    bool styledFrame() const;
    void setStyledFrame(bool styled);
    bool blurBehindWindowEnabled() const;
    void setBlurBehindWindowEnabled(bool enabled);
    bool extendViewIntoTitleBar() const;
    void setExtendViewIntoTitleBar(bool extend);
    void setTitleHeight(int height);
    int titleHeight() const;
    void setBorderThickness(int thickness);
    int borderThickness() const;
    bool isSysMenuButtonVisible() const;
    void setSysMenuButtonVisible(bool visible);
    SysButtonKind sysButtonKind() const;
    void setSysButtonKind(SysButtonKind kind);
    const QColor& backgroundColor() const;
    void setBackgroundColor(const QColor& color);
    const QColor& textColor() const;
    void setTextColor(const QColor& color);
    QColor frameColor() const;
    QPalette palette() const;
    void initStyleOption(QStyleOptionTitleBar* option) const;
    bool isQWizardSupports() const;
    void setQWizardSupports(bool supports) const;
    void removeAndDelete();
    QRect systemButtonsRect() const;
    static void startDrag(QWidget* window, const QPoint& pos);
Q_SIGNALS:
    void backRequested();
    void showHelp();
protected Q_SLOTS:
    void windowDestroyed();
private:
    friend class WindowTitleBarPrivate;
    friend class MaterialWidget;
    WindowTitleBarPrivate* m_p;
};

QTITAN_END_NAMESPACE

#endif //QTN_WINDOWTITLEBAR_H


