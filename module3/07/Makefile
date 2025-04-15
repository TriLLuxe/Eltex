
CC = gcc


CFLAGS = -Wall -g 


PROG1 = chat1_posix
PROG2 = chat2_posix


SRC1 = chat1_posix.c
SRC2 = chat2_posix.c
SRC3 = source.c

all: $(PROG1) $(PROG2)


$(PROG1): $(SRC1)
	$(CC) $(CFLAGS) -o $(PROG1) $(SRC1) $(SRC3) -lrt


$(PROG2): $(SRC2)
	$(CC) $(CFLAGS) -o $(PROG2) $(SRC2) $(SRC3) -lrt


clean:
	rm -f $(PROG1) $(PROG2)
