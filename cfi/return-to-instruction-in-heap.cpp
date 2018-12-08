#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(const unsigned char* m) {
  rv = 1;
  MOD_RET_DAT(m);
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
}

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  rv = m[0];

  PUSH_LABEL(xlabel);
  helper(m);
  DECL_LABEL(xlabel);
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
  return 0;
}