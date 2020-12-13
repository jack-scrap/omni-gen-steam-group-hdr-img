import sys

sys.path.insert(1, '.')

import omni

wheel = omni.Obj(omni.get())

wheel.mv([
    wheel._loc[0] + 1, 0, 0
])
