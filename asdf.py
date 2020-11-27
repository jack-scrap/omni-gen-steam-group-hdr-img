import sys

sys.path.insert(1, '.')

import omni

tri = omni.Poly(omni.lib.tri)

for _ in range(0, 3):
    tri.update([
        tri._loc[0] + 0.1,
        tri._loc[1] + 0.1
    ])
