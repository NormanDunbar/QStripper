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

QuillDoc::~QuillDoc()
{
    // If we have a current document, delete it.
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
    fLayoutTableQL = NULL;
    fLayoutTableDOS = NULL;
    fParagraphTable = NULL;
    fTabTable = NULL;

    // Try to load the file as raw data after performing a few checks to
    // see if it may be a Quill document.
    loadFile(FileName);
    if (fValid) {
        //Build a document from the raw contents.
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
        fErrorMessage = QString("Header flag bytes not equal 'vrm1qdf0', actually = '%1'").arg(fQuillMagic);
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
    parseParagraphTable();      // Sets pointers to the raw data's paragraph table.
    parseFreeSpaceTable();      // Does nothing!!!
    parseLayoutTable();         // Sets pointers to the raw data's layout table.
    parseText();                // Actually reads the text!
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

    // We need a cursor to keep a handle on our insertion position.
    QTextCursor cursor(document);

    // Create a *paragraph* level default. This can take TABs too! (QTextOption::Tab)
    // TODO: Work out how to use the tabs. Decimal? Right? Left?
    QTextBlockFormat defaultBlockFormat;
    //defaultBlockFormat.setBackground(QColor(255, 255, 220));  // Light Yellow.


    // MESSING ABOUT HERE! ********************************************************

    QList<QTextOption::Tab> blockTabs;

    // TEST: Insert some tabs... It seems that we get the requested ones
    // but also some others afterwards, every 60 pixels? Not what I want.
    blockTabs << QTextOption::Tab(60, QTextOption::LeftTab)
              << QTextOption::Tab(120, QTextOption::LeftTab)
              << QTextOption::Tab(240, QTextOption::LeftTab)
              << QTextOption::Tab(480, QTextOption::LeftTab) ;

    defaultBlockFormat.setTabPositions(blockTabs);

    // END MESSING ABOUT! *********************************************************


    // A default "everything off" character format for new paragraphs.
    QTextCharFormat defaultFormat;

    // Set default settings for everything off in new paragraphs.
    defaultFormat.setFontFamily("Courier New");
    defaultFormat.setFontPointSize(12);

    // Italic off.
    defaultFormat.setFontItalic(false);

    // Super & subscript off.
    defaultFormat.setVerticalAlignment(QTextCharFormat::AlignNormal);

    // Bold off.
    defaultFormat.setFontWeight(QFont::Normal);

    // Underline off.
    defaultFormat.setFontUnderline(false);

    // Green or White font. Sadly has to be set in the
    // default character format, not in the default block format.
    // DOS files don't appear to have a text colour, so we use GREEN for those.
    defaultFormat.setForeground(Qt::black); // Because paper is pale yellow!

    /*----------------------------------------------------------------------
    // While I tried to use the original black paper and white or green ink
    // it seems that setting the paper to black means that the text carat
    // becomes invisible and this is not yet able to be changed. Sigh.

    if (!fPCFile) {
        // Try to use QL text colour...
        if (((layoutTableQL *)fLayoutTable)->textColour == LAYOUT_TEXT_GREEN) {
            defaultFormat.setForeground(Qt::darkGreen);
        } else {
            defaultFormat.setForeground(Qt::black); // Because paper is pale yellow!
        }
    } else {
        // Use DOS settings - which doesn't have a text colour!
        defaultFormat.setForeground(Qt::darkGreen);
    }
    ----------------------------------------------------------------------*/

    // The current *character* format in this paragraph...
    QTextCharFormat charFormat;


    // Flags that toggle formatting of characters.
    bool SuperOn = false;
    bool SubOn = false;
    bool BoldOn = false;
    bool UnderOn = false;
    bool ItalicOn = false;

    // Set the current formats, plural, for the first (system created) paragraph.
    cursor.setBlockFormat(defaultBlockFormat);
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
             cursor.insertBlock(defaultBlockFormat, defaultFormat);

             // Because we may still have bold etc in the charFormat we need
             // to reinitialise it to the defaultFormat settings. We can do this
             // by reading the current format from the cursor. This stops rogue
             // formatting when the file doesn't *explicitly* turn off some
             // formatting between paragraphs. (Derek Stewart reported this bug.)
             charFormat = cursor.charFormat();
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

         default: cursor.insertText(QString(Char));     // Everything else.

       }
    }

    // Position carat at the start. It will become visible if you use the arrow keys!
    cursor.setPosition(0);
}

//------------------------------------------------------------------------------
// Extract the paragraph table.
//------------------------------------------------------------------------------
void QuillDoc::parseParagraphTable()
{
    // Set internal pointer to the paragraph table.
    if (fValid) {
        fParagraphTable = (paraTable *)(fRawFileContents.constData() + fTextLength);

        // Convert the contents to Windows/Linux little endian format.
        // Anything that's not 8 bit anyway!
        fParagraphTable->textOffset = big2Little32(fParagraphTable->textOffset);
        fParagraphTable->textLength = big2Little16(fParagraphTable->textLength);
    }
}

//------------------------------------------------------------------------------
// Extract the free space table - which is ignored.
//------------------------------------------------------------------------------
void QuillDoc::parseFreeSpaceTable()
{
    // Do nothing.
}

//------------------------------------------------------------------------------
// Extract the layout table.
//------------------------------------------------------------------------------
void QuillDoc::parseLayoutTable()
{

    // Set internal pointer to the layout table. Assume QL format.
    if (fValid) {
        if (!fPCFile) {
            fLayoutTableQL = (layoutTableQL *)(fRawFileContents.constData() + fTextLength +
                                               fParaTableLength +
                                               fFreeSpaceLength);

            // Convert the contents to Windows/Linux little endian format.
            // Anything that's not 8 bit anyway!
            fLayoutTableQL->wordCount = big2Little16(fLayoutTableQL->wordCount);
            fLayoutTableQL->tabAreaSize = big2Little16(fLayoutTableQL->tabAreaSize);
            fLayoutTableQL->tabAreaUsed = big2Little16(fLayoutTableQL->tabAreaUsed);
        } else {
            fLayoutTableDOS = (layoutTableDOS *)(fRawFileContents.constData() + fTextLength +
                                                 fParaTableLength +
                                                 fFreeSpaceLength);

            // DOS format files are already in little Endian format!
        }

    }
}

//------------------------------------------------------------------------------
// return a pointer to the text edit's document.
//------------------------------------------------------------------------------
QTextDocument *QuillDoc::getDocument()
{
    return document;
}


//------------------------------------------------------------------------------
// Returns a Windows (iso-latin) character for a given QDOS character.
//------------------------------------------------------------------------------
quint8  QuillDoc::translate(const quint8 c)
{
    static quint8 Dos2QL[] = {  // DOS char, QL Char
            127, 127, // UNCHANGED
            128, 168, // C cedilla
            129, 135, // u umlaut
            130, 131, // e acute
            131, 142, // a circumflex
            132, 128, // a umlaut
            133, 141, // a grave
            134, 130, // a acute
            135, 136, // c cdilla
            136, 145, // e circumflex
            137, 143, // e umlaut
            138, 144, // e grave
            139, 146, // i umlaut
            140, 149, // i circumflex
            141, 148, // i grave
            142, 160, // A umlaut
            143, 143, // UNCHANGED (A circle)
            144, 163, // E acute
            145, 138, // ae ligature
            146, 170, // AE ligature
            147, 152, // o circumflex
            148, 132, // o umlaut
            149, 151, // o grave
            150, 155, // u circumflex
            151, 154, // u grave
            152, 152, // UNCHANGED (y umlaut)
            153, 164, // O umlaut
            154, 167, // U umlaut
            155, 157, // cent
            156, 156, // UNCHANGED (�)
            157, 158, // Yen
            158, 158, // UNCHANGED (Pts)
            159, 159, // UNCHANGED (Italic f)
            160, 140, // a grave
            161, 147, // i acute
            162, 150, // o acute
            163, 153, // u acute
            164, 137, // n tilde
            165, 169, // N tilde
            166, 166, // UNCHANGED (a underbar)
            167, 167, // UNCHANGED (o underbar)
            168, 180, // iquestion
            169, 169, // UNCHANGED (Like 170 but reversed.)
            170, 170, // UNCHANGED (�)
            171, 171, // UNCHANGED (1/2)
            172, 172, // UNCHANGED (1/4)
            173, 173, // UNCHANGED (iExclamation)
            174, 184, // <<
            175, 185, // >>
            176, 176, // UNCHANGED (blob)
            177, 177, // UNCHANGED (blob)
            178, 178, // UNCHANGED (blob)
            179, 179, // UNCHANGED Box
            180, 180, // UNCHANGED Box
            181, 181, // UNCHANGED Box
            182, 182, // UNCHANGED Box
            183, 183, // UNCHANGED Box
            184, 184, // UNCHANGED Box
            185, 185, // UNCHANGED Box
            186, 186, // UNCHANGED Box
            187, 187}; // UNCHANGED Box

    static quint8 Ql2Win[] = {  // QL char, Windows char
           127, 169, // Copyright (c)
           128, 228, // a umlaut
           129, 227, // a tilde
           130, 229, // a circle
           131, 233, // e acute
           132, 246, // o umlaut
           133, 245, // o tilde
           134, 248, // o with /
           135, 252, // u umlaut
           136, 231, // c cedilla
           137, 241, // n tilde
           138, 230, // ae ligature
           139, 156, // oe Ligature
           140, 225, // a acute
           141, 224, // a grave
           142, 226, // a circumflex (a^)
           143, 235, // e umlaut
           144, 232, // e grave
           145, 234, // e curcumflex
           146, 239, // i umlaut
           147, 237, // i acute
           148, 236, // i grave
           149, 238, // � circumflex
           150, 243, // o acute
           151, 242, // o grave
           152, 244, // o circumflex
           153, 250, // u acute
           154, 249, // u grave
           155, 251, // u circumflex
           156, 223, // sz ligature (German B?)
           157, 162, // cent
           158, 165, // Yen
           159, 180, // acute
           160, 196, // A umlaut
           161, 195, // A tilde
           162, 194, // A circumflex
           163, 201, // E acute
           164, 214, // O umlaut
           165, 213, // O tilde
           166, 216, // O with /
           167, 220, // U umlaut
           168, 199, // C cedilla
           169, 209, // N tilde
           170, 198, // AE ligature
           171, 140, // OE Ligature
           172, 172, // ???
           173, 173, // �???
           174, 174, // ???
           175, 175, // ???
           176, 181, // Mu (micro as in u)
           177, 112, // Pi (as in p)
           178, 178, // ???
           179, 161, // iexclamation (upsside down !)
           180, 191, // iquestion (upside down ?)
           181, 128, // �Euro
           182, 167, // Section marker
           183, 183, // ???
           184, 171, // Double left arrow <<
           185, 187, // Double right arrow >>
           186, 176, // Degree
           187, 247}; // Divide

    // First, PC to QL translation. This is incomplete at present!
    quint8 cc = c;

    // If this is a PC file, convert from DOS to QL first...
    if (fPCFile) {
        // Pound Sterling sign (�)
        if (cc == 156) {cc = 96; goto ql_convert;}

        // Higher than 187 characters.
        switch (cc) {
           case 225: cc = 156; goto ql_convert; break; // sz ligature
           case 227: cc = 177; goto ql_convert; break; // Pi(co) (p)
           case 230: cc = 176; goto ql_convert; break; // micro (u)
           case 237: cc = 166; goto ql_convert; break; // O slash
           case 246: cc = 187; goto ql_convert; break; // Divide
           case 248: cc = 186; goto ql_convert; break; // Degree
        }

        // Unchanged characters
        if (cc < 127 || cc > 187) { goto ql_convert; } // Do nothing.

        // All the rest.
        cc = Dos2QL[ ((cc - 127) *2) +1];
    }

ql_convert:
    // Now convert from QL to Windows.
    // Pound Sterling sign (�).
    if (cc == 96) return 163;

    // Unchanged characters.
    if (cc < 127 || cc > 187) return cc;
        
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


//------------------------------------------------------------------------------
// Big Endian to Little Endian 16 bit word conversion.
//------------------------------------------------------------------------------
quint16 QuillDoc::big2Little16(quint16 big)
{
    ushort le;

    le =  ((0x00ff & big) << 8);
    le |= ((0xff00 & big) >> 8);

    return le;
}

//------------------------------------------------------------------------------
// Big Endian to Little Endian 32 bit long word conversion.
//------------------------------------------------------------------------------
quint32 QuillDoc::big2Little32(quint32 big)
{
    unsigned le;

    le =  ((0x000000ff & big) << 24);
    le |= ((0x0000ff00 & big) << 8);
    le |= ((0x00ff0000 & big) >> 8);
    le |= ((0xff000000 & big) >> 24);

    return le;
}
