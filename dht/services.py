import os
from simplerpc.marshal import Marshal
from simplerpc.future import Future

class _ChordService(object):
    PING = 0x5bff9777
    LEAVE = 0x5d243ae9
    GET = 0x116f241a
    PUT = 0x6ad9429f

    __input_type_info__ = {
        'ping': [],
        'leave': [],
        'get': ['std::string'],
        'put': ['std::string','std::string'],
    }

    __output_type_info__ = {
        'ping': [],
        'leave': [],
        'get': ['rpc::i32','std::string'],
        'put': ['rpc::i32'],
    }

    def __bind_helper__(self, func):
        def f(*args):
            return getattr(self, func.__name__)(*args)
        return f

    def __reg_to__(self, server):
        server.__reg_func__(_ChordService.PING, self.__bind_helper__(self.ping), [], [])
        server.__reg_func__(_ChordService.LEAVE, self.__bind_helper__(self.leave), [], [])
        server.__reg_func__(_ChordService.GET, self.__bind_helper__(self.get), ['std::string'], ['rpc::i32','std::string'])
        server.__reg_func__(_ChordService.PUT, self.__bind_helper__(self.put), ['std::string','std::string'], ['rpc::i32'])

    def ping(__self__):
        raise NotImplementedError('subclass _ChordService and implement your own ping function')

    def leave(__self__):
        raise NotImplementedError('subclass _ChordService and implement your own leave function')

    def get(__self__, key):
        raise NotImplementedError('subclass _ChordService and implement your own get function')

    def put(__self__, key, value):
        raise NotImplementedError('subclass _ChordService and implement your own put function')

class _ChordProxy(object):
    def __init__(self, clnt):
        self.__clnt__ = clnt

    def async_ping(__self__):
        return __self__.__clnt__.async_call(_ChordService.PING, [], _ChordService.__input_type_info__['ping'], _ChordService.__output_type_info__['ping'])

    def async_leave(__self__):
        return __self__.__clnt__.async_call(_ChordService.LEAVE, [], _ChordService.__input_type_info__['leave'], _ChordService.__output_type_info__['leave'])

    def async_get(__self__, key):
        return __self__.__clnt__.async_call(_ChordService.GET, [key], _ChordService.__input_type_info__['get'], _ChordService.__output_type_info__['get'])

    def async_put(__self__, key, value):
        return __self__.__clnt__.async_call(_ChordService.PUT, [key, value], _ChordService.__input_type_info__['put'], _ChordService.__output_type_info__['put'])

    def sync_ping(__self__):
        __result__ = __self__.__clnt__.sync_call(_ChordService.PING, [], _ChordService.__input_type_info__['ping'], _ChordService.__output_type_info__['ping'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

    def sync_leave(__self__):
        __result__ = __self__.__clnt__.sync_call(_ChordService.LEAVE, [], _ChordService.__input_type_info__['leave'], _ChordService.__output_type_info__['leave'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

    def sync_get(__self__, key):
        __result__ = __self__.__clnt__.sync_call(_ChordService.GET, [key], _ChordService.__input_type_info__['get'], _ChordService.__output_type_info__['get'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

    def sync_put(__self__, key, value):
        __result__ = __self__.__clnt__.sync_call(_ChordService.PUT, [key, value], _ChordService.__input_type_info__['put'], _ChordService.__output_type_info__['put'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

