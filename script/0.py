# Move the container into the other index

import sys

sys.path.insert(1, '.')

from omni import *

i = 0
while i < 3:
    crane.ped(0)

    i += 1

i = 0
while i < 3:
    crane.zoom(1)

    i += 1
