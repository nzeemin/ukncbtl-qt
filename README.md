# ukncbtl-qt
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
[![Build status](https://ci.appveyor.com/api/projects/status/ipvgt62iivnc6j7f?svg=true)](https://ci.appveyor.com/project/nzeemin/ukncbtl-qt)
[![Build Status](https://github.com/nzeemin/ukncbtl-qt/actions/workflows/push-matrix.yml/badge.svg?branch=master)](https://github.com/nzeemin/ukncbtl-qt/actions/workflows/push-matrix.yml)
[![CodeFactor](https://www.codefactor.io/repository/github/nzeemin/ukncbtl-qt/badge)](https://www.codefactor.io/repository/github/nzeemin/ukncbtl-qt)

UKNCBTL emulator, Qt version.

The UKNCBTL project consists of:
* [ukncbtl](https://github.com/nzeemin/ukncbtl/) written for Win32 and works under Windows 2000/2003/2008/XP/Vista/7/8/10.
* [ukncbtl-renders](https://github.com/nzeemin/ukncbtl-renders/) — renderers for UKNCBTL Win32.
* [ukncbtl-qt](https://github.com/nzeemin/ukncbtl-qt/) is based on Qt framework and works under Windows, Linux and Mac OS X.
* [ukncbtl-testbench](https://github.com/nzeemin/ukncbtl-testbench/) — test bench for regression and performance testing.
* [ukncbtl-utils](https://github.com/nzeemin/ukncbtl-utils/) — various utilities: rt11dsk, sav2wav, ESCParser etc.
* [ukncbtl-doc](https://github.com/nzeemin/ukncbtl-doc/) — documentation and screenshots.
* [ukncbtl-wasm](https://github.com/nzeemin/ukncbtl-wasm/) — web version of the emulator.
* Project wiki: https://github.com/nzeemin/ukncbtl-doc/wiki
  * Screenshots: https://github.com/nzeemin/ukncbtl-doc/wiki/Screenshots-ru
  * User's Manual (in Russian): https://github.com/nzeemin/ukncbtl-doc/wiki/Users-Manual-ru


## Как запустить под Linux

### Собрать из исходников

 1. Установить пакеты: Qt 5 + QtScript<br>
    `sudo apt install qtbase5-dev qt5-qmake qtscript5-dev`
 2. Скачать исходники: либо через<br>
    `git clone https://github.com/nzeemin/ukncbtl-qt.git`<br>
    либо скачать как .zip и распаковать
 3. Выполнить команды:<br>
   `cd emulator`<br>
   `qmake "CONFIG+=release" QtUkncBtl.pro`<br>
   `make`<br>
 4. Дать права на выполнение: `chmod +x QtUkncBtl`
 5. Запустить `QtUkncBtl`

### Используя готовый AppImage

 1. Зайти в [Releases](https://github.com/nzeemin/bkbtl-qt/releases) найти последний AppImage-релиз и скачать `*.AppImage` файл
 2. Дать права на выполнение: `chmod +x UKNCBTL_Qt-a808c28-x86_64.AppImage` (подставить тут правильное название AppImage файла)
 3. Запустить AppImage файл
