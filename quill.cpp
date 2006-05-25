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
    bool BoldOn = false;
    bool UnderOn = false;
    bool SubOn = false;
    bool SuperOn = false;
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

    // Skip the three 2 byte pointers - we don't need them.
    in.skipRawData(6);

    // Read the document header from position 20 up to the next zero byte.
    WhereAmI = 20;
    fHeader.clear();
    for (int x = 0; ; ++x) {
       in >> Char;
       ++WhereAmI;
       if (Char == 0) break;
       fHeader.append(Char);
    }

    // Read the footer next, up to the next zero byte.
    fFooter.clear();
    for (int x = 0; ; ++x) {
       in >> Char;
       ++WhereAmI;
       if (Char == 0) break;
       fFooter.append(Char);
    }

    // Now the contents.
    fText.clear();
    bool FormatError = false;
    for (unsigned int x = WhereAmI; x < fTextLength; ++x) {
       in >> Char;
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
         case 96: fText.append("&pound;"); break;     // £ for UK currency
         case 127: fText.append("&copy;"); break;
         default: fText.append(Char);                 // Pass through.
       }
    }

     fValid = true;
     fErrorMessage = "";
}


//------------------------------------------------------------------------------
// Returns a QString holding the body text from the original Quill document.
//------------------------------------------------------------------------------
QString QuillDoc::getText()
{
    return fText;
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
