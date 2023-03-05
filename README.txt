README.txt

== DISCLAIMER:
THIS SOFTWARE IS PROVIDED AS IS. NO LIABILITIES GUARANTEED WHATSOEVER.
USE ENTIRELY AT YOUR OWN RISK.

== Intro:

Use only if you know what a debugging software is meant to do. See
DISCLAIMER.

== Requirements:

You need an updated beta kernel to use this software. It is currently not
compliant with latest public release of the system.

If you have an updated AISS, the buttons will look nicer.

== Use:

Should be straightforward. Pipe is not functional on Amiga, so standard output
from child apps will be shown in their own shell window on the Workbench.

Executables need to be built with the -gstabs command line option in gcc.
This is the only way, that Spotless can identify symbols in the executable.

== Source code:

https://github.com/alfkil/Spotless

== History:

2.3.0 :
        -- Reworked the stabs interpreter to handle new style of gcc 11.
        
2.2.1 :
        -- Correct highlighing of sourceline on trace/stepwise execution.
        -- Iconify now shows the Spotless icon on the workbench.
        -- Corrected menu texts.
        -- Added __DATE__ macro in version cookie and screen title.

2.2.0 : 
        -- Many improvements to the menus :         
           -- added split window mode with ability to show/hide individual
                windows
           -- added ability to choose also from RMB menu extra windows : config
                and memory surfer ones.
           -- added code to reflect every case with extra windows: does not
                matter if choosen by RMB or buttons, RMB menu reflect all the
                states in any mode, on the fly.
           
       -- Added highlighting of source file name in Sources panel upon breaks.
       -- Added auto-generated configs to auto save size and positions of
                windows across switch in different modes, including restarting.
       -- Added option to "Ask for arguments" or not to ask, when load a binary.
       -- Reworked disassembler window in the split window mode, so it doesn't
                feel like there is alot of empty space.
       -- Improvements to overall stability and a lot of code cleaning work
                being done.
                
       --  And lot's of bug fixes: 
           -- stacktrace (varius fixes, including kas1e'e "line 30" problem)
           -- iconify (proper refresh of window's content in all modes in all
                conditions, remove debug output)
           -- about window and public screen title (copyright symbol (c) )
           -- fixes in binary handling area (ability to run self, db101 and all
                the binaries using external amiga libraries).
           -- fixed a "long waiting bug" when parsing binary's stabs 
           -- fixed main stepinto (tracer)
           -- fixed memory surfer's bugs (clean break points, assmebler steps, etc)
           -- fixed crash with relative paths
           -- things which not worth noting, there were many with this release!
 
2.1.0 : Fix menus.
        Fix public screen.
        Fix cleanup proceedure.
        Copied libstdc++.so to sobjs/ for compatibility.
        Fix is_readable_address.
        Fix interaction with subwindows.
        Update MemorySurfer interface when selected.
        Version string added.

2.0.2 : Fix repository criss-cross.

2.0.1 : Fix gui issue with Configure window.
        Fix gui error when cancelling load.
        Enable globals using checkbox.
        
2.0.0 : Fix various symbol types.
        Fix crashes and stalls.
        Brush over of entire workflow.
        Added MemorySurfer for low level assembly debugging.

1.0.1 : Fix ability to load/run self.

1.0.2 : Fix problems related to c++.

== Donations:

PayPal : alfkil()gmail.com

== Thanks to:

kas1e, Thomas Frieden, Colin Wenzel, nbache, NinjaDNA and Jamie Krueger.
And the entire Amiga community.