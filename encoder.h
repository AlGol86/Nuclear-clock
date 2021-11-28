#include "iostm8s103f3.h"

#define rim() asm("rim")
#define sim() asm("sim")
#define nop() asm("nop")

#define PORT_ENCODER_L_DDR PD_DDR
#define PORT_ENCODER_L_ODR PD_ODR
#define PORT_ENCODER_L_IDR PD_IDR
#define PORT_ENCODER_L_CR1 PD_CR1
#define PORT_ENCODER_L_CR2 PD_CR2

#define PORT_ENCODER_R_B_DDR PC_DDR
#define PORT_ENCODER_R_B_ODR PC_ODR
#define PORT_ENCODER_R_B_IDR PC_IDR
#define PORT_ENCODER_R_B_CR1 PC_CR1
#define PORT_ENCODER_R_B_CR2 PC_CR2

#define pin_enc_l      2
#define bit_enc_l                       (1<<pin_enc_l)
#define pin_enc_r      7
#define bit_enc_r                       (1<<pin_enc_r)
#define pin_enc_b      6
#define bit_enc_b                       (1<<pin_enc_b)


typedef struct {
	char cnt;
	int cnt_button;
	char f_push :1;
	char f_long_push :1;
	
	int first_enc_data;
	int enc_data;
	int enc_data_lim_h;
	int enc_data_lim_l;
	char but_data;
        char but_data_long;
	char but_data_lim;
	char but_data_lim_long;
} encoder_t;

//encoder
#pragma vector = 15
__interrupt void TIM2Interrupt (void);
void init_tim2 (void);
void init_encoder(encoder_t* enc);
void encoder_handler(encoder_t* enc);
void encoder_setter(int lim_l,int lim_h,int first_value);
char check_encoder_button(encoder_t* enc);
