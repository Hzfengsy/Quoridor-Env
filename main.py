import sys
import time
import threading
import locale
import random
import json
from board import Board
from queue import Empty
from pprint import pprint
from stdio_ipc import ChildProcess

steps = 0
p2dv = False
record_json = {}
running = True
ai0 = 0
ai1 = 0


def finish(winner):
    global running, record_json, ai0, ai1
    record_json['total'] = steps
    record_json['result'] = winner
    record_json['err'] = [ai0.err, ai1.err]
    print(ai0.ai)
    print(ai1.ai)
    print(ai0.err, ai1.err)
    if type(ai0.ai) is not dict:
        ai0.ai.exit()
    if type(ai1.ai) is not dict:
        ai1.ai.exit()

    if not p2dv:
        print(record_json)

    json_out = open('result.json', 'w')
    json.dump(record_json, json_out)
    json_out.close()

    running = False
    sys.exit(0)


def spawnAI(args, save_stdin_path, save_stdout_path, save_stderr_path):
    try:
        ai = ChildProcess(args, save_stdin_path,
                          save_stdout_path, save_stderr_path)
        return ai
    except:
        return {'err': 'fail to spawn the program.' + str(sys.exc_info()[1])}


class AI(object):

    def __init__(self, file, id):
        self.name = 'ai' + str(id)
        self.id = id
        self.file = file
        self.err = ""

    def load(self):
        self.ai = spawnAI(self.file, self.name + '_stdin.log',
                          self.name + '_stdout.log', self.name + '_stderr.log')
        if type(ai0) == dict:
            self.err = str(ai0)
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


def judge():
    global steps, ai0, ai1, turn, record_json
    nw = Board(record_json)
    ai0 = AI(sys.argv[1], 0)
    ai1 = AI(sys.argv[2], 1)
    suc0, suc1 = ai0.load(), ai1.load()
    if not suc0 and not suc1:
        finish(2)
    if not suc0:
        finish(1)
    if not suc1:
        finish(0)

    should_reverse = turn = random.randint(0, 1)
    record_json['id'] = [turn, 1 - turn]

    suc0, suc1 = ai0.init(turn), ai1.init(1 - turn)
    if not suc0 and not suc1:
        finish(2)
    if not suc0:
        finish(1)
    if not suc1:
        finish(0)

    record_json['user'] = [ai0.name, ai1.name]
    record_json['step'] = []

    ins = ""
    while True:

        if turn == 0:  # To make the order right
            suc0 = ai0.run(nw, ins)
            if type(suc0) != str:
                if suc0:
                    finish(1 - nw.result() if should_reverse else nw.result())
                else:
                    finish(1)
            else:
                ins = suc0
            steps += 1
        turn = 0
        if steps > 99:
            finish(2)

        suc1 = ai1.run(nw, ins)
        if type(suc1) != str:
            if suc1:
                finish(1 - nw.result() if should_reverse else nw.result())
            else:
                finish(0)
        else:
            ins = suc1
        steps += 1
        if steps > 99:
            finish(2)


def p2dv():
    global running
    t = threading.Thread(target=judge)
    t.start()

    while True:
        line = sys.stdin.readline()
        if not running:
            sys.stderr.write('finished\n')
            sys.stderr.flush()
            break

        if line == 'get steps\n':
            sys.stderr.write('%d\n' % steps)

        sys.stderr.flush()


def main():
    global running, p2dv

    if (not len(sys.argv) in [3, 4]):
        print('usage:   ./main.py ai0Path ai1Path')
        print('example: ./main.py ./sample_ai ./sample_ai')
        print('')
        sys.exit(1)

    if len(sys.argv) == 4 and sys.argv[3] == 'p2dv':
        is_p2dv = True
        p2dv()
    else:
        is_p2dv = False
        judge()


locale.setlocale(locale.LC_ALL, 'en_US.UTF-8')
with open('record.txt', 'w') as f:
    f.write("Init\n")
main()
finish(2, ai0, ai1)
