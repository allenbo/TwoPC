#include "twopc/clt.hpp"

using namespace COMMON;

namespace twopc {

Clt::Clt(std::string addr)
    :ch_(nullptr), count_(0), mutex_(), cond_(&mutex_) {
  ch_ = new Channel(addr, true);
  ch_->set_async_handler(this);
}

Request Clt::excute_trans(Trans& trans) {
  CHECK(trans.tid() == 0);
  ScopeLock _(&mutex_);
  count_ ++;
  trans.set_tid(count_);

  trans_states_[count_] = TransState::SENDING;
  Request req(this, count_);

  ch_->isend(trans.toBuffer());

  return req;
}

void Clt::on_send_complete(Channel* ch) {
  ch_->irecv();
}

void Clt::on_recv_complete(Channel* ch, Buffer buffer) {
  ScopeLock _(&mutex_);
  VoteResult rst;
  rst.fromBuffer(buffer);

  LOG(DEBUG) << "Get vote result for " << rst.tid() << ":" << rst.commit() << std::endl;
  trans_states_[rst.tid()] = TransState::FINISHED;
  trans_results_[rst.tid()] = rst;
  cond_.notify_all();
}

void Clt::on_channel_close(Channel* ch) {
  Status st = ch->connect();
  CHECK(st.ok());
}

Request::Request(Clt* clt, TID tid)
    :clt_(clt), tid_(tid) {
}

Request::Request(const Request& o)
    :clt_(o.clt_), tid_(o.tid_) {
}

Status Request::wait(VoteResult* rst) {
  ScopeLock _(&clt_->mutex_);

  while (clt_->trans_results_.count(tid_) == 0) {
    clt_->cond_.wait();
  }

  *rst = clt_->trans_results_[tid_];
  CHECK_EQ(tid_, rst->tid());
  clt_->trans_states_.erase(tid_);
  clt_->trans_results_.erase(tid_);
  LOG(DEBUG) << "Transaction " << tid_ << " got the result " << rst->commit() << std::endl;
  return Status();
}

}
