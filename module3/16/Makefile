CC = gcc
CFLAGS = -Wall -g


SERVER_OBJS = tcp_server.o math_operations.o file_operations.o network_utils.o


CLIENT_OBJS = tcp_client.o network_utils.o file_operations_client.o


HEADERS = math_operations.h file_operations.h network_utils.h

all: tcp_server tcp_client clean_objs


tcp_server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o tcp_server $(SERVER_OBJS)


tcp_client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o tcp_client $(CLIENT_OBJS)


tcp_server.o: tcp_server.c $(HEADERS)
	$(CC) $(CFLAGS) -c tcp_server.c

tcp_client.o: tcp_client.c network_utils.h
	$(CC) $(CFLAGS) -c tcp_client.c

math_operations.o: math_operations.c math_operations.h
	$(CC) $(CFLAGS) -c math_operations.c

file_operations.o: file_operations.c file_operations.h
	$(CC) $(CFLAGS) -c file_operations.c

network_utils.o: network_utils.c network_utils.h
	$(CC) $(CFLAGS) -c network_utils.c
file_operations_client.o: file_operations_client.c file_operations_client.h network_utils.h
	$(CC) $(CFLAGS) -c file_operations_client.c

clean_objs:
	rm -f *.o
clean:
	rm -f tcp_server tcp_client *.o