#include "iostm8s103f3.h"
#include "iic.h"
#include "oled.h"
#include "rtc.h"
#include "bme_280.h"
#include "main.h"

//globals
char hr_previous; 
char p_data_graphic[128];

int main()
{
char position=0;

init_rtc();
//rtc_set_time();/////////////
//reset & switch on from halt

//init_BME280();
 
init_ssd1306_2(oled_32_128_adr);
//oled_blink1(0, 127, 20,1);
init_tim1();
asm("nop");
 while(1)
 { 

   oled_Clear_Screen();

   
  for(char i=0;i<30;i++)
    {
      print_time_giga();

    }
   oled_Clear_Screen();
   
   oled_print_graphic(p_data_graphic,128,0,2);
   init_AWU();
   oled_Clear_Screen(); 
   
   for(char i=0;i<10;i++)
    {
  //   position=print_bme_280_p_t_h(4,0,0,MEGA);
  //   print_bme_280_p_t_h(2,position+10,0,MEGA); 
  //   print_bme_280_p_t_h(1,0,2,MEGA); 
     delay(30000);  
    }

 }
}
