#pragma once

#define STANDARD_MODE 64
#define IV_MODE 32

/*
 * �������:  snow_loadkey
 *
 * ��������������:
 *   ��������� �������� ����� � ��������� �������������� �������������.
 *
 * ����������: void
 *
 * ���������� ��������:
 *   ������ ����� 128 ��� 256 ���
 *   ����� ��� STANDARD_MODE, ��� IV_MODE
 *   ���� ������ � ������ �������� ����,
 *   ����� �������	key[0]->msb of lfsr[0]
 *					key[1]->second msb of lfsr[0]
 *						...
 *					key[3]-> lsb of lfsr[0]
 *						...
 *					key[keysize/8-1] -> lsb of lfsr[keysize/32-1]
 */
extern void snow_loadkey(unsigned char* key,
	unsigned long keysize, int mode,
	unsigned long  IV2, unsigned long IV1);


/*
 * �������: snow_keystream
 *
 * ��������������:
 *   ������� ������� �������� ����� � ��������� lfsr � fsm.
 *
 * ����������: �������� �����
 *
 */
extern unsigned long snow_keystream();
