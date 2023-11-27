#include "include/mss.hpp"
#include "include/conf.hpp"
#include "include/assembly.hpp"

int main(int argc, char* argv[])
{

  INIT_BUFFER;
  int flow_type  = argv[1][0] - '0';

  int rv;

  if(flow_type == 0){
    update_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), 0, 2*BUFFER_SIZE,  1, 'c');
    rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), overflow),      BUFFER_SIZE,  1, 'c');
  }else if(flow_type == 1){
    update_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), 0, -(BUFFER_SIZE+1),  -1, 'c');
    rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), underflow),      BUFFER_SIZE,  1, 'c');
  }else if(flow_type >= 2 && flow_type < 4){
    int assem_dist;
    long long pa = (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data);
    if (flow_type == 2){
      long long pb = (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),overflow);
      GET_DISTANCE(assem_dist, pa, pb);
      update_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), assem_dist, BUFFER_SIZE,  1, 'c');
      rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), overflow),      BUFFER_SIZE,  1, 'c');
    }else if(flow_type == 3){
      long long pb = (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),underflow);
      GET_DISTANCE(assem_dist, pa, pb);
      update_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), assem_dist, -(BUFFER_SIZE+1),  -1, 'c');
      rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), underflow),      BUFFER_SIZE,  1, 'c');
    }
  }else if(flow_type >= 4){
    int othersrc_dist = argv[2][0] - '0';
    if(flow_type == 4){
      update_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), othersrc_dist, BUFFER_SIZE,  1, 'c');
      rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), overflow),      BUFFER_SIZE,  1, 'c');
    }else if(flow_type == 5){
      update_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), othersrc_dist, -(BUFFER_SIZE+1),  -1, 'c');
      rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), underflow),      BUFFER_SIZE,  1, 'c');
    }
  }

  UNINIT_BUFFER;
  return rv;
}
