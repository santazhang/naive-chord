import os
from simplerpc.marshal import Marshal
from simplerpc.future import Future

_keyrange = Marshal.reg_type('_keyrange', [('low', 'std::string'), ('high', 'std::string')])

class _ChordService(object):
    FETCH_LOOKUP_TABLE = 0x51cb03d9
    MIGRATE_SOME_RANGE = 0x1259d0e9
    START_MIGRATION = 0x4185dce6
    FINISH_MIGRATION = 0x67ae4770
    LEAVE = 0x37f0d9c5
    DEBUG_PRINT = 0x6a2997f1
    GET = 0x1130ea02
    PUT = 0x5710eb19
    REMOVE = 0x488a665b

    __input_type_info__ = {
        'fetch_lookup_table': [],
        'migrate_some_range': ['std::string'],
        'start_migration': ['_keyrange'],
        'finish_migration': ['_keyrange'],
        'leave': [],
        'debug_print': [],
        'get': ['std::string'],
        'put': ['std::string','std::string'],
        'remove': ['std::string'],
    }

    __output_type_info__ = {
        'fetch_lookup_table': ['std::map<_keyrange, std::string>'],
        'migrate_some_range': ['rpc::i32'],
        'start_migration': ['rpc::i32'],
        'finish_migration': ['rpc::i32'],
        'leave': [],
        'debug_print': [],
        'get': ['rpc::i32','std::string'],
        'put': ['rpc::i32'],
        'remove': ['rpc::i32'],
    }

    def __bind_helper__(self, func):
        def f(*args):
            return getattr(self, func.__name__)(*args)
        return f

    def __reg_to__(self, server):
        server.__reg_func__(_ChordService.FETCH_LOOKUP_TABLE, self.__bind_helper__(self.fetch_lookup_table), [], ['std::map<_keyrange, std::string>'])
        server.__reg_func__(_ChordService.MIGRATE_SOME_RANGE, self.__bind_helper__(self.migrate_some_range), ['std::string'], ['rpc::i32'])
        server.__reg_func__(_ChordService.START_MIGRATION, self.__bind_helper__(self.start_migration), ['_keyrange'], ['rpc::i32'])
        server.__reg_func__(_ChordService.FINISH_MIGRATION, self.__bind_helper__(self.finish_migration), ['_keyrange'], ['rpc::i32'])
        server.__reg_func__(_ChordService.LEAVE, self.__bind_helper__(self.leave), [], [])
        server.__reg_func__(_ChordService.DEBUG_PRINT, self.__bind_helper__(self.debug_print), [], [])
        server.__reg_func__(_ChordService.GET, self.__bind_helper__(self.get), ['std::string'], ['rpc::i32','std::string'])
        server.__reg_func__(_ChordService.PUT, self.__bind_helper__(self.put), ['std::string','std::string'], ['rpc::i32'])
        server.__reg_func__(_ChordService.REMOVE, self.__bind_helper__(self.remove), ['std::string'], ['rpc::i32'])

    def fetch_lookup_table(__self__):
        raise NotImplementedError('subclass _ChordService and implement your own fetch_lookup_table function')

    def migrate_some_range(__self__, dst_addr):
        raise NotImplementedError('subclass _ChordService and implement your own migrate_some_range function')

    def start_migration(__self__, in0):
        raise NotImplementedError('subclass _ChordService and implement your own start_migration function')

    def finish_migration(__self__, in0):
        raise NotImplementedError('subclass _ChordService and implement your own finish_migration function')

    def leave(__self__):
        raise NotImplementedError('subclass _ChordService and implement your own leave function')

    def debug_print(__self__):
        raise NotImplementedError('subclass _ChordService and implement your own debug_print function')

    def get(__self__, key):
        raise NotImplementedError('subclass _ChordService and implement your own get function')

    def put(__self__, key, value):
        raise NotImplementedError('subclass _ChordService and implement your own put function')

    def remove(__self__, key):
        raise NotImplementedError('subclass _ChordService and implement your own remove function')

class _ChordProxy(object):
    def __init__(self, clnt):
        self.__clnt__ = clnt

    def async_fetch_lookup_table(__self__):
        return __self__.__clnt__.async_call(_ChordService.FETCH_LOOKUP_TABLE, [], _ChordService.__input_type_info__['fetch_lookup_table'], _ChordService.__output_type_info__['fetch_lookup_table'])

    def async_migrate_some_range(__self__, dst_addr):
        return __self__.__clnt__.async_call(_ChordService.MIGRATE_SOME_RANGE, [dst_addr], _ChordService.__input_type_info__['migrate_some_range'], _ChordService.__output_type_info__['migrate_some_range'])

    def async_start_migration(__self__, in0):
        return __self__.__clnt__.async_call(_ChordService.START_MIGRATION, [in0], _ChordService.__input_type_info__['start_migration'], _ChordService.__output_type_info__['start_migration'])

    def async_finish_migration(__self__, in0):
        return __self__.__clnt__.async_call(_ChordService.FINISH_MIGRATION, [in0], _ChordService.__input_type_info__['finish_migration'], _ChordService.__output_type_info__['finish_migration'])

    def async_leave(__self__):
        return __self__.__clnt__.async_call(_ChordService.LEAVE, [], _ChordService.__input_type_info__['leave'], _ChordService.__output_type_info__['leave'])

    def async_debug_print(__self__):
        return __self__.__clnt__.async_call(_ChordService.DEBUG_PRINT, [], _ChordService.__input_type_info__['debug_print'], _ChordService.__output_type_info__['debug_print'])

    def async_get(__self__, key):
        return __self__.__clnt__.async_call(_ChordService.GET, [key], _ChordService.__input_type_info__['get'], _ChordService.__output_type_info__['get'])

    def async_put(__self__, key, value):
        return __self__.__clnt__.async_call(_ChordService.PUT, [key, value], _ChordService.__input_type_info__['put'], _ChordService.__output_type_info__['put'])

    def async_remove(__self__, key):
        return __self__.__clnt__.async_call(_ChordService.REMOVE, [key], _ChordService.__input_type_info__['remove'], _ChordService.__output_type_info__['remove'])

    def sync_fetch_lookup_table(__self__):
        __result__ = __self__.__clnt__.sync_call(_ChordService.FETCH_LOOKUP_TABLE, [], _ChordService.__input_type_info__['fetch_lookup_table'], _ChordService.__output_type_info__['fetch_lookup_table'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

    def sync_migrate_some_range(__self__, dst_addr):
        __result__ = __self__.__clnt__.sync_call(_ChordService.MIGRATE_SOME_RANGE, [dst_addr], _ChordService.__input_type_info__['migrate_some_range'], _ChordService.__output_type_info__['migrate_some_range'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

    def sync_start_migration(__self__, in0):
        __result__ = __self__.__clnt__.sync_call(_ChordService.START_MIGRATION, [in0], _ChordService.__input_type_info__['start_migration'], _ChordService.__output_type_info__['start_migration'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

    def sync_finish_migration(__self__, in0):
        __result__ = __self__.__clnt__.sync_call(_ChordService.FINISH_MIGRATION, [in0], _ChordService.__input_type_info__['finish_migration'], _ChordService.__output_type_info__['finish_migration'])
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

    def sync_debug_print(__self__):
        __result__ = __self__.__clnt__.sync_call(_ChordService.DEBUG_PRINT, [], _ChordService.__input_type_info__['debug_print'], _ChordService.__output_type_info__['debug_print'])
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

    def sync_remove(__self__, key):
        __result__ = __self__.__clnt__.sync_call(_ChordService.REMOVE, [key], _ChordService.__input_type_info__['remove'], _ChordService.__output_type_info__['remove'])
        if __result__[0] != 0:
            raise Exception("RPC returned non-zero error code %d: %s" % (__result__[0], os.strerror(__result__[0])))
        if len(__result__[1]) == 1:
            return __result__[1][0]
        elif len(__result__[1]) > 1:
            return __result__[1]

