# Move the container into the index

from omni import *

i = 0
while i < 10:
	crane.ped(-1.0)

	i += 1

i = 0
while i < 5:
	crane.zoom(1.0)

	i += 1

crane.grab()

i = 0
while i < 2:
    crane.zoom(-1.0)

    i += 1

crane.grab()
