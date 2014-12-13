#include "twopc/networking/buffer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <assert.h>

#include <iostream>

using twopc::networking::Buffer;
using twopc::Status;

int main() {
  Buffer buffer;

  int n = rand();
  unsigned int un = rand();
  int64_t ln = rand();
  uint64_t uln = rand();

  std::string s = "abcdefghijklmn";
  Buffer::Byte b = 'c';

  buffer.writeInt(n);
  buffer.writeUInt(un);
  buffer.writeLongInt(ln);
  buffer.writeULongInt(uln);
  buffer.writeString(s);
  buffer.writeByte(b);

  buffer.reset();
  Status st;

  int n2;
  st = buffer.readInt(&n2);
  assert(st.ok() && n2 == n);

  unsigned int un2;
  st = buffer.readUInt(&un2);
  assert(st.ok() && un2 == un);

  int64_t ln2;
  st = buffer.readLongInt(&ln2);
  assert(st.ok() && ln2 == ln);

  uint64_t uln2;
  st = buffer.readULongInt(&uln2);
  assert(st.ok() && uln2 == uln);

  std::string s2;
  st = buffer.readString(&s2);
  assert(st.ok() && s2 == s);

  Buffer::Byte b2;
  st = buffer.readByte(&b2);
  assert(st.ok() && b2 == b);
}
