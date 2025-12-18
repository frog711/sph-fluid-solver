import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

f = open("../tubeDensity4/density.txt", "r")
lines = f.readlines()
print(len(lines))
f.close()
x1 = []
y1 = []
for line in lines[1:]:
    x1 += [float(line.split(",")[0]) * 2e-5]
    y1 += [float(line.split(",")[1])]
f.close()

f = open("../tubeDensity5/density.txt", "r")
lines = f.readlines()
print(len(lines))
f.close()
x2 = []
y2 = []
for line in lines[1:]:
    x2 += [float(line.split(",")[0]) * 2e-5]
    y2 += [float(line.split(",")[1])]
f.close()

f = open("./density.txt", "r")
lines = f.readlines()
print(len(lines))
f.close()
x3 = []
y3 = []
for line in lines[1:]:
    x3 += [float(line.split(",")[0]) * 2e-7]
    y3 += [float(line.split(",")[1])]
f.close()


#avg = np.array(y).mean()
#print(avg)
ye = [1.0 for x in range(len(x1))]

plt.plot(x1, y1)
plt.plot(x2, y2)
plt.plot(x3, y3)
plt.plot(x1, ye)

plt.show()