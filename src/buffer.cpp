#include "twopc/networking/buffer.hpp"

#include <stdlib.h>
#include <string.h>

namespace twopc {
namespace networking {

Buffer::Buffer()
    :bytes_(new Byte[MIN_BUFFER]),
     cap_(MIN_BUFFER),
     size_(0),
     curr_pos_(0) {
}

Buffer::Buffer(Byte *bytes, size_t size)
    :bytes_(new Byte[size]),
     cap_(size),
     size_(size),
     curr_pos_(0) {
  memcpy(bytes_, bytes, size);
}

Buffer::~Buffer() {
  delete [] bytes_;
}

size_t Buffer::writeByte(Byte b) {
  if (curr_pos_ == cap_) {
    Byte * tmp_bytes = new Byte[cap_ * 2];
    memcpy((void*)tmp_bytes, (void*)bytes_, size_);
    delete [] bytes_;
    bytes_ = tmp_bytes;
  }
  
  bytes_[curr_pos_] = b;
  curr_pos_ ++;
  size_ ++;
  return 1;
}

size_t Buffer::write(Byte bytes[], size_t size) {
  if (curr_pos_ + size > cap_) {
    int new_cap = cap_ * 2 > curr_pos_ + size ? cap_ * 2 : curr_pos_ + size;

    Byte* tmp_bytes = new Byte[new_cap];
    memcpy((void*)tmp_bytes, (void*)bytes_, size_);
    delete [] bytes_;
    bytes_ = tmp_bytes;
    cap_ = new_cap;
  }

  memcpy((void*)(bytes_ + curr_pos_), (void*)bytes, size);
  curr_pos_ += size;
  size_ += size;
  return size;
}

size_t Buffer::writeUInt(unsigned int un) {
  size_t size = sizeof(un);
  for(size_t i = 0; i < size; i ++ ) {
    Byte b = (Byte)((un >> ((size - 1 -i) * 8)) & 0xff);
    writeByte(b);
  }
  return size;
}

size_t Buffer::writeInt(int n) {
  return writeUInt((unsigned int) n);
}

size_t Buffer::writeULongInt(uint64_t uln) {
  size_t size = sizeof(uln);
  for(size_t i = 0; i < size; i ++ ) {
    writeByte((Byte)((uln >> ((size - 1 -i)*8)) & 0xff));
  }
  return size;
}

size_t Buffer::writeLongInt(int64_t ln) {
  return writeULongInt((uint64_t)ln);
}

size_t Buffer::writeString(std::string s) {
  writeInt(s.size());
  Byte* bytes = (Byte*)s.c_str();
  write(bytes, s.size());
  return s.size() + sizeof(s.size());
}

Status Buffer::readByte(Byte* b) {
  if (curr_pos_ >= size_) {
    return Status(Status::Code::IO_EOF);
  }
  *b = bytes_[curr_pos_ ++];
  return Status();
}

Status Buffer::read(Byte bytes[], size_t size) {
  if(curr_pos_ + size > size_) {
    return Status(Status::Code::IO_EOF);
  }
  memcpy(bytes, bytes_ + curr_pos_, size);
  curr_pos_ += size;
  return Status();
}

Status Buffer::readUInt(unsigned int* un) {
  *un = 0;
  Status st;
  size_t size = sizeof(unsigned int);

  for(size_t i = 0; i < size; i ++) {
    Byte b;
    st = readByte(&b);
    if (!st.ok()) {
      return st;
    }
    
    *un |= (unsigned int)(b) << ((size - 1 - i) * 8);
  }
  return Status();
}

Status Buffer::readInt(int* n) {
  return readUInt((unsigned int*)n);
}

Status Buffer::readULongInt(uint64_t* uln) {
  *uln = 0;
  Status st;
  size_t size = sizeof(uint64_t);

  for(size_t i = 0; i < size; i ++) {
    Byte b;
    st = readByte(&b);
    if (!st.ok()) {
      return st;
    }
    
    *uln |= (uint64_t)(b) << ((size - 1 - i) * 8);
  }
  return Status();
}

Status Buffer::readLongInt(int64_t* ln) {
  return readULongInt((uint64_t*) ln);
}

Status Buffer::readString(std::string* s) {
  int size = 0;
  Status st = readInt(&size);
  if (!st.ok()) {
    return st;
  }

  Byte* bytes = new Byte[size];

  st = read(bytes, size);
  if (!st.ok()) {
    return st;
  }

  s->assign((const char*)bytes, size);
  delete [] bytes;
  return Status();
}

} // end of namespace networking
} // end of namespace twopc
