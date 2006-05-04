/****************************************************************************
**
** Copyright (C) 2006-2006 Dunbar IT Consultants Ltd.
**
** This file is part of the QStripper application.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "ndworkspace.h"

void NDWorkspace::tileVertically()
{
    QWidgetList windows = windowList();
    if (windows.count() < 2) {
        tile();
        return;
    }

    //int wHeight = height() / windows.count();
    int MaximisedWindows = 0;
    int y = 0;

    // Only make space for non-iconised windows.
    for (int i = 0; i < windows.count(); ++i)
        if (!windows[i]->isMinimized()) MaximisedWindows++;

    int wHeight = height() / MaximisedWindows;

    // Only resize non-iconised windows.
    for (int i = 0; i < windows.count(); ++i) {
        QWidget *widget = windows[i];
        if (!widget->isMinimized()) {
            widget->parentWidget()->resize(width(), wHeight);
            widget->parentWidget()->move(0, y);
            y += wHeight;
        }
    }
}

void NDWorkspace::tileHorozontally()
{
    QWidgetList windows = windowList();
    if (windows.count() < 2) {
        tile();
        return;
    }

    int x = 0;
    int MaximisedWindows = 0;

    // Only make space for non-iconised windows.
    for (int i = 0; i < windows.count(); ++i)
        if (!windows[i]->isMinimized()) MaximisedWindows++;

    int wWidth = width() / MaximisedWindows;

    // Only resize non-iconised windows.
    for (int i = 0; i < windows.count(); ++i) {
        QWidget *widget = windows[i];
        if (!widget->isMinimized()) {
            widget->parentWidget()->resize(wWidth, height());
            widget->parentWidget()->move(x, 0);
            x += wWidth;
        }
    }
}
