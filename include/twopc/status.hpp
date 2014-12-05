#ifndef __TWOPC_STATUS_HPP__
#define __TWOPC_STATUS_HPP__

namespace twopc {

class Status {
  public:

    enum class Code {
      OK,

      MAP_KEY_DUPLICATE,

      NET_TIMEOUT,
      NET_RD_ERROR,
      NET_WR_ERROR,
      NET_WRONG_SOCKET,
      NET_WRONG_TAG,
      NET_CNT_FAIL,
      NET_CLOSED,
      NET_BAD_ADDR,

      IO_EOF,
      PROTO_ERROR,
    };

    Status() : code_(Code::OK) {}
    Status(Code code) : code_(code) {}
    Status(const Status& o) : code_(o.code_) {}

    inline bool ok() { return code_ == Code::OK; }

  private:

    Code code_;
};

}

#endif
