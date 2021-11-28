#include "iostm8s103f3.h"

#define CONST_eeprom_pointer ((char*)0x4010)

#pragma vector = 3
__interrupt void awu(void);

#pragma vector = 13
__interrupt void time1(void);

#pragma vector = 16
__interrupt void time2cc(void);

#pragma vector = 15
__interrupt void time2(void);

typedef enum {time, set_hr, set_min, set_day, set_month, set_year, set_correction, set_bell} menu_t;

void init_AWU(void);