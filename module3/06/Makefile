
CC = gcc


CFLAGS = -Wall -g 


PROG1 = chat1
PROG2 = chat2


SRC1 = chat1.c
SRC2 = chat2.c
SRC3 = source.c

all: $(PROG1) $(PROG2)


$(PROG1): $(SRC1)
	$(CC) $(CFLAGS) -o $(PROG1) $(SRC1) $(SRC3)


$(PROG2): $(SRC2)
	$(CC) $(CFLAGS) -o $(PROG2) $(SRC2) $(SRC3)


clean:
	rm -f $(PROG1) $(PROG2)
