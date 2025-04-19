CC = gcc
TARGET = random_gen_pipe

all: $(TARGET)

$(TARGET): main.c semaphore.c
	$(CC) -Wall -g main.c semaphore.c -o $(TARGET)

clean:
	rm -f $(TARGET)