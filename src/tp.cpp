#include "twopc/tp.hpp"
#include "twopc/srvapp.hpp"

namespace twopc {

using networking::Buffer;

TP::TP(std::string tag, Config& config)
    :handler_(nullptr), tag_(tag), networking_(tag, config), stop_(false){
}

Status TP::register_handler(SrvApp* handler) {
  if (handler_) {
    return Status(Status::Code::HANDLER_EXIST);
  }
  handler_ = handler;
  return Status();
}

Status TP::loop() {
  Status st;
  while (!stop_) {
    Trans::SubTrans subtrans;
    st = catch_subtrans(&subtrans);
    if (!st.ok()) {
      return st;
    }
    LOG(DEBUG) << "Catch subtrans " << subtrans.tid() << std::endl;

    bool v = handler_->excute_callback(subtrans);
    
    st = vote(v, subtrans.tid());
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

Status TP::stop() {
  stop_ = true;
  return Status();
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

Status TP::vote(bool v, TID tid) {
  Status st;
  Buffer buffer = Vote(v, tid).toBuffer();

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
