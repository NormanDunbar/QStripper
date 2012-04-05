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
const int Italic = 16;


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
    fPCFile = false;

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

    // The first two bytes are 0x00 and 0x14 = 20 = Size of header block. (QL)
    // or 0x14 and 0x00 = 5,120 if we are reading a PC Quill document. (PC)
    fValid = false;
    in >> fHeaderLength;

    if (fHeaderLength != 20 && fHeaderLength != 5120) {  // 20 = QL, 5,120 = PC
        fValid = false;
        fErrorMessage = QString("Header block length not equal 20 bytes, actually = %1").arg(fHeaderLength);
        return;
    }

    // Are we reading a PC Quill doc? If so, adjust things accordingly.
    if (fHeaderLength == 5120) {
        fHeaderLength = 20;
        fPCFile = true;
        in.setByteOrder(QDataStream::LittleEndian);
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

    // The next 4 bytes are the text length.
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

    bool ItalicOn = false;
    defaultFormat.setFontItalic(ItalicOn);

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
             BoldOn = UnderOn = SuperOn = SubOn = ItalicOn = false;
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

         case 19: if (ItalicOn) {
                    charFormat.setFontItalic(false);
                  } else {
                    charFormat.setFontItalic(true);
                  }

                  ItalicOn = !ItalicOn;
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
    static quint8 PC2QL[] = {
        0x81, 0x87, // u umlaut
        0x82, 0x83, // e acute
        0x83, 0x8e, // a circumflex
        0x84, 0x80, // a umlaut
        0x85, 0x8d, // a grave
        0x87, 0x88, // c cedilla
        0x88, 0x91, // e curcumflex
        0x89, 0x8f, // e umlaut
        0x8a, 0x90, // e grave
        0x8b, 0x92, // i umlaut
        0x8c, 0x95, // i circumflex
        0x93, 0x98, // o circumflex
        0x94, 0x84, // o umlaut
        0x96, 0x9b, // u curcumflex
        0x97, 0x9a  // u grave
    };

    static quint8 Ql2Win[] = {                // From char, to char


           127, 169, // ©
           128, 228, // ä
           129, 227, // ã
           130, 229, // å
           131, 233, // é
           132, 246, // ö
           133, 245, // õ
           134, 248, // ø
           135, 252, // ü
           136, 231, // ç
           137, 241, // ñ
           138, 230, // æ
           139, 156, // œ
           140, 225, // á
           141, 224, // à
           142, 226, // â
           143, 235, // ë
           144, 232, // è
           145, 234, // ê
           146, 239, // ï
           147, 237, // í
           148, 236, // ì
           149, 238, // î
           150, 243, // ó
           151, 242, // ò
           152, 244, // ô
           153, 250, // ú
           154, 249, // ù
           155, 251, // û
           156, 223, // ß
           157, 162, // ¢
           158, 165, // ¥
           159, 180, // ´
           160, 196, // Ä
           161, 195, // Ã
           162, 194, // Â
           163, 201, // É
           164, 214, // Ö
           165, 213, // Õ
           166, 216, // Ø
           167, 220, // Ü
           168, 199, // Ç
           169, 209, // Ñ
           170, 198, // Æ
           171, 140, // Œ
           172, 172, // ¬
           173, 173, // ­
           174, 174, // ®
           175, 175, // ¯
           176, 181, // µ
           177, 112, // p
           178, 178, // ²
           179, 161, // ¡
           180, 191, // ¿
           181, 128, // €
           182, 167, // §
           183, 183, // ·
           184, 171, // «
           185, 187, // »
           186, 176, // °
           187, 247}; // ÷

    // First, PC to QL translation. This in incomplete at present!
    quint8 cc = c;
    switch (cc) {
        case 0x81: cc = 0x87 ; break;
        case 0x82: cc = 0x83 ; break;
        case 0x83: cc = 0x8e ; break;
        case 0x84: cc = 0x80 ; break;
        case 0x85: cc = 0x8d ; break;
        case 0x87: cc = 0x88 ; break;
        case 0x88: cc = 0x91 ; break;
        case 0x89: cc = 0x8f ; break;
        case 0x8a: cc = 0x90 ; break;
        case 0x8b: cc = 0x92 ; break;
        case 0x8c: cc = 0x95 ; break;
        case 0x93: cc = 0x98 ; break;
        case 0x94: cc = 0x84 ; break;
        case 0x96: cc = 0x9b ; break;
        case 0x97: cc = 0x9a ; break;
    }


    // Pound Sterling sign (£).
    if (cc == 96) return 163;

    // Unchanged characters.
    if (cc < 127 || cc > 187) return c;
        
    // All the rest.
    return (Ql2Win[ ((cc - 127) * 2) + 1 ]);
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
