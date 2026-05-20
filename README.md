# fedup

Cross-platform Notepad++ clone written in C++ using Qt 5 for the GUI and
QScintilla2 for the text editing widget.

# Screenshot

<img width="832" height="616" alt="fedup_screenshot" src="https://github.com/user-attachments/assets/0bc0afe5-314f-44c5-84f7-10224048e681" />

## Dependencies

* Qt 5 (widgets and network modules)
* QScintilla2 for Qt 5

### Ubuntu 18.04

The relevant development packages to install on Ubuntu 18.04 are:

```
sudo apt-get install qt5-default libqscintilla2-qt5-dev
```

### Debian 13

The relevant development packages to install on Debian 13 (trixie) are:

```
sudo apt-get install qtbase5-dev libqscintilla2-qt5-dev
```

## Compiling

On Linux, run the following commands in the top-level directory with the `CMakeLists.txt` file:

```
mkdir build
cd build
cmake ..
cmake --build .
```

## Running

On Linux, you can launch the built `fedup` executable that will be put into the `build` directory:

```
./fedup
```
