#include "include/mss.hpp"

charBuffer::charBuffer(const char uf, const char d, const char of){
  for(unsigned int i=0; i!=CB_BUF_LEN-1; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
  }
  underflow[CB_BUF_LEN-1] = 0;
  data[CB_BUF_LEN-1]      = 0;
  overflow[CB_BUF_LEN-1]  = 0;
}

void charBuffer::updateBuffer(const char uf, const char d, const char of){
  for(unsigned int i=0; i!=CB_BUF_LEN-1; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
  }
  underflow[CB_BUF_LEN-1] = 0;
  data[CB_BUF_LEN-1]      = 0;
  overflow[CB_BUF_LEN-1]  = 0;
}

LARGEMEMBERBUFFERCLASS_INIT(1)
LARGEMEMBERBUFFERCLASS_INIT(2)
LARGEMEMBERBUFFERCLASS_INIT(3)
LARGEMEMBERBUFFERCLASS_INIT(4)
LARGEMEMBERBUFFERCLASS_INIT(5)
LARGEMEMBERBUFFERCLASS_INIT(6)
LARGEMEMBERBUFFERCLASS_INIT(7)
LARGEMEMBERBUFFERCLASS_INIT(8)

LARGEMEMBERBUFFERCLASS_UPDATE(1)
LARGEMEMBERBUFFERCLASS_UPDATE(2)
LARGEMEMBERBUFFERCLASS_UPDATE(3)
LARGEMEMBERBUFFERCLASS_UPDATE(4)
LARGEMEMBERBUFFERCLASS_UPDATE(5)
LARGEMEMBERBUFFERCLASS_UPDATE(6)
LARGEMEMBERBUFFERCLASS_UPDATE(7)
LARGEMEMBERBUFFERCLASS_UPDATE(8)

void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c) {
  for(long long i=offset; i != size+offset; i += step)
    cb.data[i] = c;
}

void FORCE_NOINLINE update_by_pointer(char *buf, long long offset, long long size, int step, char c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step)
    *buf = c;
}

int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c) {
  for(long long i=offset; i != size+offset; i += step)
    if(cb.data[i] != c) return 1;
  return 0;
}
