#include "twopc/srv.hpp"

namespace twopc {

Srv::Srv(Config& config, std::string addr)
    :channels_(), acceptor_(nullptr), stop_(false),
     tc_(nullptr), change_mutex_() {
  acceptor_ = new Acceptor(addr);
  tc_ = new TC(config);
}

Srv::~Srv() {
  if (stop_) {
    stop();
  }
  delete acceptor_;
  delete tc_;

  auto it = channels_.begin();
  for(; it != channels_.end(); it ++) {
    it->second->close();
    delete it->second;
  }
  channels_.clear();
}

void Srv::stop() {
  COMMON::ScopeLock _(&change_mutex_);
  stop_ = true;
}

void Srv::on_send_complete(Channel* ch) {
  ch->irecv();
}

void Srv::on_recv_complete(Channel* ch, Buffer buffer) {
  Trans trans;
  trans.fromBuffer(buffer);

  VoteResult rst;
  Status st = tc_->excuteTrans(trans, &rst);
  CHECK(st.ok());
  ch->isend(rst.toBuffer());
}

void Srv::on_channel_close(Channel* ch) {
  ch->close();
  channels_.erase(ch->socket());
  delete ch;
}

void Srv::loop() {
  while (!stop_) {
    Channel * ch = new Channel(true); // async
    Status st = acceptor_->accept(ch);
    ch->set_async_handler(this);

    channels_[ch->socket()] = ch;
    ch->irecv();
  }
}

}
