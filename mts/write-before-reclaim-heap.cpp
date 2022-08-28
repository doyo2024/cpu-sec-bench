#include "include/mss.hpp"
#include <set>

#define RELOC_NUM 64

int main() {
  std::set<charBuffer *> pset;
  charBuffer *buffer = new charBuffer('u','d','o');
  pset.insert(buffer);
  delete buffer;
  update_by_pointer(buffer->data, 0, 8,  1, 'c');

  for(int i=0; i<RELOC_NUM; i++) {
    buffer = new charBuffer;
    if(pset.count(buffer))
      return check(buffer->data, 8,  1, 'c');
    else {
      pset.insert(buffer);
      buffer->updateBuffer('u', 'd', 'o');
    }
    delete buffer;
    update_by_pointer(buffer->data, 0, 8,  1, 'c');
  }
  return 2;
}
