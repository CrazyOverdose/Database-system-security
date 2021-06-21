#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <netinet/in.h>

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
    int PORT =  2004;

    // Адрес ip-сокета
    // Содержит три поля: sin_family, sin_port и sin_addr
    // sin_family - всегда содержит AF_INET
    // sin_port - номер порта, который намерен занять процесс
    // sin_addr - IP-адрес
    struct sockaddr_in name;
    name.sin_family = AF_INET;

    //Преобразование числа в схему, которая используется в сети
    name.sin_port = htons(PORT);

    // Сервер принимает соединение с любого IP-адреса
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    int tracking; // Переменная для отслеживания безошибочного выполнения функций
    int inet_socket; // номер сокет-дескриптора
    char buffer[BUFFER_SIZE]; // Буффер

    // Создание inet сокета. 
    // Домен сокета - AF_INET, так как создаем ip-сокет.
    // Тип сокета - SOCK_STREAM. Передача потока данных с предварительной установкой соединения
    // Протокол, используемый для передачи данных, по умолчанию (0).
    inet_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Функция socket() возвращает номер сокет-дескриптора.
    // Если сокет не был создан, функция возвращает -1
    Check(inet_socket, "socket");

    // Назначение адреса, заданного в name, сокету,
    // указываемому дескриптором файла inet_socket
    tracking = connect (inet_socket, (const struct sockaddr *) &name,
                   sizeof(struct sockaddr_in));
    Check(inet_socket, "connect");

    // Массив для данных с консоли от пользователя
    char write_buffer[BUFFER_SIZE];
    
    memset(buffer, 0, sizeof(buffer));

    bool flag = true;
    char login[BUFFER_SIZE]; // логин пользователя

    while (read(inet_socket, buffer, sizeof(buffer)) > 0 ) {

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

        write(inet_socket, write_buffer, strlen(write_buffer));
    }

    exit(EXIT_SUCCESS);
}
