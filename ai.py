import sys
import time
import threading
from queue import Empty
from stdio_ipc import ChildProcess



def spawnAI(args, save_stdin_path, save_stdout_path, save_stderr_path):
    try:
        ai = ChildProcess(args, save_stdin_path,
                        save_stdout_path, save_stderr_path)
        return ai
    except:
        return {'err': 'fail to spawn the program.' + str(sys.exc_info()[1])}

class AI(object):
    

    def __init__(self, file, id, pros):
        self.name = 'ai' + str(id)
        self.id = id
        self.file = file
        self.err = ""
        self.pros = pros

    def load(self):
        self.ai = spawnAI(self.file, self.name + '_stdin' + str(self.pros) + '.log',
                          self.name + '_stdout' + str(self.pros) + '.log', self.name + '_stderr' + str(self.pros) + '.log')
        if type(self.ai) == dict:
            self.err = str(self.ai)
            return False
        return True

    def init(self, side):
        self.side = side
        try:
            self.ai.send(side)
            self.name = self.ai.recv(timeout=3)
        except Empty as e:
            self.err = "Timeout when asking about name"
            return False
        except Exception as e:
            self.err = str(e)
            return False
        return True

    def run(self, board, instruction=""):
        def valid(x):
            xl = x.split()
            if len(xl) != 2:
                self.err = "length Error"
                raise IOError
            for word in xl:
                if not word.isdigit():
                    self.err = "Output not digits"
                    raise IOError
        feedback = ""
        try:
            if instruction != "":
                self.ai.send(instruction)
            self.ai.send('Action')
            feedback = self.ai.recv(timeout=1)
            valid(feedback)
            loc = list(map(int, feedback.split()))
            rec = board.update(loc)
            if rec != True:
                self.err = rec
                raise IOError
            if board.result() < 2:
                return True
        except Empty as e:
            self.err = "Timeout while running"
            return False
        except Exception as e:
            self.err = str(e)
            return False
        return feedback