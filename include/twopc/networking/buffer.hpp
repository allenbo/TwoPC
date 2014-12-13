#ifndef __TWOPC_NETWORKING_BUFFER_HPP__
#define __TWOPC_NETWORKING_BUFFER_HPP__

#include <string>

#include "twopc/status.hpp"
#include <stdint.h>

namespace twopc {
namespace networking {

class Buffer {
  public:
    typedef unsigned char Byte;
    static const size_t MIN_BUFFER = 1024;

    Buffer();
    Buffer(const Buffer& buffer);
    Buffer(Byte *bytes, size_t);
    ~Buffer();

    inline void reset() { curr_pos_ = 0; }
    const size_t size() const { return size_; }
    const unsigned char*  data() const { return bytes_; }

    size_t writeInt(int n);
    size_t writeUInt(unsigned int un);
    size_t writeLongInt(int64_t ln);
    size_t writeULongInt(uint64_t uln);
    size_t writeString(std::string s);
    size_t writeByte(Byte b);
    size_t write(Byte bytes[], size_t size);

    Status readInt(int* n);
    Status readUInt(unsigned int* un);
    Status readLongInt(int64_t* ln);
    Status readULongInt(uint64_t* uln);
    Status readString(std::string * s);
    Status readByte(Byte* b);
    Status read(Byte bytes[], size_t size);

  private:
    Byte   *bytes_;
    size_t cap_;
    size_t size_;
    size_t curr_pos_;
};


} // end of namespace networking
} // end of nemespace twopc

#endif
