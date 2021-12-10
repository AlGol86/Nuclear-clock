#include "iostm8s103f3.h"

#define PIN_sda     5   //port B
#define PIN_clk     4   //port B

#define BIT_sda    (1<<PIN_sda)
#define BIT_clk    (1<<PIN_clk)

#define TRANSFERED_SIZE 9
#define RX_ADDR         0x06

typedef struct {
  char menu;
  
  char year;
  char month;
  char dayOfMonth;
  
  char hr;
  char min;
  char sec;
  
  char displayedValue[2];
  char valueIsPositive;
  
} TransferBody; 


void init_iic_emb__tx();
void i2c_wr_reg(char address, char reg_addr, char * data, char length);
