#include "snow.h"
#include "snowtab.h"

/*
 * GF2^32 генерируется с помощью f(x)=x^32+x^29+x^20+x^15+x^10+x+1
 * GF(2^32)^16 генерируется с помощью g(t)=t^16+t^13+t^7+a^(-1)
 * где a примитивный корень f.
 */
#define highbit	 0x80000000
#define alphaxor 0x20108403
#define LFSRLEN  16
#define S1       1
#define S7       7
#define S13      13
#define S16      16

/* Компилируя с флагом -DUSEINLINE некоторые рутины могут быть записаны в одну строку */
#ifdef USEINLINE
#define INLINE
#else
#define INLINE
#endif


typedef unsigned long u32;
typedef unsigned char u8;


/* Объявления глобальных переменных */

u32 lfsr[2 * LFSRLEN];  /* реализовано с помощью техники "скользящего окна" */
u32* ptr;
u32 r1, r2;  /* FSM регистры */
u32 outfrom_fsm;
u32 next_r1, next_r2;


/*
 * Функция: snow_update_internals
 *
 * Предназначение:
 *   Обновляем все внутренние значения и производим ключевое слово.
 *   Обычно вызывается после шифрования, как это сделано
 *   в snow_clock.
 *
 * Возвращает: void
 */
INLINE void  snow_update_internals() {
	u32 tmp;
	outfrom_fsm = (r1 + *(ptr + S1)) ^ r2;
	tmp = outfrom_fsm + r2;
	tmp = ((tmp << 7) | (tmp >> 25));
	next_r1 = tmp ^ r1;
	next_r2 = SBox_0[r1 & 0xff] | SBox_1[(r1 >> 8) & 0xff] |
		SBox_2[(r1 >> 16) & 0xff] | SBox_3[(r1 >> 24) & 0xff];
} 

/*
 * Функция: snow_clock
 *
 * Предназначение:
 *    Вычисляем новый символ LFSR и обновляем регистры LFSR и FSM.
 *
 * Возвращает: void
 */
INLINE void snow_clock() {
	u32 feedback;
	/* обновляем LFSR сначала */
	feedback = *(ptr + S7) ^ *(ptr + S13) ^ *(ptr + S16);
	if (feedback & highbit) feedback = (feedback << 1) ^ alphaxor;
	else feedback = (feedback << 1);
	*ptr = *(ptr + LFSRLEN) = feedback;
	if (ptr == lfsr) ptr = lfsr + 15; else ptr--;

	/* и обновляем FSM регистры */
	r1 = next_r1;
	r2 = next_r2;
}

/*
 * Функция: snow_feedback_clock
 *
 * Предназанчение:
 *    Рассчитываем новый символ LFSR, когда выходной сигнал от FSM используется
 *	  в контуре обратной связи, и обновляем LFSR и FSM.
 *
 * Возвращает: void
 */
INLINE void snow_feedback_clock() {
	u32 feedback;
	/* обновляем LFSR сначала */
	feedback = *(ptr + S7) ^ *(ptr + S13) ^ *(ptr + S16) ^ outfrom_fsm;
	if (feedback & highbit) feedback = (feedback << 1) ^ alphaxor;
	else feedback = (feedback << 1);
	*ptr = *(ptr + LFSRLEN) = feedback;
	if (ptr == lfsr) ptr = lfsr + 15; else ptr--;

	/* и обновляем FSM регистры */
	r1 = next_r1;
	r2 = next_r2;
} 

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
		lfsr[4] = ~lfsr[0]; /* побитовая инверсия */
		lfsr[5] = ~lfsr[1];
		lfsr[6] = ~lfsr[2];
		lfsr[7] = ~lfsr[3];
		lfsr[8] = lfsr[0]; /* простое копирование */
		lfsr[9] = lfsr[1];
		lfsr[10] = lfsr[2];
		lfsr[11] = lfsr[3];
		lfsr[12] = ~lfsr[0]; /* побитовая инверсия */
		lfsr[13] = ~lfsr[1];
		lfsr[14] = ~lfsr[2];
		lfsr[15] = ~lfsr[3];
	}
	else {  /* предпологаем, что размер ключа равен 256 */
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
		lfsr[8] = ~lfsr[0]; /* побитовая нверсия */
		lfsr[9] = ~lfsr[1];
		lfsr[10] = ~lfsr[2];
		lfsr[11] = ~lfsr[3];
		lfsr[12] = ~lfsr[4];
		lfsr[13] = ~lfsr[5];
		lfsr[14] = ~lfsr[6];
		lfsr[15] = ~lfsr[7];
	}
	if (mode == IV_MODE) {   /* XOR значений IV */
		lfsr[0] = lfsr[0] ^ IV1;
		lfsr[3] = lfsr[3] ^ IV2;
	}


	/* обновим вторую половину lfsr для реализации "скользящего окна" */
	for (i = 0; i < LFSRLEN; i++)
		lfsr[i + LFSRLEN] = lfsr[i];

	r1 = 0;
	r2 = 0;

	ptr = lfsr + 15;  /* начнем с ptr регистра, который будет обновлен */

	snow_update_internals();
	for (i = 0; i < mode; i++) {
		snow_feedback_clock();
		snow_update_internals();
	}
}

/*
 * Функция: snow_keystream
 *
 * Предназначение:
 *   Создает рабочее ключевое слово и обновляет lfsr и fsm.
 *
 * Возвращает: ключевое слово
 *
 */
u32 snow_keystream() {
	u32 runningkey;

	runningkey = outfrom_fsm ^ *(ptr + S16);
	snow_clock();
	snow_update_internals();

	return(runningkey);

}