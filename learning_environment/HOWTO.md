# **HOWTO** #

1. Write a controller
2. Find the current score in a game that is not listed in score_description.txt


Write a controller
==================

The best way to get to know the interface is to look at [sit.c](example_agents/sit.c) and to
[randomplayer.py](example_agents/randomplayer.py) for the C and Python bindings respectively.

In both cases the interface is almost identical.  Only one function
whose name is predefined is necessary to be supplied by the agent
library. This should be called le_get_functions. This function is supposed to 
return a
le_functions structure (look at [learning-environment-common.h](learning-environment-common.h)
for all structure definitions). This contains pointers to functions that

* Gets called once at start up aka the start function
* Gets called once on exit aka the exit function
* Takes an update every frame of the current score, whether the game is over and 
  a snapshot of the framebuffer (the update function)
* Gets polled on each frame for the state of each of the buttons (eg up, down, button 1, etc)
  (the actions function)
* Gets polled on each frame to check whether the MAME emulator should be reset aka the reset function
* Takes a dump of the memory every frame aka the memory consumer function

Any of these functions can be set to null if you do not want that function called. eg you might not care
about the state of the memory, so would set the le_functions.consume_memory function to null.

Flow of calls
-------------

This is a summary of the order of the calls to your agent:

1. MAME starts, le_get_functions is called with the parameters passed on the 
command line through le_options.

2. start function called and passed a structure specifying the name
of the game, the bitmap size to be expected, and a list of the buttons
that the game uses (eg galaxian only uses left, right and button 1)

3. While in game:
  * update function is called with current score, whether the game is over
  and the current frame buffer.  The update function is meant to return the number of
  frames to skip till the next update (0 to receive the next
  update available). This is used just to speed up processing in
  case that not all updates are desired.  Even if the update is
  not sent, the actions will still be polled on every frame.

  * memory consumer is called with a dump of the memory

  * actions function is called with no parameters asking for the
  state of the buttons (non-zero for pressed, zero for not
  pressed). Look in learning-environment-common.h for the order of the buttons.  

  * the reset checker is called with no parameters. It should return with a non-zero
  value if the MAME environment should be reset

4. on exit, the exit function is called with no parameters.


The format of the bitmap passed to the agent is BGRA (although this
will probably be run-time selectable in the future).  That is, one
unsigned byte for the blue, green, red and alpha values of each pixel
in turn.  The alpha channel can be ignored since it will always be
255. In the python bindings, this is passed as a memoryview.  Look at
scorepicker.py to find out how to convert it into a more usual image format
through PIL. Note that the memoryview is only valid through the duration 
of the update call. If you want to keep it you should make a copy of it

The format of the memory dumped is as a linked list of le_memory_t structs (see 
learning-environment-common.h). In Python, this gets translated to a list of 
(memory_start, memoryview) objects. Similarly to the bitmap, the memoryview 
objects are only valid during the call to the memory consumer.


Finding the current score
=========================

TOWRITE
