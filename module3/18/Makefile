
CC = gcc


CFLAGS = -Wall -g

all: tcp_server_multiplex tcp_client


tcp_server_multiplex: tcp_server_multiplex.c
	$(CC) $(CFLAGS) -o tcp_server_multiplex tcp_server_multiplex.c


tcp_client: tcp_client.c
	$(CC) $(CFLAGS) -o tcp_client tcp_client.c


clean:
	rm -f tcp_server_multiplex tcp_client