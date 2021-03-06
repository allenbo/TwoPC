#include "twopc/networking/acceptor.hpp"
#include "common/all.hpp"
#include <string.h>

using namespace COMMON;

namespace twopc {
namespace networking {

Acceptor::Acceptor(std::string addr)
    :socket_(-1), addrinfo_(nullptr) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  CHECK_EQ(getaddrinfo(nullptr, addr.c_str(), &hints, &addrinfo_), 0);

  socket_ = ::socket(addrinfo_->ai_family, addrinfo_->ai_socktype, addrinfo_->ai_protocol);
  CHECK_NE(socket_, -1);

  int optval = 1;
  setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

  CHECK_EQ(::bind(socket_, addrinfo_->ai_addr, addrinfo_->ai_addrlen), 0);
  CHECK_EQ(::listen(socket_, 128), 0);
  LOG(DEBUG) << "Listening at port " << addr << std::endl;
}

Acceptor::~Acceptor() {
  if (addrinfo_)
    freeaddrinfo(addrinfo_);
}

Status Acceptor::accept(Channel* ch) {
  struct sockaddr_in client_info;
  socklen_t client_len = sizeof(struct sockaddr_in);
  int client_sock = ::accept(socket_, (struct sockaddr*) &client_info, &client_len);
  LOG(DEBUG) << "Accepted a new channel " << client_sock << std::endl;

  ch->set_socket(client_sock);
  ch->set_alive();
  return Status();
}

} // end of networking
} // end of twopc
