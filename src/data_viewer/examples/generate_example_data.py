#!/usr/bin/env python3

import numpy as np


values_array = np.linspace(-np.pi, np.pi, 201)

for index, x in np.ndenumerate(values_array):
     print(index[0], x, np.sin(x))

