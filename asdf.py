import sys

sys.path.insert(1, '.')

import omni

asdf = omni.Asdf(omni.lib.asdf)

asdf.update([
    asdf._loc[0] + 0.1,
    asdf._loc[1] + 0.1
])
