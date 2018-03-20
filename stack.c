#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "stack.h"
#include "da.h"

struct STACK
{
	DA *dynamicArray;
	void (*display)(FILE* file, void* data);
};

STACK *newSTACK(void (*d)(FILE *, void *))
{
	STACK *mStack = malloc(sizeof(STACK));
	assert(mStack != 0);
	mStack->dynamicArray = newDA(d);
	mStack->display = d;
	return mStack;
}
void push(STACK *items, void *value)
{
	insertDA(items->dynamicArray, value);
}
void *pop(STACK *items)
{
	assert(sizeDA(items->dynamicArray) > 0);
	void* pop = removeDA(items->dynamicArray);
	return pop;
}
void *peekSTACK(STACK *items)
{
	assert(sizeDA(items->dynamicArray) > 0);
	void* peek = getDA(items->dynamicArray, (sizeDA(items->dynamicArray) -1));
	return peek;
}
int sizeSTACK(STACK *items)
{
	int size = sizeDA(items->dynamicArray);
	return size;
}
void displaySTACK(FILE *fp, STACK *items)
{
	int i;
	int stackSize = sizeDA(items->dynamicArray) - 1;

	fprintf(fp, "|");

	for(i = stackSize; i >= 0; i--) //Prints in reverse
	{
		items->display(fp, getDA(items->dynamicArray, i));
		if(i != 0) fprintf(fp, ",");
	}

	fprintf(fp, "|");
}
void visualizeSTACK(FILE *fp ,STACK *items)
{
	displayDA(fp, items->dynamicArray);
}
int isEmptySTACK(STACK *items)
{
	int size = sizeSTACK(items);
	if(size == 0) return 1;
	else return 0;
}
