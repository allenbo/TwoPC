#ifndef __TWOPC_TRANS_HPP__
#define __TWOPC_TRANS_HPP__

#include "twopc/status.hpp"
#include "twopc/networking/buffer.hpp"

#include <string>
#include <vector>
#include <set>

namespace twopc {

using networking::Buffer;

typedef uint64_t TID;

/* virtual class for Transaction
 */
class Trans {
  public:

    typedef std::string (*KeyToTag) (std::string);
    /* sub transactions
     */
    class SubTrans {
      public:

        SubTrans();
        SubTrans(std::string tag, TID tid);
        SubTrans(SubTrans&& o);
        SubTrans(const SubTrans& o);

        inline bool isvalid() { return tid_ != 0; }
        inline std::string tag() { return tag_; }
        inline TID tid() { return tid_; }

        /**
         * append command to this sub transaction
         */
        void append(std::string command);

        /**
         * Serialize to a networking buffer object
         */
        Buffer toBuffer();

        /**
         * Deserialize from a networking buffer
         */
        Status fromBuffer(Buffer& buffer);

      private:

        std::string tag_;
        TID tid_;
        std::vector<std::string> commands_;

    };

    Trans();

    Status set_tid(TID tid_);
    inline TID tid() { return tid_; }

    /**
     * Add a new command to transaction
     */
    Status append(std::string key, std::string command);

    /**
     * split transaction to sub transactions based on tag
     */
    std::vector<SubTrans> split(KeyToTag key2tag = nullptr);

    /**
     * Serialize to a networking buffer
     */
    Buffer toBuffer();

    /**
     * Deserialiaze from a networking buffer
     */
    Status fromBuffer(Buffer& buffer);

  private:

    std::vector<std::string> keys_;
    std::vector<std::string> commands_;
    
    TID tid_;
};

typedef std::vector<Trans::SubTrans> SubTransVec;

}

#endif
