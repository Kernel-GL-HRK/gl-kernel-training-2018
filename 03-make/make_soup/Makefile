#!/usr/bin/make -f 

# Author: Volodymyr Savchenko savchenko.volod@gmail.com

KITCHEN        = Kitchen
TABLE          = $(KITCHEN)/Table
STAVE          = $(KITCHEN)/Stave
CUPBOARD       = $(KITCHEN)/Cupboard
POTS           = $(CUPBOARD)/Pots
REFRIGERATOR   = $(CUPBOARD)/Refrigerator
SCRIPTS        = Scripts
CHK_DIR_SCRIPT = $(SCRIPTS)/chk_dir.sh
POT_SIZE_DEF   = medium
POT_SIZE       = $(POT_SIZE_DEF) # set pot size
POTS_PATTERN   = small medium big
VEGS_PATTERN   = potato carrot onion

#.ONESHELL: pots

all: soup

soup: cooking clean_table #  water meat potato pasta carrot onion salt spice
	@mv $(STAVE)/$(strip $(POT_SIZE))_pot.txt $(TABLE)/soup.txt
	$(info Soup in the $(TABLE)/$@.txt.)

clean_table:
	@if [ -d $(TABLE) ] ; then rm -rf $(TABLE)/*  ; fi

cooking: receipe.txt get_water put_pot_on_stave add_meat add_vegies add_salt_and_spices
	$(info Lets cook a soup! Receipe in $(TABLE)/$<.)


receipe.txt: kitchen
	@echo "Put the pot on the stave." >> $(TABLE)/$@
	@echo "Turn on the stave." >> $(TABLE)/$@
	@echo "Cook for 45-60 minutes." >> $(TABLE)/$@
	@echo "Put vegies. Cook for 20-25 minutes" >> $(TABLE)/$@
	@echo "Add salt and spices." >> $(TABLE)/$@
	@echo "Cook for 5 minutes." >> $(TABLE)/$@
	@echo "Turn off the stave." >> $(TABLE)/$@

add_salt_and_spices: kitchen pots get_pot
	@echo "Salt and spices." >> $(TABLE)/$(strip $(POT_SIZE))_pot.txt

put_pot_on_stave: kitchen pots get_pot
	@mv $(TABLE)/$(strip $(POT_SIZE))_pot.txt $(STAVE)
	 
add_vegies: kitchen pots get_meat get_vegies
	@echo "Potato, carrot and onion " >> $(STAVE)/$(strip $(POT_SIZE))_pot.txt

add_meat: kitchen pots get_meat 
	@echo "Meat, 1/2 kg" >> $(STAVE)/$(strip $(POT_SIZE))_pot.txt

get_water: kitchen pots
	@echo "Cold water, 3/4 of the pot volume" >> $(TABLE)/$(strip $(POT_SIZE))_pot.txt

get_vegies: kitchen
	@$(foreach  v,  $(VEGS_PATTERN), $(shell touch $(TABLE)/$(v).txt ) )

get_pot: kitchen pots 
	@mv $(POTS)/$(strip $(POT_SIZE))_pot.txt $(TABLE)

get_meat: meat.txt
	@if [ -f $(REFRIGERATOR)/meat.txt ] ; then mv $(REFRIGERATOR)/meat.txt $(TABLE) ; fi

pots: kitchen
	@if ! [ -f  $(POTS)/$(strip $(POT_SIZE))_pot.txt ] ; then touch $(POTS)/$(strip $(POT_SIZE))_pot.txt ; fi
	@$(foreach p, $(POTS_PATTERN), $(shell touch $(POTS)/$(p)_pot.txt) )

meat.txt: kitchen
	@touch $(REFRIGERATOR)/$@

kitchen: 
	@$(CHK_DIR_SCRIPT) $(KITCHEN)
	@$(CHK_DIR_SCRIPT) $(CUPBOARD)
	@$(CHK_DIR_SCRIPT) $(TABLE)
	@$(CHK_DIR_SCRIPT) $(POTS)
	@$(CHK_DIR_SCRIPT) $(STAVE)
	@$(CHK_DIR_SCRIPT) $(REFRIGERATOR)


.PHONY: clean

clean:
	rm -rf $(KITCHEN)



