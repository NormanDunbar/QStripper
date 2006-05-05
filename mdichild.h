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

class MdiChild : public QTextEdit
{
    Q_OBJECT

public:
    MdiChild();

    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    bool ExportText();
    bool ExportHTML();
    bool ExportPDF();
    bool ExportDocbook();
    bool TextBold(const bool Checked);
    bool TextItalic(const bool Checked);
    bool TextUnderline(const bool Checked);
    bool TextSubScript(const bool Checked);
    bool TextSuperScript(const bool Checked);
    bool FilePrint();
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;
    QString ReadQuillFile(QDataStream &in, bool &FormatError);
};

#endif
