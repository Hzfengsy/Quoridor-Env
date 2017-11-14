# Quoridor-Env
Quoridor Environment for reinforcement learning

## API
### reset
```python
def reset(self):
	return copy.deepcopy(self.state(0)), copy.deepcopy(result), copy.deepcopy(opp_state), copy.deepcopy(k)
```
Reset the game and start a new game.  
Return the state, the result, the opposite state before action and the action kind.

### action
```python
def action(self, kind):
	return copy.deepcopy(self.state(0)), copy.deepcopy(result), copy.deepcopy(opp_state), copy.deepcopy(k)
```
Move one step by giving a step code.  
Return the state, the result, the opposite state before action and the action kind.

### findPath
```python
def findPath(self, ai):
	return copy.deepcopy(min_dis), copy.deepcopy(min_path)
```
Get the shortest distance and the shortPath to the destination.  
Return the shortest distance and the next step

## Code rule
### Step kind
if `kind < 128` then place a matched wall, else move as the short path.

### ai_code and result
Your ai_code is always `0`, while the oppsite ai_code is always `1`.  But the your side is random.  
if `result == 2` the game is still in process.  
elseif `result == 0`, you win the game.  
elseif `result == 1`, the oppsite player win. 

### state
No matter whether your side is `0` or `1`, you are always at side of `0` in the state.  
`0`: the blank block.  
`1`: the wall block.  
`2`: your chess.  
`-2`: oppsite chess.
