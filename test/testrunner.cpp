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
    QTest::addColumn<bool>("hasError");

    for (auto iter = m_testFiles.cbegin(); iter != m_testFiles.cend(); iter++)
    {
        QTest::newRow(QFileInfo(iter->first).baseName().toLatin1()) << iter->first << iter->second << iter->first.contains("error");
    }
}

QString TestRunner::readFile(const QString & fileName)
{
    QFile inputFile(fileName);
    inputFile.open(QFile::ReadOnly | QFile::Text);
    return QString(inputFile.readAll());
}

QString TestRunner::readStream(const QString & inputFileName)
{
    if (!m_process->waitForFinished())
    {
        QTest::qFail("waitForFinished failed", __FILE__, __LINE__);
        return QString();
    }

    QString output;

    if (inputFileName.endsWith("-stderr.qml")) {
      output = QString::fromUtf8(m_process->readAllStandardError()).replace("\r", "");
    }
    else
      output = QString::fromUtf8(m_process->readAllStandardOutput()).replace("\r", "");

    return output;
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
    QFETCH(bool, hasError);

    m_process->setArguments({ input, "-l", "-e" });
    m_process->start();
    QString formatted = readStream(input);
    QCOMPARE(hasError ? readFile(expected) : input + "\n", formatted);
}

void TestRunner::DiffWithFormatted()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    m_process->setArguments({ input, "-d", "-e" });
    m_process->start();
    QString diff = readStream(input);
    bool identicalFiles = readFile(input) == readFile(expected);
    QCOMPARE(input.size() == 0, identicalFiles);
}

void TestRunner::FormatFileOverwrite()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QFETCH(bool, hasError);

    QString temporaryFileName = getTemporaryFileName();
    QCOMPARE(QFile::copy(input, temporaryFileName), true);

    m_process->setArguments({ temporaryFileName, "-w", "-e" });
    m_process->start();
    QCOMPARE(m_process->waitForFinished(), true);

    QString formattedQml = readFile(temporaryFileName);
    QString expectedQml =  readFile(hasError ? input : expected);
    QCOMPARE(expectedQml, formattedQml);
}

void TestRunner::FormatFileToStdOut()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    m_process->setArguments({ input , "-e" });
    m_process->start();
    QString formattedQml = readStream(input);
    QString expectedQml = readFile(expected);
    QCOMPARE(expectedQml, formattedQml);
}

void TestRunner::FormatStdInToStdOut()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);

    m_process->setArguments({ "-e" });
    m_process->start();
    QCOMPARE(m_process->waitForStarted(), true);

    m_process->write(readFile(input).toUtf8());
    m_process->closeWriteChannel();
    QString formatted = readStream(input);
    QCOMPARE(formatted, readFile(expected));
}
