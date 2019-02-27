# Flopbook
A very simple flipbook animation program that closely resembles Flipnote Studio for the Nintendo DSi and 3DS made using FLTK. It has its own file format for animation projects but animations can be exported as animated GIFs.

Because this program was made using FLTK, proper tablet input support may not be possible as FLTK lacks support for such funtionality and that there does not appear to be any platform and toolkit independent libraries for handling tablet input. This program may be considered more of a prototype than something to be used seriously due to aforementioned issues regarding tablet support unless FLTK receives an update that adds tablet support or this program gets ported over to GTK+. This was made mostly as a small side project for an artist friend.

# Compiling
This program requires fltk-1.3.4-1 and freeimage libraries.

Compiling is easiest done through the Netbeans IDE. There are debug and release configurations already defined in the project for both Windows and Linux targets, modify configuration settings if necessary.

# Changelog
**Version 0.22b (2/27/2019)**
* Initial 'prototype' release.