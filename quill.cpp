/****************************************************************************
**
** Copyright (C) 2006-2009 Dunbar IT Consultants Ltd.
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

#include "quill.h"

const int Bold = 1;
const int Under = 2;
const int Subscript = 4;
const int Superscript = 8;

QuillDoc::~QuillDoc()
{
    if (document) delete document;
}

//------------------------------------------------------------------------------
// Constructor - opens the filename and reads in the raw data. As the data is
// in QDOS format, it is converted to 'proper' ASCII format as it is read. This
//is done with a simple translation table.
//------------------------------------------------------------------------------
QuillDoc::QuillDoc(const QString FileName)
{
    // Initialise everything.
    fHeaderLength = 0;
    fQuillMagic.clear();
    fHeader.clear();
    fFooter.clear();
    document = new QTextDocument();     // We need to delete this!
    fTextLength = 0;
    fParaTableLength = 0;
    fFreeSpaceLength = 0;
    fLayoutTableLength = 0;
    fValid = false;
    fErrorMessage.clear();

    // Try to load the file as raw data after performing a few checks to
    // see if it may be a Quill document.
    loadFile(FileName);
    if (fValid) {
        // Must be a valid Quill file. Build a document from the raw contents.
        parseFile();
    }
}

//------------------------------------------------------------------------------
// Open the supplied file and check if it is actually a valid Quill document. If
// so, read in the header data and the raw bytes. The file is closed here as
// well. Everything else happens with the raw data.
//------------------------------------------------------------------------------

void QuillDoc::loadFile(const QString FileName)
{
    // Make sure we read integers and stuff in BigEndian mode - like the QL does :o)
    QFile file(FileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in.setByteOrder(QDataStream::BigEndian);

    // The first two bytes are 0x00 and 0x14 = 20 = Size of header block.
    fValid = false;
    in >> fHeaderLength;

    if (fHeaderLength != 20) {
        fValid = false;
        fErrorMessage = QString("Header block length not equal 20 bytes, actually = %1").arg(fHeaderLength);
        return;
    }

    // The next 8 bytes are "vrm1qdf0"
    fQuillMagic.clear();
    quint8 Char;
    for (int x = 0; x < 8; ++x) {
       in >> Char;
       fQuillMagic.append(Char);
    }

    if (fQuillMagic != "vrm1qdf0") {
        fValid = false;
        fErrorMessage = QString("Header flag bytes not equal 'vrm1qdf0', actually = %1").arg(fQuillMagic);
        return;
    }

    // The next 4 bytes are big-endian text length.
    in >> fTextLength;

    // Fetch the three 2 byte pointers.
    in >> fParaTableLength;
    in >> fFreeSpaceLength;
    in >> fLayoutTableLength;

    file.close();
    fValid = true;
    fErrorMessage = "";

    // Now, read in the entire file as a QByteArray.
    file.open(QIODevice::ReadOnly);
    fRawFileContents = file.readAll();
    file.close();
}

//------------------------------------------------------------------------------
// Extract various bits from the document's raw data.
//------------------------------------------------------------------------------
void QuillDoc::parseFile()
{
    parseText();
    parseParagraphTable();
    parseFreeSpaceTable();
    parseLayoutTable();
}

//------------------------------------------------------------------------------
// Extract the text including headers and footers.
//------------------------------------------------------------------------------
void QuillDoc::parseText()
{
    // The text area always starts at offset 20. The first two paragraphs are
    // the header and footer - which may be blank. The terminating byte of zero
    // will always be found. (All paragraphs are terminated by a zero byte.)
    quint8 Char;

    // Header first.
    fRawPointer = 20;       // Always the start of the text area.

     do {
       Char = fRawFileContents[fRawPointer++];  // Points to NEXT character now.
       if (Char == 0) break;
       Char = translate(Char);
       fHeader.append(Char);
    } while (1);

    // Footer next.
    do {
       Char = fRawFileContents[fRawPointer++];  // Points to NEXT character now.
       if (Char == 0) break;
       Char = translate(Char);
       fFooter.append(Char);
    } while (1);

    // The actual text comes next. We stop when we reach offset fTextLength as
    // that is the first byte of the following Paragraph table.

    QTextCursor cursor(document);
    QTextCharFormat charFormat;
    QTextCharFormat defaultFormat;

    // Set default settings for everything off in new paragraphs.
    defaultFormat.setFontFamily("Courier New");
    defaultFormat.setFontPointSize(12);
    defaultFormat.setFontItalic(false);

    bool SuperOn = false;
    bool SubOn = false;
    defaultFormat.setVerticalAlignment(QTextCharFormat::AlignNormal);

    bool BoldOn = false;
    defaultFormat.setFontWeight(QFont::Normal);

    bool UnderOn = false;
    defaultFormat.setFontUnderline(UnderOn);

    // Set the current format as well - for the first (system created) paragraph.
    cursor.setCharFormat(defaultFormat);

    // And read the format for later too.
    charFormat = cursor.charFormat();

    // And now, process the text contents.
    while (fRawPointer < fTextLength) {
       Char = fRawFileContents[fRawPointer++];
       Char = translate(Char);

       // Process each character to see if it is a control code or not.
       switch (Char) {
         case 0 : // Paragraph end & reset attributes.
             BoldOn = UnderOn = SuperOn = SubOn = false;
             cursor.insertBlock();
             cursor.setCharFormat(defaultFormat);
             break;

         case 12: break;                                  // Form Feed - ignored.

         case 15: if (BoldOn) {
                    charFormat.setFontWeight(QFont::Normal);
                } else {
                    charFormat.setFontWeight(QFont::Bold);
                }

                BoldOn = !BoldOn;
                cursor.setCharFormat(charFormat);
                continue;
                break;

         case 16: if (UnderOn) {
                    charFormat.setFontUnderline(false);
                } else {
                    charFormat.setFontUnderline(true);
                }
                UnderOn = !UnderOn;

                cursor.setCharFormat(charFormat);
                continue;
                break;

         case 17: if (SubOn) {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                  } else {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignSubScript);
                  }
                  SubOn = !SubOn;

                cursor.setCharFormat(charFormat);
                continue;
                break;

         case 18: if (SuperOn) {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                  } else {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
                  }
                  SuperOn = !SuperOn;

                cursor.setCharFormat(charFormat);
                continue;
                break;

         case 30: continue; break;                       // Soft hyphen - ignored.

         //case 169: cursor.insertText(QString(Char));    // Copyright
         //          break;

         //case 128: cursor.insertText(QString(Char));  // Euro symbol
         //          break;

         default: cursor.insertText(QString(Char));     // Everything else.

       }
    }
}

//------------------------------------------------------------------------------
// Extract the paragraph table.
//------------------------------------------------------------------------------
void QuillDoc::parseParagraphTable()
{
}

//------------------------------------------------------------------------------
// Extract the free space table - which is ignored.
//------------------------------------------------------------------------------
void QuillDoc::parseFreeSpaceTable()
{
}

//------------------------------------------------------------------------------
// Extract the layout table.
//------------------------------------------------------------------------------
void QuillDoc::parseLayoutTable()
{
}

QTextDocument *QuillDoc::getDocument()
{
    return document;
}


//------------------------------------------------------------------------------
// Returns a Windows (iso-latin) character for a given QDOS character.
//------------------------------------------------------------------------------
quint8  QuillDoc::translate(const quint8 c)
{
    static quint8 Ql2Win[] = {                // From char, to char
           127, 169, // �
           128, 228, // �
           129, 227, // �
           130, 229, // �
           131, 233, // �
           132, 246, // �
           133, 245, // �
           134, 248, // �
           135, 252, // �
           136, 231, // �
           137, 241, // �
           138, 230, // �
           139, 156, // �
           140, 225, // �
           141, 224, // �
           142, 226, // �
           143, 235, // �
           144, 232, // �
           145, 234, // �
           146, 239, // �
           147, 237, // �
           148, 236, // �
           149, 238, // �
           150, 243, // �
           151, 242, // �
           152, 244, // �
           153, 250, // �
           154, 249, // �
           155, 251, // �
           156, 223, // �
           157, 162, // �
           158, 165, // �
           159, 180, // �
           160, 196, // �
           161, 195, // �
           162, 194, // �
           163, 201, // �
           164, 214, // �
           165, 213, // �
           166, 216, // �
           167, 220, // �
           168, 199, // �
           169, 209, // �
           170, 198, // �
           171, 140, // �
           172, 172, // �
           173, 173, // �
           174, 174, // �
           175, 175, // �
           176, 181, // �
           177, 112, // p
           178, 178, // �
           179, 161, // �
           180, 191, // �
           181, 128, // �
           182, 167, // �
           183, 183, // �
           184, 171, // �
           185, 187, // �
           186, 176, // �
           187, 247}; // �

    // Pound Sterling sign (�).
    if (c == 96) return 163;

    // Unchanged characters.
    if (c < 127 || c > 187) return c;
        
    // All the rest.
    return (Ql2Win[ ((c - 127) * 2) + 1 ]);
}


//------------------------------------------------------------------------------
// Returns a QString holding the body text from the original Quill document.
//------------------------------------------------------------------------------
QString QuillDoc::getText()
{
    return document->toPlainText();
}


//------------------------------------------------------------------------------
// Returns the entire contents of the Quill document as a QByteArray.
//------------------------------------------------------------------------------
QByteArray QuillDoc::getRawText()
{
    return fRawFileContents;
}

//------------------------------------------------------------------------------
// Returns a QString holding the body text from the original Quill document.
//------------------------------------------------------------------------------
quint32 QuillDoc::getTextLength()
{
    return fTextLength;
}


//------------------------------------------------------------------------------
// Returns a QString holding the header text from the original Quill document.
//------------------------------------------------------------------------------
QString QuillDoc::getHeader()
{
    return fHeader;
}


//------------------------------------------------------------------------------
// Returns a QString holding the footer text from the original Quill document.
//------------------------------------------------------------------------------
QString QuillDoc::getFooter()
{
    return fFooter;
}


//------------------------------------------------------------------------------
// Did we get a valid Quill document ?
//------------------------------------------------------------------------------
bool QuillDoc::isValid()
{
  return fValid;
}


//------------------------------------------------------------------------------
// What was the last error that occurred ?
//------------------------------------------------------------------------------
QString QuillDoc::getError()
{
  return fErrorMessage;
}
