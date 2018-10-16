#include <stdio.h>

char read_user(){
	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	char user;
	while(user != 'r' && user != 'p' && user != 's')
		scanf("%c", &user);

	return user;
} 

int main(){
	char user = read_user();

	return 0;	
}
