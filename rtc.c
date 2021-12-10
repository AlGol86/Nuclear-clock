//this example is intended for 8 MHz(?) clock frequency
#include "iostm8s103f3.h"
#include "rtc.h"
#include "iic_embedded_tx.h"

void delay_rtc(char del) {
  char i;
  for(i=0;i<(del);i++) {asm("nop");}
}

void init_rtc(void) {
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

void rtc_set_time(LocalTime time, LocalDate date) {
  send_rtc(CONTR,0);  //WRITE-PROTECT: OFF
  send_rtc(TR_CH,0);    //TRICKLE CHARGER: OFF
  send_rtc(YEAR, date.year % 100); 
  //send_rtc(DAY,  WEEK_INIT);    //DAY OF WEEK
  send_rtc(MONTH, date.month); 
  send_rtc(DATE, date.dayOfMonth); 
  send_rtc(HR, time.hr); 
  send_rtc(MIN, time.min); 
  send_rtc(SEC, 0);
}

void send_byte_rtc(char data) {
  for(char i=1;i<=8;i++) {
    PC_ODR|=(data&1)<<PIN_dat_rtc;
    data>>=1;
    PC_ODR|=BIT_clk_rtc; // clk=1
    asm("nop");
    PC_ODR&=~BIT_dat_rtc; // sda=1
    PC_ODR&=~BIT_clk_rtc;// clk=0
  }
}

char  receive_byte_rtc(void) {
  char data=0;
  PC_CR1|=BIT_dat_rtc;
  asm("nop");
  PC_DDR&=~BIT_dat_rtc;
  for(char i=0;i<=7;i++) {
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

void send_rtc(char reg_adr, char data) { 
  data = ((data/10)<<4) | data%10;
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

char receive_rtc(char reg_adr) {
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


/*************************************************
*                 TIME-ENHANCEMENT               *
*************************************************/
 
unsigned long receiveEpochSecondsRtcMoscow() {
  
  extern SecondsRtcUtcCache secondsRtcUtcCache;
  
  char currentSec = receive_plain_val_rtc(SEC);
  if(currentSec == secondsRtcUtcCache.sec && secondsRtcUtcCache.cacheEneble) {
    return secondsRtcUtcCache.epochSec;
  }
  secondsRtcUtcCache.sec = currentSec;
  unsigned int currentMinFromMidnight = ((unsigned int)receive_plain_val_rtc(HR) * 60) + receive_plain_val_rtc(MIN); 
   if(currentMinFromMidnight == secondsRtcUtcCache.minFromMidnight && secondsRtcUtcCache.cacheEneble) {
    secondsRtcUtcCache.epochSec =  secondsRtcUtcCache.epochSecToMimute + currentSec;
    return secondsRtcUtcCache.epochSec;
  }
  secondsRtcUtcCache.minFromMidnight = currentMinFromMidnight;
  
  char dayOfMonth = receive_plain_val_rtc(DATE);
  if(dayOfMonth == secondsRtcUtcCache.day && secondsRtcUtcCache.cacheEneble) {
     secondsRtcUtcCache.epochSecToMimute = secondsRtcUtcCache.epochSecToDay + ((unsigned long)currentMinFromMidnight) * 60;
     secondsRtcUtcCache.epochSec = secondsRtcUtcCache.epochSecToMimute + currentSec;
     return secondsRtcUtcCache.epochSec;
  }
  secondsRtcUtcCache.day = dayOfMonth;
  
  secondsRtcUtcCache.epochSecToDay = getEpochDaysOfDate(receive_plain_val_rtc(YEAR), receive_plain_val_rtc(MONTH), dayOfMonth) * SECOND_PER_DAY;
  secondsRtcUtcCache.epochSecToMimute = secondsRtcUtcCache.epochSecToDay + ((unsigned long)currentMinFromMidnight) * 60;
  secondsRtcUtcCache.epochSec = secondsRtcUtcCache.epochSecToMimute + currentSec;
  secondsRtcUtcCache.cacheEneble = 1;
  return secondsRtcUtcCache.epochSec;
}

unsigned long getEpochDaysOfDate(char year, char month, char day) {
  unsigned long result = 0;
  for (int i = FIRST_YEAR; i < (year + 2000); i++) {
    result += isLeapYear(i) ? LEAP_DAYS : NON_LEAP_DAYS;
  }
  for (char i = 1; i < month; i++) {
    result += getMonthLength(i, isLeapYear(year + 2000));
  }
  return result + day - 1;;
}



void refreshTimeTransferBody(void) {  
  
  extern TransferBody transferBody; 
  extern TimeTransferBodyCache timeTransferBodyCache;
  
  unsigned long epochRawSec = receiveEpochSecondsRtcMoscow();
  if (timeTransferBodyCache.epochRawSeconds == epochRawSec && timeTransferBodyCache.cacheEneble) {
    return;
  }
  timeTransferBodyCache.epochRawSeconds = epochRawSec;
  
  unsigned long actual_now =  getActualSeconds(epochRawSec);    
  
  LocalDate date = getDateFromEpochDays(actual_now / SECOND_PER_DAY);
  transferBody.dayOfMonth = date.dayOfMonth;
  transferBody.month      = date.month;
  transferBody.year       = date.year % 100;  
  
  transferBody.sec = actual_now % 60;
  actual_now /= 60; 
   
  transferBody.min = actual_now % 60;
  actual_now /= 60;  
   
  transferBody.hr  = actual_now % 24;  
  
  timeTransferBodyCache.cacheEneble = 1;
 }

unsigned long getActualSeconds(unsigned long epochRawSec) {

  extern TimeAlignment timeAlignment;
  extern AlignmentTimeCache alignmentTimeCache;

  unsigned long secFromFirst = epochRawSec - timeAlignment.epochSecFirstPoint;
  
  unsigned long hoursFromFirst = secFromFirst / 3600;
  
  if (alignmentTimeCache.hoursFromFirst != hoursFromFirst) {
    alignmentTimeCache.hoursFromFirst = hoursFromFirst;
    unsigned long daysFromFirst = hoursFromFirst / 24;
    unsigned int correctionDecaMsPerDay = (unsigned int) timeAlignment.timeCorrSec * 100 + timeAlignment.timeCorrDecaMs; 
    timeAlignment.shiftSeconds = daysFromFirst * (correctionDecaMsPerDay) / 100;
    timeAlignment.shiftSeconds += ((secFromFirst % SECOND_PER_DAY) * (correctionDecaMsPerDay)) / 3600 / 2400;
    alignmentTimeCache.shiftSeconds = timeAlignment.shiftSeconds;
  }
  
  if (timeAlignment.positiveCorr) return epochRawSec + timeAlignment.shiftSeconds;
  return epochRawSec - timeAlignment.shiftSeconds;
}

LocalDate getDateFromEpochDays(unsigned int days) {
  
   extern DateFromEpochDaysCache dateFromEpochDaysCache;
   
   if (days == dateFromEpochDaysCache.rawDays) {
     return dateFromEpochDaysCache.date;
   }
   dateFromEpochDaysCache.rawDays = days;
   
   int year = FIRST_YEAR;
   unsigned int dayCntr = 0;
   unsigned int dayInYear = 0;
   while (dayCntr + dayInYear <= days) {
     dayCntr += dayInYear;
     dayInYear = isLeapYear(year) ? LEAP_DAYS : NON_LEAP_DAYS;
     year++;
   }
   year--;
   int currentDays = days - dayCntr + 1;
   LocalDate date;
   date.year = year;
   date.month = getMonth(currentDays, year).month;
   date.dayOfMonth = getMonth(currentDays, year).dayOfMonth;
   dateFromEpochDaysCache.date = date;
   return date;
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