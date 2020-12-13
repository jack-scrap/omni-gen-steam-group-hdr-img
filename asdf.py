import sys

sys.path.insert(1, '.')

import omni

omni.wheel.mv([
    omni.wheel._loc[0] + 1, 0, 0
])
