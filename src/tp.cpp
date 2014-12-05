#include "twopc/tp.hpp"
#include "twopc/srvapp.hpp"

namespace twopc {

using networking::Buffer;

TP::TP(std::string tag, Config& config)
    :handler_(nullptr), tag_(tag), networking_(tag, config) {
}

Status TP::loop() {
  Status st;
  while (true) {
    Trans::SubTrans subtrans;
    st = catch_subtrans(&subtrans);
    if (!st.ok()) {
      return st;
    }

    bool v = handler_->excute_callback(subtrans);
    
    st = vote(v);
    if (!st.ok()) {
      return st;
    }

    VoteResult rst;
    st = catch_result(&rst);
    if (!st.ok()) {
      return st;
    }

    if(rst.commit()) {
      handler_->commit_callback(subtrans);
    } else {
      handler_->abort_callback(subtrans);
    }
  }
  return st;
}

Status TP::catch_subtrans(Trans::SubTrans* trans) {
  Status st;
  Buffer buffer;

  st = networking_.recv(&buffer);
  if (!st.ok()) {
    return st;
  }

  st = trans->fromBuffer(buffer);
  return st;
}

Status TP::vote(bool v) {
  Status st;
  Buffer buffer = Vote(v).toBuffer();

  st = networking_.send(buffer);
  return st; 
}

Status TP::catch_result(VoteResult* rst) {
  Status st;
  Buffer buffer;
  st = networking_.recv(&buffer);
  if (!st.ok()) {
    return st;
  }

  st = rst->fromBuffer(buffer);
  return st;
}

}
