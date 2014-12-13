#include "twopc/trans.hpp"
#include "twopc/clt.hpp"

#include <stdlib.h>
#include <string>
#include <iostream>

using namespace twopc;

static std::string tag1 = "1";
static std::string tag2 = "2";

static std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

std::string random_string(size_t len) {
  char* str = new char[len];
  for(size_t i = 0; i < len; i++) {
    str[i] = alphabet[rand() % alphabet.size()];
  }
  std::string rst(str);
  delete [] str;
  return rst;
}

void generate_trans(size_t len, Trans& trans) {
  for (size_t i = 0; i < len; i ++ ) {
    if (i % 2) {
      trans.append(tag1, random_string(rand() % 10));
    } else {
      trans.append(tag2, random_string(rand() % 10));
    }
  }
}

int main() {
  std::string server_addr = "127.0.0.1:8004";
  Clt client(server_addr);

  int i = 0;
  while (i++<10) {
    Trans trans;
    generate_trans(10, trans);
    Request req = client.excute_trans(trans);
    std::cout << "Waiting for " << i << " transaction" << std::endl;
    VoteResult rst;
    req.wait(&rst);
  }

}
