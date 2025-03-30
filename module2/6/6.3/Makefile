# Makefile
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -ldl
TARGET = 6_3
LIB_DIR = libs
LIB_ADD = $(LIB_DIR)/libadd.so
LIB_SUB = $(LIB_DIR)/libsubtract.so
LIB_MUL = $(LIB_DIR)/libmultiply.so
LIB_DIV = $(LIB_DIR)/libdivide.so

all: $(TARGET) $(LIB_ADD) $(LIB_SUB) $(LIB_MUL) $(LIB_DIV)

# Сборка основной программы
$(TARGET): main.o
	$(CC) main.o $(LDFLAGS) -o $(TARGET)

main.o: main.c header.h
	$(CC) $(CFLAGS) -c main.c -o main.o

# Создание директории libs 
$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Сборка динамических библиотек с зависимостью от $(LIB_DIR)
$(LIB_ADD): add.o | $(LIB_DIR)
	$(CC) -shared -o $(LIB_ADD) add.o

$(LIB_SUB): subtract.o | $(LIB_DIR)
	$(CC) -shared -o $(LIB_SUB) subtract.o

$(LIB_MUL): multiply.o | $(LIB_DIR)
	$(CC) -shared -o $(LIB_MUL) multiply.o

$(LIB_DIV): divide.o | $(LIB_DIR)
	$(CC) -shared -o $(LIB_DIV) divide.o

# Компиляция объектных файлов для библиотек
add.o: add.c header.h
	$(CC) $(CFLAGS) -fPIC -c add.c -o add.o

subtract.o: subtract.c header.h
	$(CC) $(CFLAGS) -fPIC -c subtract.c -o subtract.o

multiply.o: multiply.c header.h
	$(CC) $(CFLAGS) -fPIC -c multiply.c -o multiply.o

divide.o: divide.c header.h
	$(CC) $(CFLAGS) -fPIC -c divide.c -o divide.o

# Очистка
clean:
	rm -f $(TARGET) *.o $(LIB_DIR)/*.so
	rmdir $(LIB_DIR)

rebuild: clean all
