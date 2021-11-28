#include "iostm8s103f3.h"
#include "main.h"
#include "rtc.h"
#include "oled.h"

__interrupt void awu(void)
{
  extern char hr_previous;
  extern char p_data_graphic[128];
  char hr1_get=0xff;
  int data[3];
  
  if(AWU_CSR1&0x20)//flag AWUF (clear after reading)
  hr1_get=receive_rtc(MIN, 1);///HR
  
  if((hr1_get&4)==(hr_previous&4)) asm("nop");
  else 
  {
   
   // get_bme_280_p_t_h(data);
    for(char i=0;i<127;i++)   p_data_graphic[i] =  p_data_graphic[i+1];

    p_data_graphic[127]=(char)((data[0]-7350)>>1);
    hr_previous=hr1_get;
  }
}





__interrupt void time1(void)
{
 TIM1_SR1_UIF=0; //flag "0"

 }


__interrupt void time2(void)
{
 TIM2_SR1_UIF=0; //flag "0"
 TIM2_IER_CC1IE=0;


}

__interrupt void time2cc(void)
{TIM2_SR1_CC1IF=0;
 TIM2_IER_UIE=1;//enable interrupt by overfollow

}

