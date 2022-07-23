from ctypes import *
import math

_crane = CDLL('libcrane.so')
_truck = CDLL('libtruck.so')
_cargo_ship = CDLL('libcargo_ship.so')
_street_sign = CDLL('libstreet_sign.so')
_scn = CDLL('libscn.so')

def _parseFloat(val):
    return round(val, 4)

def _parseByte(ptr):
    bytePtr = ptr
    byte = bytePtr.contents

    return int.from_bytes(byte._c, byteorder = 'little')

def _parseIdx(ptr):
    rep = None

    ptrIdx = cast(ptr, POINTER(_Idx))
    idx = ptrIdx.contents

    byteCont = cast(idx._data, POINTER(POINTER(_Cont)))

    if (idx.sz):
        # list
        if idx.sz > 1:
            rep = []

            for i in range(idx.sz):
                rep.append(_parseByte(byteCont[i]))

        # scalar
        else:
            rep = _parseByte(byteCont.contents)

    return rep

def _parseArray(ptr):
    rep = []

    ptrArray = cast(ptr, POINTER(_Array))
    array = ptrArray.contents

    idxCont = cast(array._data, POINTER(POINTER(_Idx)))

    # 2D
    if (array.y > 1):
        for y in range(array.y):
            sub = []

            for x in range(array.x):
                i = (y * array.y) + x

                sub.append(_parseIdx(idxCont[i]))

            rep.append(sub)

    # 1D
    else:
        for x in range(array.x):
            rep.append(_parseIdx(idxCont[x]))

        return rep

def _parseOffset(offset):
    ls = [_parseFloat(val) for val in list(offset)]

    return ls

class _CArr(Structure):
    _fields_ = [
            ('ptr', c_void_p),
            ('sz', c_size_t)
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
            ('_data', POINTER(_Cont)),
            ('sz', c_uint),
            ('_offset', c_float * 3),
            ('_parent', c_void_p)
    ]

class _Array(Structure):
    _fields_ = [
            ('_data', POINTER(POINTER(_Idx))),
            ('x', c_uint),
            ('y', c_uint),
            ('_offset', c_float * 3),
            ('_parent', c_void_p)
    ]

class _Dict(Structure):
    _fields_ = [
            ('_data', POINTER(c_void_p)),
            ('_type', POINTER(c_uint)),
            ('_key', POINTER(POINTER(c_char))),
            ('_no', c_uint),
            ('_offset', c_float * 3),
            ('_parent', c_void_p)
    ]

class _Var(Structure):
    _fields_ = [
            ('id', c_char_p),
            ('ptr', c_void_p),
            ('type', c_uint)
    ]

class _Scope:
    def __init__(self, ptr, no):
        self.__intern = {}

        for i in range(no):
            varPtr = ptr[i]
            var = varPtr.contents

            name = var.id.decode('utf-8')

            rep = None
            offset = None

            # index
            if (var.type == 0):
                rep = _parseIdx(var.ptr)

                idxPtr = cast(var.ptr, POINTER(_Idx))
                idx = idxPtr.contents

                offset = _parseOffset(idx._offset)

            # array
            if (var.type == 1):
                rep = _parseArray(var.ptr)

                arrayPtr = cast(var.ptr, POINTER(_Array))
                array = arrayPtr.contents

                offset = _parseOffset(array._offset)

            self.__intern[name] = {
                    'ptr': var.ptr,
                    'type': var.type,
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
            rep = _parseIdx(ptr)

        # array
        if (t == 1):
            rep = _parseArray(ptr)

        return {
                'val': rep,
                'offset': offset
        }

class _Bound:
    def __init__(self, ptrRng, ptrArea):
        self._intern = {
                'rng': ptrRng,
                'area': ptrArea
        }

    def _axisStr(self, i):
        val = None

        if i == 0:
            val = 'X'

        if i == 1:
            val = 'Y'

        if i == 2:
            val = 'Z'

        return val

    def __getitem__(self, k):
        cArr = self._intern[k]

        rep = []
        if k == 'rng':
            limCont = cast(cArr.ptr, POINTER(POINTER(_Lim)))

            for i in range(int(cArr.sz / 4)):
                limPtr = limCont[i]
                lim = limPtr.contents

                rep.append({
                        'axis': self._axisStr(lim.axis),
                        'val': _parseFloat(lim.val)
                })

        if k == 'area':
            coneCont = cast(cArr.ptr, POINTER(POINTER(_Cone)))

            for i in range(int(cArr.sz / 8)):
                conePtr = coneCont[i]
                cone = conePtr.contents

                rep.append({
                    'bound': cone.bound,
                    'offset': cone.offset
                })

        return rep

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
            ('_axis', c_uint),
            ('_val', c_float),
            ('_parent', c_void_p)
    ]

    @property
    def axis(self):
        return int(self._axis)

    @property
    def val(self):
        return _parseFloat(self._val)

_boundRngGet = _scn.boundRngGet
_boundRngGet.restype = _CArr
_boundRngGet.argtypes = None

class _Cone(Structure):
    _fields_ = [
            ('_bound', c_float * 2 * 2),
            ('_offset', c_float * 3),
            ('_parent', c_void_p)
    ]

    @property
    def bound(self):
            area = []
            for y in range(2):
                rng = []
                for x in range(2):
                    rng.append(_parseFloat(self._bound[y][x]))

                area.append(rng)

            return area

    @property
    def offset(self):
        return _parseOffset(self._ptr.contents.offset)

_boundAreaGet = _scn.boundAreaGet
_boundAreaGet.restype = _CArr
_boundAreaGet.argtypes = None

_ptrBoundArea = cast(_boundAreaGet().ptr, POINTER(POINTER(_Cone)))

_boundRngGet = _scn.boundRngGet
_boundRngGet.restype = _CArr
_boundRngGet.argtypes = None

_ptrBoundArea = cast(_boundRngGet().ptr, POINTER(POINTER(_Lim)))

bound = _Bound(_boundRngGet(), _boundAreaGet())

# vehicle
_callIncr = _scn.callIncr
_callIncr.restype = c_void_p
_callIncr.argtypes = None

class _Crane(_Obj):
    _fields_ = [
            ('_data', POINTER(_Cont)),
            ('_offset', c_float * 3),
            ('_parent', c_void_p)
    ]

    rng_track = [
            4.0,
            13.8
    ]
    rng_head = [
            -3.0,
            3.0
    ]

    @property
    def data(self):
        ptr = self._ptr
        data = ptr.contents._data

        val = None

        if data:
            val = data.contents._c

        return val

    @property
    def offset(self):
        return _parseOffset(self._ptr.contents._offset)

    def zoom(self, delta):
        _craneZoom(self._ptr, delta)

        _callIncr()

    def pan(self, delta):
        _cranePan(self._ptr, delta)

        _callIncr()

    def ped(self, delta):
        _cranePed(self._ptr, delta)

        _callIncr()

    def grab(self):
        _craneGrab(self._ptr)

        _callIncr()

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

class Crane:
    rng_track = _Crane.rng_track
    rng_track = _Crane.rng_head

class _Truck(_Obj):
    _fields_ = [
            ('_data', POINTER(_Array)),
            ('_offset', c_float * 3),
            ('_ang', c_float),
            ('_uni', c_uint * 2),
            ('_parent', c_void_p)
    ]

    rng_wheel = [
            -(math.pi / 2),
            math.pi / 2
    ]

    @property
    def data(self):
        ptr = self._ptr
        arrayPtr = ptr.contents._data

        return _parseArray(arrayPtr)

    @property
    def offset(self):
        return _parseOffset(self._ptr.contents._offset)

    @property
    def ang(self):
        ptr = self._ptr
        ang = ptr.contents._ang

        return _parseFloat(ang)

    def turn(self, delta):
        _truckTurn(self._ptr, delta)

        _callIncr()

    def mv(self, delta):
        _truckMv(self._ptr, delta)

        _callIncr()

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

class Truck:
    rng_wheel = _Truck.rng_wheel

class _CargoShip(_Obj):
    _fields_ = [
            ('_data', POINTER(_Array)),
            ('_offset', c_float * 3),
            ('_parent', c_void_p)
    ]

    @property
    def data(self):
        ptr = self._ptr
        arrayPtr = ptr.contents._data

        return _parseArray(arrayPtr)

    @property
    def offset(self):
        return _parseOffset(self._ptr.contents._offset)

    def mv(self, delta):
        _cargoShipMv(self._ptr, delta)

        _callIncr()

_cargoShipMv = _cargo_ship.cargoShipMv
_cargoShipMv.restype = c_void_p
_cargoShipMv.argtypes = [
                POINTER(_CargoShip),
                c_float
]

class CargoShip:
    pass

_craneGet = _scn.craneGet
_craneGet.restype = _CArr
_craneGet.argtypes = None

_cranePtr = cast(_craneGet().ptr, POINTER(POINTER(_Crane)))

crane = None
if _craneGet().sz:
    if _craneGet().sz > 8:
        crane = []

        i = 0
        while i < _craneGet().sz:
            crane.append(_Crane(_cranePtr[i]))

            i += sizeof(c_void_p)

    else:
        crane = _Crane(_cranePtr.contents)

_truckGet = _scn.truckGet
_truckGet.restype = _CArr
_truckGet.argtypes = None

_truckPtr = cast(_truckGet().ptr, POINTER(POINTER(_Truck)))

truck = None
if _truckGet().sz:
    if _truckGet().sz > 8:
        truck = []

        i = 0
        while i < _truckGet().sz:
            truck.append(_Truck(_truckPtr[i]))

            i += sizeof(c_void_p)

    else:
        truck = _Truck(_truckPtr.contents)

_cargoShipGet = _scn.cargoShipGet
_cargoShipGet.restype = _CArr
_cargoShipGet.argtypes = None

_cargoShipPtr = cast(_cargoShipGet().ptr, POINTER(POINTER(_CargoShip)))

cargo_ship = None
if _cargoShipGet().sz:
    if _cargoShipGet().sz > 8:
        cargo_ship = []

        i = 0
        while i < _cargoShipGet().sz:
            cargo_ship.append(_CargoShip(_cargoShipPtr))

            i += sizeof(c_void_p)

    else:
        cargo_ship = _CargoShip(_cargoShipPtr.contents)

class _StreetSign(_Obj):
    _fields_ = [
            ('_status', _CArr),
            ('_parent', c_void_p)
    ]

    @property
    def status(self):
        ls = []
        for i in range(int(_status._sz / 8)):
            ls.append(_status.ptr[i])

        return ls

    def toggle(self, i):
        _streetSignToggle(self._ptr, i)

        _callIncr()

# control-flow
_streetSignGet = _scn.streetSignGet
_streetSignGet.restype = _CArr
_streetSignGet.argtypes = None

_streetSignToggle = _street_sign.streetSignToggle
_streetSignToggle.restype = c_uint
_streetSignToggle.argtypes = None

_streetSignPtr = cast(_streetSignGet().ptr, POINTER(_StreetSign))

street_sign = []
i = 0
while i < _streetSignGet().sz:
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
