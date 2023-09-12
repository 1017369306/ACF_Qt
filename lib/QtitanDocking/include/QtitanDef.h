/****************************************************************************
**
** Qtitan Library by Developer Machines
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


#ifndef QTITAN_DEF_H
#define QTITAN_DEF_H

#include <qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include <QtCore/QCborStreamReader> //Bug-fix for Python-shiboken2 compilation warning.
#endif

#include <QtCore/QProcess>
#include <QString>

#define QTN_QUOTE_(x) #x
#define QTN_QUOTE(x) QTN_QUOTE_(x)

#ifdef Q_COMPILER_NULLPTR
#define Q_NULL    nullptr
#else
#define Q_NULL    NULL
#endif
#define QWIDGET_NULL static_cast<QWidget *>(Q_NULL)
#define QSTYLEOPTION_NULL static_cast<QStyleOption *>(Q_NULL)

#define Q_MAXINT INT_MAX
#define Q_DELETE_AND_NULL(x) { delete x; x = Q_NULL; }

#if defined(QTN_NAMESPACE) && !defined(QTITAN_DESIGNER)
#define QTITAN_NAMESPACE Qtitan
#endif

#if !defined(QTITAN_NAMESPACE)
#define QTITAN_BEGIN_NAMESPACE
#define QTITAN_END_NAMESPACE
#define QTITAN_USE_NAMESPACE
#define QTITAN_PREPEND_NAMESPACE(name) name
#define QTITAN_CLASSNAME(classname) classname
#define QTITAN_META_CLASSNAME(classname) classname
#else
#define QTITAN_BEGIN_NAMESPACE namespace QTITAN_NAMESPACE {
#define QTITAN_END_NAMESPACE }
#define QTITAN_USE_NAMESPACE using namespace QTITAN_NAMESPACE;
#define QTITAN_PREPEND_NAMESPACE(name) QTITAN_NAMESPACE::name
#define QTITAN_CLASSNAME(classname) QTN_QUOTE(QTITAN_NAMESPACE::)classname
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#define QTITAN_META_CLASSNAME(classname) QTN_QUOTE(QTITAN_NAMESPACE::)classname
#else
#define QTITAN_META_CLASSNAME(classname) classname
#endif
#endif


#ifndef QTITAN_LIBRARY_STATIC

#ifdef QTITAN_LIBRARY
#define QTITAN_EXPORT Q_DECL_EXPORT
#else
#define QTITAN_EXPORT Q_DECL_IMPORT
#endif

#ifdef QTITAN_BASE_LIBRARY
#define QTITAN_BASE_EXPORT Q_DECL_EXPORT
#else
#define QTITAN_BASE_EXPORT Q_DECL_IMPORT
#endif

#else
#define QTITAN_EXPORT
#define QTITAN_BASE_EXPORT
#endif

#ifndef Q_DECL_NOTHROW
#define Q_DECL_NOTHROW throw()
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))

#if !defined(QT_PRINTSUPPORT_LIB) && !defined(Q_QDOC)
#define QTITAN_NO_PRINTSUPPORT
#endif
#if !QT_CONFIG(process) && !defined(Q_QDOC)
#define QTITAN_NO_PROCESS
#endif

#endif

#if !defined(QStringLiteral)
#define QStringLiteral(str) QString(QLatin1String(str))
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#define QStringSubset QStringView
#else
#define QStringSubset QStringRef
#endif

namespace Qtitan
{
    enum SortOrder
    {
        SortNone,
        SortAscending,
        SortDescending
    };

    enum ScrollBarsMode
    {
        ScrollNone,
        ScrollBoth,
        ScrollHorizontal,
        ScrollVertical,
        ScrollAuto
    };

    enum LinesStyle
    {
        LinesNone = 0,
        LinesBoth,
        LinesBoth2D,
        LinesHorizontal,
        LinesHorizontal2D,
        LinesVertical,
        LinesVertical2D
    };

    enum ScrollItemStyle
    {
        ScrollByItem,
        ScrollByPixel
    };

    enum HighlightEffect
    {
        FlashEffect,
        AlphaEffect
    };

    enum ItemPosition
    {
        AtNone = 0,
        AtBeginning,
        AtEnd
    };

    enum LayoutArea
    {
        LeftArea   = 1,
        RightArea  = 2,
        TopArea    = 3,
        BottomArea = 4,
        DefaultArea = LeftArea,
    };

    enum BestFitModeFlag
    {
        FitToHeader = 1,
        FitToViewContent = 2,
        FitToContent = 4,
        FitToHeaderAndViewContent = FitToHeader | FitToViewContent,
        FitToHeaderAndContent = FitToHeader | FitToContent
    };
    Q_DECLARE_FLAGS(BestFitMode, BestFitModeFlag)

    enum SelectionOperator
    {
        Empty = 0x0000,
        Clear = 0x0001,
        Select = 0x0002,
        Unselect = 0x0004,
        Invert = 0x0008,
        StartCurrent = 0x0010
    };
    Q_DECLARE_FLAGS(SelectionOperation, SelectionOperator)

    enum EditStrategy
    {
        OnFieldChange,
        OnRowChange
    };

    enum IconApperance
    {
        IconInvisible = 0,
        IconOnly,
        IconAlignLeft,
        IconAlignRight
    };

    enum ExpandingMode
    {
        Expanded = 0,
        Compact = 1,
        Minimal = 2,
        Popup = 3
    };

    enum TransitionMode
    {
        Entrance,
        DrillIn,
        Suppress
    };

    enum ItemDataRoleEx 
    {
        ComboBoxRole = Qt::UserRole + 100,
        QueryIndexRole
    };

    enum ContextColor
    {
        ContextColorNone,
        ContextColorGreen,
        ContextColorBlue,
        ContextColorRed,
        ContextColorYellow,
        ContextColorCyan,
        ContextColorPurple,
        ContextColorOrange,
    };

    enum DockPanelArea
    {
        NoDockPanelArea = 0x00,
        LeftDockPanelArea = 0x01,
        RightDockPanelArea = 0x02,
        TopDockPanelArea = 0x04,
        BottomDockPanelArea = 0x08,
        InsideDockPanelArea = 0x10,
        DockPanelArea_Mask = 0xff,
        AllDockPanelAreas = DockPanelArea_Mask,
    };
    Q_DECLARE_FLAGS(DockPanelAreas, DockPanelArea)

    enum DockBarArea
    {
        DockBarNone = 0,
        DockBarLeft,
        DockBarRight,
        DockBarTop,
        DockBarBottom,
        DockBarFloat,
    };

#ifndef QTITAN_NO_PROCESS
    QTITAN_BASE_EXPORT QString getSystemEnvironmentVariable(const QString& key);
    QTITAN_BASE_EXPORT QString getEnvironmentVariable(QProcess* process, const QString& key);
    QTITAN_BASE_EXPORT void setEnvironmentVariable(QProcess* process, const QString& key, const QString& value);
#endif
}

QTITAN_BEGIN_NAMESPACE

#define QTN_DECLARE_PRIVATE(Class) friend class Class##Private; \
Class##Private *qtn_d_ptr; \
Class##Private& qtn_d() { return *qtn_d_ptr; } \
const Class##Private& qtn_d() const { return *qtn_d_ptr; }

#define QTN_DECLARE_PUBLIC(Class) \
friend class Class; \
Class *qtn_p_ptr; \
inline void setPublic(Class *ptr) { qtn_p_ptr = ptr; } \
Class& qtn_p() { return *qtn_p_ptr; } \
const Class& qtn_p() const { return *qtn_p_ptr; }
#define QTN_INIT_PRIVATE(Class) \
qtn_d_ptr = new Class##Private(); qtn_d_ptr->setPublic(this);
#define QTN_D(Class) Class##Private& d = qtn_d();
#define QTN_P(Class) Class& p = qtn_p();
#define QTN_FINI_PRIVATE() \
delete qtn_d_ptr; qtn_d_ptr = Q_NULL;

#define QTN_DECLARE_EX_PRIVATE(Class) friend class Class##Private; \
Class##Private& qtn_d() { return reinterpret_cast<Class##Private &>(*qtn_d_ptr); } \
const Class##Private& qtn_d() const { return reinterpret_cast<const Class##Private &>(*qtn_d_ptr); }

#define QTN_DECLARE_EX_PUBLIC(Class) \
friend class Class; \
Class& qtn_p() { return static_cast<Class &>(*qtn_p_ptr); } \
const Class& qtn_p() const { return static_cast<Class &>(*qtn_p_ptr); }

class QTITAN_BASE_EXPORT BaseEventArgs : public QObject
{
public:
    BaseEventArgs() : m_handled(false) {}
    bool isHandled() const { return m_handled; }
    void setHandled(bool handled) { m_handled = handled; }
private:
    bool m_handled;
};

inline QString create_qtitan_className(const QString& className)
{
    QString s;
#if !defined(QTITAN_NAMESPACE)
    s = className;
#else
    s = QStringLiteral("Qtitan::") + className;
#endif
    return s;
}


QTITAN_END_NAMESPACE

Q_DECLARE_OPERATORS_FOR_FLAGS(Qtitan::BestFitMode)
Q_DECLARE_OPERATORS_FOR_FLAGS(Qtitan::SelectionOperation)
Q_DECLARE_OPERATORS_FOR_FLAGS(Qtitan::DockPanelAreas)

QTITAN_USE_NAMESPACE

#endif //QTITAN_DEF_H

