CC = gcc
TARGET = square_areas
CFLAGS = -g -Wall
all: $(TARGET)

$(TARGET): source.c main.c
	$(CC)  $(CFLAGS) source.c main.c -o $(TARGET)

clean:
	rm -f $(TARGET) *.o