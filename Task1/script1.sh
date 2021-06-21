#!/bin/bash
 
if [ $# -eq 0 ];then
    echo "Не были введены аргументы" >&2
    exit 1
elif [ $1 != "Погода" ] || [ $# -gt 1 ];then
    echo "Введена нераспознанная команда" >&2
    exit 2
else
    curl wttr.in/Moscow?0 >&1
fi
exit 0
