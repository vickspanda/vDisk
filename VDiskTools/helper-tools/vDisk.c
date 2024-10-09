#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include"decode.c"


int getSizeOfFileName(const char *fileName)
{
	
	int count = 0;
	while(fileName[count] != '\0')
		count++;
	count++;
	return count;
}

/* 
 * calCountOfBytesSeq is the Function which takes any data of unsigned long long int and returns the Bytes required to store the Encoded Sequence
 * of the data passed
 */
unsigned char calCountOfBytesSeq(unsigned long long int length){
	unsigned char countOfBits = calCountOfBits(length);
	unsigned char val = countOfBits;
	countOfBits++;
	while(val>2){
		val = calBits(val);
		countOfBits = countOfBits + val;
		countOfBits++;
		if(val==2)
			break;
	}
	countOfBits = countOfBits + 2;
	val = bitsToBytes(countOfBits);
	return val;
}


/*
 * getLengthOFFileName returns the length of the file Name present in Virtual Disk
 */
unsigned int getLengthOfFileName(FILE *fileNamePointer){
	char ch;
	int countChars = 0;
	while((ch=fgetc(fileNamePointer))!='\0')
		countChars++;
	fseek(fileNamePointer,-(countChars+1),SEEK_CUR);
	return countChars+1;
}

/*
 * getUsedSpace Function returns the usedSpace in Virtual Disk whose file Pointer was provided as an input to it
 */
unsigned long long int getUsedSpace(FILE *encodedSeqPointer, unsigned int countOfFiles)
{
	fseek(encodedSeqPointer,4,SEEK_SET);
	unsigned long long int usedSpace = 0, curFileSize;
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(encodedSeqPointer);
		usedSpace = usedSpace + calCountOfBytesSeq(curFileSize) + curFileSize;
	}
	usedSpace = usedSpace + 4;
	fseek(encodedSeqPointer,0,SEEK_SET);	
	return usedSpace;
}

/*
 * getFreeSpace Function returns the Free Space available in Virtual Disk whose file Pointer was provided as an input to it
 */
unsigned long long int getFreeSpace(unsigned long long int usedSpace, const char *vDiskName)
{
	union decStore diskSize = getSize(vDiskName);
	return diskSize.num - usedSpace;
}

/*
 * getReqSpace Function returns the Actual Space Required by the New File to get added in Virtual Disk 
 * whose file Pointer was provided as an input to it
 * Actual Space means : File Size, File Name Size, and Encoded File Size
 */
unsigned long long int getReqSpace(unsigned long long int fileSize)
{
	unsigned long long int reqSpace = calCountOfBytesSeq(fileSize);
	return reqSpace + fileSize;
}

/*
 * canBeAdded Function returns true if Virtual Disk whose file Pointer was provided as an input to it
 * have the Space to Add new File in it
 * else returns false
 */
bool canBeAdded(const char *vDiskName, unsigned long long int fileSize, FILE *encodedSeqPointer, unsigned int countOfFiles)
{
	if(getFreeSpace(getUsedSpace(encodedSeqPointer, countOfFiles),vDiskName) > getReqSpace(fileSize))
		return true;
	else
		return false;
}



/*
 * getEncodedSeqOfFileSize returns the count Of bits of Encoded Seq Of file Size passed in it and prepares the encoded Sequence
 */
unsigned char getEncodedSeqOfFileSize(unsigned char *out, union decStore fileSize)
{
        unsigned char lengthInBits, size, *in, countOfSeqBits;

        lengthInBits = calCountOfBits(fileSize.num);

        size = bitsToBytes(lengthInBits);

        in = (unsigned char *)malloc(size);

        for(int i = 0; i < size; i++)
                *(in + i) = fileSize.numInChar[size - i - 1];

        countOfSeqBits = encode(in,lengthInBits,out);

        return countOfSeqBits;
}

/*
 * getNoOfFilesInDisk function returns the count of files present in the Virtual Disk
 */
int getNoOfFilesInDisk(FILE *countPointer)
{
	int count;
	fread(&count, sizeof(int), 1, countPointer);
	fseek(countPointer,0,SEEK_SET);
	return count;
}

/*
 * updateNoOfFiles function updates the count and set it as 2nd Argument passed in it
 */
void updateNoOfFilesInDisk(FILE *countPointer, int count)
{
	fwrite(&count, sizeof(int), 1, countPointer);
	fseek(countPointer,0,SEEK_SET);
}


/*
 * setEncodedSeqPointerToWrite is the Function, which repositions the file Pointer to correct position for writing the Encoded Sequence
 */
bool setEncodedSeqPointerToWrite(FILE *encodedSeqPointer,unsigned int countOfFiles)
{
	unsigned long long int curFileSize;
	if(fseek(encodedSeqPointer,4,SEEK_SET)!=0){
		return false;
	}
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(encodedSeqPointer);
	}
	return true;
}


/*
 * writeEncodedSeqOnDisk is the Function which writes the encoded Seq present in *out in the virtual Disk
 */
bool writeEncodedSeqOnDisk(FILE *encodedSeqPointer, unsigned char *out, unsigned char sizeOfOut)
{	
	for(int i=0; i<sizeOfOut; i++)
		if(fputc(*(out+i),encodedSeqPointer) == EOF)
			return false;
	return true;
}

/*
 * storeEncodedSeq is the Function which stores the encoded Seq present in *out in the virtual Disk by 
 * seting the file pointer and writing the content
 */
bool storeEncodedSeq(FILE *encodedSeqPointer, unsigned char *out, unsigned char sizeOfOut, unsigned int countOfFiles)
{
	if(!setEncodedSeqPointerToWrite( encodedSeqPointer, countOfFiles))
		printf("Error in Setting the Encoded Sequence pointer !!!\n");

	if(!writeEncodedSeqOnDisk(encodedSeqPointer, out, sizeOfOut))
		printf("Error in Writing the Encoded Sequence !!!\n");
		
	fseek(encodedSeqPointer,0,SEEK_SET);
}

/*
 * setFileDataPointerToWrite is the Function, which repositions the file Pointer to correct position for writing the Data of the Input File
 */
bool setFileDataPointerToWrite(FILE *fileDataPointer,FILE *encodedSeqPointer,unsigned int countOfFiles, union decStore fileSize)
{
	unsigned long long int curFileSize;
	if(fseek(encodedSeqPointer,4,SEEK_SET)!=0){
		return false;
	}
	fseek(fileDataPointer,0,SEEK_END);
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(encodedSeqPointer);
		fseek(fileDataPointer,-curFileSize,SEEK_CUR);
	}
	fseek(fileDataPointer,-fileSize.num,SEEK_CUR);
}


/*
 * writeFileDataOnDisk is the Function which writes the Data present in *inputFile in the virtual Disk
 */
bool writeFileDataOnDisk(FILE *fileDataPointer,FILE *inputFile, const char * inputFileName, unsigned long long fileSize)
{
	char ch;
	int i = 0;
	
	while(inputFileName[i] != '\0')
		fputc(inputFileName[i++],fileDataPointer);
	fputc('\0',fileDataPointer);
	
	for(i=i+1; i<fileSize; i++)
		fputc(fgetc(inputFile),fileDataPointer);
}


/*
 * storeFileData is the Function which stores the data of *inputFile in the virtual Disk by 
 * seting the file pointer and writing the content along with the fileName
 */
bool storeFileData(FILE *fileDataPointer,FILE *encodedSeqPointer,unsigned int countOfFiles, union decStore fileSize, FILE *inputFile, const char *inputFileName)
{
	setFileDataPointerToWrite( fileDataPointer, encodedSeqPointer, countOfFiles, fileSize);
	
	writeFileDataOnDisk(fileDataPointer, inputFile, inputFileName, fileSize.num);
	
	fseek(encodedSeqPointer,0,SEEK_SET);
}


/*
 * searchInFiles returns the Index of the File in the Virtual Disk if exists,
 * if not Found, then it returns -1
 */
int searchInFiles(FILE *fileDataPointer, FILE *encodedSeqPointer, unsigned int countOfFiles, const char *inputFileName)
{	
	fseek(fileDataPointer,0,SEEK_END);
	fseek(encodedSeqPointer,4,SEEK_SET);
	
	unsigned long long int curFileSize = 12;
	
	unsigned long int fileNameSize;
	
	for(int i = 0; i < countOfFiles; i++){
		
		curFileSize = decode(encodedSeqPointer);
		
		fseek(fileDataPointer,-(curFileSize),SEEK_CUR);
		
		fileNameSize = getLengthOfFileName(fileDataPointer);
		
		char fileName[fileNameSize];
		
		for(int j = 0; j < fileNameSize; j++)
			fileName[j] = fgetc(fileDataPointer);

		fseek(fileDataPointer,-(fileNameSize),SEEK_CUR);
		
		if(strcmp(fileName,inputFileName)== 0){
			fseek(encodedSeqPointer,0,SEEK_SET);
			return i;
		}
	}
	fseek(encodedSeqPointer,0,SEEK_SET);
	return -1;
}

/*
 * getSizeOfFileAtIndex returns the file size at particular Index in the Virtual Disk
 */
unsigned long long int getSizeOfFileAtIndex(FILE *encodedSeqPointer,int index)
{
	unsigned long long int curFileSize;
	fseek(encodedSeqPointer,4,SEEK_SET);
	for(unsigned int i=0; i<index+1; i++)
		curFileSize = decode(encodedSeqPointer);
	return curFileSize;
} 


/* This Function returns the extension of the file*/
void getFileExtension(const char *filename, char *extension) {
	// Find the last occurrence of the '.' character
	const char *dot = strrchr(filename, '.');
	if (dot != NULL) {
		strcpy(extension, dot + 1); 
	} else {
		strcpy(extension, "");
	}
}
