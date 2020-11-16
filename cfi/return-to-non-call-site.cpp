#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  grv = 3;

  MOD_RET_LABEL(main_mid);
  grv = 0;
}

int main()
{
  grv = 2;
  // call a function but illegally return
  helper();
  grv = 4; // failed if runs here

  // the elligal return site
  DECL_LABEL(main_mid);

  exit(grv);
}
