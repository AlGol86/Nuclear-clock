#include "iostm8s103f3.h"
#include "encoder.h"

__interrupt void TIM2Interrupt (void)
{
	extern encoder_t encoder;
	
	TIM2_SR1_UIF=0;//flag "0"
	encoder_handler(&encoder);
	
	return;
}

void init_tim2 (void) {//encoder_poller
	TIM2_IER_UIE=1;
	//TIM2->IER|=TIM1_IER_CC1IE;
        TIM2_PSCR=0;
	TIM2_ARRH=2; 
        TIM2_ARRL=20;
	TIM2_CNTRH=0;
	TIM2_CNTRL=0;
	rim();
	TIM2_CR1_CEN=1;
  }

void init_encoder(encoder_t* enc){
        init_tim2();
        
	PORT_ENCODER_R_B_CR2&=~(bit_enc_l|bit_enc_r|bit_enc_b);
	PORT_ENCODER_R_B_DDR&=~(bit_enc_l|bit_enc_r|bit_enc_b);
	PORT_ENCODER_R_B_CR1|=(bit_enc_l|bit_enc_r|bit_enc_b);
        PORT_ENCODER_L_CR2&=~(bit_enc_l|bit_enc_r|bit_enc_b);
	PORT_ENCODER_L_DDR&=~(bit_enc_l|bit_enc_r|bit_enc_b);
	PORT_ENCODER_L_CR1|=(bit_enc_l|bit_enc_r|bit_enc_b);
        enc->but_data_lim=1;
}

char check_encoder_button(encoder_t* enc){
      
        if(PORT_ENCODER_R_B_IDR&bit_enc_b) return 0;
        else return 1;

}

void encoder_handler(encoder_t* enc){ 
  char cnt_lim=7;
	int cnt_button_lim=10000;
//encoder
	if(((PORT_ENCODER_L_IDR&bit_enc_l)==0) && (enc->cnt < cnt_lim)) 
	  enc->cnt++;
  else	if (enc->cnt>0) enc->cnt--;
//button
	if(((PORT_ENCODER_R_B_IDR&bit_enc_b)==0) && (enc->cnt_button < cnt_button_lim)) 
	  enc->cnt_button++;
  else	if (enc->cnt_button > 0) 
	        if(enc->cnt_button > cnt_lim) 
					    enc->cnt_button = cnt_lim; 
					else
					 enc->cnt_button--;
//encoder
	if ((enc->cnt >(cnt_lim-2))&&(enc->f_push==0)) {
		  enc->f_push=1;
			if(((PORT_ENCODER_R_B_IDR&bit_enc_r)==0)&&(enc->enc_data < enc->enc_data_lim_h)) enc->enc_data++; 
			else if(((PORT_ENCODER_R_B_IDR&bit_enc_r)!=0)&&(enc->enc_data > enc->enc_data_lim_l)) enc->enc_data--;
	    }
   if ((enc->cnt==0)&&(enc->cnt_button==0))	
	       {enc->f_push=0;}


//button short	
	if ((enc->cnt_button >(cnt_lim-2))&&(enc->f_push==0)) {
		enc->f_push=1;
		if (enc->but_data < enc->but_data_lim) enc->but_data++;
		else enc->but_data=0;
	}			
	
}
	
void encoder_setter(int lim_l,int lim_h,int first_value){
		extern encoder_t encoder;	
		encoder.enc_data_lim_l=lim_l;
		encoder.enc_data_lim_h=lim_h;
		encoder.enc_data=first_value;
}