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

MainWindow::MainWindow()
{
    workspace = new NDWorkspace;
    setCentralWidget(workspace);
    connect(workspace, SIGNAL(windowActivated(QWidget *)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)),
            workspace, SLOT(setActiveWindow(QWidget *)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("QStripper Open Source Edition"));
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

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        MdiChild *existing = findMdiChild(fileName);
        if (existing) {
            workspace->setActiveWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
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

void MainWindow::ExportText()
{
    activeMdiChild()->ExportText();
}

void MainWindow::ExportHTML()
{
    activeMdiChild()->ExportHTML();
}

void MainWindow::ExportPDF()
{
    activeMdiChild()->ExportPDF();
}

void MainWindow::ExportDocbook()
{
    activeMdiChild()->ExportDocbook();
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
// add them both to an ActionGroup whihc has been setExclusive. This works in that it
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


void MainWindow::FilePrint()
{
    activeMdiChild()->FilePrint();
}


void MainWindow::about()
{
   QMessageBox::about(this, tr("About QStripper"),
            tr("<b>QStripper</b> allows Windows and Linux (Unix too ?) users the ability to "
               "open multiple QL Quill documents and save them in various (other) formats."
               "<br>Hopefully, you'll be able to save (export) Quill documents in the following formats : "
               "<br><br><ul><li>Text<li>Html<li>Docbook XML<li>PDF</ul><hr>"
               "<b>Contact Details :</b>"
               "Web site : http://www.dunbar-it.co.uk<br>"
               "Email : Norman@Dunbar-IT.co.uk<hr>"
               "The source code for this application is available from the above email address."));
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
    ExportTextAct->setEnabled(hasMdiChild);
    ExportDocbookAct->setEnabled(hasMdiChild);
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

    TileHAct = new QAction(tr("Tile &horozontally"), this);
    TileHAct->setStatusTip(tr("Tile the windows horozontally"));
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

    // The problem with Super and Sub script is this, they cannot both be on at the same time.
    // This action group, with exclusive set to true, allows ONE of its member to be on.
    // Unfortunately, once one member is ON, you cannot turn all of the mebers OFF again. :o(
    //ScriptActionGroup = new QActionGroup(this);
    //ScriptActionGroup->setExclusive(true);
    //ScriptActionGroup->addAction(TextSubscriptAct);
    //ScriptActionGroup->addAction(TextSuperscriptAct);
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

    textMenu = menuBar()->addMenu(tr("&Format"));
    textMenu->addAction(TextBoldAct);
    textMenu->addAction(TextItalicAct);
    textMenu->addAction(TextUnderlineAct);
    textMenu->addAction(TextSubscriptAct);
    textMenu->addAction(TextSuperscriptAct);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));

    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
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

    textToolBar = addToolBar(tr("Format"));
    textToolBar->addAction(TextBoldAct);
    textToolBar->addAction(TextItalicAct);
    textToolBar->addAction(TextUnderlineAct);
    textToolBar->addAction(TextSubscriptAct);
    textToolBar->addAction(TextSuperscriptAct);

    toolsToolBar = addToolBar(tr("Tools"));
    //toolsToolBar->addAction(RenameQuillAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("Dunbar-it", "QStripper");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("Dunbar-it", "QStripper");
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
