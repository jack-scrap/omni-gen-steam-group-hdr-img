import sys

sys.path.insert(1, '.')

import omni

tri = omni.Obj(omni.lib.obj)

for _ in range(3):
    tri.mv([
        0.5, 0.5
    ])
