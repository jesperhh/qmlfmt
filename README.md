[![Build Status](https://travis-ci.org/jesperhh/qmlfmt.svg?branch=master)](https://travis-ci.org/jesperhh/qmlfmt)
[![Build status](https://ci.appveyor.com/api/projects/status/qti9p9s9q9r3pkoo/branch/master?svg=true)](https://ci.appveyor.com/project/jesperhh/qmlfmt/branch/master)

# qmlfmt
qmlfmt - command line application that formats QML files

## Install via Homebrew

If you are on *macOS* and using [Homebrew](https://brew.sh), you can install *qmlfmt* this way:

```
brew install martindelille/tap/qmlfmt
```

## Build instructions
Requires
- CMake 3.1 or later
- Qt 5.10 or later.
- Tested with Visual Studio 2017, GCC 4.9 and Clang 5.0
- Tested on Windows, Linux and Mac OS
  
## Usage
    Usage: qmlfmt [options] path

    Without an explicit path, it processes the standard input. Given a file, it operates on that file; given a directory, it operates on all qml files in that directory, recursively. (Files starting with a period are ignored.) By default, qmlfmt prints the reformatted sources to standard output.

### Options:
    -?, -h, --help             Displays this help.
    -v, --version              Displays version information.
    -t, --tab-size <tab size>  How many spaces to replace tabs with
    -i, --indent <indent>      How many spaces to use for indentation
    -l, --list                 Do not print reformatted sources to standard
                               output. If a file's formatting is different from
                               qmlfmt's, print its name to standard output.
    -w, --overwrite            Do not print reformatted sources to standard
                               output. If a file's formatting is different from
                               qmlfmt's, overwrite it with qmlfmt's version.
    -e, --error                Print all errors.
    -d, --diff                 Do not print reformatted sources to standard
                               output. If a file's formatting is different than
                               qmlfmt's, print diffs to standard output.

### Arguments:
    path                       file(s) or directory to process. If not set,
                               qmlfmt will process the standard input.
