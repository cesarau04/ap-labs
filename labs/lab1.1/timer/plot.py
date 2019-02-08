import matplotlib.pyplot as plt
import numpy as np
x = np.array([1000, 5000, 10000, 50000, 100000])
y = np.array([1.73133364, 8.64801116, 18.5574565, 87.6586459, 211.40090542])
w = np.array([1.9258653, 9.71302725, 19.5127394, 97.35014266, 217.16078747])

plt.plot(x, y, label='mycat')
plt.plot(x, w, label='cat')

plt.xlabel('Repetitions')
plt.ylabel('Time (s)')

plt.title("mycat vs cat time comparation")

plt.legend()

plt.grid(b=True, which='both', axis='both', linestyle='-', linewidth=2)
plt.show()

