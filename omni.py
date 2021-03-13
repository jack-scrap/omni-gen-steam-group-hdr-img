from ctypes import *

obj = CDLL('libobj.so')
truck = CDLL('libtruck.so')
crane = CDLL('libcrane.so')
scn = CDLL('libscn.so')

class Obj(Structure):
    _fields_ = [
        ("_loc", c_float * 3)
    ]

    def __init__(self, ptr):
        self._ptr = ptr
        self._loc = self._ptr.contents._loc

class Truck(Obj):
    def zoom(self, loc):
        for i in range(3):
            self._loc[i] = loc[i]

            truckZoom(self._ptr, self._loc)

truckZoom = truck.truckZoom
truckZoom.restype = c_void_p
truckZoom.argtypes = [
    POINTER(Truck),
    c_float * 3
]

class Crane(Obj):
    def zoom(self, delta):
        self._loc[0] += delta

        craneZoom(self._ptr, delta)

    def pan(self, delta):
        cranePan(self._ptr, delta)

    def ped(self, delta):
        cranePed(self._ptr, delta)

    def grab(self):
        craneGrab(self._ptr)

craneZoom = crane.craneZoom
craneZoom.restype = c_void_p
craneZoom.argtypes = [
    POINTER(Crane),
    c_float
]

cranePan = crane.cranePan
cranePan.restype = c_void_p
cranePan.argtypes = [
    POINTER(Crane),
    c_float
]

cranePed = crane.cranePed
cranePed.restype = c_void_p
cranePed.argtypes = [
    POINTER(Crane),
    c_float
]

craneGrab = crane.craneGrab
craneGrab.restype = c_void_p
craneGrab.argtypes = None


vehicleGet = scn.vehicleGet
vehicleGet.restype = POINTER(POINTER(Crane))
vehicleGet.argtypes = None

vehicle = vehicleGet()

crane = Crane(vehicle[0])
