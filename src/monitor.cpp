#include "twopc/networking/monitor.hpp"
#include "common/all.hpp"
#include <vector>

using namespace COMMON;

namespace twopc {
namespace networking {

Monitor* Monitor::instance_ = nullptr;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

void init_monitor() {
  Monitor::instance_ = new Monitor;
}

Monitor::Monitor() 
    :poll_manager_(nullptr) {
  poll_manager_ = new SelectPoll();
  start();
}

Monitor* Monitor::get_instance() {
  pthread_once(&init_once, init_monitor);
  return instance_;
}

Monitor::~Monitor() {
  delete poll_manager_;
}

Status Monitor::register_channel(Channel* ch) {
  LOG(DEBUG) << "register channel " << ch->socket() << std::endl;
  if (channels_.count(ch->socket()) != 0) {
    return Status(Status::Code::MAP_KEY_DUPLICATE);
  }
  channels_[ch->socket()] = ch;
  return Status();
}

Status Monitor::unregister_channel(Channel* ch) {
  LOG(DEBUG) << "unregister channel " << ch->socket() << std::endl;
  if (channels_.count(ch->socket()) == 0) {
    return Status(Status::Code::MAP_KEY_MISSING);
  }
  poll_manager_->unwatch(ch->socket(), Pollable::MODE::READ);
  poll_manager_->unwatch(ch->socket(), Pollable::MODE::WRITE);

  channels_.erase(ch->socket());
  return Status();
}

Status Monitor::watch_read(Channel* ch) {
  if (channels_.count(ch->socket()) == 0) {
    return Status(Status::Code::MAP_KEY_MISSING);
  }
  poll_manager_->watch(ch->socket(), Pollable::MODE::READ);
  return Status();
}

Status Monitor::watch_write(Channel* ch) {
  if (channels_.count(ch->socket()) == 0) {
    return Status(Status::Code::MAP_KEY_MISSING);
  }
  poll_manager_->watch(ch->socket(), Pollable::MODE::WRITE);
  return Status();
}

Status Monitor::unwatch_read(Channel* ch) {
  if (channels_.count(ch->socket()) == 0) {
    return Status(Status::Code::MAP_KEY_MISSING);
  }
  poll_manager_->unwatch(ch->socket(), Pollable::MODE::READ);
  return Status();
}

Status Monitor::unwatch_write(Channel* ch) {
  if (channels_.count(ch->socket()) == 0) {
    return Status(Status::Code::MAP_KEY_MISSING);
  }
  poll_manager_->unwatch(ch->socket(), Pollable::MODE::WRITE);
  return Status();
}

void Monitor::run() {
  std::vector<int> read_fds;
  std::vector<int> write_fds;

  while (true) {
    if (channels_.size() == 0) {
      struct timespec ts;
      ts.tv_sec = 0;
      ts.tv_nsec = 1000; // 1us
      nanosleep(&ts, NULL);
      continue;
    }

    read_fds.clear();
    write_fds.clear();

    CHECK(poll_manager_->poll(read_fds, write_fds));

    auto it = read_fds.begin();
    for(; it != read_fds.end(); it ++) {
      if (channels_.count(*it) == 0) {
        continue;
      }
      LOG(DEBUG) << *it << " ready for read" << std::endl;
      channels_[*it]->read_pdu();
    }

    for(it = write_fds.begin(); it != write_fds.end(); it ++ ) {
      if (channels_.count(*it) == 0) {
        continue;
      }
      LOG(DEBUG) << *it << " ready for write" << std::endl;
      channels_[*it]->write_pdu();
    }
  }
}

} // end of networking
} // end of twopc 
