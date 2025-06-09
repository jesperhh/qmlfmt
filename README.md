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
- CMake 3.29 or later
- Qt 6.8 or later.
- Tested with Visual Studio 2022 and GCC. May work with Clang as well.
- Tested on Windows, Linux. May work on Mac OS as well.

After checking out the repository, `cd` to it and run:

    git submodule update --init qt-creator
    mkdir build
    cd build
    cmake ..
    make
  
## Usage
    Usage: qmlfmt [options] path

    Without an explicit path, it processes the standard input. Given a file, it operates on that file; given a directory, it operates on all qml files in that directory, recursively. (Files starting with a period are ignored.) By default, qmlfmt prints the reformatted sources to standard output.

### Options:
    -?, -h, --help                   Displays help on commandline options.
    --help-all                       Displays help including Qt specific options.
    -v, --version                    Displays version information.
    -b, --line-length <line length>  How many characters before line will be
                                     broken.
    -t, --tab-size <tab size>        How many spaces to replace tabs with
    -i, --indent <indent>            How many spaces to use for indentation
    -l, --list                       Do not print reformatted sources to standard
                                     output. If a file's formatting is different
                                     from qmlfmt's, print its name to standard
                                     output.
    -w, --overwrite                  Do not print reformatted sources to standard
                                     output. If a file's formatting is different
                                     from qmlfmt's, overwrite it with qmlfmt's
                                     version.
    -e, --error                      Print all errors.
    -d, --diff                       Do not print reformatted sources to standard
                                     output. If a file's formatting is different
                                     than qmlfmt's, print diffs to standard
                                     output.

### Arguments:
    path                       file(s) or directory to process. If not set,
                               qmlfmt will process the standard input.
