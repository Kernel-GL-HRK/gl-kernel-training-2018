#!/bin/bash

A=$1

echo "Рецепт приготовления $A л супа-харчо"
./water $(bc -q <<< "$A*0.8")
./meat $(bc -q <<< "$A*0.25")
./boil 120
./rice $(bc -q <<< "$A*150")
./boil 15
./fry_onion.sh $(bc -q <<< "$A*100")
./walnuts $(bc -q <<< "$A*125")
echo "- Положите лук и орехи в бульон"
./sause
./spices
./boil 5
echo "- Добавьте зелень. Суп харчо готов!"
