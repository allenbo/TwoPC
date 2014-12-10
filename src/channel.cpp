#include "twopc/networking/channel.hpp"
#include "twopc/networking/monitor.hpp"
#include "common/all.hpp"

using namespace COMMON;

namespace twopc {
namespace networking {

Channel::Channel(bool async)
    :socket_(-1), addrinfo_(nullptr), alive_(false),
     async_(async), handler_(nullptr),
     read_packet_(nullptr), write_packet_(nullptr),
     async_read_(false), read_waiter_(0),
     async_read_mutex_(), async_read_cond_(&async_read_mutex_),
     async_write_(false), write_waiter_(0),
     async_write_mutex_(), async_write_cond_(&async_write_mutex_) {
}

Channel::Channel(std::string addr, bool async)
    :socket_(-1), addrinfo_(nullptr), alive_(false),
     async_(async), handler_(nullptr),
     read_packet_(nullptr), write_packet_(nullptr),
     async_read_(false), read_waiter_(0),
     async_read_mutex_(), async_read_cond_(&async_read_mutex_),
     async_write_(false), write_waiter_(0),
     async_write_mutex_(), async_write_cond_(&async_write_mutex_) {

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
  ScopeLock _(&change_mutex_);
  if (socket == -1 || socket_ != -1) {
    return Status(Status::Code::NET_WRONG_SOCKET);
  }
  socket_ = socket;
  return Status();
}

Status Channel::send(const Buffer& buffer) {
  ScopeLock _(&change_mutex_);
  CHECK_NE(socket_, -1);
  CHECK_NE(async_, true);
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
  ScopeLock _(&change_mutex_);
  CHECK_NE(socket_, -1);
  CHECK_NE(async_, true);

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
  buffer->reset();
  return Status();
}

Status Channel::isend(const Buffer& buffer) {
  async_write_mutex_.lock(); // weird scope lock error

  write_waiter_ ++;
  while (async_write_) {
    async_write_cond_.wait();
    if (!is_alive()) {
      async_write_mutex_.unlock();
      return Status(Status::Code::NET_CLOSED);
    }
  }
  write_waiter_ --;

  async_write_ = true;
  write_packet_ = new packet(buffer);

  write_pdu();
  if (write_packet_) {
    Monitor::get_instance()->watch_write(this);
  }
  async_write_mutex_.unlock();
  return Status();
}

Status Channel::irecv() {
  ScopeLock _(&async_read_mutex_);

  read_waiter_ ++;
  while (async_read_) {
    async_read_cond_.wait(); 
    if (!is_alive()) {
      return Status(Status::Code::NET_CLOSED);
    }
  }
  read_waiter_ --;

  async_read_ = true;
  read_packet_ = new packet();

  Monitor::get_instance()->watch_read(this);
  return Status();
}

Status Channel::read_pdu() {
  CHECK_NOTNULL(read_packet_);

  if (read_packet_->size == 0) {
    int total_len;
    int len = ::read(socket_, (char*)&total_len, sizeof(int));

    if (len == 0){
      on_async_close();
      return Status(Status::Code::NET_CLOSED);
    }

    read_packet_->set_size(total_len);
    LOG(DEBUG) << read_packet_->size << std::endl;
  }

  size_t len = ::read(socket_, read_packet_->curr_bytes, read_packet_->remain_size());
  if (len == 0){
    on_async_close();
    return Status(Status::Code::NET_CLOSED);
  }
  read_packet_->curr_bytes += len;
  LOG(DEBUG) << "len " << len << " remain_size " << read_packet_->remain_size() << std::endl;

  if (read_packet_->ready()) {
    Buffer buffer = read_packet_->toBuffer();

    Monitor::get_instance()->unwatch_read(this);
    cleanup_read();

    if (handler_) {
      handler_->on_recv_complete(this, buffer);
    }
  }
  return Status();
}

Status Channel::write_pdu() {
  CHECK_NOTNULL(write_packet_);

  size_t len = ::write(socket_, write_packet_->curr_bytes, write_packet_->remain_size());
  write_packet_->curr_bytes += len;

  if (write_packet_->ready()) {
    Monitor::get_instance()->unwatch_write(this);
    LOG(DEBUG) << "clean up write" << std::endl;
    cleanup_write();
    LOG(DEBUG) << "after clean up write" << std::endl;

    if (handler_) {
      handler_->on_send_complete(this);
    }
  }
  return Status();
}

void Channel::cleanup_read() {
  CHECK_NOTNULL(read_packet_);
  delete read_packet_;
  read_packet_ = nullptr;

  async_read_ = false;
  async_read_cond_.notify_all();
}

void Channel::cleanup_write() {
  CHECK_NOTNULL(write_packet_);
  delete write_packet_;
  write_packet_ = nullptr;

  async_write_ = false;
  async_write_cond_.notify_all();
}

void Channel::on_async_close() {
  Monitor::get_instance()->unregister_channel(this);
  cleanup_read();

  if (handler_) {
    handler_->on_channel_close(this);
  }
}

Status Channel::connect() {
  /* if reconnect */
  ScopeLock _(&change_mutex_);
  if (socket_ != -1) {
    ::close(socket_);
    socket_ = -1;
  }

  if (addrinfo_ == nullptr) {
    return Status(Status::Code::NET_NOT_ALLOWED);
  }

  struct addrinfo * ptr = nullptr;
  for(ptr = addrinfo_; ptr != nullptr; ptr = ptr->ai_next) {
    socket_ = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
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
  ScopeLock _(&change_mutex_);
  if (alive_ && socket_ != -1) {
    ::close(socket_);
  }

  if (async_) {
    Monitor::get_instance()->unregister_channel(this);
  }

  alive_ = false;
  socket_ = -1;

}

void Channel::set_async_handler(Asio* handler) {
  CHECK(async_);
  ScopeLock _(&change_mutex_);
  nonblock_fd(socket_);
  handler_ = handler;
  Monitor::get_instance()->register_channel(this);
}

} // end of namespace networking
} // end of namesapce twopc
