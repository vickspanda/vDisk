#include<stdio.h>
#include<string.h>
#include<stdbool.h>


/* It is the Function to print the Data for the Files like the Meta Data Size, File Name Size, Holes Array*/
void printArray(int array[], int n)
{
    for(int i = 0; i < n; i++)
        printf("%d\t",array[i]);
}

/* It is the Function to print the File Size of the Files present in the Virtual Disk */
void printSizeArray(unsigned long long int array[], int n)
{
    for(int i = 0; i < n; i++)
        printf("%llu\t",array[i]);
}


/* This is the Linked List Made for Storing the Indexes for the Possible merge of the Holes for the Required Space by File that is supposed to be Added*/
struct node{
    int leftIndex, rightIndex;
    struct node *next;
};


/* It is the create Node function for the Node structure operation defined above */
struct node * createNewnode(int leftIndex, int rightIndex)
{
    struct node * newnode = (struct node*)malloc(sizeof(struct node));
    newnode->next = NULL;
    newnode->leftIndex = leftIndex;
    newnode->rightIndex = rightIndex;
    return newnode;
}

/* It is the insert Node function for the Node structure operation defined above to Add new node to the existing Linked List */
struct node * insertNode(struct node* head, int leftIndex, int rightIndex)
{
    if(head==NULL)
        return createNewnode(leftIndex,rightIndex);
    else
    {
        struct node* curNode = head;
        while(curNode->next!=NULL)
            curNode = curNode->next;
        curNode->next = createNewnode(leftIndex,rightIndex);
        return head;
    }
}

/* It is the print function for the Node structure operation defined above to print the Linked List */
void printLL(struct node * head)
{
    while(head!=NULL)
    {
        printf("%d\t%d->",head->leftIndex,head->rightIndex);
        head = head->next;
    }
    printf("\n");
}

/* It is the Function which provides the Information About the Holes Present in the Virtual Disk */
void genHolesArray(int fileNameSizeArray[], int holesArray[], unsigned int countOfFiles)
{
    int j = 0;
    for(int i = 0; i < countOfFiles; i++)
        if(fileNameSizeArray[i]==1)
            holesArray[j++] = i;
}

/* It is the Function which returns the node with left and right Index, which will consume least cost if used to merge the holes */
struct node * getMinSwapCostNode(unsigned long long int fileSizeArray[], int fileNameSizeArray[], struct node * holesComb)
{
    struct node * minSwapCostNode = NULL;
    unsigned long long int swapCost, minSwapCost;
    int flag = 0;
    while(holesComb!=NULL)
    {
        swapCost = 0;
        for(int i = holesComb->leftIndex; i < holesComb->rightIndex; i++)
        {
            if(fileNameSizeArray[i] != 1)
                swapCost = swapCost + fileSizeArray[i];
        }
        
        if(flag == 0)
        {
            minSwapCost = swapCost;
            minSwapCostNode = holesComb;
            flag = 1;
        }
        else
        {
            if(minSwapCost > swapCost)
            {
                minSwapCost = swapCost;
                minSwapCostNode = holesComb;
            }
        }
        holesComb = holesComb->next;
        printf("%llu\n",swapCost);
    }

    return minSwapCostNode;
}

/* It is the Function which returns the all possible combinations in the form of Linked List whose summed up hole size is greater than the File Size supposed to be Added */
struct node* getCombinations(union decStore fileSize, unsigned int countOfHoles, unsigned long long int fileSizeArray[], int holesArray[])
{
    int leftIndex = 0, rightIndex = 0, flag = 0;
    unsigned long long int buffer = 0;
    struct node *holeCombs = NULL;

    while(leftIndex < countOfHoles)
    {
        if(rightIndex < countOfHoles)
            buffer = buffer + fileSizeArray[holesArray[rightIndex]];

        if(rightIndex == countOfHoles-1)
            flag = 1;

        if(buffer >= fileSize.num)
        {
            printf("%llu\n",buffer);
            holeCombs = insertNode(holeCombs,holesArray[leftIndex],holesArray[rightIndex]);
            buffer = 0;
            leftIndex++;
            rightIndex = leftIndex;
        }
        else if(rightIndex < countOfHoles-1)
            rightIndex++;
        else{
            leftIndex++;
            rightIndex = leftIndex;
            buffer = 0;
        }
    }

    return holeCombs;
}

/* It is the Environment Function which all over helps to return the Node having left and right Index in which holes are Present, whose sum is greater than the required by the File to be Added and Swapping cost is the Least */
struct node * getIndexesForDeFrag(const char *vDisk, union decStore fileSize,FILE *leftMetaPointer,unsigned long long int fileSizeArray[],int fileNameSizeArray[],int metaArray[],int holesArray[],unsigned int countOfFiles,unsigned int countOfHoles)
{

    createMetaTable(leftMetaPointer,fileSizeArray,fileNameSizeArray,metaArray,countOfFiles);

    genHolesArray(fileNameSizeArray, holesArray,countOfFiles);

    struct node* holeCombs = getCombinations(fileSize,countOfHoles,fileSizeArray,holesArray);

    printLL(holeCombs);

    holeCombs = getMinSwapCostNode(fileSizeArray,fileNameSizeArray,holeCombs);

    printf("Left Index : %d\t Right Index : %d\n\n\n",holeCombs->leftIndex,holeCombs->rightIndex);

    return holeCombs;

}

/* It is the Function which replaces the Data in the Right and helps in the Swapping of the File Data  */
void replaceData(FILE *leftFilePointer, FILE *rightFilePointer, unsigned long long int curFileSize)
{
    for(unsigned long long int i=0; i < curFileSize; i++)
		fputc(fgetc(rightFilePointer),leftFilePointer);
}

/* It is the Function which replaces the Meta Data in the Right  and helps in the Swapping of the Meta Data of the File */
void replaceMetaData(FILE *leftMetaPointer, FILE *rightMetaPointer, int bytes)
{
    for(unsigned long long int i=0; i < bytes; i++)
		fputc(fgetc(rightMetaPointer),leftMetaPointer);
}

/* It is the Function which Swaps the File Data so that all required holes must achieve consecutive positions */
void swapFileData(unsigned long long int fileSizeArray[],int fileNameSizeArray[],int metaArray[],FILE *leftFilePointer,FILE *rightFilePointer,unsigned int countOfFiles, struct node * minSwapIndex)
{
    fseek(leftFilePointer,0,SEEK_END);
    fseek(rightFilePointer,0,SEEK_END);

    for(int i = 0; i <= minSwapIndex->rightIndex; i++)
    {
        fseek(leftFilePointer,-fileSizeArray[i],SEEK_CUR);
        fseek(rightFilePointer,-fileSizeArray[i],SEEK_CUR);
    }

    for(int i = minSwapIndex->rightIndex; i >= minSwapIndex->leftIndex; i--)
    {
        if(fileNameSizeArray[i]==1)
            fseek(rightFilePointer,fileSizeArray[i],SEEK_CUR);
        else
            replaceData(leftFilePointer,rightFilePointer,fileSizeArray[i]);
    }

}

/* It is the Function to copy the Meta Data in the form of String */
char * copyMetaData(FILE * rightMetaPointer, int bytes, char *metaData)
{
    for(int i = 0; i < bytes; i++)
        metaData[i] = fgetc(rightMetaPointer);
}

/* It is the Function which Swaps the Meta Data so that all required holes must achieve consecutive positions */
void swapMetaData(unsigned long long int fileSizeArray[],int fileNameSizeArray[],int metaArray[],FILE *leftMetaPointer,FILE *rightMetaPointer,unsigned int countOfFiles, struct node * minSwapIndex)
{

    int countOfNonHoles = 0, j = 0;
    fseek(leftMetaPointer,4,SEEK_SET);
    fseek(rightMetaPointer,4,SEEK_SET);

    for(int i = minSwapIndex->leftIndex; i < minSwapIndex->rightIndex; i++)
        if(fileNameSizeArray[i]!=1)
            countOfNonHoles++;

    char * metaData[countOfNonHoles];
    int metaBytes[countOfNonHoles];

    for(int i = 0; i < minSwapIndex->leftIndex; i++)
    {
        fseek(leftMetaPointer,metaArray[i]+fileNameSizeArray[i],SEEK_CUR);
        fseek(rightMetaPointer,metaArray[i]+fileNameSizeArray[i],SEEK_CUR);
    }

    for(int i = minSwapIndex->leftIndex; i <= minSwapIndex->rightIndex; i++)
    {
        if(fileNameSizeArray[i]!=1)
        {
            metaData[j] = (char *)malloc((metaArray[i]+fileNameSizeArray[i]) * sizeof(char));
            metaBytes[j] = metaArray[i]+fileNameSizeArray[i];
            copyMetaData(rightMetaPointer,metaArray[i]+fileNameSizeArray[i],metaData[j]);
            j++;
        }
        else
            replaceMetaData(leftMetaPointer,rightMetaPointer,metaArray[i]+fileNameSizeArray[i]);
    }

    for(int i = 0; i < countOfNonHoles; i++)
        for(int j = 0; j < metaBytes[i]; j++)
            fputc(metaData[i][j],leftMetaPointer);

    
}

/* It is the Environment Function for the Swapping of the Data in Both Meta Data Section and File Data Section to set the position of the holes as consecutive */
void swapData(unsigned long long int fileSizeArray[],int fileNameSizeArray[],int metaArray[],FILE *leftFilePointer,FILE *rightFilePointer,FILE *leftMetaPointer,FILE *rightMetaPointer,unsigned int countOfFiles,struct node* minSwapIndex)
{
    swapFileData(fileSizeArray,fileNameSizeArray,metaArray,leftFilePointer,rightFilePointer,countOfFiles,minSwapIndex);

    swapMetaData(fileSizeArray,fileNameSizeArray,metaArray,leftMetaPointer,rightMetaPointer,countOfFiles,minSwapIndex);
}

/* It is the Defragmentor which defragments the Disk, In other words, Makes the decision to Swap the File Data and creates the continuous available Space for File which is needed to be added in the Virtual Disk and returns the Index too, where the hole is created for the File */
int defragmentor(const char *vDisk, union decStore fileSize)
{
    FILE * countPointer = fopen(vDisk,"r+");

    unsigned int countOfFiles = getNoOfFilesInDisk(countPointer);

    FILE * leftMetaPointer = fopen(vDisk,"r+");

    FILE * rightMetaPointer = fopen(vDisk,"r+");

    FILE * leftFilePointer = fopen(vDisk,"r+");

    FILE * rightFilePointer = fopen(vDisk,"r+");

    unsigned int countOfHoles = getNoOfHolesInDisk(leftMetaPointer,countOfFiles);

    unsigned long long fileSizeArray[countOfFiles];

    int fileNameSizeArray[countOfFiles], metaArray[countOfFiles], holesArray[countOfHoles];

    struct node * minSwapIndex = getIndexesForDeFrag(vDisk,fileSize,leftMetaPointer,fileSizeArray,fileNameSizeArray,metaArray,holesArray,countOfFiles,countOfHoles);

    swapData(fileSizeArray,fileNameSizeArray,metaArray,leftFilePointer,rightFilePointer,leftMetaPointer,rightMetaPointer,countOfFiles,minSwapIndex);
    
    fclose(countPointer);
    
    fclose(leftMetaPointer);
    
    fclose(rightMetaPointer);
    
    fclose(leftFilePointer);
    
    fclose(rightFilePointer);

	mergeConsecutiveHoles(vDisk);

    return minSwapIndex->leftIndex;
}

/* It is the Function which returns the Hole Size at Particular index */
unsigned long long int getHoleSizeAtIndex(FILE *metaDataPointer,int index)
{
    fseek(metaDataPointer,4,SEEK_SET);
    for(int i = 0; i < index; i++)
    {
        decode(metaDataPointer);
        if(fgetc(metaDataPointer)!=0)
			skipName(metaDataPointer);
    }
    return decode(metaDataPointer);
}

/* It is the Function which returns the required Space for the meta data to add the Encoded Sequence for the file Size and the left out SPace in the hole to generate as new hole along with the File Name */
int getReqSpaceForMetaData(unsigned long long int fileSize, const char *inputFileName, unsigned long long int holeSize)
{
    return calCountOfBytesSeq(fileSize) + calCountOfBytesSeq(holeSize - fileSize) + strlen(inputFileName) + 3;
}

/*
 * canMetaDataBeAdded Function returns true if Virtual Disk whose file Pointer was provided as an input to it
 * have the Space to Add new File in it
 * else returns false
 */
bool canMetaDataBeAdded(const char *vDiskName, unsigned long long int fileSize, FILE *metaDataPointer, unsigned int countOfFiles, const char *inputFileName, unsigned long long int holeSize)
{
	return getFreeSpace(getUsedSpace(metaDataPointer, countOfFiles),vDiskName) >= getReqSpaceForMetaData(fileSize,inputFileName,holeSize);
}

/* It is the Function Which stores the File Data which needed to be Added in the Hole At particular index */
void storeDataInHoleAtIndex(FILE *dataPointer,FILE *metaDataPointer,unsigned long long int fileSize,const char *inputFileName, int index)
{
    fseek(dataPointer,0,SEEK_END);
    fseek(metaDataPointer,4,SEEK_SET);
    for(int i = 0; i <= index; i++)
    {
        fseek(dataPointer,-decode(metaDataPointer),SEEK_CUR);
        if (fgetc(metaDataPointer)!=0)
            skipName(metaDataPointer);
    }

    FILE * inputFile = fopen(inputFileName,"r");

    for(int i = 0; i < fileSize; i++)
        fputc(fgetc(inputFile),dataPointer);

    fclose(inputFile);
}

/* This Function Updates the Meta Data Part of the File from the Virtual Disk, After Adding the File in the Hole */
void replaceMetaDataToRight(FILE *leftMetaPointer,FILE *rightMetaPointer,unsigned int bytes)
{
	for(int i=0; i < bytes; i++)
	{
		fputc(fgetc(leftMetaPointer),rightMetaPointer);
		fseek(leftMetaPointer,-2,SEEK_CUR);
		fseek(rightMetaPointer,-2,SEEK_CUR);
	}
	fseek(rightMetaPointer,1,SEEK_CUR);
	fseek(leftMetaPointer,1,SEEK_CUR);
}

/* It updates the Meta Data by adding the encoded Seq of File size and left over size of the hole along with the File Name */
void updateMetaData(FILE *leftMetaPointer,FILE *rightMetaPointer,char *outOfFileSize, unsigned int sizeOfOutOfFileSize, char * outOfNewHoleSize, unsigned int sizeOfOutOfNewHoleSize,const char *inputFileName, unsigned int countOfFiles, int index)
{
    fseek(leftMetaPointer,4,SEEK_SET);
    fseek(rightMetaPointer,4,SEEK_SET);

	int metaDataSize[countOfFiles], bytes;

    for(int i = 0; i < countOfFiles; i++)
    {
        metaDataSize[i] = calCountOfBytesSeq(decode(rightMetaPointer));
		fseek(leftMetaPointer,metaDataSize[i],SEEK_CUR);

		if(fgetc(rightMetaPointer)==1)
			metaDataSize[i] = metaDataSize[i] + getLengthOfFileName(rightMetaPointer) + 1;
		else
			metaDataSize[i] = metaDataSize[i] + 1;

        if(i==index)
			bytes = metaDataSize[i];
			
		if(fgetc(leftMetaPointer)==1)
        	skipName(leftMetaPointer);
    }

    fseek(rightMetaPointer,(sizeOfOutOfFileSize + sizeOfOutOfNewHoleSize + strlen(inputFileName) + 3) - bytes,SEEK_CUR);

    for(unsigned int i=countOfFiles-1; i > index; i--){
		fseek(leftMetaPointer,-1,SEEK_CUR);
		fseek(rightMetaPointer,-1,SEEK_CUR);
		replaceMetaDataToRight(leftMetaPointer,rightMetaPointer,metaDataSize[i]);
	}

    fseek(leftMetaPointer,4,SEEK_SET);
		for(int i = 0; i < index; i++)
			fseek(leftMetaPointer,metaDataSize[i],SEEK_CUR);

    for(int i = 0; i < sizeOfOutOfNewHoleSize; i++)
        fputc(*(outOfNewHoleSize + i),leftMetaPointer);
    
    fputc('\0',leftMetaPointer);

    for(int i = 0; i < sizeOfOutOfFileSize; i++)
        fputc(*(outOfFileSize + i),leftMetaPointer);
    
    fputc(1,leftMetaPointer);

    for(int i = 0; i < strlen(inputFileName); i++)
        fputc(*(inputFileName + i),leftMetaPointer);

    fputc('\0',leftMetaPointer);

}

/* It it the Environment Function for Adding the FIle After Defragmentation, where the hole was created for the reuse to add the new File */
void addFileAtIndex(const char * vDisk, union decStore fileSize, const char * inputFileName, int index)
{
    FILE * countPointer = fopen(vDisk,"r+");

    FILE * leftMetaPointer = fopen(vDisk,"r+");

    FILE * rightMetaPointer = fopen(vDisk,"r+");

    FILE * dataPointer = fopen(vDisk,"r+");

    unsigned int countOfFiles = getNoOfFilesInDisk(countPointer);

    unsigned long long int holeSize = getHoleSizeAtIndex(leftMetaPointer,index);

    if(!canMetaDataBeAdded(vDisk,fileSize.num,leftMetaPointer,countOfFiles,inputFileName,holeSize))
        printf("Error : Sufficient Space Not Available to Store %s !!!\n",inputFileName);
    else
    {
        storeDataInHoleAtIndex(dataPointer,leftMetaPointer,fileSize.num,inputFileName,index);
        char *outOfFileSize = (unsigned char *)calloc(11,sizeof(unsigned char));
        char *outOfNewHoleSize = (unsigned char *)calloc(11,sizeof(unsigned char));
        unsigned int sizeOfOutOfFileSize, sizeOfOutOfNewHoleSize;

        union decStore newHoleSize;
        newHoleSize.num = holeSize - fileSize.num;

		sizeOfOutOfFileSize = bitsToBytes(getEncodedSeqOfFileSize(outOfFileSize,fileSize));
		sizeOfOutOfNewHoleSize = bitsToBytes(getEncodedSeqOfFileSize(outOfNewHoleSize,newHoleSize));
        
        updateMetaData(leftMetaPointer,rightMetaPointer,outOfFileSize,sizeOfOutOfFileSize,outOfNewHoleSize,sizeOfOutOfNewHoleSize,inputFileName,countOfFiles,index);

        free(outOfFileSize);
        free(outOfNewHoleSize);
    } 
}


/// Hard Compaction For Some Critical cases where File Can  be Added in the Free Space and Holes together ... For that Compaction is Applied Here ...


/* This Function replaces the Data Part of the File from the Virtual Disk over the Hole */
void replaceDataToCompact(FILE *leftMetaPointer,FILE *rightMetaPointer,unsigned long long int bytes)
{
	for(unsigned long long int i=0; i < bytes; i++)
	{
		fputc(fgetc(leftMetaPointer),rightMetaPointer);
		fseek(leftMetaPointer,-2,SEEK_CUR);
		fseek(rightMetaPointer,-2,SEEK_CUR);
	}
	fseek(rightMetaPointer,1,SEEK_CUR);
	fseek(leftMetaPointer,1,SEEK_CUR);
}

/* It is the Function which Swaps the File Data so that all required holes must be removed */
void compactFileData(unsigned long long int fileSizeArray[],int fileNameSizeArray[],int metaArray[],FILE *leftFilePointer,FILE *rightFilePointer,unsigned int countOfFiles)
{
    fseek(leftFilePointer,0,SEEK_END);
    fseek(rightFilePointer,0,SEEK_END);

    for(int i = 0; i < countOfFiles; i++)
    {
        if(fileNameSizeArray[i]==1)
            fseek(leftFilePointer,-fileSizeArray[i],SEEK_CUR);
        else{
            fseek(rightFilePointer,-1,SEEK_CUR);
            fseek(leftFilePointer,-1,SEEK_CUR);
            replaceDataToCompact(leftFilePointer,rightFilePointer,fileSizeArray[i]);
        }
    }

}

/* It is the Function which Swaps the Meta Data so that all holes must be Removed */
void compactMetaData(unsigned long long int fileSizeArray[],int fileNameSizeArray[],int metaArray[],FILE *leftMetaPointer,FILE *rightMetaPointer,unsigned int countOfFiles)
{
    fseek(leftMetaPointer,4,SEEK_SET);
    fseek(rightMetaPointer,4,SEEK_SET);

    for(int i = 0; i < countOfFiles; i++)
    {
        if(fileNameSizeArray[i]==1)
            fseek(rightMetaPointer,(metaArray[i]+fileNameSizeArray[i]),SEEK_CUR);
        else
            replaceMetaData(leftMetaPointer,rightMetaPointer,metaArray[i]+fileNameSizeArray[i]);
    } 
}


/* It is the Environment Function for the Compaction of the Data in Both Meta Data Section and File Data Section to remove all holes */
void compaction(unsigned long long int fileSizeArray[],int fileNameSizeArray[],int metaArray[],FILE *leftFilePointer,FILE *rightFilePointer,FILE *leftMetaPointer,FILE *rightMetaPointer,unsigned int countOfFiles)
{
    compactFileData(fileSizeArray,fileNameSizeArray,metaArray,leftFilePointer,rightFilePointer,countOfFiles);

    compactMetaData(fileSizeArray,fileNameSizeArray,metaArray,leftMetaPointer,rightMetaPointer,countOfFiles);
}


/* It it the Environment Function for Removing all the Holes to Free Up the Space */
void hardCompaction(const char *vDisk)
{
    FILE * countPointer = fopen(vDisk,"r+");

    unsigned int countOfFiles = getNoOfFilesInDisk(countPointer);

    FILE * leftMetaPointer = fopen(vDisk,"r+");

    FILE * rightMetaPointer = fopen(vDisk,"r+");

    FILE * leftFilePointer = fopen(vDisk,"r+");

    FILE * rightFilePointer = fopen(vDisk,"r+");

    unsigned int countOfHoles = getNoOfHolesInDisk(leftMetaPointer,countOfFiles);

    unsigned long long fileSizeArray[countOfFiles];

    int fileNameSizeArray[countOfFiles], metaArray[countOfFiles];

    createMetaTable(leftMetaPointer,fileSizeArray,fileNameSizeArray,metaArray,countOfFiles);

    compaction(fileSizeArray,fileNameSizeArray,metaArray,leftFilePointer,rightFilePointer,leftMetaPointer,rightMetaPointer,countOfFiles);

    updateNoOfFilesInDisk(countPointer,countOfFiles-countOfHoles);

    fclose(countPointer);
    
    fclose(leftMetaPointer);
    
    fclose(rightMetaPointer);
    
    fclose(leftFilePointer);
    
    fclose(rightFilePointer);

}