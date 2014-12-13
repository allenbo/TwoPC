#ifndef __TWOPC_SRV_HPP__
#define __TWOPC_SRV_HPP__

#include "twopc/tc.hpp"
#include "twopc/config.hpp"
#include "twopc/status.hpp"
#include "twopc/networking/channel.hpp"
#include "twopc/networking/acceptor.hpp"
#include "twopc/networking/monitor.hpp"

#include "common/all.hpp"

#include <string>

namespace twopc {

using networking::Buffer;
using networking::Acceptor;
using networking::Channel;
using networking::Asio;

/* server, will start with a tc
 */
class Srv : public Asio {
  public:

    Srv(Config& config, std::string addr);
    ~Srv();

    /**
     * Callback of isend
     */
    void on_send_complete(Channel* ch);

    /**
     * Callback of irecv
     */
    void on_recv_complete(Channel* ch, Buffer buffer);

    /**
     * Callback of channel close
     */
    void on_channel_close(Channel* ch);

    /**
     * Server loop
     */
    void loop();

    /**
     * Server stop
     */
    void stop();

    /**
     * set key to tag
     */
    Status set_key2tag(Trans::KeyToTag key2tag) { return tc_->set_key2tag(key2tag); }

  private:

    std::map<int, Channel*> channels_;
    Acceptor *acceptor_;
    bool     stop_;

    TC       *tc_;

    COMMON::Mutex change_mutex_;
};

}

#endif
