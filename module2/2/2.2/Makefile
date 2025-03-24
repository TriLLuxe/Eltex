CC = gcc
TARGET = 2_2

all: $(TARGET)

$(TARGET): main.c source.c header.h
	$(CC) -Wall -g main.c source.c -o $(TARGET)

clean:
	rm -f $(TARGET)