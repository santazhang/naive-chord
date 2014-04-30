#include <signal.h>

#include "base/all.h"
#include "chord.h"

using namespace base;
using namespace rpc;
using namespace std;
using namespace dht;

static bool g_stop_flag = false;

static void signal_handler(int sig) {
    if (g_stop_flag == false) {
        Log::info("caught signal %d, stopping server now", sig);
        g_stop_flag = true;
    } else {
        Log::info("caught signal %d for the second time, kill server now", sig);
        exit(-sig);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <bind-addr> [join-addr]\n", argv[0]);
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    signal(SIGALRM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);

    string my_addr = argv[1];
    string bind_addr = my_addr;
    // on ec2, we have to bind at 0.0.0.0 to use both external and internal IP
    bind_addr = "0.0.0.0" + bind_addr.substr(bind_addr.find(":"));

    Server *server = new Server;
    Chord* chord_svc;
    if (argc < 3) {
        chord_svc = Chord::create(my_addr);
    } else {
        string join_addr = argv[2];
        chord_svc = Chord::join(my_addr, join_addr);
    }

    int ret_code = 0;
    if (server->start(bind_addr.c_str()) != 0) {
        ret_code = 1;
        g_stop_flag = true;
    }

    while (g_stop_flag == false) {
        sleep(1);
    }
    delete server;
    delete chord_svc;

    return ret_code;
}
