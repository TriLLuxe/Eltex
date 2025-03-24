CC = gcc
TARGET = 2_3

all: $(TARGET)

$(TARGET): main.c source.c
	$(CC) -Wall -g main.c source.c -o $(TARGET)

clean:
	rm -f $(TARGET)