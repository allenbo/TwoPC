#include "twopc/config.hpp"

namespace twopc {

Config::Config() {
}

Config::Config(Config&& o)
    :coord_addr_(o.coord_addr_),
     part_addrs_(std::move(o.part_addrs_)) {
}

Config::Config(const Config& o)
    :coord_addr_(o.coord_addr_),
     part_addrs_(o.part_addrs_) {
}

Status Config::add_part(std::string tag, std::string addr) {
  if (part_addrs_.count(tag) != 0) {
    return Status(Status::Code::MAP_KEY_DUPLICATE);
  }

  if (addr.find(':') == std::string::npos) {
    return Status(Status::Code::NET_BAD_ADDR);
  }

  part_addrs_[tag] = addr;
  return Status();
}

Status Config::set_coord(std::string addr) {
  coord_addr_ = addr;
  return Status();
}

std::vector<std::string> Config::tags() {
  std::vector<std::string> rst;
  auto it = part_addrs_.begin();

  for(; it != part_addrs_.end(); it ++) {
    rst.push_back(it->first);
  }
  return rst;
}

}
