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

#include <QtTest/QtTest>
#include <memory>

class TestRunner : public QObject
{
    Q_OBJECT

public:
    TestRunner(const QString& qmlfmtPath, QObject *parent = nullptr);

private:
    typedef QPair<QString, QString> TestInput;
    std::unique_ptr<QProcess> m_process;
    QList<TestInput> m_testFiles;
    QString m_qmlfmtPath;

    void prepareTestData();

    QString readFile(const QString& fileName);

    /* Read stderr if inputFileName has suffix '-stderr', from stdout otherwise */
    QString readStream(const QString& inputFileName = QString());

    QString getTemporaryFileName();

private slots:
    void init();

    void PrintWithDifferences();
    void PrintWithDifferences_data() { prepareTestData(); }

    void DiffWithFormatted();
    void DiffWithFormatted_data() { prepareTestData(); }

    void FormatFileOverwrite();
    void FormatFileOverwrite_data() { prepareTestData(); }

    void FormatFileToStdOut();
    void FormatFileToStdOut_data() { prepareTestData(); }

    void FormatStdInToStdOut();
    void FormatStdInToStdOut_data() { prepareTestData(); }
};
