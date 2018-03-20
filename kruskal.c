#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "integer.h"
#include "da.h"
#include "set.h"
#include "queue.h"
#include "scanner.h"
#include "rbt.h"
#include "bst.h"

typedef struct LISTNODE LISTNODE;
typedef struct LIST LIST;

typedef struct EDGE EDGE;
typedef struct GRAPH GRAPH;

static LISTNODE *newLISTNODE(EDGE*);
static void insertLISTNODESorted(LIST *, LISTNODE *);
static LIST *getSORTEDLIST(QUEUE*, int*);
static int getLISTNODEDest(LISTNODE*);

static int sizeLIST(LIST*);

static EDGE* newEDGE(int, int, int);
static void displayEDGE(FILE*, void*);
static int compareEDGE(void*, void*);
static int getEDGEV1(EDGE *);
static int getEDGEV2(EDGE *);
static int getEDGEWeight(EDGE *);

static GRAPH *newGRAPH(void (*d)(FILE *,void *), int);
static void addEDGE(GRAPH *, EDGE *);
static void displayGRAPH(FILE*, void*);
static int displayTREE(FILE *fp, GRAPH *graph, QUEUE *listQUEUE, int *visitedVERTICES);

static int readDataIntoRBT(FILE*, RBT*, RBT*);

struct EDGE
{
	int vertex1;
	int vertex2;
	int weight;
};

struct GRAPH
{
	int vertexNum;
	int smallestVertex;
	int largestVertex;
	LIST *arrayList;
	void (*display)(FILE *,void *);
};

// A structure to represent an adjacency list node
struct LISTNODE
{
		EDGE *edge;
    LISTNODE *next;
};
struct LIST
{
    LISTNODE *head;
		int nodeCount;
};
LISTNODE *newLISTNODE(EDGE *edge)
{
	LISTNODE *node = malloc(sizeof(LISTNODE));
	node->edge = edge;
	node->next = NULL;
	return node;
}
void insertLISTNODE(LIST *list, EDGE *edge)
{
	LISTNODE *newNODE = newLISTNODE(edge);

	if(list->head == NULL)
	{
		list->head = newNODE;
	}
	else
	{
		LISTNODE *current = list->head;
		while(current->next != NULL) current = current->next;
		current->next = newNODE;
	}

	list->nodeCount++;
}
void insertLISTNODESorted(LIST *list, LISTNODE *old)
{
	LISTNODE *node = newLISTNODE(old->edge);
	LISTNODE *current = list->head;
	LISTNODE *previous = NULL;
	list->nodeCount++;

	if(current == NULL) list->head = node;

	else
	{
		while(getLISTNODEDest(node) > getLISTNODEDest(current))
		{
			previous = current;
			current = current->next;

			if(current == NULL)
			{
				previous->next = node;
				return;
			}
		}
		if(previous == NULL) //NEW NODE LESS THAN HEAD
		{
			LISTNODE *oldHead = current;
			list->head = node;
			node->next = oldHead;
		}
		else
		{
			LISTNODE *temp = previous->next;
			previous->next = node;
			node->next = temp;
		}
	}
	return;
}
int getLISTNODESrc(LISTNODE * node)
{
	int src = getEDGEV1(node->edge);
	return src;
}
int getLISTNODEDest(LISTNODE * node)
{
	int dest = getEDGEV2(node->edge);
	return dest;
}
int getLISTNODEWeight(LISTNODE * node)
{
	int weight = getEDGEWeight(node->edge);
	return weight;
}
void displayLISTNODE(FILE *fp, LISTNODE *node)
{
	EDGE *e = node->edge;
	fprintf(fp, "%d(%d)%d", getEDGEV2(e), getEDGEV1(e), getEDGEWeight(e));
}
LIST *newLIST(int size)
{
	LIST *list = malloc(size * sizeof(LIST));

	int i = 0;
	for(i = 0; i < size; i++)
	{
		list[i].head = NULL;
		list[i].nodeCount = 0;
	}

	return list;
}
LIST *getLISTItem(LIST* list, int index)
{
	LIST *listITEM = &(list[index]);
	return listITEM;
}
int sizeLIST(LIST *list)
{
	int size = list->nodeCount;
	return size;
}
LIST *getSORTEDLIST(QUEUE *listQUEUE, int *visitedVERTICES)
{
	LIST *returnLIST = newLIST(1);
	LISTNODE *currentNode = NULL;

	while(!isEmptyQUEUE(listQUEUE))
	{
		//printf("Not empty ");
		LIST *currentLIST = dequeue(listQUEUE);
		currentNode = currentLIST->head;
		while(currentNode)
		{
			if(!visitedVERTICES[getLISTNODEDest(currentNode)]) //Only insert if not already visited
			{
				insertLISTNODESorted(returnLIST, currentNode);
			}
			currentNode = currentNode->next;
		}
	}
	return returnLIST;
}
GRAPH *newGRAPH(void (*d)(FILE *,void *), int largestVertex)
{
	GRAPH *graph = malloc(sizeof(GRAPH));
	graph->display = d;

	graph->largestVertex = largestVertex;
	graph->arrayList = newLIST(largestVertex);

	return graph;
}
void addEDGE(GRAPH *graph, EDGE *edge)
{
	int srcV = getEDGEV1(edge);
	LIST *list = getLISTItem(graph->arrayList, srcV);
	insertLISTNODE(list, edge);

	int destV = getEDGEV2(edge);
	EDGE *edge2 = newEDGE(destV, srcV, getEDGEWeight(edge));
	list = getLISTItem(graph->arrayList, destV);
	insertLISTNODE(list, edge2);
}
void displayGRAPH(FILE *fp, void* value)
{
	  GRAPH *graph = (GRAPH*) value;
		int size = graph->vertexNum;
		int listIndex = graph->smallestVertex; //PARENT VERTEX
		int largestVertex = graph->largestVertex;
		int nodesPrinted = 0;
		QUEUE *listQUEUE = newQUEUE(NULL);
		int *visitedVERTICES = malloc(largestVertex * sizeof(int));

		int i;
		for(i = 0; i < largestVertex; i++) visitedVERTICES[i] = 0;

		LIST *currentLIST = getLISTItem(graph->arrayList, listIndex);

		while(size > 0 && listIndex < largestVertex)
		{
			if(!visitedVERTICES[listIndex]) //NOT ALREADY VISITED
			{
				currentLIST = getLISTItem(graph->arrayList, listIndex);

				if(sizeLIST(currentLIST)) //LIST IS NOT EMPTY
				{
					fprintf(fp, "0 : ");
					fprintf(fp, "%d", listIndex);
					visitedVERTICES[listIndex] = 1;

					enqueue(listQUEUE, currentLIST);

					nodesPrinted = displayTREE(fp, graph, listQUEUE, visitedVERTICES);
					size -= nodesPrinted;

					listIndex = graph->smallestVertex;
				}

				else
				{
					listIndex++;
				}
			}

			else
			{
				listIndex++;
			}
		}
}
int displayTREE(FILE *fp, GRAPH *graph, QUEUE *listQUEUE, int *visitedVERTICES)
{
	int totalWeight = 0;
	LIST *nodeLIST;
	int level = 1;
	int nodesPrinted = 0;
	int listIndex;
	int levelPrinted = 0;

	while(!isEmptyQUEUE(listQUEUE)) //PRINT THE INDIVDUAL TREE
	{
			nodeLIST = getSORTEDLIST(listQUEUE, visitedVERTICES);
			LISTNODE *current = nodeLIST->head;

			while(current != NULL) //DISPLAY LEVEL
			{
				listIndex = getLISTNODEDest(current);

				if(!visitedVERTICES[listIndex])
				{
					if(levelPrinted == 0)
					{
						fprintf(fp, "\n%d : ", level);
						levelPrinted = 1;
					}
					displayLISTNODE(fp, current);
					if(current->next != NULL)
					{
						fprintf(fp, " ");
					}
					enqueue(listQUEUE, getLISTItem(graph->arrayList, listIndex)); //QUEUE NEXT LEVELS
					totalWeight += getLISTNODEWeight(current);
					visitedVERTICES[listIndex] = 1;
					visitedVERTICES[getLISTNODESrc(current)] = 1;
					nodesPrinted++;
				}

				current = current->next;
			}
			levelPrinted = 0;
			level++;

	}

	if(nodesPrinted != 0)
	{
		fprintf(fp, "\ntotal weight: %d\n", totalWeight);
		fprintf(fp, "----\n");
	}

	return nodesPrinted;
}

//-------------------------------EDGE FUNCTIONS------------------------//

EDGE* newEDGE(int v1, int v2, int weight)
{
	EDGE *edge = malloc(sizeof(EDGE));

	edge->vertex1 = v1;
	edge->vertex2 = v2;
	edge->weight = weight;

	return edge;
}

void displayEDGE(FILE *fp, void *value)
{
	EDGE *edge = value;

	fprintf(fp, "%d", getEDGEV1(edge));
	fprintf(fp, "---");
	fprintf(fp, "%d", getEDGEV2(edge));
	fprintf(fp, "  : ");
	fprintf(fp, "%d\n", getEDGEWeight(edge));
}
int compareEDGE(void *edge1, void *edge2)
{
	EDGE *e1 = edge1; //eg. (5, 8)
	EDGE *e2 = edge2; //eg. (3, 7)
	int diff = 0;

	if(getEDGEWeight(e1) != getEDGEWeight(e2)) //ABSOLUTE
	{
			return getEDGEWeight(e1) - getEDGEWeight(e2);
	}

	else if(getEDGEV1(e1) != getEDGEV1(e2)) // 5 != 3 (Not the same)
	{
			return getEDGEV1(e1) - getEDGEV1(e2);
	}

	else
	{
		  return getEDGEV2(e1) - getEDGEV2(e2);
	}

	return diff;
}

int getEDGEV1(EDGE *edge)
{
	int v1 = edge->vertex1;
	return v1;
}
int getEDGEV2(EDGE *edge)
{
	int v2 = edge->vertex2;
	return v2;
}
int getEDGEWeight(EDGE *edge)
{
	int weight = edge->weight;
	return weight;
}
//-----------------------------END EDGE FUNCTIONS------------------------//


//-----------------------------PRIVATE FUNCTIONS------------------------//

int readDataIntoRBT(FILE* fp, RBT* edgeRBT, RBT *vertexRBT)
{
	char *tokenString;
	int count = 0, v1 = 0, v2 = 0, largestVertex = -1;
	int edgeWeight = 1;
	EDGE *edge = NULL;

	//printf("Got here");
	while( ( tokenString = readToken(fp) ) )
	{
		  //printf("Reading %s\n", tokenString);
			if(count == 0) //Vertex one read : Guarunteed
			{
				v1 = atoi(tokenString);
				if(v1 > largestVertex) largestVertex = v1;
			}
			else if(count == 1) //Vertex two read : Guarunteed
			{
				v2 = atoi(tokenString);
				if(v2 > largestVertex) largestVertex = v2;
			}
			else if(count == 2)
			{
				if(strcmp(";", tokenString) != 0) //There is a given edge
				{
						edgeWeight = atoi(tokenString);
				}
				else if(strcmp(";", tokenString) == 0) //Value is semicolon
				{
					  edgeWeight = 1;
						count = -1; //Reset
				}

				if(v1 < v2) edge = newEDGE(v1, v2, edgeWeight);
				else edge = newEDGE(v2, v1, edgeWeight);

				insertRBT(edgeRBT, edge);
        insertRBT(vertexRBT, newINTEGER(v1));
        insertRBT(vertexRBT, newINTEGER(v2));
			}

			else if (strcmp(";" , tokenString) == 0)
			{
				count = -1; //Reset
			}

			count++;
	}
	return largestVertex;
}

/*KRUSKAL(G):
1 T = ∅
2 foreach v ∈ G.V:
3 	MAKE-SET(v)
4 foreach (u, v) ordered by weight(u, v), increasing:
5 	if FIND-SET(u) ≠ FIND-SET(v):
6 		T = T ∪ {(u, v)}
7 		UNION(u, v)
8 return T*/
int main(int argc, char **argv)
{
	int i, largestVertex = 0, smallestVertex;
	void **sortedEDGES;
  void **sortedVERTICES;

	if(strcmp(argv[argc-1], "-v") == 0)
	{
		printf("Isuru Nanayakkara\n");
		return 0;
	}

	FILE *inputFile = fopen(argv[argc-1], "r");
	//C:\\cygwin64\\home\\hp-pc\\assign3_v2
	//FILE *debugOutput = fopen("mst.txt","w");
	//FILE *rbtOutput = fopen("rbt.txt","w");
	//FILE *rbtOutput2 = fopen("rbt2.txt","w");
	SET *disjointSET = newSET(displayINTEGER);
	RBT *edgeRBT = newRBT(displayEDGE, compareEDGE);
  RBT *vertexRBT = newRBT(displayINTEGER, compareINTEGER); //Handles Duplicate vertices

	largestVertex = readDataIntoRBT(inputFile, edgeRBT, vertexRBT) + 1;
	sortedEDGES = getInorderArray(edgeRBT); //Sort the edges
  sortedVERTICES =  getInorderArray(vertexRBT); //Sort the vertixes. Has no duplicates

	int sizeEdges = sizeRBT(edgeRBT);
  int sizeVertices = sizeRBT(vertexRBT);
	int vertexIndixes[largestVertex];
	memset(vertexIndixes, -1, largestVertex * sizeof(int));

	i = 0;
	smallestVertex = largestVertex;

  //Make-set on all vertices
  for(i = 0; i < sizeVertices; i++)
  {
    int vertex = getINTEGER(sortedVERTICES[i]);
    vertexIndixes[vertex] = makeSET(disjointSET, newINTEGER(vertex));
		if(vertex < smallestVertex) smallestVertex = vertex;
  }

	GRAPH *kruskalGraph = newGRAPH(displayINTEGER, largestVertex);
	kruskalGraph->smallestVertex = smallestVertex;
	kruskalGraph->vertexNum = sizeVertices;

  //Ordered by weight union
  int v1, v2;
  for(i = 0; i < sizeEdges; i++)
  {
	  EDGE *edge = (EDGE*) sortedEDGES[i];
		v1 = getEDGEV1(edge);
		v2 = getEDGEV2(edge);

	  if(unionSET(disjointSET, vertexIndixes[v1], vertexIndixes[v2]))
		{
			addEDGE(kruskalGraph, edge);
		}
  }

	displayGRAPH(stdout, kruskalGraph);

	return 0;
}
