
CC = gcc


TARGET = 6_1


LIB = libcontact.a


CFLAGS = -Wall -g

#Цель 
all: $(TARGET)

# Сборка исполняемого файла (Линковка)
$(TARGET): main.o $(LIB)
	$(CC) main.o -L. -lcontact -o $(TARGET)

# Компиляция main.c в объектный файл
main.o: main.c header.h
	$(CC) $(CFLAGS) -c main.c -o main.o

# Создание статической библиотеки
$(LIB): source.o
	ar rcs $(LIB) source.o

# Компиляция source.c в объектный файл
source.o: source.c header.h
	$(CC) $(CFLAGS) -c source.c -o source.o

# Очистка
clean:
	rm -f $(TARGET) *.o $(LIB)
# Цель для пересборки
rebuild: clean all