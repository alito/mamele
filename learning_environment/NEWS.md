2025-04-27 - v4.4.276
---------------------

* Update to MAME 0276
* Remove support for Python 2
* Update docs a bit

2017-07-09 - v4.3.187
---------------------

* Add support for Python 3
* Fix memory leak in Python bindings

2017-06-29 - v4.2.187
---------------------

* Update to MAME 0187

2017-06-25 - v4.2.183
---------------------

* Score-finder script
* Less useless warnings
* Score for targ
* Get rid off the '0.' prefix to every version. It doesn't help anyone


2017-04-08 - v0.4.1-183
-----------------------

* Change the interface so that if Python bindings return None for a method, we can be more efficient in not doing work
* Add a -le_datapath parameter to specify where the score and game over description files are
* Quit when there are errors in the Python driver
* Fix game over detection for Galaxian. 
* Add support for circus, ripcord, crash and sidetrac.

2017-02-26 - v0.4.0-183
-----------------

* Move to GitHub
* Keep as a fork of upstream MAME
* Change name to MAME learning environment.
* Change all variables and functions to match newer name and lower-case format
* Extract the buttons actually used in the game from MAME and send it through
* Add a function to check whether we should reset the machine
* Add a function that consumes the memory
* Update to MAME 0.183

2012-02-13 - v0.3.3145
----------------------
* Update diff to MAME 0145

2012-02-05 - v0.3.2144
----------------------
* Examples/templates are documented better
* Python bindings slightly faster, especially for larger buffer sizes
* Can specify None for unwanted methods in Python bindings.

2012-01-23 - v0.3.1144
----------------------
* Get rid of installac.py
* Add installscores makefile target

2012-01-22 - v0.3.0144
----------------------
* Move over to main MAME distribution, since SDLMAME has been folded in
* Update diff to Mame 0.144
* Get rid of Python dependency

2010-01-10 - v0.2.3136
----------------------
* Update diff to SDLMame 0.136

2009-09-27 - v0.2.3134
----------------------
* Update diff to SDLMame 0.134

2009-04-11 - v0.2.3130
----------------------
* Update diff to SDLMame 0.130

2009-03-09 - v0.2.3129
----------------------
* Update diff to SDLMame 0.129

2007-07-22 - v0.2.2
-------------------
* Keys are no longer hard-wired.  Interface now extracts them from MAME.
* Add high score retrieval for puckmana.
* Clean up makefile a bit more.
* Add a small install script to copy 'scores.dat' to somewhere
* findable.
* Big internal changes.  As a side effect, AC's start function is
* now called just before the first frame is sent to it.

2007-07-16 - v0.2.1
-------------------
* Update diff to SDLMame 0.117
* Clean up the makefile a bit
* Get rid of warnings in ac code.

2007-07-15 - v0.2.0
-------------------
* Ported to SDLMame due to XMAME activity having nearly ceased.  In the
* process, move patch from the OSD layer down into the MAME core, so
* this patch should be portable to other operating systems without
* too much hassle (dynamic loading should be the main obstacle).
* Should work with all bit-depths now.
* Can play as player two.
* No need to specify full path of library any more.
* 
2006-04-23 - v0.1.4
-------------------
* Update diff to be in sync with XMAME 0.105

2006-02-28 - v0.1.3
-------------------
* Update diff to be in sync with XMAME 0.104

2006-01-28 - v0.1.2
-------------------
* Update diff to be in sync with XMAME 0.103

2005-12-29 - v0.1.1
-------------------
* Update diff to be in sync with XMAME 0.102

2005-11-08 - v0.1
-----------------
* Add 15 bit depth games (same code, just had to check it)
* Got rid of portage dependency in acutils.py
* Added dummy.py as sample python template

2005-10-29 - v0.0.3
-------------------
* Split package into framework and agents. Only release python
* bindings and shell python agent (rest are really not related)
* Add TODO file

2005-08-10 - v0.0.2
-------------------
* Major restructuring.  Separate XMAME interface
* from implementation so that there's no need to recompile XMAME 
* when updating a controller. Controller modules are
* now loaded at runtime. Now python binding is just a
* loadable module.

2005-08-02 - v0.0.1
-------------------
* Initial Release
