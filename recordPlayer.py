import pygame
pygame.init()

# Define some colors
BLACK = (0,   0,   0)
GRAY = (200, 200, 200)
WHITE = (255, 255, 255)
GREEN = (0, 255,   0)
RED = (255,   0,   0)
BLUE = (0,   0, 255)

screen_h = 900
screen_w = 900
edge = 50
screen = pygame.display.set_mode((screen_w, screen_h))
pygame.display.set_caption("Quoridor Player v2 by littleRound")

block_h = (screen_h - 2 * edge) // 9
block_w = (screen_w - 2 * edge) // 9

# Loop until the user clicks the close button.
done = False

# Used to manage how fast the screen updates
clock = pygame.time.Clock()


def DrawBoard():
    # Draw on the screen a green line from (0, 0) to (100, 100)
    # that is 5 pixels wide.
    def BoardLine(y1, x1, y2, x2):
        pygame.draw.line(screen, GRAY, [x1, y1], [x2, y2], 2)

    for i in range(edge, edge + 9 * block_h, block_h):
        for j in range(edge, edge + 9 * block_w, block_w):
            BoardLine(i, j, i + block_h, j)
            BoardLine(i, j, i, j + block_w)

    for i in range(edge, edge + 9 * block_h, block_h):
        BoardLine(i, edge + 9 * block_w, i + block_h, edge + 9 * block_w)

    for j in range(edge, edge + 9 * block_w, block_w):
        BoardLine(edge + 9 * block_h, j, edge + 9 * block_h, j + block_w)


def DrawWall(wall):
    joint = 10

    def WallLine(y1, x1, y2, x2):
        pygame.draw.line(screen, BLACK, [x1, y1], [x2, y2], joint * 2 - 4)
    assert 0 <= wall[0] < 16
    assert 0 <= wall[1] < 8
    if wall[0] % 2 == 0:
        # it is a -
        x = edge + wall[0] // 2 * block_h
        y = edge + wall[1] * block_w + block_w
        WallLine(x + joint, y, x + block_h * 2 - joint, y)
    else:
        # it is a |
        x = edge + (wall[0] + 1) // 2 * block_h
        y = edge + wall[1] * block_w
        WallLine(x, y + joint, x, y + block_w * 2 - joint)


def DrawPlayer(location, color):
    joint = 30

    def Square(y, x, color):
        pygame.draw.rect(screen, color, [
                         x + joint + 2, y + joint + 2, block_w - 2 * joint, block_h - 2 * joint])
    Square(edge + location[0] * block_h, edge + location[1] * block_w, color)


walls = []
redLocation = (0, 4)
blueLocation = (8, 4)
init = (walls.copy(), redLocation, blueLocation, "Red goes First")

history = [init]
nwState = 0

# construct the history from 'record.txt'
record = open('record.txt', 'r')
for line in record.readlines():
    tokens = line.split()
    # assert len(tokens) == 3
    if tokens[0] == 'wall':
        walls.append((int(tokens[1]), int(tokens[2])))
    elif tokens[0] == 'red':
        redLocation = (int(tokens[1]), int(tokens[2]))
    elif tokens[0] == 'blue':
        blueLocation = (int(tokens[1]), int(tokens[2]))
    history.append((walls.copy(), redLocation, blueLocation, line[:-1]))
record.close()


def LastMove():
    global nwState
    if nwState <= 0:
        return
    nwState -= 1


def NextMove():
    global nwState
    if nwState + 1 >= len(history):
        return
    nwState += 1


# Bold: True, Italics: False
font = pygame.font.SysFont('Calibri', 15, True, False)

text = "Good morning"
import sys
# -------- Main Program Loop -----------
playing = -1
speed = 32
while not done:
    # --- Main event loop
    for event in pygame.event.get():  # User did something
        if event.type == pygame.QUIT:  # If user clicked close
            done = True  # Flag that we are done so we exit this loop
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LEFT:
                LastMove()
            elif event.key == pygame.K_RIGHT:
                NextMove()
            elif event.key == pygame.K_SPACE:
                if playing == -1:
                    playing = speed
                else:
                    playing = -1
            elif event.key == pygame.K_UP:
                speed //= 2
            elif event.key == pygame.K_DOWN:
                speed *= 2
    screen.fill(WHITE)
    # --- Game logic should go here
    walls, redLocation, blueLocation, text = history[nwState]
    if playing > 0:
        playing -= 1
    elif playing == 0:
        NextMove()
        playing = speed
    # --- Drawing code should go here
    DrawBoard()
    for wall in walls:
        DrawWall(wall)
    DrawPlayer(redLocation, RED)
    DrawPlayer(blueLocation, BLUE)
    text_f = font.render(text, True, BLACK)
    screen.blit(text_f, [10, 10])
    text_f2 = font.render(
        'ARROW_KEY <- and -> to view step by step, up down to change speed, KEY_SPACE to start and pause auto-play', True, BLACK)
    screen.blit(text_f2, [70, screen_h - edge + 10])
    # --- Go ahead and update the screen with what we've drawn.
    pygame.display.flip()

    # --- Limit to 60 frames per second
    clock.tick(60)
