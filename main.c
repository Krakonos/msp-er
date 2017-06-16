/******************************************************************************
 * Filename: main.c
 *
 * Author: Ladislav Láska
 * e-mail: krakonos@krakonos.org
 * License: BSD
 *
 ******************************************************************************/

#include <msp430.h>
#include <legacymsp430.h>

#define __INFOC __attribute__((section(".infoc")))
#define __INFOD __attribute__((section(".infod")))

#include "program.h"


int prgcnt = 0;
int prgdata = 0;
int skip = 0;
//int vbat = 0xee;
int display;
int push = 0;
int delay = 0;

#define INFO __attribute__((section(".infob")))
/* Setup parameters */
int info[] INFO = {0,0,0,0,1};
#define DOFF 3
int * const _brightness __INFOD = &info[DOFF];
int * const _speed __INFOD = &info[DOFF+1];

int brightness;
int speed;

void store() {
	dint();
	/* Erase memory first */
	FCTL1 = FWKEY+ERASE;
	FCTL3 = FWKEY;
	(*_brightness) = 0;
	/* Done, switch to writing */
	FCTL1 = FWKEY+WRT;
	(*_brightness) = brightness;
	(*_speed) = speed;
	FCTL1 = FWKEY;
	FCTL3 = FWKEY+LOCK;
	eint();
}

int main() {
	/* Setup clock and disable watchdog */
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	WDTCTL = WDTPW + WDTHOLD;

	/* Setup the flash controller */
 	FCTL2 = FWKEY+FSSEL_1+FN1; /* MCLK / 3 */

	/* Set ACL to VLO */
	BCSCTL3 = LFXT1S1;

	/* Setup watchdog as timer */
	WDTCTL = WDTPW + WDTTMSEL + WDTSSEL + WDTIS1; 
	IE1 |= WDTIE;

	/* Set the LED pins to output. They are P1.x and P2.7*/
	P1DIR |= 0xff;
	P1OUT |= 0xff;
	P2DIR |= BIT7;
	P2SEL &= ~BIT7;
	P2OUT |= BIT7;

	/* Setup P2.6 for PWM output */
	P2DIR |= BIT6;
	P2SEL |= BIT6;

	/* Load the store configuration */
	brightness = (*_brightness);
	speed = (*_speed);

	/* Set the the PWM output */
	TACCTL1 = OUTMOD_7;
	CCR0 = 1000;
	CCR1 = 999-brightness*10;
	TACTL = TASSEL_2 + MC_1;

	/* Setup the ADC */
	//ADC10CTL1 = INCH_12 + ADC10DIV_3 + ADC10SSEL1 ;         // Channel 3, ADC10CLK/3, ACLK
	//ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE + ADC10SR + REFON + REF2_5V;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable

	/* Enable interrupts! */
	eint();

	_BIS_SR(CPUOFF);
}

/* TODO
 *
 * - button sense
 * - theme set (short press): running doubles, inverse doubles, binary counter, blinking, shining, morse, braile, semaphore, ascii
 * - configuration (long press)
 *   - brightness (B):
 *   - speed (S):
 *   - random mode (R):
 *   - battery voltage (U):
 */


int braille()  {
	prgcnt++;
	if (prgcnt == sizeof(prg3)/sizeof(int)) prgcnt = 0;
	return prg3[prgcnt];
}

int running_doubles()  {
	prgcnt++;
	if (prgcnt == sizeof(prg1)/sizeof(int)) prgcnt = 0;
	return prg1[prgcnt];
}

int knightrider() {
	prgcnt++;
	if (prgcnt == 16) prgcnt = 0;
	if (prgcnt > 8) {
		return ~(7 << (14-prgcnt+1));
	} else {
		return ~(7 << (prgcnt-1));
	}
}

int shine() {
	return 0x0;
}

unsigned char morseflag = 0;
unsigned char morseshift = 0;
int morse() {
	morseflag ^= 1;
	if (!(morseflag & 1)) {
		return ~0;
	}
	/* Prg data has stopbit. It stars with zero before the first letter, and ends with every 1 */
	if (prgdata <= 1) {
		prgdata = prg2[prgcnt++];
		delay = 6;
		if (prgcnt > sizeof(prg2)) {
			prgcnt = 0;
			morseflag = 0;
			delay += 6;
		}
		return ~0;
	}
	if (morseshift > 8) morseshift -= 9;
	if (prgdata & 1) {	/* Show dash */
		prgdata >>= 1;
		delay = 2;
		morseshift += 2;
		if (morseshift == 10) return ~(0x101);
		return ~(3 << (morseshift-2));
	} else {	/* Show dot */
		prgdata >>= 1;
		morseshift++;
		return ~(1 << (morseshift-1));
	}
}


int (* const programs[])() __INFOC = { shine, knightrider, running_doubles, morse, braille };
const int prg_skip[] __INFOD = { 0xffff, 3, 1, 4, 5 };
int program = 0;
int mode = 0;
int setup_mode = 0;

interrupt(WDT_VECTOR) WDT_ISR(void) {
	/* The PROGRAM mode */
	if (mode == 0) {
		P2DIR &= ~BIT7;
		skip++;
		int skiplimit = prg_skip[program]-speed;
		if (skiplimit < 1) skiplimit = 1;
		if (skip % skiplimit == 0) {
			if (delay <= 0) {
				display = programs[program]();
			} else delay--;
		}
		/*if (skip % 100 == 0) {
			ADC10CTL0 |= ENC +ADC10SC;
		} */
		//display=~vbat;

		/* Read the button! */
		if (P2IN & BIT7) {
			if (push == 3) {
				/* Switch program on first press. */
				program++;
				if (program == sizeof(programs)/sizeof(void*)) program = 0;
				prgcnt = 0;
			}
			push++;
			/* If it's pressed long enough, enter setup mode */
			if (push > 10) {
				mode = 1;
				display = ~0;
				P2IE |= BIT7;
				P2IES |= BIT7;
				push = 0;
				setup_mode = 1;
				display = ~setup_mode;
				while (P2IN & BIT7);
				__delay_cycles(1000);
				goto setup;
			}
		}
		if (((P2IN & BIT7) == 0) && push) {
			push = 0;
		}
		P2DIR |= BIT7;
	}

	/* The SETUP mode; most of it is in the interrupt handler though */
	if (mode == 1) {
		setup:;
		if (P2IN & BIT7) { 
			push++;
			if (push >= 30) {
					/* Exit setup  */
					P2IE &= ~BIT7;
					mode = 0;
					push = 0;
					display = ~0;
					push = -30;
					/* Store the configuration */
					store();
			} else if (push == 10) { 
					setup_mode++;
					if (setup_mode >= 4) setup_mode = 1;
			}
		} else {
			if (push >= 10) push = 0;
			if (push >= 3) {
				switch (setup_mode) {
					case 1: /* Brightness */
						brightness++;
						if (brightness >= 4) brightness = 0;
						CCR1 = 999-brightness*10;
						break;
					case 2: /* Speed */
						speed++;
						if (speed >= 4) speed = 0;
						break;
					case 3: /* Stuff? */
						break;

				}
			}
			push = 0;
		}
		switch (setup_mode) {
			case 1:
						display = 1 << (brightness+4);
						break;
			case 2:
						display = 1 << (speed+4);
						break;
			case 3:
						display = 0;
						break;
		}
		display |= 1 << (setup_mode-1);
		display = ~display;
	}

	/* Flush the output */
	P1OUT = display & 0xff;
	if (display & 0x100) P2OUT |= BIT7;
	else P2OUT &= ~BIT7;
}

/*
interrupt(ADC10_VECTOR) ADC_ISR (void) {
	vbat = ADC10MEM >> 1;
}
*/
