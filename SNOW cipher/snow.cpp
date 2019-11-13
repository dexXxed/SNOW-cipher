﻿/*
 * REFERENCE IMPLEMENTATION OF STREAM CIPHER SNOW 1.0
 *
 * Filename: snow.c
 *
 * Authors:
 * Patrik Ekdahl & Thomas Johansson
 * Dept. of Information Technology
 * P.O. Box 118
 * SE-221 00 Lund, Sweden,
 * email: {patrik,thomas}@it.lth.se
 *
 * Synopsis:
 *  This file contains functions that implement the
 *  stream cipher SNOW. At all time, the cipher is
 *  clocked directly after a running key has been read out.
 *  This means that between consecutive calls to snow_runningkey()
 *  the cipher will already be clocked and the next running key
 *  word is "outfrom_fsm^*(ptr+S16)"
 *
 *  Interface: Two functions are declared as external, snow_loadkey and snow_keystream.
 *     Check snow.h for calling conventions. Each program using snow.c should
 *     use snow.h as header.
 */

#include "snow.h"
#include "snowtab.h"



 /*
  * GF2^32 generated by f(x)=x^32+x^29+x^20+x^15+x^10+x+1
  * GF(2^32)^16 generated by g(t)=t^16+t^13+t^7+a^(-1)
  * where a is the primitive root of f.
  * The register is taped on t^1 down to the blender.
  *
  */
#define highbit 0x80000000
#define alphaxor 0x20108403
#define LFSRLEN  16
#define S1       1
#define S7       7
#define S13      13
#define S16      16


  /* By compiling with option -DUSEINLINE some routines can be inlined */
#ifdef USEINLINE
#define INLINE
#else
#define INLINE
#endif


typedef unsigned long u32;
typedef unsigned char u8;


/* Global variable declaration */

u32 lfsr[2 * LFSRLEN];  /* implemented using "sliding window" technique */
u32* ptr;
u32 r1, r2;  /* FSM registers */
u32 outfrom_fsm;
u32 next_r1, next_r2;



/*
 * Function: snow_update_internals
 *
 * Synopsis:
 *   Update all internal values and produced keystream word.
 *   Typically called after a clocking of the cipher, as done
 *   in snow_clock.
 *
 * Returns: void
 */
INLINE void  snow_update_internals() {
	u32 tmp;
	outfrom_fsm = (r1 + *(ptr + S1)) ^ r2;
	tmp = outfrom_fsm + r2;
	tmp = ((tmp << 7) | (tmp >> 25));
	next_r1 = tmp ^ r1;
	next_r2 = SBox_0[r1 & 0xff] | SBox_1[(r1 >> 8) & 0xff] |
		SBox_2[(r1 >> 16) & 0xff] | SBox_3[(r1 >> 24) & 0xff];
} /* end of snow_update_internals() */

/*------------------------------*/

/*
 * Function: snow_clock
 *
 * Synopsis:
 *    Calculate new LFSR symbol and update the LFSR and FSM registers.
 *
 * Returns: void
 */
INLINE void snow_clock() {
	u32 feedback;
	/* update LFSR first */
	feedback = *(ptr + S7) ^ *(ptr + S13) ^ *(ptr + S16);
	if (feedback & highbit) feedback = (feedback << 1) ^ alphaxor;
	else feedback = (feedback << 1);
	*ptr = *(ptr + LFSRLEN) = feedback;
	if (ptr == lfsr) ptr = lfsr + 15; else ptr--;

	/* then update fsm registers */
	r1 = next_r1;
	r2 = next_r2;
}/* end of snow_clock() */

/*-------------------------------*/


/*
 * Function: snow_feedback_clock
 *
 * Synopsis:
 *    Calculate new LFSR symbol when the output from the FSM
 *    is used in the feedback loop, and update the LFSR and FSM.
 *
 * Returns: void
 */
INLINE void snow_feedback_clock() {
	u32 feedback;
	/* update LFSR first */
	feedback = *(ptr + S7) ^ *(ptr + S13) ^ *(ptr + S16) ^ outfrom_fsm;
	if (feedback & highbit) feedback = (feedback << 1) ^ alphaxor;
	else feedback = (feedback << 1);
	*ptr = *(ptr + LFSRLEN) = feedback;
	if (ptr == lfsr) ptr = lfsr + 15; else ptr--;

	/* then update fsm registers */
	r1 = next_r1;
	r2 = next_r2;
} /* end of snow_feedback_clock() */

/*----------------------------------*/

/*
 * Function:  snow_loadkey
 *
 * Synopsis:
 *   Loads the key material and performs the initial mixing.
 *
 * Returns: void
 *
 * Assumptions:
 *   keysize is either 128 or 256.
 *   mode is either STANDARD_MODE or IV_MODE as defined in snow.h
 *   key is of proper length, for keysize=128, key is of lenght 16 bytes
 *      and for keysize=256, key is of length 32 bytes.
 *   key is given in big endian format,
 *   thus key[0]->msb of lfsr[0]
 *        key[1]->second msb of lfsr[0]
 *         ...
 *        key[3]-> lsb of lfsr[0]
 *         ...
 *        key[keysize/8-1] -> lsb of lfsr[keysize/32-1]
 */
void snow_loadkey(u8* key, u32 keysize, int mode, u32 IV2, u32 IV1)
{
	int i;

	if (keysize == 128) {
		lfsr[0] = (((u32) * (key + 0)) << 24) ^ (((u32) * (key + 1)) << 16) ^
			(((u32) * (key + 2)) << 8) ^ (((u32) * (key + 3)));
		lfsr[1] = (((u32) * (key + 4)) << 24) ^ (((u32) * (key + 5)) << 16) ^
			(((u32) * (key + 6)) << 8) ^ (((u32) * (key + 7)));
		lfsr[2] = (((u32) * (key + 8)) << 24) ^ (((u32) * (key + 9)) << 16) ^
			(((u32) * (key + 10)) << 8) ^ (((u32) * (key + 11)));
		lfsr[3] = (((u32) * (key + 12)) << 24) ^ (((u32) * (key + 13)) << 16) ^
			(((u32) * (key + 14)) << 8) ^ (((u32) * (key + 15)));
		lfsr[4] = ~lfsr[0]; /* bitwise inverse */
		lfsr[5] = ~lfsr[1];
		lfsr[6] = ~lfsr[2];
		lfsr[7] = ~lfsr[3];
		lfsr[8] = lfsr[0]; /* just copy */
		lfsr[9] = lfsr[1];
		lfsr[10] = lfsr[2];
		lfsr[11] = lfsr[3];
		lfsr[12] = ~lfsr[0]; /* bitwise inverse */
		lfsr[13] = ~lfsr[1];
		lfsr[14] = ~lfsr[2];
		lfsr[15] = ~lfsr[3];
	}
	else {  /* assume keysize=256 */
		lfsr[0] = (((u32) * (key + 0)) << 24) ^ (((u32) * (key + 1)) << 16) ^
			(((u32) * (key + 2)) << 8) ^ (((u32) * (key + 3)));
		lfsr[1] = (((u32) * (key + 4)) << 24) ^ (((u32) * (key + 5)) << 16) ^
			(((u32) * (key + 6)) << 8) ^ (((u32) * (key + 7)));
		lfsr[2] = (((u32) * (key + 8)) << 24) ^ (((u32) * (key + 9)) << 16) ^
			(((u32) * (key + 10)) << 8) ^ (((u32) * (key + 11)));
		lfsr[3] = (((u32) * (key + 12)) << 24) ^ (((u32) * (key + 13)) << 16) ^
			(((u32) * (key + 14)) << 8) ^ (((u32) * (key + 15)));
		lfsr[4] = (((u32) * (key + 16)) << 24) ^ (((u32) * (key + 17)) << 16) ^
			(((u32) * (key + 18)) << 8) ^ (((u32) * (key + 19)));
		lfsr[5] = (((u32) * (key + 20)) << 24) ^ (((u32) * (key + 21)) << 16) ^
			(((u32) * (key + 22)) << 8) ^ (((u32) * (key + 23)));
		lfsr[6] = (((u32) * (key + 24)) << 24) ^ (((u32) * (key + 25)) << 16) ^
			(((u32) * (key + 26)) << 8) ^ (((u32) * (key + 27)));
		lfsr[7] = (((u32) * (key + 28)) << 24) ^ (((u32) * (key + 29)) << 16) ^
			(((u32) * (key + 30)) << 8) ^ (((u32) * (key + 31)));
		lfsr[8] = ~lfsr[0]; /* bitwise inverse */
		lfsr[9] = ~lfsr[1];
		lfsr[10] = ~lfsr[2];
		lfsr[11] = ~lfsr[3];
		lfsr[12] = ~lfsr[4];
		lfsr[13] = ~lfsr[5];
		lfsr[14] = ~lfsr[6];
		lfsr[15] = ~lfsr[7];
	}
	if (mode == IV_MODE) {   /* XOR IV values */
		lfsr[0] = lfsr[0] ^ IV1;
		lfsr[3] = lfsr[3] ^ IV2;
	}


	/* update second half of lfsr for the sliding window implementation */
	for (i = 0; i < LFSRLEN; i++)
		lfsr[i + LFSRLEN] = lfsr[i];

	r1 = 0;
	r2 = 0;

	ptr = lfsr + 15;  /* start with ptr->the register that will be updated */

	snow_update_internals();
	for (i = 0; i < mode; i++) {
		snow_feedback_clock();
		snow_update_internals();
	}
} /* end of snow_loadkey */

/*------------------------------*/

/*
 * Function: snow_keystream
 *
 * Synopsis:
 *   Produces a running key word and updates the lfsr and fsm.
 *
 * Returns: the running key word
 *
 */

u32 snow_keystream() {
	u32 runningkey;

	runningkey = outfrom_fsm ^ *(ptr + S16);
	snow_clock();
	snow_update_internals();

	return(runningkey);

} /* end of snow_keystream */
/*--------------------------*/
