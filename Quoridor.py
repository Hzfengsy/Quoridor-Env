import random
import json
import copy
import networkx as nx
from board import Board
from ai import AI

exec_file = "./cpp/"

class Quoridor(object):

    def __init__(self, pros):
        self.pros = pros
        self.size = 19
        self.ai_num = 5
        self.ai = ''

    def step_ai(self):
        if self.ai_id == 0:
            dis, path = self.findPath(1)
            x, y = path
            return self.step(x, y, 1)
        else:
            suc = self.ai.run(self.nw, self.ins)
            if type(suc) != str:
                if suc:
                    return self.finish(1 - self.nw.result() if self.should_reverse else self.nw.result())
                else:
                    return self.finish(0)
            else:
                self.ins = suc
            self.steps += 1
        return 2


    def reset(self):
        with open('record.txt', 'w') as f:
            f.write("Init\n")
        if self.ai != '' and type(self.ai.ai) is not dict:
            self.ai.ai.exit()
        self.ai_id = random.randint(1, self.ai_num)
        self.err = ['', '']
        self.record_json = {}
        self.running = True
        self.steps = 0
        self.nw = Board(self.record_json)
        self.json_out = open('result' + str(self.pros) + '.json', 'w')
        self.should_reverse = turn = random.randint(0, 1)
        # self.should_reverse = turn = 0
        self.record_json['id'] = [turn, 1 - turn]
        self.record_json['step'] = []
        self.ins = ""
        opp_state = self.state(1)
        if self.ai_id == 0:
            self.record_json['user'] = ["training", "short_path"]
        else:
            self.ai = AI(exec_file + str(self.ai_id), 1, self.pros)
            suc = self.ai.load()
            if not suc:
                print("fail")
                return copy.deepcopy(self.state(0)), copy.deepcopy(self.finish(0)), copy.deepcopy(opp_state), copy.deepcopy(-1), copy.deepcopy(self.ai_id)
            suc = self.ai.init(1- turn)
            if not suc:
                print("fail2")
                return copy.deepcopy(self.state(0)), copy.deepcopy(self.finish(0)), copy.deepcopy(opp_state), copy.deepcopy(-1), copy.deepcopy(self.ai_id)
            self.record_json['user'] = ["training", self.ai.name]
        if turn == 1:
            result = self.step_ai()
            if result != 2:
                k = -1
            else:
                xx, yy = map(int, self.ins.split())
                k = self.change_from_loc(xx, yy) if result == 2 else -1
        else:
            result = self.nw.result()
            k = -1
        
        return copy.deepcopy(self.state(0)), copy.deepcopy(result), copy.deepcopy(opp_state), copy.deepcopy(k), copy.deepcopy(self.ai_id)

    def state(self, id):
        length = 20
        state = []
        side = id ^ self.should_reverse
        if side == 0:
            for i in range(1, length):
                state.append([])
                for j in range(1, length):
                    state[i - 1].append(1 if self.nw.board[i][j] == True else 0)
            state[self.nw.loc[side][0] - 1][self.nw.loc[side][1] - 1] = 2
            state[self.nw.loc[side ^ 1][0] - 1][self.nw.loc[side ^ 1][1] - 1] = -2
        else:
            for i in range(1, length):
                state.append([])
                for j in range(1, length):
                    state[i - 1].append(1 if self.nw.board[length - i][j] == True else 0)
            state[length - self.nw.loc[side][0] - 1][self.nw.loc[side][1] - 1] = 2
            state[length - self.nw.loc[side ^ 1][0] - 1][self.nw.loc[side ^ 1][1] - 1] = -2
        return state

    def step(self, x, y, ai):
        side = ai ^ self.should_reverse
        if (side == 0):
            self.ins = str(x + 1) + ' ' + str(y + 1)
            res = self.nw.update([x + 1, y + 1])
        else:
            if x % 2 == 1 and y % 2 == 0:
                self.ins = str(17 - x) + ' ' + str(y + 1)
                res = self.nw.update([17 - x, y + 1])
            else:
                self.ins = str(19 - x) + ' ' + str(y + 1)
                res = self.nw.update([19 - x, y + 1])
        self.steps += 1
        if res != True:
            self.err[ai] = res
            return self.finish(ai ^ 1)
        if self.nw.result() < 2:
            ans = 1 - self.nw.result() if self.should_reverse else self.nw.result()
            return self.finish(ans)
        else:
            ans = 2
        return ans

    def finish(self, winner):
        self.record_json['total'] = self.steps
        self.record_json['result'] = winner
        self.record_json['err'] = [self.err, ' ']
        if self.ai != '' and type(self.ai.ai) is not dict:
            self.ai.ai.exit()

        json_out = open('result' + str(self.pros) + '.json' , 'w')
        json.dump(self.record_json, json_out)
        json_out.close()
        self.running = False
        return winner

    def build_graph(self, ai):
        state = copy.deepcopy(self.state(ai))
        u = ((0, 1), (0, -1), (1, 0), (-1, 0))
        G = nx.DiGraph()
        _G = nx.DiGraph()
        for i in range(1, self.size, 2):
            for j in range(1, self.size, 2):
                if state[i][j] == 2:
                    pos = (i, j)
                if state[i][j] == -2:
                    _pos = (i, j)
                G.add_node((i, j))
                _G.add_node((i, j))
        for i in range(1, self.size, 2):
            for j in range(1, self.size, 2):
                for k in range(len(u)):
                    if (state[i + u[k][0]][j + u[k][1]]):
                        continue
                    x = i + u[k][0] * 2
                    y = j + u[k][1] * 2
                    _G.add_weighted_edges_from([((i, j), (x, y), 1)])
                    if (state[x][y] == 0 or state[x][y] == 2):
                        G.add_weighted_edges_from([((i, j), (x, y), 1)])
                    else:
                        if state[x + u[k][0]][y + u[k][1]]:
                            for kk in range((k < 2) * 2, (k < 2) * 2 + 2):
                                if (state[x + u[kk][0]][y + u[kk][1]]):
                                    continue
                                G.add_weighted_edges_from([((i, j), (x + u[kk][0] * 2, y + u[kk][1] * 2), 1)])
                        else:
                            G.add_weighted_edges_from([((i, j), (x + u[k][0] * 2, y + u[k][1] * 2), 1)])
        return G, pos, _G, _pos

    def findPath(self, ai):
        G, pos, _G, _pos = self.build_graph(ai)
        path = nx.shortest_path(G, source = pos) 
        dis = nx.shortest_path_length(G, source = pos)
        min_dis = 1000
        min_path = ()
        for i in range(1, self.size, 2):
            now_dis = dis.get((17, i), 1001)
            if now_dis < min_dis or (now_dis == min_dis and random.randint(0, 4) == 0):
                min_dis = now_dis
                min_path = path[(17, i)][1]

        if min_path == ():
            _path = nx.shortest_path(_G, source = pos) 
            _dis = nx.shortest_path_length(_G, source = pos)
            for i in range(1, self.size, 2):
                now_dis = _dis.get((17, i), 1000)
                if now_dis < min_dis:
                    min_dis = now_dis
            if _dis[_pos] == 1:
                for i in range(1, self.size, 2):
                    for j in range(1, self.size, 2):
                        now_dis = dis.get((i, j), 1000)
                        if (now_dis == 1):
                            min_path = (i, j)
            else:
                min_path = _path[_pos][1]
            
        return copy.deepcopy(min_dis), copy.deepcopy(min_path)
        G, pos = self.build_graph(ai)
        path = nx.shortest_path(G, source = pos) 
        dis = nx.shortest_path_length(G, source = pos)
        min_dis = 1000
        min_path = ()
        for i in range(1, self.size, 2):
            now_dis = dis.get((17, i), 1000)
            if now_dis < min_dis:
                min_dis = now_dis
                min_path = path[(17, i)][1]
        if min_path == ():
            print(self.record_json)
            min_path = pos
        return copy.deepcopy(min_dis), copy.deepcopy(min_path)

    def wall_pos(self, kind):
        row = kind // 8
        col = kind % 8
        if row % 2 == 0:
            return row + 1, col * 2 + 2
        else:
            return row + 1, col * 2 + 1

    def change_from_loc(self, x, y):
        if x % 2 == 0 and y % 2 == 0:
            return 128
        if (self.should_reverse ^ 1 == 0):
            return (x - 2) * 8 + (y // 2 - 1)
        else:
            if x % 2 == 0:
                return (16 - x) * 8 + (y // 2 - 1)
            else:
                return (18 - x) * 8 + (y // 2 - 1)

    def action(self, kind):
        if kind < 128:
            x, y = self.wall_pos(kind)
        else:
            dis, path = self.findPath(0)
            x, y = path
        result = self.step(x, y, 0)
        opp_state = self.state(1)
        if result != 2:
            return copy.deepcopy(self.state(0)), copy.deepcopy(result), copy.deepcopy(opp_state), copy.deepcopy(-1)
        result = self.step_ai()
        xx, yy = map(int, self.ins.split())
        k = self.change_from_loc(xx, yy) if result == 2 else -1
        return copy.deepcopy(self.state(0)), copy.deepcopy(result), copy.deepcopy(opp_state), copy.deepcopy(k)
