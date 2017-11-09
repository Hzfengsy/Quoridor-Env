import random
import json
import copy
import networkx as nx
from board import Board

class Quoridor(object):

    def __init__(self, pros):
        self.pros = pros
        self.err = ['', '']
        self.size = 19

    def step_ai(self):
        dis, path = self.findPath(1)
        x, y = path
        return self.step(x, y, 1)

    def reset(self):
        with open('record.txt', 'w') as f:
            f.write("Init\n")
        self.record_json = {}
        self.running = True
        self.steps = 0
        self.nw = Board(self.record_json)
        self.json_out = open('result' + str(self.pros) + '.json', 'w')
        self.should_reverse = turn = random.randint(0, 1)
        # self.should_reverse = turn = 1
        self.record_json['id'] = [turn, 1 - turn]
        self.record_json['user'] = ["training", "short_path"]
        self.record_json['step'] = []
        ins = ""
        if turn == 1:
            result = self.step_ai()
        else:
            result = self.nw.result()
        return copy.deepcopy(self.state(0)), copy.deepcopy(result)

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
            res = self.nw.update([x + 1, y + 1])
        else:
            res = self.nw.update([19 - x, y + 1])
        self.steps += 1
        if res != True:
            self.err[ai] = res
            self.finish(ai ^ 1)
            return ai ^ 1
        if self.nw.result() < 2:
            ans = 1 - self.nw.result() if self.should_reverse else self.nw.result()
            self.finish(ans)
            return ans
        else:
            ans = 2
        return ans

    def finish(self, winner):
        self.record_json['total'] = self.steps
        self.record_json['result'] = winner
        self.record_json['err'] = [self.err, ' ']
        print(self.record_json)

        json_out = open('result' + str(self.pros) + '.json' , 'w')
        json.dump(self.record_json, json_out)
        json_out.close()
        self.running = False

    def build_graph(self, ai):
        state = copy.deepcopy(self.state(ai))
        u = ((0, 1), (0, -1), (1, 0), (-1, 0))
        G = nx.DiGraph()
        for i in range(1, self.size, 2):
            for j in range(1, self.size, 2):
                if state[i][j] == 2:
                    pos = (i, j)
                G.add_node((i, j))
        for i in range(1, self.size, 2):
            for j in range(1, self.size, 2):
                for k in range(len(u)):
                    if (state[i + u[k][0]][j + u[k][1]]):
                        continue
                    x = i + u[k][0] * 2
                    y = j + u[k][1] * 2
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
        return G, pos

    def findPath(self, ai):
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
        return copy.deepcopy(min_dis), copy.deepcopy(min_path)

    def wall_pos(self, kind):
        row = kind // 8
        col = kind % 8
        if row % 2 == 0:
            return row + 2, col * 2 + 3
        else:
            return row + 2, col * 2 + 2

    def action(self, kind):
        if kind < 128:
            x, y = self.wall_pos(kind)
        else:
            dis, path = self.findPath(0)
            x, y = path
        result = self.step(x, y, 0)
        if result != 2:
            return copy.deepcopy(self.state(0)), copy.deepcopy(result)
        result = self.step_ai()
        return copy.deepcopy(self.state(0)), copy.deepcopy(result)