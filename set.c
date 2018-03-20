#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "set.h"
#include "da.h"

typedef struct SETNODE SETNODE;

struct SETNODE
{
	SETNODE* parent;
	void *value;
	int rank;
	int index;
};

static SETNODE* findSETNODE(SETNODE* node)
{
	SETNODE *parentSETNODE = node->parent;
	if(parentSETNODE == node)
	{
		return parentSETNODE;
	}
	node->parent = findSETNODE(node->parent);
	return node->parent;
}

struct SET
{
	DA *setMembers;
	void (*display)(FILE *, void *);
};

SET *newSET(void (*d)(FILE *,void *))
{
	SET *disjointSET = malloc(sizeof(SET));

	disjointSET->setMembers = newDA(d);
	disjointSET->display = d;

	return disjointSET;
}

/*makeSET - The makeSET method inserts a wrapped value
	into the underlying dynamic array.
	It returns the index of the inserted value.
	Presumably, the caller of makeSET saves the return value
	so that findSET and unionSET can be called in the future. */

int makeSET(SET *d,void *value)
{
	SETNODE *newNode = malloc(sizeof(SETNODE));

	newNode->parent = newNode;
	newNode->value = value;
	newNode->rank = 0;

	int index = sizeDA(d->setMembers);
	newNode->index = index;
	insertDA(d->setMembers, newNode);

	return index;
}
// findSET - The findSET method returns the index of the representative of the value stored at the given index.
int findSET(SET *d,int index)
{
	SETNODE *currentSETNODE = getDA(d->setMembers, index);
	SETNODE *parentSETNODE = findSETNODE(currentSETNODE);
	//int parentIndex = findSETNODE(currentSETNODE)->index;
	return parentSETNODE->index;
}

/* 								||unionSET||
--------------------------------------------------------------------------------
1) The union method runs findSET on each of the arguments.

2) If they have the same representatives, then a zero is returned.

3) If the arguments have different representatives,
the two disjoint sets are unioned and a value of one is returned.

4) If the two differing representatives have the same rank,
then the representative with the lower index becomes the parent of the other.

*/

int unionSET(SET *d,int index1,int index2)
{
	//1
	if(findSET(d, index1) != findSET(d, index2))
	{
		SETNODE *represent1 = getDA(d->setMembers, findSET(d, index1));
		SETNODE *represent2 = getDA(d->setMembers, findSET(d, index2));

		if(represent1->rank == represent2->rank)
		{
			//4
			if(represent1->index < represent2->index)
			{
				represent1->rank++;
				represent2->parent = represent1;
			}

			else
			{
				represent2->rank++;
				represent1->parent = represent2;
			}
		}

		else
		{
			//3
			if(represent1->rank < represent2->rank)
			{
				represent2->rank++;
				represent1->parent = represent2;
			}

			else
			{
				represent1->rank++;
				represent2->parent = represent1;
			}
		}

		return 1;
	}

	//2
	return 0;
}
int countSET(SET *d)
{
	int size = sizeDA(d->setMembers);
	int i, count = 0;
	int parentIndex;

	for(i = 0; i < size; i++)
	{
		parentIndex = findSET(d, i);
		if(getDA(d->setMembers, i) == getDA(d->setMembers, parentIndex))
		{
			count++;
		}
	}

	return count;
}
int displaySET(FILE *fp,SET *d)
{
	int size = sizeDA(d->setMembers);
	int i;

	for(i = 0; i < size; i++)
	{
		fprintf(fp, "%d:", i);

		SETNODE *node = getDA(d->setMembers, i);
		SETNODE *parent = node->parent;

		while(1)
		{
			fprintf(fp, " ");
			d->display(fp, node->value);
			if(node == parent) break;
			else
			{
				node = parent;
				parent = node->parent;
			}
		}

		fprintf(fp, "\n");
	}
	return 0;
}
