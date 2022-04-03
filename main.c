#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define TLB_SIZE 16

struct TLB{
	unsigned char TLBpage[16], TLBframe[16];
	int ind;
};

int readBin(int pageNum, char *PM, int* OF){
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	FILE *fp;

	fp = fopen("BACKING_STORE.bin", "rb");
	if (fp == NULL){
		printf("File failed to open\n");
		exit(0);
	}

	if(fseek(fp, pageNum * BUFFER_SIZE, SEEK_SET) != 0){
        printf("error in fseek\n");
    }

	else if(fread(buffer, sizeof(char), BUFFER_SIZE, fp) == 0){
		printf("error in fread\n");
    }
	
	for(int x=0; x<BUFFER_SIZE; x++){
		*((PM + (*OF) *BUFFER_SIZE) + x) = buffer[x];
	}
	
	(*OF)++;
	return (*OF)-1;
}

int findPage(int logicalAddr, char* PT, struct TLB *tlb,  char* PM, int* OF, int* pageFaults, int* TLBhits){
	unsigned char mask = 0xFF, offset, pageNum;
	bool TLBhit = false;
	int frame = 0, value, newFrame = 0;

	printf("Virtual adress: %d\t", logicalAddr);

	pageNum = (logicalAddr >> 8) & mask;	

	offset = logicalAddr & mask;

	for(int x=0; x<TLB_SIZE; x++){
		if(tlb->TLBpage[x] == pageNum){
			frame = tlb->TLBframe[x];
			TLBhit = true;
			(*TLBhits)++;
		}
	}

	if(TLBhit == false){
		if(PT[pageNum] == -1){
			newFrame = readFromDisk(pageNum, PM, OF);
			PT[pageNum] = newFrame;
			(*pageFaults)++;
		}

		frame = PT[pageNum];
		tlb->TLBpage[tlb->ind] = pageNum;
		tlb->TLBframe[tlb->ind] = PT[pageNum];
		tlb->ind = (tlb->ind + 1) % TLB_SIZE;
	}
	
	int index = ((unsigned char) frame * BUFFER_SIZE) + offset;
	value = *(PM + index);

	printf("Physical address: %d\t Value: %d\n", index, value);	
	return 0;
}

int main (int argc, char* argv[]){
	int val;
	int openFrame = 0;
	int pageFaults = 0;
	int TLBhits = 0;
	int inputCount = 0;
	float pageFaultRate;
	float TLBHitRate;
	FILE *fd;
	char PhyMem[BUFFER_SIZE][BUFFER_SIZE]; 
	unsigned char PageTable[BUFFER_SIZE];

	memset(PageTable, -1, sizeof(PageTable));	

	struct TLB tlb;	
	memset(tlb.TLBpage, -1, sizeof(tlb.TLBpage));
	memset(tlb.TLBframe, -1, sizeof(tlb.TLBframe));
	tlb.ind = 0;

	if(argc < 2){
		printf("Not enough arguments\nProgram Exiting\n");
		exit(0);
	}

	fd = fopen(argv[1], "r");
	if(fd == NULL){
		printf("File failed to open\n");
		exit(0);
	}
	
	while(fscanf(fd, "%d", &val) == 1){
		findPage(val, PageTable, &tlb, (char*) PhyMem, &openFrame, &pageFaults, &TLBhits);
		inputCount++;
	}

	pageFaultRate = (float) pageFaults / (float) inputCount;
	TLBHitRate = (float) TLBhits / (float) inputCount;
	printf("Page Fault Rate = %.4f\nTLB hit rate= %.4f\n", pageFaultRate, TLBHitRate);

	close(fd);
	return 0;
}