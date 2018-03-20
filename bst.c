#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bst.h"
#include "queue.h"

static BSTNODE *findPredecessor(BST *tree, BSTNODE *node);
static BSTNODE *findSuccessor(BST *tree, BSTNODE *node);
static int minDepth(BST *tree, BSTNODE *node);
static int maxDepth(BST *tree, BSTNODE *node);
static int isBSTNODEleft(BSTNODE* node);
static int isBSTNODEright(BSTNODE* node);
static int isBSTNODELeaf(BSTNODE *node);

static void swapBSTNODE(BSTNODE *node1, BSTNODE *node2); //Default swapper

void swapBSTNODE(BSTNODE *node1, BSTNODE *node2)
{
  void *temp = getBSTNODE(node1);
  setBSTNODE(node1, getBSTNODE(node2));
  setBSTNODE(node2, temp);
}

int isBSTNODEleft(BSTNODE* node) //Is the passed node a left child
{
  if(node != NULL)
  {
    BSTNODE *parent = getBSTNODEparent(node);
    if(parent == NULL) return 0;  //Root of tree

    BSTNODE *left = getBSTNODEleft(parent);
    if(left == node) return 1;
  }

  return 0;
}

int isBSTNODEright(BSTNODE* node) //Is the passed node a right child
{
  if(node != NULL)
  {
    BSTNODE *parent = getBSTNODEparent(node);
    if(parent == NULL) return 0;  //Root of tree

    BSTNODE *right = getBSTNODEright(parent);
    if(right == node) return 1;
  }

  return 0;
}

int isBSTNODELeaf(BSTNODE *node)
{
  BSTNODE *left = getBSTNODEleft(node);
  BSTNODE *right = getBSTNODEright(node);
  if(left == NULL && right == NULL) return 1;
  return 0;
}

BSTNODE *findPredecessor(BST *tree, BSTNODE *node)
{
  BSTNODE *max = getBSTNODEleft(node);
  while(getBSTNODEright(max) != NULL) max = getBSTNODEright(max);
	return max;
}

BSTNODE *findSuccessor(BST *tree, BSTNODE *node)
{
  BSTNODE *min = getBSTNODEright(node);
  while(getBSTNODEleft(min) != NULL) min = getBSTNODEleft(min);
	return min;
}

int minDepth(BST *tree, BSTNODE *node)
{
	if(node == NULL) return 0;
	int depth = 1;

	QUEUE *depthQueue = newQUEUE(NULL);
	enqueue(depthQueue, node);

	while(1)
	{
		int nodeCount = sizeQUEUE(depthQueue);
		if (nodeCount == 0) break;

		while(nodeCount > 0)
		{
			BSTNODE *currentNode = dequeue(depthQueue);
			if(isBSTNODELeaf(currentNode)) return depth;
			if(getBSTNODEleft(currentNode) == NULL) return depth;
			if(getBSTNODEright(currentNode) == NULL) return depth;

			else
			{
					enqueue(depthQueue, getBSTNODEleft(currentNode));
					enqueue(depthQueue, getBSTNODEright(currentNode));
			}

			nodeCount--;

		}
		depth++;
	}

	return depth;
}

int maxDepth(BST* tree, BSTNODE *node)
{
  if (node == NULL) return 0;

  else
  {
     /* compute the depth of each subtree */
     int lDepth = maxDepth(tree, getBSTNODEleft(node));
     int rDepth = maxDepth(tree, getBSTNODEright(node));

     /* use the larger one */
     if (lDepth > rDepth) return(lDepth+1);
     else return(rDepth+1);
  }
}

struct BSTNODE
{
	void *nodeValue;   //Float for this assignment
	BSTNODE *leftNode;
	BSTNODE *rightNode;
	BSTNODE *parentNode;
};

void *getBSTNODE(BSTNODE *node)
{
	void *val = node->nodeValue;
	return val;
}
void setBSTNODE(BSTNODE *node, void *newVal)
{
	node->nodeValue = newVal;
}
BSTNODE *getBSTNODEleft(BSTNODE *node)
{
	BSTNODE *left = node->leftNode;
	return left;
}
void setBSTNODEleft(BSTNODE *parent , BSTNODE *replacement)
{
	parent->leftNode = replacement;
}
BSTNODE *getBSTNODEright(BSTNODE *node)
{
	BSTNODE *right = node->rightNode;
	return right;
}
void setBSTNODEright(BSTNODE *parent, BSTNODE *replacement)
{
	parent->rightNode = replacement;
}
BSTNODE *getBSTNODEparent(BSTNODE *node)
{
	BSTNODE *parent = node->parentNode;;
	return parent;
}
void setBSTNODEparent(BSTNODE *child, BSTNODE *replacement)
{
	child->parentNode = replacement;
}

struct BST
{
	BSTNODE *rootNode;
	int nodeCount;
	void (*display)(FILE *, void *);
	int (*comparator)(void *, void *);
	void (*swapper)(BSTNODE *, BSTNODE *);
};

BST *newBST(void (*d)(FILE *,void *), int (*c)(void *,void *), void (*s)(BSTNODE*, BSTNODE*))
{
	BST *newBST = malloc(sizeof(BST));
	assert(newBST != 0);

	newBST->rootNode = NULL;
	newBST->nodeCount = 0;
	newBST->display = d;
	newBST->comparator = c;

	if(s != NULL) newBST->swapper = s;
	else newBST->swapper = swapBSTNODE;

	return newBST;
}
void setBSTroot(BST *tree, BSTNODE *replacement)
{
	if(tree->rootNode == NULL)
	{
		setBSTNODEleft(replacement, NULL);
		setBSTNODEright(replacement, NULL);
    setBSTNODEparent(replacement, replacement);
		tree->rootNode = replacement;
	}
	else
	{
    setBSTNODEparent(replacement, replacement);
		tree->rootNode = replacement;
	}
}
BSTNODE *getBSTroot(BST *tree)
{
	BSTNODE *root = tree->rootNode;
	return root;
}
BSTNODE *insertBST(BST *tree, void *value)
{
	BSTNODE *newNode = malloc(sizeof(BSTNODE));
	assert(newNode != 0);

  //LEAF INSERT
	setBSTNODE(newNode, value);
	setBSTNODEleft(newNode, NULL);
	setBSTNODEright(newNode, NULL);

	if(getBSTroot(tree) == NULL)
	{
		setBSTroot(tree, newNode);
		tree->nodeCount++;
	}

	else
	{
		BSTNODE* tempNode = getBSTroot(tree);

		while(tempNode != NULL)
		{
			if(tree->comparator(value, getBSTNODE(tempNode)) < 0)
			{
				if(getBSTNODEleft(tempNode) == NULL)
				{
					setBSTNODEleft(tempNode, newNode);
					setBSTNODEparent(newNode, tempNode);
					tempNode = NULL;
					tree->nodeCount++;
				}

				else
				{
					tempNode = getBSTNODEleft(tempNode);
				}
			}

			else if(tree->comparator(value, getBSTNODE(tempNode)) > 0)
			{
				if(getBSTNODEright(tempNode) == NULL)
				{
					setBSTNODEright(tempNode, newNode);
					setBSTNODEparent(newNode, tempNode);
					tempNode = NULL;
					tree->nodeCount++;
				}

				else
				{
					tempNode = getBSTNODEright(tempNode);
				}
			}
      else
      {
        tempNode = NULL;
      }
		}
	}

	return newNode;
}
BSTNODE *findBST(BST *tree, void *value)
{
	BSTNODE *tempNode = getBSTroot(tree);

	while(tempNode != NULL)
	{
		if(tree->comparator(value, getBSTNODE(tempNode)) == 0)
		{
			return tempNode;
		}

		else
		{
			if(tree->comparator(value, getBSTNODE(tempNode)) < 0)
			{
				tempNode = getBSTNODEleft(tempNode);
			}

			else if(tree->comparator(value, getBSTNODE(tempNode)) > 0)
			{
				tempNode = getBSTNODEright(tempNode);
			}
		}
	}
	return NULL;
}

int sizeBST(BST *tree)
{
	int size = tree->nodeCount;
	return size;
}
void displayBST(FILE *fp,BST *tree)
{
	BSTNODE* root = getBSTroot(tree);
	int bstSize = sizeBST(tree);
	int level = 0;

	if(tree->nodeCount == 0)
	{
		fprintf(fp, "EMPTY\n");
		return;
	}

	QUEUE *nodeQueue = newQUEUE(tree->display);
	enqueue(nodeQueue, root);

	while(bstSize > 0)
	{
		int nodeCount = sizeQUEUE(nodeQueue);
    if (nodeCount == 0) break;

		printf("%d: ", level);
		while(nodeCount > 0)
		{

			BSTNODE *currentNode = dequeue(nodeQueue);
      //displayNODESegment(fp, tree, currentNode);
      {
        BSTNODE *parent = getBSTNODEparent(currentNode);

        if(isBSTNODELeaf(currentNode))
        {
          fprintf(fp, "=");
        }

        tree->display(fp, getBSTNODE(currentNode));
        fprintf(fp, "(");
        if(parent != NULL)
        {
          tree->display(fp, getBSTNODE(parent));
        }
        else tree->display(fp, getBSTNODE(currentNode));

        fprintf(fp, ")-");

        if(parent != currentNode)
        {
          if(isBSTNODEleft(currentNode)) fprintf(fp, "l");
          else fprintf(fp, "r");
        }
			}
			bstSize--;

			if(getBSTNODEleft(currentNode) != NULL)
			{
					enqueue(nodeQueue, getBSTNODEleft(currentNode));
			}

			if(getBSTNODEright(currentNode) != NULL)
			{
					enqueue(nodeQueue, getBSTNODEright(currentNode));
			}

			nodeCount--;
      if(nodeCount != 0) fprintf(fp, " ");
		}
		level++;
		printf("\n");
	}
}

BSTNODE *deleteBST(BST *tree, void *value)
{
	BSTNODE *nodeToDelete = findBST(tree, value);
  BSTNODE *prunedNODE = NULL;

	if(nodeToDelete != NULL)
	{
		prunedNODE = swapToLeafBST(tree, nodeToDelete);
		pruneLeafBST(tree, prunedNODE);
	}

	return nodeToDelete;  //Return pruned node
}

BSTNODE *swapToLeafBST(BST *tree, BSTNODE *node)
{
	while(getBSTNODEleft(node) != NULL)
	{
		BSTNODE *predecessor = findPredecessor(tree, node);
		tree->swapper(node, predecessor);
		node = predecessor;
	}

	while(getBSTNODEright(node) != NULL)
	{
		BSTNODE *successor = findSuccessor(tree, node);
		tree->swapper(node, successor);
		node = successor;
	}

  return node;
}
void pruneLeafBST(BST *tree, BSTNODE *node)
{
  if(node != NULL)
  {
  	BSTNODE* parent  = getBSTNODEparent(node);
  	if(isBSTNODEleft(node)) setBSTNODEleft(parent, NULL);
  	if(isBSTNODEright(node)) setBSTNODEright(parent, NULL);
		tree->nodeCount--;

		if(tree->nodeCount == 0) tree->rootNode = NULL;
	}
}
void statisticsBST(FILE *fp, BST *tree)
{
  int min = minDepth(tree, getBSTroot(tree));
  int max = maxDepth(tree, getBSTroot(tree));

	fprintf(fp, "Nodes: %d\n", tree->nodeCount);
	fprintf(fp, "Minimum depth: %d\n", min);
	fprintf(fp, "Maximum depth: %d\n", max);
}
