#include <string>
#include <iostream>

#include "base/all.h"
#include "marshal-chord.h"
#include "services.h"
#include "rpc/client.h"

using namespace base;
using namespace std;
using namespace rpc;
using namespace dht;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <server-addr>\n", argv[0]);
        exit(1);
    }

    string connect_to = argv[1];
    ClientPool* clnt = new ClientPool;

    ChordProxy proxy(clnt->get_client(connect_to));
    for (;;) {
        cout << "> ";
        string cmd;
        cin >> cmd;
        if (cmd == "exit") {
            break;
        } else if (cmd == "put") {
            string key, value;
            cin >> key >> value;
            Log::debug("id(%s) = %s", key.c_str(), BigId(key).str().c_str());
            proxy.put(key, value);
        } else if (cmd == "get") {
            string key, value;
            cin >> key;
            i8 ok;
            proxy.get(key, &value, &ok);
            Log::debug("get(%s) -> %s, err_code=%d", key.c_str(), value.c_str(), ok);
        } else if (cmd == "del") {
            string key;
            cin >> key;
            i8 ok;
            proxy.remove(key, &ok);
            Log::debug("remove(%s) -> err_code=%d", key.c_str(), ok);
        } else if (cmd == "dump") {
            proxy.dump();
        } else {
            cout << "unknown command: '" << cmd << "', use: put key value, get key, del key, exit" << endl;
        }
    }

    delete clnt;

    return 0;
}
