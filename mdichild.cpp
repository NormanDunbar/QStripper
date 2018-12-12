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
//#include <QtDebug>

#include "mdichild.h"
#include "quill.h"

MdiChild::~MdiChild()
{
    if (Input) delete Input;
}

MdiChild::MdiChild()
{
    setAttribute(Qt::WA_DeleteOnClose);
    silentRunning = false;

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    connect(this, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
            this, SIGNAL(FormatChanged(const QTextCharFormat &)));

    // Try to make a decent size.
    setMinimumSize(QSize(400, 250));
}

// Reimplemented to force a call to FormatChanged() when we
// change child windows - this makes the buttons for fonts etc
// update properly.
// Unfortunately, this leaves us with an invisible cursor until we move it
// using an arrow key (or similar). Not good. Actually, it's a QT 'feature'
// as disabling this signal, still shows the problem.
void MdiChild::focusInEvent(QFocusEvent *event)
{
    if (event->gotFocus()) {
        emit FormatChanged(currentCharFormat());
    }
}


bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    Input = new QuillDoc(fileName);
    if (!Input->isValid()) {
      QMessageBox::critical(this, tr("QStripper"),
                            tr("This is not a Quill file.\nError message :\n\n") + QString(Input->getError()));
      return false;
    }

    // Use the quill document as our document.
    setDocument(Input->getDocument());
    setFocus();
    setCurrentFile(fileName);

    return true;
}

bool MdiChild::ExportText()
{
   QString fileName = TXTFile;
   if (fileName.isEmpty()) {
     fileName = filePath(curFile) + "/" +
                fileBasename(curFile) +
                ".txt";
   }

    if (!silentRunning)
        fileName = QFileDialog::getSaveFileName(this, tr("Export as plain text"), fileName, "Text Files (*.txt)");

    if (fileName.isEmpty())
        return false;
        
    if (fileExtension(fileName).toLower() != "txt")
        fileName += ".txt";

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write plain text file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextDocumentWriter txt;
    txt.setFormat("plaintext");
    txt.setCodec(QTextCodec::codecForName("UTF-8"));
    txt.setFileName(fileName);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    txt.write(document());
    QApplication::restoreOverrideCursor();

    TXTFile = fileName;
    document()->setModified(false);
    return true;
}

bool MdiChild::ExportHTML()
{
   QString fileName = HTMLFile;
   if (fileName.isEmpty()) {
     fileName = filePath(curFile) + "/" +
                fileBasename(curFile) +
                ".html";
   }

    if (!silentRunning)
        fileName = QFileDialog::getSaveFileName(this, tr("Export as HTML"), fileName, "HTML Files (*.html;*.htm)");

    if (fileName.isEmpty())
        return false;

    if (fileExtension(fileName).toLower() != "html")
        fileName += ".html";

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write HTML file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
/*
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    //-------------------------------------------------------------------
    // This works, but not very nicely. Need to add a method to make sure
    // that proper HTML is generated from the Quill document.
    //-------------------------------------------------------------------
    out << toHtml();
    QApplication::restoreOverrideCursor();
*/
    QTextDocumentWriter html;
    html.setFormat("HTML");
    html.setCodec(QTextCodec::codecForName("UTF-8"));
    html.setFileName(fileName);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    html.write(document());
    QApplication::restoreOverrideCursor();

    HTMLFile = fileName;
    document()->setModified(false);
    return true;
}

bool MdiChild::ExportPDF()
{
   QString fileName = PDFFile;
   if (fileName.isEmpty()) {
     fileName = filePath(curFile) + "/" +
                fileBasename(curFile) +
                ".pdf";
   }

    if (!silentRunning)
        fileName = QFileDialog::getSaveFileName(this, "Export PDF", fileName, "PDF files (*.pdf)");

    if (fileName.isEmpty())
        return false;

    if (fileExtension(fileName).toLower() != "pdf")
        fileName += ".pdf";

    QPrinter Pdf(QPrinter::HighResolution);
    Pdf.setOutputFormat(QPrinter::PdfFormat);
    Pdf.setOutputFileName(fileName);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    document()->print(&Pdf);
    QApplication::restoreOverrideCursor();

    PDFFile = fileName;
    document()->setModified(false);
    return true;
}

bool MdiChild::ExportODF()
{
   QString fileName = ODFFile;
   if (fileName.isEmpty()) {
     fileName = filePath(curFile) + "/" +
                fileBasename(curFile) +
                ".odf";
   }

    if (!silentRunning)
        fileName = QFileDialog::getSaveFileName(this, "Export ODF", fileName, "ODF files (*.odf)");

    if (fileName.isEmpty())
        return false;

    if (fileExtension(fileName).toLower() != "odf")
        fileName += ".odf";

    QTextDocumentWriter odf;
    odf.setFormat("odf");
    odf.setFileName(fileName);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    odf.write(document());
    QApplication::restoreOverrideCursor();

    ODFFile = fileName;
    document()->setModified(false);
    return true;
}

bool MdiChild::ExportDocbook()
{
   QString fileName = XMLFile;
   if (fileName.isEmpty()) {
     fileName = filePath(curFile) + "/" +
                fileBasename(curFile) +
                ".xml";
   }

    if (!silentRunning)
        fileName = QFileDialog::getSaveFileName(this, "Export DocBook", fileName, "XML files (*.xml)");

    if (fileName.isEmpty())
        return false;

    if (fileExtension(fileName).toLower() != "xml")
        fileName += ".xml";

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write DocBook XML file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    // Ask user for a title for the Article.
    bool ok = false;
    QString ArticleTitle;

    if (!silentRunning)
        ArticleTitle= QInputDialog::getText(this,
                                            tr("Enter DocBook Article Title"),
                                            tr("Please enter a title for the DocBook article"),
                                            QLineEdit::Normal, "", &ok);
    if (!ok) {
       ArticleTitle = "**** PUT YOUR TITLE HERE PLEASE ****";
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO 8859-15"));
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // XML header first.
    out << "<?xml version=\"1.0\" encoding=\"iso-8859-15\"?>\n";
    out << "<!DOCTYPE article PUBLIC \"-//OASIS//DTD DocBook XML V4.2//EN\"\n";
    out << "\"http://www.oasis-open.org/docbook/xml/4.2/docbookx.dtd\">\n";

    // Make this an article, so get a title from the user.
    out << "<article>\n";
    out << "<title>" << ArticleTitle << "</title>\n";

    // Iterate over all text blocks in the document and process each one
    // as a paragraph. Empty paragraphs are ignored.
    QTextBlock tb = document()->begin();
    while (tb.isValid()) {
        QString Paragraph = DocBookParagraph(tb);
        if (!Paragraph.isEmpty())
        out << "<para>" << Paragraph << "</para>\n";

        tb = tb.next();
    }

    // Finish off the article.
    out << "</article>\n";

    QApplication::restoreOverrideCursor();
    XMLFile = fileName;
    document()->setModified(false);
    return true;
}


// For each and every paragraph, iterate over each fragment of text,
// where we build up an XML 'statement'.
QString MdiChild::DocBookParagraph(const QTextBlock &ThisBlock)
{
    QString Paragraph;
    for (QTextBlock::iterator it = ThisBlock.begin(); !it.atEnd(); it++) {
      QTextFragment tf = it.fragment();
      Paragraph += DocBookFragment(tf);
    }

    return Paragraph;
}

// This is where we process each paragraph's text fragments and remove
// invalid XML characters.
//
// TODO : Foreign character translation isn't working yet and can cause
//        illegal characters in the XML file.
QString MdiChild::DocBookFragment(const QTextFragment &ThisFragment)
{
    QTextCharFormat Format = ThisFragment.charFormat();
    QString ThisText = ThisFragment.text();

    // We've got hard spaces, +/- etc in the text to translate.
    char Nbsp = 0xA0;
    char PlusMinus = 0xB1;
    QChar Euro = 0x20ac;

    if (!ThisText.isEmpty()) {
         // Do '&' first - so we don't change '&lt;' to '&amp;lt;' !
         ThisText.replace(QString("&"), QString("&amp;"));
         ThisText.replace(QString("<"), QString("&lt;"));
         ThisText.replace(QString(">"), QString("&gt;"));
         ThisText.replace(QString("\t"), QString("    "));
         ThisText.replace(QString(PlusMinus), QString("&plusmn;"));
         ThisText.replace(QString(Nbsp), QString(" "));
         //ThisText.replace(QString(Euro), QString("&euro;"));
    }

    // Here we try to decode what text attributes have been applied
    // and return a suitable XML 'statment' to accomodate them.
    if (Format.font().italic())
       return "<emphasis>" + ThisText + "</emphasis>";

    if (Format.font().underline())
       return "<emphasis role=\"underline\">" + ThisText + "</emphasis>";

    if (Format.font().bold())
       return "<emphasis role=\"bold\">" + ThisText + "</emphasis>";

     switch (Format.verticalAlignment()) {
        case QTextCharFormat::AlignNormal: return ThisText;
        case QTextCharFormat::AlignSuperScript: return "<superscript>" +
                                                       ThisText +
                                                       "</superscript>";
        case QTextCharFormat::AlignSubScript: return "<subscript>" +
                                                       ThisText +
                                                       "</supbscript>";
        case QTextCharFormat::AlignMiddle: break;
        case QTextCharFormat::AlignTop: break;
        case QTextCharFormat::AlignBottom: break;
        case QTextCharFormat::AlignBaseline: break;
     }
     
     return ThisText;
}


// Export a document in ReStructuredText, in UTF8 encoding.
bool MdiChild::ExportRST()
{
   QString fileName = RSTFile;
   if (fileName.isEmpty()) {
     fileName = filePath(curFile) + "/" +
                fileBasename(curFile) +
                ".rst";
   }

    if (!silentRunning)
        fileName = QFileDialog::getSaveFileName(this, "Export RST", fileName, "RST files (*.rst)");

    if (fileName.isEmpty())
        return false;

    if (fileExtension(fileName).toLower() != "rst")
        fileName += ".rst";

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write ReStructuredText (RST) file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    // Ask user for a title for the RST Document.
    bool ok = false;
    QString ArticleTitle;

    if (!silentRunning)
        ArticleTitle= QInputDialog::getText(this,
                                            tr("Enter Article Title"),
                                            tr("Please enter a title for the article"),
                                            QLineEdit::Normal, "", &ok);
    if (!ok) {
       ArticleTitle = "==========\n"
                      "YOUR TITLE\n"
                      "==========\n\n";
    } else {
        // Work out under and overlines for the title.
        int titleSize = ArticleTitle.size();
        QString overUnderLine = QString().fill('=', titleSize) + "\n";
        ArticleTitle = overUnderLine + ArticleTitle + "\n" + overUnderLine;
    }


    QTextStream out(&file);

    // Pandoc and other converters require UTF8.
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Make this an article, so get a title from the user.
    out << ArticleTitle;

    // Iterate over all text blocks in the document and process each one
    // as a paragraph. Empty paragraphs are ignored.
    QTextBlock tb = document()->begin();
    while (tb.isValid()) {
        QString Paragraph = RSTParagraph(tb);
        if (!Paragraph.isEmpty())
        out << endl << Paragraph << endl;

        tb = tb.next();
    }

    // Finish off the article.
    out << endl;

    QApplication::restoreOverrideCursor();
    RSTFile = fileName;
    document()->setModified(false);
    return true;
}


// For each and every paragraph, iterate over each fragment of text.
QString MdiChild::RSTParagraph(const QTextBlock &ThisBlock)
{
    QString Paragraph;
    for (QTextBlock::iterator it = ThisBlock.begin(); !it.atEnd(); it++) {
      QTextFragment tf = it.fragment();
      Paragraph += RSTFragment(tf);
    }

    return Paragraph;
}

// This is where we process each paragraph's text fragments and remove
// invalid RST characters.
QString MdiChild::RSTFragment(const QTextFragment &ThisFragment)
{
    QTextCharFormat Format = ThisFragment.charFormat();
    QString ThisText = ThisFragment.text();

    QString euroInput = QString(QChar(0x80));
    QString euroOutput = QString(QChar(0x20ac));  // Unicode U+20AC for Euro.

    if (!ThisText.isEmpty()) {
         // Do '\' first or else you get all sorts of stuff going wrong!
         // And '\' needs to be escaped, so becomes '\\' - don't forget!
         ThisText.replace(QString("\\"), QString("\\\\"));
         ThisText.replace(QString("_"), QString("\\_"));
         ThisText.replace(QString("*"), QString("\\*"));
         ThisText.replace(QString("$"), QString("\\$"));
         //ThisText.replace(QString("\t"), QString("    "));
         ThisText.replace(QString("`"), QString("\\`"));
         //ThisText.replace(euroInput, euroOutput);
    }

    // Here we try to decode what text attributes have been applied
    // and return a suitable XML 'statment' to accomodate them.
    // BEWARE: if an italic fragment has leading whitspace, the
    //         italics wont work in RST as no whitespace is permitted.
    if (Format.font().italic())
       ThisText = "*" + ThisText + "*\\ ";

    // There is no underline in RST. :-(
    if (Format.font().underline())
       ; // do nothing. (Unless we can fix RST of course!)

    // BEWARE: if a bold fragment has leading whitspace, the bold
    //         wont work in RST as no whitespace is permitted.
    if (Format.font().bold())
       ThisText = "**" + ThisText + "**\\ ";

     // These are mutuallu exclusive.
     switch (Format.verticalAlignment()) {
        case QTextCharFormat::AlignSuperScript:
            return ":sup:`" + ThisText + "`\\ ";
            break;
        case QTextCharFormat::AlignSubScript:
            return ":sub:`" + ThisText + "`\\ ";
            break;
        case QTextCharFormat::AlignNormal: break;
        case QTextCharFormat::AlignMiddle: break;
        case QTextCharFormat::AlignTop: break;
        case QTextCharFormat::AlignBottom: break;
        case QTextCharFormat::AlignBaseline: break;
     }

     return ThisText;
}


// Export a document in ASCIIDoc[tor], in UTF8 encoding.
bool MdiChild::ExportASC()
{
   QString fileName = ASCFile;
   if (fileName.isEmpty()) {
     fileName = filePath(curFile) + "/" +
                fileBasename(curFile) +
                ".adoc";
   }

    if (!silentRunning)
        fileName = QFileDialog::getSaveFileName(this, "Export ASCIIdoctor", fileName, "ASCIIdoctor files (*.adoc;*.ad;*.txt)");

    if (fileName.isEmpty())
        return false;

    if (fileExtension(fileName).toLower() != "adoc")
        fileName += ".adoc";

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QStripper"),
                             tr("Cannot write ASCIIdoctor (ASC) file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    // Ask user for a title for the ASC Document.
    bool ok = false;
    QString ArticleTitle;

    if (!silentRunning)
        ArticleTitle= QInputDialog::getText(this,
                                            tr("Enter Article Title"),
                                            tr("Please enter a title for the article"),
                                            QLineEdit::Normal, "", &ok);
    if (!ok) {
       ArticleTitle = "= YOUR TITLE\n";
    } else {
        ArticleTitle = "= " + ArticleTitle + "\n";
    }


    QTextStream out(&file);

    // Pandoc and other converters require UTF8.
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Make this an article, so use the title from the user.
    out << ArticleTitle;

    // Iterate over all text blocks in the document and process each one
    // as a paragraph. Empty paragraphs are ignored.
    QTextBlock tb = document()->begin();
    while (tb.isValid()) {
        QString Paragraph = ASCParagraph(tb);
        if (!Paragraph.isEmpty())
            out << endl << Paragraph << endl;

        tb = tb.next();
    }

    // Finish off the article.
    out << endl;

    QApplication::restoreOverrideCursor();
    ASCFile = fileName;
    document()->setModified(false);
    return true;
}


// For each and every paragraph, iterate over each fragment of text.
QString MdiChild::ASCParagraph(const QTextBlock &ThisBlock)
{
    QString Paragraph;
    for (QTextBlock::iterator it = ThisBlock.begin(); !it.atEnd(); it++) {
      QTextFragment tf = it.fragment();
      Paragraph += ASCFragment(tf);
    }

    return Paragraph;
}

// This is where we process each paragraph's text fragments and remove
// invalid ASCIIdoctor characters.
QString MdiChild::ASCFragment(const QTextFragment &ThisFragment)
{
    QTextCharFormat Format = ThisFragment.charFormat();
    QString ThisText = ThisFragment.text();

    QString euroInput = QString(QChar(0x80));
    QString euroOutput = QString(QChar(0x20ac));  // Unicode U+20AC for Euro.

    if (!ThisText.isEmpty()) {
         // Do '\' first or else you get all sorts of stuff going wrong!
         // And '\' needs to be escaped, so becomes '\\' - don't forget!
         //ThisText.replace(QString("\\"), QString("\\\\"));
         //ThisText.replace(QString("_"), QString("\\_"));
         //ThisText.replace(QString("*"), QString("\\*"));
         //ThisText.replace(QString("$"), QString("\\$"));
         //ThisText.replace(QString("\t"), QString("    "));
         //ThisText.replace(QString("`"), QString("\\`"));
         //ThisText.replace(euroInput, euroOutput);
    }

    // Here we try to decode what text attributes have been applied
    // and return a suitable XML 'statment' to accomodate them.
    // BEWARE: if an italic fragment has leading whitespace, the
    //         italics wont work in ASCIIdoctor as no whitespace is permitted.
    if (Format.font().italic()) {
       ThisText = "__" + ThisText + "__";
    }

    // There is no underline in ASCIIdoctor. :-(
    if (Format.font().underline()) {
       ; // Do nothing, until ASCIIdoctor is fixed.
    }

    // BEWARE: if a bold fragment has leading whitspace, the bold
    //         bold wont work in RST as no whitespace is permitted.
    if (Format.font().bold()) {
       ThisText =  "**" + ThisText + "**";
    }

     switch (Format.verticalAlignment()) {
        case QTextCharFormat::AlignSuperScript:
            return "^" + ThisText + "^";
            break;
        case QTextCharFormat::AlignSubScript:
            return "~" + ThisText + "~";
            break;
        case QTextCharFormat::AlignNormal: break;
        case QTextCharFormat::AlignMiddle: break;
        case QTextCharFormat::AlignTop: break;
        case QTextCharFormat::AlignBottom: break;
        case QTextCharFormat::AlignBaseline: break;
     }

     return ThisText;
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
    setCurrentCharFormat(Format);
    return Checked;
}


bool MdiChild::TextSuperScript(const bool Checked)
{
    QTextCharFormat Format = currentCharFormat();
    Format.setVerticalAlignment(Checked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal);
    setCurrentCharFormat(Format);
    return Checked;
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

bool MdiChild::maybeSave()
{
    if (document()->isModified()) {
        int ret = QMessageBox::warning(this, tr("QStripper"),
                     tr("'%1' has been modified.\n"
                        "Do you want to export your changes before exiting?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return false;
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
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

QString MdiChild::fileExtension(const QString &fullFileName)
{
    return QFileInfo(fullFileName).suffix();
}

QString MdiChild::fileBasename(const QString &fullFileName)
{
    return QFileInfo(fullFileName).baseName();
}

QString MdiChild::filePath(const QString &fullFileName)
{
    return QFileInfo(fullFileName).path();
}

void MdiChild::setSilent(bool silent)
{
    silentRunning = silent;
}
