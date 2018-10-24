#!/usr/bin/make -f

.PHONY: clean

soup: boil_water.o potatoes.o porridge.o
	echo "Mix all ingredients"
	rm ./kitchen/table/pot/*
	cat $^ > ./kitchen/table/pot/soup
	echo "Done! Bon appetit"


boil_water.o: pot.o plate.o water.o
	echo "Boil water"
	echo "$$(cat plate.o) $$(cat water.o)" > $@

pot.o:
	echo "Preparing a pot"
	mkdir -p ./kitchen/table/pot

plate.o:
	echo "Hot" > $@

water.o:
	if [ ! -e ./kitchen/fridge/water ]; then		\
	echo "Buy water";								\
	cp ./food-market/water ./kitchen/table/pot;		\
	else											\
	echo "Get water from a fridge";					\
	cp ./kitchen/fridge/water ./kitchen/table/pot;	\
	fi
	echo "Water" > $@
	echo "Add water"

potatoes.o:
	if [ ! -e ./kitchen/fridge/potato ]; then			\
		echo "Buy potatoes";							\
		cp ./food-market/potato ./kitchen/table/pot;	\
	else												\
		echo "Get potatoes from a fridge";				\
		cp ./kitchen/fridge/potato ./kitchen/table/pot;	\
	fi
	echo "Potatoes" > $@
	echo "Add potatoes"

porridge.o:
	if [ ! -e ./kitchen/fridge/porridge ]; then			\
		echo "Buy porridge";							\
		cp ./food-market/porridge ./kitchen/table/pot;	\
	else												\
		echo "Get porridge from a fridge";				\
		cp ./kitchen/fridge/porridge ./kitchen/table/pot;\
	fi
	echo "Porridge" > $@
	echo "Add porridge"

clean:
	rm -f ./kitchen/table/pot/*
	rm -f *.o
	