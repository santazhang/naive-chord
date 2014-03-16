import os
from simplerpc.marshal import Marshal
from simplerpc.future import Future

class ChordService(object):
    PING = 0x17c722ee

    __input_type_info__ = {
        'ping': [],
    }

    __output_type_info__ = {
        'ping': [],
    }

    def __bind_helper__(self, func):
        def f(*args):
            return getattr(self, func.__name__)(*args)
        return f

    def __reg_to__(self, server):
        server.__reg_func__(ChordService.PING, self.__bind_helper__(self.ping), [], [])

    def ping(__self__):
        raise NotImplementedError('subclass ChordService and implement your own ping function')

class ChordProxy(object):
    def __init__(self, clnt):
        self.__clnt__ = clnt

    def async_ping(__self__):
        return __self__.__clnt__.async_call(ChordService.PING, [], ChordService.__input_type_info__['ping'], ChordService.__output_type_info__['ping'])

    def sync_ping(__self__):
        __result__ = __self__.__clnt__.sync_call(ChordService.PING, [], ChordService.__input_type_info__['ping'], ChordService.__output_type_info__['ping'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

