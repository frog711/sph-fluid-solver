from random import randint
import math
phi = 30
h = 0.5
n = 150 * 8 + 2 * 128 + 2500
dim = 2
k = 150000
m = 1 * h * h
nu = 0.1
kernelSupport = 2
step = 0.00001
acc = [0, 9.81]

def box():
    f = open("boundary", "w+")
    f.write(f"{n} 2 {h} {k} {kernelSupport} 0 {step} {acc[0]} {acc[1]} 600 600\n")
    f.write("0,3.5,300,300,0,0\n")
    for x in range(1, 49):
        x1 = 50 + 10 * x
        f.write(f"1,{m},{x1},{40},0,0\n")
        f.write(f"1,{m},{x1},{50},0,0\n")
        f.write(f"1,{m},{x1},{540},0,0\n")
        f.write(f"1,{m},{x1},{550},0,0\n")
        f.write(f"1,{m},{40},{x1},0,0\n")
        f.write(f"1,{m},{50},{x1},0,0\n")
        f.write(f"1,{m},{540},{x1},0,0\n")
        f.write(f"1,{m},{550},{x1},0,0\n")
    f.close()

def plane():
    f = open("plane", "w+")
    f.write(f"{21} {dim} {h} {k} {nu} {kernelSupport} {1} {step} {acc[0]} {acc[1]} {15 * h} {15 * h}\n")
    f.write(f"0,{m},{5 * h},0,0,0\n")
    for x in range(0, 10):
        x1 = h * x
        f.write(f"1,{m},{x1},{10 * h},0,0\n")
        f.write(f"1,{m},{x1},{11 * h},0,0\n")
    f.close()

def grid():
    dx = 200
    dy = 200
    f = open("angled", "w+")
    f.write(f"{100} 2 {20} {k} {0} {1.9} 0 {step} {acc[0]} {acc[1]} 600 600\n")
    for x in range(0,10):
        for y in range(0, 10):
            x1 = math.cos(phi) * 20 * x + math.sin(phi) * 20 * y + dx
            y1 = math.cos(phi) * 20 * y - math.sin(phi) * 20 * x + dy
            f.write(f"0,3.5,{x1},{y1},0,0\n")
    f.close()

def breakingDam():
    f = open("dam", "w+")
    f.write(f"{n} {dim} {h} {k} {nu} {kernelSupport} 0 {step} {acc[0]} {acc[1]} 110 110\n")
    diff = 5
    for x in range(50):
        for y in range(100, 150):
            f.write(f"0, {m},{diff + h * (x + 2)},{diff + h * y},0,0\n")

    for x in range(0, 150):
        f.write(f"1,{m},{0 + diff},{h * x + diff},0,0\n")
        f.write(f"1,{m},{h + diff},{h * x + diff},0,0\n")

        f.write(f"1,{m},{h * x + diff},{h * 150 + diff},0,0\n")
        f.write(f"1,{m},{h * x + diff},{h * 151 + diff},0,0\n")

        f.write(f"1,{m},{h * 150 + diff},{h * (x + 2) + diff},0,0\n")
        f.write(f"1,{m},{h * 151 + diff},{h * (x + 2) + diff},0,0\n")

        f.write(f"1,{m},{h * (x + 2) + diff},{0 + diff},0,0\n")
        f.write(f"1,{m},{h * (x + 2) + diff},{h + diff},0,0\n")
    for x in range(2, 130):
        f.write(f"1,{m},{h * 52 + diff},{h * x + diff},0,0\n")
        f.write(f"1,{m},{h * 53 + diff},{h * x + diff},0,0\n")
    f.close()

def box(n, h):
    f = open("box", "w+")
    f.write(f"{16*(n+1) + n * n} {dim} {h} {k} {nu} {kernelSupport} 0 {step} {acc[0]} {acc[1]} {(2 * n + 10) * h} {(2 * n + 10) * h}\n")
    for i in range(0, n):
        for j in range(0, n):
            f.write(f"0,{m},{(i + 5) * h},{(j + 5 + n) * h},0,0\n")
    for i in range(0, 2 * n + 2):
        f.write(f"1,{m},{(i + 3) * h},{3 * h},0,0\n")
        f.write(f"1,{m},{(i + 3) * h},{4 * h},0,0\n")

        f.write(f"1,{m},{(5 + 2 * n) * h},{(i + 3) * h},0,0\n")
        f.write(f"1,{m},{(6 + 2 * n) * h},{(i + 3) * h},0,0\n")

        f.write(f"1,{m},{(i + 5) * h},{(2 * n + 5) * h},0,0\n")
        f.write(f"1,{m},{(i + 5) * h},{(2 * n + 6) * h},0,0\n")

        f.write(f"1,{m},{3 * h},{(i + 5) * h},0,0\n")
        f.write(f"1,{m},{4 * h},{(i + 5) * h},0,0\n")

def dam(nx, ny, h, vy):
    f = open("tube", "w+")
    f.write(f"{4*(1.5 * ny + 4) + 4 * nx + nx * ny} {dim} {h} {k} {nu} {kernelSupport} 0 {step} {acc[0]} {acc[1]} {(2 * ny + 10) * h} {(2 * ny + 10) * h}\n")
    for x in range(0, nx):
        for y in range(0, ny):
            f.write(f"0,{m},{(x + 5) * h},{(y + 5 + 0.5 * ny - vy) * h},0,{0}\n")
    for i in range(0, int(1.5 * ny) + 4):
        f.write(f"1,{m},{(5 + nx) * h},{(i + 3) * h},0,0\n")
        f.write(f"1,{m},{(6 + nx) * h},{(i + 3) * h},0,0\n")

        f.write(f"1,{m},{3 * h},{(i + 3) * h},0,0\n")
        f.write(f"1,{m},{4 * h},{(i + 3) * h},0,0\n")
    for i in range(0, nx):
        #continue
        f.write(f"1,{m},{(i + 5) * h},{3 * h},0,0\n")
        f.write(f"1,{m},{(i + 5) * h},{4 * h},0,0\n")

        f.write(f"1,{m},{(i + 5) * h},{(1.5 * ny + 5) * h},0,0\n")
        f.write(f"1,{m},{(i + 5) * h},{(1.5 * ny + 6) * h},0,0\n")
        

box(30, 0.5)
#dam(10,30,0.5,1)
#grid()