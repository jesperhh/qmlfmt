#include <time.h>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <qmljs/qmljsdocument.h>
#include <qmljs/qmljsmodelmanagerinterface.h>
#include <qmljs/qmljsreformatter.h>

#include <diff_match_patch.h>

namespace {
    QCommandLineParser parser;

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

    int handle(QIODevice& input, const QString& path)
    {
        QTextStream qstdout(stdout);
        QString source = QString::fromUtf8(input.readAll());

        QmlJS::Document::MutablePtr document = QmlJS::Document::create(path,
            QmlJS::ModelManagerInterface::guessLanguageOfFile(path));

        document->setSource(source);
        document->parse();      
        if (!document->diagnosticMessages().isEmpty())
        {
            if (parser.isSet(errorOption))
            {
                for (const QmlJS::DiagnosticMessage& msg : document->diagnosticMessages())
                {
                    qstdout << (msg.isError() ? "Error:" : "Warning:");

                    if (msg.loc.isValid())
                        qstdout << msg.loc.startLine << ':' << msg.loc.startColumn << ':';

                    qstdout << ' ' << msg.message << "\n";
                }
            }
            return 1;
        }

        const QString reformatted = QmlJS::reformat(document);
        if (source == reformatted)
            return 0;

        if (parser.isSet(listOption))
        {
            // List filename
            qstdout << path << "\n";
        }
        else if (parser.isSet(diffOption))
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
            if (parser.isSet(overwriteOption))
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

    int handle()
    {
        QFile file;
        file.open(stdin, QFile::ReadOnly | QFile::Text);
        return handle(file, "stdin.qml");
    }

    int handle(const QString& path)
    {
        QFileInfo fileInfo(path);
        if (fileInfo.isFile())
        {
            QFile file(path);
            file.open(QFile::ReadOnly | QFile::Text);
            return handle(file, path);
        }
        else if (fileInfo.isDir())
        {
            QDirIterator iter(
                path, 
                QStringList{ "*.qml" }, 
                QDir::Filter::Files, 
                QDirIterator::IteratorFlag::Subdirectories);

            while (iter.hasNext())
            {
                QFile file(iter.next());
                file.open(QFile::ReadOnly | QFile::Text);
                handle(file, file.fileName());
            }
        }
        else
        {
            QTextStream(stdout) << "Path is not valid file or directory\n";
            return 1;
        }

        return 0;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("qmlfmt");
    QCoreApplication::setApplicationVersion("0.1");

    parser.setApplicationDescription(
        "qmlfmt formats QML files."
        "\n"
        "Without an explicit path, it processes the standard input. "
        "Given a file, it operates on that file; given a directory, it operates on all qml files in that directory, recursively. "
        "(Files starting with a period are ignored.) By default, qmlfmt prints the reformatted sources to standard output."
        );

    // set up options
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(diffOption);
    parser.addOption(errorOption);
    parser.addOption(listOption);
    parser.addOption(overwriteOption);
    parser.addPositionalArgument("path", "file or directory to process. If not set, qmlfmt will process the standard input.");

    // process command line arguments
    parser.process(app);

    // validate arguments
    if ((parser.isSet(overwriteOption) || parser.isSet(listOption)) && parser.positionalArguments().count() == 0)
    {
        QTextStream(stdout) << "Cannot combine -" << overwriteOption.names().first() << " and -" << listOption.names().first()
            << " with standard input\n";
        return 1;
    }
    else if (parser.isSet(diffOption) + parser.isSet(overwriteOption) + parser.isSet(listOption) > 1)
    {
        QTextStream(stdout) << "-" << diffOption.names().first() << ", -" << overwriteOption.names().first() << " and -" <<
            listOption.names().first() << " are mutually exclusive\n";
        return 1;
    }

    if (parser.positionalArguments().count() > 0)
        return handle(parser.positionalArguments().first());
    else
        return handle();
}
