#include "iostm8s103f3.h"
#include "iic.h"
#include "oled.h"
#include "rtc.h"
#include "main.h"
#include "encoder.h"

//globals
encoder_t encoder;

int cachedDays = -1;
LocalDate cachedDate;
LocalTime cachedTime;
OledDigitBuffer oledBuffer;
TimeAlignment timeAlignment;
menu_t menu;

int main()
{
  init_encoder(&encoder);
  init_rtc();
//rtc_set_time();/////////////
//reset & switch on from halt
  init_ssd1306_2();
  oled_Clear_Screen();
  encoder.but_data_lim=set_year;
//oled_print_XXXnumber(-456,0);
 while(1)
 { 
   if(menu!=encoder.but_data){
     menu=encoder.but_data;
     oled_Clear_Screen();
   }
   switch(menu){
   case time: 
     print_time_giga(); 
     break;
   case set_hr: 
     encoder_setter(0,23,cachedTime.hr);
     while(menu==encoder.but_data){
     oled_print_XXnumber(encoder.enc_data,0); 
     }
     send_rtc(HR, encoder.enc_data); 
     break;
   case set_min: 
     encoder_setter(0,59,cachedTime.min);
     while(menu==encoder.but_data){
     oled_print_XXnumber(encoder.enc_data,20); 
     }
     send_rtc(MIN, encoder.enc_data); 
     break;
   case set_day: 
     encoder_setter(0,31,cachedDate.dayOfMonth);
     while(menu==encoder.but_data){
     oled_print_XXnumber(encoder.enc_data,40); 
     }
     send_rtc(DATE, encoder.enc_data); 
     break;
   case set_month: 
     encoder_setter(0,12,cachedDate.month);
     while(menu==encoder.but_data){
     oled_print_XXnumber(encoder.enc_data,60); 
     }
     send_rtc(MONTH, encoder.enc_data); 
     break;

   case set_year: 
     encoder_setter(0,99,cachedDate.year%100);
     while(menu==encoder.but_data){
     oled_print_XXnumber(encoder.enc_data,80); 
     }
     send_rtc(YEAR, encoder.enc_data); 
     timeAlignment.epochSecFirstPoint=receive_seconds_rtc_utc();
     break;
   }
   

  

 }
}
