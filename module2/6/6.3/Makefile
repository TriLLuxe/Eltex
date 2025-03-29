CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -ldl

# Имена файлов
TARGET = 6_3
LIB = libcalc.so
OBJECTS = add.o subtract.o multiply.o divide.o

# Основная цель
all: $(TARGET)

# Сборка основной программы
$(TARGET): main.o $(LIB)
	$(CC) -o $(TARGET) main.o $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# Сборка динамической библиотеки
$(LIB): $(OBJECTS)
	$(CC) -shared -o $(LIB) $(OBJECTS)

# Компиляция отдельных операций
add.o: add.c
	$(CC) $(CFLAGS) -fPIC -c add.c -o add.o

subtract.o: subtract.c
	$(CC) $(CFLAGS) -fPIC -c subtract.c -o subtract.o

multiply.o: multiply.c
	$(CC) $(CFLAGS) -fPIC -c multiply.c -o multiply.o

divide.o: divide.c
	$(CC) $(CFLAGS) -fPIC -c divide.c -o divide.o
# Очистка
clean:
	rm -f *.o $(TARGET) $(LIB)
