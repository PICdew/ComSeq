// Interrupt service routines
#include <pic18fregs.h>
#include <signal.h>
#include "comseq.h"

extern volatile unsigned long seconds;
extern volatile unsigned int timer;
extern volatile unsigned char char_in;
extern volatile unsigned char char_rdy;
unsigned char half_second_flag;

#define TMR0_RELOAD (0x100 - 0x26)
#define TIMER_RELOAD 0x1F5

// Interrupts
DEF_INTHIGH(high_handler)

DEF_HANDLER(SIG_RCIF, rx_isr)
DEF_HANDLER2(SIG_TMR0, SIG_TMR0IF, timer0_isr)

END_DEF

SIGHANDLER(rx_isr) {
	/***** Usart1 Code *****/
	if (!char_rdy) {
	    char_in = RCREG;
	    char_rdy = 1;
	}
	TXREG = RCREG;
	PIR1bits.RCIF = 0;	// clear event flag
}

SIGHANDLER(timer0_isr) {
	/***** Timer 0 Code *****/
        TMR0L = TMR0_RELOAD;
	if (timer == 0) {
                half_second_flag ^= 1;
                if (half_second_flag & 0x01) {
                    seconds++;
                }
		heart_beat ^= 1;
                timer = TIMER_RELOAD;
                //TMR0H = TIMERH_RELOAD;
                //TMR0L = TIMERL_RELOAD;
	}
        else {
            timer--;
        }
	INTCONbits.T0IF = 0;	// clear event flag
}

//SIGHANDLER(my_isr) {

//	/***** A2D Converter Code *****/
//	if((ADIE)&&(ADIF)){
//		ADIF=0;	// clear event flag
//	}
	
//	/***** EEPROM Code *****/
//	if((PIE2bits.EEIE)&&(PIR2bits.EEIF)){
//		EEIF=0;	// clear event flag
//	}
	
	
//	/***** Usart1 Code *****/
//	if((PIE1bits.TXIE)&&(PIR1bits.TXIF)){
//		PIR1bits.TXIF = 0;	// clear event flag
//	}
//	
	
//}
