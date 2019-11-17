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

#include <QtCore>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "qmlfmt.h"
#include "main.h"

int ParseIntOption(QCommandLineParser &parser, QCommandLineOption &option)
{
    bool ok = true;
    int optionValue = parser.value(option).toInt(&ok);
    if (!ok || optionValue < 0)
    {
        QTextStream(stderr) << "Invalid value for option " << option.names().last() << "\n";
        optionValue = -1;
    }

    return optionValue;
}

void SetupVersionInfo(QCoreApplication &app)
{
    app.setApplicationName("qmlfmt");
    app.setApplicationVersion(QMLFMT_VERSION " based on Qt Creator " QT_CREATOR_VERSION);
    app.setOrganizationDomain("www.oktet.net");
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    SetupVersionInfo(app);

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "qmlfmt formats QML files."
        "\n\n"
        "Without an explicit path, it processes the standard input. "
        "Given a file, it operates on that file; given a directory, it operates on all qml files in that directory, recursively. "
        "(Files starting with a period are ignored.) By default, qmlfmt prints the reformatted sources to standard output."
    );

    QCommandLineOption diffOption(QStringList() << "d" << "diff",
        "Do not print reformatted sources to standard output. "
        "If a file\'s formatting is different than qmlfmt\'s, print diffs "
        "to standard output.");

    QCommandLineOption errorOption(QStringList() << "e" << "error", "Print all errors.");

    QCommandLineOption listOption(QStringList() << "l" << "list",
        "Do not print reformatted sources to standard output. "
        "If a file\'s formatting is different from qmlfmt\'s, print its name "
        "to standard output.");

    QCommandLineOption overwriteOption(QStringList() << "w" << "overwrite",
        "Do not print reformatted sources to standard output. "
        "If a file\'s formatting is different from qmlfmt\'s, overwrite it "
        "with qmlfmt\'s version.");

    QCommandLineOption indentSizeOption(QStringList() << "i" << "indent", "How many spaces to use for indentation", "indent", "4");
    QCommandLineOption tabSizeOption(QStringList() << "t" << "tab-size", "How many spaces to replace tabs with", "tab size", "4");


    QMultiMap<QmlFmt::Option, QCommandLineOption> optionMap = {
        { QmlFmt::Option::PrintDiff, diffOption },
        { QmlFmt::Option::ListFileName, listOption },
        { QmlFmt::Option::PrintError, errorOption },
        { QmlFmt::Option::OverwriteFile, overwriteOption },
        { QmlFmt::Option::None, indentSizeOption},
        { QmlFmt::Option::None, tabSizeOption}
    };

    // set up options
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(optionMap.values());
    parser.addPositionalArgument("path", "file(s) or directory to process. If not set, qmlfmt will process the standard input.");

    // process command line arguments
    parser.process(app);

    // validate arguments
    if ((parser.isSet(overwriteOption) || parser.isSet(listOption)) && parser.positionalArguments().count() == 0)
    {
        QTextStream(stderr) << "Cannot combine -" << overwriteOption.names().last() << " and -" << listOption.names().last()
            << " with standard input\n";
        return 1;
    }
    else if (parser.isSet(diffOption) + parser.isSet(overwriteOption) + parser.isSet(listOption) > 1)
    {
        QTextStream(stderr) << "-" << diffOption.names().last() << ", -" << overwriteOption.names().last() << " and -" <<
            listOption.names().last() << " are mutually exclusive\n";
        return 1;
    }

    int indentSize = ParseIntOption(parser, indentSizeOption);
    int tabSize = ParseIntOption(parser, tabSizeOption);

    if (tabSize < 0 || indentSize < 0)
    {
        return 1;
    }

    QmlFmt::Options options;
    for (auto kvp = optionMap.constKeyValueBegin(); kvp != optionMap.constKeyValueEnd(); ++kvp)
    {
        if (parser.isSet((*kvp).second))
            options |= (*kvp).first;
    }

    QmlFmt qmlFmt(options, indentSize, tabSize);
    return qmlFmt.Run(parser.positionalArguments());
}
