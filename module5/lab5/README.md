# Модуль ядра Linux: Netlink

 Проект реализует модуль ядра Linux, который обменивается данными с пользовательским пространством через Netlink. Пользовательская программа отправляет сообщение в ядро, а ядро отвечает строкой "Hello from kernel space!".

## Состав проекта
- `netlink_kern.c` — код модуля ядра.
- `main_user.c` — пользовательская программа для отправки и получения сообщений.
- `Makefile` — для компиляции модуля.

## Требования
- дистрибутив Linux (протестировано на ядре 6.8.0-60-generic).
- Установленные заголовки ядра:
  ```bash
  sudo apt install linux-headers-$(uname -r)
  ```
- GCC для компиляции программы:
  ```bash
  sudo apt install build-essential
  ```

## Сборка
1. Скомпилируйте модуль ядра:
   ```bash
   make
   ```
2. Скомпилируйте пользовательскую программу:
   ```bash
   gcc -o main_user main_user.c
   ```

## Использование
1. Загрузите модуль ядра:
   ```bash
   sudo insmod netlink_kern.ko
   ```
   Проверьте, что модуль загружен:
   ```bash
   lsmod | grep "netlink_kern"
   ```
2. Запустите программу, указав сообщение (или без аргументов для стандартного "Hello from user"):
   ```bash
   ./main_user "Custom message"
   ```
   Пример вывода:
   ```
   Sending message to kernel: HELLO KERNEL
   Waiting for message from kernel
   Received message payload: Hello from kernel space!
   ```
3. Проверьте логи ядра:
   ```bash
   dmesg | tail -5
   ```
   Пример вывода:
   ```
   Netlink received msg payload from process <PID>: Custom message
   ```
4. Выгрузите модуль:
   ```bash
   sudo rmmod netlink_kern
   ```

## Очистка
Удалите скомпилированные файлы:
```bash
make clean
```
