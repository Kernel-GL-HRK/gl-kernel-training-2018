#!/usr/bin/make

kitchen=./kitchen
food-market=./food-market
dinning-room=./dinning-room
table=${kitchen}/table
knife=${kitchen}/knife
pot=${table}/pot
spoon=${table}/spoon

serve_soup.o: make_soup.o
	echo
	echo "Serve some soup"
	cp ${pot}/soup.txt ${dinning-room}/plate_of_soup.txt
	echo "Soup served"

make_soup.o: add_ingridients.o  boil_water.o
	echo
	echo "Make soup"
	cat ${pot}/boiled_water.txt > ${pot}/soup.txt
	rm ${pot}/boiled_water.txt
	cat ${pot}/cut_carrot.txt >> ${pot}/soup.txt
	rm ${pot}/cut_carrot.txt
	cat ${pot}/cut_potato.txt >> ${pot}/soup.txt
	rm ${pot}/cut_potato.txt
	cat ${pot}/salt.txt >> ${pot}/soup.txt
	rm ${pot}/salt.txt
	echo "Soup done"

boil_water.o:
	echo
	echo "boil water"
	mv ${pot}/water.txt ${pot}/boiled_water.txt
	echo "some boiled water">${pot}/boiled_water.txt
	echo "water boiled"

ingridients.o:  get_ingridients.o prepare_ingridients.o
	echo
	echo "ingridients for soup are ready"

get_ingridients.o:
	echo
	echo "get ingridients"
	cp -a ${food-market}/. ${table}/
	echo "ingridients received"

prepare_ingridients.o: cut_ingridients.o prepare_salt.o
	echo
	echo "Ingridients prepeared"

cut_ingridients.o:
	echo
	echo "Cut ingridietns"
	mv ${table}/carrot.txt ${knife}/cut_carrot.txt
	echo "some cut carrot">${knife}/cut_carrot.txt
	mv ${table}/potato.txt ${knife}/cut_potato.txt
	echo "some cut potato">${knife}/cut_potato.txt
	echo "Ingridients are cut"

prepare_salt.o:
	echo
	echo "Prepare salt"
	mv ${table}/salt.txt ${spoon}/salt.txt
	echo "Salt prepared"

add_ingridients.o: ingridients.o
	echo
	echo "Add ingridients"
	mv ${table}/water.txt ${pot}/water.txt
	mv ${knife}/cut_carrot.txt ${pot}/cut_carrot.txt
	mv ${knife}/cut_potato.txt ${pot}/cut_potato.txt
	mv ${spoon}/salt.txt ${pot}/salt.txt
	echo "Ingridients added"
