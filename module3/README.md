## Задание 01 (Процессы, 2 балла).
Написать программу, вычисляющую площади квадратов с заданной длиной стороны.
Длины сторон передаются как аргументы запуска.
Расчеты делают родительский и дочерний процессы, разделяя задачи примерно поровну.

## Задание 02 (Процессы, 2 балла).
Написать программу, похожую на командный интерпретатор.
При запуске выводится приглашение, и пользователь вводит имя и аргументы программы, которую желает запустить.
Программы для запуска написать самостоятельно и поместить в тот же каталог (например, программа для вычисления суммы аргументов, «склеивания» строк, поиска наибольшего значения или наибольшей длины строки и т.д.).

## Задание 03 (Файлы, 3 балла).
Скорректировать решение задачи о книге контактов из модуля 2 так, чтобы список контактов хранился в файле.
Использовать небуферизованный ввод-вывод.
При запуске программы список считывается из файла, при завершении программы список записывается в файл.
Учесть, что при запуске программы может не быть сохраненных данных.

## Задание 04 (Каналы, 2 балла).
Написать программу, порождающую дочерний процесс и использующую однонаправленный обмен данными.
Процесс-потомок генерирует случайные числа и отправляет родителю.
Родитель выводит числа на экран и в файл.
Количество чисел задается в параметрах запуска приложения.

## Задание 05 (Сигналы, 3 балла).
Изменить программу 04 так, чтобы дочерний процесс выводил информацию из файла.
Если родительский процесс собирается изменить файл, то он отправляет сигнал SIGUSR1 (блокировка доступа к файлу).
Когда родительский процесс завершил модификацию файла, он отправляет сигнал SIGUSR2 (разрешение доступа к файлу).
Дочерний процесс отправляет новое число родительскому после того, как прочитал файл.

## Задание 06 (Очереди сообщений System V, 3 балла).
Написать две программы, использующие одну очередь сообщений для двухстороннего взаимодействия (персональный чат).
Сообщения отправляются по очереди (пинг-понг).
Для завершения обмена предусмотреть отправку данных с заранее известным приоритетом.

## Задание 07 (Очереди сообщений POSIX, 3 балла).
Написать две программы, использующие одну очередь сообщений для двухстороннего взаимодействия (персональный чат)
Сообщения отправляются по очереди (пинг-понг).
Для завершения обмена предусмотреть отправку данных с заранее известным приоритетом.

## Задание 08 (Семафоры System V, 2 балла)
Скорректировать программу 05 так, чтобы доступ к файлу регулировался семафором.

## Задание 09 (Семафоры System V, 2 балла)
Скорректировать программу 08 так, чтобы несколько процессов (ограниченное количество) могли читать из файла. Запись в файл возможна, когда он никем не читается.

## Задание 10 (Семафоры POSIX, 2 балла)
Скорректировать программу 05 так, чтобы доступ к файлу регулировался семафором.

## Задание 11 (Семафоры POSIX, 2 балла)
Скорректировать программу 10 так, чтобы несколько процессов (ограниченное количество) могли читать из файла. Запись в файл возможна, когда он никем не читается.

## Задание 12 (Разделяемая память System V, 3 балла)
Родительский процесс генерирует наборы из случайного количества случайных чисел и помещает в разделяемую память.
Дочерний процесс находит максимальное и минимальное число и также помещает их в разделяемую память, после чего родительский процесс выводит найденные значения на экран.
Процесс повторяется до получения сигнала SIGINT, после чего выводится количество обработанных наборов данных.

## Задание 13 (Разделяемая память POSIX, 3 балла)
Родительский процесс генерирует наборы из случайного количества случайных чисел и помещает в разделяемую память.
Дочерний процесс находит максимальное и минимальное число и также помещает их в разделяемую память, после чего родительский процесс выводит найденные значения на экран.
Процесс повторяется до получения сигнала SIGINT, после чего выводится количество обработанных наборов данных.

## Задание 14 (UDP сокеты, 2 балла)
Реализовать чат для двух клиентов с использованием UDP-сокетов.

## Задание 15 (TCP сокеты, 2 балла)
Отладить программы ТСP-клиента и сервера из лекции.
Добавить возможность вычисления разности, произведения, частного двух чисел.
Необходимое математическое действие указывает клиент.

## Задание 16 (TCP сокеты, 3 балла)
Добавить в программу из Задания 15 возможность передачи файлов.

## Задание 17 (RAW сокеты, 3 балла)
Написать программу, получающую копии пакетов, предназначенных приложению-серверу из задачи 14 (снифер).
Программа может формировать дамп данных, который нужно вручную проанализировать (просмотр бинарных файлов) и убедиться, что это действительно информация от программы-клиента.
Можно дополнить программу так, чтобы она расшифровывала и выводила в файл или на экран полученные сообщения.

## Задание 18 (Мультиплексирование, 3 балла)
Реализовать программу-сервер, написанную при решении задач 15 или 16, с использованием мультиплексирования ввода-вывода и одного потока исполнения.
Можно применить любую функцию (select/poll/epoll).
