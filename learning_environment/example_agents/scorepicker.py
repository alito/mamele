"""
Helps to work out the location of the score in memory

Pressing the 'w' key will write the memory contents and an image snapshot. You can then quit and run:

python scorepicker.py -s <the_score_when_you_pressed_w> <the_memory_file_it_wrote_while_running>

Enter score locations into ../score_description.txt

Call with a command line like:
mame -use_le -le_library python2binding.so -le_options scorepicker <romname>
"""

import os, sys, logging
import datetime

import curses
from PIL import Image


if sys.version_info >= (3, 0):
    def to_byte_array(memoryview):
        return memoryview.tobytes()
else:
    def to_byte_array(memoryview):
        return bytearray(memoryview.tobytes())


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
            memory_out.write('%s\n' % ''.join('%02x' % byte for byte in line))


def le_get_functions(args):


    state = ScorePicker(args)
    return (state.start, state.update, None, None, state.shutdown, state.consume_memory)


class ScorePicker(object):
    def __init__(self, args):

        self.actions = [False for i in range(12)]

        self.update_count = 0
        self.current_score = 0

        # setup to capture keystrokes
        self.stdscr = curses.initscr()
        curses.cbreak()
        self.stdscr.keypad(True)
        self.stdscr.nodelay(True)
        
        self.memory = None
        self.image = None

        self.memory_start_addresses = None

        logging.basicConfig(level=logging.DEBUG)


    def start(self, game_name, width, height, buttons_used):
        self.game_name = game_name
        self.width = width
        self.height = height
        self.buttons_used = buttons_used

        self.directory = '%s_%s' % (self.game_name, datetime.datetime.now().strftime('%Y%m%d-%H%M%S'))
        os.mkdir(self.directory)


    def handle_input(self):
        key = self.stdscr.getch()
        if key == ord('w'):
            self.stdscr.addstr(0, 0, "Writing")
            self.stdscr.refresh()

            for start_address, memory in zip(self.memory_start_addresses, self.memory):
                memory_filename = os.path.join(self.directory, 'memory_%s_start_%s.txt' % (self.update_count, start_address))
                write_memory_to_filename(memory_filename, memory)

            image_filename = os.path.join(self.directory, 'image_%s.png' % self.update_count)
            image = Image.frombytes("RGBA",(self.width, self.height), self.image,'raw', ("BGRA",0,1))
            image.putalpha(255)
            image.save(image_filename)

            self.stdscr.addstr(0, 0, "Done")
            self.stdscr.refresh()


    def update(self, score, game_over, video_frame):
        self.handle_input()        
        self.update_count += 1
        self.current_score = score

        self.image = video_frame.tobytes()

        return 0  #number of frames you want to skip

    
    
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
        self.memory_start_addresses = [region[0] for region in memory]
        self.memory = [to_byte_array(region[1]) for region in memory]


def find_score(args):
    from argparse import ArgumentParser
    
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('-s', '--score', dest='score', type=int, required=True,
        help="Score to look for")    
    parser.add_argument('filenames', nargs="+",
        help="Memory filenames to look over")

    parameters = parser.parse_args(args)

    # look for the score in hex
    score_in_hex = "%x" % parameters.score
    if len(score_in_hex) % 2 == 1:
        score_in_hex = '0' + score_in_hex

    to_look_for = [score_in_hex]

    # little-endian 
    if len(score_in_hex) == 4:
        to_look_for.append(''.join([score_in_hex[2:], score_in_hex[0:2]]))

    # that strange readable format
    score_in_decimal = "%s" % parameters.score
    if len(score_in_decimal) % 2 == 1:
        score_in_decimal = '0' + score_in_decimal

    to_look_for.append(score_in_decimal)

    if len(score_in_decimal) == 4:
        to_look_for.append(''.join([score_in_decimal[2:], score_in_decimal[0:2]]))

    multi = len(parameters.filenames) > 1
    for filename in parameters.filenames:
        if multi:
            prefix = '%s: ' % filename
        else:
            prefix = ''
        base = os.path.basename(filename)
        try:
            address_start = int(base.rsplit('_', 1)[1].split('.')[0])
        except IndexError:
            address_start = 0

        with open(filename) as file_input:
            contents = file_input.readlines()

        memory = ''.join(line.strip() for line in contents)

        for candidate in to_look_for:
            position = 0
            while True:
                position = memory.find(candidate, position)
                if position < 0:
                    break
                if position % 2 == 0:
                    print("%s%s (%s)" % (prefix, position / 2 + address_start, candidate))
                position += 1


    return 0

if __name__ == '__main__':
    sys.exit(find_score(sys.argv[1:]))
