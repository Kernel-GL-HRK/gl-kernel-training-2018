#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char random[] = {'s','r','p','p','s','r','r','p','s'};
char rr[] = "rock";
char pp[] = "paper";
char ss[] = "scissors";

static char human = ' ';
static char robot = ' ';
static int id = 0;
static char hu[10] = "wrong char";
static char ro[10] = "";

int main()
{
    srand(time(NULL));

    printf("Please choose: rock (r) - paper (p) - scissors (s) or other to quit. \n");
    scanf("%c", &human);
    id = rand() % 10;

    robot = random[id];

    if (human == 'r') memcpy(hu, rr, sizeof(hu));
    if (human == 'p') memcpy(hu, pp, sizeof(hu));
    if (human == 's') memcpy(hu, ss, sizeof(hu));

    if (robot == 'r') memcpy(ro, rr, sizeof(ro));
    if (robot == 'p') memcpy(ro, pp, sizeof(ro));
    if (robot == 's') memcpy(ro, ss, sizeof(ro));

    printf("You choose %s, I choose %s \n", hu, ro);

    if (human == robot) {
        printf("Dead Heat \n");
    } else {
        switch(human)
        {
            case 'r':
            {
                if (robot == 'p') {
                    printf("I win: paper beats rock \n");
                } else {
                    printf("You win: rock beats scissors \n");
                }
                break;
            }
            case 'p':
            {
                if (robot == 's') {
                    printf("I win: scissors beats paper \n");
                } else {
                    printf("You win: paper beats rock \n");
                }
                break;
            }
            case 's':
            {
                if (robot == 'r') {
                    printf("I win: rock beats scissors \n");
                } else {
                    printf("You win: scissors beats paper \n");
                }
                break;
            }
            default: {
                printf("You lost \n");
            }
        }
    }
    return 0;
}
