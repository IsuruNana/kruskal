#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "cda.h"
#include "queue.h"

struct QUEUE
{
	CDA *circularDA;
	void (*display)(FILE* file, void* data);
};

QUEUE *newQUEUE(void (*d)(FILE *,void *))
{
	QUEUE *mQueue = malloc(sizeof(QUEUE));
	assert(mQueue != 0);
	mQueue->circularDA = newCDA(d);
	mQueue->display = d;
	return mQueue;
}
void enqueue(QUEUE *items,void *value)
{
	insertCDAback(items->circularDA, value);
}
void *dequeue(QUEUE *items)
{
	assert(sizeCDA(items->circularDA) > 0);
	void *deqVal = removeCDAfront(items->circularDA);
	return deqVal;
}
void *peekQUEUE(QUEUE *items)
{
	assert(sizeCDA(items->circularDA) > 0);
	void *peekVal = getCDA(items->circularDA, 0);
	return peekVal;
}
int sizeQUEUE(QUEUE *items)
{
	int size = sizeCDA(items->circularDA);
	return size;
}
int isEmptyQUEUE(QUEUE *items)
{
	if(sizeQUEUE(items) > 0 ) return 0;  //FALSE
	else return 1;                       //TRUE
}
void displayQUEUE(FILE *fp, QUEUE *items)
{
	int i;
        int queueSize = sizeCDA(items->circularDA);

        fprintf(fp, "<");

        for(i = 0; i < queueSize; i++)
        {
		items->display(fp, getCDA(items->circularDA, i));
		if(i != queueSize - 1) fprintf(fp, ",");
	}

        fprintf(fp, ">");
}
void visualizeQUEUE(FILE *fp, QUEUE *items)
{
	displayCDA(fp, items->circularDA);
}
