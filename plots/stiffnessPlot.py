import matplotlib.pyplot as plt

data = {
    0.0: 27400,
    0.5: 62560,
    1.0: 109050,
    1.5: 154410,
    2.0: 199460,
    2.5: 244310,
    3.0: 289090,
    3.5: 333870,
    4.0: 378550,
    4.5: 423240,
    5.0: 467940
}

data2 = {
    0.0: 27410,
    1.0: 109040,
    2.0: 199450,
    3.0: 289080,
    4.0: 378540,
    5.0: 467940
}

# Falling from height h, nu = 0
dataSpeed = {
    0.0: 62550,
    1.0: 67460,
    2.0: 81870,
    3.0: 105410,
    4.0: 137910,
    5.0: 179350,
    6.0: 229800,
    7.0: 289230,
    8.0: 357630,
    9.0: 435110,
    10.0: 521660
}

# timestep 0.0001
dataSpeed2 = {
    0.0: 30280,
    5.0: 178780,
    6.0: 229380,
    
}

fig, (ax1) = plt.subplots(1, 1, layout='constrained')
# log x axis

ax1.grid()
ax1.grid(which="minor", color="0.9")
#ax1.plot(data.keys(), data.values())
ax1.plot(dataSpeed.keys(), dataSpeed.values())
#ax1.plot(range(0, 6), [19327 + 89722 * x for x in range(0,6)] )
ax1.legend()
plt.show()