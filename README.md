# TUI project
TUI (text user interface) library

## The story

Back in 2000 when I was working for one local book store we had a legacy backoffice written in C++ for DOS.
The foundation was written by my boss several years before. The code style was messy but the most painful part
was the user interface. I could not stand the dumb way the UI was programmed (mostly by copy-pasting blocks
of existing code) so I decided to make it my way. I tried using Turbo Vision but it was too heavy for our
needs and used too many memory and remember we were using only 640 Kb of RAM for running the program (and 
extended  memory for the data). Also I was keen to try to develop something (much simplier than Turbo Vision)
myself. So I came out with this TUI library which I used for about a year until I left the company.

The demo program (TUIDEMO.EXE) only uses 69 Kb of memory, demonstrates some of the most useful components --
open file dialog, message box, input box, calendar, different lists and a progress bar which we were using 
MUCH those days :)

## Screenshot

![TUI demo](https://github.com/bhmj/tui/blob/master/screenshot.png?raw=true)

## Binary

The `TUIDEMO.EXE` is already built and can be run in DOSBox. Please keep the `TUINOTE.TXT` file besides, it is
loaded at startup.

## Build

To build TUI you need to install Borland C++ 3.1 compiler. You may install it anywhere convenient (for 
example in C:\TOOLS\BC) and then you should create a mapped drive H: using subst command
```
subst H: C:\TOOLS
```
so that you have a `bcc` binary under `H:\BC\BIN` and standard library in `H:\BC\LIB`.

Now just run `BUILD.BAT` with a memory model of your choice. Use large model if in doubt.
```
BUILD.BAT l
```

This will build a `TUIDEMO.EXE` from sources.

## Licence

[MIT](http://opensource.org/licenses/MIT)