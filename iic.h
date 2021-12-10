//this example is intended for 8 MHz clock frequency
#include "iostm8s103f3.h"
#define PIN_sda     5   //port B
#define PIN_clk     4   //port B

#define BIT_sda    (1<<PIN_sda)
#define BIT_clk    (1<<PIN_clk)
#define BIT_read    1
#define READ        1
#define BIT_write   0
#define WRITE       0
#define ACK         0 //acknowledge
#define NOT_ACK     1 //not acknowledge
//#define PA GPIOA
//#define PB GPIOB
//#define PD GPIOD
//#define PE GPIOE

void sys_del_us(char del_us);
char start_iic (char adr_iic, char read_write_bit);
char send_byte (char data_byte);
unsigned char receive_byte (char acknowledge);
void stop_iic (void);
char get_addresses_iic( char n );