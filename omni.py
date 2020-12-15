from ctypes import *

obj = CDLL('libobj.so')
truck = CDLL('libtruck.so')
scn = CDLL('libscn.so')

class Obj(Structure):
    _fields_ = [
        ("_loc", c_float * 3)
    ]

    def mv(self, loc):
        for i in range(3):
            self._loc[i] = loc[i]

        objMv(self._ptr, self._loc)

    def __init__(self, ptr):
        self._ptr = ptr
        self._loc = ptr.contents._loc

# class Truck(Structure):
#     _fields_ = [
#         ("_loc", c_float * 3)
#     ]

#     def mv(self, loc):
#         for i in range(3):
#             self._loc[i] = loc[i]

#         truckSet(self._ptr, self._loc)

#     def __init__(self, ptr):
#         self._ptr = ptr
#         self._loc = ptr.contents._loc

objGet = obj.objGet
objGet.restype = POINTER(Obj)
objGet.argtypes = None

objSet = obj.objSet
objSet.restype = c_void_p
objSet.argtypes = [
    POINTER(Obj),
    c_float * 3
]

objMv = obj.objMv
objMv.restype = c_void_p
objMv.argtypes = [
    POINTER(Obj),
    c_float * 3
]

# truckGet = truck.truckGet
# truckGet.restype = POINTER(Truck)
# truckGet.argtypes = None

# truckSet = truck.truckSet
# truckSet.restype = c_void_p
# truckSet.argtypes = [
#         POINTER(Truck),
#         c_float * 3
# ]

wheel = Obj(objGet())
# truck = Truck(truckGet())
