#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include "string.h"

struct STRING
{
    char* value;
};

STRING *newSTRING(char* x)
{
    STRING *p = malloc(sizeof(STRING));
    assert(p != 0);
    p->value = x;
    return p;
}

char* getSTRING(STRING *v)
{
    return v->value;
}

char* setSTRING(STRING *v,char* x)
{
    char* old = v->value;
    v->value = x;
    return old;
}

void displaySTRING(FILE *fp, void *v)
{
    fprintf(fp,"%s", getSTRING((STRING*) v));
}

int compareSTRING(void *v1, void *v2)
{
    int diff = strcmp(getSTRING((STRING*) v1) , getSTRING((STRING*) v2));
    if(diff > 0) return 1;
    else if(diff < 0) return -1;
    else return 0;
}

double atofSTRING(STRING* v)
{
   double val = atof(getSTRING(v));
   return val;
}

void freeSTRING(STRING *v)
{
    free(v);
}
