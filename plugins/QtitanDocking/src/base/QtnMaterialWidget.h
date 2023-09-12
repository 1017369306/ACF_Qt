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
#ifndef QTN_MATERIAL_WIDGET_H
#define QTN_MATERIAL_WIDGET_H

#include <QWidget>
#include <QGraphicsEffect>
#include "QtitanDef.h"
#ifdef QTN_OPENGL_ENABLED
#include "QtnOpenGLEffect.h"
#endif

QTITAN_BEGIN_NAMESPACE

class QTITAN_BASE_EXPORT MaterialWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(BlendType)
    Q_PROPERTY(BlendType blend READ blendType WRITE setBlendType)
    Q_PROPERTY(QColor tintColor READ tintColor WRITE setTintColor)
    Q_PROPERTY(double tintOpacity READ tintOpacity WRITE setTintOpacity)
    Q_PROPERTY(double noiseOpacity READ noiseOpacity WRITE setNoiseOpacity)
public:
    enum BlendType
    {
        DisabledBlend,
        InAppBlend,
        BackgroundBlend
    };
    explicit MaterialWidget(QWidget* parent = Q_NULL, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~MaterialWidget();
    BlendType blendType() const;
    void setBlendType(BlendType type);
    const QColor& tintColor() const;
    void setTintColor(const QColor& color);
    double tintOpacity() const;
    void setTintOpacity(double opacity);
    double noiseOpacity() const;
    void setNoiseOpacity(double opacity);
protected:
    QBrush createAcrylicBrush() const;
    void paintBackground(QPainter* painter, const QRegion& clip);
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
    virtual void changeEvent(QEvent * event);
    const QImage& grabBackstageToBuffer(const QRegion& clip, bool& hasChanges);
    void setBackstageToWindow(const QImage& newBackstage, const QRegion& clip);
    QRect clipRect() const;
private:
    friend class MaterialWidgetEffect;
    BlendType m_blend;
    QColor m_tintColor;
    double m_tintOpacity;
    double m_noiseOpacity;
    QBrush m_acrylicBrush;
    QImage m_buffer;
};

QTITAN_END_NAMESPACE

#endif// QTN_MATERIAL_WIDGET_H




