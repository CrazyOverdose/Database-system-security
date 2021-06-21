#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/wait.h>

char* ReadScript(int exit_script){
    
    // Массив для считываемых данных
    char* buff = new char[7000];

    // Считывание данных из файлого дескриптора
    read(exit_script, buff, 7000);

    return buff;
}

void WriteResult(int state, std::string data){
    FILE *stdout_, *stderr_;

    // Открытие двух файлов для записи
    stdout_ = fopen("stdout.txt", "w");
    stderr_ = fopen("stderr.txt", "w");
    
    if(state == 0){
        fprintf(stdout_, data.c_str());
        fprintf(stderr_, "Ошибок не было\n");
    }
    else{
        fprintf(stdout_, "Программа отработала с ошибкой. Данные не записаны\n");
        fprintf(stderr_, data.c_str());
    }
    // Закрытие файлов
    fclose(stdout_);
    fclose(stderr_);
}

int main(int args, char* argv[]){
    
    // Файловые дескрипторы для выходного потока и потока ошибок
    int fd_stdout[2];
    int fd_stderr[2];

    if(pipe(fd_stdout) < 0 || pipe(fd_stderr) < 0 ){
        printf("Не удалось создать pipe");
        exit(3);
    }

    // Данные, передаваемые в скрипт
    std::string data;

    switch (args)
    {
        case 1:
            printf("Не указано запускаемое приложение\n");
            exit(3);
            
        case 2:
            data ="";
            break;

        default:
            data = argv[2];

            // argv[1] - название скрипта, 
            // argv[2] - первый аргумент скрипта, 
            // argv[i] - остальные аргумента скрипта
            for(int i = 3; i < args; ++i){
                data += " " + std::string(argv[i]);
            } 
            break;
    }

    // Путь к скрипту
    std::string path = "./";
    path += argv[1];

    // Состояние дочернего процесса
    int state;

    // Создание дочернего процесса. Он зайдет в этот блок
    pid_t pid = fork();
    if (pid == (pid_t) 0){

        // Создание двух дескрипторов для stdout и 
        // stderr запускаемого скрипта
        dup2(fd_stderr[1], 2);
        dup2(fd_stdout[1], 1);

        // Запуск скрипта
        execl(path.c_str(), path.c_str(), data.c_str(), NULL);    
    }
    else if (pid < (pid_t) 0){
        printf("При вызове fork() возникла ошибка");
        exit(3);
    }
    else{

           // Ожидание, пока дочерний процесс закончит работу. 
           // В переменную state положится информация о состоянии.
           // 0 - ожидание любого потомка, 
           // чей идентификатор группы процессов
           // равен таковому у вызвавшего процесса
           waitpid(pid, &state, 0); 

           // WIFEXITED(state) вернет true, 
           // если потомок вызвал exit
           if(WIFEXITED(state))

                // WEXITSTATUS возвращает 8 старших битов,
                // который вернул дочерний процесс (в exit)
                printf("Exit: %d\n" , WEXITSTATUS(state));
           else
                printf("Child did not terminate with exit \n");
    }

    // Закрытие файловых дескрипторов (записи)
    close(fd_stderr[1]);
    close(fd_stdout[1]);

    // Считывание результата работы скрипта
    char* exit_stdout = ReadScript(fd_stdout[0]);
    char* exit_stderr = ReadScript(fd_stderr[0]);

    // Закрытие файловых дескрипторов (чтения)
    close(fd_stdout[0]);
    close(fd_stderr[0]);

    // Если приложение отработало без ошибок,
    // данные о погоде запишутся в файл "stdout.txt"
    if(WEXITSTATUS(state)==0){
        printf("Результат: %s", exit_stdout);

        // Запись результата приложения в файл
        WriteResult(state, exit_stdout);
    }
    // Если приложение отработало без ошибок,
    // данные о погоде запишутся в файл "stderr.txt"
    else{
        printf("Результат: %s", exit_stderr);
        
        // Запись результата приложения в файл
        WriteResult(state, exit_stderr);
    }
    exit(0);
}
