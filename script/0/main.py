# Move the container into the index

from omni import *

i = 0
while i < 10:
    crane.ped(-1.0)
     
    crane.grab()

    i += 1

i = 0
while i < 10:
    crane.zoom(1)

    crane.grab()

    i += 1
