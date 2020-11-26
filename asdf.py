from ctypes import *

class Asdf(Structure):
    _fields_ = [
        ("_x", c_float),
        ("_y", c_float)
    ]

    def update(self, x, y):
        self._x = x
        self._y = y

        set(self._ptr, self._x, self._y)

    def __init__(self, ptr):
        self._ptr = cast(ptr, POINTER(Asdf))

        self.update(self._ptr.contents._x, self._ptr.contents._y)

lib = CDLL('libhjkl.so')

set = lib.set
set.restype = c_void_p
set.argtypes = [
    POINTER(Asdf),
    c_float,
    c_float,
]

asdf = Asdf(lib.asdf)

asdf._x += 1
asdf._y += 1
asdf.update(asdf._x, asdf._y)
