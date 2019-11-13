#pragma once
/*
 * REFERENCE IMPLEMENTATION OF THE STREAM CIPHER SNOW 1.0
 *
 * Filename: snow.h
 *
 * Authors:
 * Patrik Ekdahl & Thomas Johansson
 * Dept. of Information Technology
 * P.O. Box 118
 * SE-221 00 Lund, Sweden,
 * email: {patrik,thomas}@it.lth.se
 *
 * Synopsis:
 *  Header file for snow.c
 *
 */


#define STANDARD_MODE 64
#define IV_MODE 32


 /* Exported functions */
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
extern void snow_loadkey(unsigned char* key,
	unsigned long keysize, int mode,
	unsigned long  IV2, unsigned long IV1);


/*
 * Function: snow_keystream
 *
 * Synopsis:
 *   Produces a running key word and updates the lfsr and fsm.
 *
 * Returns: the running key word
 *
 */

extern unsigned long snow_keystream();
