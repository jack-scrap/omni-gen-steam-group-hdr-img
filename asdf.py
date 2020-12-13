import sys

sys.path.insert(1, '.')

import omni

wheel = omni.Obj(omni.scn.wheel)

wheel.mv([
    0.5, 0.0, 0.5
])
