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

class QFile;
class QDataStream;

#include <QtGui>
#include <QObject>

class QuillDoc {

private:
    quint16 fHeaderLength;                  // Quill header size - should be 20.
    QString fQuillMagic;                    // Quill 'magic' flag = 'vrm1qdf0'.
    QString fHeader;                        // Document header text.
    QString fFooter;                        // Document footer text.
    QString fText;                          // Document contents text.
    quint32 fTextLength;                    // Size of the above.
    bool    fValid;                         // Is this a valid Quill document? 
    QString fErrorMessage;                  // What went wrong ?

public :
    QuillDoc(QString FileName);
    ~QuillDoc() {};

    QString getText();
    quint32 getTextLength();
    QString getHeader();
    QString getFooter();
    bool    isValid();
    QString getError();
};

#endif
