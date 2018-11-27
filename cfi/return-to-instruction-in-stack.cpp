#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

typedef unsigned int (*func_type)(void);

void FORCE_NOINLINE helper(unsigned char* m) {
	rv = 1;
	MOD_RET_DAT(m);
}

int main()
{
  unsigned char m[] = FUNC_MACHINE_CODE;
  rv = m[0];
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  PUSH_LABEL(xlabel);
  helper(m);
  DECL_LABEL(xlabel);
  return 0;
}
