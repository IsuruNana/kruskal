KRUSOBJS = integer.o da.o cda.o queue.o stack.o scanner.o set.o bst.o rbt.o kruskal.o
OOPTS = -Wall -std=c99 -c -g
LOOPTS = -Wall -std=c99 -g

all : kruskal

kruskal : $(KRUSOBJS)
	gcc $(LOOPTS) -o kruskal $(KRUSOBJS)

bst.o : bst.c bst.h
	gcc $(OOPTS) bst.c

rbt.o : rbt.c bst.h rbt.h
	gcc $(OOPTS) rbt.c

integer.o : integer.c integer.h
	gcc $(OOPTS) integer.c

da.o : da.c da.h
	gcc $(OOPTS) da.c

cda.o : cda.c cda.h
	gcc $(OOPTS) cda.c

queue.o : queue.c queue.h cda.h
	gcc $(OOPTS) queue.c

stack.o : stack.c stack.h da.h
	gcc $(OOPTS) stack.c

scanner.o : scanner.c scanner.h
	gcc $(OOPTS) scanner.c

set.o : set.c set.h
	gcc $(OOPTS) set.c

kruskal.o : kruskal.c string.h integer.h queue.h cda.h da.h set.h bst.h rbt.h
	gcc $(OOPTS) kruskal.c

test :	kruskal
	./kruskal graph

clean   :
	rm -f *.o kruskal test-set
