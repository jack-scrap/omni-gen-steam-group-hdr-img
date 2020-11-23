from ctypes import *

class Asdf(Structure):
    _fields_ = [
        ("_asdf", c_int)
    ]

    def update(self, asdf):
        self._asdf = asdf

        set(self._ptr, self._asdf)

    def __init__(self, ptr, asdf):
        self._ptr = cast(ptr, POINTER(Asdf))

        self._asdf = asdf

        self.update(self._asdf)

lib = CDLL('libhjkl.so')

set = lib.set
set.restype = c_void_p
set.argtypes = [
    POINTER(Asdf),
    c_int
]

asdf = Asdf(lib.asdf, 3)

asdf.update(7)

print(asdf._asdf)
