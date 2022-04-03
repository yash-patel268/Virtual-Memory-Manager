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

