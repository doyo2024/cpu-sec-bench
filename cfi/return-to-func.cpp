#include <cstdlib>
#include "include/global_var.hpp"

volatile arch_int_t offset = 0;

int FORCE_NOINLINE fake_ret() {
  exit(gvar());
}

void FORCE_NOINLINE helper(void * label) {
  gvar_init(0);
  MOD_STACK_DAT(label, offset);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure offset is modified as otherwise
   * the stack is not expanded similarily with
   * the -within-analysis test.
   */
  offset = rand();
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  offset = 4 * (argv[1][0] - '0');
  void *label = (void *)(fake_ret);

  helper(label);
  return 1;
}
