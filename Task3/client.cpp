#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <algorithm>
#include <string>
#include <unistd.h>

// Отслеживание ошибочного выполнения различных функций
// Возвращенное значение -1 от многих функций означает
// их ошибочное выполнение
void Check(int var, const char* name){
    if (var == -1) {
        fprintf(stderr, "%s", name);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    int BUFFER_SIZE = 3000;
    char SOCKET_NAME[] =  "./socket";

    // Адрес unix-сокета
    // Содержит два поля: sun_family и  sun_path.
    // sun_family - всегда содержит AF_UNIX
    // sun_path - имя пути
    struct sockaddr_un name;
    name.sun_family = AF_UNIX;
    // Копирование первых sizeof(name.sun_path) - 1) символов из
    // переменной SOCKET_NAME в имя пути sun_path
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    int tracking; // Переменная для отслеживания безошибочного выполнения функций
    int unix_socket; // номер сокет-дескриптора
    char buffer[BUFFER_SIZE]; // Буффер

    // Создание unix сокета. 
    // Домен сокета - AF_UNIX, так как создаем unix-сокет.
    // Тип сокета - SOCK_STREAM. Передача потока данных с предварительной установкой соединения
    // Протокол, используемый для передачи данных, по умолчанию (0).
    unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    // Функция socket() возвращает номер сокет-дескриптора.
    // Если сокет не был создан, функция возвращает -1
    Check(unix_socket, "socket");

    // Назначение адреса, заданного в name, сокету,
    // указываемому дескриптором файла unix_socket
    tracking = connect (unix_socket, (const struct sockaddr *) &name,
                   sizeof(struct sockaddr_un));
    Check(unix_socket, "connect");

    // Массив для данных с консоли от пользователя
    char write_buffer[BUFFER_SIZE];

    memset(buffer, 0, sizeof(buffer));

    bool flag = true;
    char login[BUFFER_SIZE]; // логин пользователя

    while (read(unix_socket, buffer, sizeof(buffer)) > 0 ) {

        fprintf(stdout, "%s", buffer);

        memset(buffer, 0, sizeof(buffer));
        memset(buffer, 0, sizeof(write_buffer));

        // Считывание с консоли
        fscanf(stdin, "%s", write_buffer);

        if(flag){
            memcpy(login, write_buffer, strlen(write_buffer));
            flag = false;
        }

        fprintf(stdout, "%s -> Service : %s\n", login, write_buffer);

        write(unix_socket, write_buffer, strlen(write_buffer));
    }

    close(unix_socket);
    exit(EXIT_SUCCESS);
}
