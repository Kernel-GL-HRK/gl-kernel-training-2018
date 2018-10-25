#!/bin/bash

A=$1

echo "- Возьмите чистую сковородку"
./oil $(bc -q <<< "$A*0.5")
./onion $(bc -q <<< "$A")
./fry 15
