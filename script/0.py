# Move the container into the other index

import sys

sys.path.insert(1, '.')

from omni import *

i = 0
while i < 3:
    crane.ped(False)

    i += 1

i = 0
while i < 3:
    crane.zoom(True)

    i += 1
