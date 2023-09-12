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

#ifndef QTN_VISUALSTUDIO2019STYLE_H
#define QTN_VISUALSTUDIO2019STYLE_H

#include "QtnCommonStyle.h"

QTITAN_BEGIN_NAMESPACE

class VisualStudio2019StylePrivate;
/* VisualStudio2019Style */
class QTITAN_EXPORT VisualStudio2019Style : public CommonStyle
{
    Q_OBJECT
    Q_ENUMS(Theme)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme)
public:
    enum Theme
    {
        Blue,
        Dark,
        Light,
    };
public:
    explicit VisualStudio2019Style();
    ~VisualStudio2019Style() override;
public:
    Theme theme() const;
    void setTheme(Theme theme);
public:
    void polish(QApplication* app) override;
    void polish(QWidget* widget) override;
    void unpolish(QWidget* widget) override;
    QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget = QWIDGET_NULL) const override;
    QPixmap standardPixmap(QStyle::StandardPixmap sp, const QStyleOption* opt, const QWidget* widget = QWIDGET_NULL) const override;
protected:
    QTN_DECLARE_EX_PRIVATE(VisualStudio2019Style)
private:
    Q_DISABLE_COPY(VisualStudio2019Style)
};

QTITAN_END_NAMESPACE

#endif // QTN_QTNVISUALSTUDIO2019STYLE_H
