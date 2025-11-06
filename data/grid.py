from random import randint
import math
phi = 0.3
dx = 100
dy = 50

f = open("angled", "w+")
f.write("100 2 500 500\n")
for x in range(0, 10):
    for y in range(0, 10):
        x1 = math.cos(phi) * 20 * x + math.sin(phi) * 20 * y + dx
        y1 = math.cos(phi) * 20 * y - math.sin(phi) * 20 * x + dy
        f.write(f"{20},{x1},{y1},{randint(-50, 50) / 100},{randint(-50, 50) / 100}\n")

f.close()