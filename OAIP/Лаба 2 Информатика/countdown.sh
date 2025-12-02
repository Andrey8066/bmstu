#!/bin/bash/
read -p ">" n
for (( i=0; i<n; i+1))
do
echo "$((n-i))"
((i++))
done
echo "Готово!"