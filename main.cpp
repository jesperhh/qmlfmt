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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("qmlfmt");

#ifdef QMLFMT_VERSION
    QCoreApplication::setApplicationVersion(QMLFMT_VERSION);
#endif // QMLFMT_VERSION
    
    QCommandLineParser parser;
    parser.setApplicationDescription(
        "qmlfmt formats QML files."
        "\n"
        "Without an explicit path, it processes the standard input. "
        "Given a file, it operates on that file; given a directory, it operates on all qml files in that directory, recursively. "
        "(Files starting with a period are ignored.) By default, qmlfmt prints the reformatted sources to standard output."
    );

    QCommandLineOption diffOption("d",
        "Do not print reformatted sources to standard output. "
        "If a file\'s formatting is different than qmlfmt\'s, print diffs "
        "to standard output.");

    QCommandLineOption errorOption("e", "Print all errors.");

    QCommandLineOption listOption("l",
        "Do not print reformatted sources to standard output. "
        "If a file\'s formatting is different from qmlfmt\'s, print its name "
        "to standard output.");

    QCommandLineOption overwriteOption("w",
        "Do not print reformatted sources to standard output. "
        "If a file\'s formatting is different from qmlfmt\'s, overwrite it "
        "with qmlfmt\'s version.");

    QMap<QmlFmt::Option, QCommandLineOption> optionMap = {
        { QmlFmt::Option::PrintDiff, diffOption },
        { QmlFmt::Option::ListFileName, listOption },
        { QmlFmt::Option::PrintError, errorOption },
        { QmlFmt::Option::OverwriteFile, overwriteOption }
    };

    // set up options
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(optionMap.values());
    parser.addPositionalArgument("path", "file or directory to process. If not set, qmlfmt will process the standard input.");

    // process command line arguments
    parser.process(app);

    // validate arguments
    if ((parser.isSet(overwriteOption) || parser.isSet(listOption)) && parser.positionalArguments().count() == 0)
    {
        QTextStream(stderr) << "Cannot combine -" << overwriteOption.names().first() << " and -" << listOption.names().first()
            << " with standard input\n";
        return 1;
    }
    else if (parser.isSet(diffOption) + parser.isSet(overwriteOption) + parser.isSet(listOption) > 1)
    {
        QTextStream(stderr) << "-" << diffOption.names().first() << ", -" << overwriteOption.names().first() << " and -" <<
            listOption.names().first() << " are mutually exclusive\n";
        return 1;
    }

    QmlFmt::Options options;
    for (auto kvp = optionMap.constKeyValueBegin(); kvp != optionMap.constKeyValueEnd(); ++kvp)
    {
        if (parser.isSet((*kvp).second))
            options |= (*kvp).first;
    }

    QmlFmt qmlFmt(options);
    return qmlFmt.Run(parser.positionalArguments());
}
