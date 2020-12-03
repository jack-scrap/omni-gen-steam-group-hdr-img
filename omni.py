from ctypes import *

lib = CDLL('libscn.so')

class Obj(Structure):
    _fields_ = [
        ("_loc", c_float * 3)
    ]

    def mv(self, loc):
        for i in range(3):
            self._loc[i] = loc[i]

        set(self._ptr, self._loc)

    def __init__(self, ptr):
        self._ptr = cast(ptr, POINTER(Obj))

        self.mv(self._ptr.contents._loc)

set = lib.set
set.restype = c_void_p
set.argtypes = [
    POINTER(Obj),
    c_float * 3
]
