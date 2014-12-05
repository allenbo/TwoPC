#ifndef __TWOPC_NETWORKING_CHANNEL_HPP__
#define __TWOPC_NETWORKING_CHANNEL_HPP__

#include "twopc/status.hpp"
#include "twopc/networking/buffer.hpp"

#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <string.h>

namespace twopc {
namespace networking {

class Channel {
  public:

    struct packet {
      packet(const Buffer& buffer) {
        bytes = new char[buffer.size() + sizeof(int)];
        *(int*)bytes = buffer.size();
        memcpy(bytes + sizeof(int), buffer.data(), buffer.size());
        size = buffer.size() + sizeof(int);
        curr_bytes = bytes;
      }
      ~packet() {
        delete [] bytes;
        bytes = nullptr;
        curr_bytes = nullptr;
        size = 0;
      }

      inline size_t remain_size() {
        return size - (curr_bytes - bytes);
      }

      char*  bytes;
      char*  curr_bytes;
      size_t size;
    };

    Channel();
    Channel(std::string addr);
    ~Channel();

    Status set_socket(int socket);

    Status send(const Buffer& buffer);
    Status recv(Buffer* buffer);

    Status connect();
    void close();

    inline bool is_alive() { return alive_; }
    inline void set_alive() { alive_ = true; }

  private:

    int socket_;
    struct addrinfo* addrinfo_;
    bool alive_;
};

} // end of namespace networking
} // end of namespace twopc

#endif
