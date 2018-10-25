
VEGETABLES=kitchen/fridge/carrot\
 kitchen/fridge/potato\
 kitchen/fridge/onion

MEAT=kitchen/fridge/meat

KNIFE=kitchen/table/knife
GAS_COOKER=kitchen/gas_cooker
POT=kitchen/table/pot
POT_SOUP=dining-room/pot

CHOP=./chop.sh
BUY=./move_resource.sh

cook_soup: $(POT) add_water chop_vegetables chop_meat $(GAS_COOKER)
	@echo "Now it's tasty soup" >> $(POT) &&\
	mv $(POT) $(POT_SOUP)

#implicit rules for shopping 
kitchen/fridge/% : food-market/%
	@touch $@ && \
	$(BUY) $< $@ 10


chop_vegetables: $(VEGETABLES) $(KNIFE)
	@for VEGETABLE in $(VEGETABLES); do \
    		$(CHOP) $$VEGETABLE $(POT) 2 ;\
	done


chop_meat: $(MEAT) $(KNIFE)
	@$(CHOP) $(MEAT) $(POT) 2



add_water: $(POT)
	@echo "water" >>$(POT)
	
#throw away garbage from fridge and wash the pot
clean:
	@mv $(POT_SOUP) $(POT) && echo "" > $(POT) &&\
	-rm kitchen/fridge/*
