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

fig, (ax1) = plt.subplots(1, 1, layout='constrained')
# log x axis

ax1.grid()
ax1.grid(which="minor", color="0.9")
ax1.plot(data.keys(), data.values())
ax1.plot(range(0, 6), [19327 + 89722 * x for x in range(0,6)] )
ax1.legend()
plt.show()