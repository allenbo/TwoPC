#include "twopc/networking/channel.hpp"
#include "common/all.hpp"

using namespace COMMON;

namespace twopc {
namespace networking {

Channel::Channel()
    :socket_(-1), addrinfo_(nullptr), alive_(false) {
}

Channel::Channel(std::string addr)
    :socket_(-1), addrinfo_(nullptr), alive_(false) {
  std::vector<std::string> pair = VarString::split(addr, ":");
  CHECK_EQ(pair.size(), 2);
  CHECK_NE(pair[0], "");
  CHECK_NE(pair[1], "");

  std::string host = pair[0];
  std::string port = pair[1];

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  CHECK_EQ(getaddrinfo(host.c_str(),
                       port.c_str(),
                       &hints,
                       &addrinfo_),
           0);
  Status st = connect();
  CHECK(st.ok());
  alive_ = true;
}

Channel::~Channel() {
  if (addrinfo_)
    freeaddrinfo(addrinfo_);
  
  if (alive_) {
    close();
  }
}

Status Channel::set_socket(int socket) {
  if (socket == -1 || socket_ != -1) {
    return Status(Status::Code::NET_WRONG_SOCKET);
  }
  socket_ = socket;
  return Status();
}

Status Channel::send(const Buffer& buffer) {
  CHECK_NE(socket_, -1);
  packet p(buffer);
  
  while(true) {
    int len = ::write(socket_, p.curr_bytes, p.remain_size());
    if ((size_t)len != p.remain_size()) {
      p.curr_bytes += len;
    } else {
      break;
    }
  }

  return Status();
}

Status Channel::recv(Buffer* buffer) {
  CHECK_NE(socket_, -1);
  int total_len;
  int len = ::read(socket_, (char*)&total_len, sizeof(int));

  if (len == 0){
    return Status(Status::Code::NET_CLOSED);
  }

  int curr_len = 0;
  char bytes[1024];

  while (curr_len < total_len) {
    len = ::read(socket_, (char*)bytes, 1024);
    if (len == 0) {
      return Status(Status::Code::NET_CLOSED);
    }
    buffer->write((Buffer::Byte*)bytes, len);

    curr_len += len;
  }
  return Status();
}

Status Channel::connect() {
  /* if reconnect */
  if (socket_ != -1) {
    ::close(socket_);
    socket_ = -1;
  }

  struct addrinfo * ptr = nullptr;
  for(ptr = addrinfo_; ptr != nullptr; ptr = ptr->ai_next) {
    socket_ = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (socket_ == -1) {
      continue;
    }
    if (::connect(socket_, ptr->ai_addr, (int)ptr->ai_addrlen) == 0) {
      break;
    }
    ::close(socket_);
  }

  if (ptr == nullptr) {
    return Status(Status::Code::NET_CNT_FAIL);
  }
  return Status();
}

void Channel::close() {
  if (alive_ && socket_ != -1) {
    ::close(socket_);
  }
  alive_ = false;
  socket_ = -1;
}

} // end of namespace networking
} // end of namesapce twopc
