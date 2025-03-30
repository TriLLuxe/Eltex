#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#define MAX_OPERATIONS 10
typedef struct {
    const char* name;      // Название операции
    OperationFunc func;    // Указатель на функцию
    void* handle;          // Дескриптор библиотеки
} Operation;

int main(int argc, char* argv[]) {
    Operation operations[MAX_OPERATIONS]; // Максимум операций
    int num_operations = 0;   // Количество операций
    DIR* dir;
    struct dirent* entry;
    char lib_dir[] = "./libs"; // Директория с библиотеками
    
    if((dir = opendir(lib_dir)) == NULL) {
        fprintf(stderr, "Не удалось открыть директорию %s\n", lib_dir);
        return 1;
    }
    while ((entry = readdir(dir)) != NULL && num_operations < MAX_OPERATIONS) {
        if(strstr(entry->d_name, ".so") != NULL) { //Проверяем что файл .so
            char lib_path[256];
            snprintf(lib_path, sizeof(lib_path), "%s/%s", lib_dir, entry->d_name);

            void* handle = dlopen(lib_path, RTLD_LAZY);
            if (!handle) {
                fprintf(stderr, "Ошибка загрузки библиотеки %s: %s\n", entry->d_name, dlerror());
                continue;
            }
            //Извлекаем название операции
            char* func_name = strdup(entry->d_name+3);// Пропускаем "lib"
            func_name[strlen(func_name)-3] = '\0'; // Убираем ".so"
            OperationFunc func = (OperationFunc)dlsym(handle, func_name);
            char* error = dlerror();
            if (error){
                fprintf(stderr, "Ошибка извлечения функции %s: %s\n", func_name, error);
                free(func_name);
                dlclose(handle);
                continue;
            }

            char* op_name = NULL;
            if (strcmp(func_name, "add") == 0) op_name = "Сложение";
            else if (strcmp(func_name, "subtract") == 0) op_name = "Вычитание";
            else if (strcmp(func_name, "multiply") == 0) op_name = "Умножение";
            else if (strcmp(func_name, "divide") == 0) op_name = "Деление";
            else {
                fprintf(stderr, "Неизвестная операция %s\n", func_name);
                free(func_name);
                dlclose(handle);
                continue;
            }

            // Добавляем операцию в массив
            operations[num_operations].name = op_name;
            operations[num_operations].func = func;
            operations[num_operations].handle = handle;
            num_operations++;
            free(func_name);

            
        }
    }
      closedir(dir);
      if (num_operations == 0) {
        fprintf(stderr, "Не удалось загрузить ни одну операцию\n");
        return 1;
      }
      
      for (int i = 0; i < num_operations; i++) {
        printf("%d. %s\n", i+1, operations[i].name);
      }

      // Запускаем калькулятор
      double a, b, result;
      int choice;
      while (1) {
          printf("\nКалькулятор\n");
          for (int i = 0; i < num_operations; i++) {
              printf("%d. %s\n", i + 1, operations[i].name);
          }
          printf("%d. Выход\n", num_operations + 1);
          printf("Выберите операцию (1-%d): ", num_operations + 1);
  
          if (scanf("%d", &choice) != 1) {
              printf("Ошибка ввода, попробуйте снова.\n");
              while (getchar() != '\n' && getchar() != EOF);
              continue;
          }
          getchar();
  
          if (choice == num_operations + 1) {
              printf("Выход из программы.\n");
              break;
          }
  
          if (choice < 1 || choice > num_operations + 1) {
              printf("Ошибка: выберите число от 1 до %d\n", num_operations + 1);
              continue;
          }
  
          printf("Введите два числа: ");
          if (scanf("%lf %lf", &a, &b) != 2) {
              printf("Ошибка: введите ровно два числа.\n");
              while (getchar() != '\n' && getchar() != EOF);
              continue;
          }
          getchar();
  
          OperationFunc func = operations[choice - 1].func;
          int status = func(a, b, &result);
  
          if (status == 0) {
              printf("Результат: %lf\n", result);
          } else if (status == -2) {
              printf("Ошибка: деление на ноль\n");
          } else {
              printf("Ошибка выполнения операции\n");
          }
      }
  
      // Закрываем все библиотеки
      for (int i = 0; i < num_operations; i++) {
          dlclose(operations[i].handle);
      }
      return 0;
  }
        
    
