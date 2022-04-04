#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Define the buffer size which will be used to define physical memory and what not
#define BUFFER_SIZE 256
//Define the table size 
#define TLB_SIZE 16

//Initialize a struct called TLB
struct TLB{
	//The struct has parameters of page, frame, and index
	unsigned char TLBpage[16], TLBframe[16];
	int ind;
};

//Initialize and define readBin function
int readBin(int pageNum, char *PM, int* OF){
	//Initialize a buffer
	char buffer[BUFFER_SIZE];
	//Set buffer to zero
	memset(buffer, 0, sizeof(buffer));
	//Initialize file variable
	FILE *fp;

	//Open BACKING_STORE bin file using rb as the file is an non text file
	fp = fopen("BACKING_STORE.bin", "rb");
	//If the fopen fails then exit program
	if (fp == NULL){
		printf("File failed to open\n");
		exit(0);
	}
	//Set the file position of the stream to the offset
	if(fseek(fp, pageNum * BUFFER_SIZE, SEEK_SET) != 0){
        printf("error in fseek\n");
    }
	//Read the data within the file
	else if(fread(buffer, sizeof(char), BUFFER_SIZE, fp) == 0){
		printf("error in fread\n");
    }
	//For loop that will add the to buffer
	for(int x=0; x<BUFFER_SIZE; x++){
		*((PM + (*OF) *BUFFER_SIZE) + x) = buffer[x];
	}
	//Increment pointer and return
	(*OF)++;
	return (*OF)-1;
}

//Initialize and define findPage function
int findPage(int logicalAddr, char* PT, struct TLB *tlb,  char* PM, int* OF, int* pageFaults, int* TLBhits){
	//Initialize variables that will be used within the function
	unsigned char mask = 0xFF, offset, pageNum;
	bool TLBhit = false;
	int frame = 0, value, newFrame = 0;

	printf("Virtual adress: %d\t", logicalAddr);
	//Assign variable to logicaladder shifted to the right and anded wit
	pageNum = (logicalAddr >> 8) & mask;	
	//Assigned offset to logicalAdder AND mask
	offset = logicalAddr & mask;
	//Loop through pages in the tab;e
	for(int x=0; x<TLB_SIZE; x++){
		//If the page is the page we are looking for
		if(tlb->TLBpage[x] == pageNum){
			//Add the frame
			frame = tlb->TLBframe[x];
			//Set TLBhit to true and increment counter
			TLBhit = true;
			(*TLBhits)++;
		}
	}
	//Check if the hit checker variable was never changed
	if(TLBhit == false){
		//Check if the value is in the bin file
		if(PT[pageNum] == -1){
			newFrame = readBin(pageNum, PM, OF);
			PT[pageNum] = newFrame;
			(*pageFaults)++;
		}
		//Set frame to the page table value
		frame = PT[pageNum];
		//Add to table
		tlb->TLBpage[tlb->ind] = pageNum;
		tlb->TLBframe[tlb->ind] = PT[pageNum];
		tlb->ind = (tlb->ind + 1) % TLB_SIZE;
	}
	//Assign index
	int index = ((unsigned char) frame * BUFFER_SIZE) + offset;
	//Assign value
	value = *(PM + index);
	//Print values and return
	printf("Physical address: %d\t Value: %d\n", index, value);	
	return 0;
}

//Initialize and define main function
int main (int argc, char* argv[]){
	//Initialize variables that will be used in the function
	int val, openFrame = 0, pageFaults = 0, TLBhits = 0, inputCount = 0;
	float pageFaultRate, TLBHitRate;
	//Intialize physical memory and page table variables
	char phyMem[BUFFER_SIZE][BUFFER_SIZE], pageTable[BUFFER_SIZE];
	//Initialize file variable
	FILE *fd;
	//Initialize tlb struct
	struct TLB tlb;	
	//Set the variables to -1
	memset(pageTable, -1, sizeof(pageTable));	
	memset(tlb.TLBpage, -1, sizeof(tlb.TLBpage));
	memset(tlb.TLBframe, -1, sizeof(tlb.TLBframe));
	//Set starting index to 0
	tlb.ind = 0;
	//Check is theres enough arguments in the execution line in the terminal
	if(argc < 2){
		printf("Not enough arguments\nProgram Exiting\n");
		exit(0);
	}
	//Use the file passed through the execution line as the file to use for program
	fd = fopen(argv[1], "r");
	//If the file doesnt exist, exit the program
	if(fd == NULL){
		printf("File failed to open\n");
		exit(0);
	}
	//Scan the values within the file opened
	while(fscanf(fd, "%d", &val) == 1){
		//Call findPage function
		findPage(val, pageTable, &tlb, (char*) phyMem, &openFrame, &pageFaults, &TLBhits);
		//Incremement input count
		inputCount++;
	}
	//Find fault rate
	pageFaultRate = (float) pageFaults / (float) inputCount;
	//Find hit rate
	TLBHitRate = (float) TLBhits / (float) inputCount;
	//Print the values, close file, and exit program
	printf("Page Fault Rate = %.4f\nTLB hit rate= %.4f\n", pageFaultRate, TLBHitRate);
	fclose(fd);
	return 0;
}