#include <stdint.h>
#include <string.h>
#include "init.h"
#include <pic18fregs.h>
#include "comseq.h"

/* global program defines */
#define PROGRAM_TITLE   "ComSeq\0"
#define PROGRAM_VERSION "0.02\0"

/* Sequencer opcodes */

#define CMND_NOP     0
#define SET_OUTPUT   1
#define CLR_OUTPUT   2
#define DELAY        3
#define CMND_END   255

volatile unsigned long seconds;
volatile unsigned int timer;
volatile unsigned char char_in;
volatile unsigned char char_rdy;
unsigned char buffer_shadow;

typedef struct {
    uint8_t opcode;
    uint16_t parameter;         /* opcode parameter */
} cmnd_rec;


#define MAX_COMMANDS 32

cmnd_rec cmnd_list[MAX_COMMANDS];
uint8_t cmnd_ptr;

void buffer_oe(unsigned char state) {
    /* Enable/Disable the buffer outputs
     */
    if (state) {
       buffer_OE  = 0;
    }
    else {
       buffer_OE  = 1;
    }
}

void buffer_le(unsigned char state) {
    /* Enable/Disable the buffer outputs
     */
    if (state) {
       buffer_LE  = 1;
    }
    else {
       buffer_LE  = 0;
    }
}

void set_buffer(void) {
	/* sets the output buffer
	 */
    buffer_le(0);
    buffer = buffer_shadow;
    buffer_le(1);
}

/***********************************************************
    adj_BCD
***********************************************************/

void adj_bcd (unsigned char *bcd_digit) {

    if ((*bcd_digit + 0x03) & 0x08) {
        *bcd_digit += 0x03;
    }
    if ((*bcd_digit + 0x30) & 0x80) {
        *bcd_digit += 0x30;
    }
}

/***********************************************************
    bin_to_BCD
***********************************************************/

void bin_to_BCD(unsigned long *bcd, unsigned int bin) {

    unsigned int i, b;

    *bcd = 0;

    for (i = 0; i <= 15; i++) {
        adj_bcd((unsigned char *) bcd);
        adj_bcd((unsigned char *) bcd+1);
        adj_bcd((unsigned char *) bcd+2);
        adj_bcd((unsigned char *) bcd+3);
        b = bin & 0x8000;
        bin <<= 1;
        *bcd <<= 1;
        if (b) {
            *bcd |= 1;
        }
        else {
            *bcd &= 0xFFFFFFFE;
        }
    }
}

/***********************************************************
    nputc
***********************************************************/

void nputc (unsigned char c) {

    /* wait for buffer to empty */
    while(!PIR1bits.TXIF);
    TXREG = c;
}

/***********************************************************
    printfu04
***********************************************************/

void printfu04(uint8_t Data) {
    /* Send 4-bit hex value */
    uint8_t c = Data&0x0f;
    if (c > 9) {
        c += 'A'-10;
    }
    else {
        c += '0';
    }
    nputc(c);
}

/***********************************************************
    print_BCD
***********************************************************/

void print_BCD (unsigned long bcd, int dp) {

    int i;
    unsigned char p_zero, c;

    p_zero = 0;
    for (i = 24; i >= 0; i -= 4) {
        c = (bcd >> i) & 0x0f;
        p_zero = p_zero || (c != 0);
        if (p_zero || (i == 0) || ((i == 4) && dp)) {
            if (dp && (i == 4)) {
                nputc('.');
            }
            nputc(c + '0');
        }
    }
}

/***********************************************************
    print_string
***********************************************************/

void print_string(unsigned char *str) {

    unsigned char *ptr;

    ptr = str;
    while (*ptr != '\0') {
        while(!PIR1bits.TXIF);
      TXREG = *ptr;
        ptr++;
    }
}

/***********************************************************
    print_bit_pos
***********************************************************/

void print_bit_pos (uint8_t b) {

    uint8_t i;

    for (i = 0; i < 8; i++) {
        if (b & (1 << i)) {
            nputc(i + '0');
            SPACE;
        }
    }
}

/***********************************************************
    print_cmnd
***********************************************************/

void print_cmnd(cmnd_rec *r) {

    nputc('|');
    switch (r->opcode) {
        case SET_OUTPUT : {
                          print_bit_pos(r->parameter);
                          print_string("ON");
                          break;
                          }
        case CLR_OUTPUT : {
                          print_bit_pos(r->parameter);
                          print_string("OFF");
                          break;
                          }
        case DELAY      : {
                          print_string("WAIT");
                          break;
                          }
        default         : { /* basically a NOP */
                          print_string("Uknown opcode");
                          }
    }
}

/***********************************************************
    setup_cmnd_list
***********************************************************/

uint8_t setup_cmnd_list(void) {

    uint8_t ptr;

    ptr = 0;
    /* Ambient */
    cmnd_list[ptr].opcode = CLR_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000111;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 830;
    /* A on */
    ptr++;
    cmnd_list[ptr].opcode = SET_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* B on */
    ptr++;
    cmnd_list[ptr].opcode = SET_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000010;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* A Off */
    ptr++;
    cmnd_list[ptr].opcode = CLR_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 290;
    ptr++;
    /* A On */
    cmnd_list[ptr].opcode = SET_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* B Off */
    ptr++;
    cmnd_list[ptr].opcode = CLR_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000010;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* A Off */
    ptr++;
    cmnd_list[ptr].opcode = CLR_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 290;
    /* A On */
    ptr++;
    cmnd_list[ptr].opcode = SET_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* B C On */
    ptr++;
    cmnd_list[ptr].opcode = SET_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000110;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* A Off */
    ptr++;
    cmnd_list[ptr].opcode = CLR_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 290;
    /* A On */
    ptr++;
    cmnd_list[ptr].opcode = SET_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* B C Off */
    ptr++;
    cmnd_list[ptr].opcode = CLR_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000110;
    ptr++;
    cmnd_list[ptr].opcode = DELAY;
    cmnd_list[ptr].parameter = 5;
    /* A Off */
    ptr++;
    cmnd_list[ptr].opcode = CLR_OUTPUT;
    cmnd_list[ptr].parameter = 0b00000001;
    ptr++;
    cmnd_list[ptr].opcode = CMND_END;
    cmnd_list[ptr].parameter = 0;

    return (ptr);
}

/***********************************************************
    handle_opcode
***********************************************************/

uint16_t handle_opcode(cmnd_rec *r) {

    uint16_t delta = 0;

    switch (r->opcode) {
        case SET_OUTPUT : {
                          /* turn on requested output pins */
                          buffer_shadow = buffer_shadow |  (r -> parameter & 0xFF);
                          set_buffer();
                          break;
                          }
        case CLR_OUTPUT : {
                          /* turn off requested output pins */
                          buffer_shadow = buffer_shadow & ~(r -> parameter & 0xFF);
                          set_buffer();
                          break;
                          }
        case DELAY      : {
                          /* pause execution for the indicated period */
                          delta = r -> parameter;
                          break;
                          }
        case CMND_NOP   : {
                          }
        default         : {
                          }
    }
    return(delta);
}

/***********************************************************
    perform_scan
***********************************************************/

uint8_t perform_scan(cmnd_rec *i, uint16_t *t_delta, uint8_t i_ptr) {

    /* travel through the command list until a delay is reached or the END flag is reached */
    while ((i[i_ptr].opcode != CMND_END) && (*t_delta == 0) ) {
        print_cmnd(&i[i_ptr]);
        *t_delta = handle_opcode(&i[i_ptr]);
        i_ptr++;
    }
    return(i_ptr);
}

/***********************************************************
    main
***********************************************************/

void main(void) {

    unsigned long bcd_result;
    uint32_t start_time;
    uint32_t scan_time;
    uint16_t time_adjustment;
    uint16_t scan_period;
    uint8_t num_events;

    init();                   /* init peripherals and interrupt handlers */
    seconds = 0;
    buffer_oe(1);
    num_events = setup_cmnd_list();
    cmnd_ptr = 0;
    scan_time = 0;
    scan_period = 1800;
    /* Communications startup delay. The USART seems to require some clock settling time */
    while (seconds == 0);
    print_string(PROGRAM_TITLE);
    SPACE;
    print_string(PROGRAM_VERSION);
    CRLF;
    scan_time = seconds;
    start_time = scan_time;
    while (1){
        if (scan_time == seconds) {
            bin_to_BCD(&bcd_result, scan_time);
            print_BCD(bcd_result, 0);
            SPACE;
            time_adjustment = 0;
            cmnd_ptr = perform_scan(cmnd_list, &time_adjustment, cmnd_ptr);
            if (time_adjustment) {
                /* delay scan, continue after delay at current command pointer value */
                /* TODO check to make sure delay doesn't push it over the next normal scan time */
                scan_time += time_adjustment;
                nputc('|');
                bin_to_BCD(&bcd_result, (uint32_t) time_adjustment);
                print_BCD(bcd_result, 0);
            }
            else {
                /* normal scan period, reset command pointer */
                scan_time = start_time + scan_period;
                start_time = scan_time;
                cmnd_ptr = 0;
            }
            nputc('|');
            bin_to_BCD(&bcd_result, (uint32_t) scan_time);
            print_BCD(bcd_result, 0);
            CRLF;
        }
        if (char_rdy) {
            if (char_in == 't') {
                print_string("Pressed Heartbeat button\0");
                heart_beat ^= 1;
            }
            if (char_in == '1') {
                buffer_shadow = 1;
                set_buffer();
            }
            else if (char_in == '2') {
                buffer_shadow = 0;
                set_buffer();
            }
            char_rdy = 0;
        }
    }
}
