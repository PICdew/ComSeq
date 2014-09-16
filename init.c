#include <pic18fregs.h>

// Initialize user ID locations
//__IDLOC(00000000);

/*** Device configuration registers ***/
/*
 * Oscillator = HS
 * Osc. Switch Enable = Disabled
 */
//__CONFIG(1, HS & OSCSDIS);
//code char at __CONFIG1H _config1h = _OSC_HS_1H & _OSCS_OFF_1H;
#pragma config OSC=HS, OSCS=OFF

/*
 * Brown Out Detect = Enabled
 * Brown Out Voltage = 4.2V
 * Power Up Timer = Enabled
 * Watchdog Timer = Disabled-Controlled by SWDTEN bit
 * Watchdog Postscaler = 1:1
 */
//__CONFIG(2, BOREN & BORV42 & PWRTEN & WDTDIS & WDTPS1);
code char at __CONFIG2L config2l = 0xFF & _PUT_ON_2L & _BODEN_ON_2L & _BODENV_4_2V_2L;
code char at __CONFIG2H config2h = 0xFF & _WDT_OFF_2H;
/*
 * CCP2 Mux = RB3
 */
//__CONFIG(3, CCP2RB3);
code char at __CONFIG3H config3h = 0xFF & _CCP2MUX_RC1_3H;

/*
 * Background Debug = Disabled
 * Low Voltage Program = Disabled
 * Stack Overflow Reset = Disabled
 */
//__CONFIG(4, DEBUGDIS & LVPDIS & STVRDIS);
code char at __CONFIG4L config4l = 0xFF & _LVP_OFF_4L & _BACKBUG_OFF_4L & _STVR_OFF_4L;

/*
 * Code Protect Boot = Disabled
 * Data EE Read Protect = Disabled
 * Code Protect 00200-01FFF = Disabled
 * Code Protect 02000-03FFF = Disabled
 * Code Protect 04000-05FFF = Disabled
 * Code Protect 06000-07FFF = Disabled
 */
//__CONFIG(5, 0xFFFF);
code char at __CONFIG5L config5l = 0xFF & _CP_1_OFF_5L & _CP_2_OFF_5L & _CP_3_OFF_5L;
code char at __CONFIG5H config5h = 0xFF & _CPD_OFF_5H & _CPB_OFF_5H;
/*
 * Table Write Protect Boot = Disabled
 * Config. Write Protect = Disabled
 * Data EE Write Protect = Disabled
 * Table Write Protect 00200-01FFF = Disabled
 * Table Write Protect 02000-03FFF = Disabled
 * Table Write Protect 04000-05FFF = Disabled
 * Table Write Protect 06000-07FFF = Disabled
 */
//__CONFIG(6, 0xFFFF);
code char at __CONFIG6L config6l = 0xFF & _WRT_0_OFF_6L & _WRT_1_OFF_6L & _WRT_2_OFF_6L & _WRT_3_OFF_6L;
code char at __CONFIG6H config6h = 0xFF & _WRTD_OFF_6H & _WRTB_OFF_6H & _WRTC_OFF_6H;
/*
 * Table Read Protect Boot = Disabled
 * Table Read Protect 00200-01FFF = Disabled
 * Table Read Protect 02000-03FFF = Disabled
 * Table Read Protect 04000-05FFF = Disabled
 * Table Read Protect 06000-07FFF = Disabled
 */
//__CONFIG(7, 0xFFFF);
code char at __CONFIG7L config7l = 0xFF & _EBTR_1_OFF_7L & _EBTR_2_OFF_7L & _EBTR_3_OFF_7L;
code char at __CONFIG7H config7h = 0xFF & _EBTRB_OFF_7H;

// Peripheral initialization function
void init(void){
	/***** Common Code ****
	 *  MSSP interrupt is disabled
	 *  Usart1TX interrupt enabled
	 *  Usart1RX interrupt enabled
	 *  A2D Converter interrupt is enabled
	 */

	PIE1	= 0xE0; //0b01110000;

	/*
	 *  EEPROM interrupt is disabled
	 */

	PIE2	= 0x00; //0b00000000;

	/*
	 *  Port directions: 1=input, 0=output
	 */

	TRISE	= 0x01; //0b00000001;

	/*
	 *  Port directions: 1=input, 0=output
	 *  A2D Converter input (AN1) on RA1
	 *  A2D Converter input (AN2) on RA2
	 *  A2D Converter input (AN3) on RA3
	 *  A2D Converter input (AN4) on RA5
	 */

	TRISA	= 0x7F; //0b01111111;

	/*
	 *  Port directions: 1=input, 0=output
	 *  Bit 6 Configured for Usart1 TX
	 *  Bit 7 Configured for Usart1 RX
	 */

	TRISC	= 0x87; //0b10000111;

	/*
	 *  EEPROM interrupt priority is high
	 */

	IPR2	= 0x10; //0b00010000;

	/*
	 *  MSSP interrupt priority is high
	 *  Usart1TX interrupt is high priority
	 *  Usart1RX interrupt is high priority
	 *  A2D Converter interrupt priority is high
	 */

	IPR1	= 0x78; //0b01111000;

	/*
	 *  PortB interrupt-on-change is disabled
	 *  Timer 0 interrupt is enabled
	 *  Peripheral interrupts will be disabled
	 *  All interrupts temporarily disabled during init
	 */

	INTCON	= 0x20; //0b00100000;

	/*
	 *  PortB interrupt-on-change priority is high
	 *  Timer 0 interrupt priority is high
	 */

	INTCON2	= 0x05; //0b00000101;
	
	/***** 18F452 Code ****
	 *  Interrupt priority system not required
	 */

	RCON	= 0x00; //0b00000000;
	
	/***** A2D Converter Code ****
	 *  A2D Converter is enabled
	 *  A2D status is idle
	 *  Analog Channel is AN0
	 *  Conversion Clock (FOSC/2)
	 */
	ADCON0	= 0x01; //0b00000001;

	/*
	 *  Analog/Digital ports using configuration 2
	 *  Conversion Clock (FOSC/2)
	 *  A2D Converter is Left Justified
	 */

	ADCON1	= 0x02; //0b00000010;

	/*
	 *  Left Justified
	 */

	ADRESL	= 0x00; //0b00000000;
	
	/***** PortB Code ****
	 *  Port directions: 1=input, 0=output
            00       - ICSP
              0      - I/O Buffer ~OE  output enable
               0     - I/O Buffer LE   latch enable
                000  -
	 */
        LATB    = 0b00010000;
	TRISB	= 0xC7; //0b11000000;
	
	/***** PortD Code ****
	 *  Port directions: 1=input, 0=output
	 */
	TRISD	= 0x00; //0b00000000;
	
	/***** MSSP Code ****
	 *  Synchronous Serial port mode is 7-Bit address with Start/Stop interrupts
	 *  Release SCK clock
	 *  Serial Ports are enabled
	 *  Overflow enabled
	 *  Write collison detect enabled
	 */

	SSPCON1	= 0xFE; //0b11111110;

	/*
	 *  Clock stretching is enabled
	 *  General call enabled
	 */
	SSPCON2	= 0x81; //0b10000001;

	/*
	 *  SMBus is enabled
	 *  Slew rate control is enabled
	 */

	SSPSTAT	= 0xC0; //0b11000000;
	
	/***** Usart1 Code ****
	 *  Baud rate is 9600 bps
	 */
	SPBRG	= 0x40; //0b01000000;

	/*
	 *  Address detection enabled
	 *  Continuous data reception enabled
	 *  Data reception is 8 bits
	 *  Usart1 is on
	 */

	RCSTA	= 0x98; //0b10011000;

	/*
	 *  High speed baud generator used
	 *  USART is asynchronous
	 *  Data transmission is enabled
	 *  Data transmission is 8 bits
	 */

	TXSTA	= 0x24; //0b00100100;
	
	/***** Timer 0 Code ****
	 *  Timer prescale ratio = /32:
	 *  Timer 0 is clocked internally.
	 *  8-Bit operation
	 *  Timer 0 is active
	 */

	T0CON	= 0xC5; //0b11000101;
	
   /* Global interrupts enabled */
   
   INTCONbits.GIE = 1;
	
}
