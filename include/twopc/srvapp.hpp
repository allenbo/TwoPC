#ifndef __TWOPC_SRVAPP_HPP__
#define __TWOPC_SRVAPP_HPP__

#include "twopc/trans.hpp"
#include "twopc/tp.hpp"
#include "twopc/config.hpp"

namespace twopc {

/* server app, will start with a tp
 */
class SrvApp {
  public:

    SrvApp(std::string tag, Config& config);

    /**
     * execute the transaction w/o commit,
     * result true if transaction can be committed
     */
    virtual bool excute_callback(Trans::SubTrans& trans);

    /**
     * commit the transaction
     */
    virtual bool commit_callback(Trans::SubTrans& trans);

    /**
     * abor the transaction
     */
    virtual bool abort_callback(Trans::SubTrans& trans);

    /**
     * loop
     */
    void loop() {
      tp_->loop();
    }

    void stop() {
      tp_->stop();
    }

    virtual ~SrvApp();

  private:
    TP* tp_;
};

}

#endif
