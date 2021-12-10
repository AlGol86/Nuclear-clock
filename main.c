#include "iostm8s103f3.h"
#include "rtc.h"
#include "main.h"
#include "encoder.h"
#include "iic_embedded_tx.h"

//globals
encoder_t encoder;

LocalDate presetDate;
LocalTime presetTime;

TimeAlignment timeAlignment;
AlignmentTimeCache alignmentTimeCache;
TransferBody transferBody; 
TimeTransferBodyCache timeTransferBodyCache;
SecondsRtcUtcCache secondsRtcUtcCache;
DateFromEpochDaysCache dateFromEpochDaysCache;


TransferBody transferBody;
TimeAlignment timeAlignment;
eeprom_data_t eeprom_data @0x4100;         //store setting variables (in EEPROM)()

int main() { 
  init_iic_emb__tx();
  init_encoder(&encoder);
  init_rtc();
  populate_timeAlignment_from_eeprom();
  encoder.but_data_lim=set_corr_millis;

  while(1) { 
    transferBody.menu = encoder.but_data;
    if (encoder.but_data == time) {
      refreshTimeTransferBody();
    } else { 
      menu_partition(encoder.but_data);
    }
    i2c_wr_reg(RX_ADDR, transferBody.menu, &transferBody.year, TRANSFERED_SIZE);
  }
}


void menu_partition(menu_t m) {

 // time, set_hr, set_min,    set_day, set_month, set_year,   set_corr_sec, set_corr_millis
  int enc_lims_l[MENU_SIZE]={0, 0, 0, 1, 1, 0,-99, 0};
  int enc_lims_h[MENU_SIZE]={0,23,59,31,12,99, 99,99};
  int init_vals[MENU_SIZE] ={0,transferBody.hr, transferBody.min, transferBody.dayOfMonth, transferBody.month, transferBody.year, timeAlignment.timeCorrSec, timeAlignment.timeCorrDecaMs};
  
  if((m == set_corr_sec) && (!timeAlignment.positiveCorr)) init_vals[m]=-init_vals[m];  
  encoder_setter(enc_lims_l[m],enc_lims_h[m],init_vals[m]);

  
  while(m==encoder.but_data) {
    switch(m) {
      case set_hr: presetTime.hr = encoder.enc_data;break;
      case set_min: presetTime.min = encoder.enc_data;break;
  
      case set_day: presetDate.dayOfMonth = encoder.enc_data;break;
      case set_month: presetDate.month = encoder.enc_data;break;
      case set_year: presetDate.year = encoder.enc_data;break;
  
      case set_corr_sec: 
        if(encoder.enc_data < 0) {
          timeAlignment.timeCorrSec = -encoder.enc_data;
          timeAlignment.positiveCorr = 0;
        } else {
          timeAlignment.timeCorrSec = encoder.enc_data;
          timeAlignment.positiveCorr = 1;
        }
        break;
      case set_corr_millis: timeAlignment.timeCorrDecaMs = encoder.enc_data;break;
    }
  
    if (m==set_corr_sec || m==set_corr_millis) {
      transferBody.displayedValue[0] = timeAlignment.timeCorrSec;
      transferBody.displayedValue[1] = timeAlignment.timeCorrDecaMs;
      transferBody.valueIsPositive = timeAlignment.positiveCorr;
    } else {
      transferBody.displayedValue[0] = encoder.enc_data; 
    } 
    transferBody.menu = m;
    i2c_wr_reg(RX_ADDR, transferBody.menu, &transferBody.year, TRANSFERED_SIZE);
   }

   if (encoder.but_data==time) {
     rtc_set_time(presetTime, presetDate);  
     timeAlignment.epochSecFirstPoint = receiveEpochSecondsRtcMoscow();
     save_timeAlignment_to_eeprom();
   }
} 
  
void save_timeAlignment_to_eeprom() { 
  sim();
  if (!FLASH_IAPSR_DUL) {  // unlock EEPROM
       FLASH_DUKR = 0xAE;
       FLASH_DUKR = 0x56;
  }
  rim();
  while (!FLASH_IAPSR_DUL) {;}
  
  for(char i =0; i<4; i++) {
    eeprom_data.epochSecFirstPoint[i] =  (timeAlignment.epochSecFirstPoint >> (i * 8)) & 0xFF;
  }
  eeprom_data.timeCorrSec = timeAlignment.timeCorrSec;
  eeprom_data.timeCorrDecaMs = timeAlignment.timeCorrDecaMs;
  eeprom_data.positiveCorr = timeAlignment.positiveCorr;
  
  FLASH_IAPSR_DUL=0; // lock EEPROM
}  

void populate_timeAlignment_from_eeprom() { 
  
  for(char i =0; i<4; i++) {
     timeAlignment.epochSecFirstPoint |=  ((unsigned long) eeprom_data.epochSecFirstPoint[i]) << (i * 8);
  }
  timeAlignment.timeCorrSec = eeprom_data.timeCorrSec;
  timeAlignment.timeCorrDecaMs = eeprom_data.timeCorrDecaMs;
  timeAlignment.positiveCorr = eeprom_data.positiveCorr;
} 
