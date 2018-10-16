#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char read_user(){
	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	char user;
	while(user != 'r' && user != 'p' && user != 's')
		scanf("%c", &user);

	return user;
} 

char rnd(){
	char arr[]={'r','p','s'};
	
	srand(time(NULL));
	int rnd =  rand()%3;
	
	return arr[rnd];
}

int main(){
	char user = read_user();
	char comp = rnd();

	return 0;	
}
