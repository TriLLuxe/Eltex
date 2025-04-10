CC = gcc
CFLAGS = -g -Wall

all: shell sum concat

shell: shell.c source.c
	$(CC) $(CFLAGS) shell.c source.c -o shell

sum: sum.c
	$(CC) $(CFLAGS) sum.c -o sum

concat: concat.c
	$(CC) $(CFLAGS) concat.c -o concat

clean:
	rm -f shell sum concat *.o

