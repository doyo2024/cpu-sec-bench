#include "include/assembly.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  grv--;
}

int *FORCE_NOINLINE helper1(int *p) {
  return p - 1;
}

int main() {
  int *func_p = (int *)helper;
  // force revision on function pointer
  CALL_DAT(helper1(func_p) + 1);
  return grv;
}
