/****************************************************************************
**
** Qtitan Library by Developer Machines (Office Style implementation for Qt.C++)
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
#ifndef QTN_OFFICESTYLE_H
#define QTN_OFFICESTYLE_H

#include <QPalette>
#include <QStylePlugin>

#include "QtitanDef.h"
#include "QtnCommonStyle.h"

inline void initStyleResource()
{
    Q_INIT_RESOURCE(QtnAdobePhotoshopStyle);
    Q_INIT_RESOURCE(QtnWindows7ScenicStyle);
    Q_INIT_RESOURCE(QtnOffice2007Style);
    Q_INIT_RESOURCE(QtnOffice2010Style);
    Q_INIT_RESOURCE(QtnOffice2013Style);
    Q_INIT_RESOURCE(QtnOffice2016Style);
    Q_INIT_RESOURCE(QtnVisualStudio2019Style);
}

QTITAN_BEGIN_NAMESPACE

class OfficeStylePrivate;

class QTITAN_EXPORT OfficeStyle : public CommonStyle
{
    Q_OBJECT
    Q_PROPERTY(bool activeTabAccented READ isActiveTabAccented WRITE setActiveTabAccented)
    Q_PROPERTY(bool ignoreMDIWindowTitle READ isMDIWindowTitleIgnored WRITE setMDIWindowTitleIgnored)
public:
    enum OfficePopupDecoration
    {
        PopupSystemDecoration,
        PopupCompatibleDecoration,
        PopupOfficeFlatDecoration,
        PopupMSNDecoration
    };

    enum AccentColor
    {
        AccentColorBlue,
        AccentColorBrown,
        AccentColorGreen,
        AccentColorLime,
        AccentColorMagenta,
        AccentColorOrange,
        AccentColorPink,
        AccentColorPurple,
        AccentColorRed,
        AccentColorTeal,
    };

public:
    virtual ~OfficeStyle();
public:
    QColor accentColor() const;
    void setAccentColor(const QColor& accentcolor);

    OfficeStyle::AccentColor accentIndexColor() const;
    void setAccentColor(AccentColor indexcolor);

    bool isActiveTabAccented() const;
    void setActiveTabAccented(bool accented);

    void setMDIWindowTitleIgnored(bool ignore = false);
    bool isMDIWindowTitleIgnored() const;

    OfficeStyle::OfficePopupDecoration popupDecoration() const;
    void setPopupDecoration(OfficeStyle::OfficePopupDecoration typeStyle);
public:
    virtual void polish(QWidget* widget);
    virtual void polish(QPalette& palette);
    virtual void polish(QApplication*);
    virtual void unpolish(QWidget* widget);
    virtual QPixmap standardPixmap(StandardPixmap sp, const QStyleOption* opt = QSTYLEOPTION_NULL, const QWidget* widget = QWIDGET_NULL) const;
    virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* w = QWIDGET_NULL) const;
    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget = QWIDGET_NULL) const;
    virtual int styleHint(QStyle::StyleHint sh, const QStyleOption* opt = QSTYLEOPTION_NULL, const QWidget* w = QWIDGET_NULL, QStyleHintReturn* shret = Q_NULL) const;
protected:
    OfficeStyle(OfficeStylePrivate& d);
    QTN_DECLARE_EX_PRIVATE(OfficeStyle)
private:
    friend class OfficePopupWindow;
    friend class OfficePopupWindowPrivate;
    Q_DISABLE_COPY(OfficeStyle)
};

QTITAN_END_NAMESPACE

#endif //QTN_OFFICESTYLE_H

