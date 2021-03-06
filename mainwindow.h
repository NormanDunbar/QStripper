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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

class QAction;
class QActionGroup;
class QMenu;
class QWorkspace;
class MdiChild;
class QSignalMapper;
class NDWorkspace;
class QTextCharFormat;
class QDropEvent;
class QMimeData;
class QFontComboBox;
class QComboBox;
class QImage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void openFile(const QString &fileName);
    bool processArgs(int argc, char *argv[]);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void open();
    void cut();
    void copy();
    void paste();
    void about();
    void help();
    void updateMenus();
    void updateWindowMenu();
    void ExportText();
    void ExportHTML();
    void ExportDocbook();
    void ExportPDF();
    void ExportODF();
    void ExportRST();
    void ExportASC();
    void TextBold();
    void TextSize(const QString &size);
    void TextFamily(const QString &family);
    void TextItalic();
    void TextUnderline();
    void TextSubScript();
    void TextSuperScript();
    void FilePrint();
    void FormatChanged(const QTextCharFormat &Format);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void openRecent();

    MdiChild *createMdiChild();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MdiChild *activeMdiChild();
    MdiChild *findMdiChild(const QString &fileName);
    void updateRecentActionList();
    void adjustForCurrentFile(const QString &filePath);

    NDWorkspace *workspace;
    QSignalMapper *windowMapper;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *exportMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *toolsMenu;
    QMenu *textMenu;
    QMenu *recentFilesMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *exportToolBar;
    QToolBar *toolsToolBar;
    QToolBar *textToolBar;

    QAction *openAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *helpAct;
    QAction *TileHAct;
    QAction *TileVAct;
    QAction *ExportHTMLAct;
    QAction *ExportTextAct;
    QAction *ExportDocbookAct;
    QAction *ExportPDFAct;
    QAction *ExportODFAct;
    QAction *RenameQuillAct;
    QAction *ExportRSTAct;
    QAction *ExportASCAct;
    QAction *TextBoldAct;
    QAction *TextItalicAct;
    QAction *TextUnderlineAct;
    QAction *TextSubscriptAct;
    QAction *TextSuperscriptAct;
    QAction *FilePrintAct;

    QActionGroup *ScriptActionGroup;

    QFontComboBox *comboFont;
    QComboBox *comboSize;
    QString fileName;

    QImage Jupiter;
    QList<QAction *> recentFileActionList;
    enum {maxRecentFiles = 10};
};

#endif
