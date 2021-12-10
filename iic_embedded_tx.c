#include "iostm8s103f3.h"
#include "iic_embedded_tx.h"

extern char b[100];
extern char i;

void init_iic_emb__tx(){
        I2C_CR2_SWRST = 1; 
        I2C_CR2_SWRST = 0; 
        I2C_CR1_PE = 0; 
        I2C_OARH |= MASK_I2C_OARH_ADDCONF;
        I2C_FREQR_FREQ = 2; //Master f (2MHz by default)
        I2C_TRISER_TRISE = 3; //= I2C_FREQR_FREQ + 1
        I2C_CCRL = 20; // for fMaster = 2MHZ -> ~ >= 10 (4 still works)
        I2C_CCRH_CCR = 0;
        
//        I2C_ITR_ITBUFEN = 1;                //  Buffer interrupt enabled.
//        I2C_ITR_ITEVTEN = 1;                //  Event interrupt enabled.
//        I2C_ITR_ITERREN = 1;
        
        I2C_CR1 |= MASK_I2C_CR1_PE;
        I2C_CR2  |= MASK_I2C_CR2_ACK;
}

void i2c_wr_reg(char address, char reg_addr, char * data, char length) {  

while(I2C_SR3_BUSY){
  I2C_CR2_SWRST = 1; 
  init_iic_emb__tx();
}
I2C_CR2_START = 1;
while(!I2C_SR1_SB);
I2C_DR = address << 1;
while(!I2C_SR1_ADDR){if(I2C_SR2_AF) {
  I2C_CR2_STOP = 1; 
  return;}}
I2C_SR3;
while(!I2C_SR1_TXE){if(I2C_SR2_AF) {
  I2C_CR2_STOP = 1; 
  return;}}
I2C_DR = reg_addr;

while(length--){
  while(!I2C_SR1_TXE){
    if(I2C_SR2_AF) {
      I2C_CR2_STOP = 1; 
      return;}
  }
  char c = *data++;
  I2C_DR = c;
}

while(!(I2C_SR1_TXE && I2C_SR1_BTF)){if(I2C_SR2_AF) {
  I2C_CR2_STOP = 1; 
  return;}
}
I2C_CR2_STOP = 1;
while(I2C_CR2_STOP);

}



