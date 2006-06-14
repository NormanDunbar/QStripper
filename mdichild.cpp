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
#include "quill.h"

MdiChild::MdiChild()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFontPointSize(16.0);

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    connect(this, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
            this, SIGNAL(FormatChanged(const QTextCharFormat &)));


}

// Reimplemented to force a call to FormatChanged() when we
// change child windows - this makes the buttons for fonts etc
// update properly.
// Unfortunately, this leaves us with an invisible cursor until we move it
// using an arrow key (or similar). Not good. Actually, it's a QT 'feature'
// as disabling this signal, still shows the problem.
void MdiChild::focusInEvent(QFocusEvent*)
{
    emit FormatChanged(currentCharFormat());
}


bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    QuillDoc *Input = new QuillDoc(fileName);
    if (!Input->isValid()) {
      QMessageBox::critical(this, tr("QStripper"),
                            tr("This is not a Quill file.\nError message :\n\n") + QString(Input->getError()));
      delete Input;
      return false;
    }

    //setPlainText(Contents);
    setHtml(Input->getHeader() + QString("<hr>") +
            Input->getText() + QString("<hr>") +
            Input->getFooter());

    delete Input;
    setCurrentFile(fileName);
    return true;
}

const int Bold = 1;
const int Under = 2;
const int Subscript = 4;
const int Superscript = 8;

bool MdiChild::ExportText()
{
   static QString fileName;
   fileName = QFileDialog::getSaveFileName(this, tr("Export as plain text"), fileName, "Text Files (*.txt);;All files (*.*)");
  // QString fileName = QFileDialog::getSaveFileName(this, tr("Export As Plain Text"), curFile);
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
    static QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Export as HTML"), fileName, "HTML Files (*.html;*.htm);;All files (*.*)");
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
    static QString FileName;
    FileName = QFileDialog::getSaveFileName(this, "Export PDF", FileName, "PDF files (*.pdf);;All files (*.*)");
    if (FileName.isEmpty())
        return false;

    QPrinter Pdf(QPrinter::HighResolution);
    Pdf.setOutputFormat(QPrinter::PdfFormat);
    Pdf.setOutputFileName(FileName);
    document()->print(&Pdf);
    return true;
}

bool MdiChild::ExportDocbook()
{
    QString FileName;
    FileName = QFileDialog::getSaveFileName(this, "Export PDF", FileName, "XML files (*.xml);;All files (*.*)");
    if (FileName.isEmpty())
        return false;

    QFile file(FileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write DocBook XML file %1:\n%2.")
                             .arg(FileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // XML header first.
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<!DOCTYPE article PUBLIC \"-//OASIS//DTD DocBook XML V4.2//EN\"\n";
    out << "\"http://www.oasis-open.org/docbook/xml/4.2/docbookx.dtd\">\n";

    // Make this an article, or book, or ...
    out << "<article>\n";
    out << "<title>" << "TITLE HERE" << "</title>\n";

    // We must have at least one section ...
    //out << "<section>\n";
    //out << "<title>" << FileName << "</title>\n";

    // We've got hard spaces, +/- etc in the text !
    char Nbsp = 0xa0;
    char PlusMinus = 0xB1;

    // The content is plain paragraphs.
    for (QTextBlock ThisBlock = document()->begin();
                    ThisBlock != document()->end();
                    ThisBlock = ThisBlock.next()) {
      QString ThisPara = ThisBlock.text();

      if (!ThisPara.isEmpty()) {
         // Do this first - so we don't change &lt; to &amp;lt; !
         ThisPara.replace(QString("&"), QString("&amp;"));
         ThisPara.replace(QString("<"), QString("&lt;"));
         ThisPara.replace(QString(">"), QString("&gt;"));
         ThisPara.replace(QString("\t"), QString("    "));
         ThisPara.replace(QString(PlusMinus), QString("&plusmn;"));
         ThisPara.replace(QString(Nbsp), QString(" "));

         out << "<para>" << ThisPara << "</para>\n\n";
      }
    }

    //out << "</section>\n";
    out << "</article>\n";

    QApplication::restoreOverrideCursor();
    return true;
}


bool MdiChild::FilePrint()
{
    QTextDocument *doc = document();
    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return false;

    doc->print(&printer);
    return true;
}


bool MdiChild::TextBold(const bool Checked)
{
    QTextCharFormat Format = currentCharFormat();
    Format.setFontWeight(Checked ? QFont::Bold : QFont::Normal);
    setCurrentCharFormat(Format);
    return Checked;
}


bool MdiChild::TextItalic(const bool Checked)
{
    QTextCharFormat Format = currentCharFormat();
    Format.setFontItalic(Checked);
    setCurrentCharFormat(Format);
    return Checked;
}


bool MdiChild::TextUnderline(const bool Checked)
{
    QTextCharFormat Format = currentCharFormat();
    Format.setFontUnderline(Checked);
    setCurrentCharFormat(Format);
    return Checked;
}

bool MdiChild::TextSubScript(const bool Checked)
{
    QTextCharFormat Format = currentCharFormat();

    Format.setVerticalAlignment(Checked ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal);

    // Subscript seems to get coloured grey and made very small - fix this !
    // Setting point size to 12.0 results in the same sized text as normal - whihc is 16.0 :o(
    Format.setFontPointSize(12.0);
    setCurrentCharFormat(Format);
    return Checked;
}


bool MdiChild::TextSuperScript(const bool Checked)
{
    QTextCharFormat Format = currentCharFormat();

    Format.setVerticalAlignment(Checked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal);

    // Superscript seems to get coloured grey and made very small - fix this !
    // Setting point size to 12.0 results in the same sized text as normal - whihc is 16.0 :o(
    Format.setFontPointSize(12.0);
    setCurrentCharFormat(Format);
    return Checked;
}


QString MdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MdiChild::documentWasModified()
{
    setWindowModified(document()->isModified());
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
