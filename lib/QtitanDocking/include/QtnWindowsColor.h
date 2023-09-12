/****************************************************************************
**
** Qtitan Library by Developer Machines (Navigation UI for Qt.C++)
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
#ifndef QTN_WINDOWSCOLOR_H
#define QTN_WINDOWSCOLOR_H

#include <QColor>
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class QTITAN_BASE_EXPORT WindowsColor : public QColor
{
public:
    enum GlobalColor
    {
        DefaultColor,
        MetroUI_Sapphire,
        MetroUI_KleinBlue,
        MetroUI_Cardinal,
        MetroUI_Chocolate,
        MetroUI_RawUmber,
        MetroUI_DarkCerulean,
        MetroUI_GreenHouse,
        MetroUI_VidaLoca,
        MetroUI_NavyBlue,
        MetroUI_Matterhorn,
        MetroUI_Watercourse,

        MetroUI_LightGreen,
        MetroUI_Green,
        MetroUI_DarkGreen,
        MetroUI_Magenta,
        MetroUI_LightPurple,
        MetroUI_Purple,
        MetroUI_DarkPurple,
        MetroUI_Darken,
        MetroUI_Teal,
        MetroUI_LightBlue,
        MetroUI_Blue,
        MetroUI_DarkBlue,
        MetroUI_Yellow,
        MetroUI_Orange,
        MetroUI_DarkOrange,
        MetroUI_Red,
        MetroUI_DarkRed,
        MetroUI_White,

        MetroUI_Tile_Gradient1,
        MetroUI_Tile_Gradient2,
        MetroUI_Blue_Gradient1,
        MetroUI_Blue_Gradient2,
        MetroUI_Purple_Gradient1,
        MetroUI_Purple_Gradient2,
        MetroUI_DarkPurple_Gradient1,
        MetroUI_DarkPurple_Gradient2,
        MetroUI_Red_Gradient1,
        MetroUI_Red_Gradient2,
        MetroUI_Orange_Gradient1,
        MetroUI_Orange_Gradient2,
        MetroUI_Green_Gradient1,
        MetroUI_Green_Gradient2,
        MetroUI_SkyBlue_Gradient1,
        MetroUI_SkyBlue_Gradient2,

        Phone_Lime,
        Phone_Green,
        Phone_Emerald,
        Phone_Teal,
        Phone_Cyan,
        Phone_Cobalt,
        Phone_Indigo,
        Phone_Violet,
        Phone_Pink,
        Phone_Magenta,
        Phone_Crimson,
        Phone_Red,
        Phone_Orange,
        Phone_Amber,
        Phone_Yellow,
        Phone_Brown,
        Phone_Olive,
        Phone_Steel,
        Phone_Mauve,
        Phone_Taupe,
    };

public:
    WindowsColor(GlobalColor color);
};

QTITAN_END_NAMESPACE

#endif //QTN_WINDOWSCOLOR_H



