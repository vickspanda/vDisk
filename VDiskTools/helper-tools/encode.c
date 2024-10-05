#include<stdio.h>
#include<stdlib.h>

/* 
 * encode(unsigned char *, unsigned char, unsigned char *) is defined in encode.c and it encodes the value present in *in
 * and Stores in *out and also returns the count of Bits required by the Encoded Sequence
 */
unsigned char encode( unsigned char *in, unsigned char length, unsigned char *out){
	
	unsigned char k = 0, remBits = 8, countOfBits = calCountOfBitsSeq(length), bits, size;
	
	/* This while deals with Elements of Singly Linked List along with apending the checkBit 0, and 1 after all the elements of Linked List are encoded successfully */
	while(seqOfBits!=NULL)
	{
		/* calBits returns the count of bits required to store the unsigned char */
		bits = calBits(seqOfBits->data);
		
		/* This if Condition appends the elements if there is bits availbale for it */
        	if(remBits >= bits){
                	*(out + k) = *(out + k) | (seqOfBits->data << (remBits - bits));
        		remBits = remBits - bits;
		}
		
		/* Else it appends the first of the Bits of element in the remaining Bits of Out 
		 * Then Rest of the Bits of element in new out
		 * by incrementing k offset for out
		 * updating rembits as per the new char
		 */
        	else{
                	*(out + k) = *(out + k) | (seqOfBits->data >> (bits - remBits));
			k++;
			*(out + k) = *(out + k) | (seqOfBits->data << (8 - (bits - remBits)));
			remBits = 8 - (bits - remBits);

		}
		
		/* as soon as remBits becomes 0, offset for out will be incremented and remBits will be updated to 8 */
		if(remBits==0)
                {
                        k++;
                        remBits = 8;
                }
                
                /* Appending 0 in the out if next is availble in the LinkedList */
		if(seqOfBits->next!=NULL)
			remBits--;
		/* Appending 1 in out if next is not available in the Linked List */	
		else{
			*(out + k) = *(out + k) | (1 << (remBits - calBits(1)));
			remBits--;
		}
		
		/* as soon as remBits becomes 0, offset for out will be incremented and remBits will be updated to 8 */
		if(remBits==0)
                {
                	k++;
                	remBits = 8;
                }
                
                /* Traversing the next of the Linked List */
		seqOfBits = seqOfBits->next;
	}

	size = length/8;
        if(length%8!=0)
                size++;
        /*
         * size is the number of bytes required to store the data present in *in
	 *
	 *
	 * for loop appends the data in the *out just after checkbit 1
         */
        for(int i = 0; i < size; i++)
        {
		/* 
		 * this if only deals with the first iteration because only 1st byte of in will have garbage bits,
		 * which may not be expected to be appended 
		 * */
		if(i == 0)
		{
			/* calBits returns the count of bits required to store the bits of this Byte */
			bits = calBits(*(in + i));

			/* 
			 * if handles for the condition if remBits is greater than/ equals to the bits required to be appended
			 * and have to avoid the garbage value, so that garbage can't be appended
			 */
			if(remBits >= bits){
				/*
				 * if handles if the data in the current byte requires less than 2 bits and is the only byte to be appended
				 * It means the number is either Zero or One
				 */
				if(bits < 2 && size == (i+1)){
                        		*(out + k) = *(out + k) | (*(in + i) << (remBits - 2));
                		}
				/*
				 * Else handles if the Byte is not the only byte to be appended and 
				 * bits to store the Data of this Byte (*in) is less than
				 * the remBits in current out char
				 */
				else{
					*(out + k) = *(out + k) | (*(in + i) << (remBits - bits));
					remBits = remBits - bits;
				}
			}
			/*
			 * Else handles the condition if remBits is less than the bits required to store the data of current Byte
			 */
                	else{
                        	*(out + k) = *(out + k) | (*(in + i) >> (bits - remBits));
                        	k++;
                        	*(out + k) = *(out + k) | (*(in + i) << (8 - (bits - remBits)));
                        	remBits = 8 - (bits - remBits);
                	}	
		}
		/*
		 * Else handles the condition for the rest of the iterations
		 */
		else{
			/*
			 * If handles the condition where remBits is greater or Equal to 8 bits
			 */
			if(remBits >= 8){
				*(out + k) = *(out + k) | (*(in + i) << (remBits - 8));
				k++;
			}
			/*
			 * Else is handling the scenario where remBits is less than 8 bits
			 */
			else
			{
				*(out + k) = *(out + k) | (*(in + i) >> (8 - remBits));
                        	k++;
                        	*(out + k) = *(out + k) | (*(in + i) << remBits);
			}
		}
	}
	
	/*  
	 * printEncodedSeq prints the Encoded Sequence in Binary Format which got prepared in encode function
	 * It was written just to develop the program and progress of the same
	 
	printf("Encoded Sequence:\t");
	printEncodedSeq(out,countOfBits);*/
	return countOfBits;
}
