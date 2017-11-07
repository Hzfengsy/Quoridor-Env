import random
import copy
import judge
import networkx as nx
total_exec = 4
side = 0
size = 19

def reset(pros):
	global state
	ai = random.randint(4, total_exec)
	with open('record.txt', 'w') as f:
		f.write("Init\n")
	state, result, side = judge.init_judge(ai, pros)
	# print(state)
	return copy.deepcopy(state), copy.deepcopy(result)

def build():
	global state
	u = ((0, 1), (0, -1), (1, 0), (-1, 0))
	G = nx.DiGraph()
	for i in range(1, size, 2):
		for j in range(1, size, 2):
			if state[i][j] == 2:
				pos = (i, j)
			G.add_node((i, j))
	for i in range(1, size, 2):
		for j in range(1, size, 2):
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

def findPath():
	G, pos = build()
	path = nx.shortest_path(G, source = pos) 
	dis = nx.shortest_path_length(G, source = pos)
	min_dis = 1000
	min_path = ()
	for i in range(1, size, 2):
		now_dis = dis.get((17, i), 1000)
		if now_dis < min_dis:
			min_dis = now_dis
			min_path = path[(17, i)][1]
	return min_path

def step(kind, x, y):
	global state
	if (kind == 0):
		x, y = findPath()
	state, result = judge.step(x, y)
	return copy.deepcopy(state), copy.deepcopy(result)