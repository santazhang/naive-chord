#pragma once

#include "rpc/server.h"
#include "rpc/client.h"

#include <errno.h>


namespace dht {

class ChordService: public rpc::Service {
public:
    enum {
        PUT = 0x1b4436d3,
        GET = 0x2fdc2dd7,
        REMOVE = 0x652d2bbb,
        FIND_SUCCESSOR = 0x37c92d2a,
        GET_PREDECESSOR = 0x6eaf782b,
        NOTIFY = 0x56c96b9d,
        PING = 0x2d2aab1d,
    };
    int __reg_to__(rpc::Server* svr) {
        int ret = 0;
        if ((ret = svr->reg(PUT, this, &ChordService::__put__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(GET, this, &ChordService::__get__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(REMOVE, this, &ChordService::__remove__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(FIND_SUCCESSOR, this, &ChordService::__find_successor__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(GET_PREDECESSOR, this, &ChordService::__get_predecessor__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(NOTIFY, this, &ChordService::__notify__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(PING, this, &ChordService::__ping__wrapper__)) != 0) {
            goto err;
        }
        return 0;
    err:
        svr->unreg(PUT);
        svr->unreg(GET);
        svr->unreg(REMOVE);
        svr->unreg(FIND_SUCCESSOR);
        svr->unreg(GET_PREDECESSOR);
        svr->unreg(NOTIFY);
        svr->unreg(PING);
        return ret;
    }
    // these RPC handler functions need to be implemented by user
    // for 'raw' handlers, remember to reply req, delete req, and sconn->release(); use sconn->run_async for heavy job
    virtual void put(const std::string& key, const std::string& value, rpc::DeferredReply* defer) = 0;
    virtual void get(const std::string& key, std::string* value, rpc::i8* ok, rpc::DeferredReply* defer) = 0;
    virtual void remove(const std::string& key, rpc::i8* ok, rpc::DeferredReply* defer) = 0;
    virtual void find_successor(const dht::BigId& id, dht::host_port* addr, rpc::DeferredReply* defer) = 0;
    virtual void get_predecessor(dht::host_port* addr) = 0;
    virtual void notify(const dht::host_port& maybe_pred) = 0;
    virtual void ping(const dht::host_port& sender) = 0;
private:
    void __put__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        std::string* in_0 = new std::string;
        req->m >> *in_0;
        std::string* in_1 = new std::string;
        req->m >> *in_1;
        auto __marshal_reply__ = [=] {
        };
        auto __cleanup__ = [=] {
            delete in_0;
            delete in_1;
        };
        rpc::DeferredReply* __defer__ = new rpc::DeferredReply(req, sconn, __marshal_reply__, __cleanup__);
        this->put(*in_0, *in_1, __defer__);
    }
    void __get__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        std::string* in_0 = new std::string;
        req->m >> *in_0;
        std::string* out_0 = new std::string;
        rpc::i8* out_1 = new rpc::i8;
        auto __marshal_reply__ = [=] {
            *sconn << *out_0;
            *sconn << *out_1;
        };
        auto __cleanup__ = [=] {
            delete in_0;
            delete out_0;
            delete out_1;
        };
        rpc::DeferredReply* __defer__ = new rpc::DeferredReply(req, sconn, __marshal_reply__, __cleanup__);
        this->get(*in_0, out_0, out_1, __defer__);
    }
    void __remove__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        std::string* in_0 = new std::string;
        req->m >> *in_0;
        rpc::i8* out_0 = new rpc::i8;
        auto __marshal_reply__ = [=] {
            *sconn << *out_0;
        };
        auto __cleanup__ = [=] {
            delete in_0;
            delete out_0;
        };
        rpc::DeferredReply* __defer__ = new rpc::DeferredReply(req, sconn, __marshal_reply__, __cleanup__);
        this->remove(*in_0, out_0, __defer__);
    }
    void __find_successor__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        dht::BigId* in_0 = new dht::BigId;
        req->m >> *in_0;
        dht::host_port* out_0 = new dht::host_port;
        auto __marshal_reply__ = [=] {
            *sconn << *out_0;
        };
        auto __cleanup__ = [=] {
            delete in_0;
            delete out_0;
        };
        rpc::DeferredReply* __defer__ = new rpc::DeferredReply(req, sconn, __marshal_reply__, __cleanup__);
        this->find_successor(*in_0, out_0, __defer__);
    }
    void __get_predecessor__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        dht::host_port out_0;
        this->get_predecessor(&out_0);
        sconn->begin_reply(req);
        *sconn << out_0;
        sconn->end_reply();
        delete req;
        sconn->release();
    }
    void __notify__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        dht::host_port in_0;
        req->m >> in_0;
        this->notify(in_0);
        sconn->begin_reply(req);
        sconn->end_reply();
        delete req;
        sconn->release();
    }
    void __ping__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        dht::host_port in_0;
        req->m >> in_0;
        this->ping(in_0);
        sconn->begin_reply(req);
        sconn->end_reply();
        delete req;
        sconn->release();
    }
};

class ChordProxy {
protected:
    rpc::Client* __cl__;
public:
    ChordProxy(rpc::Client* cl): __cl__(cl) { }
    rpc::Future* async_put(const std::string& key, const std::string& value, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(ChordService::PUT, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << key;
            *__cl__ << value;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 put(const std::string& key, const std::string& value) {
        rpc::Future* __fu__ = this->async_put(key, value);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_get(const std::string& key, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(ChordService::GET, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << key;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 get(const std::string& key, std::string* value, rpc::i8* ok) {
        rpc::Future* __fu__ = this->async_get(key);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *value;
            __fu__->get_reply() >> *ok;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_remove(const std::string& key, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(ChordService::REMOVE, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << key;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 remove(const std::string& key, rpc::i8* ok) {
        rpc::Future* __fu__ = this->async_remove(key);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *ok;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_find_successor(const dht::BigId& id, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(ChordService::FIND_SUCCESSOR, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << id;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 find_successor(const dht::BigId& id, dht::host_port* addr) {
        rpc::Future* __fu__ = this->async_find_successor(id);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *addr;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_get_predecessor(const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(ChordService::GET_PREDECESSOR, __fu_attr__);
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 get_predecessor(dht::host_port* addr) {
        rpc::Future* __fu__ = this->async_get_predecessor();
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *addr;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_notify(const dht::host_port& maybe_pred, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(ChordService::NOTIFY, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << maybe_pred;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 notify(const dht::host_port& maybe_pred) {
        rpc::Future* __fu__ = this->async_notify(maybe_pred);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_ping(const dht::host_port& sender, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(ChordService::PING, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << sender;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 ping(const dht::host_port& sender) {
        rpc::Future* __fu__ = this->async_ping(sender);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
};

} // namespace dht



