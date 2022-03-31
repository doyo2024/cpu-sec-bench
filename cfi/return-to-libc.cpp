#include <cstdlib>
#include "include/assembly.hpp"

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label) {
  ENFORCE_NON_LEAF_FUNC;
  arch_int_t m = 0;
  MOD_STACK_DAT(label, offset);
  COMPILER_BARRIER;
  PASS_INT_ARG(0, m);
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  offset = 4 * (argv[1][0] - '0');

  void *label = (void *)exit;
  helper(label);
  return 3;
}
