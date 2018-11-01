#!/usr/bin/make

.PHONY: pot stove dish

ENV=env
FOOD_MARKET=food_market
FOOD :=
SPICE :=

make_soup: prepare_enviroment cook_soup serve

prepare_enviroment: create_food_market create_kitchen create_dining_room

create_food_market: create_market create_food

create_market: 
	echo "Create market"
	mkdir -p ${ENV}/${FOOD_MARKET}

create_food: create_market potato.food carrot.food onion.food salt.spice pepper.spice
	echo "Create food"
	$(eval FOOD := $(shell ls ${ENV}/${FOOD_MARKET}/*.food))
	$(eval SPICE := $(shell ls ${ENV}/${FOOD_MARKET}/*.spice))

%.food:
	echo "Create food at market"
	echo "$@,1kg" | sed 's/.food//g' > ${ENV}/${FOOD_MARKET}/$@

%.spice:
	echo "Create spice at market"
	echo "$@,100mg" | sed 's/.spice//g' > ${ENV}/${FOOD_MARKET}/$@

create_kitchen: create_table create_washer
	echo "Create kitchen"

create_table: pot stove dish
	echo "Create table on kitchen"
	for item in ${^}; do \
		mkdir -p ${ENV}/kitchen/table/$$item ; \
	done

create_washer: 
	echo "Create washer on kitchen"
	mkdir -p ${ENV}/kitchen/washer

create_dining_room: 
	echo "Create dining room with table"
	mkdir -p ${ENV}/dining_room/table/

cook_soup: boil 
	echo "Cook soup"

get_ingrediends: 
	echo "Get ingredients from market" 
	mv ${ENV}/food_market/* ${ENV}/kitchen/table/
	
wash_vegetables: get_ingrediends
	echo "Wash vegetables"
	for item in $$(ls ${ENV}/kitchen/table/*.food); do \
		mv $$item ${ENV}/kitchen/washer; \
	done
	for item in $$(ls ${ENV}/kitchen/washer/*.food); do \
		echo "washed" >> $$item; \
	done

slice_vegetables: wash_vegetables
	echo "Slice vegetables"
	for item in $$(ls ${ENV}/kitchen/washer/*.food); do \
		mv $$item ${ENV}/kitchen/table; \
	done
	for item in $$(ls ${ENV}/kitchen/table/*.food); do \
		echo "sliced" >> $$item; \
	done

add_water: slice_vegetables
	echo "Add water to pot"
	echo "water,3l" > ${ENV}/kitchen/table/pot/water

add_vegetables: add_water
	echo "Add vegetables to pot"
	for item in $$(ls ${ENV}/kitchen/table/*.food); do \
		mv $$item ${ENV}/kitchen/table/pot; \
	done

add_spice: add_vegetables
	echo "Add spices to pot"
	for item in $$(ls ${ENV}/kitchen/table/*.spice); do \
		mv $$item ${ENV}/kitchen/table/pot; \
	done

boil: add_spice
	echo "Boil ingredients"
	mv ${ENV}/kitchen/table/pot ${ENV}/kitchen/table/stove
	for item in $$(ls ${ENV}/kitchen/table/stove/pot/*); do \
		echo "boiled" >> $$item; \
	done

serve: cook_soup
	echo "Get portion"
	cp ${ENV}/kitchen/table/stove/pot/* ${ENV}/kitchen/table/dish
	for item in $$(ls ${ENV}/kitchen/table/stove/pot/*); do \
		echo "1/8 less" >> $$item; \
	done
	for item in $$(ls ${ENV}/kitchen/table/dish/*); do \
		echo "1/8" >> $$item; \
	done
	mv ${ENV}/kitchen/table/dish ${ENV}/dining_room/table

clean:
	rm -rf ${ENV}/ 