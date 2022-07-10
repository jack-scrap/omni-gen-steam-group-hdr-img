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
		('data', POINTER(_Cont)),
                ('sz', c_uint),
                ('offset', c_float * 3),
                ('_parent', c_void_p)
	]

class _Array(Structure):
	_fields_ = [
		('data', POINTER(POINTER(_Idx))),
		('x', c_uint),
		('y', c_uint),
		('offset', c_float * 3),
                ('_parent', c_void_p)
	]

class _Dict(Structure):
	_fields_ = [
		('data', POINTER(c_void_p)),
		('type', POINTER(c_uint)),
		('no', c_uint),
                ('offset', c_float * 3),
                ('_parent', c_void_p)
	]

class _Var(Structure):
	_fields_ = [
		('_id', c_char_p),
		('_ptr', c_void_p),
                ('_type', c_uint)
	]

class _Scope:
    def __parseByte(self, ptr):
        contPtr = ptr
        cont = contPtr.contents

        return int.from_bytes(cont._c, byteorder = 'little')

    def __parseIdx(self, ptr):
        rep = None

        ptrIdx = cast(ptr, POINTER(_Idx))
        idx = ptrIdx.contents

        ls = cast(idx.data, POINTER(POINTER(_Cont)))

        if (idx.sz):
            # list
            if idx.sz > 1:
                rep = []

                for i in range(idx.sz):
                    rep.append(self.__parseByte(ls[i]))

            # scalar
            else:
                rep = self.__parseByte(ls[0])

        return rep

    def __parseArray(self, ptr):
        rep = []

        ptrArray = cast(ptr, POINTER(_Array))
        array = ptrArray.contents

        lsIdx = cast(array.data, POINTER(POINTER(_Idx)))

        # 2D
        if (array.y > 1):
            for y in range(array.y):
                sub = []

                for x in range(array.x):
                    i = (y * array.y) + x

                    sub.append(self.__parseIdx(lsIdx[i]))

                rep.append(sub)

        # 1D
        else:
            for x in range(array.x):
                rep.append(self.__parseIdx(lsIdx[x]))

        return rep

    def __init__(self, ptr, no):
        self.__intern = {}

        for i in range(no):
            varPtr = ptr[i]
            var = varPtr.contents

            name = var._id.decode('utf-8')

            rep = None
            offset = None

            # index
            if (var._type == 0):
                rep = self.__parseIdx(var._ptr)

                idxPtr = cast(var._ptr, POINTER(_Idx))
                idx = idxPtr.contents

                offset = [round(val, 4) for val in list(idx.offset)]

            # array
            if (var._type == 1):
                rep = self.__parseArray(var._ptr)

                arrayPtr = cast(var._ptr, POINTER(_Array))
                array = arrayPtr.contents

                offset = [round(val, 4) for val in list(array.offset)]

            self.__intern[name] = {
                    'ptr': var._ptr,
                    'type': var._type,
                    'offset': offset
            }

    def __getitem__(self, k):
        el = self.__intern[k]

        ptr = el['ptr']
        t = el['type']
        offset = el['offset']

        rep = None

        # index
        if (t == 0):
            rep = self.__parseIdx(ptr)

        # array
        if (t == 1):
            rep = self.__parseArray(ptr)

        return {
                'val': rep,
                'offset': offset
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
goal = _Scope(_goal, _noData)

# bound
class _Lim(Structure):
	_fields_ = [
		('axis', c_uint),
		('val', c_float),
                ('_parent', c_void_p)
	]

_boundRngGet = _scn.boundRngGet
_boundRngGet.restype = _CArr
_boundRngGet.argtypes = None

class _Cone(Structure):
	_fields_ = [
		('bound', c_float * 2 * 2),
		('offset', c_float * 3),
                ('_parent', c_void_p)
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
_cIncr = _scn.cIncr
_cIncr.restype = c_void_p
_cIncr.argtypes = None

class _Crane(_Obj):
	_fields_ = [
		('data', POINTER(_Cont)),
		('offset', c_float * 3),
                ('_parent', c_void_p)
	]

	rngTrack = [
		4.0,
		13.8
	]
	rngHead = [
		-3.0,
		3.0
        ]

	def __getattribute__(self, attr):
            ptr = super(_Crane, self).__getattribute__(attr)

            if attr == 'data':
                val = None

                if ptr:
                    val = ptr.contents._c

                return val

            if attr == 'offset':
                ls = []
                for i in range(3):
                    ls.append(ptr[i])

                return ls

	def zoom(self, delta):
		_craneZoom(self._ptr, delta)

		for i in range(3):
			self.offset[i] = self._ptr.contents.offset[i]

		_cIncr()

	def pan(self, delta):
		_cranePan(self._ptr, delta)

		for i in range(3):
			self.offset[i] = self._ptr.contents.offset[i]

		_cIncr()

	def ped(self, delta):
		_cranePed(self._ptr, delta)

		for i in range(3):
			self.offset[i] = self._ptr.contents.offset[i]

		_cIncr()

	def grab(self):
		_craneGrab(self._ptr)

		self.data = self._ptr.contents.data

		_cIncr()

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
		('data', POINTER(_Array)),
		('offset', c_float * 3),
		('ang', c_float),
                ('_uni', c_uint * 2),
                ('_parent', c_void_p)
	]

	rngWheel = [
		-(math.pi / 2),
		math.pi / 2
	]

	def turn(self, delta):
		_truckTurn(self._ptr, delta)

		self.ang = self._ptr.contents.ang

		for i in range(3):
			self.offset[i] = self._ptr.contents.offset[i]

		_cIncr()

	def mv(self, delta):
		for i in range(3):
			self.offset[i] = self._ptr.contents.offset[i]

		_truckMv(self._ptr, delta)

		_cIncr()

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
		('data', POINTER(_Array)),
		('offset', c_float * 3),
                ('_parent', c_void_p)
	]

	def mv(self, delta):
		_cargoShipMv(self._ptr, delta)

		for i in range(3):
			self.offset[i] = self._ptr.contents.offset[i]

		_cIncr()

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
		('_pass', _CArr),
                ('_parent', c_void_p)
	]

	def toggle(self, i):
		_streetSignToggle(self._ptr, i)

		_cIncr()

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
