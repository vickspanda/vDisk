#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

/*
 * This is the union which is used to store the unsigned long long int and allows to use that as array of unsigned char
 */
union decStore
{
        unsigned long long int num;
        unsigned char numInChar[sizeof(unsigned long long int)];
};

/* Singly Linked is constructed to store the Seq like if I want to Store A number which takes Four Bits 
 * For Storing 4 bits -> 3 Bits 
 * For Storing 3 bits -> 2 Bits
 * For Storing 2 bits -> 2 Bits
 
 * Then Singly linked List will store the Seq as Follows :
 * 2 -> 3 -> 4 -> NULL
 */

struct sLinkedList{
	unsigned char data;
       	struct sLinkedList *next;
};

// Declarations 
struct sLinkedList *seqOfBits = NULL, *seq = NULL;

/* createNode Function to create a new Node after getting the data and returns the address of the new node*/
struct sLinkedList * createNode(unsigned char input){
	struct sLinkedList * newNode = (struct sLinkedList *)malloc(sizeof(struct sLinkedList));
	newNode->data = input;
	newNode->next = NULL;
	return newNode;
}

/* insert Function to insert the Node at the beginning of the existing Linked list it returns the head of the linked list */
struct sLinkedList * insert(struct sLinkedList * head, unsigned char data)
{
	struct sLinkedList * newNode = createNode(data);
	if(head==NULL)
		head = newNode;
	else
		newNode->next = head;
	seq = newNode;
	return newNode;
}

/* printDLL prints the Data of Singly Linked List and not used in the whole Program 
 * It was written just to develop the program and progress of the same
 */
void printDLL(struct sLinkedList *head)
{
	struct sLinkedList *curNode = head;
	while(curNode!=NULL)
	{
		printf("%d -> ",curNode->data);
		curNode = curNode->next;
	}
	printf("NULL\n");
}

/* bitsToBytes is the Function which takes Number of Bits and convert it into Number of Bytes */
unsigned char bitsToBytes(unsigned char bits)
{
	unsigned char size = bits/8;

        if(bits%8!=0)
                size++;
                
        return size;
}

/* calBits returns the count of bits required to store the unsigned char */
unsigned char calBits(unsigned char input){
	unsigned char countOfBits = 0;
	while(input!=0)
	{
		input = input >> 1;
		countOfBits++;
	}
	return countOfBits;
}

/*
 * calCountOfBitsSeq takes the bits required to store the unsigned long long int and returns the Bits required to create the Encoded Sequence
 * Also it stores the seq in Singly Linked List
 */
unsigned char calCountOfBitsSeq(unsigned char countOfBits){
	unsigned char val = countOfBits;
	seqOfBits = insert(seqOfBits, val);
	countOfBits++;
	while(val>2){
		val = calBits(val);
		seqOfBits = insert(seqOfBits, val);
		countOfBits = countOfBits + val;
		countOfBits++;
		if(val==2)
			break;
	}
	countOfBits = countOfBits + 2;
	return countOfBits;
}


/* printBinary prints the unsigned char in Binary Format and not used in the whole Program 
 * It was written just to develop the program and progress of the same
 */
void printBinary(unsigned char data){
	int noOfBits = sizeof(data)*8 - 1;
	while(noOfBits>=0){
		printf("%d",(data >> noOfBits) & 1);
		noOfBits--;
	}
}

/* calCountOfBits returns the count of bits required to store the unsigned char */
unsigned char calCountOfBits(unsigned long long int input){
	unsigned char countOfBits = 0;
	while(input!=0)
	{
		input = input >> 1;
		countOfBits++;
	}
	if(countOfBits<2)
		countOfBits = 2;
	return countOfBits;
}

/* writeSeqInFiletoView  stores the Encoded Sequence in the File line by line After calculating number of Bytes Required in Readable Format */
void writeSeqInFiletoView(FILE * file, unsigned char *out, unsigned char countOfBits)
{
        unsigned char k=0, noOfBits=7;
	
	fprintf(file,"Encoded Sequence:\t");
        // Here Storing Bit in Byte
        if(countOfBits!=0){
                while(countOfBits>0){
                        fprintf(file,"%d",(*(out + k) >> noOfBits) & 1);
			if(noOfBits==0){
				noOfBits = 8;
				k++;
			}
                        countOfBits--;
                        noOfBits--;
                }
        }
        fprintf(file,"\n\n");

}

/* writeSeqInFile  stores the Encoded Sequence in the File line by line After calculating number of Bytes Required */
void writeSeqInFile(FILE *file, unsigned char *out, unsigned char countOfBits)
{
	unsigned char size = bitsToBytes(countOfBits);

	for(int i=0; i<size; i++)
		fputc(*(out+i),file);
		
	fputc('\n',file);
}

/* writeDataInFile  stores the Data in the File line by line */
void writeDataInFile(FILE *file, unsigned long long int data)
{
	fprintf(file,"%llu",data);
	fputc('\n',file);
}

/* printEncodedSeq prints the Encoded Sequence in Binary Format 
 * It was written just to develop the program and progress of the same
 */
void printEncodedSeq(unsigned char *out, unsigned char countOfBits)
{
	unsigned char k, noOfBits=7, size;
	size = countOfBits / 8;
	countOfBits = countOfBits % 8;
	
	// Here Storing Byte by Byte
	for(k=0;k<size;k++)
                printBinary(*(out+k));	
                
        // Here Storing Bit by Bit
       	if(countOfBits!=0){
		while(countOfBits>0){
                	printf("%d",(*(out + k) >> noOfBits) & 1);
                	countOfBits--;
                	noOfBits--;
        	}
	}
        printf("\n");

}

/* 
 * getSize is the function which returns the size of the file whose filename is passed
 */
union decStore getSize(const char *filename)
{
	union decStore size;
	/* Opening the FILE */
	FILE *file = fopen(filename,"r");
	fseek(file,0,SEEK_END);
	size.num  = ftello(file);
	fclose(file);
	return size;
}

/* 
 * isBinaryFile takes filename as input and tells whether the input File is Binary File or NOT
 */

int isBinaryFile(const char *filename)
{
	/* Opening the FILE */
	FILE *file = fopen(filename,"r");
	
	if(!file)
	{
		printf("Error: Entered Virtual Disk Not Found!!!\n");
		return 0;
	}
	
	union decStore diskSize = getSize(filename);

	int ch;
	/* Check Byte by Byte */
	for(int i=0;i<diskSize.num;i++)
	{
		ch = fgetc(file);
		if(!isprint(ch) && !isspace(ch))
		{
			fclose(file);
			return 1;
		}
	}

	fclose(file);
	printf("Error: Invalid Virtual Disk Passed!!!\n");
	return 0;
}



