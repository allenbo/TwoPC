#ifndef __TWOPC_VOTE_HPP__
#define __TWOPC_VOTE_HPP__

#include "twopc/status.hpp"
#include "twopc/trans.hpp"
#include "twopc/networking/buffer.hpp"

#include <vector>

namespace twopc {

using networking::Buffer;

class Vote;       // yes or no
class VoteResult; // commit or result

class Vote {
  friend class VoteResult;
  public:

    Vote();
    Vote(bool, TID tid);

    /**
     * Serialize to a networking buffer
     */
    Buffer toBuffer();

    /**
     * Deserialize from buffer
     */
    Status fromBuffer(Buffer& buffer);

    inline TID tid() { return tid_; }
    inline bool vote() { return vote_; }

  private:
    bool vote_;
    TID  tid_;
};

class VoteResult {
  public:

    VoteResult();
    VoteResult(TID tid);

    inline bool commit() { return commit_; }

    /**
     * Serialize to a networking buffer
     */
    Buffer toBuffer();

    /**
     * Deserialize from a buffer
     */
    Status fromBuffer(Buffer& buffer);

    /**
     * parse votes
     */
    static Status parse_votes(std::vector<Vote>& votes, VoteResult* rst);

    inline TID tid() { return tid_; }
    inline void set_tid(TID tid) { tid_ = tid; }

  private:

    bool commit_;
    TID  tid_;
};

}

#endif
