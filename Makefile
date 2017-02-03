CC=msp430-gcc
CPU=-mmcu=msp430g2212
CFLAGS=-std=c1x -Os -Wall -g $(CPU) -Ilib -I. -ffunction-sections -fdata-sections

OBJS=main.o

all: main.elf size

main.elf: $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -fr *.elf $(OBJS) *.o

size:
	msp430-size *.elf

dis:
	msp430-objdump -d main.elf | less

prog: main.elf
	msp430-objcopy -R .infob main.elf main.prog 
	mspdebug rf2500 "prog main.prog"

reset:	
	mspdebug rf2500 "prog main.elf"
	mspdebug rf2500 "verify main.elf"

.PHONY: prog all size usbif dozi
