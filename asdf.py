from ctypes import *

class Asdf(Structure):
    _fields_ = [
        ("_loc", c_float * 2)
    ]

    def update(self, loc):
        self._loc = loc

        set(self._ptr, self._loc)

    def __init__(self, ptr):
        self._ptr = cast(ptr, POINTER(Asdf))

        self.update(self._ptr.contents._loc)

lib = CDLL('libhjkl.so')

set = lib.set
set.restype = c_void_p
set.argtypes = [
    POINTER(Asdf),
    c_float * 2
]

asdf = Asdf(lib.asdf)

for i in range(0, 2):
    asdf._loc[i] += 1
asdf.update(asdf._loc)
