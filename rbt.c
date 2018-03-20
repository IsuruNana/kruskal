#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bst.h"
#include "rbt.h"
#include "stack.h"

//--------------------------------PRIVATE PROTOTOTYPES----------------------//

typedef struct RBTNODE RBTNODE;

struct RBTNODE
{
  int frequency;
  void *value;
  char color;
  void (*display)(FILE *, void *);
  int (*comparator)(void *, void *); //Knows how to compare the value it stores
};

struct RBT
{
  BST* rbtBST;
  int words;
  void (*display)(FILE *, void *);
  int (*comparator)(void *, void *);
};

static RBTNODE *findRBTNODE(RBT*, void*);
static void *getRBTNODEvalue(BSTNODE*);
static void displayRBTNODE(FILE *, void *); //Given to BST
static int compareRBTNODE(void *, void *); //Given to BST
static void swapRBTNODE(BSTNODE *, BSTNODE *); //Given to BST
static char getRBTcolor(BSTNODE*);
static void setRBTcolor(BSTNODE*, char);
static void insertionFixUp(RBT*, BSTNODE*);
static void deletionFixUp(RBT*, BSTNODE*);
static int isBSTNODEleft(BSTNODE*);
static int isBSTNODEright(BSTNODE*);
static int isBSTNODElinear(BSTNODE*, BSTNODE*);
static void rotate(RBT*, BSTNODE*);
static BSTNODE* getBSTNODEuncle(BSTNODE*);
static BSTNODE* getBSTNODEnephew(BSTNODE*);
static BSTNODE* getBSTNODEniece(BSTNODE*);
static BSTNODE* getBSTNODEsibling(BSTNODE*);

//static void displayNodeData(char* name, RBT *rbt, BSTNODE *node);
//static void inorder(FILE *fp, BSTNODE* node);

//**************************************************************************//

//----------------BEGINNING OF PRIVATE METHOD DEFINITIONS----------//

RBTNODE *findRBTNODE(RBT *tree, void *value)
{
  RBTNODE *rbtNode = malloc(sizeof(RBTNODE));
  RBTNODE *returnNode = NULL;

  rbtNode->value = value;   //Need to wrap searching value in RBTNODE
  rbtNode->display = tree->display;
  rbtNode->comparator = tree->comparator;

  BSTNODE *bstNODE = findBST(tree->rbtBST, rbtNode);

  if(bstNODE != NULL)
  {
    returnNode = getBSTNODE(bstNODE);
    free(rbtNode);
    return returnNode;
  }

  return NULL;
}

static void *getRBTNODEvalue(BSTNODE* b)
{
  RBTNODE *node = getBSTNODE(b);
  void *value = node->value;
  return value;
}

void displayRBTNODE(FILE *fp, void *value)   //GOOD
{
  RBTNODE *rbtNode = value;
  rbtNode->display(fp, rbtNode->value);
  if(rbtNode->frequency > 1) fprintf(fp, "-%d", rbtNode->frequency);
  fprintf(fp, "-%c", rbtNode->color);
}
int compareRBTNODE(void *node1, void *node2) //GOOD
{
  RBTNODE *nodeA = node1;
  RBTNODE *nodeB = node2;
  int compare = nodeB->comparator(nodeA->value, nodeB->value);
  return compare;
}
void swapRBTNODE(BSTNODE *node1, BSTNODE *node2) //GOOD
{
  RBTNODE *ra = getBSTNODE(node1);
  RBTNODE *rb = getBSTNODE(node2);

  /* swap the keys stored in the RBT value objects */
  void *vtemp = ra->value;
  ra->value = rb->value;
  rb->value = vtemp;

  /* swap the counts stored in the RBT value objects */
  int ftemp = ra->frequency;
  ra->frequency = rb->frequency;
  rb->frequency = ftemp;

  /* note: colors are NOT swapped */
}
char getRBTcolor(BSTNODE* node)
{
  if(node == NULL) return 'B';

  RBTNODE *rbtNode = getBSTNODE(node);
  char color = rbtNode->color;
  return color;
}
void setRBTcolor(BSTNODE* node, char newColor)
{
  if(node == NULL) return;

  RBTNODE *rbtNode = getBSTNODE(node);
  rbtNode->color = newColor;
}
void insertionFixUp(RBT *rbt, BSTNODE* currentNode)
{
  BSTNODE* parent = NULL;
  BSTNODE* uncle = NULL;
  BSTNODE* grandParent = NULL;

  while(1)
  {
    if (getBSTroot(rbt->rbtBST) == currentNode) break; // x is root => exit

    parent = getBSTNODEparent(currentNode); //Update parent

    if (getRBTcolor(parent) == 'B') break; //parent is black => exit

    grandParent = getBSTNODEparent(parent);
    uncle = getBSTNODEuncle(currentNode);

    if (getRBTcolor(uncle) == 'R')   //uncle is red
    {
      setRBTcolor(parent, 'B');      //color parent black
      setRBTcolor(uncle, 'B');       //color uncle black
      setRBTcolor(grandParent, 'R'); //color grandparent red
      currentNode = grandParent;     // x = grandparent
    }
    else //if (getRBTcolor(uncle) == 'b')
    {
      // uncle must be black
      // x and parent are not linear
      if (!isBSTNODElinear(currentNode, parent))
      {
        //printf("Not linear\n");
        BSTNODE *oldParent = parent;
        BSTNODE *oldX = currentNode;

        rotate(rbt, currentNode);

        currentNode = oldParent;  //x = old parent
        parent = oldX;            //parent = old x
      }

      //else{
      setRBTcolor(parent, 'B'); //color parent black
      setRBTcolor(grandParent, 'R'); //color grandparent red
      rotate(rbt, parent);  //rotate parent to grandparent
      break; //exit the loop

    }
  }

  //color root black
  setRBTcolor(getBSTroot(rbt->rbtBST), 'B');
}

void deletionFixUp(RBT* rbt, BSTNODE* x)
{
  BSTNODE *sibling = NULL, *parent = NULL, *niece = NULL, *nephew = NULL;

  while(1)
  {
    if (x == getBSTroot(rbt->rbtBST)) break; //exit the loop
    if (getRBTcolor(x) == 'R') break; //exit the loop

    sibling = getBSTNODEsibling(x);
    parent = getBSTNODEparent(x);
    niece = getBSTNODEniece(x);
    nephew = getBSTNODEnephew(x);

    //displayNodeData("x", rbt, x);
    //displayNodeData("sibling", rbt, sibling);
    //displayNodeData("parent", rbt, parent);
    //displayNodeData("niece", rbt, niece);
    //displayNodeData("nephew", rbt, nephew);

    if (getRBTcolor(sibling) == 'R')    //sibling is red
    {
        //displayNodeData("Sibling case", rbt, sibling);
        //printf("\n\n");
        setRBTcolor(parent, 'R');  //color parent red
        setRBTcolor(sibling, 'B'); //color sibling black
        rotate(rbt, sibling);      //rotate sibling to parent
        // should have black sibling now
    }
    else if (getRBTcolor(nephew) == 'R') //Works
    {
        //displayNodeData("Nephew case", rbt, nephew);
        //printf("\n\n");
        setRBTcolor(sibling, getRBTcolor(parent));              //color sibling the same as parent
        setRBTcolor(parent, 'B');                               //color parent black
        setRBTcolor(nephew, 'B');                               //color nephew black
        rotate(rbt, sibling);                                   //rotate sibling to parent
                                                                //subtree and tree is BH balanced
        break;   //exit the loop
    }
    else if (getRBTcolor(niece) == 'R') //Works
    {
        // nephew must be black
        //displayNodeData("Niece case", rbt, niece);
        //printf("\n\n");
        setRBTcolor(niece, 'B');      //color niece black
        setRBTcolor(sibling, 'R');    //color sibling red
        rotate(rbt, niece);           //rotate niece to sibling
        // should have red nephew now
    }
    else // sibling, niece, and nephew must be black
    {
        //printf("All black\n\n\n");
        setRBTcolor(sibling, 'R'); //color sibling red
        x = parent; //x = parent
        // this subtree is BH balanced, but tree is not
    }
  }

  setRBTcolor(x, 'B');
}

BSTNODE* getBSTNODEuncle(BSTNODE* node)
{
  BSTNODE *parent = getBSTNODEparent(node);

  if(parent == NULL) return NULL;

  BSTNODE *grandParent = getBSTNODEparent(parent);

  if(grandParent == NULL) return NULL;

  BSTNODE *left = getBSTNODEleft(grandParent);
  BSTNODE *right = getBSTNODEright(grandParent);

  if(left == parent) return right;
  else if(right == parent) return left;

  return NULL;
}

BSTNODE* getBSTNODEsibling(BSTNODE* node)
{
  BSTNODE *parent = getBSTNODEparent(node);

  if(parent == node) return NULL;

  BSTNODE *left = getBSTNODEleft(parent);
  BSTNODE *right = getBSTNODEright(parent);

  if(left == node) return right;
  else if(right == node) return left;

  return NULL;
}

BSTNODE* getBSTNODEniece(BSTNODE* node)
{
  BSTNODE *sibling = getBSTNODEsibling(node);
  BSTNODE *niece = NULL;

  if(isBSTNODEleft(node)) niece = getBSTNODEleft(sibling);
  else niece = getBSTNODEright(sibling);

  return niece;
}

BSTNODE* getBSTNODEnephew(BSTNODE* node)
{
  BSTNODE *sibling = getBSTNODEsibling(node);
  BSTNODE *nephew = NULL;

  if(isBSTNODEleft(node)) nephew = getBSTNODEright(sibling);
  else nephew = getBSTNODEleft(sibling);

  return nephew;
}

int isBSTNODEleft(BSTNODE* node)
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
int isBSTNODEright(BSTNODE* node)
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
int isBSTNODElinear(BSTNODE* node1, BSTNODE* node2)
{
  if(isBSTNODEleft(node1) && isBSTNODEleft(node2)) return 1;
  else if(isBSTNODEright(node1) && isBSTNODEright(node2)) return 1;
  return 0;
}

void rotate(RBT *rbt, BSTNODE *x)
{
  BSTNODE *parent = getBSTNODEparent(x);
  BSTNODE *grandParent = getBSTNODEparent(parent);
  BSTNODE *left = getBSTNODEleft(x);
  BSTNODE *right = getBSTNODEright(x);

  //rotating left
  if(x == getBSTNODEright(parent))
  {
    //printf("Rotating left\n");
    setBSTNODEleft(x, parent);
    setBSTNODEparent(parent, x);
    setBSTNODEright(parent,left);

    if(left != NULL)
      setBSTNODEparent(left, parent);

    if(getBSTroot(rbt->rbtBST) == parent)
    {
      setBSTNODEparent(x, x);
      setBSTroot(rbt->rbtBST, x);
    }

    else
    {
      if(parent == getBSTNODEleft(grandParent))
      {
        setBSTNODEleft(grandParent, x);
      }
      else
      {
        setBSTNODEright(grandParent, x);
      }
      setBSTNODEparent(x, grandParent);
    }
    //printf("Done rotating left");
  }

  //rotating right
  else //if(x == getBSTNODEleft(parent))
  {
    //printf("Rotating right\n");
    setBSTNODEright(x, parent);
    setBSTNODEparent(parent, x);
    setBSTNODEleft(parent,right);

    if(right != NULL)
      setBSTNODEparent(right, parent);

    if(getBSTroot(rbt->rbtBST) == parent)
    {
      setBSTNODEparent(x, x);
      setBSTroot(rbt->rbtBST, x);
    }

    else
    {
      if(parent == getBSTNODEright(grandParent))
      {
        setBSTNODEright(grandParent, x);
      }
      else
      {
        setBSTNODEleft(grandParent, x);
      }
      setBSTNODEparent(x, grandParent);
    }
    //printf("Done rotating right");
  }

}

//------------------------------PUBLIC RBT FUNCTIONS------------------------//


RBT *newRBT(void (*display)(FILE *,void *), int (*comparator)(void *,void *))
{
  RBT *newRBT = malloc(sizeof(RBT));

  newRBT->display = display;
  newRBT->comparator = comparator;
  newRBT->rbtBST = newBST(displayRBTNODE, compareRBTNODE, swapRBTNODE);
  newRBT->words = 0;

  return newRBT;
}
void insertRBT(RBT *rbt, void *value)
{
  RBTNODE *rbtWrapperNode = findRBTNODE(rbt, value);

  if(rbtWrapperNode != NULL) rbtWrapperNode->frequency++;

  else
  {
    RBTNODE *dataNode = malloc(sizeof(RBTNODE));

    dataNode->value = value;
    dataNode->frequency = 1;
    dataNode->color = 'R';
    dataNode->comparator = rbt->comparator;
    dataNode->display = rbt->display;

    //BSTNODE *currentNode = insertBST(rbt->rbtBST, dataNode);
    insertionFixUp(rbt, insertBST(rbt->rbtBST, dataNode));
  }

  rbt->words++;
}
int findRBT(RBT *rbt, void *value)
{
  RBTNODE *rbtNode = findRBTNODE(rbt, value);
  int freq = 0;

  if(rbtNode != NULL) freq = rbtNode->frequency;

  return freq;
}
void deleteRBT(RBT *rbt, void *value)
{
  RBTNODE *rbtNode = malloc(sizeof(RBTNODE));
  rbtNode->value = value;
  rbtNode->display = rbt->display;
  rbtNode->comparator = rbt->comparator;

  BSTNODE *bstNode = findBST(rbt->rbtBST, rbtNode);
  BSTNODE *prunedNode = NULL;

  if(bstNode != NULL)  //value exists
  {
    free(rbtNode);
    rbtNode = getBSTNODE(bstNode);

    if(rbtNode->frequency == 1)
    {
      prunedNode = swapToLeafBST(rbt->rbtBST, bstNode);
      deletionFixUp(rbt, prunedNode);
      pruneLeafBST(rbt->rbtBST, prunedNode);
    }

    else rbtNode->frequency--;

    rbt->words--;
  }

  else  //Not found
  {
    printf("Value ");
    rbt->display(stdout, value);
    printf(" not found.\n");
  }
}
int sizeRBT(RBT *rbt)
{
  int size = sizeBST(rbt->rbtBST);
  return size;
}
int wordsRBT(RBT *rbt)
{
  int words = rbt->words;
  return words;
}
void statisticsRBT(FILE *fp, RBT *rbt)
{
  fprintf(fp, "Words/Phrases: %d\n", wordsRBT(rbt));
  statisticsBST(fp, rbt->rbtBST);
}
void displayRBT(FILE *fp, RBT *rbt)
{
  displayBST(fp, rbt->rbtBST);
}

//------------------------NEW FUNCTIONS-----------------//

/*void inorder(FILE *fp, BSTNODE* node)
{
	if (node == NULL) return;
	// first recur on left child
	inorder(fp, getBSTNODEleft(node));

	// then insert the data of node
	//insertDA(sortedEdges, getBSTNODE(node));
	displayRBTNODE(fp, getBSTNODE(node));
  fprintf(fp, "\n");

	// now recur on right child
	inorder(fp , getBSTNODEright(node));
}*/

/*void displayRBTinorder(FILE *fp, RBT *rbt)
{
  BSTNODE *root = getBSTroot(rbt->rbtBST);
  inorder(fp, root);
}*/

void **getInorderArray(RBT *rbt)
{
  BSTNODE *current = getBSTroot(rbt->rbtBST);
  STACK *stack = newSTACK(rbt->display); /* Initialize stack s */
  int done = 0;
  int i = 0;

  void **returnArray = malloc(sizeof(void*) * sizeBST(rbt->rbtBST));

  while (!done)
  {
    /* Reach the left most tNode of the current tNode */
    if(current !=  NULL)
    {
      /* place pointer to a tree node on the stack before traversing
        the node's left subtree */
      push(stack, current);
      current = getBSTNODEleft(current);
    }
    /* backtrack from the empty subtree and visit the tNode
       at the top of the stack; however, if the stack is empty,
      you are done */
    else
    {
      if (!isEmptySTACK(stack))
      {
        current = pop(stack);
        returnArray[i] = getRBTNODEvalue(current);
        i++;
        //displayRBTNODE(stdout, getBSTNODE(current));
        //printf("\n");

        /* we have visited the node and its left subtree.
          Now, it's right subtree's turn */
        current = getBSTNODEright(current);
      }
      else
        done = 1;
    }
  }

  return returnArray;
}
//--------------------------------END----------------------//


/*static void displayNodeData(char* name, RBT *rbt, BSTNODE *node)
{
  printf("%s is ", name);
  if(node != NULL)
  rbt->display(stdout, getRBTNODEvalue(node));
  else printf("NULL");
  printf(" and its color is %c\n", getRBTcolor(node));
}*/
