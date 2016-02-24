# Makefile for the CESRF command sequencer project ComSeq

SDCCDIR=/opt/sdcc-3.5.0/
SDCC=$(SDCCDIR)bin/sdcc
GPASM=gpasm
GPLINK=gplink

INCLUDE=-I.
PROCESSOR_FLAGS=-mpic16 -p18f452 --use-non-free

CFLAGS=-V -c
LDFLAGS=--use-crt=crt0.o -Wl -m

LIBS=libc18f.lib #libsdcc.lib libio18f452.lib

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
