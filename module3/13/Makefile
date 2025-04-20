CC = gcc
TARGET = random_gen_shared

all: $(TARGET)

$(TARGET): main.c 
	$(CC) -Wall -g main.c -o $(TARGET) -lrt

clean:
	rm -f $(TARGET)