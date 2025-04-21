
CC = gcc


CFLAGS = -Wall -g

all: tcp_server tcp_client


tcp_server: tcp_server.c
	$(CC) $(CFLAGS) -o tcp_server tcp_server.c


tcp_client: tcp_client.c
	$(CC) $(CFLAGS) -o tcp_client tcp_client.c


clean:
	rm -f tcp_server tcp_client