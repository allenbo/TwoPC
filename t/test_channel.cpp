#include "twopc/networking/channel.hpp"
#include "twopc/networking/acceptor.hpp"
#include "common/all.hpp"

using namespace COMMON;
using twopc::Status;
using twopc::networking::Channel;
using twopc::networking::Acceptor;
using twopc::networking::Buffer;

const char* stc_msg = "hello world";
const char* cts_msg = "This is a message";

class Server : public Thread {
  public:
    Server(std::string addr) {
      acceptor = new Acceptor(addr);
    }

    void run() {
      Channel ch;
      acceptor->accept(&ch);
      Buffer buffer;
      buffer.writeString(std::string(stc_msg));

      Status st = ch.send(buffer);
      assert(st.ok());

      Buffer buffer2;
      st = ch.recv(&buffer2);
      assert(st.ok());

      std::string msg;
      st = buffer2.readString(&msg);
      assert(st.ok() && msg == cts_msg);
    }

    ~Server() {
      delete acceptor;
    }

  private:
    Acceptor *acceptor;
};

int main() {
  Server server("8888");
  server.start();

  Channel ch("127.0.0.1:8888");
  Buffer buffer;
  Status st = ch.recv(&buffer);
  assert(st.ok());

  std::string msg;
  st = buffer.readString(&msg);
  assert(st.ok() && msg == stc_msg);

  Buffer buffer2;
  buffer2.writeString(std::string(cts_msg));

  st = ch.send(buffer2);
  assert(st.ok());
  server.join();
}
