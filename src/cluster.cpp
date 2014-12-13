#include "twopc/networking/cluster.hpp"
#include "common/all.hpp"
#include <algorithm>

using namespace COMMON;

namespace twopc {
namespace networking {

CoordCluster::CoordCluster(Config& config) {
  ready_    = false;
  size_     = config.part_size();
  acceptor_ = new Acceptor(config.coord_addr());
  tags_     = config.tags();
}

CoordCluster::~CoordCluster() {
  delete acceptor_;
  auto it = channels_.begin();
  for(; it != channels_.end(); it ++) {
    delete it->second;
  }
  channels_.clear();
}

Status CoordCluster::init() {
  while (channels_.size() != (size_t)size_) {
    Channel *chan = new Channel();
    Status st = acceptor_->accept(chan);

    Buffer buffer;
    st = chan->recv(&buffer);
    if (!st.ok()) {
      return st;
    }

    std::string tag;
    st = buffer.readString(&tag);
    if (!st.ok()) {
      return st;
    }
    
    CHECK_NE(std::find(tags_.begin(), tags_.end(), tag), tags_.end());
    CHECK_EQ(channels_.count(tag), 0);

    channels_[tag] = chan;
    LOG(DEBUG) << "Cluster received connection from " << tag << std::endl;
  }
  ready_ = true;
  LOG(DEBUG) << "Cluster ready" << std::endl;
  return Status();
}

Status CoordCluster::send(std::string& tag, const Buffer& buffer) {
  if (channels_.count(tag) == 0) {
    return Status(Status::Code::NET_WRONG_TAG);
  }
  Channel& ch = *channels_[tag];
  return ch.send(buffer);
}

Status CoordCluster::recv(std::string& tag, Buffer* buffer) {
  if (channels_.count(tag) == 0) {
    return Status(Status::Code::NET_WRONG_TAG);
  }
  Channel& ch = *channels_[tag];
  return ch.recv(buffer);
}

Status CoordCluster::broadcast(const Buffer& buffer) {
  auto it = channels_.begin();
  Status st;

  for(; it != channels_.end(); it ++) {
    st = it->second->send(buffer);
    if (!st.ok()) {
      return st;
    }
  }

  return st;
}

PartCluster::PartCluster(std::string tag, Config& config)
    :tag_(tag) {
  channel_ = new Channel(config.coord_addr());
  Status st = init();

  CHECK(st.ok());
}

PartCluster::~PartCluster() {
  delete channel_;
}

Status PartCluster::init() {
  Buffer buffer;
  buffer.writeString(tag_);

  return channel_->send(buffer);
}

Status PartCluster::send(const Buffer& buffer) {
  return channel_->send(buffer);
}

Status PartCluster::recv(Buffer* buffer) {
  return channel_->recv(buffer);
}

} // end of networking
} // end of twopc
