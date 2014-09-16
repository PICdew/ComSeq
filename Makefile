# Makefile for the CESRF command sequencer project ComSeq

INCLUDE=-I. -I/opt/sdcc-3.4.0/lib/pic16
#LDFLAGS=-w -I/usr/local/share/sdcc/lib/pic16 -c -s /usr/local/share/gputils/lkr/18f452.lkr
LDFLAGS=-I/opt/sdcc-3.4.0/lib/pic16

LIBS=libc18f.lib libsdcc.lib libio18f452.lib
PROCESSOR_FLAGS=-mpic16 -p18f452 --use-non-free
SDCC=/opt/sdcc-3.4.0/bin/sdcc
GPASM=gpasm
CFLAGS=-V -c
#CFLAGS=-V -S
GPLINK=gplink

OBJS=init.o isr.o main.o

all:comseq.hex

%.o : %.c
	$(SDCC) -V $(PROCESSOR_FLAGS) $(CFLAGS) $(INCLUDE) $<

comseq.hex: ${OBJS}
	$(SDCC) -V ${PROCESSOR_FLAGS} --use-crt=crt0.o -o comseq.hex $(OBJS) $(LDFLAGS) $(LIBS)

#%.o : %.c
#	$(SDCC) $(INCLUDE) $(PROCESSOR_FLAGS) $(CFLAGS) $<
#	$(GPASM) -c $*.asm

#comseq.hex: $(OBJS)
#	$(GPLINK) $(LDFLAGS) -o comseq.hex $(OBJS) $(LIBS)
	
clean:
	rm -f *.o *.asm *.hex *.lnk *.cof *.cod *.lst
