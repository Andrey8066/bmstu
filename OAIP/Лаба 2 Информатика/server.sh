#!/bin/bash

port=12345
nc -nvlp $port >> output.txt &
echo "Запуск сервера на порту $port"

while true; do
x=$( tail -1 output.txt )
echo x | nc -w 1 localhost $port

done