#include "iostm8s103f3.h"
//#include "iic.h"
#include "oled.h"
#include "iic_embedded_tx.h"

void sendCmd(char cmd) {

  i2c_wr_reg(oled_32_128_adr, 0x80, &cmd, 1);
//  start_iic(oled_32_128_adr, WRITE);
//  send_byte (0x80);
//  send_byte (cmd);
//  stop_iic();
}

void sendData(char* data, char count) {

    i2c_wr_reg(oled_32_128_adr, 0x40, data, count);
//  start_iic(oled_32_128_adr,WRITE);
//  send_byte (0x40);
//  for(int j=0;j<count;j++) {
//    send_byte (*(data+j));
//  }
//  stop_iic();
}

void init_ssd1306_2()
{                                                                         //conrtast=7f//
  char initCmds[] = {0xAE,0x20,0x00,0xC0,0xA0,  0xB0,0x00,0x10,0x40,0x81,     0x7F,       0xA6,0xA8,0x3F,0xA4,
                     0xD3,0x00,0xD5,0XF0,0xD9,  0x22,0xDA,0x02,0xDB,0x20,  0x8D, 0x14, 0xAF};
   for(int j=0;j<28;j++) sendCmd(initCmds[j]);
   oled_Clear_Screen();
}

void blankScreen(char n)
{
send_repeat_data(0x00,n);
}
void oled_Clear_Screen(void)
{ 
  extern OledDigitBuffer oledBuffer;
  set_cursor(0,0 );send_repeat_data(0x00,128);
  set_cursor(0,1 );send_repeat_data(0x00,128);
  set_cursor(0,2 );send_repeat_data(0x00,128);
  set_cursor(0,3 );send_repeat_data(0x00,128);
  for(char i=0; i<DIG_BUF_SIZE; i++) {oledBuffer.position[i] = EMPTY;}
}

void ff_string(char n)
{
send_repeat_data(0xff,n);
}

void send_repeat_data(char data_byte,char n)
{

  for(int j=1;j<=n;j++) i2c_wr_reg(oled_32_128_adr, 0x40, &data_byte, 1);
//start_iic(oled_32_128_adr, WRITE);
//send_byte (0x40);
//for(int j=1;j<=n;j++) send_byte (data_byte);
//stop_iic();
}

void set_cursor(char x,char y )
{   sendCmd(0x0f&x); 
    sendCmd(0x10|(0x0f&(x>>4)));
    sendCmd(0xb0|y); 
}


void oled_print_giga_digit(char d,char x)
{
  const char c=4;
  const char const_dig[10][9]={{0xff,0x0f,0xff,0xff,0x00,0xff,0x0f,0x0f,0x0f},  //0
                               {0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x0f},  //1
                               {0x0f,0x0f,0xff,0xff,0x0f,0x0f,0x0f,0x0f,0x0f},  //2
                               {0x0f,0x0f,0xff,0x0f,0x0f,0xff,0x0f,0x0f,0x0f},  //3
                               {0xff,0x00,0xff,0x0f,0x0f,0xff,0x00,0x00,0x0f},  //4
                               {0xff,0x0f,0x0f,0x0f,0x0f,0xff,0x0f,0x0f,0x0f},  //5
                               {0xff,0x0f,0x0f,0xff,0x0f,0xff,0x0f,0x0f,0x0f},  //6
                               {0x0f,0x0f,0xff,0x00,0x00,0xff,0x00,0x00,0x0f},  //7
                               {0xff,0x0f,0xff,0xff,0x0f,0xff,0x0f,0x0f,0x0f},  //8
                               {0xff,0x0f,0xff,0x0f,0x0f,0xff,0x0f,0x0f,0x0f}}; //9
  char dig[9];
  for (char i=0;i<10;i++) dig[i]=const_dig[d][i]; 
  
  set_cursor(x,0);
  for (char i=0;i<3;i++) send_repeat_data(dig[i],c);
  set_cursor(x,1);
  for (char i=3;i<6;i++) send_repeat_data(dig[i],c);
  set_cursor(x,2);
  for (char i=6;i<9;i++) send_repeat_data(dig[i],c);
  }


char oled_print_giga_char(char c,char x)
{
  const char cc=3;
  const char const_ch[3][12]={{0x00,0x38,0x38,0x00, 0x00,0xc0,0xc0,0x00, 0x00,0x01,0x01,0x00},  //0   ':'
                              {0x00,0x00,0x00,0x00, 0x06,0x06,0x06,0x06, 0x00,0x00,0x00,0x00},  //1   '-'
                              {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x0e,0x00,0x00}  //2   '.'
                              }; 
                              
  char pos;
  char ch[12];
  switch(c)
  {
  case ':':{c=0; pos=16; break;}
  case '-':{c=1; pos=16; break;}
  case '.':{c=2; pos=12; break;}
  }
  
  
  for (char i=0;i<12;i++) ch[i]=const_ch[c][i]; 
  
  set_cursor(x,0);
  for (char i=0;i<4;i++) send_repeat_data(ch[i],cc);
  set_cursor(x,1);
  for (char i=4;i<8;i++) send_repeat_data(ch[i],cc);
  set_cursor(x,2);
  for (char i=8;i<12;i++) send_repeat_data(ch[i],cc);
  return (pos+x);
  }

void oled_print_XXnumber(int n, char pos) {
  extern OledDigitBuffer oledBuffer;
  for (char i=0; i<DIG_BUF_SIZE; i++) {if (n == oledBuffer.value[i] && pos == oledBuffer.position[i]) return;}
    
  char bufIndex = 0;
  while(oledBuffer.position[bufIndex] != 255 && oledBuffer.position[bufIndex] != pos)
  {bufIndex++;}
  oledBuffer.position[bufIndex] = pos;
  oledBuffer.value[bufIndex] = n;
  
  if (n<0) {oled_print_giga_char('-',pos);pos+=16;n=-n;} 
  if (n >= 10) {oled_print_giga_digit(n/10, pos);}
  else {oled_print_giga_digit(0, pos);}
  pos += 16;
  oled_print_giga_digit(n%10, pos);
}


                       
                       