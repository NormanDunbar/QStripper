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

#include "mainwindow.h"
#include "mdichild.h"
#include "ndworkspace.h"
#include "version.h"

MainWindow::MainWindow()
{
    workspace = new NDWorkspace;
    setCentralWidget(workspace);
    connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), workspace, SLOT(setActiveWindow(QWidget *)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowIcon(QIcon(":/images/quill.jpg"));
    setWindowTitle(tr("QStripper Open Source Edition"));
    fileName.clear();
    
    // Allow drops from Explorer etc.
    setAcceptDrops(true);

    // Try to make a decent size.
    workspace->setMinimumSize(QSize(600, 480));

    // Background Image.
    //QBrush mdiBrush(QColor(255, 255, 220));

    // Set up the default background imange.
    Jupiter = QImage(":/images/Jupiter.png");

    workspace->setScrollBarsEnabled(true);
}

// Slot to update the various Text Formatting Actions when the cursor moves
// over a 'new' format in any child window.
void MainWindow::FormatChanged(const QTextCharFormat &Format)
{
   TextBoldAct->setChecked(Format.font().bold());
   TextItalicAct->setChecked(Format.font().italic());
   TextUnderlineAct->setChecked(Format.font().underline());
   switch (Format.verticalAlignment()) {
     case QTextCharFormat::AlignNormal: TextSubscriptAct->setChecked(false);
                                        TextSuperscriptAct->setChecked(false);
                                        break;
     case QTextCharFormat::AlignSuperScript: TextSubscriptAct->setChecked(false);
                                             TextSuperscriptAct->setChecked(true);
                                             break;
     case QTextCharFormat::AlignSubScript: TextSubscriptAct->setChecked(true);
                                           TextSuperscriptAct->setChecked(false);
                                           break;

     // These just avoid warnings about unused elephants.
     case QTextCharFormat::AlignMiddle: break;
     case QTextCharFormat::AlignTop: break;
     case QTextCharFormat::AlignBottom: break;
     case QTextCharFormat::AlignBaseline: break;
   }

   comboFont->setCurrentIndex(comboFont->findText(Format.fontFamily()));
   comboSize->setCurrentIndex(comboSize->findText(QString::number(Format.fontPointSize())));
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // Simply accept dropped files - the openFile
    // slot will sort them out if invalid.
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    // Extract a list of (dropped) filenames and  open them.
    QList<QUrl> fileNames;
    QString fileName;
    QList<QUrl>::const_iterator i;

    if (event->mimeData()->hasUrls()) {
      fileNames = event->mimeData()->urls();
       for (i = fileNames.begin(); i != fileNames.end(); i++) {
         fileName = (*i).toLocalFile();
         openFile(fileName);
       }
    }

    // Cascade the current open windows.
    workspace->cascade();

    event->acceptProposedAction();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    workspace->closeAllWindows();
    if (activeMdiChild()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // When we resize the main window, we want the image to remain centered.

    // From here: http://stackoverflow.com/questions/18959083/qpainter-drawimage-centeraligned

    // The new background image is a different size. We also need to paint on it
    // so we need a painter too. The new image will be the new size of the window.
    QImage newJupiter(event->size(), QImage::Format_ARGB32_Premultiplied);
    newJupiter.fill(Qt::black);

    QPainter painter(&newJupiter);

    // Fetch the actual image size.
    QRect JupiterRect(Jupiter.rect());

    // Create a new image, correctly sized.
    QRect newRect(0, 0, painter.device()->width(), painter.device()->height());

    // Move the center of the image to the center of the new image.
    // This handles when the image is bigger than the window etc.
    //JupiterRect.moveCenter(newRect.center());
    JupiterRect.moveTop(newRect.top());

    // Draw the image, centered in the new winow's rectangle.
    painter.drawImage(JupiterRect.topLeft(), Jupiter);

    // And finally, paint the new image on the background.
    workspace->setBackground(newJupiter);

    // And we have to signal that the event has been processed.
    // So that it doesn't go back up to the parent (QMainWindow).
    event->accept();
}


void MainWindow::open()
{
    // Attempt to open multiple files from the last directory used.
    //fileName = QFileDialog::getOpenFileName(this, tr("Open Quill Document"), fileName);
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Quill Documents"), fileName);

    // Open all the files. This looks like only the final one opened ...
    QStringList::const_iterator ci;
    for (ci = fileNames.constBegin(); ci != fileNames.constEnd(); ++ci) {
        fileName = (*ci);
        openFile(fileName);
    }

    // ... so cascade the current open windows - only if more than one was opened.
    if (fileNames.size() != 1) {
        workspace->cascade();
    }
}

void MainWindow::openFile(const QString &fileName)
{
    // Anything to open ?
    if (fileName.isEmpty())
      return;

    // Already open ?
    MdiChild *existing = findMdiChild(fileName);
    if (existing) {
        workspace->setActiveWindow(existing);
        return;
    }

    // Must be a new one, open it.
    MdiChild *child = createMdiChild();
    if (child->loadFile(fileName)) {
        // Each child has a signal when the text format changes, hook this up to
        // our own slot so that we cah set/unset the various font formatting actions.
        connect(child, SIGNAL(FormatChanged(const QTextCharFormat &)), this, SLOT(FormatChanged(const QTextCharFormat &)));
        statusBar()->showMessage(tr("File loaded"), 2000);
        child->showMaximized();
    } else {
        child->close();
    }

}

void MainWindow::cut()
{
    activeMdiChild()->cut();
}

void MainWindow::copy()
{
    activeMdiChild()->copy();
}

void MainWindow::paste()
{
    activeMdiChild()->paste();
}

//-----------------------------------------------------------------------------
// Something wierd here. When we do an exportXXXX the active window is changed
// to the first open window in the list. The 'hack' below saves the current one
// the resets it after the export. I suspect, but have not proved, that it is
// propbably because the active window changes when the FileOpen dialogue is
// displayed.
//-----------------------------------------------------------------------------
void MainWindow::ExportText()
{
    MdiChild *x = activeMdiChild();
    x->ExportText();
    workspace->setActiveWindow(x);
}

void MainWindow::ExportHTML()
{
    MdiChild *x = activeMdiChild();
    x->ExportHTML();
    workspace->setActiveWindow(x);
}

void MainWindow::ExportPDF()
{
    MdiChild *x = activeMdiChild();
    x->ExportPDF();
    workspace->setActiveWindow(x);
}

void MainWindow::ExportODF()
{
    MdiChild *x = activeMdiChild();
    x->ExportODF();
    workspace->setActiveWindow(x);
}

void MainWindow::ExportDocbook()
{
    MdiChild *x = activeMdiChild();
    x->ExportDocbook();
    workspace->setActiveWindow(x);
}

void MainWindow::ExportRST()
{
    MdiChild *x = activeMdiChild();
    x->ExportRST();
    workspace->setActiveWindow(x);
}

void MainWindow::TextBold()
{
    activeMdiChild()->TextBold(TextBoldAct->isChecked());
}

void MainWindow::TextItalic()
{
    activeMdiChild()->TextItalic(TextItalicAct->isChecked());
}

void MainWindow::TextUnderline()
{
    activeMdiChild()->TextUnderline(TextUnderlineAct->isChecked());
}

// Superscript and SubScript are/should be mutually exclusive. To enable this, I can
// add them both to an ActionGroup which has been setExclusive. This works in that it
// does allow only one of the two actions to be 'checked'. Unfortunately, I cannot get
// them all off again. The ActionGroup has nothing like an AllowAllUp property, so I
// have to do it manually here.
// Bummer.
void MainWindow::TextSubScript()
{
    bool Checked = TextSubscriptAct->isChecked();
    activeMdiChild()->TextSubScript(Checked);
    TextSuperscriptAct->setChecked(false);

}

void MainWindow::TextSuperScript()
{
    bool Checked = TextSuperscriptAct->isChecked();
    activeMdiChild()->TextSuperScript(Checked);
    TextSubscriptAct->setChecked(false);
}

void MainWindow::TextSize(const QString &size)
{
    if (size.toFloat() > 0.0) {
        activeMdiChild()->setFontPointSize(size.toFloat());
    }
}


void MainWindow::TextFamily(const QString &family)
{
    activeMdiChild()->setFontFamily(family);
}


void MainWindow::FilePrint()
{
    activeMdiChild()->FilePrint();
}


void MainWindow::about()
{
   QMessageBox::about(this, tr("About QStripper"),
            tr("<h1><b>QSTRIPPER version "
               QSTRIPPER_VERSION
               "</b></h1><br>"
               "<b>QStripper</b> allows Windows and Linux (Unix too ?) users the ability to "
               "open multiple QL Quill documents and save them in various (other) formats."
               "<br>Hopefully, you'll be able to save (export) Quill documents in the following formats : "
               "<br><ul>"
               "<li>Text<li>Html<li>Docbook XML<li>PDF<li>ODF: Open Document Format for Open/Libre Office"
               "<li>RST: ReStructuredText"
               "</ul><hr>"
               "<b>Contact Details :</b><br><br>"
               "Web site : <b>http://qdosmsq.dunbar-it.co.uk</b><br>"
               "Email : Norman@Dunbar-it.co.uk<hr>"
               "The source code for this application is available from SourceForge, as follows, using Subversion:"
               "<br><br><b>svn checkout https://svn.code.sf.net/p/qstripper/code/ qstripper</b>"
               "<br><br>or as a daily snapshot (in tar format) from</br>"
               "<br><br><b>https://sourceforge.net/p/qstripper/code/HEAD/tarball</b>"));
}

void MainWindow::help()
{
   QMessageBox::about(this, tr("QStripper Commandline Help"),
            tr("<h1><b>QSTRIPPER version "
               QSTRIPPER_VERSION
               "</b></h1><br>"
               "<b>QStripper --help</b>"
               "<br>or<br>"
               "<b>QStripper [--export &lt;FORMAT&gt;] list_of_Quill_files</b><br>"
               "<br><b>--help</b> - displays this help page, and exits.<br>"
               "<br><b>--export</b> indicates that you wish to run silently and export the list of files to "
               "a desired format, pdf for example."
               "<br>"
               "<br>If --export is present, it <em>must</em> be the first parameter. It <em>must</em> also be followed "
               "by a valid export format, which must be one of the following:"
               "<br>"
               "<br><b>--pdf</b> - Export all files to pdf."
               "<br><b>--text</b> - Export all files to text."
               "<br><b>--odf</b> - Export all files to Libre Office odf format."
               "<br><b>--docbook</b> - Export all files to Docbook XML."
               "<br><b>--html</b> - Export all files to HTML format."
               "<br><b>--RST</b> - Export all files to ReStructuredText format."
               "<br><br>All files will be created in the <em>same folder as the input file(s).</em>"
               ));
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    pasteAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    TileHAct->setEnabled(hasMdiChild);
    TileVAct->setEnabled(hasMdiChild);
    ExportHTMLAct->setEnabled(hasMdiChild);
    ExportPDFAct->setEnabled(hasMdiChild);
    ExportODFAct->setEnabled(hasMdiChild);
    ExportTextAct->setEnabled(hasMdiChild);
    ExportDocbookAct->setEnabled(hasMdiChild);
    ExportRSTAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);
    TextBoldAct->setEnabled(hasMdiChild);
    TextItalicAct->setEnabled(hasMdiChild);
    TextUnderlineAct->setEnabled(hasMdiChild);
    TextSubscriptAct->setEnabled(hasMdiChild);
    TextSuperscriptAct->setEnabled(hasMdiChild);
    FilePrintAct->setEnabled(hasMdiChild);

    pasteAct->setEnabled(hasMdiChild);

    bool hasSelection = (activeMdiChild() && activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);

    comboFont->setEnabled(hasMdiChild);
    comboSize->setEnabled(hasMdiChild);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(TileHAct);
    windowMenu->addAction(TileVAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QWidget *> windows = workspace->windowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i));

        QString text;
        if (i < 9) {
            text = tr("&%1. %2").arg(i + 1)
                                .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1. %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, child);
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    workspace->addWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    return child;
}


void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    FilePrintAct = new QAction(QIcon(":/images/fileprint.png"), tr("&Print..."), this);
    FilePrintAct->setShortcut(tr("Ctrl+P"));
    FilePrintAct->setStatusTip(tr("Print the active document"));
    connect(FilePrintAct, SIGNAL(triggered()), this, SLOT(FilePrint()));

    exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setShortcut(tr("Ctrl+F4"));
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),  workspace, SLOT(closeActiveWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),
            workspace, SLOT(closeAllWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), workspace, SLOT(tile()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), workspace, SLOT(cascade()));

    TileHAct = new QAction(tr("Tile &horizontally"), this);
    TileHAct->setStatusTip(tr("Tile the windows horizontally"));
    connect(TileHAct, SIGNAL(triggered()), workspace, SLOT(tileHorozontally()));

    TileVAct = new QAction(tr("Tile &vertically"), this);
    TileVAct->setStatusTip(tr("Tile the windows vertically"));
    connect(TileVAct, SIGNAL(triggered()), workspace, SLOT(tileVertically()));


    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcut(tr("Ctrl+F6"));
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            workspace, SLOT(activateNextWindow()));

    previousAct = new QAction(tr("&Previous"), this);
    previousAct->setShortcut(tr("Ctrl+Shift+F6"));
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            workspace, SLOT(activatePreviousWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(QIcon(":/images/fileinfo.png"), tr("&About"), this);
    aboutAct->setShortcut(tr("Ctrl+F1"));
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(":/images/qt.png"), tr("About &Qt"), this);
    aboutQtAct->setShortcut(tr("Ctrl+Shift+F1"));
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    helpAct = new QAction(QIcon(":/images/fileinfo.png"), tr("&Help"), this);
    helpAct->setShortcut(tr("F1"));
    helpAct->setStatusTip(tr("Show the application's Help details"));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

    ExportTextAct = new QAction(QIcon(":/images/exporttxt.png"), tr("Export &Text"), this);
    ExportTextAct->setShortcut(tr("Ctrl+Shift+T"));
    ExportTextAct->setStatusTip(tr("Export the current file as plain text"));
    connect(ExportTextAct, SIGNAL(triggered()), this, SLOT(ExportText()));

    ExportHTMLAct = new QAction(QIcon(":/images/exporthtml.png"), tr("Export &HTML"), this);
    ExportHTMLAct->setShortcut(tr("Ctrl+Shift+H"));
    ExportHTMLAct->setStatusTip(tr("Export the current file as HTML"));
    connect(ExportHTMLAct, SIGNAL(triggered()), this, SLOT(ExportHTML()));

    ExportDocbookAct = new QAction(QIcon(":/images/exportxml.png"), tr("Export &DocBook XML"), this);
    ExportDocbookAct->setShortcut(tr("Ctrl+Shift+D"));
    ExportDocbookAct->setStatusTip(tr("Export the current file as DocBook XML"));
    connect(ExportDocbookAct, SIGNAL(triggered()), this, SLOT(ExportDocbook()));

    ExportPDFAct = new QAction(QIcon(":/images/exportpdf.png"), tr("Export &PDF"), this);
    ExportPDFAct->setShortcut(tr("Ctrl+Shift+P"));
    ExportPDFAct->setStatusTip(tr("Export the current file as PDF"));
    connect(ExportPDFAct, SIGNAL(triggered()), this, SLOT(ExportPDF()));

    ExportODFAct = new QAction(QIcon(":/images/exportodf.png"), tr("Export &ODF"), this);
    ExportODFAct->setShortcut(tr("Ctrl+Shift+O"));
    ExportODFAct->setStatusTip(tr("Export the current file as ODF"));
    connect(ExportODFAct, SIGNAL(triggered()), this, SLOT(ExportODF()));

    ExportRSTAct = new QAction(QIcon(":/images/exportrst.png"), tr("Export &RST"), this);
    ExportRSTAct->setShortcut(tr("Ctrl+Shift+R"));
    ExportRSTAct->setStatusTip(tr("Export the current file as RST"));
    connect(ExportRSTAct, SIGNAL(triggered()), this, SLOT(ExportRST()));

    TextBoldAct = new QAction(QIcon(":/images/textbold.png"), tr("&Bold"), this);
    TextBoldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    QFont bold;
    bold.setBold(true);
    TextBoldAct->setFont(bold);
    connect(TextBoldAct, SIGNAL(triggered()), this, SLOT(TextBold()));
    TextBoldAct->setCheckable(true);

    TextUnderlineAct = new QAction(QIcon(":/images/textunder.png"), tr("&Underline"), this);
    TextUnderlineAct->setShortcut(Qt::CTRL + Qt::Key_U);
    QFont Under;
    Under.setUnderline(true);
    TextUnderlineAct->setFont(Under);
    connect(TextUnderlineAct, SIGNAL(triggered()), this, SLOT(TextUnderline()));
    TextUnderlineAct->setCheckable(true);

    TextItalicAct = new QAction(QIcon(":/images/textitalic.png"), tr("&Italic"), this);
    TextItalicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont Italic;
    Italic.setItalic(true);
    TextItalicAct->setFont(Italic);
    connect(TextItalicAct, SIGNAL(triggered()), this, SLOT(TextItalic()));
    TextItalicAct->setCheckable(true);

    TextSubscriptAct = new QAction(QIcon(":/images/textsubscript.png"), tr("SubScript"), this);
    connect(TextSubscriptAct, SIGNAL(triggered()), this, SLOT(TextSubScript()));
    TextSubscriptAct->setCheckable(true);

    TextSuperscriptAct = new QAction(QIcon(":/images/textsuperscript.png"), tr("SuperScript"), this);
    connect(TextSuperscriptAct, SIGNAL(triggered()), this, SLOT(TextSuperScript()));
    TextSuperscriptAct->setCheckable(true);
}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(FilePrintAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    exportMenu = menuBar()->addMenu(tr("Ex&port"));
    exportMenu->addAction(ExportTextAct);
    exportMenu->addAction(ExportHTMLAct);
    exportMenu->addAction(ExportDocbookAct);
    exportMenu->addAction(ExportPDFAct);
    exportMenu->addAction(ExportODFAct);
    exportMenu->addAction(ExportRSTAct);

    textMenu = menuBar()->addMenu(tr("&Format"));
    textMenu->addAction(TextBoldAct);
    textMenu->addAction(TextItalicAct);
    textMenu->addAction(TextUnderlineAct);
    textMenu->addAction(TextSubscriptAct);
    textMenu->addAction(TextSuperscriptAct);

    // Not used at present.
    //toolsMenu = menuBar()->addMenu(tr("&Tools"));

    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(helpAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(FilePrintAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    exportToolBar = addToolBar(tr("Export"));
    exportToolBar->addAction(ExportTextAct);
    exportToolBar->addAction(ExportHTMLAct);
    exportToolBar->addAction(ExportDocbookAct);
    exportToolBar->addAction(ExportPDFAct);
    exportToolBar->addAction(ExportODFAct);
    exportToolBar->addAction(ExportRSTAct);

    textToolBar = addToolBar(tr("Format"));
    textToolBar->addAction(TextBoldAct);
    textToolBar->addAction(TextItalicAct);
    textToolBar->addAction(TextUnderlineAct);
    textToolBar->addAction(TextSubscriptAct);
    textToolBar->addAction(TextSuperscriptAct);

    toolsToolBar = addToolBar(tr("Tools"));
    //toolsToolBar->addAction(RenameQuillAct);

    // For now, use the tools toolbar for fonts.
    comboFont = new QFontComboBox(toolsToolBar);
    toolsToolBar->addWidget(comboFont);
    connect(comboFont, SIGNAL(activated(const QString &)),this, SLOT(TextFamily(const QString &)));

    comboSize = new QComboBox(toolsToolBar);
    comboSize->setObjectName("comboSize");
    toolsToolBar->addWidget(comboSize);
    comboSize->setEditable(true);

    QFontDatabase db;
    foreach(int size, db.standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(activated(const QString &)),this, SLOT(TextSize(const QString &)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("Dunbar IT Consultants Ltd", "QStripper");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(600, 480)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("Dunbar IT Consultants Ltd", "QStripper");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

MdiChild *MainWindow::activeMdiChild()
{
    return qobject_cast<MdiChild *>(workspace->activeWindow());
}

MdiChild *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *window, workspace->windowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window);
        if (mdiChild->currentFile() == canonicalFilePath)
            return mdiChild;
    }
    return 0;
}

// If we have any passed args on startup, process them here.
bool MainWindow::processArgs(int argc, char *argv[])
{
    // The commandline format is:
    //
    // qstripper --help
    //
    // or
    //
    // qstripper --export --fmt list_of_files
    //
    // Fmt is one of the following:
    // --pdf --docbook --odf --html --text --rst
    //

    // What's the fisrt argument passed?
    QString optionArg = QString(argv[1]).toLower();

    // Help requested?
    if (optionArg == "--help") {
        help();
        return true;
    }

    // Check if we are exporting next:
    if (optionArg == "--export") {
        // We are exporting!
        // Which export format?
        QString exportFormat = QString(argv[2]).toLower();

        // Valid format?
        if (exportFormat != "--pdf" &&
            exportFormat != "--docbook" &&
            exportFormat != "--text" &&
            exportFormat != "--odf" &&
            exportFormat != "--rst" &&
            exportFormat != "--html") {
            QMessageBox::critical(this, "QStripper - Invalid export format", QString(argv[2]) + " is not a valid export format!");
            return true;
        }

        // We have a valid format for export.

        // For each filename passed in argv[3] onwards, export to the
        // same folder, in the desired format.
        MdiChild *c = new MdiChild();
        c->setSilent(true);

        for (int Files = 3; Files < argc; Files++) {
            c->curFile = QString(argv[Files]);
            c->loadFile(c->curFile);

            if (exportFormat == "--pdf") {
                c->PDFFile.clear();
                c->ExportPDF();
            }

            if (exportFormat == "--docbook") {
                c->XMLFile.clear();
                c->ExportDocbook();
            }

            if (exportFormat == "--text") {
                c->TXTFile.clear();
                c->ExportText();
            }

            if (exportFormat == "--odf") {
                c->ODFFile.clear();
                c->ExportODF();
            }

            if (exportFormat == "--html") {
                c->HTMLFile.clear();
                c->ExportHTML();
            }

            // "Close" the file.
            c->clear();
        }

        // Finished from the working window.
        delete c;

        // Don't show the GUI.
        return true;

    } else {
        // Otherwise, we just have a list of files to open.
        for (int Files = 1; Files < argc; Files++) {
            openFile(argv[Files]);
        }

        // Then cascade the open windows.
        workspace->cascade();

        // Show the GUI.
        return false;
    }

}
