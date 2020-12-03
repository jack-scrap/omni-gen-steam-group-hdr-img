import sys

sys.path.insert(1, '.')

import omni

wheel = omni.Obj(omni.lib.wheel)

for _ in range(3):
    wheel.mv([
        0.5, 0.0, 0.5
    ])
