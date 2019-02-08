import timeit
import numpy as np
import matplotlib.pyplot as plt

x = np.array([1000,5000,10000,50000,100000], dtype=np.int32)
y = np.zeros(5, dtype=np.float64)
w = np.zeros(5, dtype=np.float64)
mysetup = "import subprocess"
mycat = "subprocess.run([\"./../mycat.out\",\"../makefile\"], stdout=subprocess.PIPE)"
cat = "subprocess.run([\"cat\",\"../makefile\"], stdout=subprocess.PIPE)"
for index in range(len(x)):
    y[index] = timeit.timeit(setup = mysetup, stmt=mycat, number=x[index]);
    w[index] = timeit.timeit(setup = mysetup, stmt=cat, number=x[index]);
print(x)
print(y)
print(w)
