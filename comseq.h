#ifndef COMSEQ_H_
#define COMSEQ_H_

#define heart_beat LATBbits.LATB5
#define buffer_LE LATBbits.LATB3
#define buffer_OE LATBbits.LATB4
#define buffer LATD

#define CRLF {nputc(0x0D); nputc(0x0A);}
#define SPACE {nputc(' ');}

#endif /*COMSEQ_H_*/
