#include "include/bof.hpp"
#include "include/assembly.hpp"

#include <cstdio>

void FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, b, dummy.data);
  update_by_index(dummy, delta, 8, 1, 'c');
}

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'u', 'd', 'o');
  helper(buffer.data);
  return check(buffer.data, 8, 1, 'c');
}
