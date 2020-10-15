/*
  Copyright (c) 2015-2018, Jesper Hellesø Hansen
  jesperhh@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <time.h>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

#include <qmljs/parser/qmljsengine_p.h>
#include <qmljs/qmljsdocument.h>
#include <qmljs/qmljsmodelmanagerinterface.h>
#include <qmljs/qmljsreformatter.h>

#include <diff_match_patch.h>
#include "qmlfmt.h"

// Listing files with incorrect formatting, overwriting files with formatted content and printing diffs generate no output when files are identical.
static const QmlFmt::Options SkipIdenticalFilesMask = QmlFmt::Option::ListFileName | QmlFmt::Option::OverwriteFile | QmlFmt::Option::PrintDiff;

int QmlFmt::InternalRun(QIODevice& input, const QString& path)
{
    QTextStream qstdout(stdout);
    QTextStream qstderr(stderr);
    const QString source = QString::fromUtf8(input.readAll());
    const QmlJS::Dialect dialect = QmlJS::ModelManagerInterface::guessLanguageOfFile(path);

    QmlJS::Document::MutablePtr document = QmlJS::Document::create(path, dialect);
    document->setSource(source);
    document->parse();
    if (!document->diagnosticMessages().isEmpty())
    {
        if (this->m_options.testFlag(Option::PrintError))
        {
            for (const QmlJS::DiagnosticMessage& msg : document->diagnosticMessages())
            {
                qstderr << (msg.isError() ? "Error:" : "Warning:");

                qstderr << msg.loc.startLine << ':' << msg.loc.startColumn << ':';

                qstderr << ' ' << msg.message << "\n";
            }
        }
        return 1;
    }

    const QString reformatted = QmlJS::reformat(document, m_indentSize, m_tabSize);

    // Only continue if we are printing to stdout, in that case we should always print the file content,
    // changed or not. If we are printing diff/overwriting/listing files there will be nothing to do,
    // so we can just skip this.
    if (source == reformatted && (this->m_options & SkipIdenticalFilesMask) != 0)
        return 0;

    if (this->m_options.testFlag(Option::ListFileName))
    {
        // List filename
        qstdout << path << "\n";
    }
    else if (this->m_options.testFlag(Option::PrintDiff))
    {
        // Create and print diff
        diff_match_patch differ;
        const QList<Patch> patches = differ.patch_make(source, reformatted);
        qstdout << differ.patch_toText(patches);
    }
    else
    {
        // Print reformatted file to stdout/original file
        QFile outFile;
        if (this->m_options.testFlag(Option::OverwriteFile))
        {
            outFile.setFileName(path);
            outFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
        }
        else
        {
            outFile.open(stdout, QFile::WriteOnly | QFile::Text);
        }

        const QByteArray bytes = reformatted.toUtf8();
        outFile.write(bytes);
    }

    return 0;
}

QmlFmt::QmlFmt(Options options, int indentSize, int tabSize)
    : m_options(options)
    , m_indentSize(indentSize)
    , m_tabSize(tabSize)
{
    new QmlJS::ModelManagerInterface();
}

int QmlFmt::Run()
{
    QFile file;
    file.open(stdin, QFile::ReadOnly | QFile::Text);
    return this->InternalRun(file, "stdin.qml");
}

int QmlFmt::Run(QStringList paths)
{
    if (paths.count() == 0)
    {
        return Run();
    }

    int returnValue = 0;
    for (const QString& fileOrDir : paths)
    {
        QFileInfo fileInfo(fileOrDir);
        if (fileInfo.isFile())
        {
            QFile file(fileOrDir);
            file.open(QFile::ReadOnly | QFile::Text);
            returnValue |= this->InternalRun(file, fileOrDir);
        }
        else if (fileInfo.isDir())
        {
            QDirIterator iter(
                fileOrDir,
                QStringList{ "*.qml" },
                QDir::Filter::Files,
                QDirIterator::IteratorFlag::Subdirectories);

            while (iter.hasNext())
            {
                QFile file(iter.next());
                file.open(QFile::ReadOnly | QFile::Text);
                returnValue |= this->InternalRun(file, file.fileName());
            }
        }
        else
        {
            QTextStream(stderr) << "Path is not valid file or directory: " << fileOrDir << "\n";
            returnValue |= 1;
        }
    }

    return returnValue;
}
