#include "mainwindow.h"
#include <QIcon>
#include <QAction>

class MainWindow;


void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/save.png"), tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

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
    connect(closeAct, SIGNAL(triggered()),
            workspace, SLOT(closeActiveWindow()));

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

    ExportTextAct = new QAction(QIcon(":/images/save.png"), tr("Export &Text"), this);
    ExportTextAct->setShortcut(tr("Ctrl+Shift+T"));
    ExportTextAct->setStatusTip(tr("Export the current file as plain text"));
    connect(ExportTextAct, SIGNAL(triggered()), this, SLOT(ExportText()));

    ExportHTMLAct = new QAction(QIcon(":/images/save.png"), tr("Export &HTML"), this);
    ExportHTMLAct->setShortcut(tr("Ctrl+Shift+H"));
    ExportHTMLAct->setStatusTip(tr("Export the current file as HTML"));
    connect(ExportHTMLAct, SIGNAL(triggered()), this, SLOT(ExportHTML()));

    ExportDocbookAct = new QAction(QIcon(":/images/save.png"), tr("Export &DocBook XML"), this);
    ExportDocbookAct->setShortcut(tr("Ctrl+Shift+D"));
    ExportDocbookAct->setStatusTip(tr("Export the current file as DocBook XML"));
    connect(ExportDocbookAct, SIGNAL(triggered()), this, SLOT(ExportDocbook()));

}
