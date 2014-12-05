#include "twopc/tc.hpp"

namespace twopc {

TC::TC(Config& config)
    :inproc_mutex_(), networking_(config) {
  Status st = networking_.init();
  CHECK(st.ok());
}

Status TC::excuteTrans(Trans& trans) {
  ScopeLock _(&inproc_mutex_);

  auto subtranss = trans.split();
  
  VoteResult rst;
  Status st = propose(subtranss, &rst);

  if (!st.ok()) {
    return st;
  }

  return decide(rst);
}


Status TC::propose(SubTransVec& subtranss, VoteResult * rst) {
  Status st;
  std::vector<Vote> votes;
  for(auto& subtrans: subtranss) {
    std::string tag = subtrans.tag();
    st = networking_.send(tag, subtrans.toBuffer());
    if (!st.ok()) {
      return st;
    }

    Buffer buf;
    st = networking_.recv(tag, &buf);
    if (!st.ok()) {
      return st;
    }

    Vote vote;
    st = vote.fromBuffer(buf);
    if (!st.ok()) {
      return st;
    }

    votes.push_back(vote);
  }
  
  st = VoteResult::parse_votes(votes, rst);
  return st;
}

Status TC::decide(VoteResult& rst) {
  return networking_.broadcast(rst.toBuffer());
}

}
