#!/bin/bash/
touch notes.txt
read -p ">" p
while [[ "$p" != "exit" ]]; do
    echo "$p" >> notes.txt
    read -p ">" p
done
echo "СОдержимое файла"
cat ./notes.txt
rm ./notes.txt
