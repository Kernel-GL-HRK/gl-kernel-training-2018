#!/bin/bash

A=$1

echo "Рецепт приготовления $A кг плова"
./meat $(bc -q <<< "$A*0.25")
./fry 10
./fry_onion.sh $(bc -q <<< "$A*100")
./rice $(bc -q <<< "$A*150")
./boil 10
echo "- Добавьте обжаренное мясо, лук и морковь"
./boil 15
./sause
./spices
echo "- Добавьте зелень. Плов готов!"
