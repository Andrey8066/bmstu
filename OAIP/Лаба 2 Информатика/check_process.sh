#!/bin/bash/
x=$1
y=$(pgrep $1)
echo $y
if [[ -n $y ]];
then
    echo "Процесс запущен"
else
    echo "Процесс не найден"
fi