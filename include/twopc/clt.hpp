#ifndef __TWOPC_CLIENT_HPP__
#define __TWOPC_CLIENT_HPP__

#include "twopc/status.hpp"
#include "twopc/trans.hpp"
#include "twopc/vote.hpp"
#include "twopc/networking/buffer.hpp"
#include "twopc/networking/channel.hpp"
#include "common/all.hpp"

#include <unordered_map>

namespace twopc {

using networking::Buffer;
using networking::Asio;
using networking::Channel;

class Request;

/**
 * Client connects to TC
 */
class Clt : public Asio {
  public:

    enum class TransState {
      SENDING,
      FINISHED,
    };

    Clt(std::string addr);

    /**
     * async excute_trans, call request.wait to get result
     */
    Request excute_trans(Trans& trans);

    void on_send_complete(Channel* ch);
    void on_recv_complete(Channel* ch, Buffer buffer);
    void on_channel_close(Channel* ch);

    friend class Request;

  private:
    Channel            *ch_;
    TID                count_;
    COMMON::Mutex      mutex_;
    COMMON::Condition  cond_;
    std::unordered_map<TID, TransState> trans_states_;
    std::unordered_map<TID, VoteResult> trans_results_;
};


class Request {
  public:

    Request(Clt* clt, TID tid);
    Request(const Request& o);

    Status wait(VoteResult* rst);

  private:
    Clt* clt_;
    TID  tid_;
};

}

#endif
