 #include "iostm8s103f3.h"
#define oled_32_128_adr 0x3c
#define DIG_BUF_SIZE 5
#define EMPTY 255;

typedef struct {
  signed char value[DIG_BUF_SIZE];
  char position[DIG_BUF_SIZE];
} OledDigitBuffer; 

void init_ssd1306_2();
void delay(unsigned int del);
void sendCmd(char cmd);
void sendData(char* data, char count); //send [count] data byts 
void blankScreen(char n);
void oled_Clear_Screen(void);
void set_cursor(char x,char y);
void ff_string(char n);
void send_repeat_data(char data_byte,char n);
void send_single_data(char data_byte);


void oled_print_giga_digit(char d,char x);   //12*20
char oled_print_giga_char(char c,char x); //12*20
void oled_print_XXnumber(int n,char pos);
