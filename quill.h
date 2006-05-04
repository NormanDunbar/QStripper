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

#ifndef QUILL_H
#define QUILL_H

#include <QTextStream>

class QuillDoc
{

public:
    QuillDoc(const QString &Filename);
    ~QuillDoc();
    QString TextHeader();              // Return stripped Quill document header.
    QString TextFooter();              // Ditto the footer.
    QString TextContents();            // And the contents.
    QString HTMLHeader();              // Return Quill header as HTML.
    QString HTMLFooter();              // Ditto the footer.
    QString HTMLContents();            // And the contents.
    QString XMLHeader();               // Return Quill header as DocBook XML.
    QString XMLFooter();               // Ditto the footer.
    QString XMLContents();             // And the contents.
protected:

private:
    QString DocFile;                   // Original Quill file.
    QString HTMLFile;                  // Exported HTML filename.
    QString XMLFile;                   // Exported DocBook filename.
    QString TextFile;                  // Exported Text filename.
    
    
};

#endif
