TXT:=cat #computes variable value once and just stores it
TARGET?=sup.txt #if the variable was not defined
all : stol.o

	cp $< $(TARGET) #copy the first dependency to target file
stol.o : pechka.o magazin.o
	$(TXT) "./stol/kasha.txt" "./stol/kastrulya.txt" $^ > $@ #> to  the target of the rule
pechka.o :
	$(TXT) "./pechka/gaz.txt" "./pechka/pechka.txt" > $@
magazin.o :
	$(TXT) "./magazin/ovoshchi.txt" "./stol/kasha.txt" > $@
clean:
	rm -rf *.o $(TARGET)
