from ctypes import *
import math

_crane = CDLL('libcrane.so')
_truck = CDLL('libtruck.so')
_cargo_ship = CDLL('libcargo_ship.so')
_street_sign = CDLL('libstreet_sign.so')
_scn = CDLL('libscn.so')

def _parse_float(val):
    return round(val, 4)

def _parse_byte(ptr):
    byte = ptr.contents

    return int.from_bytes(byte._c, byteorder = 'little')

def _parse_idx(ptr):
    rep = None

    ptr_idx = cast(ptr, POINTER(_Idx))
    idx = ptr_idx.contents

    byte_cont = cast(idx._data, POINTER(POINTER(_Cont)))

    if (idx.sz):
        # List
        if idx.sz > 1:
            rep = []

            for i in range(idx.sz):
                rep.append(_parse_byte(byte_cont[i]))

        # Scalar
        else:
            rep = _parse_byte(byte_cont.contents)

    return rep

def _parse_array(ptr):
    rep = []

    ptr_array = cast(ptr, POINTER(_Array))
    array = ptr_array.contents

    idx_cont = cast(array._data, POINTER(POINTER(_Idx)))

    # 2D
    if (array.y > 1):
        for y in range(array.y):
            sub = []

            for x in range(array.x):
                i = (y * array.y) + x

                sub.append(_parse_idx(idx_cont[i]))

            rep.append(sub)

    # 1D
    else:
        for x in range(array.x):
            rep.append(_parse_idx(idx_cont[x]))

        return rep

def _parse_offset(offset):
    ls = [_parse_float(val) for val in list(offset)]

    return ls

class _CArr(Structure):
    _fields_ = [
            ('ptr', c_void_p),
            ('sz', c_size_t)
    ]

class _Obj(Structure):
    def __init__(self, ptr):
        self._ptr = ptr

# Data
class _Cont(Structure):
    _fields_ = [
            ('_c', c_char),
            ('_parent', c_void_p)
    ]

class _Idx(Structure):
    _fields_ = [
            ('_data', POINTER(_Cont)),
            ('sz', c_uint),
            ('_parent', c_void_p)
    ]

class _Array(Structure):
    _fields_ = [
            ('_data', POINTER(POINTER(_Idx))),
            ('x', c_uint),
            ('y', c_uint),
            ('_parent', c_void_p)
    ]

class _Dict(Structure):
    _fields_ = [
            ('_data', POINTER(c_void_p)),
            ('_type', POINTER(c_uint)),
            ('_key', POINTER(POINTER(c_char))),
            ('_no', c_uint),
            ('_parent', c_void_p)
    ]

class _Var(Structure):
    _fields_ = [
            ('id', c_char_p),
            ('ptr', c_void_p),
            ('type', c_uint),
            ('offset', c_float * 3)
    ]

class _Scope:
    def __init__(self, ptr, no):
        self.__intern = {}

        for i in range(no):
            var_ptr = ptr[i]
            var = var_ptr.contents

            name = var.id.decode('utf-8')

            rep = None

            # Index
            if (var.type == 0):
                rep = _parse_idx(var.ptr)

                idx_ptr = cast(var.ptr, POINTER(_Idx))
                idx = idx_ptr.contents

            # Array
            if (var.type == 1):
                rep = _parse_array(var.ptr)

                array_ptr = cast(var.ptr, POINTER(_Array))
                array = array_ptr.contents

            self.__intern[name] = {
                    'ptr': var.ptr,
                    'type': var.type
            }

    def __getitem__(self, k):
        el = self.__intern[k]

        ptr = el['ptr']
        t = el['type']

        rep = None

        # Index
        if (t == 0):
            rep = _parse_idx(ptr)

        # Array
        if (t == 1):
            rep = _parse_array(ptr)

        return {
                'val': rep
        }

class _Bound:
    def __init__(self, ptr_rng, ptr_area):
        self._intern = {
                'rng': ptr_rng,
                'area': ptr_area
        }

    def _axis_str(self, i):
        val = None

        if i == 0:
            val = 'X'

        if i == 1:
            val = 'Y'

        if i == 2:
            val = 'Z'

        return val

    def __getitem__(self, k):
        c_arr = self._intern[k]

        rep = []
        if k == 'rng':
            lim_cont = cast(c_arr.ptr, POINTER(POINTER(_Lim)))

            for i in range(int(c_arr.sz / 4)):
                lim_ptr = lim_cont[i]
                lim = lim_ptr.contents

                rep.append({
                        'axis': self._axis_str(lim.axis),
                        'val': _parse_float(lim.val)
                })

        if k == 'area':
            cone_cont = cast(c_arr.ptr, POINTER(POINTER(_Cone)))

            for i in range(int(c_arr.sz / 8)):
                cone_ptr = cone_cont[i]
                cone = cone_ptr.contents

                rep.append({
                    'bound': cone.bound,
                    'offset': cone.offset
                })

        return rep

_data_get = _scn.dataGet
_data_get.restype = POINTER(POINTER(_Var))
_data_get.argtypes = None

_goal_get = _scn.goalGet
_goal_get.restype = POINTER(POINTER(_Var))
_goal_get.argtypes = None

_no_data_get = _scn.noDataGet
_no_data_get.restype = c_uint
_no_data_get.argtypes = None

_data = _data_get()
_goal = _goal_get()
_noData = _no_data_get()

data = _Scope(_data, _noData)
goal = _Scope(_goal, _noData)

# Bound
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
        return _parse_float(self._val)

_bound_rng_get = _scn.boundRngGet
_bound_rng_get.restype = _CArr
_bound_rng_get.argtypes = None

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
                    rng.append(_parse_float(self._bound[y][x]))

                area.append(rng)

            return area

    @property
    def offset(self):
        return _parse_offset(self._ptr.contents.offset)

_bound_area_get = _scn.boundAreaGet
_bound_area_get.restype = _CArr
_bound_area_get.argtypes = None

_ptr_bound_area = cast(_bound_area_get().ptr, POINTER(POINTER(_Cone)))

_bound_rng_get = _scn.boundRngGet
_bound_rng_get.restype = _CArr
_bound_rng_get.argtypes = None

_ptr_bound_area = cast(_bound_rng_get().ptr, POINTER(POINTER(_Lim)))

bound = _Bound(_bound_rng_get(), _bound_area_get())

# Vehicle
_call_incr = _scn.callIncr
_call_incr.restype = c_void_p
_call_incr.argtypes = None

class _Crane(_Obj):
    _fields_ = [
            ('_data', POINTER(_Cont)),
            ('_offset', c_float * 3),
            ('_offsetTrack', c_float * 3),
            ('_offsetHead', c_float * 3),
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
        data = self._ptr.contents._data

        val = None

        if data:
            val = data.contents._c

        return val

    @property
    def offset(self):
        return _parse_offset(self._ptr.contents._offset)

    @property
    def offset_track(self):
        offset = self._ptr.contents._offsetTrack

        ls = _parse_offset(offset)

        return ls

    @property
    def offset_head(self):
        offset = self._ptr.contents._offsetHead

        ls = _parse_offset(offset)

        return ls

    def zoom(self, delta):
        _crane_zoom(self._ptr, delta)

        _call_incr()

    def pan(self, delta):
        _crane_pan(self._ptr, delta)

        _call_incr()

    def ped(self, delta):
        _crane_ped(self._ptr, delta)

        _call_incr()

    def grab(self):
        _crane_grab(self._ptr)

        _call_incr()

_crane_zoom = _crane.craneZoom
_crane_zoom.restype = c_void_p
_crane_zoom.argtypes = [
        POINTER(_Crane),
        c_float
]

_crane_pan = _crane.cranePan
_crane_pan.restype = c_void_p
_crane_pan.argtypes = [
        POINTER(_Crane),
        c_float
]

_crane_ped = _crane.cranePed
_crane_ped.restype = c_void_p
_crane_ped.argtypes = [
        POINTER(_Crane),
        c_float
]

_crane_grab = _crane.craneGrab
_crane_grab.restype = c_void_p
_crane_grab.argtypes = None

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
        array_ptr = self._ptr.contents._data

        return _parse_array(array_ptr)

    @property
    def offset(self):
        return _parse_offset(self._ptr.contents._offset)

    @property
    def ang(self):
        ang = self._ptr.contents._ang

        return _parse_float(ang)

    def turn(self, delta):
        _truck_turn(self._ptr, delta)

        _call_incr()

    def mv(self, delta):
        _truck_mv(self._ptr, delta)

        _call_incr()

_truck_turn = _truck.truckTurn
_truck_turn.restype = c_void_p
_truck_turn.argtypes = [
                POINTER(_Truck),
                c_float
]

_truck_mv = _truck.truckMv
_truck_mv.restype = c_void_p
_truck_mv.argtypes = [
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
        array_ptr = self._ptr.contents._data

        return _parse_array(array_ptr)

    @property
    def offset(self):
        return _parse_offset(self._ptr.contents._offset)

    def mv(self, delta):
        _cargo_ship_mv(self._ptr, delta)

        _call_incr()

_cargo_ship_mv = _cargo_ship.cargoShipMv
_cargo_ship_mv.restype = c_void_p
_cargo_ship_mv.argtypes = [
                POINTER(_CargoShip),
                c_float
]

class CargoShip:
    x = 10
    y = 4

_crane_get = _scn.craneGet
_crane_get.restype = _CArr
_crane_get.argtypes = None

_crane_ptr = cast(_crane_get().ptr, POINTER(POINTER(_Crane)))

crane = None
if _crane_get().sz:
    if _crane_get().sz > 8:
        crane = []

        i = 0
        while i < _crane_get().sz:
            crane.append(_Crane(_crane_ptr[i]))

            i += sizeof(c_void_p)

    else:
        crane = _Crane(_crane_ptr.contents)

_truck_get = _scn.truckGet
_truck_get.restype = _CArr
_truck_get.argtypes = None

_truck_ptr = cast(_truck_get().ptr, POINTER(POINTER(_Truck)))

truck = None
if _truck_get().sz:
    if _truck_get().sz > 8:
        truck = []

        i = 0
        while i < _truck_get().sz:
            truck.append(_Truck(_truck_ptr[i]))

            i += sizeof(c_void_p)

    else:
        truck = _Truck(_truck_ptr.contents)

_cargo_ship_get = _scn.cargoShipGet
_cargo_ship_get.restype = _CArr
_cargo_ship_get.argtypes = None

_cargo_ship_ptr = cast(_cargo_ship_get().ptr, POINTER(POINTER(_CargoShip)))

cargo_ship = None
if _cargo_ship_get().sz:
    if _cargo_ship_get().sz > 8:
        cargo_ship = []

        i = 0
        while i < _cargo_ship_get().sz:
            cargo_ship.append(_CargoShip(_cargo_ship_ptr))

            i += sizeof(c_void_p)

    else:
        cargo_ship = _CargoShip(_cargo_ship_ptr.contents)

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
        street_sign_toggle(self._ptr, i)

        _call_incr()

# Control-flow
_street_sign_get = _scn.streetSignGet
_street_sign_get.restype = _CArr
_street_sign_get.argtypes = None

street_sign_toggle = _street_sign.streetSignToggle
street_sign_toggle.restype = c_uint
street_sign_toggle.argtypes = None

_street_sign_ptr = cast(_street_sign_get().ptr, POINTER(_StreetSign))

street_sign = []
i = 0
while i < _street_sign_get().sz:
    street_sign.append(_StreetSign(_street_sign_ptr[i]))

    i += sizeof(c_void_p)

# Path
_node_get = _scn.nodeGet
_node_get.restype = _CArr
_node_get.argtypes = None

node = _node_get()

_path_get = _scn.pathGet
_path_get.restype = _CArr
_path_get.argtypes = None

path = _path_get()
