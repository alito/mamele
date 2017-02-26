"""
Helps to work out what memory changes when the game ends.
Press a key when the game is over. This will dump the last 100 frames of memory and images on a directory

Call with a command line like:
mame -use_le -learning_environment pythonbinding.so -le_options gameoversleuth <romname>
"""

import os, sys, logging
import datetime
import struct

import curses
from PIL import Image

DefaultFrames = 100


def atatime(iterator, count):
    """
    Returns a generator <count> items at a time.
    Unlike other versions on the net (eg http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/439095),
    this one returns the leftovers as is
    ie
    >>> for l in atatime(range(10),3):
    ...     print list(l)
    ... 
    [0, 1, 2]
    [3, 4, 5]
    [6, 7, 8]
    [9]


    This means that the only way to get results from it safely is to return them into one variable and go from there
    """
    itr = iter(iterator)
    cargo = []
    i = 0
    for item in itr:
        cargo.append(item)
        i += 1
        if i >= count:
            yield tuple(cargo)
            i = 0
            cargo = []
    if len(cargo) > 0:
        yield tuple(cargo)


def write_memory_to_filename(filename, memory):
    with open(filename, 'w') as memory_out:
        # print the memory in a readable format, 50 bytes at a time
        for line in atatime(memory, 50):
            memory_out.write('%s\n' % ''.join('%02x' % ord(byte) for byte in line))

def write_differences_to_filename(filename, differences):
    with open(filename, 'w') as out:
        for difference in differences:
            out.write("%s:%s\n" % tuple(difference))

def different_bits(previous, current):
    """
    Return list of different bits between the two snapshots listed as lists of ints
    """

    differences = []
    for index, (old_byte, new_byte) in enumerate(zip(previous, current)):
        difference = old_byte ^ new_byte
        if difference:
            for location in range(8):
                if difference & (1 << location):
                    differences.append((index, location))

    return differences

def le_get_functions(args, game_name, width, height, buttons_used):


    state = GameOverSleuth(game_name, width, height, buttons_used, args)
    return (state.update, state.get_actions, state.check_reset, state.shutdown, state.consume_memory)


class GameOverSleuth(object):
    def __init__(self, game_name, width, height, buttons_used, args):
        self.game_name = game_name
        self.width = width
        self.height = height
        self.buttons_used = buttons_used

        self.actions = [False for i in range(12)]

        self.update_count = 0
        self.current_score = 0

        # setup to capture keystrokes
        self.stdscr = curses.initscr()
        curses.cbreak()
        self.stdscr.keypad(True)
        self.stdscr.nodelay(True)

        self.frames = DefaultFrames
        if args:
            try:
                self.frames = int(args)
            except ValueError:
                # ignore
                pass

        
        self.memory = [None] * self.frames
        self.images = [None] * self.frames

        self.initial_memory = None
        self.ored_memory = [None] * self.frames
        self.anded_memory = [None] * self.frames
        self.unpacker = None

        logging.basicConfig(level=logging.DEBUG)


    def handle_input(self):
        key = self.stdscr.getch()
        if key == ord('w'):
            self.stdscr.addstr(0, 0, "Writing")
            self.stdscr.refresh()

            directory = '%s_%s' % (self.game_name, datetime.datetime.now().strftime('%Y%m%d-%H%M%S'))
            os.mkdir(directory)

            if self.update_count <= self.frames:
                start_frame = 0
                frame_count = self.update_count

            else:
                start_frame = self.update_count % self.frames
                frame_count = self.frames

            frame = start_frame
            for index in range(frame_count):
                memory_filename = os.path.join(directory, 'memory_%s.txt' % index)
                write_memory_to_filename(memory_filename, self.memory[frame])

                image_filename = os.path.join(directory, 'image_%s.png' % index)
                image = Image.frombuffer("RGBA",(self.width, self.height), self.images[frame],'raw', ("BGRA",0,1))
                image.save(image_filename)

                frame += 1
                if frame >= self.frames:
                    frame = 0


            if self.initial_memory is not None:
                write_memory_to_filename(os.path.join(directory, 'initial_memory.txt'), self.initial_memory)

                # check through ored and anded memory
                # if anything changed this late, it's likely to be relevant

                frame = (start_frame + 1) % self.frames
                for index in range(frame_count-1):
                    previous_frame_index = (frame - 1) % self.frames
                    differences = different_bits(self.ored_memory[previous_frame_index], self.ored_memory[frame])
                    if differences:
                        write_differences_to_filename(os.path.join(directory, 'diff_or_%s.txt' % index), differences)

                    differences = different_bits(self.anded_memory[previous_frame_index], self.anded_memory[frame])
                    if differences:
                        write_differences_to_filename(os.path.join(directory, 'diff_and_%s.txt' % index), differences)

                    frame += 1
                    if frame >= self.frames:
                        frame = 0

            self.stdscr.addstr(0, 0, "Done")
            self.stdscr.refresh()
        elif key == ord('s'):
            # start a game
            current_frame_index = (self.update_count-1) % self.frames
            self.initial_memory = self.memory[current_frame_index][:]
            self.unpacker = struct.Struct('!' + 'B' * len(self.initial_memory))
            self.ored_memory[current_frame_index] = list(self.unpacker.unpack(self.initial_memory))
            self.anded_memory[current_frame_index] = list(self.unpacker.unpack(self.initial_memory))

            self.stdscr.addstr(0, 0, "Game started")
            self.stdscr.refresh()


    def update(self, score, game_over, video_frame):
        """
        This will be called with a score if available (otherwise zero), and the video_frame
        
        The frame can be converted to a nice PIL image with something like

        frame = PIL.Image.frombuffer("RGBA",(self.width, self.height),video_frame,'raw', ("BGRA",0,1))

        Return the number of frames you want skipped before being called again.  Due to conversions, it's much faster
        to return a positive number here than to keep an internal count on when to react
        """


        self.handle_input()        
        self.update_count += 1
        self.current_score = score

        self.images[(self.update_count-1) % self.frames] = video_frame[:]
        
        return 0  #number of frames you want to skip

    
    def get_actions(self):
        return self.actions

    def check_reset(self):
        """
        This will be called after get_actions every frameskip frames. Return whether
        you want the emulator reset
        """ 
        return False

    
    def shutdown(self):
        """
        This will be called when MAME shuts down
        """

        # set the terminal back to normality
        curses.nocbreak()
        self.stdscr.keypad(False)
        curses.echo()
        curses.endwin()
    
    def consume_memory(self, memory):
        current_frame_index = (self.update_count-1) % self.frames
        self.memory[current_frame_index] = memory[:]

        if self.initial_memory is not None:
            previous_frame_index = (self.update_count-2) % self.frames
            current = self.unpacker.unpack(memory)

            # This is dog-slow code. We'll speed it up when we need to
            self.ored_memory[current_frame_index] = [(old_byte | new_byte) for old_byte, new_byte in zip(self.ored_memory[previous_frame_index], current)]
            self.anded_memory[current_frame_index] = [(old_byte & new_byte) for old_byte, new_byte in zip(self.anded_memory[previous_frame_index], current)]

