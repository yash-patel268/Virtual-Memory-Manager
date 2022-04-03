#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define TLB_SIZE 16

typedef struct {
	unsigned char TLBpage[16];
	unsigned char TLBframe[16];
	int ind;
} TLB;

int read(int pageNum, char *phyMem, int* offset){
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	FILE *fp;

	fp = fopen("BACKING_STORE.bin", "rb");
	if (fp == NULL){
		printf("File failed to open\n");
		exit(0);
	}

	if (fseek(fp, pageNum * BUFFER_SIZE, SEEK_SET)!=0){
        printf("error in fseek\n");
    }

	else if (fread(buffer, sizeof(char), BUFFER_SIZE, fp)==0){
		printf("error in fread\n");
    }
	
	for(int x=0; x<BUFFER_SIZE; x++){
		*((phyMem + (*offset)*BUFFER_SIZE)+x) = buffer[x];
	}
	
	(*offset)++;
	return (*offset)-1;
}