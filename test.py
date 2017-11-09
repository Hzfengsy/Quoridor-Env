from Quoridor import Quoridor
import random
x = Quoridor(0)

for i in range(10000):
	_, res = x.reset()
	while res == 2:
		_, res = x.action(88)
