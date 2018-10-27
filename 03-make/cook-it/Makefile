
AMOUNT?=1

all: soup pilaf tea

soup: soup.sh water meat rice fried_onion walnuts sause spices
	@./soup.sh $(AMOUNT) > $@

fried_onion: oil onion
	@./fry_onion.sh $(AMOUNT) >$@

pilaf: pilaf.sh water rice meat fried_onion spices
	@./pilaf.sh $(AMOUNT) >$@

tea: water boil
	@echo "Рецепт приготовления $(AMOUNT) л чая" > $@
	@./water $(AMOUNT) >>$@
	@echo "- Взять 50 гр чайного листа" >> $@
	@./boil 1 >> $@
	@echo "- Добавить сахар по вкусу. Чай готов!" >> $@
