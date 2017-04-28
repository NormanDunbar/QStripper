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

// Some stuff for the paragraph table.

// Text justification values. QL and DOS are different.

const quint8    JUSTIFY_LEFT_QL = 0;
const quint8    JUSTIFY_CENTRE_QL = 1;
const quint8    JUSTIFY_RIGHT_QL = 2;
const quint8    JUSTIFY_LEFT_DOS = 4;
const quint8    JUSTIFY_CENTRE_DOS = 5;
const quint8    JUSTIFY_RIGHT_DOS = 6;


typedef struct paraTable {
    quint32 textOffset;
    quint16 textLength;
    quint8  unused_1;
    quint8  leftMargin;
    quint8  indentMargin;
    quint8  rightMargin;
    quint8  justification;
    quint8  tabTableEntry;
    quint16 unused_2;
} paraTable;




// A couple of structs for the layout table.

typedef struct oneTab {
    quint8 tabPosition;
    quint8 tabType;
} oneTab;

typedef struct tabTable {
    quint8 tabNumber;
    quint8 tabSize;
    oneTab tabArray[1];     // Seriously dodgy code alert!
} tabTable;

// Constants for the display width.

const quint8    LAYOUT_80 = 0;
const quint8    LAYOUT_40 = 1;
const quint8    LAYOUT_64 = 2;

// Constants for the text colour.
const quint8    LAYOUT_TEXT_GREEN = 0;
const quint8    LAYOUT_TEXT_WHITE = 1;

// Constants for the header/footer justification.
const quint8    LAYOUT_HF_JUSTIFY_NONE = 0;
const quint8    LAYOUT_HF_JUSTIFY_LEFT = 1;
const quint8    LAYOUT_HF_JUSTIFY_CENTRE = 2;
const quint8    LAYOUT_HF_JUSTIFY_RIGHT = 3;

// Constants for the header/footer bold.
const quint8    LAYOUT_HF_NORMAL = 0;
const quint8    LAYOUT_HF_BOLD = 1;

// This is for QL files only.
typedef struct layoutTableQL {
    quint8  bottomMargin;
    quint8  displayMode;
    quint8  lineGap;
    quint8  pageLength;
    quint8  firstPage;
    quint8  textColour;
    quint8  topMargin;
    quint8  unused_1;
    quint16 wordCount;
    quint16 tabAreaSize;
    quint16 tabAreaUsed;
    quint8  headerJustification;
    quint8  footerJustification;
    quint8  headerMargin;
    quint8  footerMargin;
    quint8  headerBold;
    quint8  footerBold;

    tabTable    docTabs;
} layoutTableQL;

// This is for DOS files only.
typedef struct layoutTableDOS {
    quint8  bottomMargin;
    quint8  lineGap;
    quint8  pageLength;
    quint8  firstPage;
    quint8  topMargin;
    quint8  unused_1;
    quint8  unused_2;
    quint8  unused_3;
    quint8  unused_4;
    quint8  flag_1;
    quint16 wordCount;
    quint16 tabAreaSize;
    quint16 tabAreaUsed;
    quint8  headerJustification;
    quint8  footerJustification;
    quint8  headerMargin;
    quint8  footerMargin;
    quint8  headerBold;
    quint8  footerBold;

    tabTable    docTabs;
} layoutTableDOS;



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
    layoutTableQL *fLayoutTableQL;          // QL layout table address.  }
    layoutTableDOS *fLayoutTableDOS;        // DOS layout table address. } One or other, not both!
    paraTable *fParagraphTable;             // Address of paragraph table.
    tabTable *fTabTable;                    // Tab table for the document.

    void    loadFile(const QString FileName); // Load a valid Quill file?
    void    parseFile();                    // Parse it into a document.
    void    parseText();                    // The next 4 do as they say!
    void    parseParagraphTable();          // Parse the paragraph table.
    void    parseFreeSpaceTable();          // Ignore the free space table.
    void    parseLayoutTable();             // Parse the layout table.

    quint8  translate(const quint8 c);      // Convert from QDOS to Win/Lin chars.

    // Convert from QL big end to PC/Linux Little end.
    quint16 big2Little16(quint16 big);
    quint32 big2Little32(quint32 big);

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
