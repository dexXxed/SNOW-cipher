/*
 * REFERENCE IMPLEMENTATION OF STREAM CIPHER SNOW 1.0
 *
 * Filename: testvectors.c
 *
 * Authors:
 * Patrik Ekdahl & Thomas Johansson
 * Dept. of Information Technology
 * P.O. Box 118
 * SE-221 00 Lund, Sweden,
 * email: {patrik,thomas}@it.lth.se
 *
 * Synopsis:
 *    Generates testvectors from the reference implementation of SNOW 1.0.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snow.h"



#define U32TO8_BIG(c,v) do {\
		u32 x = (v);\
		u8 *d = (c);\
		d[0]=(u8)(x>>24);\
		d[1]=(u8)(x>>16);\
		d[2]=(u8)(x>>8);\
		d[3]=(u8)(x);\
		} while (0)



typedef unsigned long u32;
typedef unsigned char u8;



void print_data(char* str, u8* val, int len)
{
	int i;

	static char* hex = "0123456789ABCDEF";

	printf("%25s=", str);

	for (i = 0; i < len; i++) {
		putchar(hex[(val[i] >> 4) & 0xF]);
		putchar(hex[(val[i]) & 0xF]);
	}
	putchar('\n');
}


void testvectors()
{
	u32 i;
	u8 key[32];
	u8 keystream[4];
	u32 IV1, IV2;

	/*
	 * 128 bits key
	 */
	printf("Test vectors for SNOW 1.0, 128 bit key, standard mode\n");
	printf("Each key is given in bigendian format (MSB...LSB) in hexadecimal\n");
	printf("==================\n\n");
	IV1 = IV2 = 0;
	memset(key, 0, 16);
	key[0] = 0x80;
	snow_loadkey(key, 128, STANDARD_MODE, IV2, IV1);
	//  printf("        (IV2,IV1)=(%lu,%lu)\n",IV2,IV1);
	print_data("key", key, 16);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("==================\n\n");
	IV1 = IV2 = 0;
	memset(key, 0xaa, 16);
	snow_loadkey(key, 128, STANDARD_MODE, IV2, IV1);
	//  printf("        (IV2,IV1)=(%lu,%lu)\n",IV2,IV1);
	print_data("key", key, 16);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("=========== End of test vectors =========\n\n");

	printf("Test vectors for SNOW 1.0, 128 bit key, IV mode\n");
	printf("Each key is given in bigendian format (MSB...LSB) in hexadecimal\n");
	printf("==================\n\n");
	memset(key, 0, 16);
	key[0] = 0x80;
	IV1 = 0xaaaaaaaa;
	IV2 = 0x01234567;
	snow_loadkey(key, 128, IV_MODE, IV2, IV1);
	printf("        (IV2,IV1)=(0x%lx,0x%lx)\n", IV2, IV1);
	print_data("key", key, 16);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("==================\n\n");
	IV1 = 0xabcdef01;
	IV2 = 0x10203040;
	memset(key, 0xaa, 16);
	snow_loadkey(key, 128, IV_MODE, IV2, IV1);
	printf("        (IV2,IV1)=(0x%lx,0x%lx)\n", IV2, IV1);
	print_data("key", key, 16);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("=========== End of test vectors =========\n\n");

	/*
	 * 256 bits key
	 */
	printf("Test vectors for SNOW 1.0, 256 bit key, standard mode\n");
	printf("Each key is given in bigendian format (MSB...LSB) in hexadecimal\n");
	printf("==================\n\n");
	IV1 = IV2 = 0;
	memset(key, 0, 32);
	key[0] = 0x80;
	snow_loadkey(key, 256, STANDARD_MODE, IV2, IV1);
	//  printf("        (IV2,IV1)=(%lu,%lu)\n",IV2,IV1);
	print_data("key", key, 32);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("==================\n\n");
	IV1 = IV2 = 0;
	memset(key, 0xaa, 32);
	snow_loadkey(key, 256, STANDARD_MODE, IV2, IV1);
	//  printf("        (IV2,IV1)=(%lu,%lu)\n",IV2,IV1);
	print_data("key", key, 32);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("=========== End of test vectors =========\n\n");

	printf("Test vectors for SNOW 1.0, 256 bit key, IV mode\n");
	printf("Each key is given in bigendian format (MSB...LSB) in hexadecimal\n");
	printf("==================\n\n");
	memset(key, 0, 32);
	key[0] = 0x80;
	IV1 = 0xaaaaaaaa;
	IV2 = 0x01234567;
	snow_loadkey(key, 256, IV_MODE, IV2, IV1);
	printf("        (IV2,IV1)=(0x%lx,0x%lx)\n", IV2, IV1);
	print_data("key", key, 32);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("==================\n\n");
	IV1 = 0xabcdef01;
	IV2 = 0x10203040;
	memset(key, 0xaa, 32);
	snow_loadkey(key, 256, IV_MODE, IV2, IV1);
	printf("        (IV2,IV1)=(0x%lx,0x%lx)\n", IV2, IV1);
	print_data("key", key, 16);
	printf("Keystream output 1...16:\n");
	for (i = 0; i < 0x10; i++) {
		U32TO8_BIG(keystream, snow_keystream());
		print_data("keystream", keystream, 4);
	}
	printf("=========== End of test vectors =========\n\n");


}




int main(int argc, char** argv)
{
	printf("REFERENCE IMPLEMENTATION\n");
	testvectors();
	return(0);
}
