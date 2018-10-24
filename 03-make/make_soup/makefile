# Author: Illia Iorin	illia.iorin@gmail.com

#struct
#/kitchen
#  /table
#    /pot
#    /spoon
#  /knife
#/dinning-room
#/food-market



cooked_soup.txt: bouillon.txt zazharka.txt
	cd kitchen/table; cat $^ > $@
	mv kitchen/table/$@   dinning-room
	mv kitchen/table/spoon dinning-room

bouillon.txt: meat.txt water.txt
	cd kitchen/table ; ./pot/cook.sh  boil $^ > $@

meat.txt: init
	echo "meat" > food-market/$@
	mv food-market/$@ kitchen/table
	./kitchen/knife/cut.sh "$(shell pwd)/kitchen/table/$@"

water.txt: init
	echo "water" > food-market/$@
	mv food-market/$@ kitchen/table

zazharka.txt: carrot.txt onion.txt
	cd kitchen/table; ./pot/cook.sh  fry  $^ > $@

carrot.txt: init
	echo "carrot" > food-market/$@
	mv food-market/$@ kitchen/table
	./kitchen/knife/cut.sh "$(shell pwd)/kitchen/table/$@"

onion.txt: init
	echo "onion" > food-market/$@
	mv food-market/$@ kitchen/table
	./kitchen/knife/cut.sh "$(shell pwd)/kitchen/table/$@"

init:
	./src/dir_tree.sh
	cp src/cut.sh kitchen/knife
	cp src/cook.sh kitchen/table/pot

clean:
	rm -r kitchen dinning-room food-market
