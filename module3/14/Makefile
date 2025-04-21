
CC = gcc


CFLAGS = -Wall -g

all: udp_server udp_client


udp_server: udp_server.c
	$(CC) $(CFLAGS) -o udp_server udp_server.c


udp_client: udp_client.c
	$(CC) $(CFLAGS) -o udp_client udp_client.c


clean:
	rm -f udp_server udp_client