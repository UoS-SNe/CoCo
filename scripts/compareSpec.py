import sys
import numpy as np
import matplotlib.pyplot as plt


for i in range(1,len(sys.argv)):
    SPEC = np.loadtxt(sys.argv[i], unpack=True)
    plt.plot(SPEC[0], (SPEC[1] / SPEC[1].mean()), label=str(i))

plt.legend()
plt.show()
