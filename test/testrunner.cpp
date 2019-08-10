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

#include "testrunner.h"
#include <time.h>
#include <QtTest>
#include <diff_match_patch.h>

TestRunner::TestRunner(const QString& qmlfmtPath, QObject *parent) : m_qmlfmtPath(qmlfmtPath)
{
    m_testFiles.clear();
    QDir sourceDir = QFileInfo(QFile::decodeName(__FILE__)).absoluteDir();
    sourceDir.cd("data");
    QDirIterator iter(sourceDir.absolutePath(),
        QStringList{ "in_*.qml" },
        QDir::Filter::Files,
        QDirIterator::IteratorFlag::NoIteratorFlags);

    while (iter.hasNext())
    {
        QFileInfo fileInfo(iter.next());
        QDir dir = fileInfo.dir();
        const QString inputFileName = fileInfo.fileName();
        QString outputFileName = inputFileName;
        outputFileName.replace("in_", "out_");
        m_testFiles.append(TestInput(dir.absoluteFilePath(inputFileName),
            dir.absoluteFilePath(outputFileName)));
    }
}

void TestRunner::prepareTestData()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<bool>("hasError");

    for (auto iter = m_testFiles.cbegin(); iter != m_testFiles.cend(); iter++)
    {
        QTest::newRow(QFileInfo(iter->first).baseName().toLatin1()) << iter->first << iter->second << iter->first.contains("error");
    }

    for (auto iter = m_testFiles.cbegin(); iter != m_testFiles.cend(); iter++)
    {
        if (!iter->first.contains("error"))
            QTest::newRow(QFileInfo(iter->second).baseName().toLatin1()) << iter->second << iter->second << false;
    }
}

QString TestRunner::readFile(const QString & fileName)
{
    static QHash<QString, QString> cache;
    if (cache.contains(fileName))
    {
        return cache[fileName];
    }

    QFile inputFile(fileName);
    inputFile.open(QFile::ReadOnly | QFile::Text);
    QString content(inputFile.readAll());
    cache.insert(fileName, content);
    return content;
}

QString TestRunner::readOutputStream(bool fromStdError)
{
    m_process->waitForFinished();

    QByteArray output = fromStdError ? m_process->readAllStandardError() : m_process->readAllStandardOutput();
    return QString::fromUtf8(output).replace("\r", "");
}

QString TestRunner::getTemporaryFileName()
{
    // (Ab)use QTemporaryFile to generate a temporary file name. We don't use the actual file
    // for anything, so it is a bit wasteful, but there is no API to get the filename without the file.
    QString result;
    QTemporaryFile temporaryFile(QDir::temp().filePath("XXXXXX.qml"));
    temporaryFile.open();
    result = temporaryFile.fileName();
    return result;
}

void TestRunner::init()
{
    m_process.reset(new QProcess());
    m_process->setProgram(m_qmlfmtPath);
}

void TestRunner::cleanup()
{
    QVERIFY(m_process->exitStatus() == QProcess::ExitStatus::NormalExit);
}

void TestRunner::PrintWithDifferences()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QFETCH(bool, hasError);

    m_process->setArguments({ input, "-l", "-e" });
    m_process->start();
    QString output = readOutputStream(hasError);
    bool identicalFiles = !hasError && readFile(input) == readFile(expected);

    if (hasError)
    {
        QCOMPARE(output, readFile(expected));
    }
    else
    {
        expected = identicalFiles ? "" : input + "\n";
        QCOMPARE(output, expected);
    }
}

void TestRunner::DiffWithFormatted()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QFETCH(bool, hasError);

    m_process->setArguments({ input, "-d", "-e" });
    m_process->start();
    QString diff = readOutputStream(hasError);
    if (hasError)
    {
        QCOMPARE(diff, readFile(expected));
    }
    else
    {
        diff_match_patch differ;
        QString unformatted = readFile(input);
        QString formatted = readFile(expected);
        QList<Patch> patch = differ.patch_fromText(diff);
        QString patchedUnformatted = differ.patch_apply(patch, unformatted).first;
        QCOMPARE(patchedUnformatted, formatted);
    }
}

void TestRunner::FormatFileOverwrite()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QFETCH(bool, hasError);

    QString temporaryFileName = getTemporaryFileName();
    QVERIFY(QFile::copy(input, temporaryFileName));

    m_process->setArguments({ temporaryFileName, "-w", "-e" });
    m_process->start();
    QVERIFY(m_process->waitForFinished());

    QString formattedQml = readFile(temporaryFileName);
    QString expectedQml =  readFile(hasError ? input : expected);
    QCOMPARE(formattedQml, expectedQml);
}

void TestRunner::FormatFileToStdOut()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QFETCH(bool, hasError);

    m_process->setArguments({ input , "-e" });
    m_process->start();
    QString output = readOutputStream(hasError);
    bool identicalFiles = !hasError && readFile(input) == readFile(expected);
    QCOMPARE(output, identicalFiles ? "" : readFile(expected));
}

void TestRunner::FormatStdInToStdOut()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QFETCH(bool, hasError);

    m_process->setArguments({ "-e" });
    m_process->start();
    QVERIFY(m_process->waitForStarted());

    m_process->write(readFile(input).toUtf8());
    m_process->closeWriteChannel();
    QString output = readOutputStream(hasError);
    bool identicalFiles = !hasError && readFile(input) == readFile(expected);
    QCOMPARE(output, identicalFiles ? "" : readFile(expected));
}

void TestRunner::PrintFolderWithDifferences()
{
    QDir sourceDir = QFileInfo(QFile::decodeName(__FILE__)).absoluteDir();
    m_process->setArguments({ sourceDir.absolutePath(), "-l", "-e" });
    m_process->start();
    QString stdOut = readOutputStream(false);
    QString stdError = readOutputStream(true);

    for (auto iter = m_testFiles.cbegin(); iter != m_testFiles.cend(); iter++)
    {
        if (iter->first.contains("error"))
        {
            // Expected error message is included
            QVERIFY(stdError.contains(readFile(iter->second)));
        }
        else
        {
            // Non-formatted (input) is listed, formatted (output) is not
            QVERIFY(stdOut.contains(iter->first));
            QVERIFY(!stdOut.contains(iter->second));
        }
    }
}

void TestRunner::PrintMultipleFilesWithDifferences()
{
    QStringList arguments = { "-l", "-e" };
    QString changedFiles, errors;
    for (auto iter = m_testFiles.cbegin(); iter != m_testFiles.cend(); iter++)
    {
        arguments.append(iter->first);
        if (iter->first.contains("error"))
        {
            errors.append(readFile(iter->second));
        }
        else
        {
            changedFiles.append(iter->first + "\n");
        }
        
    }

    m_process->setArguments(arguments);
    m_process->start();

    QString stdOut = readOutputStream(false);
    QString stdError = readOutputStream(true);
    QCOMPARE(stdOut, changedFiles);
    QCOMPARE(stdError, errors);
}

void TestRunner::FormatWithDifferentTabAndIndentSize()
{
    QDir sourceDir = QFileInfo(QFile::decodeName(__FILE__)).absoluteDir();
    sourceDir.cd("data");
    QString input = sourceDir.absoluteFilePath("tab_and_indent_size_in.qml");
    QString expectedFile = sourceDir.absoluteFilePath("tab_and_indent_size_out.qml");
        
    m_process->setArguments({ "-e", "-t", "20", "-i", "3" });
    m_process->start();
    QVERIFY(m_process->waitForStarted());

    m_process->write(readFile(input).toUtf8());
    m_process->closeWriteChannel();
    QString output = readOutputStream(false);
    QString expected = readFile(expectedFile);
    QCOMPARE(output, expected);
}

void TestRunner::InvalidIndentationError()
{
    QDir sourceDir = QFileInfo(QFile::decodeName(__FILE__)).absoluteDir();
    sourceDir.cd("data");
    QString input = sourceDir.absoluteFilePath("tab_and_indent_size_in.qml");
    QStringList arguments = { "-l", "-e", "-t" , "X", "-i", "Y", input };

    m_process->setArguments(arguments);
    m_process->start();

    QString stdOut = readOutputStream(false);
    QString stdError = readOutputStream(true);

    QVERIFY(m_process->exitCode() != 0);
    QCOMPARE(stdOut, QString());
    QCOMPARE(stdError, "Invalid value for option indent\nInvalid value for option tab-size\n");
}

void TestRunner::VersionNumberIncluded()
{
    QStringList arguments = { "-v"};

    m_process->setArguments(arguments);
    m_process->start();
    QString stdOut = readOutputStream(false);
    

    // Disabled - will always fail for PRs and branch builds.
    // QString version(QMLFMT_VERSION);
    // QVERIFY(!version.isEmpty());
    // QCOMPARE(stdOut, QString("qmlfmt ") + version + "\n");
}
