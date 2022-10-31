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

== History:

2.2.0 : Fix menus further.
        Fix stacktrace.
        Fix iconify.
        Enable split windows (experimental).
        Fix copyright symbol in about (c).
        Remove stray debug output when doing iconify.

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

Thomas Frieden, Colin Wenzel, nbache, kas1e, NinjaDNA and Jamie Krueger. And the entire Amiga
community.