#include "twopc/srvapp.hpp"
#include "twopc/tp.hpp"
#include "twopc/config.hpp"

#include <iostream>

using namespace twopc;

static std::string tag1 = "1";
static std::string tag2 = "2";

Config make_config() {
  Config conf;
  conf.add_part(tag1, "127.0.0.1:8001");
  conf.add_part(tag2, "127.0.0.1:8002");
  conf.set_coord("127.0.0.1:8003");
  return conf;
}

int main(int argc, char** args) {
  if (argc != 2) {
    std::cerr << "Error" << std::endl;
    return -1;
  }
  Config conf(make_config());

  std::string tag;
  if (tag1 == args[1]) {
    tag = tag1;
  } else {
    tag = tag2;
  }

  SrvApp app(tag, conf);
  app.loop();
}
