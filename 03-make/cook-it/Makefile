
AMOUNT?=1

soup: soup.sh water meat rice fried_onion walnuts sause spices
	@./soup.sh $(AMOUNT) > $@

fried_onion: oil onion
	@./fry_onion.sh $(AMOUNT) >$@

pilaf: pilaf.sh water rice meat fried_onion spices
	@./pilaf.sh $(AMOUNT) >$@

