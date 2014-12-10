#ifndef __TWOPC_NETWORKING_CLUSTER_HPP__
#define __TWOPC_NETWORKING_CLUSTER_HPP__

#include "twopc/status.hpp"
#include "twopc/config.hpp"

#include "twopc/networking/buffer.hpp"
#include "twopc/networking/channel.hpp"
#include "twopc/networking/acceptor.hpp"

#include <vector>
#include <map>

namespace twopc {
namespace networking {

class CoordCluster {
  public:

    CoordCluster(Config& config);
    ~CoordCluster();

    /**
     * initialize the cluster, wait for participants
     */
    Status init();

    /**
     * Send buffer to remote server based on tag
     */
    Status send(std::string& tag, const Buffer& buffer);

    /**
     * Receive a buffer from remote server
     */
    Status recv(std::string& tag, Buffer* buffer);
    /**
     * Broadcast buffer to all remote server.
     * Only coordinator can call this function
     */
    Status broadcast(const Buffer& buffer);

    inline bool is_ready() { return ready_; }

  private:
    typedef std::map<std::string, Channel*> TTChannel;

    int       size_;
    bool      ready_;
    Acceptor  *acceptor_;
    TTChannel channels_;

    std::vector<std::string> tags_;
};


class PartCluster {
  public:

    PartCluster(std::string tag, Config& config);
    ~PartCluster();

    /**
     * Can only send message to coordinator
     */
    Status send(const Buffer& buffer);

    /**
     * Can only receive a message from coordinator
     */
    Status recv(Buffer* buffer);

  private:

    Status  init();

    std::string tag_;
    Channel     *channel_;

};
} // end of namespace networking
} // end of namespace twopc

#endif
