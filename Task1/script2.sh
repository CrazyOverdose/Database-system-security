#!/bin/bash
 
if [ $# -gt 0 ];then
    path=$1
    $path 1>stdout.txt 2>stderr.txt
    echo $?>exit.txt
    echo "Приложение отработало. Проверьте результат"
    exit 0
else
    echo "Не было введено запускаемое приложение"
fi
exit 1
