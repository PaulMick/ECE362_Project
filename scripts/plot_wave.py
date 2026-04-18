import math
import matplotlib.pyplot as plt
import numpy as np

x = list(np.linspace(0, 256, num = 10000))
y = [int(50 * math.sin((int(v) / 256) * 2 * 3.14159265) + 50) for v in x]

plt.plot(x, y)
plt.show()