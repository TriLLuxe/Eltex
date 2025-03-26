CC = gcc
TARGET = 3_1

all: $(TARGET)

$(TARGET): main.c source.c header.h
	$(CC) -Wall -g main.c source.c -o $(TARGET)

clean:
	rm -f $(TARGET)