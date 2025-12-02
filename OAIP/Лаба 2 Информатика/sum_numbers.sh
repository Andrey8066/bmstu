#!/bin/bash/
x=$1
y=$2
if (x==0 || y == 0);
then
    echo $((x+y))
else
    echo "Передайте два числа"
fi