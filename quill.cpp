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
#include <QStack>


QuillDoc::QuillDoc(const QString &Filename)
{
  
  /*
    // Read in a Quill doc file - assuming we have one so check first.
    // The first two bytes are ignored.
    // The next 8 bytes are "vrm1qdf0"
    // The next 4 bytes are 'big-endian' long pointer to the text length.
    // The next 6 bytes are ignored.
    // The first paragraph is the doc header and starts at position 20 and
    // finishes at a byte of zero.
    // The second one is the doc footer which also finishes on a zero byte.
    // The rest are the doc contents. This starts at byte 20 if no header and footer
    // and ends just before the end pointer.

    QString Header;
    QString Footer;
    QString Contents;
    quint32 Pointer = 0;
    ulong WhereAmI = 20;
    quint8 Char;
    bool BoldOn = false;
    bool UnderOn = false;
    bool SubOn = false;
    bool SuperOn = false;
    QStack<int> stack;


    // The first two bytes are ignored.
    in.skipRawData(2);

    // The next 8 bytes are "vrm1qdf0"
    for (int x = 0; x < 8; ++x) {
       in >> Char;
       Header.append(Char);
    }

    if (Header != "vrm1qdf0") {
       return "";
    }

    // The next 4 bytes are big-endian pointer to the text length.
    in.setByteOrder(QDataStream::BigEndian);
    in >> Pointer;

    // Read the header from position 20 up to the next zero byte.
    in.skipRawData(6);
    Header.clear();

    for (int x = 0; ; ++x) {
       in >> Char;
       ++WhereAmI;
       if (Char == 0) break;
       Header.append(Char);
    }

    // Read the footer next, up to the next zero byte.
    for (int x = 0; ; ++x) {
       in >> Char;
       ++WhereAmI;
       if (Char == 0) break;
       Footer.append(Char);
    }

    Contents.append("<P>");

    FormatError = false;
    for (int x = WhereAmI; x < Pointer; ++x) {
       in >> Char;
       switch (Char) {
         case 0 : Contents.append("</P>\n<P>"); break;    // Paragraph end.
//         case 9 : break;                         // Tab - passed through. (Default)
         case 12: break;                           // Form Feed - ignored.
         case 15: if (BoldOn) {
                    // Bold is currently on - check the stack as top one must be bold too.
                    if (stack.pop() != Bold) {
                      FormatError = true;
                      Contents.append("</B>**** ERROR BOLD OFF OUT OF SEQUENCE *****");
                    } else {
                      Contents.append("</B>");
                    }
                  } else {
                      // Bold is currently off - stack it and turn it on.
                      Contents.append("<B>");
                      stack.push(Bold);
                  }
                  BoldOn = !BoldOn;
                  continue;
                  break;
         case 16: if (UnderOn) {
                    // Underline is currently on - check the stack as top one must be the same.
                    if (stack.pop() != Under) {
                      FormatError = true;
                      Contents.append("</U>**** ERROR UNDERLINE OFF OUT OF SEQUENCE *****");
                    } else {
                      Contents.append("</U>");
                    }
                  } else {
                      // Under is currently off - stack it and turn it on.
                      Contents.append("<U>");
                    stack.push(Under);
                  }
                  UnderOn = !UnderOn;
                  continue;
                  break;
         case 17: if (SubOn) {
                    // Subscript is currently on - check the stack as top one must be the same.
                    if (stack.pop() != Subscript) {
                      FormatError = true;
                      Contents.append("</SUB>**** ERROR SUBSCRIPT OFF OUT OF SEQUENCE *****");
                    } else {
                      Contents.append("</SUB>");
                    }
                  } else {
                      // Subscript is currently off - stack it and turn it on.
                      Contents.append("<SUB>");
                    stack.push(Subscript);
                  }
                  SubOn = !SubOn;
                  continue;
                  break;
         case 18: if (SuperOn) {
                    // Superscript is currently on - check the stack as top one must be the same.
                    if (stack.pop() != Superscript) {
                      FormatError = true;
                      Contents.append("</SUP>**** ERROR SUPERSCRIPT OFF OUT OF SEQUENCE *****");
                    } else {
                      Contents.append("</SUP>");
                    }
                  } else {
                      // Superscript is currently off - stack it and turn it on.
                      Contents.append("<SUP>");
                    stack.push(Superscript);
                  }
                  SuperOn = !SuperOn;
                  continue;
                  break;
         case 30: continue; break;                 // Soft hyphen - ignored.
         default: Contents.append(Char);           // Pass through.
       }
    }

    Contents.append("</P>");
    return "Header = " + Header +
           "\n\n\n\nContents = " + Contents +
           "\n\n\n\nFooter = " + Footer;
    */
}
