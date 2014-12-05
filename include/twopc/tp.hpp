#ifndef __TWOPC_TP_HPP__
#define __TWOPC_TP_HPP__

#include "twopc/status.hpp"
#include "twopc/trans.hpp"
#include "twopc/vote.hpp"

#include "twopc/networking/cluster.hpp"

namespace twopc {

using networking::PartCluster;

class SrvApp;

/* Transaction participant
 */
class TP {
  public:

    TP(std::string tag, Config& config);

    /**
     * register server app as a call back object
     */
    Status register_handler(SrvApp* handler);

    Status loop();


  private:

    SrvApp* handler_;
    std::string tag_;
    PartCluster networking_;

    /**
     * catch transaction
     */
    Status catch_subtrans(Trans::SubTrans* trans);
    
    /**
     * send back to tc the vote
     */
    Status vote(bool);

    /**
     * catch vote result from tc
     */
    Status catch_result(VoteResult* rst);

};

}

#endif
