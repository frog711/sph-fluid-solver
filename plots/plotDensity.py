import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

f = open("../tubeDensity7/density.txt", "r")
lines = f.readlines()
f.close()
x1 = []
y1 = []
for line in lines[1000:]:
    x1 += [int(line.split(",")[0])]
    y1 += [float(line.split(",")[1])]
f.close()

f = open("../tubeDensity5/density.txt", "r")
lines = f.readlines()
f.close()
x2 = []
y2 = []
for line in lines[1000:]:
    x2 += [int(line.split(",")[0])]
    y2 += [float(line.split(",")[1])]
f.close()


#avg = np.array(y).mean()
#print(avg)
ye = [1.0 for x in range(len(x1))]

plt.plot(x1, y1)
plt.plot(x2, y2)
plt.plot(x1, ye)

plt.show()