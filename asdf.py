import sys

sys.path.insert(1, '.')

import omni

asdf = omni.Asdf(omni.lib.asdf)

for i in range(0, 2):
    asdf._loc[i] += 1
asdf.update(asdf._loc)
