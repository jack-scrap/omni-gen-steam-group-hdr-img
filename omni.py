from ctypes import *

_truck = CDLL('libtruck.so')
_crane = CDLL('libcrane.so')
_cargo_ship = CDLL('libcargo_ship.so')
_street_light = CDLL('libstreet_light.so')
_scn = CDLL('libscn.so')

class _CArr(Structure):
	_fields_ = [
		('_ptr', c_void_p),
		('_sz', c_size_t)
	]

class _Obj(Structure):
	def __init__(self, ptr):
		self._ptr = ptr

# data
class _Cont(Structure):
	_fields_ = [
		('_c', c_char)
	]

class _Idx(Structure):
	_fields_ = [
		('_data', POINTER(_Cont))
	]

class _Arr(Structure):
	_fields_ = [
		('_data', POINTER(POINTER(_Idx))),
		('_x', c_uint),
		('_y', c_uint),
		('_loc', c_float * 3)
	]

class _Var(Structure):
	_fields_ = [
		('_id', c_char_p),
		('_ptr', c_void_p)
	]

_dataGet = _scn.dataGet
_dataGet.restype = POINTER(POINTER(_Var))
_dataGet.argtypes = None

_goalGet = _scn.goalGet
_goalGet.restype = POINTER(POINTER(_Var))
_goalGet.argtypes = None

_typeGet = _scn.typeGet
_typeGet.restype = POINTER(c_uint)
_typeGet.argtypes = None

_noDataGet = _scn.noDataGet
_noDataGet.restype = c_uint
_noDataGet.argtypes = None

_data = _dataGet()
_goal = _goalGet()
_type = _typeGet()
_noData = _noDataGet()

data = {}
goal = {}
for i in range(_noData):
	id = _data[i].contents._id.decode('utf-8')

	t = None
	if _type[i] == 0:
		ptr = cast(_data[i].contents._ptr, POINTER(_Idx))
		idx = _Idx(ptr.contents._data)

		data[id] = idx
		goal[id] = idx

	if _type[i] == 1:
		ptr = cast(_data[i].contents._ptr, POINTER(_Arr))
		arr = _Arr(ptr.contents._data, ptr.contents._x, ptr.contents._y, ptr.contents._loc)

		data[id] = arr
		goal[id] = arr

# bound
class _Lim(Structure):
	_fields_ = [
		('_axis', c_uint),
		('_val', c_float)
	]

_boundRngGet = _scn.boundRngGet
_boundRngGet.restype = _CArr
_boundRngGet.argtypes = None

class _Cone(Structure):
	_fields_ = [
		('_bound', c_float * 2 * 2),
		('_loc', c_float * 3)
	]

_boundAreaGet = _scn.boundAreaGet
_boundAreaGet.restype = _CArr
_boundAreaGet.argtypes = None

_ptrBoundArea = cast(_boundAreaGet()._ptr, POINTER(POINTER(_Cone)))

_boundRngGet = _scn.boundRngGet
_boundRngGet.restype = _CArr
_boundRngGet.argtypes = None

_ptrBoundArea = cast(_boundRngGet()._ptr, POINTER(POINTER(_Lim)))

bound = {
	'rng': _boundRngGet(),
	'area': _boundAreaGet()
}

# vehicle
class _Crane(_Obj):
	_fields_ = [
		('_data', POINTER(_Cont)),
		('_loc', c_float * 3),
		('_rngHead', c_float * 2),
		('_rngClaw', c_float * 2)
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

		self._data = self._ptr.contents._data

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
		('_data', POINTER(_Arr)),
		('_loc', c_float * 3),
		('_ang', c_float),
		('_rngWheel', c_float * 2)
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
		('_data', POINTER(_Arr)),
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
_craneGet.restype = _CArr
_craneGet.argtypes = None

_cranePtr = cast(_craneGet()._ptr, POINTER(POINTER(_Crane)))

crane = None
if _craneGet()._sz:
	if _craneGet()._sz > 8:
		crane = []

		i = 0
		while i < _craneGet()._sz:
			crane.append(_cranePtr[i])

			i += sizeof(c_void_p)

	else:
		crane = _Crane(_cranePtr[0])

_truckGet = _scn.truckGet
_truckGet.restype = _CArr
_truckGet.argtypes = None

_truckPtr = cast(_truckGet()._ptr, POINTER(POINTER(_Truck)))

truck = None
if _truckGet()._sz:
	if _truckGet()._sz > 8:
		truck = []

		i = 0
		while i < _truckGet()._sz:
			truck.append(_Truck(_truckPtr[i]))

			i += sizeof(c_void_p)

	else:
		truck = _Truck(_truckPtr[0])

_cargoShipGet = _scn.cargoShipGet
_cargoShipGet.restype = _CArr
_cargoShipGet.argtypes = None

_cargoShipPtr = cast(_cargoShipGet()._ptr, POINTER(POINTER(_CargoShip)))

cargoShip = None
if _cargoShipGet()._sz:
	if _cargoShipGet()._sz > 8:
		cargoShip = []

		i = 0
		while i < _cargoShipGet()._sz:
			cargoShip.append(_CargoShip(_cargoShipPtr))

			i += sizeof(c_void_p)
	
	else:
		cargoShip = _CargoShip(_cargoShipPtr[0])

class _StreetLight(_Obj):
	_fields_ = [
		('_pass', POINTER(c_bool)),
		('_no', c_int)
	]

	def toggle(self, i):
		_streetLightToggle(self._ptr, i)

# control-flow
_streetLightGet = _scn.streetLightGet
_streetLightGet.restype = _CArr
_streetLightGet.argtypes = None

_streetLightToggle = _street_light.streetLightToggle
_streetLightToggle.restype = c_uint
_streetLightToggle.argtypes = None

_streetLightPtr = cast(_streetLightGet()._ptr, POINTER(_StreetLight))

street_light = []
i = 0
while i < _streetLightGet()._sz:
	street_light.append(_StreetLight(_streetLightPtr[i]))

	i += sizeof(c_void_p)

# path
_pathGet = _scn.pathGet
_pathGet.restype = _CArr
_pathGet.argtypes = None

# getter
# _asdfGet = _scn.asdfGet
# _asdfGet.restype = c_int
# _pathGet.argtypes = None

class _Asdf(Structure):
	def __init__(self):
		self._asdf = _dataGet()
	
	@property
	def asdf(self):
		self._asdf = cast(_data[i].contents._ptr, POINTER(_Idx))._data.contents

		return self._asdf

asdf = _Asdf()
