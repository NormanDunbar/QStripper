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

#ifndef NDWORKSPACE_H
#define NDWORKSPACE_H

#include <qworkspace.h>


class NDWorkspace : public QWorkspace
{
    Q_OBJECT
    
public:
    NDWorkspace(QWidget *parent = 0) : QWorkspace(parent) {}

public slots:
    void tileVertically();
    void tileHorozontally();
};




#endif
