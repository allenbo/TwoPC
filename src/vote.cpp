#include "twopc/vote.hpp"
#include "common/all.hpp"

using namespace COMMON;

namespace twopc {

Vote::Vote()
    :vote_(false) {
}

Vote::Vote(bool v, TID tid)
    :vote_(v), tid_(tid) {
}

Buffer Vote::toBuffer() {
  Buffer buffer;
  buffer.writeULongInt(tid_);
  if (vote_) {
    buffer.writeString("yes");
  } else {
    buffer.writeString("no");
  }
  return buffer;
}

Status Vote::fromBuffer(Buffer& buffer) {
  TID tid;
  Status st;

  st = buffer.readULongInt(&tid);
  if (!st.ok()) {
    return st;
  }

  std::string vs;
  st = buffer.readString(&vs);
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
  tid_ = tid;
  return st;
}

VoteResult::VoteResult()
    :VoteResult(0) {
}

VoteResult::VoteResult(TID tid)
    :commit_(false), tid_(tid) {
}

Buffer VoteResult::toBuffer() {
  Buffer buffer;
  buffer.writeULongInt(tid_);
  if (commit_) {
    buffer.writeString("commit");
  } else {
    buffer.writeString("abort");
  }

  return buffer;
}

Status VoteResult::fromBuffer(Buffer& buffer) {
  TID tid;
  Status st;

  st = buffer.readULongInt(&tid);
  if (!st.ok()) {
    return st;
  }

  std::string vs;
  st = buffer.readString(&vs);
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
  tid_ = tid;
  return st;
}

Status VoteResult::parse_votes(std::vector<Vote>& votes, VoteResult* rst) {
  CHECK(votes.size() >= 1);
  TID tid = votes[0].tid();
  LOG(DEBUG) << "Set the tid of vote result to " << tid << std::endl;
  rst->set_tid(tid);

  Status st;
  bool commit = true;
  for(auto& v : votes) {
    if (v.vote_ == false) {
      commit = false;
    }
    CHECK(v.tid() == tid);
  }
  rst->commit_ = commit;
  return st;
}

}
