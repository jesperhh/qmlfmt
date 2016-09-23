/*
  Copyright (c) 2015-2016, Jesper Hellesø Hansen
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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    if (app.arguments().size() < 2)
        return 0;

    TestRunner tc(app.arguments().at(1));
    QTEST_SET_MAIN_SOURCE_PATH;
    // Trim off the argument containing qmlfmt path, QTest will not understand it.
    return QTest::qExec(&tc, argc - 1, argv);
}

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

    for (auto iter = m_testFiles.cbegin(); iter != m_testFiles.cend(); iter++)
    {
        QTest::newRow(QFileInfo(iter->first).baseName().toLatin1()) << iter->first << iter->second;
    }
}

QByteArray TestRunner::readFile(const QString & fileName)
{
    QFile inputFile(fileName);
    inputFile.open(QFile::ReadOnly | QFile::Text);
    return inputFile.readAll();
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

void TestRunner::PrintWithDifferences()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    m_process->setArguments({ input, "-l" });
    m_process->start();
    QCOMPARE(m_process->waitForFinished(), true);
    QByteArray formatted = m_process->readAllStandardOutput();
    QCOMPARE(input, QString::fromUtf8(formatted).trimmed());
}

void TestRunner::DiffWithFormatted()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    m_process->setArguments({ input, "-d" });
    m_process->start();
    QCOMPARE(m_process->waitForFinished(), true);
    QByteArray diff = m_process->readAllStandardOutput();
    bool identicalFiles = readFile(input) == readFile(expected);
    QCOMPARE(input.size() == 0, identicalFiles);
}

void TestRunner::FormatFileOverwrite()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    QString temporaryFileName = getTemporaryFileName();
    QCOMPARE(QFile::copy(input, temporaryFileName), true);

    m_process->setArguments({ temporaryFileName, "-w" });
    m_process->start();
    QCOMPARE(m_process->waitForFinished(), true);
    QByteArray formattedQml = readFile(temporaryFileName);
    QByteArray expectedQml = readFile(expected);
    QCOMPARE(expectedQml, formattedQml);
}

void TestRunner::FormatFileToStdOut()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    m_process->setArguments({ input });
    m_process->start();
    QCOMPARE(m_process->waitForFinished(), true);
    QByteArray formattedQml = m_process->readAllStandardOutput().replace("\r", "");
    QByteArray expectedQml = readFile(expected);
    QCOMPARE(expectedQml, formattedQml);
}

void TestRunner::FormatStdInToStdOut()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    m_process->start();
    QCOMPARE(m_process->waitForStarted(), true);
    m_process->write(readFile(input));
    m_process->closeWriteChannel();
    QCOMPARE(m_process->waitForFinished(), true);
    QByteArray formatted = m_process->readAllStandardOutput().replace("\r", "");
    QCOMPARE(readFile(expected), formatted);
}
