#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which Delete the File present in given virtual Disk
 */
 
/* This Function Updates the Seq Part of the File from the Virtual Disk, if After Renaming, size of new encoded Seq of Size is greater than Size of previous Encoded Sequence */
void replaceSeqToRight(FILE *leftSeqPointer,FILE *rightSeqPointer,unsigned char bytes)
{
	for(unsigned long long int i=0; i < bytes; i++)
	{
		fputc(fgetc(leftSeqPointer),rightSeqPointer);
		fseek(leftSeqPointer,-2,SEEK_CUR);
		fseek(rightSeqPointer,-2,SEEK_CUR);
	}
	fseek(rightSeqPointer,1,SEEK_CUR);

}

/* This Function Updates the Seq Part of the File from the Virtual Disk, if After Renaming, size of new encoded Seq of Size is lesser than Size of previous Encoded Sequence */
void replaceSeq(FILE *leftSeqPointer,FILE *rightSeqPointer,unsigned char bytes)
{
	char ch;
	for(unsigned int i=0; i<bytes; i++)
		fputc(fgetc(rightSeqPointer),leftSeqPointer);
}


/* This Function Updates the File Name in the Virtual Disk */
int renameFile(FILE *rightFilePointer, const char newName[])
{
	int i = 0;
	while(newName[i] !='\0')
		fputc(newName[i++],rightFilePointer);
	return i;
}

/* 
 * This Function Shifts the Seq Part of the File from the index of that file which needs to be renamed, 
 * Only if After Renaming, size of new encoded Seq of File Size is greater than or lesser than Size of previous Encoded Sequence of File Size
 * and updates the new encoded seq on the Virtual Disk of the File which needs to be renamed (Also if size does not change)
 */
void shiftEncodedSeq(FILE *rightSeqPointer, FILE *leftSeqPointer, int offset, unsigned int countOfFiles, int index)
{
	unsigned char bytes;
	union decStore curFileSize;
	unsigned char *out, countOfSeqBits, sizeOfOut;
	
	curFileSize.num = getSizeOfFileAtIndex(rightSeqPointer,index);
	
	fseek(rightSeqPointer,4,SEEK_SET);
	fseek(leftSeqPointer,4,SEEK_SET);
	
	
	int oldSize = calCountOfBytesSeq(curFileSize.num);
	
	curFileSize.num = curFileSize.num + offset;
	
	out = (unsigned char *)calloc(11,sizeof(unsigned char));

	countOfSeqBits = getEncodedSeqOfFileSize(out,curFileSize);
	/* getEncodedSeqOfFileSize returns the count Of bits of Encoded Seq Of file Size passed in it and prepares the encoded Sequence */

	sizeOfOut = bitsToBytes(countOfSeqBits);
	/* bitsToBytes is the Function which takes Number of Bits and convert it into Number of Bytes */
	
	
	// if size of new encoded Seq of Size is lesser than Size of previous Encoded Sequence 
	if(sizeOfOut < oldSize)
	{
		for(int i=0;i<=index;i++)
			fseek(rightSeqPointer,calCountOfBytesSeq(decode(leftSeqPointer)),SEEK_CUR);
		
		fseek(leftSeqPointer,-(oldSize-sizeOfOut),SEEK_CUR);
		
		for(unsigned int i=index+1; i < countOfFiles; i++){
			curFileSize.num = decode(rightSeqPointer);
			bytes = calCountOfBytesSeq(curFileSize.num);
			fseek(rightSeqPointer,-bytes,SEEK_CUR);
			replaceSeq(leftSeqPointer,rightSeqPointer,bytes);
		}
	}
	// if size of new encoded Seq of Size is greater than Size of previous Encoded Sequence 
	else if(sizeOfOut > oldSize)
	{
		for(int i=0;i<countOfFiles;i++)
			fseek(rightSeqPointer,calCountOfBytesSeq(decode(leftSeqPointer)),SEEK_CUR);
		
		fseek(rightSeqPointer,sizeOfOut-oldSize,SEEK_CUR);
		
		
		for(unsigned int i=countOfFiles-1; i > index; i--){
			curFileSize.num = getSizeOfFileAtIndex(leftSeqPointer,i);
			fseek(leftSeqPointer,-1,SEEK_CUR);
			fseek(rightSeqPointer,-1,SEEK_CUR);
			bytes = calCountOfBytesSeq(curFileSize.num);
			replaceSeqToRight(leftSeqPointer,rightSeqPointer,bytes);
		}
		
	}
	
	// Writes the new encoded seq on the Virtual Disk of the File which needs to be renamed (Also if size does not change)
	
	fseek(leftSeqPointer,4,SEEK_SET);
	for(int i=0;i<index;i++)
		curFileSize.num = decode(leftSeqPointer);
	writeEncodedSeqOnDisk(leftSeqPointer, out, sizeOfOut);
}


/* This Function Removes the Data Part of the File from the Virtual Disk*/
void compaction(FILE *leftFilePointer, FILE *rightFilePointer, unsigned int curFileSize)
{
	for(unsigned long long int i=0; i < curFileSize; i++)
	{
		fputc(fgetc(leftFilePointer),rightFilePointer);
		fseek(leftFilePointer,-2,SEEK_CUR);
		fseek(rightFilePointer,-2,SEEK_CUR);
	}
	fseek(leftFilePointer,1,SEEK_CUR);
	fseek(rightFilePointer,1,SEEK_CUR);
}

void compactionToLeft(FILE *leftFilePointer, FILE *rightFilePointer, unsigned int curFileSize)
{
	for(unsigned long long int i=0; i < curFileSize; i++)
		fputc(fgetc(rightFilePointer),leftFilePointer);
}


/* 
 * This Function Shifts the Data of the File to the Right, 
 * Only if After Renaming, length of File Name is lesser than length of previous File Name 
 * and Renames the File on the Virtual Disk of the File which needs to be renamed
 */
void shiftDataToRight(FILE *leftFilePointer,FILE *rightFilePointer, const char *newFileName, int index, int offset, FILE *encodedSeqPointer, unsigned int countOfFiles)
{
	fseek(rightFilePointer,0,SEEK_END);
	fseek(encodedSeqPointer,4,SEEK_SET);
	unsigned long long int curFileSize;
	
	for(int i=0; i<=index; i++){
		curFileSize = decode(encodedSeqPointer);
		fseek(rightFilePointer,-curFileSize,SEEK_CUR);
	}
	
	fseek(rightFilePointer,offset,SEEK_CUR);
	
	fseek(rightFilePointer,-(renameFile(rightFilePointer,newFileName)),SEEK_CUR);
	
	for(int i=index+1; i < countOfFiles; i++){
		fseek(leftFilePointer,-1,SEEK_CUR);
		fseek(rightFilePointer,-1,SEEK_CUR);
		curFileSize = decode(encodedSeqPointer);
		compaction(leftFilePointer,rightFilePointer,curFileSize);
	}
}

/* 
 * This Function Shifts the Data of the File to the Left, 
 * Only if After Renaming, length of File Name is greater than length of previous File Name 
 * and Renames the File on the Virtual Disk of the File which needs to be renamed
 */
void shiftDataToLeft(FILE *leftFilePointer,FILE *rightFilePointer, const char *newFileName, int index, int offset, FILE *encodedSeqPointer, unsigned int countOfFiles)
{
	fseek(rightFilePointer,0,SEEK_END);
	fseek(leftFilePointer,0,SEEK_END);
	fseek(encodedSeqPointer,4,SEEK_SET);
	unsigned long long int curFileSize;
	
	for(int i=0; i< countOfFiles; i++){
		curFileSize = decode(encodedSeqPointer);
		fseek(rightFilePointer,-curFileSize,SEEK_CUR);
		fseek(leftFilePointer,-curFileSize,SEEK_CUR);
	}
	
	fseek(leftFilePointer,-offset,SEEK_CUR);
	
	for(int i=countOfFiles-1; i > index; i--){
		curFileSize = getSizeOfFileAtIndex(encodedSeqPointer,i);
		compactionToLeft(leftFilePointer,rightFilePointer,curFileSize);
	}
	renameFile(leftFilePointer,newFileName);
}


/* It is the Main Function for vdDelete Operation which deals with all kinds of Validations for Input */
int main(int argc, char *argv[])
{
	/* Validating Number of Arguments */
	if(argc!=4)
	{
		printf("Error : Invalid Number of Arguments Passed!!!\n");
		return 0;
	}

	/* Validating whether the Disk passed is Binary File or Not */
	if(isBinaryFile(argv[1])==0)
		return 0;

	unsigned int countOfFiles;

	unsigned long int offset = 0;
	
	// Getting Length of new and old file names
	int oldFileNameSize = strlen(argv[2]), newFileNameSize = strlen(argv[3]);
	
	char extension1[oldFileNameSize], extension2[newFileNameSize];
	
	// Getting Extensions for both File Names
	getFileExtension(argv[2], extension1);
	
	getFileExtension(argv[3], extension2);
	
	/* Three File Pointers , one for countOfFiles, Second for encoded seq of sizes of files and third for file's names*/
	
	
	FILE *countPointer = fopen(argv[1],"r+");

	FILE *encodedSeqPointer = fopen(argv[1],"r+");

	FILE *leftFilePointer = fopen(argv[1],"r+");

	FILE *rightFilePointer = fopen(argv[1],"r+");
	
	FILE *leftSeqPointer = fopen(argv[1],"r+");
			
	FILE *rightSeqPointer = fopen(argv[1],"r+");
		
		
	countOfFiles = getNoOfFilesInDisk(countPointer);
		// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
		
		
	if(countOfFiles == 0)
	{
		printf("Disk is Empty !!!\n");
		return 0;
	}
	

		
	// searchInFiles returns the Index of the File in the Virtual Disk if exists, else it will return -1
	// Checking Whether new File Name already exists or not
	int index = searchInFiles(leftFilePointer, encodedSeqPointer, countOfFiles, argv[3]);
	if(index!=-1)
		printf("Error : %s File Name Already Exists in %s !!!\n",argv[3],argv[1]);
	else{
		// Checking Whether Old File Name already exists or not
		index = searchInFiles(leftFilePointer, encodedSeqPointer, countOfFiles, argv[2]);
		if(index==-1)
		printf("Error : %s File Does Not Exists in %s !!!\n",argv[2],argv[1]);
		else{
			// Checking Whether Extensions of both File is same or not
			if (strcmp(extension1, extension2) != 0)
			{
				printf("Error : Extensions of Old File Name and New File Name are Not Same\n");
				return 0;
			}
			
			// Checking Whether both File Names are same or not
			if (strcmp(argv[2], argv[3]) == 0)
			{
				printf("Error : Old File Name and New File Name are Same\n");
				return 0;
			}
			
			// If size of old and new file name is same
			if(oldFileNameSize==newFileNameSize)
			{
				renameFile(leftFilePointer, argv[3]);
			}
			// If size of old and new file name are different
			else if( oldFileNameSize < newFileNameSize )
			{
				shiftDataToLeft(leftFilePointer,rightFilePointer, argv[3], index, newFileNameSize - oldFileNameSize, encodedSeqPointer, countOfFiles);
				shiftEncodedSeq(leftSeqPointer, rightSeqPointer, newFileNameSize - oldFileNameSize, countOfFiles, index);
			}
			else
			{
				shiftDataToRight(leftFilePointer,rightFilePointer, argv[3], index, oldFileNameSize - newFileNameSize, encodedSeqPointer, countOfFiles);
				shiftEncodedSeq(leftSeqPointer, rightSeqPointer,-(oldFileNameSize - newFileNameSize), countOfFiles, index);
			}
			
			
			/* CLosing All File Pointers */
			fclose(countPointer);
				
			fclose(encodedSeqPointer);
		
			fclose(leftFilePointer);
	
			fclose(rightFilePointer);
			
			fclose(rightSeqPointer);
					
			fclose(leftSeqPointer);
			
			printf("Success : %s File Renameded to %s Successfully !!!\n",argv[2],argv[3]);
		}
	}


	
	/* 
	 * deleteFileData Function writes the content for the retrieval of the File		
	 */
	 
	 
	
	return 0;
}
