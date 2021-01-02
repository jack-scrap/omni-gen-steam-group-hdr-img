from ctypes import *

obj = CDLL('libobj.so')
truck = CDLL('libtruck.so')
crane = CDLL('libcrane.so')
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

class Truck(Obj):
    def mv(self, loc):
        for i in range(3):
            self._loc[i] = loc[i]

        truckMv(self._ptr, self._loc)

class Crane(Obj):
    def mv(self, loc):
        for i in range(3):
            self._loc[i] = loc[i]

        objMv(self._ptr.contents._parent, self._loc)

    def pan(self, dir):
        cranePan(self._ptr, dir)

    def ped(self, dir):
        cranePed(self._ptr, dir)

    def grab(self):
        craneGrab(self._ptr)

objMv = obj.objMv
objMv.restype = c_void_p
objMv.argtypes = [
    POINTER(Obj),
    c_float * 3
]

truckGet = truck.truckGet
truckGet.restype = POINTER(Truck)
truckGet.argtypes = None

truckSet = truck.truckSet
truckSet.restype = c_void_p
truckSet.argtypes = [
    POINTER(Truck),
    c_float * 3
]

truckMv = truck.truckMv
truckMv.restype = c_void_p
truckMv.argtypes = [
    POINTER(Truck),
    c_float * 3
]

craneGet = crane.craneGet
craneGet.restype = POINTER(Crane)
craneGet.argtypes = None

craneSet = crane.craneSet
craneSet.restype = c_void_p
craneSet.argtypes = [
    POINTER(Crane),
    c_float * 3
]

cranePan = crane.cranePan
cranePan.restype = c_void_p
cranePan.argtypes = [
    POINTER(Crane),
    c_bool
]

cranePed = crane.cranePed
cranePed.restype = c_void_p
cranePed.argtypes = [
    POINTER(Crane),
    c_bool
]

craneGrab = crane.craneGrab
craneGrab.restype = c_void_p
craneGrab.argtypes = [
    POINTER(Crane)
]

truck = Truck(truckGet())
crane = Crane(craneGet())
