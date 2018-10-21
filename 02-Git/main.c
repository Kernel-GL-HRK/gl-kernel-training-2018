/*
A simple gaime Rock–paper–scissors.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROCK_KEY	'r'
#define ROCK_STR	"ROCK"

#define PAPER_KEY	'p'
#define PAPER_STR	"PAPER"

#define SCISSORS_KEY	's'
#define SCISSORS_STR	"SCISSORS"

#define EXIT_KEY	'x'

#define SUCCESS		1
#define FAIL		0

enum {
    ROCK = 0,
    PAPER,
    SCISSORS,
    EXIT,
};

enum {
    WIN = 0,
    LOSE,
    DRAW,
};


void print_controls(void);
unsigned char get_choice(unsigned char *choice);
void do_game(unsigned char *choice);
char *get_item_name(char *choice);


int main(int argc, char const *argv[])
{
    unsigned char choice;    

    printf("*** A Rock–paper–scissors game***\n");
    print_controls();
        
    do
    {
        printf("Make your choice: ");
        
        if (get_choice(&choice) == SUCCESS){
            do_game(&choice);
        }  

    } while (choice != EXIT);
    
    printf("Bye-bye!!!\n\n");
    return 0;
}

void print_controls(void)
{
    printf("The control is: %s-%c, %s-%c, %s-%c, Exit-%c \n\n", \
     ROCK_STR, ROCK_KEY, PAPER_STR, PAPER_KEY, SCISSORS_STR, SCISSORS_KEY, EXIT_KEY);

    return;
}

unsigned char get_choice(unsigned char *choice)
{
    char chr;
    unsigned char ret = SUCCESS;    
    do
    {
        scanf("%c", &chr);
    } while (chr == '\n');    
    
    switch (chr)
    {
        case ROCK_KEY:
            *choice = ROCK;
            break;
        case PAPER_KEY:
            *choice = PAPER;
            break;            
        case SCISSORS_KEY:
            *choice = SCISSORS;
            break; 
        case EXIT_KEY:
            *choice = EXIT;
            ret = FAIL;
            break;   
        default:
            printf("Wrong choice: %c\n", chr);
            ret = FAIL;
            break;
    }
    
    return ret;
}

void do_game(unsigned char *choice)
{
    int random_number;
    char *items[] = {ROCK_STR, PAPER_STR, SCISSORS_STR};
    int result;

    srand(time(NULL));
    random_number = rand() % 3;
    
    printf("Your chose is %s, My chose is %s\n", items[*choice], items[random_number]);


    if (*choice == random_number) {
        result = DRAW;
    } else  if (*choice == ROCK) {      
        (random_number == SCISSORS) ? (result = WIN) : (result = LOSE);        
    } else if (*choice == PAPER) {        
         (random_number == ROCK) ? (result = WIN) : (result = LOSE);
    } else if (*choice == SCISSORS) {
         (random_number == PAPER) ? (result = WIN) : (result = LOSE);
    }
    
    switch (result)
    {
        case DRAW:
            printf("Hmmm... DRAW\n\n");
            break;
        case WIN:
            printf("You are a winner!!!\n\n");
            break;
        case LOSE:
            printf("You are a loser!!!\n\n");
            break;            
    
        default:
            break;
    }

    return;
}
