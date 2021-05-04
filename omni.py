from ctypes import *

_obj = CDLL('libobj.so')
_truck = CDLL('libtruck.so')
_crane = CDLL('libcrane.so')
_cargo_ship = CDLL('libcargo_ship.so')
_scn = CDLL('libscn.so')

# data
class _Cont(Structure):
	_fields_ = [
		('_c', c_char)
	]

class _Data(Structure):
	_fields_ = [
		('_ptr', c_ulong),
		('_depth', c_uint),
		('_x', c_uint),
		('_y', c_uint),
		('_z', c_uint),
		('_loc', c_float * 3),
	]

class _Var(Structure):
    _fields_ = [
	    ('_id', POINTER(c_char)),
	    ('_szId', c_uint),
	    ('_ptr', POINTER(_Data))
    ]

_dataGet = _scn.dataGet
_dataGet.restype = POINTER(POINTER(_Var))
_dataGet.argtypes = None

_szGet = _scn.szGet
_szGet.restype = c_uint
_szGet.argtypes = None

_data = _dataGet()
_sz = _szGet()

data = {}
for i in range(_sz):
	id = ''

	for c in range(_data[i].contents._szId):
		id += _data[i].contents._id[c].decode("utf-8")

	data[id] = _data[i].contents._ptr

_goalGet = _scn.goalGet
_goalGet.restype = POINTER(c_char)
_goalGet.argtypes = None

goal = _goalGet()

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
		('_bound', c_float * 2 * 2),
		('_loc', c_float * 3)
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
	_fields_ = [
		('_op', POINTER(_Cont)),
		('_loc', c_float * 3)
	]

	def zoom(self, delta):
		_craneZoom(self._ptr, delta)

		for i in range(3):
		    self._loc[i] = self._ptr.contents._loc[i]

	def pan(self, delta):
		_cranePan(self._ptr, delta)

		for i in range(3):
		    self._loc[i] = self._ptr.contents._loc[i]

	def ped(self, delta):
		_cranePed(self._ptr, delta)

		for i in range(3):
		    self._loc[i] = self._ptr.contents._loc[i]

	def grab(self):
		_craneGrab(self._ptr)

		self._op = self._ptr.contents._op

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
		('_data', POINTER(_Data)),
		('_loc', c_float * 3),
		('_ang', c_float),
	]

	def turn(self, delta):
		_truckTurn(self._ptr, delta)

		self._ang = self._data.contents._ang

		for i in range(3):
		    self._loc[i] = self._ptr.contents._loc[i]

	def mv(self, delta):
		for i in range(3):
		    self._loc[i] = self._ptr.contents._loc[i]

		_truckMv(self._ptr, delta)

_truckTurn = _truck.truckTurn
_truckTurn.restype = c_void_p
_truckTurn.argtypes = [
		POINTER(_Truck),
		c_float
]

_truckMv = _truck.truckMv
_truckMv.restype = c_void_p
_truckMv.argtypes = [
		POINTER(_Truck),
		c_float
]

class _CargoShip(_Obj):
	_fields_ = [
		('_data', POINTER(_Data)),
		('_loc', c_float * 3)
	]

	def zoom(self, delta):
		_cargoShipMv(self._ptr, delta)

		for i in range(3):
		    self._loc[i] = self._ptr.contents._loc[i]

_cargoShipMv = _cargo_ship.cargoShipMv
_cargoShipMv.restype = c_void_p
_cargoShipMv.argtypes = [
		POINTER(_CargoShip),
		c_float
]

_vehicleGet = _scn.vehicleGet
_vehicleGet.restype = POINTER(POINTER(_CargoShip))
_vehicleGet.argtypes = None

_vehicle = _vehicleGet()

cargoShip = _CargoShip(_vehicle[0])
