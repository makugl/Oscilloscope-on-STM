#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H
//#include <stm32f072xb.h>
#include <stdbool.h>
#include <stdlib.h>

#define OK 0
#define ERROR_WRONG_INPUT -1
#define ERROR_BUFFER_IS_EMPTY -2
#define WARNING_BUFFER_FULL 1

typedef struct Buffer_s { 
	double *buffer;
	size_t head;
	size_t tail;
	size_t max; //of the buffer
  bool full;
} Buffer_t;

int Buffer_Initialize(Buffer_t * buffer, double * data, size_t size);
int Buffer_PutByte(Buffer_t * buffer, const double data);
int Buffer_GetByte(Buffer_t * buffer, double * data);

#endif
