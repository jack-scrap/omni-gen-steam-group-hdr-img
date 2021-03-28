# Move the container into the index

from omni import *

i = 0
while i < 10:
    crane.ped(-1.0)

    i += 1

i = 0
while i < 6:
    crane.zoom(1)

    i += 1

crane.grab()

i = 0
while i < 6:
    crane.zoom(-1)

    i += 1

crane.grab()

i = 0
while i < 3:
    crane.zoom(-1)

    i += 1
