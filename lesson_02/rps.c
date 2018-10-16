#include "stdio.h"
#include "stdlib.h"
#include "time.h"

int main(void)
{
    char player = 0, machine = 0;
    int b = 0, i;
    time_t t;
    //Intializes random number generator
    srand((unsigned) time(&t));
    printf("RPS game. choose r, p or s\r\n");
    while (1) {
	scanf("%c", &player);
	if (player) {
	    if (player == 'r' || player == 'p' || player == 's') {
		break;
	    } else if ( player == '\n' || player == '\r') {
		continue;
	    } else {
		printf("Wrong symbol. Try again. \r\n");
		player = 0;
	    }
	}
    }
    printf("read symbol %c\r\n", player);
    b = rand() % 3; // will return value from 0 to 2
    if (b == 0)
	machine = 'r';
    else if (b == 1)
	machine = 'p';
    else 
	machine = 's';
    printf ( "i choose %c\r\n", machine);
    if (player == machine) {
	printf ("draw \r\n");
    } else if (( player == 'r' && machine == 's' ) || ( player == 'p' 
		&&  machine == 'r') || ( player == 's' && machine == 'p')) {
	printf ("you win \r\n");
    } else {
	printf ("you lose \r\n");
    }
    return 1;
}