#include "barr.h"
#include <assert.h>

size_t BitsArrayCountOn(unsigned long bit_array)
{
	size_t counter = 0;

	while (bit_array)
	{
		counter += bit_array & 1;

		bit_array >>= 1;
	}

	return counter;
}

size_t BitsArrayCountOff(unsigned long bit_array)
{
	return (LENGTH - BitsArrayCountOn(bit_array));
}

int BitsArrayIsOn(unsigned long bit_array, size_t index)
{
	assert(index > 0 && index < LENGTH);

	return ((bit_array >> index) & 1UL);
}

int BitsArrayIsOff(unsigned long bit_array, size_t index)
{
	assert(index > 0 && index < LENGTH);

	return (!((bit_array >> index) & 1UL));
}

unsigned long BitsArraySetOn(unsigned long bit_array, size_t index)
{
	unsigned long mask = 1;

	assert(index > 0 && index < LENGTH);

	mask <<= index;

	return (bit_array | mask);
}

unsigned long BitsArraySetOff(unsigned long bit_array, size_t index)
{
	unsigned long mask = 1;

	assert(index > 0 && index < LENGTH);

	if (1 == BitsArrayIsOff(bit_array, index))
	{
		return bit_array;
	}

	mask <<= index;

	return (bit_array ^ mask);
}

unsigned long BitsArraySetBit(unsigned long bit_array, size_t index, int value)
{
	assert(index > 0 && index < LENGTH);
	assert(value == 1 || value == 0);

	if (1 == value)
	{
		return (BitsArraySetOn(bit_array, index));
	}
	
	return (BitsArraySetOff(bit_array, index));
}

unsigned long BitsArrayFlipBit(unsigned long bit_array, size_t index)
{
	unsigned long mask = 1;

	assert(index > 0 && index < LENGTH);

	mask <<= index;

	return (bit_array ^ mask);
}

unsigned long BitsArrayRotL(unsigned long bit_array, size_t number_of_shifts)
{
	unsigned long a = (bit_array >> (LENGTH - number_of_shifts));

	assert(number_of_shifts < LENGTH);

	return ((bit_array << number_of_shifts) | a);
}

unsigned long BitsArrayRotR(unsigned long bit_array, size_t number_of_shifts)
{
	assert(number_of_shifts < LENGTH);

	return ((bit_array >> number_of_shifts) | 
		bit_array << (LENGTH - number_of_shifts));
}

unsigned long BitsArrayMirror(unsigned long bit_array)
{
	bit_array = (((bit_array & 0xaaaaaaaaaaaaaaaa) >> 1)  | ((bit_array & 0x5555555555555555) << 1));
 	bit_array = (((bit_array & 0xcccccccccccccccc) >> 2)  | ((bit_array & 0x3333333333333333) << 2));
 	bit_array = (((bit_array & 0xf0f0f0f0f0f0f0f0) >> 4)  | ((bit_array & 0x0f0f0f0f0f0f0f0f) << 4));
 	bit_array = (((bit_array & 0xff00ff00ff00ff00) >> 8)  | ((bit_array & 0x00ff00ff00ff00ff) << 8));
 	bit_array = (((bit_array & 0xffffffff00000000) >> 16) | ((bit_array & 0x00000000ffffffff) << 16));
 	 
	return ((bit_array >> 32) | (bit_array << 32));
}

void BitsArrayPrintBinary(unsigned long bit_array)
{
	int i = LENGTH;
	
	for(; i > 0; i--)
	{
		printf("%lu", (bit_array >> i) & 1UL);
	}
}

unsigned long BitsArrayCountOnLUT(unsigned long bit_array)
{
	unsigned int i = 0, res = 0;
	static char arr[256] = {0};
	static int flag = 0;

	if (0 == flag)
	{
		for (; i < 256; i++)
		{
			arr[i] = BitsArrayCountOn(i);
		}

		flag = 1;
	}

	for (i=0; i < 7; i++)
	{
		res += (unsigned int)arr[((bit_array >> i * 8) & 0x00000000000000FF)];
	}

	return res;
}

unsigned long BitsArrayMirrorLUT(unsigned long bit_array)
{
	unsigned long res = 0;
	static unsigned char arr[256] = {0};
	static int flag = 0;
	unsigned int helper = 0, i = 0;

	if (0 == flag)
	{
		for (; i < 256; i++)
		{
			helper = (((helper & 0xaaaaaaaa) >> 1)  | ((helper & 0x55555555) << 1));
	 		helper = (((helper & 0xcccccccc) >> 2)  | ((helper & 0x33333333) << 2));
 			helper = (((helper & 0xf0f0f0f0) >> 4)  | ((helper & 0x0f0f0f0f) << 4));
			arr[i] = helper;
			helper = i + 1;
		}

		flag = 1;
	}

	for (i=0; i < 7; i++)
	{
		helper = arr[bit_array & 0x00000000000000ff];

		res |= helper;

		bit_array >>= 8;
		
		res <<= 8;
	}

	return res;
}

