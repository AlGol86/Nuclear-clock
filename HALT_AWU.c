#include "iostm8s103f3.h"
#include "main.h"

void init_AWU(void)
{

AWU_APR_APR=0x09;//0x09; APRdiv = (AWU_APR+2)

AWU_TBR|=0x0f;//0x0f;/T=5.25...30.72 sec - it depends on  APRdiv= 11...64 = (AWU_APR+2)

AWU_CSR1|=0x10; //AWU enable
asm("rim");
asm("halt");
}