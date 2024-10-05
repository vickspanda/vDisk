#include<stdio.h>
#include<stdlib.h>

/* 
 * decode(FILE*)			
 * returns the actual data which was encoded in that respective sequence
 */
unsigned long long int decode(FILE *encodedSeqPointer)
{
	unsigned char out = fgetc(encodedSeqPointer);
	union decStore data; 		/* defined in helper.c */
	unsigned char k = 0, length = 2, checkBit = 0, remBits = 7, value = 0, size, remBitsForData;
	data.num = 0;
	while(1)			/* while will be running till actual data is not retrieved */
	{
		/*
		 * if handles the condition if checkBit is 0,
		 * It implies that till now we have not got actual length of the data
		 */
		if(checkBit == 0){
			/* It is handling the condition, to get the value stored in the next length bits */
			if(length != 0){
				value = value << 1;
				value = value | (1 & (out >> (remBits)));
				length--;
			}
			/* It is checking the checkbit, and assigning the new length */
			else
			{
				checkBit = 0;
				checkBit = 1 & (out >> (remBits));
				length = value;
				value = 0;
			}
		}
		/*
                 * else handles the condition if checkBit is 1,
                 * It implies that till now we have got actual length of the data and can start the retrieval of the data
                 */
		else
		{
			/*
			 * size is the number of Bytes required to store the data of length bits
			 */
			size = length/8 + 1;
			/*
			 * remBitsForData is nothing but the number of bits will be remained and have to add 0 in those rest of the places
			 */
			remBitsForData = length%8;
			/* If remBitsForData is zero than size will be decremented and remBitsForData be updated to 7;
			 * it is required so that we can store the data in data.numInChars[] in little Endian Format
			 */
			if(remBitsForData==0)
			{
				size--;
				remBitsForData = 7;
			}
			
			/*
			 * This if handles to read all length bits to get data
			 */
			if(length!=0)
			{
				data.numInChar[size-1] = data.numInChar[size-1] << 1;
				data.numInChar[size-1] = data.numInChar[size-1] | (1 & (out >> (remBits)));
				length--;
				remBitsForData--;
			}
			/* when the data will be retrieved, then automatically will get out of the while loop */
			else
				break;
		}
		/* if remBits becomes zero, it will be updated to 8 and offset for out will also get incremented, 
		 * in every iteration remBits is getting decremented
		 */
		if(remBits==0){
			remBits = 8;
			out = fgetc(encodedSeqPointer);
		}
		remBits--;
	}
	return data.num;
}
