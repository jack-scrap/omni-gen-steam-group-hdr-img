from ctypes import *
import math

_crane = CDLL('libcrane.so')
_truck = CDLL('libtruck.so')
_cargo_ship = CDLL('libcargo_ship.so')
_street_sign = CDLL('libstreet_sign.so')
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
		('_c', c_char),
                ('_parent', c_void_p)
	]

class _Idx(Structure):
	_fields_ = [
		('_i', c_uint),
		('_data', POINTER(_Cont)),
                ('_sz', c_uint),
                ('_parent', c_void_p)
	]

class _Array(Structure):
	_fields_ = [
		('_data', POINTER(POINTER(_Idx))),
		('_x', c_uint),
		('_y', c_uint),
		('_loc', c_float * 3)
	]

class _Dict(Structure):
	_fields_ = [
		('_data', POINTER(c_void_p)),
		('_type', POINTER(c_uint)),
		('_no', c_uint)
	]

class _Var(Structure):
	_fields_ = [
		('_id', c_char_p),
		('_ptr', c_void_p),
                ('_type', c_uint)
	]

class _Scope:
    _intern = {}

    def __init__(self, raw, no):
        for i in range(no):
            varPtr = raw[i]
            var = varPtr.contents

            name = var._id.decode('utf-8')

            idxPtr = cast(var._ptr, POINTER(_Idx))
            idx = idxPtr.contents

            arr = cast(idx._data, POINTER(POINTER(_Cont)))

            contPtr = []
            if idx._sz:
                for i in range(idx._sz):
                    contPtr.append(arr[i])

            self._intern[name] = contPtr

    def __getitem__(self, k):
        arr = self._intern[k]

        rep = []
        for i in range(len(arr)):
            contPtr = arr[i]
            cont = contPtr.contents

            rep.append(int.from_bytes(cont._c, byteorder = 'little'))

        return {
                'val': rep
        }

_dataGet = _scn.dataGet
_dataGet.restype = POINTER(POINTER(_Var))
_dataGet.argtypes = None

_goalGet = _scn.goalGet
_goalGet.restype = POINTER(POINTER(_Var))
_goalGet.argtypes = None

_noDataGet = _scn.noDataGet
_noDataGet.restype = c_uint
_noDataGet.argtypes = None

_data = _dataGet()
_goal = _goalGet()
_noData = _noDataGet()

data = _Scope(_data, _noData)

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
_cInc = _scn.cInc
_cInc.restype = c_void_p
_cInc.argtypes = None

class _Crane(_Obj):
	_fields_ = [
		('_data', POINTER(_Cont)),
		('_loc', c_float * 3)
	]

	_rngTrack = [
		4.0,
		13.8
	]
	_rngHead = [
		-3.0,
		3.0
        ]

	def zoom(self, delta):
		_craneZoom(self._ptr, delta)

		for i in range(3):
			self._loc[i] = self._ptr.contents._loc[i]

		_cInc()

	def pan(self, delta):
		_cranePan(self._ptr, delta)

		for i in range(3):
			self._loc[i] = self._ptr.contents._loc[i]

		_cInc()

	def ped(self, delta):
		_cranePed(self._ptr, delta)

		for i in range(3):
			self._loc[i] = self._ptr.contents._loc[i]

		_cInc()

	def grab(self):
		_craneGrab(self._ptr)

		self._data = self._ptr.contents._data

		_cInc()

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
	_rngWheel = [
		-(math.pi / 2),
		math.pi / 2
	]

	_fields_ = [
		('_data', POINTER(_Array)),
		('_loc', c_float * 3),
                ('_ang', c_float)
	]

	def turn(self, delta):
		_truckTurn(self._ptr, delta)

		self._ang = self._ptr.contents._ang

		for i in range(3):
			self._loc[i] = self._ptr.contents._loc[i]

		_cInc()

	def mv(self, delta):
		for i in range(3):
			self._loc[i] = self._ptr.contents._loc[i]

		_truckMv(self._ptr, delta)

		_cInc()

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
		('_data', POINTER(_Array)),
		('_loc', c_float * 3)
	]

	def mv(self, delta):
		_cargoShipMv(self._ptr, delta)

		for i in range(3):
			self._loc[i] = self._ptr.contents._loc[i]

		_cInc()

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
			crane.append(_Crane(_cranePtr[i]))

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

cargo_ship = None
if _cargoShipGet()._sz:
	if _cargoShipGet()._sz > 8:
		cargo_ship = []

		i = 0
		while i < _cargoShipGet()._sz:
			cargo_ship.append(_CargoShip(_cargoShipPtr))

			i += sizeof(c_void_p)

	else:
		cargo_ship = _CargoShip(_cargoShipPtr[0])

class _StreetSign(_Obj):
	_fields_ = [
		('_pass', POINTER(c_bool)),
		('_no', c_int)
	]

	def toggle(self, i):
		_streetSignToggle(self._ptr, i)

		_cInc()

# control-flow
_streetSignGet = _scn.streetSignGet
_streetSignGet.restype = _CArr
_streetSignGet.argtypes = None

_streetSignToggle = _street_sign.streetSignToggle
_streetSignToggle.restype = c_uint
_streetSignToggle.argtypes = None

_streetSignPtr = cast(_streetSignGet()._ptr, POINTER(_StreetSign))

street_sign = []
i = 0
while i < _streetSignGet()._sz:
	street_sign.append(_StreetSign(_streetSignPtr[i]))

	i += sizeof(c_void_p)

# path
## node
_nodeGet = _scn.nodeGet
_nodeGet.restype = _CArr
_nodeGet.argtypes = None

node = _nodeGet()

## strip
_pathGet = _scn.pathGet
_pathGet.restype = _CArr
_pathGet.argtypes = None

path = _pathGet()
