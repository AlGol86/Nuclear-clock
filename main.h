#include "iostm8s103f3.h"
#define MENU_SIZE 8

typedef enum {time, set_hr, set_min, set_day, set_month, set_year, set_corr_sec, set_corr_millis, set_bell} menu_t;

typedef struct {
	char epochSecFirstPoint[4];
        char timeCorrSec;
        char timeCorrDecaMs;
        char positiveCorr;
} eeprom_data_t;

void menu_partition(menu_t menu);
void populate_timeAlignment_from_eeprom();
void save_timeAlignment_to_eeprom();