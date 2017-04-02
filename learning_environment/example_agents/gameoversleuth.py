"""
Helps to work out what memory locations change when the game is over.

Press the 'g' key while the state of the game is in "Game over" mode to take snapshots of 
the memory while in game over mode

Press the 's' key when the game starts. Pressing the 'w' key will write the last 100 frames,
a list of memory bits that were never set to 0, and a list of memory bits that were
never set to 1.

If you press 'w' as soon as the game is over, you should be able to extract a bit that
will work as a game over detector which you can then enter into ../gameover_description.txt

Call with a command line like:
mame -use_le -le_library pythonbinding.so -le_options gameoversleuth <romname>
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


def write_memory_to_filename(filename, memory, region_starts):
    with open(filename, 'w') as memory_out:
        for start, region in zip(region_starts, memory):
            # print the memory in a readable format, 50 bytes at a time
            memory_out.write("Starting at %s\n" % start)
            for line in atatime(region[1], 50):
                memory_out.write('%s\n' % ''.join('%02x' % ord(byte) for byte in line))

def write_differences_to_filename(filename, differences):
    with open(filename, 'w') as out:
        for difference in differences:
            address, bit = difference
            out.write("%s:%s\n" % (address, bit))

def different_bits(previous, current, region_starts):
    """
    Return list of different bits between the two snapshots listed as lists of ints
    """

    differences = []

    for region_start, previous_region, current_region in zip(region_starts, previous, current):
        for index, (old_byte, new_byte) in enumerate(zip(previous_region, current_region)):
            difference = old_byte ^ new_byte
            if difference:
                for location in range(8):
                    if difference & (1 << location):
                        differences.append((index + region_start, location))

    return differences

def validate_differences(differences, snapshots, region_starts, on):
    """
    Filter the differences to the ones that match the game over snapshots that we've taken

    if 'on' is True, it means the bits have to be on in the game over snapshots, otherwise
    they have to be off
    """

    filtered = []
    for difference in differences:
        all_ok = True
        for snapshot in snapshots:
            address = difference[0]
            bit_filter = 1 << difference[1]
            for start, region in zip(region_starts, snapshot):
                if start <= address < start + len(region):
                    # this section
                    value_in_snapshot = bool(region[address - start] & bit_filter)
                    break
            else:
                raise ValueError("Couldn't find the RAM location in the regions given")

            if value_in_snapshot != on:
                all_ok = False
                break
        if all_ok:
            filtered.append(difference)

    return filtered



def le_get_functions(args):


    state = GameOverSleuth(args)
    return (state.start, state.update, state.get_actions, state.check_reset, state.shutdown, state.consume_memory)


class GameOverSleuth(object):
    def __init__(self, args):

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

        self.memory_start_addresses = None
        self.initial_memory = None
        self.ored_memory = [None] * self.frames
        self.anded_memory = [None] * self.frames
        self.game_over_snapshots = []
        self.unpackers = None

        logging.basicConfig(level=logging.DEBUG)


    def start(self, game_name, width, height, buttons_used):
        self.game_name = game_name
        self.width = width
        self.height = height
        self.buttons_used = buttons_used

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
                write_memory_to_filename(memory_filename, self.memory[frame], self.memory_start_addresses)

                image_filename = os.path.join(directory, 'image_%s.png' % index)
                image = Image.frombuffer("RGBA",(self.width, self.height), self.images[frame],'raw', ("BGRA",0,1))
                image.putalpha(255)
                image.save(image_filename)

                frame += 1
                if frame >= self.frames:
                    frame = 0


            if self.initial_memory is not None:
                write_memory_to_filename(os.path.join(directory, 'initial_memory.txt'), self.initial_memory, self.memory_start_addresses)

                # check through ored and anded memory
                # if anything changed this late, it's likely to be relevant

                frame = (start_frame + 1) % self.frames
                for index in range(frame_count-1):
                    previous_frame_index = (frame - 1) % self.frames
                    differences = different_bits(self.ored_memory[previous_frame_index], self.ored_memory[frame], self.memory_start_addresses)
                    differences = validate_differences(differences, self.game_over_snapshots, self.memory_start_addresses, True)
                    if differences:
                        write_differences_to_filename(os.path.join(directory, 'diff_or_%s.txt' % index), differences)

                    differences = different_bits(self.anded_memory[previous_frame_index], self.anded_memory[frame], self.memory_start_addresses)
                    differences = validate_differences(differences, self.game_over_snapshots, self.memory_start_addresses, False)                    
                    if differences:
                        write_differences_to_filename(os.path.join(directory, 'diff_and_%s.txt' % index), differences)

                    frame += 1
                    if frame >= self.frames:
                        frame = 0

            self.stdscr.addstr(0, 0, "Done")
            self.stdscr.refresh()

        elif key == ord('g'):
            # record the current memory as a "Game over" snapshot used to filter potential candidates.
            current_frame_index = (self.update_count-1) % self.frames
            current = self.memory[current_frame_index]
            if current is not None:
                if self.unpackers is None:
                    self.unpackers = [struct.Struct('!' + 'B' * len(region)) for region in current]

                self.game_over_snapshots.append([list(unpacker.unpack(region[:])) for unpacker, region in zip(self.unpackers, current)])
                self.stdscr.addstr(0, 0, "Snashot taken")
            else:
                self.stdscr.addstr(0, 0, "Not ready")

            self.stdscr.refresh()
        elif key == ord('s'):
            # start a game
            current_frame_index = (self.update_count-1) % self.frames
            current = self.memory[current_frame_index]
            if current is not None:
                self.initial_memory = [region[:] for region in current]
                if self.unpackers is None:
                    self.unpackers = [struct.Struct('!' + 'B' * len(region)) for region in self.initial_memory]
                self.ored_memory[current_frame_index] = [list(unpacker.unpack(region)) for unpacker, region in zip(self.unpackers, self.initial_memory)]
                self.anded_memory[current_frame_index] = [list(unpacker.unpack(region)) for unpacker, region in zip(self.unpackers, self.initial_memory)]

                self.stdscr.addstr(0, 0, "Game started")
            else:
                self.stdscr.addstr(0, 0, "Not ready")
            self.stdscr.refresh()


    def update(self, score, game_over, video_frame):
        self.handle_input()        
        self.update_count += 1
        self.current_score = score

        self.images[(self.update_count-1) % self.frames] = video_frame[:]

        return 0  #number of frames you want to skip

    
    def get_actions(self):
        return self.actions

    def check_reset(self):
        return False

    
    def shutdown(self):
        # set the terminal back to normality
        curses.nocbreak()
        self.stdscr.keypad(False)
        curses.echo()
        curses.endwin()
    
    def consume_memory(self, memory):
        self.memory_start_addresses = [region[0] for region in memory]
        current_frame_index = (self.update_count-1) % self.frames
        self.memory[current_frame_index] = [region[1][:] for region in memory]

        if self.initial_memory is not None:
            previous_frame_index = (self.update_count-2) % self.frames
            current = [unpacker.unpack(region[1]) for unpacker, region in zip(self.unpackers, memory)]

            # This is dog-slow code. We'll speed it up when we need to
            self.ored_memory[current_frame_index] = [[(old_byte | new_byte) for old_byte, new_byte in zip(previous_region, current_region)] for previous_region, current_region in zip(self.ored_memory[previous_frame_index], current)]
            self.anded_memory[current_frame_index] = [[(old_byte & new_byte) for old_byte, new_byte in zip(previous_region, current_region)] for previous_region, current_region in zip(self.anded_memory[previous_frame_index], current)]

if __name__ == '__main__':
    print(__doc__)
