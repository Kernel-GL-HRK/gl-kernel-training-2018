#!/usr/bin/make -f

#variables for hirerarchy
KITCHEN = kitchen
KNIFE = ${KITCHEN}/knife
TABLE = ${KITCHEN}/table
POT = ${TABLE}/pot
SPOON = ${TABLE}/spoon
DINNING_ROOM = dinning-room
FOOD_MARKET = food-market

#files with goods
POTATO = potato
WATER = water
CARROT = carrot

#commands
CREATE_DIR = mkdir -p
REMOVE = rm -rf
LIST_DIR = ls -la
CAT = -cat

MOVE_SCRIPT = ./move_script.sh

#default count
DEFAULT_ITEMS_CNT=10

all: ${POT}/${POTATO} ${POT}/${WATER} ${POT}/${CARROT}
	echo Soup is done
	${REMOVE} $^

${POT}/%: ${TABLE}/% ${MOVE_SCRIPT}
	#move from table to pot
	${MOVE_SCRIPT} $< $@

${TABLE}/%: ${FOOD_MARKET}/% ${MOVE_SCRIPT}
	#move from market to table
	${MOVE_SCRIPT} $< $@

${MOVE_SCRIPT}:
	echo '#!/bin/bash' > ${MOVE_SCRIPT}
	echo '# $$1 - from' >> ${MOVE_SCRIPT}
	echo '# $$2 - to' >> ${MOVE_SCRIPT}
	echo 'declare -i count=`cat $$1`' >> ${MOVE_SCRIPT}
	echo 'declare -i new=count-1' >> ${MOVE_SCRIPT}
	echo 'echo 2 > $$2' >> ${MOVE_SCRIPT}
	echo 'if ((new == 0)); then' >> ${MOVE_SCRIPT}
	echo '	rm $$1' >> ${MOVE_SCRIPT}
	echo 'else' >> ${MOVE_SCRIPT}
	echo '	echo $$new > $$1' >> ${MOVE_SCRIPT}
	echo 'fi' >> ${MOVE_SCRIPT}
	chmod +x ${MOVE_SCRIPT}

init: clean
	${CREATE_DIR} ${KITCHEN}
	${CREATE_DIR} ${KNIFE}
	${CREATE_DIR} ${TABLE}
	${CREATE_DIR} ${POT}
	${CREATE_DIR} ${SPOON}
	${CREATE_DIR} ${DINNING_ROOM}
	${CREATE_DIR} ${FOOD_MARKET}
	echo ${DEFAULT_ITEMS_CNT} > ${FOOD_MARKET}/${POTATO}
	echo ${DEFAULT_ITEMS_CNT} > ${FOOD_MARKET}/${WATER}
	echo ${DEFAULT_ITEMS_CNT} > ${FOOD_MARKET}/${CARROT}

clean:
	${REMOVE} ${KITCHEN}
	${REMOVE} ${DINNING_ROOM}
	${REMOVE} ${FOOD_MARKET}
	${REMOVE} ${MOVE_SCRIPT}

list:
	${LIST_DIR} ${KITCHEN}
	${LIST_DIR} ${KNIFE}
	${LIST_DIR} ${TABLE}
	${LIST_DIR} ${POT}
	${LIST_DIR} ${SPOON}
	${LIST_DIR} ${DINNING_ROOM}
	${LIST_DIR} ${FOOD_MARKET}
	${CAT} ${FOOD_MARKET}/${POTATO}
	${CAT} ${FOOD_MARKET}/${WATER}
	${CAT} ${FOOD_MARKET}/${CARROT}

.PHONY: clean init list rebuild

