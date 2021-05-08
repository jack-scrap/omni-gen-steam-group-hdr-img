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
	    ('_ptr', POINTER(_Data))
    ]

_dataGet = _scn.dataGet
_dataGet.restype = POINTER(POINTER(_Var))
_dataGet.argtypes = None

_noDataGet = _scn.noDataGet
_noDataGet.restype = c_uint
_noDataGet.argtypes = None

_data = _dataGet()
_noData = _noDataGet()

data = {}
for i in range(_noData):
	id = ''

	c = 0
	while _data[i].contents._id[c] != b'\0':
		id += _data[i].contents._id[c].decode("utf-8")

		c += 1

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

_boundAreaGet = _scn.boundAreaGet
_boundAreaGet.restype = POINTER(POINTER(_Cone))
_boundAreaGet.argtypes = None

bound = {
	'rng': _boundRngGet(),
	'area': _boundAreaGet()
}

# vehicle
class _Obj(Structure):
	def __init__(self, ptr):
		self._ptr = ptr

class _Crane(_Obj):
	_fields_ = [
		('_op', POINTER(_Cont)),
		('_loc', c_float * 3),
		('_rngHead', c_float * 2),
		('_rngClaw', c_float * 2),
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

_craneGet = _scn.craneGet
_craneGet.restype = POINTER(POINTER(_Crane))
_craneGet.argtypes = None

_noCraneGet = _scn.noCraneGet
_noCraneGet.restype = c_uint
_noCraneGet.argtypes = None

crane = None
if _noCraneGet():
    if _noCraneGet() > 1:
	    crane = []

	    for i in range(_noCraneGet()):
		    crane.append(_Crane(_craneGet()[i]))

    else:
	    crane = _Crane(_craneGet()[0])

_truckGet = _scn.truckGet
_truckGet.restype = POINTER(POINTER(_Truck))
_truckGet.argtypes = None

_noTruckGet = _scn.noTruckGet
_noTruckGet.restype = c_uint
_noTruckGet.argtypes = None

truck = None
if _noTruckGet():
    if _noTruckGet() > 1:
	    truck = []

	    for i in range(_noTruckGet()):
		    truck.append(_Truck(_truckGet()[i]))
    else:
	    truck = _Truck(_truckGet()[0])

_cargoShipGet = _scn.cargoShipGet
_cargoShipGet.restype = POINTER(POINTER(_CargoShip))
_cargoShipGet.argtypes = None

_noCargoShipGet = _scn.noCargoShipGet
_noCargoShipGet.restype = c_uint
_noCargoShipGet.argtypes = None

cargoShip = None
if _noCargoShipGet():
    if _noCargoShipGet() > 1:
	    cargoShip = []

	    for i in range(_noCargoShipGet()):
		    cargoShip.append(_CargoShip(_cargoShipGet()[i]))
	
    else:
	    cargoShip = _CargoShip(_cargoShipGet()[0])
