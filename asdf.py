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

asdf._loc[0] += 1
asdf._loc[1] += 1
asdf.update(asdf._loc)
