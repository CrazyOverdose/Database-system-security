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
        perror(name);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    
    char SOCKET_NAME[] =  "./socket";
    int BUFFER_SIZE = 3000;

    // Адрес unix-сокета
    // Содержит два поля: sun_family и  sun_path.
    // sun_family - всегда содержит AF_UNIX
    // sun_path - имя пути
    struct sockaddr_un name;
    name.sun_family = AF_UNIX;
    // Копирование первых sizeof(name.sun_path) - 1) символов из
    // переменной SOCKET_NAME в имя пути sun_path
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    // Флаг для отслеживания выбора пользователя
    // true - работа сервиса
    // false - закрытие сервиса
    bool flag = true;
    int tracking; // Переменная для отслеживания безошибочного выполнения функций
    int unix_socket; // номер сокет-дескриптора
    int new_unix_socket; // файловый дескриптор на подключенный сокет
    char buffer[BUFFER_SIZE]; // Буффер
    char login[BUFFER_SIZE]; // login пользователя

    // Удалить старый сокет
    unlink(SOCKET_NAME);

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
    tracking = bind(unix_socket, (const struct sockaddr *) &name, sizeof(struct sockaddr_un));
    //В случае ошибки bind() возвращает -1
    Check(tracking, "bind");

    fprintf(stdout, "Подключен к %s\n", SOCKET_NAME);

    // Информирование ОС, что сервер ожидает запросы связи на сокете unix_socket
    // Второй аргумент определяет, как много запросов связи может быть принято
    // на сокет одновременно
    tracking = listen(unix_socket, 5);

    //В случае ошибки listen() возвращает -1
    Check(tracking, "listen");

    while (true){
    
        // Извлечение первого запроса на соединение из очереди
        // ожидающих соединений прослушивающего сокета unix_socket,
        // создает новый подключенный сокет и возвращает новый дескриптор,
        // указывающий на новый сокет
        new_unix_socket = accept(unix_socket, NULL, NULL);

        //В случае ошибки accept() возвращает -1
        Check(new_unix_socket, "accept");

        // Очистка буфферов
        memset(buffer, 0, sizeof(buffer));
        memset(login, 0, sizeof(login));

        fprintf(stdout,"Есть новое подключение\n");

        // Запись в буфер
        sprintf(buffer, "Service -> unknown: Здравствуйте, как можно к вам обращаться?\n");

        fprintf(stdout,"Service -> unknown: Здравствуйте, как можно к вам обращаться?\n");

        // Запись из буфера buffer в файл, на который ссылается 
        // файловый описатель new_unix_socket
        write(new_unix_socket, buffer, strlen(buffer));

        // Запись strlen(buffer) байтов файлового описателя new_unix_socket
        // в буфер, адрес которого начинается с login
        read(new_unix_socket, login, BUFFER_SIZE);

        fprintf(stdout, "%s -> Service : %s \n", login, login);

        memset(buffer, 0, sizeof(buffer));

        // Правила сервиса
        char rules[] = "\n1. Сервис выдает количество букв в слове. При вводе нескольких слов, подсчитает для каждого. \n2. Закончить работу пользователю \"logout\" \n3. Закончить работу с сервисом - \"end\"" ;
         
        sprintf(buffer, "Service -> %s : %s \nВы были авторизованы как %s. \nВведите слово\n", login, rules, login);
        
        fprintf(stdout, "Service -> %s : %s \nВы были авторизованы как %s. \nВведите слово\n", login, rules, login);

        write(new_unix_socket, buffer, strlen(buffer));

        // Очистка буффера
        memset(buffer, 0, sizeof(buffer));

        while (flag)
        {
            
            read(new_unix_socket, buffer, BUFFER_SIZE);

            fprintf(stdout, "%s - > Service : %s\n", login, buffer);

            if(strcmp(buffer, "logout") == 0){

                fprintf(stdout, "Service - > %s : Подключение пользователя %s остановлено\n", login, login);
                memset(buffer, 0, sizeof(buffer));
                close(new_unix_socket);
                break;
            }

            else if(strcmp(buffer, "end") == 0){
                flag = false;
                fprintf(stdout, "Service - > %s : Закрытие сервиса\n", login);
                memset(buffer, 0, sizeof(buffer));
                close(new_unix_socket);
                break;
            }
            
            // Промежуточный массив для введенного слова
            // Переменная для количества 
            int count = strlen(buffer);
            char interim[BUFFER_SIZE];
            memcpy(interim, buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));

            fprintf(stdout, "Service -> %s : \nВ слове %s %d символов.\nВведите новое слово\n", login, interim, count);

            sprintf(buffer, "Service -> %s : \nВ слове %s %d символов.\nВведите новое слово\n", login, interim, count);

            write(new_unix_socket, buffer, strlen(buffer));

            memset(buffer, 0, sizeof(buffer));
            memset(interim, 0, sizeof(interim));           
        }

        if(!flag)
            break;
    }
    close(unix_socket);

    // Удаление сокета
    unlink(SOCKET_NAME);
    
    exit(EXIT_SUCCESS);
}
