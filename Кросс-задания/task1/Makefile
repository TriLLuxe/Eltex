CC = gcc
CFLAGS = -Wall -g

all: echo_server echo_client

echo_server: echo_server.c source.c
	$(CC) $(CFLAGS) echo_server.c source.c -o echo_server

echo_client: echo_client.c source.c
	$(CC) $(CFLAGS) echo_client.c source.c -o echo_client

clean:
	rm -f echo_server echo_client