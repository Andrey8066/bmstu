#!/bin/bash

port=12345

echo "Запуск сервера на порту $port"
nc -nvlp $port > output &

while true; do
{ echo "Сообщение получено $?"; cat; } 
done