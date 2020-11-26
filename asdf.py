from ctypes import *

class Asdf(Structure):
    _fields_ = [
        ("_asdf", c_int)
    ]

    def update(self, asdf):
        self._asdf = asdf

        set(self._ptr, self._asdf)

    def __init__(self, ptr):
        self._ptr = cast(ptr, POINTER(Asdf))

        self.update(self._ptr.contents._asdf)

lib = CDLL('libhjkl.so')

set = lib.set
set.restype = c_void_p
set.argtypes = [
    POINTER(Asdf),
    c_int
]

asdf = Asdf(lib.asdf)

asdf._asdf += 1
asdf.update(asdf._asdf)
