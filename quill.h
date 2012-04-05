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
class QByteArray;
class QTextDocument;

#include <QtGui>
#include <QObject>

class QuillDoc {

private:
    quint16 fHeaderLength;                  // Quill header size - should be 20.
    QString fQuillMagic;                    // Quill 'magic' flag = 'vrm1qdf0'.
    QString fHeader;                        // Document header text.
    QString fFooter;                        // Document footer text.
    QByteArray fRawFileContents;            // Bytes of the document, as read.
    quint32 fRawPointer;                    // Used when scanning the raw document.
    QTextDocument *document;                // The raw text reformatted as "RTF"
    quint32 fTextLength;                    // Size of the above.
    quint16 fParaTableLength;               // Size of Paragraph table.
    quint16 fFreeSpaceLength;               // Size of free space table.
    quint16 fLayoutTableLength;             // Size of layout table.
    bool    fValid;                         // Is this a valid Quill document?
    QString fErrorMessage;                  // What went wrong ?
    bool    fPCFile;                        // This is a PC Quill file, or not.

    void    loadFile(const QString FileName); // Load a valid Quill file?
    void    parseFile();                    // Parse it into a document.
    void    parseText();                    // The next 4 do as they say!
    void    parseParagraphTable();
    void    parseFreeSpaceTable();
    void    parseLayoutTable();

    quint8  translate(const quint8 c);      // Convert from QDOS to Win/Lin chars.

public :
    QuillDoc(const QString FileName);
    ~QuillDoc();

    QString getText();
    QByteArray getRawText();
    quint32 getTextLength();
    QString getHeader();
    QString getFooter();
    bool    isValid();
    QString getError();
    QTextDocument *getDocument();
};

#endif
