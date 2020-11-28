from ctypes import *

class Poly(Structure):
    _fields_ = [
        ("_loc", c_float * 2)
    ]

    def update(self, loc):
        for i in range(0, 2):
            self._loc[i] = loc[i]

        set(self._ptr, self._loc)

    def __init__(self, ptr):
        self._ptr = cast(ptr, POINTER(Poly))

        self.update(self._ptr.contents._loc)

lib = CDLL('libscn.so')

set = lib.set
set.restype = c_void_p
set.argtypes = [
    POINTER(Poly),
    c_float * 2
]
