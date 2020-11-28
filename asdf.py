import sys

sys.path.insert(1, '.')

import omni

tri = omni.Poly(omni.lib.tri)

for _ in range(3):
    tri.mv([
        0.5, 0.5
    ])
