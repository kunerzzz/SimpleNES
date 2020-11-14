SimpleNES for Linux on Embedded Systems
=============

An NES emulator written in C++ to work on Linux without X11 support.

Based on [amhndu/SimpleNES](https://github.com/amhndu/SimpleNES)

Roughly 40-50% of games should work (ie. games that use either no mapper or mappers 1, 2 or 3).

Examples of games that have been tested to run (but NOT limited to):

(USA/Japan or World versions only i.e. NTSC compatible)

* Super Mario Bros.
* Contra
* Adventure Island
* Ninja Gaiden
* Wrecking Crew
* Megaman and Megaman 2
* Mario Bros.
* Donky Kong and Donkey Kong Jr.
* Battle City
* Paperboy
* Legend of Zelda
* Pacman
* Tennis
* Excitebike

[Here's](https://gist.github.com/amhndu/5b6da39ee06959d93dc706a0b165fb80) a big list of games that match the supported specs from SimpleNES.
(Unlike the list above, these aren't tested. Some may or may not work)

Compiling
-----------

You need:
* C++11 compliant compiler
* CMake build system

Compiling is straight forward with cmake, just run cmake on the project directory with CMAKE_BUILD_TYPE=Release
and you'll get Makefile or equivalent for your platform, with which you can compile the emulator

For e.g., on Linux/OS X/FreeBSD:
```
$ git clone https://github.com/kunerzzz/SimpleNES-ES
$ cd SimpleNES
$ mkdir build/ && cd build/
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j4    #Replace 4 with however many cores you have to spare
```

Running
-----------------
First set these environment variables:

 - $FRAMEBUFFER (framebuffer device, default: /dev/fb0)
 - $NES_CONTROLLER1 (Player1's input device, eg: /dev/input/event2)
 - $NES_CONTROLLER2 (Player2's input device, eg: /dev/input/event2)

Then pass the path to a .nes image like

```
$ ./SimpleNES ~/Games/SuperMarioBros.nes
```
<!-- To set size of the window,
```
$ ./SimpleNES -w 600 ~/Games/Contra.nes
``` -->
For supported command line options, try
```
$ ./SimpleNES -h
```

Controller
-----------------

<!-- Keybindings can be configured with keybindings.conf -->


Default keybindings:

**Player 1**

 Button        | Mapped to
 --------------|-------------
 Start         | Return/Enter
 Select        | Right Shift
 A             | J
 B             | K
 Up            | W
 Down          | S
 Left          | A
 Right         | D


**Player 2**

 Button        | Mapped to
 --------------|-------------
 Start         | Numpad9
 Select        | Numpad8
 A             | Numpad5
 B             | Numpad6
 Up            | Up
 Down          | Down
 Left          | Left
 Right         | Right


