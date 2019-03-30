# **mamele** #

The MAME learning environment. This is a fork of the main MAME repository to let programmed agents
play the games supported by the MAME emulator. If you are familiar with the 
[Arcade Learning Environment](http://www.arcadelearningenvironment.org/) then mamele is to MAME as the
Arcade Learning Environment is to Stella.

How to run
==========

Compile the mame binary with `make`. 

It is easier to use the learning environment if 
you run `make install` (don't run this as root though). This just copies the mame64 binary to your
~/bin directory, and a couple of description files that the learning environment needs at runtime
to an ~/.le directory.  You can skip this step but then you'll need to specify where the files
are with the -le_datapath parameter.

It is easiest to understand how this works by looking at an example. There are two trivial example
agents in the [learning_environment/example_agents](example_agents) directory. 
Change into that directory and run `make`. This will compile the python bindings and sit.so which is
an agent that sits there and takes it.

You can then run:

`../../mame64 -use_le -le_library sit.so galaxian`

to watch an agent get killed while sitting. Looking at [sit.c](example_agents/sit.c) is a good starting point for an agent
that will show you all the points you can modify to make the agent actually act.

You can also test out the Python bindings. The Python module to load is passed in the -le_options argument.

`../../mame64 -use_le -le_library python2binding.so -le_options randomplayer galaxian`
or
`../../mame64 -use_le -le_library python3binding.so -le_options randomplayer galaxian`

depending on whether you are using Python 2 or 3

This agent will hit random buttons so it can be a bit more fun.


If you run mame with the -showusage, the LEARNING ENVIRONMENT OPTIONS section will give you the 
rest of the relevant options

```
$ ./mame64 -showusage
...
#
# LEARNING ENVIRONMENT OPTIONS
#
-use_le              enable learning environment
-le_library          library that should be given control to
-le_show             display screen when learning environment is enabled. This is the default.
-le_options          options to pass to the learning environment
-le_player           player that learning environment plays as
-le_datapath         directory where gameover and score data is stored
...
```

* -use_le is necessary if you are going to use the learning environment. This is so that the binary
can be used as the main mame binary if this is not passed
* -le_library should point to a library linked like sit.so
* -le_options lets you pass options to the agent
* -le_player lets you select to use player 2 if you want to. This isn't well tested
* -le_show means that MAME will display the screen as usual. -no_leshow would turn this off
* -le_datapath is only needed if you didn't run `make install`. It should specify the directory where
   gameover_description.txt and score_description.txt reside. They are in the learning_environment
   directory in the source.

How to write a controlling agent
================================

See [the HOWTO.md file](HOWTO.md).


History
=======

Under [NEWS.md](NEWS.md)

License
=======

Since this is a fork of MAME, this code is also distributed under the terms of the 
[GNU General Public License, version 2](http://opensource.org/licenses/GPL-2.0) or later (GPL-2.0+).
The files under this directory are also distributed under the BSD-3 License:

Copyright 2005-2017 Alejandro Dubrovsky

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.