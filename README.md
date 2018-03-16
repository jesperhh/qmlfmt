[![Build Status](https://travis-ci.org/jesperhh/qmlfmt.svg?branch=master)](https://travis-ci.org/jesperhh/qmlfmt)
[![Build status](https://ci.appveyor.com/api/projects/status/qti9p9s9q9r3pkoo/branch/master?svg=true)](https://ci.appveyor.com/project/jesperhh/qmlfmt/branch/master)

# qmlfmt
qmlfmt - command line application that formats QML files

## Build instructions
Requires
- CMake 3.0 or later
- Qt 5.2 or later.
- Tested with Visual Studio 2017 and mingw 4.9.1.
  - Should also compile on Linux, but not tested.
- Optionally QtCreator source code.
  - If not present, it will be downloaded as part of the build.
  
## Usage
    Usage: qmlfmt [options] path

Without an explicit path, it processes the standard input. Given a file, it operates on that file; given a directory, it operates on all qml files in that directory, recursively. (Files starting with a period are ignored.) By default, qmlfmt prints the reformatted sources to standard output.

### Options:
    -?, -h, --help  Displays this help.
    -v, --version   Displays version information.
    -d              Do not print reformatted sources to standard output. If a
                    file's formatting is different than qmlfmt's, print diffs to
                    standard output.
    -e              Print all errors.
    -l              Do not print reformatted sources to standard output. If a
                    file's formatting is different from qmlfmt's, print its name
                    to standard output.
    -w              Do not print reformatted sources to standard output. If a
                    file's formatting is different from qmlfmt's, overwrite it
                    with qmlfmt's version.

### Arguments:
    path            file or directory to process. If not set, qmlfmt will process
                    the standard input.
