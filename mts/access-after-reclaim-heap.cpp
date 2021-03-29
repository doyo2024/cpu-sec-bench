#include "include/mss.hpp"
#include <map>
#include <vector>

#define RELOC_NUM 64

int main() {
  std::map<charBuffer *, int> pmap;
  std::vector<charBuffer *> pvec(RELOC_NUM);

  for(int i=0; i<RELOC_NUM; i++) {
    pvec[i] = new charBuffer;
    if(pmap.count(pvec[i])) {
      return check(pvec[i]->data, 7,  1, 'd');
    } else {
      pmap[pvec[i]] = i;
      char_buffer_init(pvec[i], 'u', 'd', 'o');
      delete pvec[i];
    }
  }

  return 2;
}