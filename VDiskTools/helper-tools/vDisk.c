#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include"decode.c"


void skipName(FILE *metaDataPointer)
{
	while(fgetc(metaDataPointer)!='\0');
}


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
	return countChars+1;
}

/*
 * getUsedSpace Function returns the usedSpace in Virtual Disk whose file Pointer was provided as an input to it
 */
unsigned long long int getUsedSpace(FILE *metaDataPointer, unsigned int countOfFiles)
{
	fseek(metaDataPointer,4,SEEK_SET);
	unsigned long long int usedSpace = 0, curFileSize, fileNameLength;
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(metaDataPointer);
		if(fgetc(metaDataPointer)!=0)
			fileNameLength = getLengthOfFileName(metaDataPointer);
		else
			fileNameLength = 0;
		usedSpace = usedSpace + calCountOfBytesSeq(curFileSize) + curFileSize + fileNameLength + 1;
	}
	usedSpace = usedSpace + 4;
	fseek(metaDataPointer,0,SEEK_SET);	
	return usedSpace;
}


/*
 * getHolesSpace Function returns the Holes Space in Virtual Disk whose file Pointer was provided as an input to it
 */
unsigned long long int getHolesSpace(FILE *metaDataPointer, unsigned int countOfFiles)
{
	fseek(metaDataPointer,4,SEEK_SET);
	unsigned long long int holesSpace = 0, curFileSize;
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(metaDataPointer);
		if(fgetc(metaDataPointer)==0)
			holesSpace = holesSpace + curFileSize;
		else
			skipName(metaDataPointer);
	}
	fseek(metaDataPointer,0,SEEK_SET);	
	return holesSpace;
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
unsigned long long int getReqSpace(unsigned long long int fileSize, const char *inputFileName)
{
	unsigned long long int reqSpace = calCountOfBytesSeq(fileSize);
	return reqSpace + fileSize + strlen(inputFileName) + 2;
}

/*
 * canBeAdded Function returns true if Virtual Disk whose file Pointer was provided as an input to it
 * have the Space to Add new File in it
 * else returns false
 */
bool canBeAdded(const char *vDiskName, unsigned long long int fileSize, FILE *metaDataPointer, unsigned int countOfFiles, const char *inputFileName)
{
	return getFreeSpace(getUsedSpace(metaDataPointer, countOfFiles),vDiskName) >= getReqSpace(fileSize,inputFileName);
}



/*
* canBeAddedInHoles Function returns true if Virtual Disk whose file Pointer was provided as an input to it
* have the Space in Form of Holes to Add new File in it	
* else returns false
*/
bool canBeAddedInHoles(const char *vDiskName, unsigned long long int fileSize, FILE *metaDataPointer, unsigned int countOfFiles, const char *inputFileName)
{
	return getHolesSpace(metaDataPointer,countOfFiles)  >= (getReqSpace(fileSize,inputFileName) - calCountOfBytesSeq(fileSize));
}

/*
* canBeAddedInHolesWithFreeSpace Function returns true if Virtual Disk whose file Pointer was provided as an input to it
* have the Space in Form of Holes along with the Free Space to Add new File in it	
* else returns false
*/
bool canBeAddedInHolesWithFreeSpace(const char *vDiskName, unsigned long long int fileSize, FILE *metaDataPointer, unsigned int countOfFiles, const char *inputFileName)
{
	return (getHolesSpace(metaDataPointer,countOfFiles) + getFreeSpace(getUsedSpace(metaDataPointer, countOfFiles),vDiskName)) >= getReqSpace(fileSize,inputFileName);
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
 * getNoOfFilesInDisk function returns the count of files present in the Virtual Disk
 */
int getNoOfHolesInDisk(FILE *metaDataPointer, int countOfFiles)
{
	int count=0;
	
	fseek(metaDataPointer,4,SEEK_SET);
	for(int i=0;i<countOfFiles;i++)
	{
		decode(metaDataPointer);
		if(fgetc(metaDataPointer)==1)
			skipName(metaDataPointer);
		else
			count++;
	}
	fseek(metaDataPointer,4,SEEK_SET);
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
bool setMetaDataPointerToWrite(FILE *metaDataPointer,unsigned int countOfFiles)
{
	unsigned long long int curFileSize;
	if(fseek(metaDataPointer,4,SEEK_SET)!=0){
		return false;
	}
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(metaDataPointer);
		if(fgetc(metaDataPointer)==1)
			skipName(metaDataPointer);
	}
	return true;
}


/*
 * writeMetaDataOnDisk is the Function which writes the encoded Seq present in *out followed by respective File Name in the virtual Disk
 */
bool writeMetaDataOnDisk(FILE *metaDataPointer, unsigned char *out, unsigned char sizeOfOut, const char *inputFileName)
{	
	for(int i=0; i<sizeOfOut; i++)
		if(fputc(*(out+i),metaDataPointer) == EOF)
			return false;

	fputc(1,metaDataPointer);
	int i=0;
	while(inputFileName[i]!='\0')
		fputc(inputFileName[i++],metaDataPointer);
	fputc('\0',metaDataPointer);
	return true;
}

/*
 * storeMetaDataSeq is the Function which stores the encoded Seq and respective File Name present in *out in the virtual Disk by 
 * seting the file pointer and writing the content
 */
bool storeMetaData(FILE *metaDataPointer, unsigned char *out, unsigned char sizeOfOut, unsigned int countOfFiles, const char *inputFileName)
{
	if(!setMetaDataPointerToWrite(metaDataPointer, countOfFiles))
		printf("Error in Setting the Meta Data pointer !!!\n");

	if(!writeMetaDataOnDisk(metaDataPointer, out, sizeOfOut, inputFileName))
		printf("Error in Writing the Meta Data Sequence !!!\n");
		
	fseek(metaDataPointer,0,SEEK_SET);
}

/*
 * setFileDataPointerToWrite is the Function, which repositions the file Pointer to correct position for writing the Data of the Input File
 */
bool setFileDataPointerToWrite(FILE *fileDataPointer,FILE *metaDataPointer,unsigned int countOfFiles, union decStore fileSize)
{
	unsigned long long int curFileSize;
	if(fseek(metaDataPointer,4,SEEK_SET)!=0){
		return false;
	}
	fseek(fileDataPointer,0,SEEK_END);
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(metaDataPointer);
		if(fgetc(metaDataPointer)==1)
			skipName(metaDataPointer);
		fseek(fileDataPointer,-curFileSize,SEEK_CUR);
	}
	fseek(fileDataPointer,-fileSize.num,SEEK_CUR);
}


/*
 * writeFileDataOnDisk is the Function which writes the Data present in *inputFile in the virtual Disk
 */
bool writeFileDataOnDisk(FILE *fileDataPointer,FILE *inputFile, unsigned long long fileSize)
{
	char ch;
	for(int i=0; i<fileSize; i++)
		fputc(fgetc(inputFile),fileDataPointer);
}


/*
 * storeFileData is the Function which stores the data of *inputFile in the virtual Disk by 
 * seting the file pointer and writing the content along with the fileName
 */
bool storeFileData(FILE *fileDataPointer,FILE *metaDataPointer,unsigned int countOfFiles, union decStore fileSize, FILE *inputFile)
{
	setFileDataPointerToWrite( fileDataPointer, metaDataPointer, countOfFiles, fileSize);
	
	writeFileDataOnDisk(fileDataPointer, inputFile, fileSize.num);
	
	fseek(metaDataPointer,0,SEEK_SET);
}


/*
 * searchInFiles returns the Index of the File in the Virtual Disk if exists,
 * if not Found, then it returns -1
 */
int searchInFiles(FILE *metaDataPointer, unsigned int countOfFiles, const char *inputFileName)
{	
	fseek(metaDataPointer,4,SEEK_SET);
	
	unsigned long long int curFileSize = 12;
	
	unsigned long int fileNameSize;
	
	for(int i = 0; i < countOfFiles; i++){
		
		curFileSize = decode(metaDataPointer);
				
		if(fgetc(metaDataPointer)==1)
		{
			fileNameSize = getLengthOfFileName(metaDataPointer);
			fseek(metaDataPointer,-fileNameSize,SEEK_CUR);
			
			char fileName[fileNameSize];
		
			for(int j = 0; j < fileNameSize; j++)
				fileName[j] = fgetc(metaDataPointer);
		
			if(strcmp(fileName,inputFileName)== 0){
				fseek(metaDataPointer,-fileNameSize,SEEK_CUR);
				return i;
			}
		}
	}
	return -1;
}

/*
 * getSizeOfFileAtIndex returns the file size at particular Index in the Virtual Disk
 */
unsigned long long int getSizeOfFileAtIndex(FILE *metaDataPointer,int index)
{
	unsigned long long int curFileSize;
	fseek(metaDataPointer,4,SEEK_SET);
	for(unsigned int i=0; i<index+1; i++){
		curFileSize = decode(metaDataPointer);
		if(fgetc(metaDataPointer)==1)
			skipName(metaDataPointer);
	}
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


// Merge Consecutive Holes
// First Portion is Written to execute the Merging of two consecutive Holes 
// So that Bytes requiured to store the consecutive Holes will be reduced


/* This Function Updates the meta Data Part of the File from the Virtual Disk, if After Renaming, size of new encoded Seq of Size is lesser than Size of previous Encoded Sequence */
void replaceSeq(FILE *leftSeqPointer,FILE *rightSeqPointer,unsigned char bytes)
{
	char ch;
	for(unsigned int i=0; i<bytes; i++)
		fputc(fgetc(rightSeqPointer),leftSeqPointer);
}


/*
 * writeMetaDataOnDisk is the Function which writes the encoded Seq present in *out followed by respective File Name in the virtual Disk
 */
bool writeMetaData(FILE *metaDataPointer, unsigned char *out, unsigned char sizeOfOut)
{	
	for(int i=0; i<sizeOfOut; i++)
		if(fputc(*(out+i),metaDataPointer) == EOF)
			return false;

	fputc(0,metaDataPointer);
	return true;
}


/*After Loading each Data About Files in the Arrays, Actual Merging Happens in this Function */
void mergeHoles(FILE * leftMetaPointer, FILE * rightMetaPointer, unsigned long long int fileSizeArray[], int fileNameSizeArray[], int metaArray[], unsigned int countOfFiles)
{
    int flag = 0, leftIndex, rightIndex;
    union decStore newSumSize;
    newSumSize.num = 0;
    unsigned char *out, countOfSeqBits, sizeOfOut;

    for(int i=0;i<countOfFiles;i++)
    {
        if(fileNameSizeArray[i]==1 && flag==0)
        {
            flag = 1;
            leftIndex = i;
        }

        if(fileNameSizeArray[i]==1 && flag == 1)
            newSumSize.num = newSumSize.num + fileSizeArray[i];

        if(fileNameSizeArray[i]!=1 && flag==1)
        {
            rightIndex = i-1;

            if(leftIndex < rightIndex)
            {   
                fseek(leftMetaPointer,4,SEEK_SET);
                fseek(rightMetaPointer,4,SEEK_SET);

                out = (unsigned char *)calloc(11,sizeof(unsigned char));

                countOfSeqBits = getEncodedSeqOfFileSize(out,newSumSize);
                /* getEncodedSeqOfFileSize returns the count Of bits of Encoded Seq Of file Size passed in it and prepares the encoded Sequence */

                sizeOfOut = bitsToBytes(countOfSeqBits);
                /* bitsToBytes is the Function which takes Number of Bits and convert it into Number of Bytes */

                for(int i=0;i<leftIndex;i++)
                    fseek(leftMetaPointer,metaArray[i]+fileNameSizeArray[i],SEEK_CUR);

                for(int i=0;i<=rightIndex;i++)
                    fseek(rightMetaPointer,metaArray[i]+fileNameSizeArray[i],SEEK_CUR);

                writeMetaData(leftMetaPointer,out,sizeOfOut);

                for(unsigned int i=rightIndex+1; i < countOfFiles; i++)
			        replaceSeq(leftMetaPointer,rightMetaPointer,metaArray[i]+fileNameSizeArray[i]);

				fseek(leftMetaPointer,0,SEEK_SET);
                updateNoOfFilesInDisk(leftMetaPointer,getNoOfFilesInDisk(leftMetaPointer)-(rightIndex-leftIndex));

				free(out);

				return;
            }
			else
			{
				flag = 0; 
				newSumSize.num = 0;
			}
        }
    }
}


/* Here the Whole Data About the Files in Disk is loaded to the Arrays */
void createMetaTable(FILE * metaDataPointer, unsigned long long int fileSizeArray[], int fileNameSizeArray[], int metaArray[], unsigned int countOfFiles)
{
    fseek(metaDataPointer,4,SEEK_SET);
    for(int i=0;i<countOfFiles;i++)
    {
        fileSizeArray[i] = decode(metaDataPointer);
        metaArray[i] = calCountOfBytesSeq(fileSizeArray[i]);
        if(fgetc(metaDataPointer)!=0)
			fileNameSizeArray[i] = getLengthOfFileName(metaDataPointer) + 1;
		else
			fileNameSizeArray[i] = 1;
    }

    fseek(metaDataPointer,4,SEEK_SET);

    /*for(int i=0;i<countOfFiles;i++)
        printf("%llu\t%d\t%d\n",fileSizeArray[i],fileNameSizeArray[i],metaArray[i]);*/
}

/* It is the Function which Provides the Environment for merger of two consecutive Holes */
void mergeConsecutiveHoles(const char * vDisk)
{
    FILE * countPointer = fopen(vDisk,"r+");

    FILE * leftMetaPointer = fopen(vDisk,"r+");

    FILE * rightMetaPointer = fopen(vDisk,"r");

	unsigned int countOfFiles = getNoOfFilesInDisk(countPointer);

    unsigned long long int fileSizeArray[countOfFiles];

    int fileNameSizeArray[countOfFiles], metaArray[countOfFiles];

    createMetaTable(rightMetaPointer,fileSizeArray,fileNameSizeArray,metaArray,countOfFiles);

    mergeHoles(leftMetaPointer,rightMetaPointer,fileSizeArray,fileNameSizeArray, metaArray,countOfFiles);

    fclose(countPointer);

    fclose(leftMetaPointer);

    fclose(rightMetaPointer);
    
}