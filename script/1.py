# Move the container into the other index

import sys

sys.path.insert(1, '.')

from omni import *

crane.ped(-1.0)

crane.grab()

crane.zoom(1.0)

crane.zoom(-1.0)

crane.grab()

crane.zoom(-1.0)
