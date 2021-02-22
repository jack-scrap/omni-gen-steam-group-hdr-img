# Move the container into the other index

import sys

sys.path.insert(1, '.')

from omni import *

i = 0
while i < 10:
    crane.ped(False)

    i += 1

crane.grab()

i = 0
while i < 10:
    crane.zoom(True)

    i += 1
