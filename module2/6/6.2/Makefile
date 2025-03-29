
CC = gcc
TARGET = 6_2
LIB = libcontact.so

all: $(TARGET)

$(TARGET): main.o $(LIB)
	$(CC) main.o -L. -lcontact -o $(TARGET)

main.o: main.c header.h
	$(CC) -Wall -g -c main.c -o main.o

$(LIB): source.o
	$(CC) -shared -o $(LIB) source.o

source.o: source.c header.h
	$(CC) -Wall -g -fPIC -c source.c -o source.o

clean:
	rm -f $(TARGET) *.o $(LIB)

rebuild: clean all
