from ctypes import *

_obj = CDLL('libobj.so')
_truck = CDLL('libtruck.so')
_crane = CDLL('libcrane.so')
_cargo_ship = CDLL('libcargo_ship.so')
_scn = CDLL('libscn.so')

# data
class _Data(Structure):
    _fields_ = [
        ('_parent', c_ulong),
        ('_ptr', c_ulong),
        ('_depth', c_uint),
        ('_x', c_uint),
        ('_y', c_uint),
        ('_z', c_uint)
    ]

_dataGet = _scn.dataGet
_dataGet.restype = POINTER(_Data)
_dataGet.argtypes = None

data = _dataGet()

_rhsGet = _scn.rhsGet
_rhsGet.restype = POINTER(c_char)
_rhsGet.argtypes = None

rhs = _rhsGet()

# bound
class _Lim(Structure):
    _fields_ = [
        ('_axis', c_uint),
        ('_val', c_float)
    ]

_boundRngGet = _scn.boundRngGet
_boundRngGet.restype = POINTER(_Lim)
_boundRngGet.argtypes = None

class _Cone(Structure):
    _fields_ = [
        ('_parent', c_ulong),
        ('_vtx', c_float * 3),
        ('_idx', c_ushort),
        ('_loc', c_float * 3),
        ('_bound', c_float * 2 * 2)
    ]

_coneRngGet = _scn.coneRngGet
_coneRngGet.restype = POINTER(POINTER(_Cone))
_coneRngGet.argtypes = None

bound = {
    'rng': _boundRngGet(),
    'cone': _coneRngGet()
}

# vehicle
class _Obj(Structure):
	def __init__(self, ptr):
		self._ptr = ptr

class _Crane(_Obj):
	def zoom(self, delta):
		_craneZoom(self._ptr, delta)

	def pan(self, delta):
		_cranePan(self._ptr, delta)

	def ped(self, delta):
		_cranePed(self._ptr, delta)

	def grab(self):
		_craneGrab(self._ptr)

_craneZoom = _crane.craneZoom
_craneZoom.restype = c_void_p
_craneZoom.argtypes = [
	POINTER(_Crane),
	c_float
]

_cranePan = _crane.cranePan
_cranePan.restype = c_void_p
_cranePan.argtypes = [
	POINTER(_Crane),
	c_float
]

_cranePed = _crane.cranePed
_cranePed.restype = c_void_p
_cranePed.argtypes = [
	POINTER(_Crane),
	c_float
]

_craneGrab = _crane.craneGrab
_craneGrab.restype = c_void_p
_craneGrab.argtypes = None

class _Truck(_Obj):
    _fields_ = [
        ('_parent', c_ulong),
        ('_data', c_ulong),
        ('_ang', c_float)
    ]

    def turn(self, delta):
        _truckTurn(self._ptr, delta)

        self._ang = self._data.contents._ang

    def zoom(self, delta):
        _truckZoom(self._ptr, delta)

_truckTurn = _truck.truckTurn
_truckTurn.restype = c_void_p
_truckTurn.argtypes = [
        POINTER(_Truck),
        c_float
]

_truckZoom = _truck.truckZoom
_truckZoom.restype = c_void_p
_truckZoom.argtypes = [
        POINTER(_Truck),
        c_float
]

class _CargoShip(_Obj):
    def zoom(self, delta):
        _cargoShipZoom(self._ptr, delta)

_cargoShipZoom = _cargo_ship.cargoShipZoom
_cargoShipZoom.restype = c_void_p
_cargoShipZoom.argtypes = [
        POINTER(_CargoShip),
        c_float
]

_vehicleGet = _scn.vehicleGet
_vehicleGet.restype = POINTER(POINTER(_Truck))
_vehicleGet.argtypes = None

_vehicle = _vehicleGet()

truck = _Truck(_vehicle[0])
