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

#include "quill.h"

const int Bold = 1;
const int Under = 2;
const int Subscript = 4;
const int Superscript = 8;


//------------------------------------------------------------------------------
// Constructor - opens the filename and reads in the raw data. As the data is
// in QDOS format, it is converted to 'proper' ASCII format as it is read. This
//is done with a simple translation table.
//------------------------------------------------------------------------------
QuillDoc::QuillDoc(QString FileName)
{
    ulong WhereAmI;
    quint8 Char;
    bool BoldOn;
    bool UnderOn;
    bool SubOn;
    bool SuperOn;
    QStack<int> stack;


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
    for (int x = 0; x < 8; ++x) {
       in >> Char;
       fQuillMagic.append(Char);
    }

    if (fQuillMagic != "vrm1qdf0") {
        fValid = false;
        fErrorMessage = QString("Header flag bytes not equal 'vrm1qdf0', actually = %1").arg(fQuillMagic);
        return;
    }

    // The next 4 bytes are big-endian pointer to the text length.
    in >> fTextLength;

    // Fetch the three 2 byte pointers.
    in >> fParaTableLength;
    in >> fFreeSpaceLength;
    in >> fLayoutTableLength;

    // Read the document header from position 20 up to the next zero byte.
    WhereAmI = 20;
    fHeader.clear();
    for (int x = 0; ; ++x) {
       in >> Char;
       Char = translate(Char);
       ++WhereAmI;
       if (Char == 0) break;
       fHeader.append(Char);
    }

    // Read the footer next, up to the next zero byte.
    fFooter.clear();
    for (int x = 0; ; ++x) {
       in >> Char;
       Char = translate(Char);
       ++WhereAmI;
       if (Char == 0) break;
       fFooter.append(Char);
    }

    // Now the contents.
    fText.clear();
    BoldOn = UnderOn = SuperOn = SubOn = false;
    bool FormatError = false;
    for (unsigned int x = WhereAmI; x < fTextLength; ++x) {
       in >> Char;
       Char = translate(Char);
       switch (Char) {
         case 0 : // Paragraph end & reset attributes.
                  BoldOn = UnderOn = SuperOn = SubOn = false;
                  while (!stack.isEmpty()) {
                    switch (stack.pop()) {
                      case Bold : fText.append("</B>"); break;
                      case Under : fText.append("</U>"); break;
                      case Subscript : fText.append("</SUB>"); break;
                      case Superscript : fText.append("</SUP>"); break;
                    }
                  }
                  fText.append("</P>\n<P>");
                  break;
         case 9 : fText.append("&nbsp;&nbsp;&nbsp;&nbsp;"); break;           // Tab - replace by 4 hard spaces
         case '<': fText.append("&lt;"); break;
         case '>': fText.append("&gt;"); break;
         case '&': fText.append("&amp;"); break;
         case 12: break;                                  // Form Feed - ignored.
         case 15: if (BoldOn) {
                    // Bold is currently on - check the stack as top one must be bold too.
                    if (stack.pop() != Bold) {
                      FormatError = true;
                      fText.append("</B>**** ERROR BOLD OFF OUT OF SEQUENCE *****");
                    } else {
                      fText.append("</B>");
                    }
                  } else {
                      // Bold is currently off - stack it and turn it on.
                      fText.append("<B>");
                      stack.push(Bold);
                  }
                  BoldOn = !BoldOn;
                  continue;
                  break;
         case 16: if (UnderOn) {
                    // Underline is currently on - check the stack as top one must be the same.
                    if (stack.pop() != Under) {
                      FormatError = true;
                      fText.append("</U>**** ERROR UNDERLINE OFF OUT OF SEQUENCE *****");
                    } else {
                      fText.append("</U>");
                    }
                  } else {
                      // Under is currently off - stack it and turn it on.
                      fText.append("<U>");
                    stack.push(Under);
                  }
                  UnderOn = !UnderOn;
                  continue;
                  break;
         case 17: if (SubOn) {
                    // Subscript is currently on - check the stack as top one must be the same.
                    if (stack.pop() != Subscript) {
                      FormatError = true;
                      fText.append("</SUB>**** ERROR SUBSCRIPT OFF OUT OF SEQUENCE *****");
                    } else {
                      fText.append("</SUB>");
                    }
                  } else {
                      // Subscript is currently off - stack it and turn it on.
                      fText.append("<SUB>");
                    stack.push(Subscript);
                  }
                  SubOn = !SubOn;
                  continue;
                  break;
         case 18: if (SuperOn) {
                    // Superscript is currently on - check the stack as top one must be the same.
                    if (stack.pop() != Superscript) {
                      FormatError = true;
                      fText.append("</SUP>**** ERROR SUPERSCRIPT OFF OUT OF SEQUENCE *****");
                    } else {
                      fText.append("</SUP>");
                    }
                  } else {
                      // Superscript is currently off - stack it and turn it on.
                      fText.append("<SUP>");
                    stack.push(Superscript);
                  }
                  SuperOn = !SuperOn;
                  continue;
                  break;
         case 30: continue; break;                       // Soft hyphen - ignored.
         case 127: fText.append("&copy;"); break;        // (c) symbol.
         case 228: fText.append("&euro;"); break;        // Euro symbol.
         case '£': fText.append("&pound;"); break;       // £ for UK currency.
         default: fText.append(Char);                    // Pass through.
       }
    }
    file.close();
    fValid = true;
    fErrorMessage = "";

    // Now, read in the entire file as a QByteArray.
    file.open(QIODevice::ReadOnly);
    fRawFileContents = file.readAll();
    file.close();

}


//------------------------------------------------------------------------------
// Returns a Windows (iso-latin) character for a given QDOS character.
//------------------------------------------------------------------------------
quint8  QuillDoc::translate(const quint8 c)
{
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

    // Unchanged characters.
    if (c < 127 || c > 187) return c;
        
    // Pound Sterling sign (£).       
    if (c == 96) return 163;   
        
    // All the rest.
    return (Ql2Win[ ((c - 127) * 2) + 1 ]);
}


//------------------------------------------------------------------------------
// Returns a QString holding the body text from the original Quill document.
//------------------------------------------------------------------------------
QString QuillDoc::getText()
{
    return fText;
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
