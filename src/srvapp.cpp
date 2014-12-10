#include "twopc/srvapp.hpp"

namespace twopc {

SrvApp::SrvApp(std::string tag, Config& config) {
  tp_ = new TP(tag, config);
  tp_->register_handler(this);
}

SrvApp::~SrvApp() {
  tp_->stop();
  delete tp_;
}

bool SrvApp::excute_callback(Trans::SubTrans& trans) {
  LOG(DEBUG) << " * excute " << trans.tid() << " @ " << trans.tag() << std::endl;
  return true;
}

bool SrvApp::commit_callback(Trans::SubTrans& trans) {
  LOG(DEBUG) << " * commit " << trans.tid() << " @ " << trans.tag() << std::endl;
  return true;
}

bool SrvApp::abort_callback(Trans::SubTrans& trans) {
  LOG(DEBUG) << " * abort " << trans.tid() << " @ " << trans.tag() << std::endl;
  return true;
}

}
