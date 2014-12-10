#ifndef __TWOPC_TC_HPP__
#define __TWOPC_TC_HPP__

#include "common/all.hpp"
#include "twopc/status.hpp"
#include "twopc/config.hpp"
#include "twopc/trans.hpp"
#include "twopc/vote.hpp"

#include "twopc/networking/cluster.hpp"


#include <vector>

using namespace COMMON;

namespace twopc {
using networking::CoordCluster;

/* Transaction co-ordinator
 */
class TC {
  public:

    TC(Config& config);

    Status set_key2tag(Trans::KeyToTag key2tag);

    /**
     * Excute a transaction
     */
    Status excuteTrans(Trans& trans, VoteResult* rst);

  private:

    Mutex inproc_mutex_;
    CoordCluster networking_;
    Trans::KeyToTag key2tag_;

    /**
     * propose transaction to tp and get voting result
     */
    Status propose(SubTransVec& subtranss, VoteResult* rst);

    /**
     * commit or abort based on voteresult
     */
    Status decide(VoteResult& rst);
};

}

#endif
