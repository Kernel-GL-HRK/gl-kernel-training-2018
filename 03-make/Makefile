#!/usr/bin/make -f 
#
# Homework 03-make Makfile
#

KITCHEN=./kitchen
TABLE=./kitchen/table
KNIFE=./kitchen/knife
POT=./kitchen/table/pot
SPOON=./kitchen/table/spoon
D_ROOM=./dinning-room
MARKET=./food-market
SCRIPT= ./dir.sh

.PHONY: clear install all

all: install dinning-room.txt 
	echo "--- DONE ---"
	

${SCRIPT}:
	echo '#!/bin/bash' > ${SCRIPT}
	echo "mkdir -p ${KITCHEN} ${TABLE} ${KNIFE} ${POT} ${SPOON} ${D_ROOM} ${MARKET}" >> ${SCRIPT}
	chmod +x ${SCRIPT}

dinning-room.txt: spoon.txt soup.txt
	mv ${POT}/potato.txt ${D_ROOM}
	mv ${POT}/carrot.txt ${D_ROOM}
	mv ${POT}/meat.txt ${D_ROOM}
	mv ${POT}/water.txt ${D_ROOM}
	mv ${SPOON}/spoon.txt ${D_ROOM}
	echo "The soup is ready !!!" >$@
	mv $@ ${D_ROOM}
	

spoon.txt: 
	echo "spoon" > $@
	mv $@ ${SPOON}

soup.txt: knife.txt pot.txt
	mv  ${KNIFE}/potato.txt ${POT}
	mv  ${KNIFE}/carrot.txt ${POT}
	echo "make soup: broth + potato + carrot --> pot" > $@
	mv $@ ${POT}

pot.txt: knife.txt kitchen_table.txt
	mv  ${TABLE}/water.txt ${POT}
	mv  ${KNIFE}/meat.txt ${POT}
	echo "make broth: water + meat  --> pot" > $@
	mv $@ ${POT}
	
knife.txt: kitchen_table.txt
	mv ${TABLE}/potato.txt ${KNIFE}
	mv ${TABLE}/carrot.txt ${KNIFE}
	mv ${TABLE}/meat.txt ${KNIFE}
	echo "Сutting potatoes, carrots and meat" > $@
	mv $@ ${KNIFE}
	
kitchen_table.txt: products.txt
	mv ${MARKET}/potato.txt ${TABLE}
	mv ${MARKET}/carrot.txt ${TABLE}
	mv ${MARKET}/meat.txt ${TABLE}
	mv ${MARKET}/water.txt ${TABLE}	
	echo "Products from the food-market laid out on the table" > $@
	mv $@ ${TABLE}

products.txt: 
	echo "potato" > ${MARKET}/potato.txt 
	echo "carrot" > ${MARKET}/carrot.txt
	echo "meat" > ${MARKET}/meat.txt
	echo "water" > ${MARKET}/water.txt
	echo "products in the food-market: potato carrot meat water\n" > $@
	mv $@ ${MARKET}

clean:
	rm -rf ${KITCHEN}
	rm -rf ${D_ROOM}
	rm -rf ${MARKET}
	rm ${SCRIPT}


install: ${SCRIPT}
	 ${SCRIPT}
