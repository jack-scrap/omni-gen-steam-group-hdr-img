from ctypes import *

obj = CDLL('libobj.so')
truck = CDLL('libtruck.so')
crane = CDLL('libcrane.so')
scn = CDLL('libscn.so')

# data
class Attr(Structure):
        _fields_ = [
                ("_ptr", POINTER(c_char)),
                ("_x", c_int),
                ("_y", c_int),
                ("_loc", c_float * 3)
        ]

attrGet = scn.attrGet
attrGet.restype = POINTER(Attr)
attrGet.argtypes = None

data = attrGet()

rhsGet = scn.rhsGet
rhsGet.restype = POINTER(c_char)
rhsGet.argtypes = None

rhs = rhsGet()

# bound
class End(Structure):
    _fields_ = [
        ("_axis", c_uint),
        ("_val", c_float)
    ]

boundRngGet = scn.boundRngGet
boundRngGet.restype = POINTER(End)
boundRngGet.argtypes = None

boundRng = boundRngGet()

# vehicle
class Obj(Structure):
	_fields_ = [
		("_loc", c_float * 3),
                ("_rot", c_float * 3)
	]

	def __init__(self, ptr):
		self._ptr = ptr
		self._loc = self._ptr.contents._loc

class Truck(Obj):
	def turn(self, delta):
            self._rot[1] += delta

            truckTurn(self._ptr, self._rot[1])

	def zoom(self, delta):
            self._loc[0] += delta

            truckZoom(self._ptr, delta)

truckTurn = truck.truckTurn
truckTurn.restype = c_void_p
truckTurn.argtypes = [
        POINTER(Truck),
        c_float
]

truckZoom = truck.truckZoom
truckZoom.restype = c_void_p
truckZoom.argtypes = [
	POINTER(Truck),
	c_float
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
vehicleGet.restype = POINTER(POINTER(Truck))
vehicleGet.argtypes = None

vehicle = vehicleGet()

truck = Truck(vehicle[0])
