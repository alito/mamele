"""
Dummy controller that saves the images it receives to a directory. It also presses keys at random. 
This is just to help with debugging

Call with a command line like:
mame64 -use_le -le_library python3binding.so -le_options 'screenrecorder.py <directory_where_to_save_images>' <romname>
"""

import sys
import random
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("You need PIL installed to run this. python3-pil in Ubuntu", file=sys.stderr)
    raise

def le_get_functions(args):
    """
    This function has to be called le_get_functions

    args is the rest of the command-line options passed in on -le_options to MAME after the name 
    of the module
    
    It should return a 6-tuple listing the function to call at the start with the game info (name, 
    width, height, buttons used), the function to callwith updates on framebuffer, the function 
    to be called to get agent input, the function to call to check whether to reset MAME, the 
    function to be called when MAME shuts down, and the function that gets passed the contents
    of the memory (this last one is rarely needed) 

    Any of them can be set to None if you don't want to be notified of that event type.
    """
    state = ScreenRecorder(args)
    return (state.start, state.update, state.get_actions, state.check_reset, state.shutdown, None)

class ScreenRecorder(object):
    def __init__(self, args):
        # some useful constants
        left_arrow_button = Button(0)       
        right_arrow_button = Button(1)
        up_arrow_button = Button(2)     
        down_arrow_button = Button(3)

        button1 = Button(4)
        button2 = Button(5)
        button3 = Button(6)
        button4 = Button(7)
        button5 = Button(8)
        button6 = Button(9)

        self.ArrowKeyNames = {
            'left' : left_arrow_button, 
            'right' : right_arrow_button, 
            'up' : up_arrow_button, 
            'down' : down_arrow_button
        }
        self.ActionKeyNames = {'button1' : button1, 'button2' : button2, 'button3' : button3, 'button4' : button4,
                               'button5' : button5, 'button6' : button6}

        self.coin_button = Button(10)
        self.player1_button = Button(11)

        self.arrow_buttons = (left_arrow_button, right_arrow_button, up_arrow_button, down_arrow_button)
        self.action_buttons = (button1, button2, button3, button4, button5, button6)
        self.game_buttons = self.arrow_buttons + self.action_buttons
        self.misc_buttons = (self.coin_button, self.player1_button)
        self.button_order = self.arrow_buttons + self.action_buttons + self.misc_buttons
        self.actions = [False for i in self.button_order]

        self.update_count = 0
        self.current_score = 0
        self.game_over = False
        self.seen_memory = False

        if args:
            self.save_directory = Path(args[0])
        else:
            self.save_directory = None

    def start(self, game_name, width, height, buttons_used):
        """
        This will be called early on, with information about the game name, its frame size
        and the buttons used by the game
        """
        self.game_name = game_name
        self.width = width
        self.height = height
        self.buttons_used = buttons_used

        # if a save directory wasn't specified, give it a reasonable name
        if self.save_directory is None:
            self.save_directory = Path('%s_images' % game_name)
    
        if not self.save_directory.is_dir():
            self.save_directory.mkdir(parents=True, exist_ok=True)

        print (self.buttons_used)
        
    def update(self, score, game_over, video_frame):
        """
        This will be called with a score if available (otherwise zero), and the video_frame
        
        The frame can be converted to a nice PIL image with something like

        frame = Image.frombytes("RGBA",(self.width, self.height), video_frame.tobytes(),'raw', ("BGRA",0,1))
        frame.putalpha(255) # since the alpha channel is sometimes set to 0 for no reason

        Return the number of frames you want skipped before being called again.  Due to conversions, it's much faster
        to return a positive number here than to keep an internal count on when to react
        """

        self.update_count += 1
        if score != self.current_score:
            print ("Score: %s" % score)
            self.current_score = score
        if game_over:
            if not self.game_over:
                print ("Game Over")
                self.game_over = True
        elif self.game_over:
            self.game_over = False
            print ("New game")

        frame = Image.frombytes("RGBA",(self.width, self.height), video_frame.tobytes(),'raw', ("BGRA",0,1))
        image_filename = self.save_directory / ('image%06d.png' % self.update_count)
        frame.save(image_filename)

        return 0  #number of frames you want to skip

    
    def get_actions(self):
        """
        This will also be called on each frame update to get the actions of the agent.

        A list of the state of the 12 buttons should be returned
        """

        # update each button. Each one has a 3% chance of toggling
        self.actions = []
        for button, used in zip(self.button_order, self.buttons_used):
            if used:
                button.update()
                self.actions.append(button.state)
            else:
                self.actions.append(False)

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
        pass
    

class Button(object):
    """
    Button class that knows how to toggle itself and will with probability ~ 3% on each frame
    """
    def __init__(self, number_in_c):
        self.state = False
        self.last_state = False
        self.number_in_c = number_in_c

        if self.number_in_c >= 10:
            self.frequency = 1.0 / 250.
        else:
            self.frequency = 1.0 / 30.

    def press(self):
        self.state = True

    def release(self):
        self.state = False

    def toggle(self):
        self.state = not self.state

    def changed(self):
        return self.state != self.last_state

    def tick(self):
        self.last_state = self.state

    def update(self):
        if random.random() < self.frequency:
            self.toggle()
