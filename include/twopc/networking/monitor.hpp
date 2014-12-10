#ifndef __TWOPC_NETWORKING_MONITOR_HPP__
#define __TWOPC_NETWORKING_MONITOR_HPP__

#include "twopc/status.hpp"
#include "twopc/networking/channel.hpp"
#include "twopc/networking/pollable.hpp"
#include "common/all.hpp"
#include <string>
#include <map>

namespace twopc {
namespace networking {

class Monitor: public COMMON::Thread {
  public:
    static Monitor* get_instance();
    ~Monitor();

    Status register_channel(Channel* ch);
    Status unregister_channel(Channel* ch);

    Status watch_read(Channel* ch);
    Status watch_write(Channel* ch);
    Status unwatch_read(Channel* ch);
    Status unwatch_write(Channel* ch);

    void run();

  private:

    Monitor();
    Monitor(const Monitor& o);
    Monitor& operator=(const Monitor& o);

    std::map<int, Channel*> channels_;
    static Monitor *instance_;
    Pollable* poll_manager_;

    RecursiveMutex mutex_;

    friend void init_monitor();
};

} // end of networking
} // end of twopc

#endif
