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
    fLayoutTableQL = nullptr;
    fLayoutTableDOS = nullptr;
    fParagraphTable = nullptr;
    fTabTable = nullptr;

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
    QChar qChar;

    // Header first.
    fRawPointer = 20;       // Always the start of the text area.

     do {
       Char = fRawFileContents[fRawPointer++];  // Points to NEXT character now.
       if (Char == 0) break;
       qChar = translate(Char);
       fHeader.append(qChar);
    } while (1);

    // Footer next.
    do {
       Char = fRawFileContents[fRawPointer++];  // Points to NEXT character now.
       if (Char == 0) break;
       qChar = translate(Char);
       fFooter.append(qChar);
    } while (1);

    // The actual text comes next. We stop when we reach offset fTextLength as
    // that is the first byte of the following Paragraph table.

    // We need a cursor to keep a handle on our insertion position.
    QTextCursor cursor(document);

    // Create a *paragraph* level default. This can take TABs too! (QTextOption::Tab)
    // TODO: Work out how to use the tabs. Decimal? Right? Left?
    QTextBlockFormat defaultBlockFormat;

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
       qChar = translate(Char);

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

         case 16: if (UnderOn) {
                    charFormat.setFontUnderline(false);
                } else {
                    charFormat.setFontUnderline(true);
                }
                UnderOn = !UnderOn;

                cursor.setCharFormat(charFormat);
                continue;

         case 17: if (SubOn) {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                  } else {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignSubScript);
                  }
                  SubOn = !SubOn;

                cursor.setCharFormat(charFormat);
                continue;

         case 18: if (SuperOn) {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                  } else {
                    charFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
                  }
                  SuperOn = !SuperOn;

                cursor.setCharFormat(charFormat);
                continue;

         case 19: if (ItalicOn) {
                    charFormat.setFontItalic(false);
                  } else {
                    charFormat.setFontItalic(true);
                  }

                  ItalicOn = !ItalicOn;
                  cursor.setCharFormat(charFormat);
                  continue;

         case 30: continue;                     // Soft hyphen - ignored.

         default: cursor.insertText(qChar);     // Everything else.

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
// Returns a QChar (in Unicode) for a given PC/QDOS character.
//------------------------------------------------------------------------------
QChar  QuillDoc::translate(const quint8 c)
{
    // Stolen from http://svn.openmoko.org/trunk/src/host/qemu-neo1973/phonesim/lib/serial/qatutils.cpp

    // The following table converts any DOS CP437 character into
    // a suitable Unicode character. I'm not sure what the EURO will
    // do as that was never part of CP437!

    static ushort DOS2Unicode[256] = {
         0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
         0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
         0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
         0x0018, 0x0019, 0x001c, 0x001b, 0x007f, 0x001d, 0x001e, 0x001f,
         0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
         0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
         0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
         0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
         0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
         0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
         0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
         0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
         0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
         0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
         0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
         0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x001a,
         0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
         0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
         0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
         0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192,
         0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
         0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
         0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
         0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
         0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
         0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
         0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
         0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
         0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x03bc, 0x03c4,
         0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
         0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
         0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0
    };



    // MOST of the QL Character set is valid ASCII, so anything under 127 or over
    // 188 should be returned as a QChar with the same ASCII code.
    // Anything else needs converting to Unicode.

    static ushort QL2Unicode[] = {  // QL char, Windows char
         169, // Copyright (c)    (127)
         228, // a umlaut (128)
         227, // a tilde  (129)
         229, // a circle (130)
         233, // e acute  (131)
         246, // o umlaut (132)
         245, // o tilde  (133)
         248, // o with / (134)
         252, // u umlaut (135)
         231, // c cedilla    (136)
         241, // n tilde  (137)
         230, // ae ligature  (138)
         339, // oe Ligature (Deprecated in Unicode)  (139)
         225, // a acute  (140)
         224, // a grave  (141)
         226, // a circumflex (a^)    (142)
         235, // e umlaut (143)
         232, // e grave  (144)
         234, // e circumflex (145)
         239, // i umlaut (146)
         237, // i acute  (147)
         236, // i grave  (148)
         238, // i circumflex (149)
         243, // o acute  (150)
         242, // o grave  (151)
         244, // o circumflex (152)
         250, // u acute  (153)
         249, // u grave  (154)
         251, // u circumflex (155)
         223, // sz ligature (German B?)  (156)
         162, // cent (157)
         165, // Yen  (158)
         180, // acute    (159)
         196, // A umlaut (160)
         195, // A tilde  (161)
         194, // A circumflex (162)
         201, // E acute  (163)
         214, // O umlaut (164)
         213, // O tilde  (165)
         216, // O with / (166)
         220, // U umlaut (167)
         199, // C cedilla    (168)
         209, // N tilde  (169)
         198, // AE ligature  (170)
         338, // OE Ligature (Deprecated in Unicode)  (171)
         0x03B1, // Lower case alpha. (172)
         0x03B4, // Lower case delta  (173)
         0x0398, // Upper case theta  (174)
         0x03BB, // Lower case lambda (175)
         0x03BC, // Lower case Mu (micro as in u) (176)
         0x03C6, // Lower case phi (as in p)  (177)
         0x03A6, // Upper case phi    (178)
         161, // iexclamation (upside down !) (179)
         191, // iquestion (upside down ?)    (180)
         0x20AC, // Euro  (181)
         167, // Section marker   (182)
         0x2295, // Cross circle ???????????????????????????  (183)
         171, // French Quote <<  (184)
         187, // French Quote >>  (185)
         176, // Degree   (186)
         247  // Divide  (187)
    };


    // A DOS file is "supposed" to be in Code Page 437 encoding. If we have
    // a DOS file, then the DOS2Unicode table above should (!) be correct.


    quint8 cc = c;

    // Usually, I suspect, we have to convert a QL file, not PC:
    if (!fPCFile) {
        if (cc == 96) return QChar(163);                // Pound Sterling.
        if (cc < 127 || cc > 187) return QChar(cc);     // Unchanged.
        return QChar(QL2Unicode[(cc - 127)]);           // Translated.
    } else {
        // Must be a PC file.
        return QChar(DOS2Unicode[cc]);
    }
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
