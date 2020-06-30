# fedup

cross platform Notepad++ clone written in C++/Qt/QScintilla2

## Dependencies

* Qt 5 (widgets and network modules)
* QScintilla2 for Qt 5

The relevant development packages to install on Ubuntu 18.04 are:

```
sudo apt-get install qt5-default libqscintilla2-qt5-dev
```

## Compiling

On Linux, run the following commands in the top-level directory with the `fedup.pro` file:

```
qmake
make
```

## Running

On Linux, you can launch the built `fedup` executable that will be put in the same directory as the `fedup.pro` file:

```
./fedup
```
