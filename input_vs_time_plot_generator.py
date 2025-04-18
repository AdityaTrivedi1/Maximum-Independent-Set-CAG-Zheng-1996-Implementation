import matplotlib.pyplot as plt
import numpy as np

input = [100,   200,       500,      1000,     2000,     5000,     10000,  20000,   50000,  100000]
time  = [0,     0.000333,  0.001666, 0.004333, 0.005666, 0.016666, 0.023,  0.041,   0.099,  0.207333]
plt.plot(input, np.log(time), marker='x')
plt.xlabel('Input Length')
plt.ylabel('Time taken (in s)')
plt.title('Input Length vs Time')
plt.show()
