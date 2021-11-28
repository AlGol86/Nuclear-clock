//this example is intended for 8 MHz clock frequency
#include "iostm8s103f3.h"
#include "rtc.h"
#include "oled.h"

  extern int cachedDays;
  extern LocalDate cachedDate;
  extern LocalTime cachedTime;

void delay_rtc(char del )
{
char i;
for(i=0;i<(del);i++)
{asm("nop");}
}
void rtc_set_time(void)
{
 send_rtc(CONTR,0);  //WRITE-PROTECT: OFF
 send_rtc(TR_CH,0);    //TRICKLE CHARGER: OFF
 send_rtc(YEAR, YEAR_INIT); 
 send_rtc(DAY,  WEEK_INIT);    //DAY OF WEEK
 send_rtc(MONTH, MONTH_INIT); 
 send_rtc(DATE, DATE_INIT); 
 send_rtc(HR, HR_INIT); 
 send_rtc(MIN, MIN_INIT); 
 send_rtc(SEC, 0);
}

void init_rtc(void)
{
  PC_CR2&=~BIT_clk_rtc ; // low speed (interrupt disabled)
  PA_CR2&=~BIT_rst_rtc ; // low speed (interrupt disabled)
  PC_CR1|=BIT_clk_rtc ; // p-p 
  PA_CR1|=BIT_rst_rtc ; // p-p 
  PC_ODR&=~BIT_clk_rtc ;  //clk_pin, rst_pin  - 0 
  PA_ODR&=~BIT_rst_rtc ;  //clk_pin, rst_pin  - 0 
  PC_DDR|=BIT_clk_rtc ; 
  PA_DDR|=BIT_rst_rtc ; 
  
  
  PC_CR2&=~BIT_dat_rtc; //dat_pin  - low speed (interrupt disabled)
  PC_CR1|=BIT_dat_rtc; //dat_pin - p-p /float in inp. direct
  PC_ODR&=~BIT_dat_rtc;  //dat_pin  - 1 (SDA)
  PC_DDR|=BIT_dat_rtc; 
 }

void send_byte_rtc(char data)
{
  
for(char i=1;i<=8;i++)
{
PC_ODR|=(data&1)<<PIN_dat_rtc;
data>>=1;
PC_ODR|=BIT_clk_rtc; // clk=1
asm("nop");
PC_ODR&=~BIT_dat_rtc; // sda=1
PC_ODR&=~BIT_clk_rtc;// clk=0
}
}

char  receive_byte_rtc(void)
{
char data=0;
PC_CR1|=BIT_dat_rtc;
asm("nop");
PC_DDR&=~BIT_dat_rtc;
for(char i=0;i<=7;i++)
{
asm("nop");
PC_ODR|=BIT_clk_rtc; // clk=1
asm("nop");
data|=((PC_IDR&BIT_dat_rtc)>>PIN_dat_rtc)<<i;
asm("nop");
PC_ODR&=~BIT_clk_rtc;// clk=0
asm("nop");
}
PC_CR1|=BIT_dat_rtc;
PC_DDR|=BIT_dat_rtc;
return (data);
}

void send_rtc(char reg_adr, char data)
{ data = ((data/10)<<4) | data%10;
  delay_rtc(3); //9*0.5us 
  PA_ODR|=BIT_rst_rtc; //rst=1(>4us)
  delay_rtc(3); //9*0.5us 
  reg_adr<<=1;
  reg_adr+=0x80;
  send_byte_rtc(reg_adr);
  asm("nop");
  send_byte_rtc(data);
  PA_ODR&=~BIT_rst_rtc; //rst=0
}

char receive_rtc(char reg_adr)
{
  char receive_data;
  delay_rtc(3); //9*0.5us 
  PA_ODR|=BIT_rst_rtc; //rst=1(>4us)
  delay_rtc(3); //9*0.5us 
  reg_adr<<=1;
  reg_adr+=0x81;
  send_byte_rtc(reg_adr);
  receive_data=receive_byte_rtc();
  PA_ODR&=~BIT_rst_rtc; //rst=0
  return receive_data;
}

char receive_plain_val_rtc(char reg_adr) {
  char val;
  val = receive_rtc(reg_adr);
  return ((val & 0xf0)>>4) * 10 + (val & 0x0f);
}

unsigned long receive_seconds_rtc_utc() {
  long sec=0;
  char currentSec = receive_plain_val_rtc(SEC);
  sec += currentSec;
  char refrashCondition = ((currentSec & 0x0f) ==0);
  if(refrashCondition) {
    cachedTime.min = receive_plain_val_rtc(MIN);    
    cachedTime.hr = receive_plain_val_rtc(HR);
  } 
  sec +=  cachedTime.min * 60;
  sec += (unsigned long)cachedTime.hr * 3600;
  LocalDate date;
  date.year       = ((refrashCondition) ? (2000 + receive_plain_val_rtc(YEAR)) : cachedDate.year);
  date.month      = (refrashCondition) ? receive_plain_val_rtc(MONTH) : cachedDate.month;
  date.dayOfMonth = (refrashCondition) ? receive_plain_val_rtc(DATE) : cachedDate.dayOfMonth;
  unsigned long t;
  t=getDaysOfDate(date) * (unsigned long) SECOND_PER_DAY + sec - (unsigned long)SECOND_MOSCOW_ALIGNMENT;
  return t;
}


void print_time_giga(void)
{
   unsigned long val = receive_seconds_rtc_utc() + 10800;
   oled_print_giga_char(':',81);
   oled_print_giga_char(':',35);
   
   oled_print_XXnumber(val % 60,96); //9*15  
   val /= 60; 
   
   oled_print_XXnumber(val % 60,50); //9*15
   val /= 60;  
   
   oled_print_XXnumber(val%24 ,4); //9*15
   
   
//  oled_print_XXnumber(receive_plain_val_rtc(HR),4); //9*15
//
//  oled_print_giga_char(':',35);
//  oled_print_XXnumber(receive_plain_val_rtc(MIN),50); //9*15
//
//  oled_print_giga_char(':',81);
//  oled_print_XXnumber(receive_plain_val_rtc(SEC),96); //9*15

}

void init_tim1(void) //count days each 24 hours
{

//TIM1_IER_UIE=0;//enable interrupt by overfollow
	//TIM1_IER_CC1IE=1;//enable interrupt by compare
  TIM1_PSCRH=0x00; //prescaler 0-255 
  TIM1_PSCRL=0x00; //prescaler 0-255 
  TIM1_ARRH=0xFF; //preload registr high bits
  TIM1_ARRL=0xFF;
  TIM1_RCR=0xFF; //0:1 hour   1:2 hours 23: 24 hours 
	//TIM1_CCR1H=12;//capture/compare registr high bits
	//TIM1_CCR1L=0;
	TIM1_CNTRH=0;
	TIM1_CNTRL=0;
      
	//asm("rim");
	TIM1_CR1_CEN=1;
       // for(int i=1;i<30000;i++){}
       // for(int i=1;i<30000;i++){}
       // TIM1_SR1_UIF=0;
     //   TIM1_IER_UIE=1;
    
}


unsigned long getDays(unsigned long seconds) {
        return seconds / (unsigned long) SECOND_PER_DAY;
    }

unsigned long getDaysOfDate(LocalDate date) {
   

        if (equals(date, cachedDate)) return cachedDays;
        int days = 0;
        for (int i = FIRST_YEAR; i < date.year; i++) {
            days += isLeapYear(i) ? LEAP_DAYS : NON_LEAP_DAYS;
        }
        for (int i = 1; i < date.month; i++) {
            days += getMonthLength(i, isLeapYear(date.year));
        }
        cachedDays = days + date.dayOfMonth - 1;
        cachedDate = date;
        return cachedDays;
    }

    LocalDate getDate(long days) {
        if (days == cachedDays) return cachedDate;
        int year = FIRST_YEAR;
        int dayCntr = 0;
        int dayInYear = 0;
        while (dayCntr + dayInYear <= days) {
              dayCntr += dayInYear;
              dayInYear = isLeapYear(year) ? LEAP_DAYS : NON_LEAP_DAYS;
            year++;
        }
        year--;
        int currentDays = days - dayCntr;
        cachedDays = days;
        LocalDate date;
        date.year = year;
        date.month = getMonth(currentDays, year).month;
        date.dayOfMonth = getMonth(currentDays, year).dayOfMonth;
        cachedDate = date;
        return cachedDate;
    }

    LocalDate getMonth(int days, int year) {
        char month = 1;
        int dayCntr = 0;
        int dayInMonth = 0;
        while (dayCntr + dayInMonth <= days) {
            dayCntr += dayInMonth;
            dayInMonth = getMonthLength(month, isLeapYear(year));
            month++;
        }
        month--;
        int currentDays = days - dayCntr;
        LocalDate date;
        date.month = month;
        date.dayOfMonth = currentDays;
        return date;
    }

 char isLeapYear(int year) {
        return (year % 400) == 0 || (year % 100) != 0 && (year % 4) == 0;
    }

 char getMonthLength(char month, char leap) {

        switch (month) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12: return 31;
            case 4:
            case 6:
            case 9:
            case 11: return 30;
            case 2: return leap ? 29 : 28;
            default: return -1;
        }
    }

char equals(LocalDate date1, LocalDate date2) {
  return date1.year == date2.year && date1.month == date2.month && date1.dayOfMonth == date2.dayOfMonth;
}
