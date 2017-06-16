/******************************************************************************
 * Filename: program.h
 *
 * Author: Ladislav Láska
 * e-mail: krakonos@krakonos.org
 *
 * This file contains program definitions.
 *
 ******************************************************************************/

#define MO_A 0b0000101 
#define MO_B 0b0011000 
#define MO_C 0b0011010 
#define MO_D 0b0001100 
#define MO_E 0b0000010 
#define MO_F 0b0010010 
#define MO_G 0b0001110 
#define MO_H 0b0010000 
#define MO_I 0b0000100 
#define MO_J 0b0010111 
#define MO_K 0b0001101 
#define MO_L 0b0010100 
#define MO_M 0b0000111 
#define MO_N 0b0000110 
#define MO_O 0b0001111 
#define MO_P 0b0010110 
#define MO_Q 0b0011101 
#define MO_R 0b0001010 
#define MO_S 0b0001000 
#define MO_T 0b0000011 
#define MO_U 0b0001001 
#define MO_V 0b0010001 
#define MO_W 0b0001011 
#define MO_X 0b0011001 
#define MO_Y 0b0011011 
#define MO_Z 0b0011100 

#define BR_A (~0b00000010)
#define BR_B (~0b00000110)
#define BR_C (~0b01000010)
#define BR_D (~0b01100010)
#define BR_E (~0b00100010)
#define BR_F (~0b01000110)
#define BR_G (~0b01100110)
#define BR_H (~0b00100110)
#define BR_I (~0b01000100)
#define BR_J (~0b01100100)
#define BR_K (~0b00001010)
#define BR_L (~0b00001110)
#define BR_M (~0b01001010)
#define BR_N (~0b01101010)
#define BR_O (~0b00101010)
#define BR_P (~0b01001110)
#define BR_Q (~0b01101110)
#define BR_R (~0b00101110)
#define BR_S (~0b01001100)
#define BR_T (~0b01101100)
#define BR_U (~0b00011010)
#define BR_V (~0b00011110)
#define BR_W (~0b01110100)
#define BR_X (~0b01011010)
#define BR_Y (~0b01111010)
#define BR_Z (~0b00111010)



const int prg1[] __INFOC = {
		~3, ~(3 << 1), ~(3 << 2), ~(3 << 3), ~(3 << 4), ~(3 << 5), ~(3 << 6), ~(3 << 7), ~((3 << 8) + 1)
};

const char prg2[] __INFOC = {
		MO_H, MO_E, MO_L, MO_L, MO_O, MO_W, MO_O, MO_R, MO_L, MO_D
};

const int prg3[] __INFOC = {
		BR_K, BR_R, BR_A, BR_K, BR_O, BR_N, BR_O, BR_S
};

