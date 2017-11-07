import sys
import time
import threading
import locale
import random
import json
import copy
from board import Board
from queue import Empty
from pprint import pprint
from stdio_ipc import ChildProcess

steps = 0
p2dv = False
record_json = {}
running = True
ai = 0
exec_file = "./cpp/"
pros = 0

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
        if type(ai) == dict:
            self.err = str(ai)
            return False
        return True

    def init(self, side):
        self.side = side
        try:
            self.ai.send(side)
            self.name = self.ai.recv(timeout=5)
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
            feedback = self.ai.recv(timeout=2)
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

def step_ai():
    global steps, ai, nw, ins, should_reverse
    suc = ai.run(nw, ins)
    if type(suc) != str:
        if suc:
            finish(1 - nw.result() if should_reverse else nw.result())
            return 1 - nw.result() if should_reverse else nw.result()
        else:
            finish(0)
            return 0
    else:
        ins = suc
    steps += 1
    if steps > 99:
        finish(2)
        return 2
    return -1

def state():
    global nw, should_reverse
    length = 20
    state = []
    if (should_reverse == 0):
        for i in range(1, length):
            state.append([])
            for j in range(1, length):
                state[i - 1].append(1 if nw.board[i][j] == True else 0)
        state[nw.loc[0][0] -1 ][nw.loc[0][1] - 1] = 2
        state[nw.loc[1][0] - 1][nw.loc[1][1] - 1] = 3
    else:
        for i in range(1, length):
            state.append([])
            for j in range(1, length):
                state[i - 1].append(1 if nw.board[length - i][j] == True else 0)
        state[length - nw.loc[0][0] - 1][nw.loc[0][1] - 1] = 3
        state[length - nw.loc[1][0] - 1][nw.loc[1][1] - 1] = 2
    return state

def step(x, y):
    global nw, should_reverse, steps
    if (should_reverse == 0):
        res = nw.update([x + 1, y + 1])
    else:
        res = nw.update([19 - x, y + 1])
    steps += 1
    if res != True:
        finish(1)
        return state(), "Invalid"
    
    if nw.result() < 2:
        finish(ans)
        ans = 1 - nw.result() if should_reverse else nw.result()
        return state(), copy.deepcopy(ans)
    else:
        ans = 2
    step_ans = step_ai()
    if step_ans != -1:
        return state(), copy.deepcopy(step_ans)
    if nw.result() < 2:
        finish(ans)
    return state(), copy.deepcopy(ans)

def init_judge(ai_id, _pros):
    global steps, ai, turn, record_json, nw, ins, should_reverse
    nw = Board(record_json)
    pros = _pros
    json_out = open('result' + str(pros) + '.json', 'w')
    ai = AI(exec_file + str(ai_id), 1, pros)
    suc = ai.load()
    if not suc:
        return finish(1)
    should_reverse = turn = random.randint(0, 1)
    print(should_reverse)
    record_json['id'] = [turn, 1 - turn]
    suc = ai.init(1- turn)
    record_json['user'] = [" ", ai.name]
    record_json['step'] = []
    ins = ""
    if turn == 1:
        step_ai()
    return state(), copy.deepcopy(nw.result()), turn

def finish(winner):
    global running, record_json, ai
    record_json['total'] = steps
    record_json['result'] = winner
    record_json['err'] = ['', ai.err]
    print(ai.ai)
    print(ai.err)
    if type(ai.ai) is not dict:
        ai.ai.exit()

    print(record_json)

    json_out = open('result' + str(pros) + '.json' , 'w')
    json.dump(record_json, json_out)
    json_out.close()
    running = False