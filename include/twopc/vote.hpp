#ifndef __TWOPC_VOTE_HPP__
#define __TWOPC_VOTE_HPP__

#include "twopc/status.hpp"
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
    Vote(bool);

    /**
     * Serialize to a networking buffer
     */
    Buffer toBuffer();

    /**
     * Deserialize from buffer
     */
    Status fromBuffer(Buffer& buffer);

  private:
    bool vote_;
};

class VoteResult {
  public:

    VoteResult();

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

  private:

    bool commit_;
};

}

#endif
