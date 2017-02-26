# **mamele** #

The MAME learning environment. This is a fork of the main MAME repository to let programmed agents
play the games supported by the MAME emulator. If you are familiar with the 
[Arcade Learning Environment](http://www.arcadelearningenvironment.org/) then mamele is to MAME as the
Arcade Learaning Environment is to Stella.

How to run
==========

Compile the mame binary with `make`

It is easiest to understand how this works by looking at an example. There are two trivial example
agents in the [learning_environment/example_agents](example_agents) directory. 
Change into that directory and run `make`. This will compile the python bindings and sit.so which is
an agent that sits there and takes it.

You can then run:

`../../mame64 -use_le -learning_environment sit.so galaxian`

to watch an agent get killed while sitting. Looking at [sit.c](example_agents/sit.c) is a good starting point for an agent
that will show you all the points you can modify to make the agent actually act.

You can also test out the Python bindings (Python 2.x for now only sorry. I need to learn about
Python 3 bindings). The Python module to load is passed in the -le_options argument.

`../../mame64 -use_le -learning_environment pythonbinding.so -le_options randomplayer galaxian`

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
-learning_environmentlibrary that should be given control to
-show_le             display screen when learning environment is enabled (dangerous to turn off)
-le_options          options to pass to the learning environment
-le_player           player that learning environment plays as
...
```

* -use_le is necessary if you are going to use the learning environment. This is so that the binary
can be used as the main mame binary if this is not passed
* -learning_environment should point to a library linked like sit.so
* -le_options lets you pass options to the agent
* -le_player lets you select to use player 2 if you want to. This isn't well tested
* -show_le means that MAME will display the screen as usual. -noshow_le would turn this off

How to write a controlling agent
================================

See [the HOWTO.md file](HOWTO.md).


History
=======

This project started in 2005 as xmameac as a patch to xmame, then moved to sdlmame when xmame died,
and then changed name to CCPMAME when sdlmame got absorbed into the mainline MAME. With the 
recent flourishing of environments for machine learning agents seemingly triggered by DeepMind's
success using the ALE environment and the transition of MAME to the GPL licence, I've put this
up in GitHub as a self-contained fork.

Details under [NEWS.md](NEWS.md)

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