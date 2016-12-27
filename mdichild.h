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

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QTextStream>
#include <QTextFragment>

class QuillDoc;

class MdiChild : public QTextEdit
{
    Q_OBJECT

public:
    MdiChild();
    ~MdiChild();

    bool loadFile(const QString &fileName);
    bool ExportText();
    bool ExportHTML();
    bool ExportPDF();
    bool ExportODF();
    bool ExportDocbook();
    bool TextBold(const bool Checked);
    bool TextItalic(const bool Checked);
    bool TextUnderline(const bool Checked);
    bool TextSubScript(const bool Checked);
    bool TextSuperScript(const bool Checked);
    bool FilePrint();
    void setSilent(bool silent);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    QString fileExtension(const QString &fullFileName);
    QString fileBasename(const QString &fullFileName);
    QString filePath(const QString &fullFileName);
    QString curFile;
    QString PDFFile;
    QString ODFFile;
    QString TXTFile;
    QString XMLFile;
    QString HTMLFile;

protected:
    void closeEvent(QCloseEvent *event);
    void focusInEvent(QFocusEvent *event);

private slots:
    void documentWasModified();

signals:
    void FormatChanged(const QTextCharFormat &Format);

private:
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    
    QString DocBookParagraph(const QTextBlock &ThisBlock);
    QString DocBookFragment(const QTextFragment &ThisFragment);
    bool maybeSave();
    bool isUntitled;
    bool silentRunning;

    QuillDoc *Input;
};

#endif
