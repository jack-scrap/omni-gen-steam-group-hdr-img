from ctypes import *

obj = CDLL('libobj.so')
scn = CDLL('libscn.so')

class Obj(Structure):
    _fields_ = [
        ("_loc", c_float * 3)
    ]

    def mv(self, loc):
        for i in range(3):
            self._loc[i] = loc[i]

        set(self._loc)

    def __init__(self, ptr):
        self._ptr = ptr
        self._loc = ptr.contents._loc

        self.mv(self._loc)

get = obj.get
get.restype = POINTER(Obj)
get.argtypes = None

set = obj.set
set.restype = c_void_p
set.argtypes = [
    c_float * 3
]

wheel = Obj(get())
