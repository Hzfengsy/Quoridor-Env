from Quoridor import Quoridor
import random
x = Quoridor(0)

for i in range(1):
	_, res = x.reset()
	while res == 2:
		_, res = x.action(random.randint(0, 200))
