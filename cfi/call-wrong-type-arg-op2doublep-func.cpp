#include <cstdlib>
#include "include/global_var.hpp"

void FORCE_NOINLINE helper(double *var) {
  if(*var == 2.0) exit(0);
  else gvar_init((unsigned int)(*var));
}

class DoubleObj
{
public:
  DoubleObj(double x): xPos(x) {}
  double xPos;
};

static double *dp;

int main(int argc, char* argv[])
{ 
  gvar_init(1);
  int num = argv[1][0] - '0';
  DoubleObj m((double)num);
  dp = (double *)(&m);
  helper(dp);
  return gvar();
}
