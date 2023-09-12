/****************************************************************************
**
** Qtitan Library by Developer Machines (Fluent Design Style implementation for Qt.C++)
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

#ifndef QTN_FLUENTDESIGNSTYLE_H
#define QTN_FLUENTDESIGNSTYLE_H

#include "QtnCommonStyle.h"

QTITAN_BEGIN_NAMESPACE

class ColorizedStylePrivate;
class QTITAN_EXPORT ColorizedStyle : public CommonStyle
{
    Q_OBJECT
    Q_ENUMS(Theme)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme)
public:
    enum Theme
    {
        Light,
        Dark,
        Blue,
        LightBlue,
        Red,
        Custom
    };
public:
    ColorizedStyle();
    virtual ~ColorizedStyle();
public:
    ColorizedStyle::Theme theme() const;
    void setTheme(ColorizedStyle::Theme theme);
    const QColor& windowColor() const;
    const QColor& windowTextColor() const;
    const QColor& materialColor() const;
    const QColor& tileMenuColor() const;
    const QColor& menuColor() const;
    const QColor& linkColor() const;
    const QColor& itemColor() const;
    const QColor& itemTextColor() const;
    const QColor& captionColor() const;
public Q_SLOTS:
    void setWindowColor(const QColor& color);
    void setWindowTextColor(const QColor& color);
    void setMaterialColor(const QColor& color);
    void setTileMenuColor(const QColor& color);
    void setMenuColor(const QColor& color);
    void setLinkColor(const QColor& color);
    void setItemColor(const QColor& color);
    void setItemTextColor(const QColor& color);
    void setCaptionColor(const QColor& color);
public:
    void polish(QPalette& palette) override;
    void polish(QWidget* widget) override;
public:
    QTN_DECLARE_EX_PRIVATE(ColorizedStyle)
    Q_DISABLE_COPY(ColorizedStyle)
};

#if 0
class QTITAN_EXPORT FluentDesignStyle : public ColorizedStyle
{
public:
};
#endif

QTITAN_END_NAMESPACE

#endif // QTN_FLUENTDESIGNSTYLE_H
