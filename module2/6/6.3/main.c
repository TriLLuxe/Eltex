#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

typedef int (*OperationFunc)(double, double, double*);
typedef struct {
    const char* name;      // Название операции
    OperationFunc func;    // Указатель на функцию
} Operation;

int main() {
    void* handle;
    OperationFunc add, subtract, multiply, divide;

    // Загрузка библиотеки
    handle = dlopen("./libcalc.so", RTLD_LAZY);
    if (!handle) {
        printf("Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }
    // Получение адресов функций
    add = (OperationFunc)dlsym(handle, "add");
    subtract = (OperationFunc)dlsym(handle, "subtract");
    multiply = (OperationFunc)dlsym(handle, "multiply");
    divide = (OperationFunc)dlsym(handle, "divide");
    if (!add || !subtract || !multiply || !divide) {
        printf("Ошибка получения адресов функций: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }
    Operation operations[] = {
        {"Сложение", add},
        {"Вычитание", subtract},
        {"Умножение", multiply},
        {"Деление", divide}
    };
    int num_operations = sizeof(operations) / sizeof(operations[0]);
    int choice;
    double a, b, result;

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
    return 0;
}
