import numpy as np

# whether allow jumping near edges
allow_jne = True


class Board(object):
    def __init__(self, json_dict):
        self.board = np.zeros((20, 20), dtype=bool)
        self.loc = []
        self.loc.append([2, 10])
        self.loc.append([18, 10])
        self.side = 0
        self.near = [[0, 2], [0, -2], [2, 0], [-2, 0]]
        self.walls = []
        self.walls.append(10)
        self.walls.append(10)
        self.recordColor = 0
        self.record_json = json_dict
        if allow_jne:
            for i in range(1, 20):
                self.board[1][i] = True
                self.board[i][1] = True
                self.board[19][i] = True
                self.board[i][19] = True

    def checkConnect(self):
        cnt = 0
        for s in self.loc:
            vis = np.zeros((20, 20), dtype=bool)

            def fill(x, y):
                # print('fill ', x, y)
                vis[x][y] = True
                for xx, yy in [(0, 1), (0, -1), (1, 0), (-1, 0)]:
                    nw_pos = (x + 2 * xx, y + 2 * yy)
                    if 2 <= nw_pos[0] <= 18 and 2 <= nw_pos[1] <= 18:
                        if not self.board[x + xx][y + yy] and not vis[nw_pos[0]][nw_pos[1]]:
                            fill(nw_pos[0], nw_pos[1])
            fill(s[0], s[1])
            # print(vis)
            if not (True in [vis[18][i] for i in range(2, 20, 2)]):
                return False
            if not (True in [vis[2][i] for i in range(2, 20, 2)]):
                return False
            cnt += 1
        return True

    def positionType(self, loc):
        if 2 <= loc[0] <= 18 and 2 <= loc[1] <= 18 and loc[0] % 2 == 0 and loc[1] % 2 == 0:
            return 0
        if 2 <= loc[0] <= 17:
            if loc[0] % 2 == 0:
                if 3 <= loc[1] <= 17 and loc[1] % 2 == 1:
                    return 1
            else:
                if 2 <= loc[1] <= 16 and loc[1] % 2 == 0:
                    return 2
        return 3

    def record(self, loc):
        file = open("record.txt", "a")
        type = self.positionType(loc)
        output = {}
        if type == 0:
            if self.recordColor == 0:
                file.write("red")
                output['kind'] = 'red'
            else:
                file.write("blue")
                output['kind'] = 'blue'
            file.write(" " + str(loc[0] // 2 - 1) + " " + str(loc[1]
                                                              // 2 - 1) + "\n")
            output['x'] = loc[0] // 2 - 1
            output['y'] = loc[1] // 2 - 1
        elif type in (1, 2):
            file.write("wall " + str(loc[0] - 2) +
                       " " + str(loc[1] // 2 - 1) + "\n")
            output['kind'] = 'wall'
            output['x'] = loc[0] - 2
            output['y'] = loc[1] // 2 - 1
        self.record_json["step"].append(output)
        file.close()
        self.recordColor = 1 - self.recordColor

    def update(self, loc):
        case = self.positionType(loc)
        otherside = 1 - self.side
        nw_loc = self.loc[self.side]
        if case == 0:
            v = [loc[0] - nw_loc[0], loc[1] - nw_loc[1]]
            if v in self.near:
                v_d2 = (v[0] // 2, v[1] // 2)
                wall = (nw_loc[0] + v_d2[0], nw_loc[1] + v_d2[1])
                if loc != self.loc[otherside] and not self.board[wall[0]][wall[1]]:
                    self.loc[self.side] = loc
                    self.side = 1 - self.side
                    self.record(loc)
                    return True
            else:
                for d in [(1, 0), (0, 1), (-1, 0), (0, -1)]:
                    if self.board[nw_loc[0] + d[0]][nw_loc[1] + d[1]]:
                        continue
                    if [nw_loc[0] + 2 * d[0], nw_loc[1] + 2 * d[1]] != self.loc[otherside]:
                        continue
                    if self.board[nw_loc[0] + 3 * d[0]][nw_loc[1] + 3 * d[1]]:
                        for dt in [(d[1], d[0]), (-d[1], d[0]), (d[1], -d[0]), (-d[1], -d[0])]:
                            if nw_loc[0] + 2 * d[0] + 2 * dt[0] != loc[0] or\
                                    nw_loc[1] + 2 * d[1] + 2 * dt[1] != loc[1]:
                                continue
                            if not self.board[nw_loc[0] + 2 * d[0] + dt[0]][nw_loc[1] + 2 * d[1] + dt[1]]:
                                self.loc[self.side] = loc
                                self.side = 1 - self.side
                                self.record(loc)
                                return True
                    else:
                        if nw_loc[0] + 4 * d[0] == loc[0] and nw_loc[1] + 4 * d[1] == loc[1]:
                            self.loc[self.side] = loc
                            self.side = 1 - self.side
                            self.record(loc)
                            return True
            return "Wrong Place to move your chess"
        elif case == 1:
            if self.walls[self.side] <= 0:
                return "Wall not enough..."
            if self.board[loc[0]][loc[1]] or\
                    self.board[loc[0] + 1][loc[1]] or\
                    self.board[loc[0] + 2][loc[1]]:
                return "A wall already here..."
            self.board[loc[0]][loc[1]] = True
            self.board[loc[0] + 1][loc[1]] = True
            self.board[loc[0] + 2][loc[1]] = True
            if not self.checkConnect():
                return "You block someone's way..."
            self.walls[self.side] -= 1
            self.side = 1 - self.side
            self.record(loc)
            return True
        elif case == 2:
            if self.walls[self.side] <= 0:
                return "Wall not enough..."
            if self.board[loc[0]][loc[1]] or\
                    self.board[loc[0]][loc[1] + 1] or\
                    self.board[loc[0]][loc[1] + 2]:
                return "A wall already here..."
            self.board[loc[0]][loc[1]] = True
            self.board[loc[0]][loc[1] + 1] = True
            self.board[loc[0]][loc[1] + 2] = True
            if not self.checkConnect():
                return "You block someone's way..."
            self.walls[self.side] -= 1
            self.side = 1 - self.side
            self.record(loc)
            return True
        else:
            return "Invalid x&y"

    def result(self):
        if self.loc[0][0] == 18:
            return 0
        elif self.loc[1][0] == 2:
            return 1
        else:
            return 2

'''

# Test
ddd = {}
ddd['step'] = []
b = Board(ddd)
np.set_printoptions(threshold=np.nan)
while (True):
    print(b.loc, b.side)
    print(b.board.astype(int))
    temp = input()
    temp = temp.split()
    temp = list(map(int, temp))
    rec = b.update(temp)
    print('rec: ', rec)

'''
