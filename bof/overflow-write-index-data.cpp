#include "include/bof.hpp"

charBuffer buffer;

int main()
{
  char_buffer_init(&buffer, 'u', 'd', 'o');
  update_by_index(buffer, 0, 16, 1, 'c');
  return check(buffer.overflow, 8, 1, 'c');
}
