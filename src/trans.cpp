#include "twopc/trans.hpp"

#include <vector>
#include <map>

namespace twopc {

Trans::SubTrans::SubTrans()
    :tag_(""), tid_(0) {
}

Trans::SubTrans::SubTrans(std::string tag, TID tid)
    :tag_(tag), tid_(tid) {
}

Trans::SubTrans::SubTrans(Trans::SubTrans&& o)
    :tag_(o.tag_), tid_(o.tid_), commands_(std::move(o.commands_)){
}

Trans::SubTrans::SubTrans(const Trans::SubTrans& o)
    :tag_(o.tag_), tid_(o.tid_), commands_(o.commands_) {
}

void Trans::SubTrans::append(std::string command) {
  commands_.push_back(command);
}

Buffer Trans::SubTrans::toBuffer() {
  Buffer buf;

  buf.writeString(tag_); // write tag
  buf.writeULongInt(tid_); // write tid

  buf.writeUInt(commands_.size());

  for(auto& command : commands_) {
    buf.writeString(command);
  }
  return buf;
}

Status Trans::SubTrans::fromBuffer(Buffer& buffer) {
  Status st;

  std::string tag;
  st = buffer.readString(&tag);
  if (!st.ok()) {
    return st;
  }

  TID tid;
  st = buffer.readULongInt(&tid);
  if (!st.ok()) {
    return st;
  }

  size_t size;
  st = buffer.readUInt(&size);
  if (!st.ok()) {
    return st;
  }

  for(size_t i = 0; i < size; i ++ ) {
    std::string command;
    st = buffer.readString(&command);
    if (!st.ok()) {
      return st;
    }
    commands_.push_back(command);
  }

  tag_ = tag;
  tid_ = tid;
  return st;
}

Status Trans::append(std::string tag, std::string command) {
  tags_.push_back(tag);
  commands_.push_back(command);
  return Status();
}

std::vector<Trans::SubTrans> Trans::split() {
  std::vector<SubTrans> rst;
  std::map<std::string, SubTrans*> tag_subtrans;

  for(size_t i = 0; i < tags_.size(); i ++) {
    auto& tag = tags_[i];
    auto& command = commands_[i];

    if (tag_subtrans.count(tag) == 0) {
      SubTrans subtrans(tag, tid_);
      rst.push_back(subtrans);
      tag_subtrans[tag] = &subtrans;
    }

    tag_subtrans[tag]->append(command);
  }

  return rst;
}

}
