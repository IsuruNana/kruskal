#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "cda.h"

//DYNAMIC ARRAY STRUCT
struct CDA 
{
    int itemsCount;
    int arrayLength;
    int frontArrow; //Arrow to front of array
    int backArrow;  //Arrow to back of array
    void **itemsArray;
    void (*display)(FILE* file, void* data);
};

void adjustSize(CDA* items, float factor); //Called to grow or shrink array

CDA *newCDA(void (*d)(FILE *,void *))
{
    CDA *newCDA = malloc(sizeof(CDA));
    assert(newCDA != 0);
    newCDA->itemsArray = malloc(sizeof(void*));
    assert(newCDA->itemsArray != 0);
    newCDA->itemsCount = 0;
    newCDA->arrayLength = 1;
    newCDA->frontArrow = 0; //Starts at front
    newCDA->backArrow = 0;
    newCDA->display = d;
    
    return newCDA;
}

void insertCDAfront(CDA *items,void *value)
{
	if(items->itemsCount == items->arrayLength) adjustSize(items, 2);
	
	if(items->itemsCount != 0) //Not empty
	{
		if(items->frontArrow == 0) items->frontArrow = items->arrayLength-1;
		else items->frontArrow--;
	}
	
	items->itemsArray[items->frontArrow] = value;
	items->itemsCount++;
}

void insertCDAback(CDA *items,void *value)
{ 
    	if(items->itemsCount == items->arrayLength) adjustSize(items, 2);
    
	if(items->itemsCount != 0) //Not empty
	{
		if(items->backArrow == items->arrayLength-1) items->backArrow = 0;
		else items->backArrow++;
	}
    
    	items->itemsArray[items->backArrow] = value;
	items->itemsCount++;
}

void *removeCDAfront(CDA *items)
{
    assert(items->itemsCount > 0);
    
    void* returnValue = items->itemsArray[items->frontArrow];
    
    if(items->itemsCount == 1) //ONE ELEMENT IN ARRAY
    {
    	items->frontArrow = 0;
	items->backArrow = 0;
    }
    
    else
    {
    	if(items->frontArrow == items->arrayLength - 1) items->frontArrow = 0;
    	else items->frontArrow++;
    }
    
    items->itemsCount--;
    
    if((float) items->itemsCount < (float) ((float) items->arrayLength / (float) 4)) 
	{
		adjustSize(items, 0.5);
	}
        
    return returnValue;
}

void *removeCDAback(CDA *items)
{
    assert(items->itemsCount > 0);
	
    void* returnValue = items->itemsArray[items->backArrow];
    
    if(items->itemsCount == 1) //One element in array
    {
    	items->frontArrow = 0;
	items->backArrow = 0;
    }
    
    else
    {
    	if(items->backArrow == 0) items->backArrow = items->arrayLength-1;
    	else items->backArrow--;
    }
    
    items->itemsCount--;
    
    if((float) items->itemsCount < (float) ((float) items->arrayLength / (float) 4)) 
	{
		adjustSize(items, 0.5);
	}
    
    return returnValue;
}

void unionCDA(CDA *recipient,CDA *donor)
{
    int i;
    int donorSize = donor->itemsCount;
    void **donorArray = extractCDA(donor);
    for(i=0; i < donorSize; i++) insertCDAback(recipient, donorArray[i]);
    free(donorArray);
}

void *getCDA(CDA *items,int index)
{
    assert(index >= 0 && index < items->itemsCount);
    if(items->itemsCount == 0) return NULL; 
    int searchIndex = (items->frontArrow + index) % items->arrayLength;
    return items->itemsArray[searchIndex];
}

void *setCDA(CDA *items,int index,void *value)
{
    assert(index >= -1 && index <= items->itemsCount);
    
    if(index == items->itemsCount) insertCDAback(items, value);
    else if(index == -1) insertCDAfront(items, value);
    
    else
    {
        void *returnValue = items->itemsArray[index];
        items->itemsArray[index] = value;
        return returnValue;
    }
    
    return NULL;
}

void **extractCDA(CDA *items)
{
	int count;
	void **returnArray;
	if(items->itemsCount > 0)
	{
		returnArray = malloc(sizeof(void*) * items->itemsCount);
		assert(returnArray != 0);
	}
	
	else
	{
	        returnArray = malloc(sizeof(void*));
	}

	for(count=0; count < items->itemsCount; count++)
	{
		returnArray[count] = getCDA(items, count);
	}
    
    free(items->itemsArray);
    items->itemsArray = malloc(sizeof(void*));
    assert(items->itemsArray != 0);

    items->itemsCount = 0;
    items->arrayLength = 1;
    items->frontArrow = 0;
    items->backArrow = 0;
    return returnArray;
}

int sizeCDA(CDA *items)
{
    return items->itemsCount;
}

void visualizeCDA(FILE *fp, CDA *items)
{
    displayCDA(fp, items);
    
    int remainingSlots = (items->arrayLength - items->itemsCount);
    fprintf(fp, "(%d)", remainingSlots);
}

void displayCDA(FILE *fp, CDA *items)
{
    int i;
    fprintf(fp, "(");
    
    for(i=0; i < items->itemsCount; i++) 
	{
		items->display(fp, getCDA(items, i));
		if(i != items->itemsCount - 1) fprintf(fp, ",");
	}
    
    fprintf(fp, ")");
}

void adjustSize(CDA* items, float factor)
{
	//Factor determines grow or shrink
    int newSize = (int) (items->arrayLength * factor);
    if(newSize == 0) newSize = 1;

    int i;
    void **newArray = malloc(sizeof(void*) * newSize);
    assert(newArray != 0);
    
    for(i = 0; i < items->itemsCount; i++) 
    {
        newArray[i] = getCDA(items, i);
    }

    free(items->itemsArray);
    items->itemsArray = newArray;
    items->arrayLength = newSize;

    if(items->itemsCount > 0)
    {
        items->frontArrow = 0;
        items->backArrow = items->itemsCount-1;
    }

    else
    {
        items->frontArrow = 0;
        items->backArrow = 0;
    }
}

