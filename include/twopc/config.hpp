#ifndef __TWOPC_CONFIG_HPP__
#define __TWOPC_CONFIG_HPP__

#include "twopc/status.hpp"
#include <map>
#include <string>
#include <vector>

namespace twopc {

class Config {
  public:

    Config();
    Config(Config&& o);
    Config(const Config& o);
    
    /**
     * add another participant
     */
    Status add_part(std::string tag, std::string addr); 

    /**
     * set address of coordinator
     */
    Status set_coord(std::string addr);

    /**
     * get address of coordinator
     */
    inline std::string coord_addr() { return coord_addr_; }

    inline int part_size() { return part_addrs_.size(); }

    std::vector<std::string> tags();

  private:

    std::string coord_addr_;
    std::map<std::string, std::string> part_addrs_;
};

}

#endif
