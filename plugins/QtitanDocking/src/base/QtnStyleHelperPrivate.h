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
#ifndef QTN_STYLEHELPERPRIVATE_H
#define QTN_STYLEHELPERPRIVATE_H

#include <QList>
#include <QStyleOption>

#include "QtitanDef.h"
#include "QtnCommonStyle.h"

QTITAN_BEGIN_NAMESPACE

class QTITAN_BASE_EXPORT StyleHelper
{
public:
    StyleHelper();
    virtual ~StyleHelper();
public:
    static void draw3DRect(QPainter& pnt, QColor& col, int x, int y, int w, int h, bool up);
    static void draw3DRect(QPainter& pnt, const QColor& colLight, const QColor& colDark, int x, int y, int w, int h, bool up);

    static void draw3dRectEx(QPainter& pnt, const QRect& rect, const QColor& clrTopLeft, const QColor& clrBottomRight);
    static void draw3dRectEx(QPainter& pnt, int x, int y, int cx, int cy, const QColor& clrTopLeft, const QColor& clrBottomRight);

    static void drawRectangle(QPainter& pnt, const QRect& rc, const QColor& pen, const QColor& brush);
    static void drawTriangle(QPainter& pnt, const QPoint& pt0, const QPoint& pt1, const QPoint& pt2, const QColor& color);

    static void drawGradientFill(QPainter& pnt, const QRect& rect, const QColor& crFrom, const QColor& crTo, bool horz);
    static void drawGradientFill4(QPainter& pnt, const QRect& rect, const QColor& crFrom1, const QColor& crTo1, 
        const QColor& crFrom2, const QColor& crTo2, bool horz = true, int percentage = 50);

    static QRgb lightColor(const QRgb& rgb, int tint);
    static QRgb darkColor(const QRgb& rgb, int tint);

    static void RGBtoHSL(const QRgb& rgb, double& h, double& s, double& l);
    static QRgb HSLtoRGB(double h, double s, double l);
    static QRgb HLStoRGB_ONE( double h, double l, double s );
    static double MakeHue_ONE(double H);
    static double MakeHue_TWO(double H);

    static QRgb colorMakePale(const QRgb& rgb, double ratio = .97);
    static QRgb colorMakeDarker(const QRgb& rgb, double ratio = .1);
    static QRgb colorMakeLighter(const QRgb& rgb, double ratio = .1);

    static QRgb pixelAlpha(const QRgb& srcPixel, int percent);
    static QRgb blendColors(const QRgb& rgb1, const QRgb& rgb2, double ratio);

    static QImage updateImage(const QImage& imageSrc, QRgb clrBase, QRgb clrTone);
    static QImage addaptColors(const QImage& imageSrc, QRgb clrBase, QRgb clrTone, bool clampHue = true);
    static QImage invertColors(const QImage& pxSrc);
private:
    static double HueToRGB(double temp1, double temp2, double temp3);
    static int HueToRGB(int m1, int m2, int h);
    static QLinearGradient generateGradient(const QRect& rect, const QColor& crFrom, const QColor& crTo, bool bHorz);
private:
    static double m_colorTolerance; // Used by 'addaptColors' method
};

QTITAN_END_NAMESPACE

#endif // QTN_STYLEHELPERPRIVATE_H
