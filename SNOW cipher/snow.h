#pragma once

#define STANDARD_MODE 64
#define IV_MODE 32

/*
 * Функция:  snow_loadkey
 *
 * Предназначение:
 *   Загружает материал ключа и выполняет первоначальное перемешивание.
 *
 * Возвращает: void
 *
 * Допустимые значения:
 *   размер ключа 128 или 256 бит
 *   режим или STANDARD_MODE, или IV_MODE
 *   ключ дается с прямым порядком байт,
 *   таким образом	key[0]->msb of lfsr[0]
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
 * Функция: snow_keystream
 *
 * Предназначение:
 *   Создает рабочее ключевое слово и обновляет lfsr и fsm.
 *
 * Возвращает: ключевое слово
 *
 */
extern unsigned long snow_keystream();
