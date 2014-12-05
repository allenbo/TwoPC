#ifndef __TWOPC_NETWORKING_ACCEPTOR_HPP__
#define __TWOPC_NETWORKING_ACCEPTOR_HPP__

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "twopc/status.hpp"
#include "twopc/networking/channel.hpp"

namespace twopc {
namespace networking {

class Acceptor {
  public:

    Acceptor(std::string addr);
    ~Acceptor();

    Status accept(Channel* ch);

  private:
    int socket_;
    struct addrinfo* addrinfo_;
};

} // end of namespace networking
} // end of namespace twopc

#endif
