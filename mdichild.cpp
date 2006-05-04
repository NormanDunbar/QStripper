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

#include <QtGui>
#include <QStack>

#include "mdichild.h"

MdiChild::MdiChild()
{
    setAttribute(Qt::WA_DeleteOnClose);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    QString Contents;
    bool Error = false;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // setPlainText(in.readAll()); - if only it were that easy :o)
    Contents = ReadQuillFile(in, Error);
    QApplication::restoreOverrideCursor();

    if (Contents.isEmpty()) {
      QMessageBox::critical(this, tr("QStripper"), tr("This is not a Quill file."));
      return false;
    }
    
    if (Error) {
       QMessageBox::warning(this, tr("QStripper"), 
                            tr("Bold, Underline, Subscript or Superscript off control code\n"
                               "found out of sequence - see text for details"));
    }

    //setPlainText(Contents);
    setHtml(Contents);
    Contents.clear();

    setCurrentFile(fileName);
    return true;
}

const int Bold = 1;
const int Under = 2;
const int Subscript = 4;
const int Superscript = 8;

QString MdiChild::ReadQuillFile(QDataStream &in, bool &FormatError)
{
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

}

bool MdiChild::save()
{
    // Not used, export file instead
    return false;
    /*
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
    */
}

bool MdiChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MdiChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

bool MdiChild::ExportText()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export As Plain Text"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write plain text file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    //-------------------------------------------------------------------
    // This works, but not very nicely. Need to add a method to make sure
    // that proper HTML is generated from the Quill document.
    //-------------------------------------------------------------------
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    //setCurrentFile(fileName);
    return true;
}

bool MdiChild::ExportHTML()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export As HTML"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write HTML file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    //-------------------------------------------------------------------
    // This works, but not very nicely. Need to add a method to make sure
    // that proper HTML is generated from the Quill document.
    //-------------------------------------------------------------------
    out << toHtml();
    QApplication::restoreOverrideCursor();

    //setCurrentFile(fileName);
    return true;
}

bool MdiChild::ExportPDF()
{
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
    if (fileName.isEmpty())
        return false;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    document()->print(&printer);
    return true;
#endif
}

bool MdiChild::ExportDocbook()
{
    return false;
}

QString MdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MdiChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool MdiChild::maybeSave()
{
    if (document()->isModified()) {
        int ret = QMessageBox::warning(this, tr("QStripper"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
