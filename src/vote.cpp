#include "twopc/vote.hpp"

namespace twopc {

Vote::Vote()
    :vote_(false) {
}

Vote::Vote(bool v)
    :vote_(v) {
}

Buffer Vote::toBuffer() {
  Buffer buffer;
  if (vote_) {
    buffer.writeString("yes");
  } else {
    buffer.writeString("no");
  }
  return buffer;
}

Status Vote::fromBuffer(Buffer& buffer) {
  std::string vs;
  Status st = buffer.readString(&vs);
  if (!st.ok()) {
    return st;
  }
  if (vs == "yes") {
    vote_ = true;
  } else if (vs == "no") {
    vote_ = false;
  } else {
    st = Status(Status::Code::PROTO_ERROR);
  }
  return st;
}

VoteResult::VoteResult()
    :commit_(false) {
}

Buffer VoteResult::toBuffer() {
  Buffer buffer;
  if (commit_) {
    buffer.writeString("commit");
  } else {
    buffer.writeString("abort");
  }

  return buffer;
}

Status VoteResult::fromBuffer(Buffer& buffer) {
  std::string vs;
  Status st = buffer.readString(&vs);
  if (!st.ok()) {
    return st;
  }
  if (vs == "commit") {
    commit_ = true;
  } else if (vs == "abort") {
    commit_ = false;
  } else {
    st = Status(Status::Code::PROTO_ERROR);
  }
  return st;
}

Status VoteResult::parse_votes(std::vector<Vote>& votes, VoteResult* rst) {
  Status st;
  bool commit = true;
  for(auto& v : votes) {
    if (v.vote_ == false) {
      commit = false;
    }
  }
  rst->commit_ = commit;
  return st;
}

}
