#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "da.h"

struct DA
{
    int itemsCount;          //Number of elements in array
    int arrayLength;	     //Actual length of array
    void **itemsArray;		//Array to store variables
    void (*display)(FILE* file, void* data); //Display function to print
};

DA* newDA(void (*d)(FILE *file, void *data))
{
	DA *newDA = malloc(sizeof(DA)); //Allocate memory for DA
	assert(newDA != 0);
	newDA->itemsArray = malloc(sizeof(void*));
	newDA->itemsCount = 0;
	newDA->arrayLength = 1;
	newDA->display = d;

	return newDA;
}

void insertDA(DA *items, void *value)
{
	if(items->itemsCount == items->arrayLength) // Array needs to expand
	{
		int newSize = items->arrayLength*2;

		items->itemsArray = realloc(items->itemsArray, sizeof(void*) * newSize);
		items->arrayLength = newSize;
	}

	items->itemsArray[items->itemsCount] = value;
	items->itemsCount++;
}

void *removeDA(DA *items)
{
    assert(items->itemsCount > 0);

    void *returnValue = items->itemsArray[items->itemsCount-1];
    items->itemsCount--;

    if( (float) (items->itemsCount) / (float) (items->arrayLength)  < 0.25 && (items->arrayLength != 1)) //Shrink if necassary
    {
        int newSize = items->arrayLength/2;
	      items->itemsArray = realloc(items->itemsArray, sizeof(void*) * newSize);
	      items->arrayLength = newSize;
    }

    return returnValue;
}

void unionDA(DA *recipient,DA *donor)
{
    int i;
    int donorSize = donor->itemsCount;
    void **donorArray = extractDA(donor); //Handle shrinking and retrieveing donor array.
    for(i = 0; i < donorSize; i++)
    {
	     insertDA(recipient, donorArray[i]); //Add to recipient
    }
    free(donorArray);
}

void *getDA(DA *items, int index)
{
    assert(index >= 0 && index < items->itemsCount);
    return items->itemsArray[index];
}

void *setDA(DA *items, int index, void *value)
{
	assert(index >= 0 && index <= items->itemsCount);
	if(index == items->itemsCount)
	{
		insertDA(items, value);
	}

	else
	{
		void* tempValue = items->itemsArray[index];
		items->itemsArray[index] = value;
		return tempValue;
	}

	return NULL;
}


void **extractDA(DA *items)
{
    if(items->itemsCount > 0)
    {
    	items->itemsArray = realloc(items->itemsArray, (sizeof(void*) * (items->itemsCount)));
	     assert(items->itemsArray != 0);
    }

    void **returnArray = items->itemsArray;
    items->itemsArray = malloc(sizeof(void*));
    assert(items->itemsArray != 0);
    items->itemsCount = 0;
    items->arrayLength = 1;
    return returnArray;
}

int sizeDA(DA *items)
{
	return items->itemsCount;
}

void displayDA(FILE *fp, DA *items)
{
	int i;
	fprintf(fp, "[");

	for(i = 0; i < items->itemsCount; i++)
	{
		items->display(fp, items->itemsArray[i]);
		if(i != items->itemsCount - 1) fprintf(fp, ",");
		//fprintf(fp, "\n");
	}

	fprintf(fp, "]");
}

void visualizeDA(FILE *fp, DA *items)
{
	displayDA(fp, items);

	int remainingSlots = (items->arrayLength - items->itemsCount);
	fprintf(fp, "[%d]", remainingSlots);
}
