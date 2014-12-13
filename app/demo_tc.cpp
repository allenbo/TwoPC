#include "twopc/srv.hpp"
#include "twopc/config.hpp"

#include <iostream>

using namespace twopc;

static std::string tag1 = "1";
static std::string tag2 = "2";

Config make_config() {
  Config conf;
  conf.add_part(tag1, "127.0.0.1:8001");
  conf.add_part(tag2, "127.0.0.1:8002");
  conf.set_coord("8003");
  return conf;
}

int main() {
  Config conf(make_config());

  Srv server(conf, "8004");
  server.loop();
}
