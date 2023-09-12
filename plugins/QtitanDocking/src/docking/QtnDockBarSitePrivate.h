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

#ifndef QTN_DOCKBARSITE_PRIVATE_H
#define QTN_DOCKBARSITE_PRIVATE_H

#include <QObject>
#include <QVector>
#include <QRect>

#include "QtnDockBarSite.h"
#include "QtnDockDef.h"

QTITAN_BEGIN_NAMESPACE

class DockBarBase;
class DockBarManager;
class DockMainWindowLayout;
/* DockBarSitePrivate */
class DockBarSitePrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(DockBarSite)
    struct DockData
    {
        DockData(DockBarBase* p = Q_NULL, QRect rect = QRect(), int n = 0)
        {
            bar = p;
            rectPos = rect;
            rectBar = rect;
            index = n;
            minWidth = 0;
            totalMinWidth = 0;
        }

        DockBarBase* bar;
        QRect rectBar;
        QRect rectPos;
        int index;
        int minWidth;
        int totalMinWidth;
    };

    class DockDataVector : public QVector<DockData>
    {
    public:
        void sortVector()
        {
            qsort(data(), size(), sizeof(DockData), compareFunc);
        }
        static int compareFunc(const void* elem1, const void* elem2)
        {
            return ((DockData*)elem1)->rectBar.left() < ((DockData*)elem2)->rectBar.left() ? -1 : 1;
        }
        void invertRects()
        {
            for (int i = 0, count = size(); i < count; i++)
            {
                DockData& info = (*this)[i];
                int x1, y1, x2, y2;
                info.rectBar.getCoords(&x1, &y1, &x2, &y2);
                qSwap(x1, y1); qSwap(x2, y2);
                info.rectBar.setCoords(x1, y1, x2, y2);
            }
        }
    };

public:
    explicit DockBarSitePrivate();
    virtual ~DockBarSitePrivate();
    inline static DockBarSitePrivate* _get(DockBarSite* bars);
    inline static const DockBarSitePrivate* _get(const DockBarSite* bars);
public:
    void initialization(Qtitan::LayoutArea area);
    bool isHorizontal() const { return m_orientation == Qt::Horizontal; }
    int adjustWidthForLine(QList<DockBarBase*>&, const QPoint& pt, int length, bool horz);
    int adjustWidthForLine(QList<DockBarBase*>&, const QPoint& pt, int length, bool horz, int& remove);
    int findControlBar(DockBarBase* bar, int indexExclude = -1);
    int insertControlBar(DockBarBase* pBarIns, QPoint pntMid);
    void updateLayout(const QRect& rectLayout);
    QSize updateLayout(int length, QRect& rect);
    void adjustExpandBars();
    void setDockBarManager(DockBarManager* manager);
protected:
    void collectSortData(const QList<DockBarBase*>& listLine, const QPoint& pt, bool horz, DockDataVector& vecDockData);
    int calculateLength(DockDataVector& vecDockData, int length) const;
    int calculateMaximumLength(const DockDataVector& vecDockData) const;
    void updateGeometryBarsByData(const DockDataVector& vecDockData) const;
public:
    DockBarManager* m_manager;
    Qtitan::LayoutArea m_layoutArea;
    Qt::Orientation m_orientation;
    QList<DockBarBase*> m_listBar;
    DockMainWindowLayout* m_layout;
};

inline DockBarSitePrivate* DockBarSitePrivate::_get(DockBarSite* bars)
{ return &bars->qtn_d(); }

inline const DockBarSitePrivate* DockBarSitePrivate::_get(const DockBarSite* bars)
{ return &bars->qtn_d();}

QTITAN_END_NAMESPACE

#endif // QTN_DOCKBARSITE_PRIVATE_H
